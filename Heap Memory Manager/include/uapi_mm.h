#ifndef __UAPI_MM_H_
#define __UAPI_MM_H_

#include "mm.h"

#define STRING_CONVERTER(str) #str
#define MM_REG_STRUCT(struct_name) mm_instantiate_new_page_family(#struct_name, sizeof(struct_name))

#endif /* __UAPI_MM_H_ */