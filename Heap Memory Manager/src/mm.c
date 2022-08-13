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
 * instantiate structure info and store it into VM Page 
 */ 
void mm_instantiate_new_page_family(char* struct_name, uint32_t struct_size){

    if(struct_size > SYSTEM_PAGE_SIZE){
        printf("%s() can not instantiate size that exceeds %ld bytes!\n", __FUNCTION__, SYSTEM_PAGE_SIZE);
        return;
    }

    vm_page_family_t* current_family = NULL;
    vm_page_family_list_t* new_vm_page_for_family = NULL;

    if(first_vm_page_for_family == NULL){

        first_vm_page_for_family = (vm_page_family_list_t*)mm_get_vm_page(1);
        first_vm_page_for_family->next = NULL;
    }

    uint32_t count = 0;

    ITERATE_PAGE_FAMILIES_BEGIN(first_vm_page_for_family, current_family)

        if(strncmp(current_family->struct_name, struct_name, MAX_NAME_LEN) == 0){

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
}


/**
 * iterate and print out structure messages
 */ 
void mm_print_registered_page_families(){

    if(first_vm_page_for_family == NULL){

        printf("first_vm_page_for_family should be instantiated first!\n");
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

        printf("first_vm_page_for_family should be instantiated first!\n");
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
