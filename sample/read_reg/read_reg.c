#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "securec.h"
#include "memmap.h"

int main(int argc, char *argv[])
{
    void* addr = NULL;
    unsigned int hex_addr = 0;
    if(argc != 2) {
        printf("usage: %s <hex addr>\n", argv[0]);
        return -1;
    }
    hex_addr = strtoul(argv[1], NULL, 16);
    addr = memmap(hex_addr,4);

    if (addr == NULL) {
        printf("map mem failed!\n");
        return -1;
    }
    printf("map mem success!\n");
    printf("addr = 0x%x\n", *(int*)addr);
    memunmap(addr);
    return 0;
}