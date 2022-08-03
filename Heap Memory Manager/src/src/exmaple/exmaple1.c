/**
 * 使用print_emp_detail()遍歷結構體emp_test1_t中的所有變數
 * print_emp_detail傳入參數僅有指向結構體變數glnode的指標
 */

#include <string.h>
#include "glthread.h"

/* 自定義結構體 */
typedef struct emp_{
    char name[30];
    unsigned int salary;
    char designation[30];
    unsigned int emp_id;
    glthread_t glthread_node_t;
}emp_t;

/* 打印結構體資訊 */
void print_emp_detail(glthread_t* glnode){
    unsigned long offset = offsetof(emp_t, glthread_node_t);
    unsigned long head = (unsigned long)glnode - offset;
    emp_t* ptr = (emp_t*)head;

    printf("name: %s\n", ptr->name);
    printf("salary: %u\n", ptr->salary);
    printf("designation: %s\n", ptr->designation);
    printf("emp_id: %u\n", ptr->emp_id);
}

/* 測試程式 */
void example_1(){
    emp_t t;
    strcpy(t.name, "Zach");
    t.salary = 64000;
    strcpy(t.designation, "engineer");
    t.emp_id = 21025;

    emp_t* emp = &t;

    print_emp_detail(&(emp->glthread_node_t));
}