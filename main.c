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

	chk->size += next->size; 
	chk->next = next->next;

	if(chk->next) {
		chk->next->prev = chk;
	}
}

void checkFreeChunks(void *heapStart) {
	chunk *currentChunk = heapStart;
	chunk *next = NULL;
	for(currentChunk; currentChunk; currentChunk = currentChunk->next) {
		next = currentChunk->next;
		if(currentChunk->free == 1 && next != NULL && next->free == 1) {
			merge(currentChunk);
		}
	}
}

int canSplit(chunk *chk, size_t size) {
	if(chk->size >= size) {
		return 1;
	}
	return 0;
}

void split(chunk *chk, size_t size) {
	size_t originalSize = chk->size;
	chk->size = size;

	chunk *newBlock = (chunk *) ((char *) chk + sizeof(chunk) + size);
	newBlock->size = originalSize - size;
	newBlock->free = 1;

	newBlock->next = chk->next;
	if(newBlock->next) {
		newBlock->next->prev = newBlock;
	}

	chk->next = newBlock;
	newBlock->prev = chk;
}

chunk *firstFit(size_t size) {
	if(heapStart == NULL) {
		heapStart = sbrk(size + sizeof(chunk));
		heapStart->size = size;
		heapStart->free = 0;
		heapStart->next = NULL;
		heapStart->prev = NULL;

		return (void *) (heapStart + 1);
	}
	
	chunk *currentChunk = heapStart;
	chunk *prev = NULL;

	for(currentChunk; currentChunk; currentChunk = currentChunk->next) {
		if(currentChunk != NULL && currentChunk->size >= size && currentChunk->free == 1) {
			if(currentChunk && canSplit(currentChunk, size) == 1) {
				split(currentChunk, size);
			}
			return (void *) (currentChunk + 1);
		}
        prev = currentChunk;
	}

	currentChunk = sbrk(size + sizeof(chunk));
	currentChunk->size = size;
	currentChunk->free = 0;
	currentChunk->next = NULL;
	currentChunk->prev = prev;
	if(prev != NULL) {
		prev->next = currentChunk;
	}
	return (void *) (currentChunk + 1);
}

void deallocate(void *data) {
    chunk *currentChunk = getChunk(data);
    currentChunk->free = 1;
}

void *allocate(size_t size){
	if(size <= 0) {
		printf("Memory size must be bigger than 0 bytes");
		return NULL;
	}

	size = align(size);
	chunk *chk = firstFit(size);
	checkFreeChunks(heapStart);

	if(chk != NULL) {
		return (void*) (chk + 1); // return the address pointer of the chunk found
	}

	printf("nah");
	exit(1);
}

int main() {
    int *breakPoint = allocate(60);
    printf("%p", breakPoint);

    return 0;
}
