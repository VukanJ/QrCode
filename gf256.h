#ifndef GF256_H
#define GF256_H

#include <vector>

#include "Data.h"

class gf256
{
public:
	gf256();
	gf256(int p, bool valinit);
	gf256 operator+(const gf256&) const;
	gf256 operator*(const gf256&) const;
	unsigned short val, power;
	unsigned short calcExpo(int p); // Used only by setupLog()
};

static std::ostream& operator<<(std::ostream& stream, const gf256& gf)
{
	printf("[%d,%d]", gf.val, gf.power);
	return stream;
}

#endif