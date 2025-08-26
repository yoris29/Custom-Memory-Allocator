#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct Block {
    size_t size;
    int free;
    struct chunk *prev;
    struct chunk *next;
    void *data;
} chunk;

size_t align(size_t size) {
    return (size + 7) & ~7;
}

void *allocate(int val) {
    int *brk_point = (int *) sbrk(val);
    return brk_point;
}

int main() {
    int *breakPoint = allocate(60);
    printf("%p", breakPoint);

    return 0;
}
