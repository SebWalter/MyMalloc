#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "halde.h"

int main(int argc, char *argv[]) {
	//randfall call malloc(0)
	//erwartetes ergebnis: invalider pointer then man freen kann
	char *m0 = malloc(0);
	//randfall erzwinge integer overflow in calloc
	// --> return NULL + errno
	char *m00 = calloc(99999, 99999);
	if (m00 == NULL) {
		perror("calloc");
	}

	char *m1 = malloc(200*1024);
	char *m2 = malloc(20*20);
	char *m3 = malloc(40*40);
	char *m4 = malloc(99*99);
	printList();
	free(m1);
	free(m2);
	free(m3);
	free(m4);
	free(m0);
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
