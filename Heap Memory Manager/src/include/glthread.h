#ifndef __GLTHREAD_H_
#define __GLTHREAD_H_

#include <stdio.h>

#define offsetof(struct_name, field_name) (unsigned long)&((struct_name*)0)->field_name

#define ITERATE_GL_THREADS_BEGIN(lstptr, struct_type, ptr)      \
{                                                               \
    glthread_node_t *_glnode = NULL, *_next = NULL;             \
    for(_glnode = lstptr->head; _glnode; _glnode = _next){      \
        _next = _glnode->right;                                 \
        ptr = (struct_type *)((char *)_glnode - lstptr->offset);

#define ITERATE_GL_THREADS_ENDS }}

typedef struct _glthread{
    struct _glthread *left;
    struct _glthread *right;
} glthread_t;

#endif /* __GLTHREAD_H_ */