#include "uapi_mm.h"

typedef struct _emp{

    char name[MAX_NAME_LEN];
    uint32_t emp_id;
}emp_t;


typedef struct _student{

    char name[MAX_NAME_LEN];
    uint32_t rollno;
    uint32_t marks_phys;
    uint32_t marks_chem;
    uint32_t marks_maths;
    struct _student* next;
}student_t;


typedef struct _teacher{

    char name[MAX_NAME_LEN];
    uint32_t id;
    uint32_t major;
    struct _teacher* next;
}teacher_t;


void struct_finder(char* struct_name){
    vm_page_family_t *ptr = lookup_page_family_by_name(struct_name);

    if(ptr){
        printf("Struct %s find!\n", struct_name);
    }else{
        printf("VM Page doesn't contain struct %s!\n", struct_name);
    }
}


void testapp_demo(){

    mm_init();

    MM_REG_STRUCT(emp_t);
    MM_REG_STRUCT(student_t);
    MM_REG_STRUCT(teacher_t);

    mm_print_registered_page_families();

    struct_finder(STRING_CONVERTER(emp_t));
    struct_finder(STRING_CONVERTER(student_t));
    struct_finder(STRING_CONVERTER(teacher_t));
    
}
