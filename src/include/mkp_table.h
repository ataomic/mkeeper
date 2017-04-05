#ifndef MKP_TABLE_H
#define MKP_TABLE_H

#include "mkp_hash.h"

typedef void* (*mkp_alloc)(size_t size);
typedef void (*mkp_free)(void* p);

typedef struct mkp_table_s {
	mkp_pool pool;
	mkp_hash hash;
	int err;
	u32 ctor_count, dtor_count,
		err_count;
	mkp_alloc alloc;
	mkp_free free;
} mkp_table;

#define mkp_table_init(table) \
	memset(table, 0, sizeof(*(table)))

#define mkp_bucket(table, p) \
	((table)->buckets+(((unsigned long)p)&UINT_MAX))

#define MKP_OBJ_MEM_SIZE(n) (sizeof(mkp_obj)*(n))
#define MKP_BKT_MEM_SIZE(n) (sizeof(mkp_list)*(n))

#define MKP_MEM_SIZE(objnum, bktnum) \
	(MKP_OBJ_MEM_SIZE(objnum)+MKP_BKT_MEM_SIZE(bktnum))

#define mkp_table_init_mem(table, osz, bsz) do { \
	u8* _mkp_p = (u8*)table->alloc(MKP_MEM_SIZE(osz, bsz)); \
	if(!_mkp_p) \
		break; \
	memset(_mkp_p, 0, MKP_MEM_SIZE(osz, bsz)); \
	mkp_pool_init(&(table)->pool, (mkp_obj*)_mkp_p, osz); \
	_mkp += MKP_OBJ_MEM_SIZE(osz); \
	mkp_hash_init(&(table)->hash, (mkp_list*)_mkp_p, bsz); \
}while(0)

#define mkp_table_fini_mem(table) do { \
} while(0)

typedef int (*mkp_obj_op)(mkp_table*, mkp_obj*);

static __inline void mkp_table_check(mkp_table *table, u32 index,
	mkp_obj *obj)
{
	return;
}

#define mkp_table_alloc_obj(table) mkp_pool_alloc_obj(&(table)->pool)

static __inline void mkp_table_free_obj(mkp_table *table,
										 mkp_obj *obj)
{
	u32 index = mkp_obj_index(table, obj);
	mkp_obj_init(obj);
	mkp_list_insert(&table->pool.free, index, obj);
}

static __inline void mkp_table_lazyfree_obj(mkp_table *table,
											 mkp_obj *obj)
{
	u32 index = mkp_obj_index(table, obj);
	struct mkp_pool *pool = &table->pool;

	obj->flags = MKP_F_LZY;
	obj->opt.flags = 0;
	obj->opt.next = pool->free.next;
	pool->free.next = index;
}

static __inline void mkp_table_rec(mkp_table *table,
								   mkp_obj *obj, u8 error)
{
	u32 index = mkp_obj_index(table, obj);

	mkp_obj_rec(obj, MKP_OP_ERR, error);
	mkp_list_insert(&table->err, index, obj);
	table->err_count ++;
}

static __inline mkp_obj *mkp_table_add(
	mkp_table *table,
	void *p, size_t sz,
	u8 flags)
{
	u8 err = MKP_SUCCESS;
	u32 index;
	mkp_obj *old, *obj = NULL, *pre;
	struct mkp_list *bucket;

	table->ctor_count ++;

	if (!table->buckets) {
		return NULL;
	}

	bucket = mkp_bucket(table, p);
	old = mkp_table_find_obj(table, bucket, p, &pre);

	if (old) {
		if (!old->flags) {
			err = MKP_ERR_CTOR;
		} else if (mkp_is_lzy(old)) {
			mkp_list_erase(bucket, pre, old);
			old = NULL;
		}
	}

	if (!old || err) {
		obj = mkp_table_alloc_obj(table);

		if (!obj) {
			return NULL;
		}

		obj->addr = p;
		obj->size = sz;
		obj->flags = flags;
	}

	if (err) {
		if (old) {
			obj->opt.next = mkp_obj_index(table, old);
		}

		mkp_table_rec(table, obj, err);
	} else if (old) { /* maybe monitor */
		obj = old;
		obj->size = sz;
		obj->flags = flags;
		mkp_obj_rec(obj, MKP_OP_NORMAL, MKP_SUCCESS);
	} else {
		index = mkp_obj_index(table, obj);
		mkp_obj_rec(obj, MKP_OP_NORMAL, MKP_SUCCESS);
		mkp_list_insert(bucket, index, obj);
		table->count ++;
	}

	return obj;
}

static __inline mkp_obj *mkp_table_del(
	mkp_table *table,
	void *p, size_t sz)
{
	u8 err = MKP_SUCCESS;
	mkp_obj *old, *obj = NULL, *pre;
	struct mkp_list *bucket;

	table->dtor_count ++;

	if (!table->buckets) {
		return NULL;
	}

	bucket = mkp_bucket(table, p);
	old = mkp_table_find_obj(table, bucket, p, &pre);

	if (!old) {
		err = MKP_ERR_MISS;
	} else if (mkp_is_lzy(old)) {
		err = MKP_ERR_DTOR;
	} else if (sz && old->size != sz) {
		err = MKP_ERR_SIZE;
	}

	if (err) {
		obj = mkp_table_alloc_obj(table);

		if (!obj) {
			return NULL;
		}

		obj->addr = p;
		obj->size = sz;

		if (old) {
			obj->opt.next = mkp_obj_index(table, old);
		}

		mkp_table_rec(table, obj, err);
	} else if (!old->flags) {
		mkp_table_lazyfree_obj(table, old);
		table->count --;
	}

	return obj;
}

static __inline int mkp_table_visit_error(mkp_table *table,
										  mkp_obj_proc visit)
{
	u32 i = table->err.next;
	int ret = 0;
	mkp_obj *obj;

	while (i) {
		obj = mkp_obj_addr(table, i);

		if (!obj) {
			break;
		}

		i = obj->next;
		ret = visit(table, obj);

		if (ret != 0) {
			break;
		}
	}

	return ret;
}

static __inline int mkp_table_visit(mkp_table *table,
									mkp_obj_proc visit)
{
	u32 count = 0;
	int i, ret;
	mkp_obj *obj;

	for (i = 0; i < table->size; i ++) {
		obj = table->pool.addr + i;

		if (!obj->addr) {
			continue;
		}

		ret = visit(table, obj);

		if (ret != 0) {
			return ret;
		}

		count ++;

		if (count == table->count) {
			break;
		}
	}

	return 0;
}


#endif

