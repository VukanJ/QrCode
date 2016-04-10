// Helper functions
#ifndef UTIL_H
#define UTIL_H

//#define DEB
#ifdef DEB
#define DEBUG(code) code
#else
#define DEBUG(code)
#endif

struct varNum
{
	varNum() : dat(0), len(0){};
	varNum(unsigned short data, short lenght)
		: dat(data), len(lenght){};
	unsigned short dat;
	short len;
};

struct qrInfo
{
	byte mask;
	int version, size;
	Err_Level error_level;
	Encoding encoding;
};

namespace{
	byte c2alpha(const char t)
	{
		switch (t){
		case ' ':return 36;
		case '$':return 37;
		case '%':return 38;
		case '*':return 39;
		case '+':return 40;
		case '-':return 41;
		case '.':return 42;
		case '/':return 43;
		case ':':return 44;
		}
		if (isdigit(t)) return t - 48;
		if (isupper(t))
			if (isalpha(t)) return t - 55;
		return 36;
	}

	void printLongBit(std::vector<bool>& vec)
	{
		int i = 0;
		for (auto b : vec){
			std::cout << (b ? 1 : 0);
			if (++i % 8 == 0)std::cout << ' ';
		}
		std::cout << std::endl;
	}

	void pushBits(unsigned int data, std::vector<bool>& vec, int amount)
	{
		for (unsigned int p = (unsigned int)0x1 << (amount - 1); p; p >>= 1)
			vec.push_back(p & data ? true : false);
	}

	template<typename C> bool isAlphaNum(C t)
	{
		if (t > 255)return false;
		switch (t){
		case ' ':case '$':case '%':case '*':
		case '+':case '-':case '.':case '/':
		case ':':return true;
		default:
			if (isdigit(t)) return true;
			if (isupper(t) && isalpha(t))
				return true;
			else return false;
		}
	}

	template<typename T> void printBit(T data)
	{
		for (T p = (T)0x1 << (sizeof(data) * 8 - 1); p; p >>= 1)
			std::cout << (p&data ? 1 : 0);
		std::cout << std::endl;
	}

	std::ostream& operator<<(std::ostream& stream, varNum& num)
	{
		for (unsigned short p = (unsigned short)0x1 << (num.len - 1);
			p; p >>= 1)
			std::cout << (p&num.dat ? 1 : 0);
		return stream;
	}

} // end anonymous namespace

#endif