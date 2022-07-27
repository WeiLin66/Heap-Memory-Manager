#include "my_malloc.h"


int main(int argc, char*argv[]){
    int t = brk((void*)0xfffff);
    printf("t = %d\n", t);

    return 0;
}