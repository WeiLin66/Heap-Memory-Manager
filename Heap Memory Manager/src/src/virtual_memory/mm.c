#include "mm.h"

static size_t SYSTEM_PAGE_SIZE = 0;

/**
 * 獲取VM Page Size
 */ 
void mm_init(){

    SYSTEM_PAGE_SIZE = getpagesize();
}


/**
 * 向kernel申請VM Page
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
        printf("Successfully mmap VM page from kernel!\n");
    #endif

    return (void*)vm_page;
}


/**
 * 釋放VM Page
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
 * 測試用function 
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
