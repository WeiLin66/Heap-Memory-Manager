#include "mm.h"

static size_t SYSTEM_PAGE_SIZE = 0;
static vm_page_family_list_t *first_vm_page_for_family = NULL;


/**
 * get VM Page Size
 */ 
void mm_init(){

    SYSTEM_PAGE_SIZE = getpagesize();
}


/**
 * request VM Page from kernel
 */ 
static void* mm_get_vm_page(uint32_t units){

    if(SYSTEM_PAGE_SIZE == 0){

        #if MM_DEBUG
            printf("System Page is 0!\n");
        #endif

        return NULL;
    }

    size_t length = units * SYSTEM_PAGE_SIZE;
    int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    int flag = MAP_ANON | MAP_PRIVATE;

    uint8_t* vm_page = mmap(0, length, prot, flag, 0, 0);

    if(vm_page == MAP_FAILED){

        #if MM_DEBUG
            printf("Fail to mmap VM page from kernel!\n");
        #endif

        return NULL;
    }

    memset(vm_page, 0x0, length);

    #if MM_DEBUG
        printf("Successfully mmap VM page from kernel!\n\n");
    #endif

    return (void*)vm_page;
}


/**
 * release VM Page
 */ 
static void mm_release_vm_page(void* vm_page, uint32_t units){

    if(SYSTEM_PAGE_SIZE == 0){

        #if MM_DEBUG
            printf("System Page is 0!\n");
        #endif

        return;
    }

    size_t length = units * SYSTEM_PAGE_SIZE;

    if(munmap(vm_page, length) == -1){

        #if MM_DEBUG
            printf("Fail to munmap VM page to kernel!\n");
        #endif
    }

    #if MM_DEBUG
        printf("Successfully munmap VM page to kernel!\n");
    #endif
}


/**
 * union two free blocks
 */ 
static void mm_union_free_blocks(meta_blk_t* first, meta_blk_t* second){

    assert(first->is_free == MM_TRUE && second->is_free == MM_TRUE);

    first->data_blk_size += META_SIZE + second->data_blk_size;
    first->next_blk = second->next_blk;
    if(first->next_blk){

        first->next_blk->pre_blk = first;
    }
}


/**
 * return the max size of VM Page
 */ 
static inline uint32_t mm_max_page_allocatable_memory(int units){

    if(units == 0 || SYSTEM_PAGE_SIZE == 0){

        #if MM_DEBUG
            printf("System Page is 0!\n");
        #endif

        return 0;
    }

    return (uint32_t)((SYSTEM_PAGE_SIZE * units) - offset_of(vm_page_t, page_data_blk));
}


/**
 * 
 * Return -1: meta_blk_data1 size is greater than meta_blk_data2
 * Return 1: meta_blk_data2 size is greater than meta_blk_data1
 * Return 0: meta_blk_data1 size is equal to meta_blk_data2
 * 
 */ 
static int free_blocks_comparison_function(void* meta_blk_data1, void* meta_blk_data2){

    assert(meta_blk_data1 && meta_blk_data2);

    if(((meta_blk_t*)meta_blk_data1)->data_blk_size > ((meta_blk_t*)meta_blk_data2)->data_blk_size){

        return -1;
    }else if(((meta_blk_t*)meta_blk_data1)->data_blk_size < ((meta_blk_t*)meta_blk_data2)->data_blk_size){

        return 1;
    }

    return 0;
}


/**
 * Add a given free meta block of Free data block to a Priority Queue of a given Page family 
 */ 
static void mm_add_free_meta_block_to_free_block_list(vm_page_family_t* vm_page_family, meta_blk_t* free_blk){

    assert(vm_page_family && free_blk);
    assert(free_blk->is_free == MM_TRUE);

    meta_blk_t* meta_blk_ptr = NULL;
    ITERATE_VM_PAGE_ALL_BLOCKS_BEGIN(vm_page_family->first_page, meta_blk_ptr)
        if(free_blocks_comparison_function(meta_blk_ptr, free_blk) <= 0){

            glthread_add(&meta_blk_ptr->priority_thread_glue, &free_blk->priority_thread_glue);
            break;
        }
    ITERATE_VM_PAGE_ALL_BLOCKS_END
}


