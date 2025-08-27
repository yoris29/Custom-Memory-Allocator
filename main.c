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
    if(next != NULL && next->free) {
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

void free(void *data) {
    chunk *currentChunk = getChunk(data);
    currentChunk->free = 1;
}

void *allocate(int val) {
    int *brk_point = (int *) sbrk(val);
    if(brk_point == (void *) - 1) {
	printf("Can't allocate memory yakho :p");
	return;
    }
    return brk_point;
}

int main() {
    int *breakPoint = allocate(60);
    printf("%p", breakPoint);

    return 0;
}
