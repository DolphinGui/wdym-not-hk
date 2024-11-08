.PHONEY: clean test

CFLAGS:=-Wall -O2
CXXFLAGS:=-std=c++26
LDFLAGS:=-lfmt -Xlinker -Map=output.map
OBJECTS = main.o       # object files forming executable
DEPENDS = ${OBJECTS:.o=.d}    # substitutes ".o" with ".d"
CPPFLAGS:= -I. -MMD

test: main
	./main

clean:
	rm *.o *.d main output.map

main: ${OBJECTS}
	g++ -o $@ $^ ${LDFLAGS} ${CXXFLAGS} ${CFLAGS}

-include ${DEPENDS}
