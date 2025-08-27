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

size_t align(size_t size) {
    return (size + 7) & ~7;
}

chunk *getChunk(void *data) {
    return (chunk *) ((char*) data  - sizeof(chunk));
}

int canMerge(chunk *chk) {
    chunk *next = chk->next;
    if(next != NULL && chk->free && next->free) {
		return 1;
    }
    return 0;
}

void merge(chunk *chk) {
    chunk *next = chk->next;

    if(next == NULL || !chk->free) {
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
	for(currentChunk; currentChunk; currentChunk = currentChunk->next) {
		if(currentChunk->free && currentChunk->next->free) {
			merge(currentChunk);
		}
	}
}

int canSplit(chunk *chk, size_t size) {
	if(chk->size >= size + size(chunk)) {
		return 1;
	}
	return 0;
}

void split(chunk *chk, size_t size) {
	size_t originalSize = chk->size;
	chk->size = size;

	chunk *newBlock = (chunk *) ((char *) chunk+ sizeof(chunk) + size);
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
	void *heapStart = sbrk(0);
	chunk *currentChunk = heapStart;

	for(currentChunk; currentChunk; currentChunk = currentChunk->next) {
		if(currentChunk->size >= size && currentChunk->free && currentChunk != NULL) {
			if(currentChunk && canSplit(currentChunk, size)) {
				split(currentChunk, size);
			}
			return currentChunk;
		}
	}
	return;
}

void free(void *data) {
    chunk *currentChunk = getChunk(data);
    currentChunk->free = 1;
}

void *allocate(size_t size){
	if(size <= 0) {
		printf("Memory size must be bigger than 0 bytes");
		return;
	}

	size = align(size);
	chunk *chk = firstFit(size);
	checkFreeChunks(sbrk(0));

	if(chk != NULL) {
		block->free = 0; // set free flag to false when we find a chunk
		return block->data; // return the address pointer of the chunk found
	}

	printf("Didn't find a memory chunk yakho");
	return;
}

int main() {
    int *breakPoint = allocate(60);
    printf("%p", breakPoint);

    return 0;
}
