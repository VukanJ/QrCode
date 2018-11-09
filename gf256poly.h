#ifndef GF256POLY_H
#define GF256POLY_H

#include <vector>
#include <algorithm>

#include "Data.h"
#include "gf256.h"

class gf256poly
{
private:
	struct pol
	{
		pol() :alpha(0, true), xex(0){};
		pol(gf256 gf, Byte e)
			:alpha(gf), xex(e){};
		gf256 alpha;
		Byte xex;
	};
	void collectLikeTerms();
public:
	gf256poly();
	gf256poly(std::vector<Byte>&, bool valinit);
	void createGenerator(int len);
	gf256poly operator/(const gf256poly&);
	std::vector<pol> poly;
	int genOrder;
};

#ifdef DEBUG_MODE
std::ostream& operator<<(std::ostream& stream, const gf256poly& poly);
#endif 

#endif // GF256POLY_H