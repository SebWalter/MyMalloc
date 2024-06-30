.PHONY = all, clean, test-ref
cc = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_GNU_SOURCE -g 
all: halde
halde: test.o halde.o
				$(cc) test.o halde.o -o halde $(CFLAGS)
test.o: test.c
				$(cc) -c test.c $(CFLAGS)
halde.o: halde.c
				$(cc) -c halde.c $(CFLAGS)
test-ref: ref
ref: halde-ref.o
				$(cc) test.c halde-ref.o -o test-ref $(CFLAGS)
clean:
				rm -f halde test.o test-ref

