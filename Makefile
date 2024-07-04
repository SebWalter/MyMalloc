.PHONY = all, clean, test-ref
cc = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_GNU_SOURCE -g 
all: test
test: test.o halde.o
				$(cc) test.o halde.o -o test $(CFLAGS)
test.o: test.c
				$(cc) -c test.c $(CFLAGS)
halde.o: halde.c
				$(cc) -c halde.c $(CFLAGS)
test-ref: ref
ref: halde-ref.o
				$(cc) test.c halde-ref.o -o test-ref $(CFLAGS)
clean:
				rm -f test test.o test-ref

