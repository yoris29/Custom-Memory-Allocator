#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void *allocate(int val) {
    int *brk_point = (int *) sbrk(val);
    return brk_point;
}

int main() {
    int *breakPoint = allocate(60);
    printf("%p", breakPoint);

    return 0;
}
