#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

/**
 * @brief version 1 using simple sbrk() to implement
 * 
 * @param size 
 * @return void* 
 */
void* my_malloc(int size){
    void* ptr = sbrk(0);

    if(sbrk(size) == NULL){
        return NULL;
    }

    return ptr;
}

/**
 * @brief using sbrk() to decrease break point
 * 
 * @param size 
 */
void my_free(int size){
    assert(size > 0);
    sbrk(size * -1);
}

int main(int argc, char** argv){
    char* ptr = (char*)my_malloc(16);
    
    strncpy(ptr, "test", 4);
    printf("ptr: %s\n", ptr);
    my_free(16);

    return 0;
}