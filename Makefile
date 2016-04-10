CC = g++
CFLAGS = -std=c++11 -Wpedantic
LFLAGS = -lsfml-window -lsfml-system -lsfml-graphics
OUTNAME = QRgen

OBJ = Main.o QR.o Encoder.o EC.o Module.o gf256.o gf256poly.o Printer.o MaskInfo.o

all: ${OBJ} EXE clean

run:
	./${OUTNAME}

bake: all run

Main.o:Main.cpp
	${CC} Main.cpp -c ${CFLAGS}

QR.o:QR.cpp
	${CC} QR.cpp -c ${CFLAGS}

gf256.o:gf256.cpp
	${CC} gf256.cpp -c ${CFLAGS}

EC.o:EC.cpp
	${CC} EC.cpp -c ${CFLAGS}

Module.o:Module.cpp
	${CC} Module.cpp -c ${CFLAGS}

Encoder.o:Encoder.cpp
	${CC} Encoder.cpp -c ${CFLAGS}

gf256poly.o:gf256poly.cpp
	${CC} gf256poly.cpp -c ${CFLAGS}

MaskInfo.o: MaskInfo.cpp
	${CC} MaskInfo.cpp -c ${CFLAGS}

Printer.o: Printer.cpp
	${CC} Printer.cpp -c ${CFLAGS}

EXE:
	${CC} ${OBJ} -o ${OUTNAME} ${LFLAGS}

clean:
	rm *.o
	clear
