#ifndef __UAPI_MM_H_
#define __UAPI_MM_H_

#include "mm.h"

#define STRING_CONVERTER(str) #str
#define MM_REG_STRUCT(struct_name) mm_instantiate_new_page_family(#struct_name, sizeof(struct_name))

void* zalloc(char* struct_name, int units);

#endif /* __UAPI_MM_H_ */