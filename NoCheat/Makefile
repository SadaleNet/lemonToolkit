CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++0x -O3

all : MurmurHash3.o noCheat.o
	${CC} ${CFLAGS} MurmurHash3.o noCheat.o ${LDFLAGS} -o noCheat

MurmurHash3.o : external/MurmurHash3.cpp
	${CC} ${CFLAGS} -c external/MurmurHash3.cpp

noCheat.o : noCheat.h noCheat.cpp
	${CC} ${CFLAGS} -c noCheat.cpp


