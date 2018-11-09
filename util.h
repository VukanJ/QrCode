// Helper functions
#ifndef UTIL_H
#define UTIL_H

//#define DEBUG_MODE

#ifdef DEBUG_MODE
	#define DEBUG(code) code
#else
	#define DEBUG(code)
#endif

#include <iostream>
#include <boost/program_options.hpp>

#include "Data.h"

namespace po = boost::program_options;

struct init_parameters
{
  std::string inputfile;
  int eclevel;

  void parse(int argc, char* argv[]);
};

struct varNum
{
	varNum() : dat(0), len(0){};
	varNum(unsigned short data, short length)
		: dat(data), len(length){};
	unsigned short dat;
	short len;
};

struct qrInfo
{
	Byte mask;
	int version, size;
	Err_Level error_level;
	Encoding encoding;
};

Byte c2alpha(const char t);

void printLongBit(std::vector<bool>& vec);

void pushBits(unsigned data, std::vector<bool>& vec, int amount);

std::ostream& operator<<(std::ostream& stream, const varNum& num);

template<typename T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vec);

template<typename C> bool isAlphaNum(const C t)
{
	if (t > 255) return false;
	switch (t){
		case ' ': case '$': case '%': case '*':
		case '+': case '-': case '.': case '/':
		case ':': return true;
	default:
		if (isdigit(t)) return true;
		return isupper(t) && isalpha(t);
	}
}

template<typename T> void printBit(const T data)
{
	for (T p = (T)0x1 << (sizeof(data) * 8 - 1); p; p >>= 1){
		std::cout << (p & data ? 1 : 0);
	}
	std::cout << '\n';
}

#endif // UTIL_H