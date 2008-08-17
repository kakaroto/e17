#ifndef _EPX_MMX_H
#define _EPX_MMX_H

/*	mmx.h

	MultiMedia eXtensions GCC interface library for IA32.

	To use this library, simply include this header file
	and compile with GCC.  You MUST have inlining enabled
	in order for mmx_ok() to work; this can be done by
	simply using -O on the GCC command line.

	Compiling with -DMMX_TRACE will cause detailed trace
	output to be sent to stderr for each mmx operation.
	This adds lots of code, and obviously slows execution to
	a crawl, but can be very useful for debugging.

	THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT
	LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY
	AND FITNESS FOR ANY PARTICULAR PURPOSE.

	1997-98 by H. Dietz and R. Fisher

 History:
	97-98*	R.Fisher	Early versions
	980501	R.Fisher	Original Release
	980611*	H.Dietz		Rewrite, correctly implementing inlines, and
		R.Fisher	 including direct register accesses.
	980616	R.Fisher	Release of 980611 as 980616.
	980714	R.Fisher	Minor corrections to Makefile, etc.
	980715	R.Fisher	mmx_ok() now prevents optimizer from using
				 clobbered values.
				mmx_ok() now checks if cpuid instruction is
				 available before trying to use it.
	980726*	R.Fisher	mm_support() searches for AMD 3DNow, Cyrix
				 Extended MMX, and standard MMX.  It returns a
				 value which is positive if any of these are
				 supported, and can be masked with constants to
				 see which.  mmx_ok() is now a call to this
	980726*	R.Fisher	Added i2r support for shift functions
	980919	R.Fisher	Fixed AMD extended feature recognition bug.
	980921	R.Fisher	Added definition/check for _MMX_H.
				Added "float s[2]" to mmx_t for use with
				  3DNow and EMMX.  So same mmx_t can be used.
	981013	R.Fisher	Fixed cpuid function 1 bug (looked at wrong reg)
				Fixed psllq_i2r error in mmxtest.c

	* Unreleased (internal or interim) versions

 Notes:
	It appears that the latest gas has the pand problem fixed, therefore
	  I'll undefine BROKEN_PAND by default.
	String compares may be quicker than the multiple test/jumps in vendor
	  test sequence in mmx_ok(), but I'm not concerned with that right now.

 Acknowledgments:
	Jussi Laako for pointing out the errors ultimately found to be
	  connected to the failure to notify the optimizer of clobbered values.
	Roger Hardiman for reminding us that CPUID isn't everywhere, and that
	  someone may actually try to use this on a machine without CPUID.
	  Also for suggesting code for checking this.
	Robert Dale for pointing out the AMD recognition bug.
	Jimmy Mayfield and Carl Witty for pointing out the Intel recognition
	  bug.
	Carl Witty for pointing out the psllq_i2r test bug.
*/

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

#define	mmx_i2r(op, imm, reg) \
	__asm__ __volatile__ (#op " $" #imm ", %%" #reg \
			      : /* nothing */ \
			      : /* nothing */);

#define	mmx_m2r(op, mem, reg) \
	__asm__ __volatile__ (#op " %0, %%" #reg \
			      : /* nothing */ \
			      : "m" (mem))

#define	mmx_r2m(op, reg, mem) \
	__asm__ __volatile__ (#op " %%" #reg ", %0" \
			      : "=m" (mem) \
			      : /* nothing */ )

#define	mmx_a2r(op, mem, reg) \
	__asm__ __volatile__ (#op " %0, %%" #reg \
			      : /* nothing */ \
			      : "m" (mem))

#define	mmx_r2a(op, reg, mem) \
	__asm__ __volatile__ (#op " %%" #reg ", %0" \
			      : "=m" (mem) \
			      : /* nothing */ )

