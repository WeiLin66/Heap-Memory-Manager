#ifndef __GLTHREAD_H_
#define __GLTHREAD_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* offset caculate macro */
#define offsetof(struct_name, field_name) (uint64_t)&((struct_name*)0)->field_name

/* Iterative macro (dll) */
#define GLTHREAD_ITERATE_BEGIN(list_ptr, struct_type, ptr)          \
{                                                                   \
    glthread_node_t* _node = list_ptr->head;                        \
    glthread_node_t* _node_next = NULL;                             \
    for(; _node; _node = _node_next){                               \
        _node_next = _node->right;                                  \
        ptr = (struct_type *)((char *)_node - list_ptr->offset);


#define GLTHREAD_ITERATE_END }}


#define ITERATE_LIST_BEGIN(list_ptr, _node)           \
{                                                     \
    _node = list_ptr->head;                           \
    glthread_node_t* _node_next = NULL;               \
    for(;_node != NULL; _node = _node_next){          \
        _node_next = (glthread_node_t*)_node->right;  \

#define ITERATE_LIST_END }}


/* glthread_node_t init macro */
#define glthread_node_init(glnode)                                  \
        glnode->left = NULL;                                        \
        glnode->right = NULL;                                       \

typedef struct glthread_node_{
    struct glthread_node_ *left;
    struct glthread_node_ *right;
} glthread_node_t;

typedef struct glthread_{
    glthread_node_t* head;
    uint32_t offset;
} glthread_t;

void glthread_init(glthread_t** list, uint32_t offset);
void glthread_add(glthread_node_t* current, glthread_node_t* new);
void glthread_add_first(glthread_t* list, glthread_node_t* glnode);
glthread_node_t* glthread_remove(glthread_node_t** current, glthread_node_t* glnode);

#endif /* __GLTHREAD_H_ */