#ifndef __MY_MALLOC_H_
#define __MY_MALLOC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>

#define DEBUG_ON    1
#define DEBUG_OFF   0
#define MY_DEBUG    DEBUG_ON

#define GET_VM_SIZE 256
#define META_SIZE   sizeof(META_BLK)
#define META_LIST_INIT(list) static META_BLK_LIST list = {NULL, NULL, NULL}

#define ITERATE_LIST_BEGIN(_node, head)         \
        {                                       \
           META_BLK* _node = (META_BLK*)head;   \
           META_BLK* _node_next = NULL;         \
           for(; _node; _node = _node_next){    \
                _node_next = _node->next;

#define ITERATE_LIST_END }}

#define GET_META_HEAD (META_BLK*)meta_blk_list.head
#define GET_META_CUR (META_BLK*)meta_blk_list.cur
#define GET_META_TAIL (META_BLK*)meta_blk_list.tail
#define GET_DATA_BLK(addr) (META_BLK*)addr + 1
#define GET_META_BLK(addr) (META_BLK*)addr - 1

#define NEXT_SPLIT_META(addr, size) (META_BLK*)((uint8_t*)addr + META_SIZE + size)

typedef struct _META_BLK{
    uint32_t data_blk_size;
    bool is_empty;
    struct _META_BLK* pre;
    struct _META_BLK* next;
}META_BLK;

typedef struct _META_BLK_LIST{
    uint8_t* head;
    uint8_t* cur;
    uint8_t* tail;
}META_BLK_LIST;

void* ff_malloc(size_t size);
void ff_free(void* addr);
void* bf_malloc(size_t size);
void bf_free(void* addr);



#endif /* __MY_MALLOC_H_ */