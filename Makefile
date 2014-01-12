CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++11 -O3

RMap.o: RMap/RMap.cpp RMap/RMap.hpp RMap/RMapSerializer/json.hpp
	${CC} ${CFLAGS} -c RMap/RMap.cpp

MurmurHash3.o: NoCheat/external/MurmurHash3.h NoCheat/external/MurmurHash3.cpp
	${CC} ${CFLAGS} -c NoCheat/external/MurmurHash3.cpp

NoCheat.o: MurmurHash3.o NoCheat/NoCheat.hpp NoCheat/NoCheat.cpp
	${CC} ${CFLAGS} -c NoCheat/NoCheat.cpp

NoOverflow.o: NoOverflow/NoOverflow.cpp NoOverflow/NoOverflow.hpp
	${CC} ${CFLAGS} -c NoOverflow/NoOverflow.cpp

all: RMap.o MurmurHash3.o NoCheat.o NoOverflow.o lemon.hpp main.cpp
	${CC} ${CFLAGS} RMap.o MurmurHash3.o NoCheat.o NoOverflow.o main.cpp -o main

clean:
	rm -f *.o

