#include "glthread.h"

/**
 * 初始化glthread list
 */
void glthread_init(glthread_t** list, uint32_t offset){

    *list = (glthread_t*)calloc(1, sizeof(glthread_t));
    (*list)->head = NULL;
    (*list)->offset = offset;
}


/**
 * 添加新節點到指定節點右側
 */
void glthread_add(glthread_node_t* current, glthread_node_t* new){

    if(current == NULL || new == NULL){
        return;
    }

    new->right = current->right;
    new->left = current;

    if(new->right != NULL){
        new->right->left = new;
    }

    current->right = new;
}


/**
 * 添加節點到首節點 
 */
void glthread_add_first(glthread_t* list, glthread_node_t* glnode){

    if(glnode == NULL || list == NULL){
        return;
    }

    glnode->right = list->head;
    glnode->left = NULL;
    
    if(glnode->right != NULL){
        glnode->right->left = glnode;
    }

    list->head = glnode; 
}


/**
 * 刪除指定節點 
 */
glthread_node_t* glthread_remove(glthread_node_t* current, glthread_node_t* glnode){

    if(current == NULL || glnode == NULL){
        return NULL;
    }

    if(current == glnode){
        glthread_node_t* next = glnode->right;
        glthread_node_t* pre = glnode->left;

        /* 處理右節點 */
        glnode->right = NULL;
        if(next){
            next->left = pre;
        }

        /* 處理左節點 */
        glnode->left = NULL;
        if(pre){
            pre->right = next;
        }

        return pre != NULL ? pre->right : next;
    }

    current->right = glthread_remove(current->right, glnode); // 返回右節點

    return current;
}

