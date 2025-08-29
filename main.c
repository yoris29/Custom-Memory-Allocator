#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct Block {
    size_t size;
    int free;
    struct Block *prev;
    struct Block *next;
    void *data;
} chunk;

chunk *heapStart = NULL;

void init_heap() {
	if(heapStart == NULL) {
		heapStart = sbrk(0);
	}
}

size_t align(size_t size) {
    return (size + 7) & ~7;
}

chunk *getChunk(void *data) {
    return (chunk *) ((char*) data  - sizeof(chunk));
}


void merge(chunk *chk) {
    chunk *next = chk->next;

    if(next == NULL || chk->free == 0) {
		return;
    }
    
	chk->size += next->size +  sizeof(chunk); 
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
	if(chk->size >= size + sizeof(chunk)) {
		return 1;
	}
	return 0;
}

void split(chunk *chk, size_t size) {
	size_t originalSize = chk->size;
	chk->size = size;

	chunk *newBlock = (chunk *) ((char *) chk + sizeof(chunk) + size);
	newBlock->size = originalSize - size - sizeof(chunk);
	newBlock->free = 1;

	newBlock->next = chk->next;
	if(newBlock->next) {
		newBlock->next->prev = newBlock;
	}

	chk->next = newBlock;
	newBlock->prev = chk;
}

chunk *firstFit(size_t size) {
	chunk *currentChunk = heapStart;

	for(currentChunk; currentChunk; currentChunk = currentChunk->next) {
		if(currentChunk != NULL && currentChunk->size >= size && currentChunk->free == 1) {
			if(currentChunk && canSplit(currentChunk, size) == 1) {
				split(currentChunk, size);
			}
			return currentChunk;
		}
	}
	return NULL;
}

void deallocate(void *data) {
    chunk *currentChunk = getChunk(data);
    currentChunk->free = 1;
}

void *allocate(size_t size){
	init_heap();
	if(size <= 0) {
		printf("Memory size must be bigger than 0 bytes");
		return;
	}

	size = align(size);
	chunk *chk = firstFit(size);
	checkFreeChunks(heapStart);

	if(chk != NULL) {
		chk->free = 0; // set free flag to false when we find a chunk
		return chk->data; // return the address pointer of the chunk found
	}

	printf("nah");
	exit(1);
}

int main() {
    int *breakPoint = allocate(60);
    printf("%p", breakPoint);

    return 0;
}
