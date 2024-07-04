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
//the dummy is used to return a pointer to free, when calling malloc with 0
//we set next to MAGIC, so the pointer passes the dereference check in free
static struct mblock dummy = {
	.size = 0,
	.next = (struct mblock*)MAGIC,
};

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
//gets new memory-page with a system call
static void *getNewMemory() {
	void *newMemory = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (newMemory == MAP_FAILED) {
		return NULL;
	}
	return newMemory;
}
//puts a block at the beginning of the memory
//sets the head to the new block 
static void setupNewMemory() {
	if (memory == NULL) {
		return;
	}
	head = (struct mblock *)memory;
	head->size = (SIZE - sizeof(struct mblock));
	head->next = NULL;
	return;
}

void *malloc (size_t size) {
	//don't accept negative values
	if (size < 0) {
		return NULL;
	}
	//malloc(0) should return a invalid pointer, that can be freed
	//So we return a dummy pointer
	if (size == 0) {
		return &dummy.memory;
	}
	if (memory == NULL) {
		void *newMemory = getNewMemory();
		if (newMemory == NULL) {
			return NULL;
		}
		memory = (char *)newMemory;
		setupNewMemory();
	}
	//happens when no more free space is in the heap
	if (head == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	struct mblock *current = head;
	struct mblock *previous = NULL;
	while (current != NULL) {
		if (current->size >= size) {
		    size_t remainingSize = current->size - size - sizeof(struct mblock);

		    if (remainingSize > sizeof(struct mblock)) {
			//There is space left to insert a new block
			struct mblock *newBlock = (struct mblock *)(current->memory + size);
			newBlock->size = remainingSize;
			newBlock->next = current->next;

			current->size = size;
			current->next = (struct mblock *)MAGIC;

			if (previous) {
			    previous->next = newBlock;
			} 
			else {
			    head = newBlock;
			}
		    } 
		    //now ne block fits in ->allocate the whole block
		    else {
			if (previous) {
			    previous->next = current->next;
			} 
			else {
			    head = current->next;
			}
			current->next = (struct mblock *)MAGIC;
		    }

		    return current->memory;
		}

		previous = current;
		current = current->next;
	}
	//when mblocks are in the free list, but nobody is big enough to 
	//fit the new space
	errno = ENOMEM;	
	return NULL;
}
void free (void *ptr) {
	if (ptr == NULL) {
		return;
	}
	struct mblock *block = (struct mblock *)((char *)ptr - sizeof(struct mblock));
	//All blocks that are "real" have Magic on the bock->next.
	//Invalid blocks don't have it so we can assume the user tried to free
	//a invalid *
	if (block->next != (struct mblock*) MAGIC) {
		abort();
	}
	block->next = head;
	head = block;
	return;
}
//for realloc we need to now how big the memory is that we want to copy
static size_t getPointerSize(void *ptr) {
	if (ptr == NULL) {
		return -1;
	}
	struct mblock *block = (struct mblock *)((char *)ptr - sizeof(struct mblock));
	if (block->next != (struct mblock*)MAGIC) {
		return -1;
	}
	return block->size;	
}

static size_t calculateN(void *new,size_t new_size, void *old) {
	size_t old_size = getPointerSize(old);
	if (old_size == -1) {
		return -1;
	}
	if (new_size > old_size) {
		return old_size;
	}
	return new_size;

}



void *realloc (void *ptr, size_t size) {
	//realloc(0) should work like free		
	if (size == 0) {
		free(ptr);
		return NULL;
	}
	char *newMem = malloc(size);
	//when malloc fails, just return
	if (newMem == NULL) {
		return NULL;
	}
	//realloc(NULL, size) should work like malloc
	if (ptr == NULL) {
		return newMem;
	}
	//for memcpy we need a value n that indicates how many bytes we want to cpy
	//to prevent overflows, we need the smallest allocated pointer size from
	//the new and the old
	size_t n = calculateN(newMem,size, ptr);
	if (memcpy(newMem, ptr, n) != newMem) {
		free(newMem);
		return NULL;
	}
	free(ptr);
	return newMem;
}

void *calloc (size_t nmemb, size_t size) {
	if (nmemb == 0 || size == 0) {
		return &dummy.memory;
	}
	//check for an overflow of nmemb * size
	if (nmemb > SIZE_MAX / size) {
		errno = ENOMEM;
		return NULL;
	}
	size_t sizeToMalloc = nmemb * size;
	char *memory = malloc(sizeToMalloc);
	if (memory == NULL) {
		return NULL;
	}
	return memory;
}
