#ifndef EC_H
#define EC_H

#include <algorithm>

#include "Data.h"
#include "util.h"
#include "gf256.h"
#include "gf256poly.h"

class EC
{
public:
	EC() = delete;
	EC(qrInfo* inf);
	
	void ErrorCorrection(std::vector<bool>&);
	byteVec createErrorCode(byteVec&, int ecCount);
	qrInfo* info;
};

#endif // EC_H