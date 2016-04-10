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
		pol(gf256 gf, byte e)
			:alpha(gf), xex(e){};
		gf256 alpha;
		byte xex;
	};
	void collectLikeTerms();
public:
	gf256poly();
	gf256poly(std::vector<byte>&, bool valinit);
	void createGenerator(int len);
	gf256poly operator/(const gf256poly&);
	std::vector<pol> poly;
	int genOrder;
};

static std::ostream& operator<<(std::ostream& stream, const gf256poly& poly)
{
	for (auto& p : poly.poly)
		std::cout << p.alpha << '+';
	return stream;
}

#endif