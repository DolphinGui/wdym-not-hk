.PHONEY: clean test

CFLAGS:=-Wall -O2
CXXFLAGS:=-std=c++26
LDFLAGS:=-lfmt
OBJECTS = main.o       # object files forming executable
DEPENDS = ${OBJECTS:.o=.d}    # substitutes ".o" with ".d"
CPPFLAGS:= -I. -MMD

test: main
	./main

clean:
	rm *.o main

main: ${OBJECTS}
	g++ -o $@ $^ ${LDFLAGS} ${CXXFLAGS} ${CFLAGS}

-include ${DEPENDS}
