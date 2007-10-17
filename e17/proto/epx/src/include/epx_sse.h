#ifndef _EPX_SSE_H
#define _EPX_SSE_H

#ifdef _EPX_SSE_
typedef union _epx_sse_t {
	long double 		qf;	/* Quad precision (128-bit) 	*/
	float 			s[4]; 	/* 4 Single-precision (32-bit) 	*/
} __attribute__ ((aligned (16))) epx_sse_t;

#define	sse_i2r(op, imm, reg) \
	__asm__ __volatile__ (#op " $" #imm ", %%" #reg \
			      : /* nothing */ \
			      : /* nothing */);

#define	sse_m2r(op, mem, reg) \
	__asm__ __volatile__ (#op " %0, %%" #reg \
			      : /* nothing */ \
			      : "m" (mem))

#define	sse_r2m(op, reg, mem) \
	__asm__ __volatile__ (#op " %%" #reg ", %0"	\
			      : "=m" (mem) \
			      : /* nothing */ )

#define	sse_a2r(op, mem, reg) \
	__asm__ __volatile__ (#op " %0, %%" #reg \
			      : /* nothing */ \
			      : "m" (mem))

#define	sse_r2a(op, reg, mem) \
	__asm__ __volatile__ (#op " %%" #reg ", %0" \
			      : "=m" (mem) \
			      : /* nothing */ )

#define	sse_r2r(op, regs, regd) \
	__asm__ __volatile__ (#op " %" #regs ", %" #regd)

/*============================================================================*
 *                             Data Transfer                                  * 
 *============================================================================*/
/* MOVSS: Move Scalar */
/* MOVAPS: Move Packed Aligned */
#define	movaps_m2r(var, reg)	sse_a2r(movaps, var, reg)
#define	movaps_r2m(reg, var)	sse_r2a(movaps, reg, var)
#define	movaps_r2r(regs, regd)	sse_r2r(movaps, regs, regd)
/* MOVUPS: Move Packed Unaligned */
#define	movups_m2r(var, reg)	sse_a2r(movups, var, reg)
#define	movups_r2m(reg, var)	sse_r2a(movups, reg, var)
#define	movups_r2r(regs, regd)	sse_r2r(movups, regs, regd)
/* MOVLPS: Move Low Packed */
/* MOVHPS: Move Low Packed */
/* MOVLHPS: Move Low High Packed */
/* MOVHLPS: Move High Low Packed */
/*============================================================================*
 *                               Arithmetic                                   * 
 *============================================================================*/
/* ADDPS: */
#define	addps_m2r(var, reg)	sse_m2r(addps, var, reg)
#define	addps_r2r(regs, regd)	sse_r2r(addps, regs, regd)
/* SUBPS: */
/* MULPS: */
#define	mulps_m2r(var, reg)	sse_m2r(mulps, var, reg)
#define	mulps_r2r(regs, regd)	sse_r2r(mulps, regs, regd)
/* DIVPS: */
/* RCPS: */
/* SQRTPS: */
/* MAXPS: */
/* MINPS: */
/* RSQRTPS: */
/*============================================================================*
 *                                 Compare                                    * 
 *============================================================================*/
/*============================================================================*
 *                        Data Shuffle and Unpacking                          * 
 *============================================================================*/
#define shufps(r1, r2, imm) \
	__asm__ __volatile__ ( \
				 "shufps $" #imm ", %" #r1 ", %" #r2 " \n" \
				 );

/*============================================================================*
 *                             Type Conversion                                * 
 *============================================================================*/
/*============================================================================*
 *                                  Logical                                   * 
 *============================================================================*/
/*============================================================================*
 *                                   Other                                    * 
 *============================================================================*/
/* PREFETCHT0: */
#define prefetch0(p) 	\
	__asm__ __volatile__ ( \
			      "prefetch0  %0 \n" \
			      : "=m" (p) \
			      : /* nothing */ )
/* PREFETCHT1: */
/* PREFETCHT2: */
/* PREFETCHNTA: */
#endif /* _EPX_SSE_ */
#endif
