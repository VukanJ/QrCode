#include "gf256.h"

static std::vector<std::vector<int>> log_antilog;

gf256::gf256() :val(0), power(0){}

gf256::gf256(int p, bool valinit)
{
	if (valinit){
		val = p;
		power = log_antilog[1][p];
	}
	else{
		val = log_antilog[0][p];
		power = p;
	}
}

gf256 gf256::operator+(const gf256& k) const
{
	return gf256(val^k.val, true);
}

gf256 gf256::operator*(const gf256& k) const
{
	if (val == 0 || k.val == 0)
		return gf256(0, true);
	unsigned short respow = k.power + power;
	return respow>255 ? gf256(respow % 255, false) : gf256(respow, false);
}

unsigned short gf256::calcExpo(int p)
{
	unsigned short _val = 1;
	while (p>0){
		p--; _val <<= 1;
		if (_val>255)
			_val ^= 285;
	}
	return _val;
}

void setupLogtable()
{
	int val = 0;
	gf256 gf;
	static bool done = false;
	if (done){
		std::cerr << "Already initialized gf256!\n";
		throw;
	}
	log_antilog.push_back(std::vector<int>(256, 0));
	log_antilog.push_back(std::vector<int>(256, 0));
	for (int i = 0; i < 255; i++){
		val = gf.calcExpo(i);
		log_antilog[0][i] = val;
		log_antilog[1][val] = i;
	}
	log_antilog[0][255] = 1; // ?
	done = true;
}

#ifdef DEBUG_MODE
std::ostream& operator<<(std::ostream& stream, const gf256& gf)
{
	printf("[%d,%d]", gf.val, gf.power);
	return stream;
}
#endif