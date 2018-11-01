#include "Module.h"

QR_Module::QR_Module()
{
	Color = false;
	locked = false;
	type = M_VOID;
}

bool QR_Module::isLocked()
{
	return locked;
}

void QR_Module::write_lock(bool w, ModType t)
{
	if (!locked) {
		Color = w;
		locked = true;
		type = t;
	}
}

void QR_Module::write(bool w, ModType t)
{
	if (!locked){
		Color = w;
		type = t;
	}
}

bool QR_Module::state() const
{
	return Color;
}

void QR_Module::unlock()
{
	locked = false;
}