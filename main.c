#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct Block {
    size_t size;
    int free;
    struct Block *prev;
    struct Block *next;
} chunk;

chunk *heapStart = NULL;

size_t align(size_t size) {
    return (size + 7) & ~7;
}

chunk *getChunk(void *data) {
    return (chunk *) ((char*) data  - sizeof(chunk));
}


void merge(chunk *chk) {
    chunk *next = chk->next;

	chk->size += next->size + sizeof(chunk); 
	chk->size = align(chk->size);
	chk->next = next->next;

	if(chk->next) {
		chk->next->prev = chk;
	}
}

void checkFreeChunks(void *heapStart) {
	chunk *currentChunk = heapStart;

	while(currentChunk != NULL && currentChunk->next != NULL) {
		while(currentChunk->free == 1 && currentChunk->next != NULL && currentChunk->next->free == 1) {
			merge(currentChunk);
		} 	
		currentChunk = currentChunk->next;
	}
}


int canSplit(chunk *chk, size_t size) {
	if(chk->size >= size + sizeof(chunk) + 1) {
		return 1;
	}
	return 0;
}

chunk *split(chunk *chk, size_t size) {
	size_t originalSize = chk->size;
	chk->size = size;

	chunk *newBlock = (chunk *) ((char *) chk + sizeof(chunk) + size);
	newBlock->size = originalSize - size - sizeof(chunk);
	newBlock->size = align(newBlock->size);
	newBlock->free = 1;

	newBlock->next = chk->next;
	if(newBlock->next) {
		newBlock->next->prev = newBlock;
	}

	chk->next = newBlock;
	chk->free = 0;
	newBlock->prev = chk;

	return chk;
}

void *firstFit(size_t size) {
	if(heapStart == NULL) {
		heapStart = sbrk(size + sizeof(chunk));
		if(heapStart == (void *)-1) {
			printf("nah");
			exit(1);
		}
		heapStart->size = size;
		heapStart->free = 0;
		heapStart->next = NULL;
		heapStart->prev = NULL;


		return (void *) (heapStart + 1);
	}
	
	chunk *currentChunk = heapStart;
	chunk *prev = NULL;

	for(currentChunk; currentChunk; currentChunk = currentChunk->next) {
		if(currentChunk != NULL && currentChunk->free == 1 && currentChunk->size >= size) {
			if(canSplit(currentChunk, size) == 1) {
				split(currentChunk, size);
			}
			currentChunk->free = 0;
			return (void *) (currentChunk + 1);
		}
        prev = currentChunk;
	}

	currentChunk = sbrk(size + sizeof(chunk));
	if(currentChunk == (void *)-1) {
			printf("nah");
			exit(1);
	}
	currentChunk->size = size;
	currentChunk->free = 0;
	currentChunk->next = NULL;
	currentChunk->prev = prev;
	if(prev != NULL) {
		prev->next = currentChunk;
	}
	return (void *) (currentChunk + 1);
}



void *allocate(size_t size){
	if(size <= 0) {
		printf("Memory size must be bigger than 0 bytes");
		return NULL;
	}

	size = align(size);
	void *chk = firstFit(size);

	if(chk != NULL) {
		return chk; // return the address pointer of the chunk found
	}

	printf("nah");
	exit(1);
}


void deallocate(void *data) {
    chunk *currentChunk = getChunk(data);
    currentChunk->free = 1;
}

void printHeap(void *heapStart) {
	chunk *current = heapStart;

	while(current != NULL) {
	    printf("  Address: %p\n", (void *)current);
	    printf("  Size: %zu\n", current->size);
	    printf("  Free: %d\n", current->free);
	    printf("  Prev: %p\n", (void *)current->prev);
	    printf("  Next: %p\n", (void *)current->next);
	    printf("----------------------\n");

	    current = current->next;
	}
}

int main() {
	chunk *chk = allocate(1024);
	chunk *chk1 = allocate(240);

	printHeap(heapStart);
	return 0;
}
