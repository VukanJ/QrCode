// Helper functions
#ifndef UTIL_H
#define UTIL_H

//#define DEB
#ifdef DEB
#define DEBUG(code) code
#else
#define DEBUG(code)
#endif

#include <boost/program_options.hpp>

namespace po = boost::program_options;

struct init_parameters
{
  std::string inputfile;
  int eclevel;

  void parse(int argc, char* argv[]){
    // Loading command line arguments...
    po::options_description desc("Supported options");
    desc.add_options()
      ("help", "show this list")
      ("f", po::value<std::string>(&inputfile), "Input file")
      ("ec", po::value<int>(&eclevel), "EC level");
	po::positional_options_description pod;
    pod.add("f", -1);
	po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(pod).run(), vm);
    po::notify(vm);

    // Help
    if(vm.count("help")){
      std::cout << desc << '\n';
      exit(0);
    }
    // Input file
    if(!vm.count("f")){
      std::cerr << "\t[Error]: An input file must be specified!\n" << desc << '\n';
      exit(1);
    }
  }
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
	byte mask;
	int version, size;
	Err_Level error_level;
	Encoding encoding;
};

namespace{
	byte c2alpha(const char t)
	{
		switch (t){
			case ' ': return 36;
			case '$': return 37;
			case '%': return 38;
			case '*': return 39;
			case '+': return 40;
			case '-': return 41;
			case '.': return 42;
			case '/': return 43;
			case ':': return 44;
		}
		if (isdigit(t)) return t - 48;
		if (isupper(t) && isalpha(t)) return t - 55;
		return 36;
	}

	void printLongBit(std::vector<bool>& vec)
	{
		int i = 0;
		for (auto b : vec){
			std::cout << (b ? 1 : 0);
			if (++i % 8 == 0) std::cout << ' ';
		}
		std::cout << '\n';
	}

	void pushBits(unsigned data, std::vector<bool>& vec, int amount)
	{
		for (unsigned p = 0x1U << (amount - 1); p; p >>= 1)
			vec.push_back(p & data ? true : false);
	}

	template<typename C> bool isAlphaNum(C t)
	{
		if (t > 255) return false;
		switch (t){
			case ' ': case '$': case '%': case '*':
			case '+': case '-': case '.': case '/':
			case ':': return true;
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
		std::cout << '\n';
	}

	std::ostream& operator<<(std::ostream& stream, varNum& num)
	{
		for (unsigned short p = (unsigned short)0x1 << (num.len - 1);
			p; p >>= 1)
			std::cout << (p&num.dat ? 1 : 0);
		return stream;
	}

} // end anonymous namespace

#endif // UTIL_H