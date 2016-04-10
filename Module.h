#ifndef MODULE_H
#define MODULE_H

#include "Data.h"

class QR_Module
{
public:
	QR_Module();
	void write(bool, ModType);
	void write_lock(bool, ModType);
	bool isLocked();
	bool state();
	void unlock();
	ModType type;
private:
	bool locked;
	bool Color; // true == WHITE
};

#endif