/**
 * return the first node of the Priority Queue
 */ 
static inline meta_blk_t* mm_get_smallest_free_block_page_family(vm_page_family_t* vm_page_family){

    if(vm_page_family == NULL){

        return NULL;
    }

    glthread_node_t* smallest_free_block_glue = vm_page_family->free_blks_pq.right;
    if(smallest_free_block_glue){

        return glthread_to_meta_block(smallest_free_block_glue);
    }

    return NULL;
}

static void insert_glthread_node_to_pq(vm_page_family_t* page_family, meta_blk_t* new_free_blk, uint32_t size){

    if(new_free_blk == NULL){

        return;
    }

    meta_blk_t* ptr = NULL;
    PQ_ITERATE_BEGIN(&page_family->free_blks_pq, meta_blk_t, priority_thread_glue, ptr)
        if(ptr->data_blk_size > size){

            glthread_add_pre(&ptr->priority_thread_glue, &new_free_blk->priority_thread_glue);
            break;
        }
    PQ_ITERATE_END
}


/* 
 * mark meta block as being Allocated for 'size' bytes of application data
 * return MM_TRUE if allocation successed
 * return MM_FALSE if the allocation failed 
 */ 
static vm_bool_t mm_split_free_data_block_for_allocation(vm_page_family_t* page_family, meta_blk_t* meta_blk, uint32_t size){

    assert(page_family);
    assert(meta_blk->is_free == MM_TRUE);

    if(page_family->free_blks_pq.right == NULL){

        return MM_FALSE;
    }

    meta_blk_t* ptr = NULL;
    PQ_ITERATE_BEGIN(&page_family->free_blks_pq, meta_blk_t, priority_thread_glue, ptr)
        if(ptr->data_blk_size == size){

            meta_blk = ptr;
            return MM_TRUE;
        }else if(ptr->data_blk_size > size + META_SIZE){

            uint32_t original_size = ptr->data_blk_size;

            meta_blk = ptr;
            ptr->data_blk_size = size;
            ptr->is_free = MM_TRUE;

            meta_blk_t* next_meta_blk = NEXT_META_BLOCK_BY_SIZE(meta_blk);
            next_meta_blk->data_blk_size = original_size - size - META_SIZE;
            next_meta_blk->is_free = MM_TRUE;

            MM_BLIND_BLKS_FOR_ALLOCATION(meta_blk, next_meta_blk);
            glthread_remove(&meta_blk->priority_thread_glue);
            insert_glthread_node_to_pq(page_family, next_meta_blk, next_meta_blk->data_blk_size);

            return MM_TRUE;
        }
    PQ_ITERATE_END

    return MM_FALSE;
}


/**
 * return meta block of free data block 
 */ 
static meta_blk_t* mm_allocate_free_data_block(vm_page_family_t* page_family, uint32_t data_blk_size){

    assert(page_family);

    if(data_blk_size == 0){

        return NULL;
    }

    meta_blk_t* free_meta_blk = NULL;
    vm_bool_t res = MM_FALSE;
    vm_page_t* vm_page = page_family->first_page;

    if(vm_page == NULL){

        allocate_vm_page(page_family);
    }

    while(vm_page){

        res = mm_split_free_data_block_for_allocation(page_family, free_meta_blk, data_blk_size);
        if(res){

            break;
        }else if(!res && vm_page){

            vm_page = vm_page->next_page;
        }else{

            allocate_vm_page(page_family);
            vm_page = page_family->first_page;
        }
    }

    return GET_DATA_BLK(free_meta_blk);
}

/**
 * instantiate structure info and store it into VM Page 
 */ 
