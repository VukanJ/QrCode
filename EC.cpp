#include "EC.h"

EC::EC(qrInfo* inf) : info(inf){}

void EC::ErrorCorrection(std::vector<bool>& dataFinal)
{
	std::cout << "Generating EC Codewords ...\n";
	// Turn bits into bytes
	byteVec fullMsg;
	for (int j = 0; j < dataFinal.size() / 8; j++){
		fullMsg.push_back(0x0);
		for (int i = 0; i < 8; i++)
			fullMsg.back() |= ((byte)1 & dataFinal[j * 8 + i]) << (7 - i);
	}
	dataFinal.clear();
	/*std::cout << "DATA\n";
	for (auto& d : fullMsg)
		std::cout << (int)d << ' ';*/
	// Get EC Info
	int blockCount_1 = errorBlockInfo[info->version * 4 + info->error_level][2];
	int blockCount_2 = errorBlockInfo[info->version * 4 + info->error_level][4];
	int errorCount   = errorBlockInfo[info->version * 4 + info->error_level][1];
	int dataLen_1    = errorBlockInfo[info->version * 4 + info->error_level][3];
	int dataLen_2    = errorBlockInfo[info->version * 4 + info->error_level][5];
	// Generate information blocks and groups
	std::vector<byteVec> dataPackage_1, errorPackage_1;
	std::vector<byteVec> dataPackage_2, errorPackage_2;

	for (int i = 0; i < blockCount_1; i++){
		// Fill data
		dataPackage_1.push_back(
			byteVec(fullMsg.begin() + i*dataLen_1,
			fullMsg.begin() + (i + 1)*dataLen_1));
		// Generate error code 
		errorPackage_1.push_back(createErrorCode(dataPackage_1.back(), errorCount));
	}
	for (int i = 0; i < blockCount_2; i++){
		// Fill data
		dataPackage_2.push_back(
			byteVec(fullMsg.begin() + i*dataLen_2+blockCount_1*dataLen_1,
			fullMsg.begin() + (i + 1)*dataLen_2 + blockCount_1*dataLen_1));
		// Generate error code 
		errorPackage_2.push_back(createErrorCode(dataPackage_2.back(), errorCount));
	}
	// Generate bits + interleave blocks
	// TEST OUT (DEBUG)
	DEBUG(
	std::cout << "\tEC INFO\n";
	std::cout << "Blocks in first group " << blockCount_1 << '\n';
	std::cout << "Blocks in second group " << blockCount_2 << '\n';
	std::cout << "Needed EC Words " << errorCount << '\n';
	std::cout << "DataLength in first group " << dataLen_1 << '\n';
	std::cout << "DataLength in second group " << dataLen_2 << '\n';)
	//

	byteVec interlData, interlError;
	// Structure Data
	for (int i = 0; i < std::max(dataLen_1, dataLen_2); i++){
		for (auto& block : dataPackage_1){
			if (i<block.size())
				interlData.push_back(block[i]);
		}
		for (auto& block : dataPackage_2){
			if (i<block.size())
				interlData.push_back(block[i]);
		}
	}
	// Interleave EC Word 
	for (int i = 0; i < errorCount; i++){
		for (auto& block : errorPackage_1)
			interlError.push_back(block[i]);
		for (auto& block : errorPackage_2)
			interlError.push_back(block[i]);
	}
	DEBUG(
	std::cout << "InterlData: \n";
	for (auto& d : interlData)
		std::cout << (int)d << ' ';
	std::cout << "\nEC: \n";
	for (auto& d : interlError)
		std::cout << (int)d << ' ';
	std::cout << '\n';
	)
	// Add to final Data
	for (auto& data : interlData)
		pushBits(data, dataFinal, 8);
	for (auto& error : interlError)
		pushBits(error, dataFinal, 8);
	// Adding remainder 0 bits
	int v = info->version + 1;
	if (v>1 && v<7)
		pushBits(0x0, dataFinal, 7);
	else if ((v>13 && v < 21) || (v>27 && v<35))
		pushBits(0x0, dataFinal, 3);
	else if (v>20 && v<28)
		pushBits(0x0, dataFinal, 4);
	//std::cout << "RAW BITS\n";
	//printLongBit(dataFinal);
	//std::cout << "BITCOUNT " << dataFinal.size() << '\n';
}

byteVec EC::createErrorCode(byteVec& message, int ecCount)
{
	gf256poly gen, res;
	byteVec ret;
	gen.createGenerator(ecCount);
	gf256poly msg = gf256poly(message, true);
	// Modify x-exponents
	for (auto& pol : msg.poly)
		pol.xex = (pol.xex + ecCount) % 255;
	if (gen.poly[0].xex != msg.poly[0].xex)
		for (auto& pol : gen.poly)
			pol.xex = (pol.xex + (msg.poly[0].xex - pol.xex)) % 255;
	// Generate Error Code by GF(256) polynomial long division
	res = msg / gen;
	//
	for (auto& p : res.poly)
		ret.push_back(p.alpha.val);
	return ret;
}