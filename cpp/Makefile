.PHONEY: clean test

CFLAGS:=-Wall -O2
CXXFLAGS:=-std=c++26
LDFLAGS:=-lfmt

test: main
	./main

clean:
	rm *.o main

main: main.o
	g++ -o $@ $^ ${LDFLAGS} ${CXXFLAGS} ${CFLAGS}

