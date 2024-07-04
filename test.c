#include <stdlib.h>
#include <unistd.h>

#include "halde.h"

int main(int argc, char *argv[]) {
	char *m5 = malloc(0);
	char *m1 = malloc(200*1024);
	char *m2 = malloc(20*20);
	char *m3 = malloc(40*40);
	char *m4 = malloc(99*99);
	printList();
	free(m1);
	free(m2);
	free(m3);
	free(m4);
	free(m5);
	printList();



	m1 = malloc(200*1023);
	m2 = malloc(45*45);
	m3 = malloc(100*100);
	m4 = malloc(69*69);
	m1 = realloc(m1, 250*1024);
	printList();

	free(m1);
	free(m2);
	free(m3);
	free(m4);

	printList();

	exit(EXIT_SUCCESS);
}
