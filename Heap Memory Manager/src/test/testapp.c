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


void testapp_demo(){

    mm_init();

    MM_REG_STRUCT(emp_t);
    MM_REG_STRUCT(student_t);
    MM_REG_STRUCT(teacher_t);

    // mm_print_registered_page_families();

#if 0
    ZMALLOC(emp_t, 1);
    ZMALLOC(emp_t, 1);
    ZMALLOC(emp_t, 1);
    ZMALLOC(emp_t, 1);
    ZMALLOC(emp_t, 1);
    ZMALLOC(emp_t, 1);

#endif

#if 1
    for(int i=0; i<500; i++){

        ZMALLOC(emp_t, 1);
    }
#endif

    mm_print_memory_usage();
    
}
