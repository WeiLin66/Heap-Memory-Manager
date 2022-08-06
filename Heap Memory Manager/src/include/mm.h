#ifndef __MM_H_
#define __MM_H_

#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <unistd.h>  // get page size from kernel
#include <sys/mman.h> // mmap(), munmap()

#define DEBUG_ON  1
#define DEBUG_OFF 0
#define MM_DEBUG  DEBUG_ON

void mm_init(void);
void mm_debug_fn(void);

#endif /* __MM_H_ */