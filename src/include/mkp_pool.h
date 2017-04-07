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
	(pool)->obj_size = (objsz); \
	(pool)->pos = 0; \
	(pool)->size = (sz); \
	mkp_list_init(&(pool)->free); \
} while(0)

#define mkp_pool_obj_index(pool, obj) \
	((obj) && (obj) < (pool)->addr+(pool)->pos ? (obj)-(pool)->addr+1 : 0)

#define mkp_pool_obj_addr(pool, idx) \
	((idx) && (idx) < (pool)->pos ? (pool)->addr+(idx)-1 : NULL)

static __inline mkp_obj* mkp_pool_alloc_obj(mkp_pool* pool)
{
	u32 i;
	struct mkp_obj* obj = NULL;

	if(pool->pos < pool->size) {
		i = pool->pos;
		obj = pool->addr + i;
		pool->pos ++;
	}
	else {
		i = pool->free.next;
		obj = mkp_pool_obj_addr(pool, i);
		pool->free.next = obj->next;
	}

	return obj;
}

static __inline void mkp_pool_free_obj(mkp_pool* pool, mkp_obj* obj)
{
	u32 index = mkp_pool_obj_index(pool, obj);
	mkp_obj_init(obj);
	mkp_list_insert(&pool->free, index, obj);
}

static __inline int mkp_pool_visit_obj(mkp_pool* pool, mkp_obj_op visit,
	void* data)
{
	int ret;
	u32 i;
	mkp_obj* obj;

	for (i = 0; i < pool->pos; i ++) {
		obj = table->pool.addr+i;
		if(!obj->addr)
			continue;
		ret = visit(obj, data);
		if(ret != 0)
			return ret;
	}

	return 0;
}

static __inline int mkp_pool_visit_list(mkp_pool* pool, mkp_list* list,
	mkp_obj_op visit, void* data)
{
	int ret;
	mkp_obj* obj;
	u32 idx;
	
	idx = list->next;
	while(idx) { 
		obj = mkp_pool_obj_addr(pool, idx);
		if(!obj)
			break;
		idx = obj->next;
		ret = visit(obj, data);
		if(ret != 0)
			break;
	}

	return 0;
}

#endif

