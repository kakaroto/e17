/*  Fixed-point arithmetic for the Small AMX
 *
 *  Fixed point numbers compromise range versus number of decimals. This
 *  library decimal fixed point numbers with an configurable number of
 *  decimals. The current setting is 3 decimals.
 *
 *  Copyright (c) ITB CompuPhase, 1998-1999
 *  This file may be freely used. No warranties of any kind.
 */  
#include <assert.h>
#include <ctype.h>
#include <stdio.h>      /* for NULL */
#include "amx.h"

#define MULTIPLIER      1000L   /* 10^decimals */


#if defined __BORLANDC__ || defined __WATCOMC__
#pragma argsused
#endif	/*  */
static cell AMX_NATIVE_CALL fixed(AMX * amx, cell * params) 
{
	
		return params[1] * MULTIPLIER;
	
}


static cell AMX_NATIVE_CALL fixedstr(AMX * amx, cell * params) 
{
	
		char str[50], *ptr;
	
		cell * cstr, intpart, decimals, mult;
	
		int len;
	
		
		amx_GetAddr(amx, params[1], &cstr);
	
		amx_StrLen(cstr, &len);
	
		if (len >= 50) {
		
			amx_RaiseError(amx, AMX_ERR_NATIVE);
		
			return 0;
		
	}							/* if */
	
		amx_GetString(str, cstr);
	
		ptr = str;
	
		intpart = 0;
	
		decimals = 0;
	
		mult = 1;
	
		while (isdigit(*ptr)) {
		
			intpart = intpart * 10 + (*ptr - '0');
		
			ptr++;
		
	}							/* while */
	
		if (*ptr == '.') {
		
			ptr++;
		
			len = 0;
		
			while (isdigit(*ptr) && len < 4) {
			
				decimals = decimals * 10 + (*ptr - '0');
			
				mult *= 10;
			
				ptr++;
			
				len++;
			
		}						/* while */
		
	}							/* if */
	
		return (intpart * MULTIPLIER) + (decimals * MULTIPLIER) / mult;
	
	// ??? ^^^^ round this
}


#if defined __BORLANDC__ || defined __WATCOMC__
#pragma argsused
#endif	/*  */
static cell AMX_NATIVE_CALL fmul(AMX * amx, cell * params) 
{
	
#if defined(__WATCOMC__) && defined(__386__)
		
		cell __fmul(void);
	
		cell a = params[1];
	
		cell b = params[2];
	
#if MULTIPLIER != 1000
#error Assembler chunks must be modified for a different base
#endif	/*  */
#pragma aux __fmul =    \
		"mov    eax, [a]" \ 
		"mov    ebx, 1000" \ 
		"imul   [b]" \ 
		"idiv   ebx" \ 
		"mov    [a], eax" \ 
		modify[eax ebx edx];
	
		__fmul();
	
		return a;
	
		
#elif _MSC_VER>=9 && defined _WIN32
		
		__int64 a = (__int64) params[1] * (__int64) params[2];
	
		a /= MULTIPLIER;
	
		return (cell) a;
	
		
#else	/*  */
		
	/* (Xs * Ys) == (X*Y)ss, where "s" stands for scaled.
	 * The desired result is (X*Y)s, so we must unscale once.
	 * but we cannot do this before multiplication, because of loss
	 * of precision, and we cannot do it after the multiplication
	 * because of the possible overflow.
	 * The technique used here is to cut the multiplicands into
	 * components and to multiply these components separately:
	 *
	 * Assume Xs == (A << 16) + B and Ys == (C << 16) + D, where A, B,
	 * C and D are 16 bit numbers.
	 *
	 *    A B
	 *    C D
	 *    --- *
	 *    D*B + (D*A << 16) + (C*B << 16) + (C*A << (16+16))
	 *
	 * Now we can unscale each of these factors just before adding them.
	 */ 
		cell a, b, c, d;
	
		
		assert(sizeof(cell) == 4);
	
		assert(MULTIPLIER <= (1 << 16));
	
		
		a = (params[1] >> 16) & 0xffff;
	
		b = params[1] & 0xffff;
	
		c = (params[2] >> 16) & 0xffff;
	
		d = params[2] & 0xffff;
	
		return (d * b / MULTIPLIER) 
		+((d * a << 16) / MULTIPLIER) 
		+((c * b << 16) / MULTIPLIER) 
		+((c * a << 32) / MULTIPLIER);
	
		
#endif	/*  */
}


