#ifndef __GLTHREAD_H_
#define __GLTHREAD_H_

#include <stdio.h>

#define offsetof(struct_name, field_name) (unsigned long)&((struct_name*)0)->field_name

typedef struct emp_{
    char name[30];
    unsigned int salary;
    char designation[30];
    unsigned int emp_id; 
}emp_t;

typedef struct _glthread{

    struct _glthread *left;
    struct _glthread *right;
} glthread_t;

#endif /* __GLTHREAD_H_ */