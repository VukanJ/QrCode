#ifndef MASKINFO_H
#define MASKINFO_H

#include <functional>
#include <algorithm>
#include <cmath>

#include "Data.h"
#include "util.h"
#include "Module.h"

using QR_Field = std::vector<std::vector<QR_Module>>;

class MaskInfo
{
public:
	MaskInfo() = delete;
	MaskInfo(qrInfo*);
	void fillData(std::vector<bool>&, QR_Field&);
	void maskEvaluate(QR_Field&);
	
private:
	void applyMask(QR_Field&,int); // Mask 1 to 7

	std::vector<std::vector<bool>> formatInfo;
	std::vector<bool> versionInfo;
	void genFormatInfo();
	void genVersionInfo();
	qrInfo* info;
};

#endif // MASKINFO_H
