#ifndef MKP_HASH_H
#define MKP_HASH_H

#include "mkp_pool.h"

typedef struct mkp_hash_s {
	mkp_pool* pool;
	mkp_list* buckets;
	u32 size, count;
} mkp_hash;

#define mkp_hash_init(hash, mpool, bkt, sz) do { \
	(hash)->pool = (mpool); \
	(hash)->buckets = (bkt); \
	(hash)->size = (bsz); \
	(hash)->count = 0; \
} while(0)

#define mkp_hash_obj_index(hash, obj) mkp_pool_obj_index((hash)->pool, obj)
#define mkp_hash_obj_addr(hash, idx) mkp_pool_obj_addr((hash)->pool, idx)

#define mkp_bucket(hash, p) \
	((hash)->buckets+(((unsigned long)p)%(hash)->size))

static __inline void mkp_hash_check_addr(mkp_table *table, u32 index,
	mkp_obj* obj)
{
	return;
}

static __inline mkp_obj* mkp_bucket_find_obj(mkp_hash* hash,
	mkp_list* bucket, const void* p, mkp_obj** pre)
{
	u32 i;
	mkp_obj *obj = NULL;

	*pre = NULL;
	i = bucket->next;

	while(i) {
		obj = mkp_hash_obj_addr(hash, i);
		if(!obj) {
			mkp_hash_check_addr(hash, i, obj);
			break;
		}
		
		if(obj->addr == p)
			return obj;

		*pre = obj;
		i = obj->next;
	}

	return obj;
}

static __inline mkp_obj* mkp_hash_find_obj(mkp_hash* hash, const void* p,
	mkp_obj** pre)
{
	return mkp_bucket_find_obj(hash, mkp_bucket(hash, p), p, pre);
}

static __inline mkp_obj* mkp_hash_insert_obj(mkp_hash* hash, mkp_obj* obj)
{
	mkp_list* bucket = mkp_bucket(hash, obj->addr);
	obj->next = bucket->next;
	bucket->next = mkp_hash_obj_index(hash, obj);
}

static __inline mkp_obj* mkp_hash_erase_obj(mkp_hash *hash, mkp_obj* old)
{
	u32 i;
	mkp_obj *obj, *pre = NULL;
	mkp_list* bucket;

	bucket = mkp_bucket(hash, old->addr);
	i = bucket->next;
	while(i) {
		obj = mkp_hash_obj_addr(hash, i);
		if(obj == old) {
			mkp_list_erase(bucket, pre, old);
			return old;
		}
		pre = obj;
		i = obj->next;
	}

	return NULL;
}

#endif

