#include "uapi_mm.h"

#define TEST1       1
#define TEST2       2
#define TEST_TYPE   TEST2   
#define LOOP        100

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

/**
 * @brief function for simply testing allocate and deallocate within singal VM Page Size
 * 
 */
void test1(){

    int wait;

    printf(ANSI_COLOR_YELLOW "Phase 1: \n\n" ANSI_COLOR_RESET);

    emp_t* emp1 = ZMALLOC(emp_t, 1);
    emp_t* emp2 = ZMALLOC(emp_t, 1);
    emp_t* emp3 = ZMALLOC(emp_t, 1);

    student_t* stu1 = ZMALLOC(student_t, 1);
    student_t* stu2 = ZMALLOC(student_t, 1);

    mm_print_memory_usage();

    printf("Please input random word to continue... ");
    scanf("%d", &wait);

    getchar();

    printf(ANSI_COLOR_YELLOW "Phase 2: \n\n" ANSI_COLOR_RESET);

    ZFREE(emp1);
    ZFREE(emp3);

    ZFREE(stu2);

    mm_print_memory_usage();

    printf("Please input random word to continue... ");
    scanf("%d", &wait);

    printf(ANSI_COLOR_YELLOW "Phase 3: \n\n" ANSI_COLOR_RESET);

    ZFREE(emp2);
    ZFREE(stu1);   

    mm_print_memory_usage();
}

/**
 * @brief function for testing allocate and deallocate `LOOP` times
 * 
 */
void test2(){

    int wait;

    emp_t* emp[LOOP];

    printf("Please input random word to begin looping test (%d times allocation)... ", LOOP);
    scanf("%d", &wait);
    getchar();

    for(int i=0; i<LOOP; i++){

        emp[i] = ZMALLOC(emp_t, 1);
    }

    mm_print_memory_usage();

    printf("Please input random word to begin looping test (%d times deallocation)... ", LOOP);
    scanf("%d", &wait);
    getchar();    

    for(int i=0; i<LOOP; i++){

        ZFREE(emp[i]);
    }

    mm_print_memory_usage();
}

void testapp_demo(){

    /* initialize VM Page Size */
    mm_init();

    /* add structures */
    MM_REG_STRUCT(emp_t);
    MM_REG_STRUCT(student_t);
    MM_REG_STRUCT(teacher_t);

    /* ZMALLOC functionality test */
#if (TEST_TYPE == TEST1)
    test1();
#elif (TEST_TYPE == TEST2)
    test2();
#endif
    
}
