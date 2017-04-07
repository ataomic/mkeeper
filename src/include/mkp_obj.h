#ifndef MKP_OBJ_H
#define MKP_OBJ_H

#include <limits.h>
#include <execinfo.h>

#include "mkp_list.h"

#define MKP_OBJ \
	u32 next:MKP_BITS; \
	u32 type:2; \
	u32 flags:30-MKP_BITS

typedef struct mkp_bktrace_s {
	MKP_OBJ;
	u32 depth:8;
	u32 bk_type:4;
	u32 unused:20;
	void* buf[MKP_STACK_DEPTH];
} mkp_bktrace;

typedef struct mkp_obj_s {
	MKP_OBJ;
	u32 bktrace:MKP_BITS;
	u32 err:32-MKP_BITS;
	u32 size;
	void *addr;
} mkp_obj;

#define mkp_obj_init(obj) \
	memset(obj, 0, sizeof(*(obj)))

typedef int (*mkp_obj_op)(mkp_obj*, void*);

#define mkp_obj_backtrace(obj, type) \
	((obj)->depth[type] = backtrace((obj)->op[type], MKP_STACK_DEPTH))

static __inline void mkp_obj_set_err(mkp_obj* obj, u8 err)
{
	obj->err = err;
}

#define mkp_obj_rec(obj, err, bk) do { \
	mkp_obj_set_err(obj, err); \
	mkp_obj_set_backtrace(obj, bk); \
}while(0)

#endif

