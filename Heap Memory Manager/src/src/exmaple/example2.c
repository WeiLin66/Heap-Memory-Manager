/**
 * 測試glthread的初始化與增刪節點功能
 */

#include "glthread.h"

typedef struct _test{
    
    int data;
    glthread_node_t glnode;
}test;

void example_2(){
    
    glthread_t* list = NULL;
    uint32_t offset = offsetof(test, glnode); // 計算結構體首地址
    glthread_init(&list, offset); // 資訊表list

    test node[3];

    node[0].data = 1;
    glthread_node_init((&(node[0].glnode))); // 初始化節點

    node[1].data = 2;
    glthread_node_init((&(node[1].glnode)));

    node[2].data = 3;
    glthread_node_init((&(node[2].glnode)));

    for(int i=0; i<3; i++){
        if(i == 0){
            glthread_add_first(list, &(node[i].glnode)); // 添加節點到頭部
        }else{
            glthread_add(&(node[i-1].glnode), &(node[i].glnode)); // 添加節點到指定位置
        }
    }

    list->head = glthread_remove(list->head, &(node[1].glnode)); // 刪除特定節點

    /* 打印所有節點 */
    test* ptr;
    GLTHREAD_ITERATE_BEGIN(list, test, ptr)
        printf("data: %d\n", ptr->data);
    GLTHREAD_ITERATE_END
}