#define	mmx_r2r(op, regs, regd) \
	__asm__ __volatile__ (#op " %" #regs ", %" #regd)

#define	mmx_m2m(op, mems, memd) \
	__asm__ __volatile__ ("movq %0, %%mm0\n\t" \
			      #op " %1, %%mm0\n\t" \
			      "movq %%mm0, %0" \
			      : "=X" (memd) \
			      : "X" (mems))

/*============================================================================*
 *                             Data Transfer                                  * 
 *============================================================================*/
/*	1x64 MOVE Quadword
	(this is both a load and a store...
	 in fact, it is the only way to store)
*/
#define	movq_m2r(var, reg)	mmx_m2r(movq, var, reg)
#define	movq_r2m(reg, var)	mmx_r2m(movq, reg, var)
#define	movq_r2r(regs, regd)	mmx_r2r(movq, regs, regd)
#define	movq(vars, vard) \
	__asm__ __volatile__ ("movq %1, %%mm0\n\t" \
			      "movq %%mm0, %0" \
			      : "=X" (vard) \
			      : "X" (vars))
#define	movntq_r2m(reg, var)   mmx_r2m(movntq, reg, var)
/*	1x32 MOVE Doubleword
	(like movq, this is both load and store...
	 but is most useful for moving things between
	 mmx registers and ordinary registers)
*/
#define	movd_m2r(var, reg)	mmx_a2r(movd, var, reg)
#define	movd_r2m(reg, var)	mmx_r2a(movd, reg, var)
#define	movd_r2r(regs, regd)	mmx_r2r(movd, regs, regd)
#define	movd(vars, vard) \
	__asm__ __volatile__ ("movd %1, %%mm0\n\t" \
			      "movd %%mm0, %0" \
			      : "=X" (vard) \
			      : "X" (vars))
/*============================================================================*
 *                               Arithmetic                                   * 
 *============================================================================*/
/*	2x32, 4x16, and 8x8 Parallel ADDs
*/
#define	paddd_m2r(var, reg)	mmx_m2r(paddd, var, reg)
#define	paddd_r2r(regs, regd)	mmx_r2r(paddd, regs, regd)
#define	paddd(vars, vard)	mmx_m2m(paddd, vars, vard)

#define	paddw_m2r(var, reg)	mmx_m2r(paddw, var, reg)
#define	paddw_r2r(regs, regd)	mmx_r2r(paddw, regs, regd)
#define	paddw(vars, vard)	mmx_m2m(paddw, vars, vard)

#define	paddb_m2r(var, reg)	mmx_m2r(paddb, var, reg)
#define	paddb_r2r(regs, regd)	mmx_r2r(paddb, regs, regd)
#define	paddb(vars, vard)	mmx_m2m(paddb, vars, vard)


/*	4x16 and 8x8 Parallel ADDs using Saturation arithmetic
*/
#define	paddsw_m2r(var, reg)	mmx_m2r(paddsw, var, reg)
#define	paddsw_r2r(regs, regd)	mmx_r2r(paddsw, regs, regd)
#define	paddsw(vars, vard)	mmx_m2m(paddsw, vars, vard)

#define	paddsb_m2r(var, reg)	mmx_m2r(paddsb, var, reg)
#define	paddsb_r2r(regs, regd)	mmx_r2r(paddsb, regs, regd)
#define	paddsb(vars, vard)	mmx_m2m(paddsb, vars, vard)


/*	4x16 and 8x8 Parallel ADDs using Unsigned Saturation arithmetic
*/
#define	paddusw_m2r(var, reg)	mmx_m2r(paddusw, var, reg)
#define	paddusw_r2r(regs, regd)	mmx_r2r(paddusw, regs, regd)
#define	paddusw(vars, vard)	mmx_m2m(paddusw, vars, vard)

#define	paddusb_m2r(var, reg)	mmx_m2r(paddusb, var, reg)
#define	paddusb_r2r(regs, regd)	mmx_r2r(paddusb, regs, regd)
#define	paddusb(vars, vard)	mmx_m2m(paddusb, vars, vard)


/*	2x32, 4x16, and 8x8 Parallel SUBs
*/
#define	psubd_m2r(var, reg)	mmx_m2r(psubd, var, reg)
#define	psubd_r2r(regs, regd)	mmx_r2r(psubd, regs, regd)
#define	psubd(vars, vard)	mmx_m2m(psubd, vars, vard)

#define	psubw_m2r(var, reg)	mmx_m2r(psubw, var, reg)
#define	psubw_r2r(regs, regd)	mmx_r2r(psubw, regs, regd)
#define	psubw(vars, vard)	mmx_m2m(psubw, vars, vard)

#define	psubb_m2r(var, reg)	mmx_m2r(psubb, var, reg)
#define	psubb_r2r(regs, regd)	mmx_r2r(psubb, regs, regd)
#define	psubb(vars, vard)	mmx_m2m(psubb, vars, vard)


/*	4x16 and 8x8 Parallel SUBs using Saturation arithmetic
*/
#define	psubsw_m2r(var, reg)	mmx_m2r(psubsw, var, reg)
#define	psubsw_r2r(regs, regd)	mmx_r2r(psubsw, regs, regd)
#define	psubsw(vars, vard)	mmx_m2m(psubsw, vars, vard)

#define	psubsb_m2r(var, reg)	mmx_m2r(psubsb, var, reg)
#define	psubsb_r2r(regs, regd)	mmx_r2r(psubsb, regs, regd)
#define	psubsb(vars, vard)	mmx_m2m(psubsb, vars, vard)


/*	4x16 and 8x8 Parallel SUBs using Unsigned Saturation arithmetic
*/
#define	psubusw_m2r(var, reg)	mmx_m2r(psubusw, var, reg)
#define	psubusw_r2r(regs, regd)	mmx_r2r(psubusw, regs, regd)
#define	psubusw(vars, vard)	mmx_m2m(psubusw, vars, vard)

#define	psubusb_m2r(var, reg)	mmx_m2r(psubusb, var, reg)
#define	psubusb_r2r(regs, regd)	mmx_r2r(psubusb, regs, regd)
#define	psubusb(vars, vard)	mmx_m2m(psubusb, vars, vard)


/*	4x16 Parallel MULs giving Low 4x16 portions of results
*/
#define	pmullw_m2r(var, reg)	mmx_m2r(pmullw, var, reg)
#define	pmullw_r2r(regs, regd)	mmx_r2r(pmullw, regs, regd)
#define	pmullw(vars, vard)	mmx_m2m(pmullw, vars, vard)


/*	4x16 Parallel MULs giving High 4x16 portions of results
*/
#define	pmulhw_m2r(var, reg)	mmx_m2r(pmulhw, var, reg)
#define	pmulhw_r2r(regs, regd)	mmx_r2r(pmulhw, regs, regd)
#define	pmulhw(vars, vard)	mmx_m2m(pmulhw, vars, vard)


/*	4x16->2x32 Parallel Mul-ADD
	(muls like pmullw, then adds adjacent 16-bit fields
	 in the multiply result to make the final 2x32 result)
*/
#define	pmaddwd_m2r(var, reg)	mmx_m2r(pmaddwd, var, reg)
#define	pmaddwd_r2r(regs, regd)	mmx_r2r(pmaddwd, regs, regd)
#define	pmaddwd(vars, vard)	mmx_m2m(pmaddwd, vars, vard)
/*============================================================================*
 *                                  Logical                                   * 
 *============================================================================*/
#define	pand_m2r(var, reg)	mmx_m2r(pand, var, reg)
#define	pand_r2r(regs, regd)	mmx_r2r(pand, regs, regd)
#define	pand(vars, vard)	mmx_m2m(pand, vars, vard)

/*	1x64 bitwise AND with Not the destination
*/
#define	pandn_m2r(var, reg)	mmx_m2r(pandn, var, reg)
#define	pandn_r2r(regs, regd)	mmx_r2r(pandn, regs, regd)
#define	pandn(vars, vard)	mmx_m2m(pandn, vars, vard)

/*	1x64 bitwise OR
*/
#define	por_m2r(var, reg)	mmx_m2r(por, var, reg)
#define	por_r2r(regs, regd)	mmx_r2r(por, regs, regd)
#define	por(vars, vard)	mmx_m2m(por, vars, vard)

/*	1x64 bitwise eXclusive OR
*/
#define	pxor_m2r(var, reg)	mmx_m2r(pxor, var, reg)
#define	pxor_r2r(regs, regd)	mmx_r2r(pxor, regs, regd)
#define	pxor(vars, vard)	mmx_m2m(pxor, vars, vard)

/*============================================================================*
 *                               Comparison                                   * 
 *============================================================================*/
/*	2x32, 4x16, and 8x8 Parallel CoMPare for EQuality
	(resulting fields are either 0 or -1)
*/
#define	pcmpeqd_m2r(var, reg)	mmx_m2r(pcmpeqd, var, reg)
#define	pcmpeqd_r2r(regs, regd)	mmx_r2r(pcmpeqd, regs, regd)
#define	pcmpeqd(vars, vard)	mmx_m2m(pcmpeqd, vars, vard)

#define	pcmpeqw_m2r(var, reg)	mmx_m2r(pcmpeqw, var, reg)
#define	pcmpeqw_r2r(regs, regd)	mmx_r2r(pcmpeqw, regs, regd)
#define	pcmpeqw(vars, vard)	mmx_m2m(pcmpeqw, vars, vard)

#define	pcmpeqb_m2r(var, reg)	mmx_m2r(pcmpeqb, var, reg)
#define	pcmpeqb_r2r(regs, regd)	mmx_r2r(pcmpeqb, regs, regd)
#define	pcmpeqb(vars, vard)	mmx_m2m(pcmpeqb, vars, vard)


/*	2x32, 4x16, and 8x8 Parallel CoMPare for Greater Than
	(resulting fields are either 0 or -1)
*/
#define	pcmpgtd_m2r(var, reg)	mmx_m2r(pcmpgtd, var, reg)
#define	pcmpgtd_r2r(regs, regd)	mmx_r2r(pcmpgtd, regs, regd)
#define	pcmpgtd(vars, vard)	mmx_m2m(pcmpgtd, vars, vard)

#define	pcmpgtw_m2r(var, reg)	mmx_m2r(pcmpgtw, var, reg)
#define	pcmpgtw_r2r(regs, regd)	mmx_r2r(pcmpgtw, regs, regd)
#define	pcmpgtw(vars, vard)	mmx_m2m(pcmpgtw, vars, vard)

#define	pcmpgtb_m2r(var, reg)	mmx_m2r(pcmpgtb, var, reg)
#define	pcmpgtb_r2r(regs, regd)	mmx_r2r(pcmpgtb, regs, regd)
#define	pcmpgtb(vars, vard)	mmx_m2m(pcmpgtb, vars, vard)
/*============================================================================*
 *                           Shift and Rotate                                 * 
 *============================================================================*/
/*	1x64, 2x32, and 4x16 Parallel Shift Left Logical
*/
#define	psllq_i2r(imm, reg)	mmx_i2r(psllq, imm, reg)
#define	psllq_m2r(var, reg)	mmx_m2r(psllq, var, reg)
#define	psllq_r2r(regs, regd)	mmx_r2r(psllq, regs, regd)
#define	psllq(vars, vard)	mmx_m2m(psllq, vars, vard)

#define	pslld_i2r(imm, reg)	mmx_i2r(pslld, imm, reg)
#define	pslld_m2r(var, reg)	mmx_m2r(pslld, var, reg)
#define	pslld_r2r(regs, regd)	mmx_r2r(pslld, regs, regd)
#define	pslld(vars, vard)	mmx_m2m(pslld, vars, vard)

#define	psllw_i2r(imm, reg)	mmx_i2r(psllw, imm, reg)
#define	psllw_m2r(var, reg)	mmx_m2r(psllw, var, reg)
#define	psllw_r2r(regs, regd)	mmx_r2r(psllw, regs, regd)
#define	psllw(vars, vard)	mmx_m2m(psllw, vars, vard)


/*	1x64, 2x32, and 4x16 Parallel Shift Right Logical
*/
#define	psrlq_i2r(imm, reg)	mmx_i2r(psrlq, imm, reg)
#define	psrlq_m2r(var, reg)	mmx_m2r(psrlq, var, reg)
#define	psrlq_r2r(regs, regd)	mmx_r2r(psrlq, regs, regd)
#define	psrlq(vars, vard)	mmx_m2m(psrlq, vars, vard)

#define	psrld_i2r(imm, reg)	mmx_i2r(psrld, imm, reg)
#define	psrld_m2r(var, reg)	mmx_m2r(psrld, var, reg)
#define	psrld_r2r(regs, regd)	mmx_r2r(psrld, regs, regd)
#define	psrld(vars, vard)	mmx_m2m(psrld, vars, vard)

#define	psrlw_i2r(imm, reg)	mmx_i2r(psrlw, imm, reg)
#define	psrlw_m2r(var, reg)	mmx_m2r(psrlw, var, reg)
#define	psrlw_r2r(regs, regd)	mmx_r2r(psrlw, regs, regd)
#define	psrlw(vars, vard)	mmx_m2m(psrlw, vars, vard)


/*	2x32 and 4x16 Parallel Shift Right Arithmetic
*/
#define	psrad_i2r(imm, reg)	mmx_i2r(psrad, imm, reg)
#define	psrad_m2r(var, reg)	mmx_m2r(psrad, var, reg)
#define	psrad_r2r(regs, regd)	mmx_r2r(psrad, regs, regd)
#define	psrad(vars, vard)	mmx_m2m(psrad, vars, vard)

#define	psraw_i2r(imm, reg)	mmx_i2r(psraw, imm, reg)
#define	psraw_m2r(var, reg)	mmx_m2r(psraw, var, reg)
#define	psraw_r2r(regs, regd)	mmx_r2r(psraw, regs, regd)
#define	psraw(vars, vard)	mmx_m2m(psraw, vars, vard)

#ifdef _EPX_SSE_
/* 
 * SSE 64bit SIMD instructions:
 * PAVGB Compute average of packed unsigned byte integers
 * PAVGW Compute average of packed unsigned word integers
 * PEXTRW Extract word
 * PINSRW Insert word
 * PMAXUB Maximum of packed unsigned byte integers
 * PMAXSW Maximum of packed signed word integers
 * PMINUB Minimum of packed unsigned byte integers
 * PMINSW Minimum of packed signed word integers
 * PMOVMSKB Move byte mask
 * PSADBW Compute sum of absolute differences
 * PSHUFW Shuffle packed integer word in MMX register
 */
/* PMULHUW Multiply packed unsigned integers and store high result */
#define	pmulhuw_m2r(var, reg)	mmx_m2r(pmulhuw, var, reg)
#define	pmulhuw_r2r(regs, regd)	mmx_r2r(pmulhuw, regs, regd)
#define	pmulhuw(vars, vard)	mmx_m2m(pmulhuw, vars, vard)
#endif

#endif /* _EPX_MMX_ */
#endif
