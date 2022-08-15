#include "glthread.h"


/**
 * init glthread node
 */
void glthread_init(glthread_node_t* glthread){

    glthread->right = NULL;
    glthread->left = NULL;
}


/**
 * insert glthread node to the right of the current node
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
 * insert node to the head of the list 
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
 * delete specific node
 */
void glthread_remove(glthread_node_t* glnode){

    if(glnode == NULL){

        return;
    }

    if(glnode->left){

        glnode->left->right = glnode->right;
    }

    if(glnode->right){

        glnode->right->left = glnode->right;
    }

    glnode->left = NULL;
    glnode->right = NULL;

}

