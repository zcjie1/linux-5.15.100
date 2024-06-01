// init.c
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("Hello, QEMU!\n");
    while (1) {
        sleep(5);
    }
    return 0;
}
