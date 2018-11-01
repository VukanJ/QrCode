#ifndef ENCODER_H
#define ENCODER_H

#ifdef WIN_32
#define
	#include <codecvt>
#endif
#include <iostream>
#include <fstream>
#include <streambuf>

#include "Data.h"
#include "util.h"

// Reads File, gets encoding, does encoding

class Encoder
{
public:
	Encoder() = delete;
	Encoder(qrInfo* inf);
	std::wstring readFile(const std::string&);
	void getEncoding(const std::string&);
	void Encode(std::vector<bool>& dataFinal);
	std::wstring read;
private:
	qrInfo* info;
	std::vector<unsigned int> rawData;
	std::vector<varNum> encoded;
	varNum count_indicator;
};


#endif
