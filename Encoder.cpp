#include "Encoder.h"

Encoder::Encoder(qrInfo* inf) :info(inf){}

void Encoder::getEncoding(const std::string& msg, bool filegiven)
{	
	if (filegiven) {
		std::ifstream file(msg, std::ios::in);
		read = std::string((std::istreambuf_iterator<char>(file)),
						    std::istreambuf_iterator<char>());
		file.close();
	} 
	else {
		read = msg;
	}

	std::cout << "Encoding the following information:\n\n";
	std::cout << read << "\n\n";

	if(read[0] > 60e3) read.erase(read.begin());
	// Set encoding type
	info->encoding = NUMERIC;
	for (auto& c : read){
		//if ((c >= 0x8140 && c <= 0x9FFC) || (c >= 0xE040 && c <= 0xEBBF)){ // Does not work this way
		//	info->encoding = KANJI;
		//	break;
		//}
		if (isalpha(c)){
			if (islower(c)){
				info->encoding = BYTE_ENC;
				break;
			}
			else {
				info->encoding = ALPHANUM;
			}
		}
		else if (!isdigit(c)){
			if (isAlphaNum(c)){
				info->encoding = ALPHANUM;
			}
			else {
				info->encoding = BYTE_ENC;
				break;
			}
		}
	}
}

void Encoder::Encode(std::vector<bool>& dataFinal)
{
	rawData = std::vector<unsigned int>(std::begin(read), std::end(read)); 
	switch(info->encoding){
		case BYTE_ENC:
			std::cout << "Mode: Byte encoding\n";
			break;
		case ALPHANUM:
			std::cout << "Mode: Alphanumeric encoding\n";
			for_each(std::begin(rawData), std::end(rawData), [](unsigned& c){ c = c2alpha(c); });
			break;
		case NUMERIC:
			std::cout << "Mode: Numeric encoding\n";
			for_each(std::begin(rawData), std::end(rawData), [](unsigned& c){ c -= '0'; });
			break;
		case KANJI:
			std::cout << "Mode: Kanji encoding\n";
			break;
	}
	read.clear();
	// Choose smallest QR Version
	unsigned short dataSize = rawData.size();
	std::cout << "Data length: " << dataSize << '\n';
	
	int i = 0;
	while (i < 40 && versionSize[4 * i + info->error_level][info->encoding] < dataSize) i++;
	if (i >= 40){
		std::cerr << "Input text too long. Try a shorter text or lower error level\n";
		std::cerr << "Maximum length for encoding version 40-L ";
		switch (info->encoding){
			case BYTE_ENC:std::cout << "Byte Mode: 2953";    break;
			case ALPHANUM:std::cout << "Alphanumeric: 4296"; break;
			case NUMERIC:std::cout  << "Numeric: 7089";      break;
			case KANJI:std::cout    << "Kanji: 1817";        break;
		}
		std::cout << " characters\nAborting ...\n";
		exit(1);
	}
	info->version = i;
	info->size = 21 + 4 * info->version;
	std::cout << "Using Version " << info->version + 1;
	std::printf(" [%d x %d] ...\n", info->size, info->size);

	// Assembling indicator bits
	count_indicator.dat = dataSize; // Save input length bits
	if (info->version + 1 <= 9){
		count_indicator.len = indicator_len[0][info->encoding];
	}
	else if (info->version + 1 <= 26){
		count_indicator.len = indicator_len[1][info->encoding];
	}
	else{
		count_indicator.len = indicator_len[2][info->encoding];
	}
	std::cout << "Count Indicator -> " << count_indicator << '\n';

	i = 0;
	if (info->encoding == NUMERIC){
		for (auto it = rawData.begin(); it != rawData.end();){
			encoded.push_back(varNum(0, 10));
			if (rawData.size() >= 3){
				encoded[i].dat += (*it)*100; it = rawData.erase(it);
				encoded[i].dat += (*it)*10;  it = rawData.erase(it);
				encoded[i].dat += (*it);     it = rawData.erase(it);
				i++;
			}
			else if (rawData.size() == 2){
				encoded[i].len = 7; // See Iso Standard
				encoded[i].dat += (*it) * 10; it = rawData.erase(it);
				encoded[i].dat += (*it);      it = rawData.erase(it);
				break;
			}
			else if (rawData.size() == 1){
				encoded[i].len = 4; // See Iso Standard
				encoded[i].dat += (*it); it = rawData.erase(it);
				break;
			}
		}
		/*for(auto& enc : encoded)
			std::cout << enc << '\n';
		std::cout << '\n';*/
	}
	else if (info->encoding == ALPHANUM){
		// Pairs of two characters are encoded as one byte
		for (auto it = rawData.begin(); it != rawData.end();){
			encoded.push_back(varNum(0, 11));
			if (rawData.size() >= 2){
				encoded[i].dat = 45 * (*it) + *(it + 1);
				it = rawData.erase(it);
				it = rawData.erase(it);
				i++;
			}
			else if (rawData.size() == 1){
				encoded[i].dat = *it;
				encoded[i].len = 6;
				it = rawData.erase(it);
			}
		}
		//std::cout << encoded << '\n';
	}
	else if (info->encoding == BYTE_ENC){
		for (auto it = rawData.begin(); it != rawData.end();it++)
			encoded.push_back(varNum(*it, 8));
	}
	else if (info->encoding == KANJI){
		for (auto it = rawData.begin(); it != rawData.end(); it++){
			if (*it >= 0x8140 && *it <= 0x9FFC){
				*it -= 0x8140;
				encoded.push_back(varNum(((*it >> 8) * 0xC0)*(static_cast<Byte>(0xFFFF) & *it), 13));
			}
			else if (*it >= 0xE040 && *it <= 0xEBBF){
				*it -= 0xC140;
				encoded.push_back(varNum(((*it >> 8) * 0xC0)*(static_cast<Byte>(0xFFFF) & *it), 13));
			}
		}
	}
	// Data string assembly
	switch (info->encoding){
		case BYTE_ENC: pushBits(0x4, dataFinal, 4); break;
		case ALPHANUM: pushBits(0x2, dataFinal, 4); break;
		case NUMERIC:  pushBits(0x1, dataFinal, 4); break;
		case KANJI:    pushBits(0x8, dataFinal, 4); break;
	}
	pushBits(count_indicator.dat, dataFinal, count_indicator.len);
	for (auto& enc : encoded)
		pushBits(enc.dat, dataFinal, enc.len);
	int lengthRequired = errorBlockInfo[info->version * 4 + info->error_level][0] * 8;
	if (lengthRequired - dataFinal.size() >= 4){
		pushBits(0, dataFinal, 4);
		// Is # of Bits multiple of 8 ? => add 0's
		while (dataFinal.size() % 8 != 0)
			pushBits(0, dataFinal, 1);
		i = 0;
		while (dataFinal.size() < lengthRequired)
			pushBits((i++ % 2 == 0 ? 236 : 17), dataFinal, 8);
	}
	else
		pushBits(0, dataFinal, lengthRequired - dataFinal.size());
	rawData.clear();
	// encoded still needed for message polynomial
	DEBUG(std::cout << "Required length: " << lengthRequired << '\n';
	std::cout << "Curret size: " << dataFinal.size() << '\n';)
	//printLongBit(dataFinal);
}
