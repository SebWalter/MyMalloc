#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "halde.h"

int main(int argc, char *argv[]) {
	printList();
	puts("malloc(m1)");
	char *m1 = malloc(200*1024);
	printList();
	puts("----------------------\n");
	puts("malloc(m2)");
	char *m2 = malloc(20*20);
	printList();
	puts("----------------------\n");
	puts("malloc(m3)");
	char *m3 = malloc(40*40);
	printList();
	puts("----------------------\n");
	puts("malloc(m4)");
	char *m4 = malloc(99*99);
	printList();
	puts("----------------------\n");
	puts("free(m1)");	
	free(m1);
	printList();
	puts("----------------------\n");
	puts("free(m2)");
	free(m2);
	printList();
	puts("----------------------\n");
	puts("free(m3)");
	free(m3);
	printList();
	puts("----------------------\n");
	puts("free(m4)");
	free(m4);
	printList();
	puts("----------------------\n");
	puts("malloc(m1)");
	m1 = malloc(200*1023);
	printList();
	puts("----------------------\n");
	puts("malloc(m2)");
	m2 = malloc(45*45);
	printList();
	puts("----------------------\n");
	puts("malloc(m3)");
	m3 = malloc(100*100);
	printList();
	puts("----------------------\n");
	puts("malloc(m4)");
	m4 = malloc(69*69);
	printList();
	puts("----------------------\n");

	puts("free(m1)");	
	free(m1);
	printList();
	puts("----------------------\n");
	puts("free(m2)");
	free(m2);
	printList();
	puts("----------------------\n");
	puts("free(m3)");
	free(m3);
	printList();
	puts("----------------------\n");
	puts("free(m4)");
	free(m4);
	printList();
	puts("----------------------\n");

	exit(EXIT_SUCCESS);
}
