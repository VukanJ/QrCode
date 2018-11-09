#include "util.h"

void init_parameters::parse(int argc, char* argv[]){
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

Byte c2alpha(const char t)
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
	if (isdigit(t)) return t - '0';
	if (isupper(t) && isalpha(t)) return t - 'A' + 10;
	return ' '; // Invalid character turns to space
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
	for (unsigned p = 0x1U << (amount - 1); p; p >>= 1){
		vec.push_back(static_cast<bool>(p & data));
	}
}

std::ostream& operator<<(std::ostream& stream, const varNum& num)
{
	for (unsigned short p = (unsigned short)0x1 << (num.len - 1); p; p >>= 1){
		stream << (p & num.dat ? 1 : 0);
	}
	return stream;
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vec)
{
	if (vec.empty()){
		stream << "[empty]";
	}
	else {
		stream << '[';
		for (const auto& ve : vec){
			stream << ve << ", ";
		}
		stream << "\b\b]";
		return stream;
	}
}
