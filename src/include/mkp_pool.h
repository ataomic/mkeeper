#ifndef MKP_POOL_H
#define MKP_POOL_H

#include "mkp_obj.h"

typedef struct mkp_pool_s {
	mkp_obj* addr;
	mkp_list free;
	u32 pos, size;
} mkp_pool;

#define mkp_pool_init(pool, p, sz) do { \
	(pool)->addr = (p); \
	(pool)->pos = 0; \
	(pool)->size = (sz); \
	mkp_list_init(&(pool)->free); \
} while(0)

#define mkp_pool_obj_index(pool, obj) \
	((obj) && (obj) < (pool)->addr+(pool)->pos ? (obj)-(pool)->addr+1 : 0)

#define mkp_pool_obj_addr(pool, idx) \
	((idx) && (idx) < (pool)->pos ? (pool)->addr+(idx)-1 : NULL)

static __inline mkp_obj* mkp_pool_alloc_obj(struct mkp_pool* pool)
{
	u32 i;
	struct mkp_obj* obj;

	if(pool->pos < pool->size) {
		i = pool->pos;
		obj = pool->addr + i;
		pool->pos ++;
	}
	else {
		i = pool->free.next;
		obj = mkp_pool_obj_addr(pool, i);

		if (mkp_is_lzy(obj)) {
			mkp_table_erase_obj(table, obj);
			i = obj->opt.next;
			mkp_obj_init(obj);
		} else {
			i = obj->next;
		}

		pool->free.next = i;
	}

	return obj;
}

#endif

