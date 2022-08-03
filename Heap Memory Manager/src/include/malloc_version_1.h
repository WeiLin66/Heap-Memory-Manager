#ifndef __MALLOC_VERSION_1_H_
#define __MALLOC_VERSION_1_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

void* my_malloc(int size);
void my_free(int size);

#endif /* __MALLOC_VERSION_1_H_ */