void mm_instantiate_new_page_family(char* struct_name, uint32_t struct_size){

    if(struct_size > SYSTEM_PAGE_SIZE){

        #if MM_DEBUG
            printf("%s() can not instantiate size that exceeds %ld bytes!\n", __FUNCTION__, SYSTEM_PAGE_SIZE);
        #endif
        return;
    }

    vm_page_family_t* current_family = NULL;
    vm_page_family_list_t* new_vm_page_for_family = NULL;

    if(first_vm_page_for_family == NULL){

        first_vm_page_for_family = (vm_page_family_list_t*)mm_get_vm_page(1);
        first_vm_page_for_family->next = NULL;

        strncpy(first_vm_page_for_family->vm_page[0].struct_name, struct_name, MAX_NAME_LEN);
        first_vm_page_for_family->vm_page[0].struct_size = struct_size;
        first_vm_page_for_family->vm_page[0].first_page = NULL;
        glthread_init(&first_vm_page_for_family->vm_page[0].free_blks_pq);

        return;
    }

    uint32_t count = 0;

    ITERATE_PAGE_FAMILIES_BEGIN(first_vm_page_for_family, current_family)

        if(strncmp(current_family->struct_name, struct_name, MAX_NAME_LEN) == 0){

            #if MM_DEBUG
                printf("same structure %s!\n", struct_name);
            #endif

            assert(0);
        }
        ++count;
    ITERATE_PAGE_FAMILIES_END

    if(count == MAX_FAMILY_PER_PAGE){

        new_vm_page_for_family = (vm_page_family_list_t*)mm_get_vm_page(1);
        new_vm_page_for_family->next = first_vm_page_for_family;
        first_vm_page_for_family = new_vm_page_for_family;
        count = 0;    
    }

    strncpy(first_vm_page_for_family->vm_page[count].struct_name, struct_name, MAX_NAME_LEN);
    first_vm_page_for_family->vm_page[count].struct_size = struct_size;
    first_vm_page_for_family->vm_page[count].first_page = NULL;
    glthread_init(&first_vm_page_for_family->vm_page[count].free_blks_pq);
}


/**
 * iterate and print out structure messages
 */ 
void mm_print_registered_page_families(){

    if(first_vm_page_for_family == NULL){

        #if MM_DEBUG
            printf("first_vm_page_for_family should be instantiated first!\n");
        #endif
        return;
    }

    vm_page_family_list_t* list_ptr = first_vm_page_for_family;
    vm_page_family_t* current_family = NULL;
    uint32_t vm_number = 1;

    while(list_ptr){

        printf("VM%d: \n", vm_number);
        ITERATE_PAGE_FAMILIES_BEGIN(list_ptr, current_family)

            printf("Struct Name: %s, Struct Size: %d\n", current_family->struct_name, current_family->struct_size);
        ITERATE_PAGE_FAMILIES_END

        list_ptr = list_ptr->next;
        ++vm_number;

        printf("\r\n");
    }

}


/**
 * find particular struct_name within vm_page_family_list_t
 */ 
vm_page_family_t* lookup_page_family_by_name(char *struct_name){

    if(first_vm_page_for_family == NULL){
        
        #if MM_DEBUG
            printf("first_vm_page_for_family should be instantiated first!\n");
        #endif
        return NULL;
    }

    vm_page_family_list_t* list_ptr = first_vm_page_for_family;
    vm_page_family_t* current_family = NULL;

    while(list_ptr){

        ITERATE_PAGE_FAMILIES_BEGIN(list_ptr, current_family)
            
            if(strncmp(current_family->struct_name, struct_name, MAX_NAME_LEN) == 0){
                return current_family;
            }
        ITERATE_PAGE_FAMILIES_END

        list_ptr = list_ptr->next;
    }

    return NULL;
}


/**
 * check the vm_page is empty or not
 */ 
