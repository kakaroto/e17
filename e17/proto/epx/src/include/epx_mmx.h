#ifndef _EPX_MMX_H
#define _EPX_MMX_H

#ifdef _EPX_MMX_
typedef union _epx_mmx_t {
	long long 		q;	/* Quadword (64-bit) */
	unsigned long long 	uq;	/* Unsigned Quadword */
	int 			d[2];	/* 2 Doubleword (32-bit) */
	unsigned int 		ud[2];	/* 2 Unsigned Doubleword */
	short 			w[4];	/* 4 Word (16-bit) */
	unsigned short 		uw[4];	/* 4 Unsigned Word */
	char 			b[8];	/* 8 Byte (8-bit) */
	unsigned char 		ub[8];	/* 8 Unsigned Byte */
#ifdef _EPX_3DNOW_
	float 			s[2];	/* Single-precision (32-bit) 3DNow! */
#endif
} __attribute__ ((aligned (8))) epx_mmx_t;
#endif

#endif
