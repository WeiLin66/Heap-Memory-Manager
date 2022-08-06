#include "malloc_version_1.h"
#include "glthread.h"

typedef struct _test{
	int data;
	glthread_node_t glnode;
}test;

int main(){
	glthread_t* list = NULL;
	uint32_t offset = offsetof(test, glnode);
	glthread_init(&list, offset);

	test node[3];

	node[0].data = 1;
	glthread_node_init((&(node[0].glnode)));

	node[1].data = 2;
	glthread_node_init((&(node[1].glnode)));

	node[2].data = 3;
	glthread_node_init((&(node[2].glnode)));

	for(int i=0; i<3; i++){
		if(i == 0){
			glthread_add_first(list, &(node[i].glnode));
		}else{
			glthread_add(&(node[i-1].glnode), &(node[i].glnode));
		}
	}

	list->head = glthread_remove(&(list->head), &(node[1].glnode));

	test* ptr;
	GLTHREAD_ITERATE_BEGIN(list, test, ptr)
		printf("data: %d\n", ptr->data);
	GLTHREAD_ITERATE_END

	return 0;
}