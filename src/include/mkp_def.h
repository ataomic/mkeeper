#ifndef MKP_DEF_H
#define MKP_DEF_H

/* error code */
enum {
	MKP_SUCCESS = 0,

	MKP_ERR_CTOR,
	MKP_ERR_DTOR,
	MKP_ERR_MISS,
	MKP_ERR_SIZE,
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

enum {
	MKP_F_LZY = bits_of(0),
	MKP_F_MON = bits_of(1),
};

#define mkp_is_lzy(obj) ((obj)->flags&MKP_F_LZY)
#define mkp_is_mon(obj) ((obj)->flags&MKP_F_MON)

#endif

