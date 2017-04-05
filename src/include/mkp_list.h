#ifndef MKP_LIST_H
#define MKP_LIST_H

#include "mkp_def.h"

typedef struct mkp_list_s {
	u32 next: MKP_BITS;
	u32 flags: 32-MKP_BITS;
} mkp_list;

#define mkp_list_erase(list, pre, obj) do { \
	if(pre) (pre)->next = (obj)->next; \
	else (list)->next = (obj)->next; \
} while(0)

#define mkp_list_insert(list, objidx, obj) do { \
	(obj)->next = (list)->next; \
	(list)->next = (objidx); \
} while(0)

#endif

