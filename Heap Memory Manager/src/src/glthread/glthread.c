#include "glthread.h"

/**
 * 初始化glthread list
 */
void glthread_init(glthread_t** list, uint32_t offset){
    *list = (glthread_node_t*)calloc(1, sizeof(glthread_t));
    (*list)->head = (glthread_node_t*)calloc(1, sizeof(glthread_node_t)); // dummy head
    (*list)->offset = offset;
}

/**
 * 添加節點到首節點 
 */
void glthread_add_first(glthread_t* list, glthread_node_t* glnode){
    if(glnode == NULL || list == NULL || list->head == NULL){
        return;
    }

    glnode->right = list->head->right;
    list->head->right = glnode; 
}

/**
 * 從list中刪除節點 
 */
void glthread_remove(glthread_t* list, glthread_node_t* glnode){

}


/**
 * 釋放所有節點
 */ 
void glthread_free(){

}