#if defined __BORLANDC__ || defined __WATCOMC__
#pragma argsused
#endif	/*  */
static cell AMX_NATIVE_CALL fdiv(AMX * amx, cell * params) 
{
	
#if defined(__WATCOMC__) && defined(__386__)
		
		cell __fdiv(void);
	
		cell a = params[1];
	
		cell b = params[2];
	
#if MULTIPLIER != 1000
#error Assembler chunks must be modified for a different base
#endif	/*  */
#pragma aux __fdiv =    \
		"mov    eax, [a]" \ 
		"cdq" \ 
		"mov    ebx, 1000" \ 
		"imul   ebx" \ 
		"idiv   [b]" \ 
		"mov    [a], eax" \ 
		modify[eax ebx edx];
	
		__fdiv();
	
		return a;
	
		
#elif _MSC_VER>=9 && defined _WIN32
		
		__int64 a = (__int64) params[1] * (__int64) 1000;
	
		a /= (__int64) params[2];
	
		return (cell) a;
	
		
#else	/*  */
		
	/* The dividend must be scaled prior to division. The dividend
	 * is a 32-bit number, however, so when shifted, it will become
	 * a value that no longer fits in a 32-bit variable. This routine
	 * does the division by using only 16-bit and 32-bit values, but
	 * with considerable effort.
	 * If your compiler supports 64-bit integers, modify this routine
	 * to use them. If your processor can do a simple 64-bit by 32-bit
	 * division in assembler, write assembler chunks.
	 * In other words: the straight C routine that follows is correct
	 * and portable, but use it only as a last resort.
	 *
	 * This function was adapted from source code that appeared in
	 * Dr. Dobb's Journal, August 1992, page 117.
	 */ 
		typedef unsigned short ushort;
	
#define LOWORD(v)     (ushort)((v) & 0xffff)
#define HIWORD(v)     (ushort)(((v) >> 16) & 0xffff)
		
		cell dividend = params[1];
	
		cell divisor = params[2];
	
		cell result;
	
		int negate = 0;
	
		ucell b[2], t[2], u, v;
	
		ushort rHigh, rLow, dHigh, dLow;
	
		
		if (divisor == 0) {
		
			amx_RaiseError(amx, AMX_ERR_NATIVE);
		
			return 0;
		
	}							/* if */
	
		
	/* make both operands positive values, but keep the sign of the result */ 
		if (dividend < 0) {
		
			dividend = -dividend;
		
			negate = !negate;	/* negate result */
		
	}							/* if */
	
		if (divisor < 0) {
		
			divisor = -divisor;
		
			negate = !negate;	/* negate result */
		
	}							/* if */
	
		
	/* pre-scale the dividend into a 64-bit number */ 
		b[0] = dividend * MULTIPLIER;
	
		b[1] = (HIWORD(dividend) * MULTIPLIER) >> 16;
	
		
	/* If the divisor is now smaller than b[1], the result
	 * will not fit in a cell.
	 */ 
		if ((ucell) divisor < b[1]) {
		
			amx_RaiseError(amx, AMX_ERR_NATIVE);
		
			return 0;
		
	}							/* if */
	
		
		dHigh = HIWORD(divisor);
	
		dLow = LOWORD(divisor);
	
		
		t[0] = b[0];			/* backup the 64-bit number */
	
		t[1] = b[1];
	
		
	/* Underestimate high half of quotient and subtract product
	 * of estimate and divisor from dividend.
	 */ 
		rHigh = (ushort) (t[1] / (dHigh + 1));
	
		u = (ucell) rHigh *(ucell) dLow;
	
		v = (ucell) rHigh *(ucell) dHigh;
	
		if ((t[0] -= (u << 16)) > (0xffffffffL - (u << 16)))
		
			t[1]--;
	
		t[1] -= HIWORD(u);
	
		t[1] -= v;
	
		
	/* Correct estimate. */ 
		while ((t[1] > dHigh) || ((t[1] == dHigh) && (t[0] >= ((ucell) dLow << 16)))) {
		
			if ((t[0] -= ((ucell) dLow << 16)) > 0xffffffffL - ((ucell) dLow << 16))
			
				t[1]--;
		
			t[1] -= dHigh;
		
			rHigh++;
		
	}							/* while */
	
	/* Underestimate low half of quotient and subtract product of
	 * estimate and divisor from what remains of dividend.
	 */ 
		rLow = (ushort) ((ucell) ((t[1] << 16) + HIWORD(t[0])) / (dHigh + 1));
	
		u = (ucell) rLow *(ucell) dLow;
	
		v = (ucell) rLow *(ucell) dHigh;
	
		if ((t[0] -= u) > (0xffffffffL - u))
		
			t[1]--;
	
		if ((t[0] -= (v << 16)) > (0xffffffffL - (v << 16)))
		
			t[1]--;
	
		t[1] -= HIWORD(v);
	
		
	/* Correct estimate. */ 
		while ((t[1] > 0) || ((t[1] == 0) && t[0] >= (ucell) divisor)) {
		
			if ((t[0] -= divisor) > (0xffffffffL - divisor))
			
				t[1]--;
		
			rLow++;
		
	}							/* while */
	
		
		result = ((ucell) rHigh << 16) + rLow;
	
		if (negate)
		
			result = -result;
	
		return result;
	
		
#endif	/*  */
}


