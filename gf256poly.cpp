#include "gf256poly.h"

gf256poly::gf256poly(){}

gf256poly::gf256poly(std::vector<Byte>& bytes, bool valinit)
{
	genOrder = 0;
	unsigned int maxex = bytes.size() - 1;
	for (auto& b : bytes) poly.push_back(pol(gf256(b, valinit), maxex--));
}

void gf256poly::createGenerator(int len)
{
	// Multiply polynomials ...
	genOrder = len;
	this->poly = { pol(gf256(0, false), 1), pol(gf256(0, false), 0) };
	std::vector<pol> nextIt;
	for (int i = 1; i <= len - 1; i++){
		for (auto& p : poly){
			nextIt.push_back(pol(p.alpha, (p.xex + 1) % 255));
			nextIt.push_back(pol(gf256((p.alpha.power + i) % 255, false), p.xex));
		}
		poly = nextIt;
		nextIt.clear();
		collectLikeTerms();
	}
}

void gf256poly::collectLikeTerms()
{
	// Used by createGenerator()
	std::vector<pol> nextIt;
	while (poly.size() != 0){
		nextIt.push_back(poly[0]);
		poly.erase(poly.begin());
		for (auto it = poly.begin(); it != poly.end();){
			if ((*it).xex == nextIt.back().xex){
				nextIt.back().alpha = nextIt.back().alpha + (*it).alpha;
				it = poly.erase(it);
			}
			else it++;
		}
	}
	poly = nextIt;
}

gf256poly gf256poly::operator/(const gf256poly& generator)
{
	// Polynomial division in GF(256)
	auto msg = *this;
	gf256poly gen = generator;
	auto itcount = msg.poly.size();
	for (int i = 0; i < itcount; i++){
		gen = generator;
		gf256 lead = msg.poly[0].alpha;
		std::for_each(gen.poly.begin(), gen.poly.end(), 
			[&](pol& p){p.alpha = p.alpha * lead;});
		for (int g = 0; g < gen.poly.size(); g++)
			if (g<msg.poly.size()) 
				msg.poly[g].alpha = msg.poly[g].alpha + gen.poly[g].alpha;
			else msg.poly.push_back(gen.poly[g]);
		if(msg.poly[0].alpha.val == 0)msg.poly.erase(msg.poly.begin());
	}
	return msg;
}

#ifdef DEBUG_MODE
std::ostream& operator<<(std::ostream& stream, const gf256poly& poly)\
{
	for (auto& p : poly.poly){
		std::cout << p.alpha << '+';
	}
	return stream;
#endif
