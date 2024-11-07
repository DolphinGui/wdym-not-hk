.PHONEY: clean test

CFLAGS:=-Wall -O2

test: main
	./main

clean:
	rm *.o main

main: main.o
	gcc -Wall -o $@ $^ ${CFLAGS}

