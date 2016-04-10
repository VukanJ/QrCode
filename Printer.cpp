#include "Printer.h"

Printer::Printer(qrInfo* inf) : info(inf){}

void Printer::printAllPatterns(QR_Field& qr_field)
{
	// initialize qr_field
	for (int i = 0; i < info->size; i++)
		qr_field.push_back(std::vector<QR_Module>(info->size, QR_Module()));
	// Print Finder Pattern
	//
	int c = 0;
	for (int i = 0; i < 7; i++){
		c = 0;
		for (byte p = 0x1; c != 7; p <<= 1, c++){
			qr_field[i][c].write_lock(finder_dat[i] & p ? true : false, ModType::M_FINDER);
			qr_field[i][c + info->size - 7].write_lock(finder_dat[i] & p ? true : false, ModType::M_FINDER);
			qr_field[i + info->size - 7][c].write_lock(finder_dat[i] & p ? true : false, ModType::M_FINDER);
		}
	}
	c = 0;
	int d = 0;
	// Print Alignment Pattern 

	unsigned int x, y;
	if (info->version > 0){
		auto posVector = alignmentDat[info->version - 1];
		for (int i = 0; i < posVector.size(); i++)
			for (int j = 0; j < posVector.size(); j++){
				x = posVector[i];
				y = posVector[j];
				//std::cout << x << ' ' << y << std::endl;
				d = 0;
				if (qr_field[x][y].type == M_VOID)
					for (auto& p : align_dat){
						c = 0;
						for (byte b = (byte)0x1; c != 5; b <<= 1, c++){
							qr_field[x - 2 + c][y - 2 + d].write_lock(p & b ? true : false, ModType::M_ALIGN);
						}
						d++;
					}
			}
	}
	// Print Timing Pattern
	bool tim = false;
	for (int i = 0; i < info->version * 4 + 21; i++){
		tim = tim ? false : true;
		if (qr_field[6][i].type == M_VOID)
			qr_field[6][i].write_lock(tim, M_TIMING);
		if (qr_field[i][6].type == M_VOID)
			qr_field[i][6].write_lock(tim, M_TIMING);
	}

	// Reserve Spaces 
	// 0: Dark Module 
	qr_field[8][4 * (info->version + 1) + 9].write_lock(true, M_DARK_MODULE);
	// 1: Separators 
	for (int i = 0; i < 8; i++){
		qr_field[7][i].write_lock(false, M_SEP);
		qr_field[info->size - 8][i].write_lock(false, M_SEP);
		qr_field[i][7].write_lock(false, M_SEP);
		qr_field[i][info->size - 8].write_lock(false, M_SEP);
		qr_field[i + info->size - 8][7].write_lock(false, M_SEP);
		qr_field[7][i + info->size - 8].write_lock(false, M_SEP);
	}
	// 2: Format information
	if (info->version + 1 >= 7){
		for (int i = 0; i < 3; i++){
			for (int j = 0; j < 6; j++){
				qr_field[info->size - 11 + i][j].write_lock(false, M_FORMAT);
				qr_field[j][info->size - 11 + i].write_lock(false, M_FORMAT);
			}
		}
	}
	for (int i = 0; i < 9; i++){
		qr_field[8][i].write_lock(false, M_FORMAT);
		qr_field[i][8].write_lock(false, M_FORMAT);
	}
	for (int i = 0; i < 8; i++){
		qr_field[i + info->size - 8][8].write_lock(false, M_FORMAT);
		qr_field[8][i + info->size - 8].write_lock(false, M_FORMAT);
	}
}

void Printer::printFinal(sf::Image& qrCode, QR_Field& qr_field)
{
	// Create image + Quiet Zone
	qrCode.create(info->size + 8, info->size + 8, sf::Color::White);
	for (int i = 0; i < info->size; i++)
		for (int j = 0; j < info->size; j++){
			qrCode.setPixel(i + 4, j + 4, qr_field[i][j].state() ?
				sf::Color::Black : sf::Color::White);
		}
}

void Printer::printDebugFinal(sf::Image& qrCode, QR_Field& qr_field)
{
	// Create image + Quiet Zone	
	sf::Color tempCol;
	qrCode.create(info->size + 8, info->size + 8, sf::Color(100, 100, 100));
	for (int i = 0; i < info->size; i++){
		for (int j = 0; j < info->size; j++){
			qrCode.setPixel(i + 4, j + 4, qr_field[i][j].state() ?
				sf::Color::Black : sf::Color::White);
			tempCol = qrCode.getPixel(i + 4, j + 4);
			switch (qr_field[i][j].type){
			case M_SEP:
			case M_FINDER: tempCol = tempCol.r == 0 ? sf::Color(0, 50, 0) : sf::Color(0, 200, 0); break;
			case M_TIMING: tempCol = tempCol.r == 0 ? sf::Color(0, 0, 100) : sf::Color(130, 130, 200); break;
			case M_ALIGN:  tempCol = tempCol.r == 0 ? sf::Color(30, 0, 0) : sf::Color(200, 0, 0); break;
			case M_DATA:   tempCol = tempCol.r == 0 ? sf::Color(100, 0, 0) : sf::Color(200, 100, 100); break;
			case M_FORMAT: tempCol = tempCol.r == 0 ? sf::Color::Black : sf::Color::White; break;
			case M_DARK_MODULE: tempCol = sf::Color::Blue; break;
			default: tempCol = sf::Color(50, 50, 50); break;
			}
			qrCode.setPixel(i + 4, j + 4, tempCol);

		}
	}
}