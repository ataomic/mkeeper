#ifndef MKP_OBJ_H
#define MKP_OBJ_H

#include <limits.h>
#include <execinfo.h>

#include "mkp_list.h"

typedef struct mkp_obj_s {
	u32 ret:4;
	u32 flags:27-MKP_BITS;
	u32 next: MKP_BITS;
	u32 size;
	struct mkp_list opt;
	u16 depth[MKP_OP_T_MAX];
	void *op[MKP_OP_T_MAX][MKP_STACK_DEPTH];
	void *addr;
} mkp_obj;

#define mkp_obj_init(obj) \
	memset(obj, 0, sizeof(*(obj)))

#define mkp_obj_backtrace(obj, type) \
	((obj)->depth[type] = backtrace((obj)->op[type], MKP_STACK_DEPTH))

static __inline void mkp_obj_set_err(mkp_obj* obj, u8 err)
{
	obj->ret = err;
}

#define mkp_obj_rec(obj, type, err) do { \
	mkp_obj_set_err(obj, err); \
	mkp_obj_backtrace(obj, type); \
}while(0)

#endif

