#ifndef MKP_DEF_H
#define MKP_DEF_H

/* obj type */
enum {
	MKP_T_OBJ = 0,
	MKP_T_BKTRyACE,
};

/* backtrace type */
enum {
	MKP_BK_CTOR = 0,
	MKP_BK_DTOR,
};

/* error code */
enum {
	MKP_SUCCESS = 0,

	MKP_ERR_CTOR,
	MKP_ERR_DTOR,
	MKP_ERR_MISS,
	MKP_ERR_SIZE,
	MKP_ERR_T_MAX,
};

/* counters */
enum {
	MKP_C_ERR,
	MKP_C_CTOR,
	MKP_C_DTOR,
	MKP_C_MISS,
	MKP_C_SIZE,
	MKP_C_MAX,
};

/* operators */
enum {
	MKP_OP_NORMAL = 0,
	MKP_OP_ERR,
	MKP_OP_T_MAX,
};

#ifndef MKP_STACK_DEPTH
#define MKP_STACK_DEPTH 16
#endif

#ifndef MKP_BITS
#define MKP_BITS 20
#endif

#ifndef bits_of
#define bits_of(x) (1ul<<(x))
#endif

/* to catch double free
 *   set monitor flag after allocated
 *   unset monitor flag in valid free
 */

/* to catch use after free
 *   set save flag when free with a id
 *   call mkp_table_check with id before use
 */
enum {
	MKP_F_MON = bits_of(0),
	MKP_F_SAV = bits_of(1),
};

#define mkp_is_mon(obj) ((obj)->flags & MKP_F_MON)
#define mkp_is_sav(obj) ((obj)->flags & MKP_F_SAV)

#endif