vm_bool_t mm_vm_page_is_empty(vm_page_t* vm_page){

    assert(vm_page);

    vm_bool_t ret = vm_page->meta_blk.is_free == MM_TRUE && 
                    vm_page->meta_blk.next_blk == NULL && 
                    vm_page->meta_blk.pre_blk == NULL
                    ? MM_TRUE : MM_FALSE;

    return ret;
}


/**
 * VM Page Insertion
 */ 
vm_page_t* allocate_vm_page(vm_page_family_t* vm_page_family){

    if(vm_page_family == NULL){

        #if MM_DEBUG
            printf("Insert VM Page is NULL!\n");
        #endif
        return NULL;
    }

    vm_page_t* new_page = mm_get_vm_page(1);

    /* set the back pointer to page family */
    new_page->page_family = vm_page_family;

    /* meta block init */
    MARK_VM_PAGE_EMPTY(new_page);
    glthread_init(&new_page->meta_blk.priority_thread_glue);
    new_page->meta_blk.data_blk_size = mm_max_page_allocatable_memory(1);
    new_page->meta_blk.offset = offset_of(vm_page_t, page_data_blk);
    new_page->pre_page = NULL;
    new_page->next_page = NULL;

    /* mantain vm_page_t dll */
    if(vm_page_family->first_page == NULL){

        vm_page_family->first_page = new_page;
        return new_page;
    }

    new_page->next_page = vm_page_family->first_page;
    vm_page_family->first_page->pre_page = new_page;

    return new_page;
}


/**
 * VM Page Deletion
 */ 
void mm_page_delete_and_free(vm_page_t* vm_page){

    if(vm_page == NULL){

        #if MM_DEBUG
            printf("Delete VM Page is NULL!\n");
        #endif
        return;
    }

    /* vm_page is the first page */
    if(vm_page->page_family->first_page == vm_page){

        vm_page->page_family->first_page = vm_page->next_page;
        if(vm_page->next_page){

            vm_page->pre_page = NULL;
        }
        vm_page->pre_page = NULL;
        vm_page->next_page = NULL;
        mm_release_vm_page(vm_page, 1);
        return;
    }

    /* vm_page is at the middle of the dll */
    vm_page->pre_page->next_page = vm_page->next_page;
    if(vm_page->next_page){

        vm_page->next_page->pre_page = vm_page->pre_page;
    }
    vm_page->pre_page = NULL;
    vm_page->next_page = NULL;
    mm_release_vm_page(vm_page, 1);
}


/**
 * test function 
 */
void mm_debug_fn(){
    
    mm_init();
    printf("VM Page Size: %lu\n", SYSTEM_PAGE_SIZE);

    void* addr1 = mm_get_vm_page(1);
    void* addr2 = mm_get_vm_page(1);
    printf("VM1: %p, VM2: %p\n", addr1, addr2);

    mm_release_vm_page(addr1, 1);
    mm_release_vm_page(addr2, 1);
}


/**
 * dynamic memory allocation fnuc for applications
 */ 
void* zalloc(char* struct_name, int units){

    if(struct_name == NULL || units == 0){

        return NULL;
    }

    vm_page_family_t* page_family = NULL;

    if((page_family = lookup_page_family_by_name(struct_name)) == NULL){

        #if MM_DEBUG
            printf("structure %s can't be found!\n", struct_name);
        #endif

        return NULL;
    }

    uint32_t total_struct_size = page_family->struct_size * units;
    meta_blk_t* free_blk = NULL;

    if(total_struct_size > mm_max_page_allocatable_memory(1)){

        #if MM_DEBUG
            printf("Memory requested exeeds page size!\n");
        #endif

        return NULL; // can add extra operation to this senario
    }

    free_blk = mm_allocate_free_data_block(page_family, total_struct_size);

    if(free_blk){

        memset(free_blk + 1, 0x0, free_blk->data_blk_size);
        return free_blk + 1;
    }

    return NULL;
}
