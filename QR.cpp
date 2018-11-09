#include "QR.h"

QR::QR() 
	: encoder(&info), 
      errorCoder(&info),
	  maskinfo(&info),
	  printer(&info)
{
	info.error_level = _UNDEF_;
	info.version = -1;
}

void QR::reset()
{
	info = qrInfo{};
	encoder = Encoder(&info);
	errorCoder = EC(&info);
	maskinfo = MaskInfo(&info);
	printer = Printer(&info);
	qr_field.clear();
	qrCode = sf::Image{};
	dataFinal.clear();
}

sf::Image QR::create(const std::string& dataFile, Err_Level level)
{
	std::cout << "\tGenerating QR-Code ...\n";
	info.error_level = level;
	std::cout << "Using error correction level ";
	switch (info.error_level){
		case _L_: std::cout << "L (7"; break;
		case _M_: std::cout << "M (15"; break;
		case _Q_: std::cout << "Q (25"; break;
		case _H_: std::cout << "H (30"; break;
		default: std::cout << "Error: Unknown error level\n"; exit(1);
	}
	std::cout << "% recovery) ...\n";

	encoder.getEncoding(dataFile);

	encoder.Encode(dataFinal);
	
	errorCoder.ErrorCorrection(dataFinal);
	printer.printAllPatterns(qr_field);
	maskinfo.fillData(dataFinal,qr_field);
	maskinfo.maskEvaluate(qr_field);
	//printer.printDebugFinal(qrCode,qr_field);
	printer.printFinal(qrCode,qr_field);
	return qrCode;
}

void QR::test(sf::RenderWindow& window)
{
 // WIP
}

void QR::print() const 
{
	auto block = "██";
	std::cout << '\n';
	for(const auto& row : qr_field){
		std::cout << block;
		for(const auto& m : row){
			std::cout << (m.state() ? "  " : block);
		}
		std::cout << block << '\n';
	}
}

int QR::getVersion()
{
	return info.version;
}