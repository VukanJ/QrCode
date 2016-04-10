#ifndef PRINTER_H
#define PRINTER_H

#include <SFML/Graphics.hpp>

#include "Data.h"
#include "util.h"
#include "Module.h"

typedef std::vector<std::vector<QR_Module>> QR_Field;

class Printer
{
public:
	Printer() = delete;
	Printer(qrInfo*);
	void printAllPatterns(QR_Field&);
	void printFinal(sf::Image&,QR_Field&);
	void printDebugFinal(sf::Image&, QR_Field&); // color coded!
private:
	qrInfo* info;
};

#endif