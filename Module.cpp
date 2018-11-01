#include "Module.h"

QR_Module::QR_Module()
{
	Color  = false;
	locked = false;
	type   = M_VOID;
}

void QR_Module::write(bool w, ModType t) noexcept
{
	if (!locked){
		Color = w;
		type  = t;
	}
}

void QR_Module::write_lock(bool w, ModType t) noexcept
{
	if (!locked){
		Color  = w;
		locked = true;
		type   = t;
	}
}

bool QR_Module::isLocked() const noexcept
{
	return locked;
}

bool QR_Module::state() const noexcept
{
	return Color;
}

void QR_Module::unlock() noexcept
{
	locked = false;
}