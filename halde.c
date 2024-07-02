#include "halde.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>

/// Magic value for occupied memory chunks.
#define MAGIC ((void*)0xbaadf00d)

/// Size of the heap (in bytes).
#define SIZE (1024*1024*1)

/// Memory-chunk structure.
struct mblock {
	struct mblock *next;
	size_t size;
	char memory[];
};

/// Heap-memory area.
static char *memory;

/// Pointer to the first element of the free-memory list.
static struct mblock *head;

/// Helper function to visualise the current state of the free-memory list.
void printList(void) {
	struct mblock *lauf = head;

	// Empty list
	if (head == NULL) {
		char empty[] = "(empty)\n";
		write(STDERR_FILENO, empty, sizeof(empty));
		return;
	}

	// Print each element in the list
	const char fmt_init[] = "(off: %7zu, size:: %7zu)";
	const char fmt_next[] = " --> (off: %7zu, size:: %7zu)";
	const char * fmt = fmt_init;
	char buffer[sizeof(fmt_next) + 2 * 7];

	while (lauf) {
		size_t n = snprintf(buffer, sizeof(buffer), fmt
			, (uintptr_t) lauf - (uintptr_t)memory, lauf->size);
		if (n) {
			write(STDERR_FILENO, buffer, n);
		}

		lauf = lauf->next;
		fmt = fmt_next;
	}
	write(STDERR_FILENO, "\n", 1);
}
static void *getNewMemory() {
	void *newMemory = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (newMemory == MAP_FAILED) {
		return NULL;
	}
	return newMemory;
}

static void setupNewMemory() {
	if (memory == NULL) {
		return;
	}
	head = (struct mblock *)memory;
	head->size = (SIZE - sizeof(struct mblock));
	head->next = NULL;
	return;
}
static char *setupMemory(struct mblock *current, size_t size){
//case 1: No enough space for new Block
	if (current->size < size + sizeof(struct mblock)) {
		head = current->next;
		current->next = (struct mblock *)MAGIC;
		return current->memory;
	}
	else {
		struct mblock *newBlock = (struct mblock *)(current->memory + size); //hier evtl size +1
		newBlock->size = (current->size -(sizeof(struct mblock) + size));
		head = newBlock;
		current->size = size;
		current->next = (struct mblock *)MAGIC;
		return current->memory;
	}
	return NULL;
}

void *malloc (size_t size) {
	if (memory == NULL) {
		void *newMemory = getNewMemory();
		if (newMemory == NULL) {
			return NULL;
		}
		memory = (char *)newMemory;
		setupNewMemory();
	}
	if (head == NULL) {
		return NULL;
	}
	struct mblock *current = head;
	while(current != NULL) {
		if (current->size >= size) {
			return (void *)setupMemory(current, size);
		}
		current = current->next;
	}
	//kein Speicher mehr frei
	return NULL;
}

void free (void *ptr) {
	if (ptr == NULL) {
		return;
	}
	struct mblock *block = (struct mblock *)((char *)ptr - sizeof(struct mblock));
	//my debut error outputs:
	if (block->next != (struct mblock*) MAGIC) {
		fprintf(stderr, "Error: tried to free, non allocated memory\n");
		return;
	}
	block->next = head;
	head = block;
	return;
}

void *realloc (void *ptr, size_t size) {
	// TODO: implement me!
	return NULL;
}

void *calloc (size_t nmemb, size_t size) {
	// TODO: implement me!
	return NULL;
}
