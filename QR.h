#ifndef QR_H
#define QR_H 

#include <SFML/Graphics.hpp>
#include <functional>
#include <iostream>

#include "Encoder.h"
#include "EC.h"
#include "Module.h"
#include "MaskInfo.h"
#include "Printer.h"

class QR final
{
public:
	QR();
	sf::Image create(const char* dataFile, Err_Level);
	int getVersion();
	void reset(); // Resets all data
	void test(sf::RenderWindow&); // Create all 160 different versions
private:
	Encoder encoder;
	EC errorCoder;
	MaskInfo maskinfo;
	Printer printer;
	
	std::vector<bool> dataFinal; // full Data-String
	std::vector<std::vector<QR_Module>> qr_field;
	qrInfo info; // Version, Encoding, Size, EC-Level, Mask_number
	sf::Image qrCode;
};

#endif