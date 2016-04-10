#include <iostream>
#include <ctime>

#include "QR.h"
#include "Data.h"
#include "gf256.h"
#include "gf256poly.h"

extern void setupLog();

int main(int argc, char* argv[])
{
	Err_Level err = _L_;
	//
	try{
		if (argc != 2)throw usageMsg;
		switch (argv[1][0]){
		case 'L': case 'l': err = _L_; break;
		case 'M': case 'm': err = _M_; break;
		case 'Q': case 'q': err = _Q_; break;
		case 'H': case 'h': err = _H_; break;
		default: throw argv[1][0];
		}
	}
	catch (const std::string& e){
		std::cerr << e;
		return 1;
	}
	catch (const char& e){
		std::cerr << e << " is not a valid ec-level\n" << usageMsg << std::endl;
		return 1;
	}
	//
	setupLog();
	QR qr;
	sf::Image qrImage = qr.create("input.txt", err);
	std::cout << "\t~~~~DONE~~~~\n\tPress S to Save\nResize code by resizing window\n";
	// SetUp Sprite
	sf::Texture qrTex;
	qrTex.loadFromImage(qrImage);
	sf::Sprite qrSpr;
	qrSpr.setTexture(qrTex);
	qrSpr.setScale(sf::Vector2f(5, 5));
	// Create window
	int qrSize = (21 + 8 + 4 * qr.getVersion());
	sf::RenderWindow window(sf::VideoMode(qrSize*5, qrSize*5), "QR_Preview");
	qr._reset_();
	window.setFramerateLimit(60);
	window.setPosition(sf::Vector2i(1500, 300));

	bool doneResize = true;

	while (window.isOpen()){
		sf::Event ev;
		while (window.pollEvent(ev)){
			switch (ev.type){
			case sf::Event::Closed: window.close(); break;
			case sf::Event::KeyPressed:
				switch (ev.key.code){
				case sf::Keyboard::Escape:window.close(); break;
				case sf::Keyboard::S:
					window.capture().saveToFile("QRCode.Bmp");
					return 0;
				}break;
			case sf::Event::Resized:
				if(!doneResize){
					unsigned int size = std::min(window.getSize().x, window.getSize().y);
					size -= size%qrSize;
					window.setSize(sf::Vector2u(size,size));
					std::cout << "resized";
					doneResize = true;
				}
				break;
			}
		}
		doneResize = false;
		window.clear(sf::Color::Blue);
		window.draw(qrSpr);
		window.display();
	}
	return 0;
}

/*
TODO:
Clean up code
Implement KANJI
Create static sfml build
Compile in Release Mode
*/
