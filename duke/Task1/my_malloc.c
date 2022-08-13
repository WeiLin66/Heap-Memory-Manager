#include "my_malloc.h"

META_LIST_INIT(meta_blk_list);

/**
 * Get Virtual Memory form kernel
 */ 
static void get_vm_from_kernel(META_BLK_LIST* list){

    void* get_mem = NULL;

    if(list->head == NULL && list->cur == NULL){

        #if (MY_DEBUG)
            printf("[Info]: First get Virtual Memoey from kernel!\n");
        #endif
        get_mem = sbrk(0);
        list->head = list->tail = get_mem;
    }

    get_mem = sbrk(GET_VM_SIZE);
    assert(get_mem != (void*)-1);
    list->tail += GET_VM_SIZE;

    #if (MY_DEBUG)
        printf("[Info]: Extend Virtual Memory: %d\n", GET_VM_SIZE);
    #endif
}


/**
 * print all dll info
 */ 
static void print_meta_blk_info(){
    #if (MY_DEBUG)

        printf("[Info]: ");
        ITERATE_LIST_BEGIN(ptr, GET_META_HEAD)
            printf("[size: %d, is_empty: %d] --> ", ptr->data_blk_size, ptr->is_empty);
        ITERATE_LIST_END

        printf("NULL\n");

        printf("[Info]: ");
        ITERATE_LIST_REVERSE_BEGIN(ptr, GET_META_CUR)
            printf("[size: %d, is_empty: %d] --> ", ptr->data_blk_size, ptr->is_empty);
        ITERATE_LIST_END

        printf("NULL\n");
    #endif
}


/**
 * split data block when demanded size is smaller than actual data block size
 */ 
static void* split(META_BLK* node, size_t size){

    #if (MY_DEBUG)
        printf("[Info]: Split Block!\n");
    #endif

    uint32_t original_size = node->data_blk_size;

    node->data_blk_size = size;
    META_BLK* new_meta = NEXT_SPLIT_META(node, size);
    new_meta->data_blk_size = original_size - (META_SIZE + size);
    new_meta->is_empty = true;
    
    new_meta->next = node->next;
    if(node->next){
        node->next->pre = new_meta;
    }
    new_meta->pre = node;

    node->next = new_meta;
    node->is_empty = false;

    return (uint8_t*)node + META_SIZE;
}


/**
 * merge free data blocks
 */ 
static void merge(META_BLK* node){

    META_BLK* pre_blk = node->pre;
    META_BLK* next_blk = node->next;
    META_BLK* ret = node;

    uint32_t total_blk_size = 0;
    bool merge = false;

    if(pre_blk == NULL && next_blk == NULL){

        return;
    }

    if(pre_blk != NULL && pre_blk->is_empty){

        total_blk_size += (pre_blk->data_blk_size + META_SIZE);
        ret = pre_blk;
        pre_blk = pre_blk->pre;
        merge = true;
    }

    if(next_blk != NULL && next_blk->is_empty){

        total_blk_size += (next_blk->data_blk_size + META_SIZE);
        next_blk = next_blk->next;
        merge = true;
    }

    if(!merge){

        return;
    }

    total_blk_size += node->data_blk_size;

    ret->data_blk_size = total_blk_size;
    ret->is_empty = true;

    ret->pre = pre_blk;
    if(pre_blk){
        pre_blk->next = ret;
    }

    ret->next = next_blk;
    if(next_blk){
        next_blk->pre = ret;
    }

    #if (MY_DEBUG)
        printf("[Info]: Merge Block!\n");
    #endif
}


/**
 * find any empty block in dll
 */ 
static void* find_empty_blk(size_t size){

    if(meta_blk_list.cur == NULL){

        return NULL;
    }

    ITERATE_LIST_BEGIN(ptr, GET_META_HEAD)
        if(ptr->is_empty){
            
            if(ptr->data_blk_size == size){

                ptr->is_empty = false;
                return (uint8_t*)ptr + META_SIZE;
            }else if(ptr->data_blk_size > META_SIZE + size){

                return split(ptr, size);
            }
        }
    ITERATE_LIST_END   

    return NULL;
}


/**
 * first fit malloc func
 */ 
void* ff_malloc(size_t size){

    uint32_t total_blk_length = size + META_SIZE;
    uint32_t curr_blk_length = 0;
    META_BLK* find_empty_blk_res = NULL;

    if(meta_blk_list.head == NULL && meta_blk_list.cur == NULL){

        get_vm_from_kernel(&meta_blk_list);
    }

    if((find_empty_blk_res = find_empty_blk(size))){

        return find_empty_blk_res;
    }

    if(meta_blk_list.cur != NULL){

        curr_blk_length = GET_META_CUR->data_blk_size + META_SIZE;
        if(meta_blk_list.cur + curr_blk_length + total_blk_length > meta_blk_list.tail){

            get_vm_from_kernel(&meta_blk_list);
        }
    }

    META_BLK* new_meta = meta_blk_list.cur == NULL ? GET_META_HEAD : (META_BLK*)(meta_blk_list.cur + curr_blk_length);
    new_meta->data_blk_size = size;
    new_meta->is_empty = false;
    new_meta->pre = NULL;
    new_meta->next = NULL;

    if(new_meta != GET_META_HEAD){

        new_meta->pre = GET_META_CUR;
        GET_META_CUR->next = new_meta;
        meta_blk_list.cur = (uint8_t*)new_meta;
    }else{

        meta_blk_list.cur = (uint8_t*)GET_META_HEAD;
    }

    return meta_blk_list.cur + META_SIZE;
}


/**
 * first fit free func
 */ 
void ff_free(void* addr){

    if(addr == NULL){

        #if (MY_DEBUG)
            printf("[Error]: Address is NULL!\n");
        #endif

        return;
    }

    META_BLK* free_target = GET_META_BLK(addr);

    ITERATE_LIST_BEGIN(ptr, GET_META_HEAD)
        if(ptr == free_target){

            ptr->is_empty = true;
            merge(ptr);
            return;
        }
    ITERATE_LIST_END
}

int main(int argc, char*argv[]){
    
    char example[] = "simple test";
    char example2[] = "new data";

    char* ptr1 = ff_malloc(20);
    char* ptr2 = ff_malloc(30);
    char* ptr3 = ff_malloc(40);
    char* ptr4 = ff_malloc(50);

    strncpy(ptr1, example, strlen(example)+1);
    strncpy(ptr2, example2, strlen(example2)+1);
    strncpy(ptr3, example2, strlen(example2)+1);
    strncpy(ptr4, example2, strlen(example2)+1);
    
    print_meta_blk_info();

    char* ptr5 = ff_malloc(100);

    strncpy(ptr5, example, strlen(example)+1);

    ff_free(ptr1);
    ff_free(ptr2);
    ff_free(ptr4);

    print_meta_blk_info();

    char* ptr6 = ff_malloc(46);

    strncpy(ptr6, example2, strlen(example2)+1);

    print_meta_blk_info();

    return 0;
}