#if defined __BORLANDC__ || defined __WATCOMC__
#pragma argsused
#endif	/*  */
static cell AMX_NATIVE_CALL ffract(AMX * amx, cell * params) 
{
	
		return params[1] % MULTIPLIER;
	
}


#if defined __BORLANDC__ || defined __WATCOMC__
#pragma argsused
#endif	/*  */
static cell AMX_NATIVE_CALL fround(AMX * amx, cell * params) 
{
	
		cell value;
	
		
		switch (params[2]) {
			
		case 1:				/* round downwards (truncate) */
			
				value = params[1] / MULTIPLIER;
			
				if (params[1] < 0 && (params[1] % MULTIPLIER) != 0)
				
					value--;
			
				break;
			
		case 2:				/* round upwards */
			
				value = params[1] / MULTIPLIER;
			
				if (params[1] >= 0 && (params[1] % MULTIPLIER) != 0)
				
					value++;
			
				break;
			
		case 3:				/* round to even number when fractional part is exactly 0.5 */
			
				value = (params[1] + MULTIPLIER / 2) / MULTIPLIER;
			
				if ((params[1] % MULTIPLIER) == MULTIPLIER / 2 && (value & 1) == 1)
				
					value--;
			
				break;
			
		default:				/* standard (fractional pert of 0.5 is rounded up */
			
				value = (params[1] + MULTIPLIER / 2) / MULTIPLIER;
			
	}							/* switch */
	
		return value;
	
}


AMX_NATIVE_INFO fixed_Natives[] = {
	
	{
		"fixed", fixed
	}
	,
	{
		"fixedstr", fixedstr
	}
	,
	{
		"fmul", fmul
	}
	,
	{
		"fdiv", fdiv
	}
	,
	{
		"ffract", ffract
	}
	,
	{
		"fround", fround
	}
	,
	{
		NULL, NULL
	}							/* terminator */
	
};

