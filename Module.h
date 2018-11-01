#ifndef MODULE_H
#define MODULE_H

enum ModType{
	M_VOID,
	M_DATA,
	M_TIMING,
	M_FINDER,
	M_SEP,
	M_ALIGN,
	M_DARK_MODULE,
	M_VERSION,
	M_FORMAT
};

// QR module (="pixel") class for reliable writing of qr codes
class QR_Module
{
public:
	QR_Module();
	void write(bool, ModType) noexcept;
	void write_lock(bool, ModType) noexcept;
	bool isLocked() const noexcept;
	bool state() const noexcept;
	void unlock() noexcept;

	ModType type;
private:
	bool locked;
	bool Color; // true == white
};

#endif