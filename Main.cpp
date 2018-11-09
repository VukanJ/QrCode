#include <iostream>
#include <ctime>
#include <algorithm>

#include "QR.h"
#include "EC.h"
#include "Data.h"
#include "gf256.h"
#include "gf256poly.h"

extern void setupLogtable();

int main(int argc, char* argv[])
{
	init_parameters initparams;
  	initparams.parse(argc, argv);

	Err_Level err = _L_;
	
	switch(initparams.eclevel){
		case 1: err = _L_; break;
		case 2: err = _M_; break;
		case 3: err = _Q_; break;
		case 4: err = _H_; break;
		default: std::cerr << argv[1][0] << " is not a valid ec-level\n" << usageMsg << '\n'; 
			return 1;
	}
	
	// Compute QR Code
	setupLogtable();
	QR qr;
	sf::Image qrImage = qr.create(initparams.inputfile, err);
	//qr.print();
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
	qr.reset();
	window.setFramerateLimit(60);
	window.setPosition(sf::Vector2i(1500, 300));

	while (window.isOpen()){
		sf::Event ev;
		while (window.pollEvent(ev)){
			switch (ev.type){
				case sf::Event::Closed: window.close(); break;
				case sf::Event::KeyPressed:
					switch (ev.key.code){
						case sf::Keyboard::Escape:window.close(); break;
						case sf::Keyboard::S:{
							// Producing QR output by taking screenshot
							sf::Texture screenshot_tex;
							screenshot_tex.update(window);
							sf::Image screenshot = screenshot_tex.copyToImage();
							screenshot.saveToFile("QRCode.bmp");
							window.close();
							return 0;
						}
						default: break;
					} break;
				case sf::Event::Resized:
					auto size = std::min(window.getSize().x, window.getSize().y);
					size -= size%qrSize;
					size = std::max<int>(size, qrSize);
					std::cout << "." << std::flush;
					//window.create(sf::VideoMode(800, 600), "My window");
					//window.setSize(sf::Vector2u(size,size));
					break;
			}
		}
		window.clear(sf::Color::Blue);
		window.draw(qrSpr);
		window.display();
	}
}