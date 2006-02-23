#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include "pixel_op.h"
#include "mmx.h"

static Pixel_Op_Func op_table
  [PIXEL_OP_LAST]/*pix op*/
  [SP_LAST]/*src p*/ [SM_LAST]/*src m*/ [SC_LAST]/*src c*/
  [D_LAST]/*dst*/
  [5]/*CPU*/
  ;

#define OPFN(n) static void n(Pixel_Op_Params *p)
#define OPREG(n, op, sp, sm, sc, dp, cpu) op_table[op][sp][sm][sc][dp][cpu] = n

/* this is only here instead of autfoo checks - not perfect */
#if defined(__POWERPC__) || defined(__powerpc__) || defined(__sparc__) || defined(sparc) ||  defined(__sparc64__) || defined(__mips__) || defined(m68k)
# define WORDS_BIGENDIAN
#else
# undef WORDS_BIGENDIAN
#endif

#ifndef WORDS_BIGENDIAN
# define A_VAL(p) ((DATA8 *)(p))[3]
# define R_VAL(p) ((DATA8 *)(p))[2]
# define G_VAL(p) ((DATA8 *)(p))[1]
# define B_VAL(p) ((DATA8 *)(p))[0]
#else
# define A_VAL(p) ((DATA8 *)(p))[0]
# define R_VAL(p) ((DATA8 *)(p))[1]
# define G_VAL(p) ((DATA8 *)(p))[2]
# define B_VAL(p) ((DATA8 *)(p))[3]
#endif

/* this is only here instead of autfoo checks - not perfect */
#if defined(i386) || defined(__x86__) || defined(__x86_64__) || defined(__i386__) || defined(__386__) || defined(__i386)
# define ARCH_X86

# define CPU_C    0
# define CPU_OS   1
# define CPU_MMX  2
# define CPU_SSE  3
# define CPU_SSE2 4

static int did_mmx = 0;

# define OP_FN
# include "op_copy_main_.c"
# include "op_blend_main_.c"
# include "op_mul_main_.c"
# include "op_copy_main_i386.c"
# include "op_blend_main_i386.c"
# include "op_mul_main_i386.c"
# undef OP_FN

#endif

/* this is only here instead of autfoo checks - not perfect */
#if (defined(__POWERPC__) || defined(__powerpc__)) && defined(__VEC__)
# define ARCH_PPC

# define CPU_C       0
# define CPU_OS      1
# define CPU_ALTIVEC 2

# define OP_FN
# include "op_copy_main_.c"
# include "op_blend_main_.c"
# include "op_mul_main_.c"
# include "op_copy_main_ppc.c"
# include "op_blend_main_ppc.c"
# include "op_mul_main_ppc.c"
# undef OP_FN

#endif

static DATA32 cpu_flags = CPU_OS;
static sigjmp_buf cpu_detect_buf;

static void
_pixel_op_cpu_sig(int sig)
{
   siglongjmp(cpu_detect_buf, 1);
}
    
static int
_pixel_op_cpu_test(void (*fn) (void))
{
   int enabled = 1;
   struct sigaction act, oact, oact2;

   act.sa_handler = _pixel_op_cpu_sig;
   act.sa_flags = SA_RESTART;
   sigemptyset(&act.sa_mask);
   sigaction(SIGILL, &act, &oact);
   
   act.sa_handler = _pixel_op_cpu_sig;
   act.sa_flags = SA_RESTART;
   sigemptyset(&act.sa_mask);
   sigaction(SIGSEGV, &act, &oact2);
   
   if (sigsetjmp(cpu_detect_buf, 1))
     {
	sigaction(SIGILL, &oact, NULL);
	sigaction(SIGSEGV, &oact2, NULL);
	return 0;
     }
   
   fn();
   
   sigaction(SIGILL, &oact, NULL);
   sigaction(SIGSEGV, &oact2, NULL);
   return enabled;
}

#ifdef ARCH_X86
static void
_pixel_op_cpu_mmx_test(void)
{
   pxor_r2r(mm1, mm1);
}

static void
_pixel_op_cpu_sse_test(void)
{
   pshufw(mm1, mm1, 0xff);
}

static void
_pixel_op_cpu_sse2_test(void)
{
   DATA8 buf[32];
   movdqa_r2r(xmm1, xmm1);
   packuswb_r2r(xmm1, xmm1);
   movq_r2r(xmm1, xmm2);
   movq_r2m(xmm1, buf);
}
#endif

#ifdef ARCH_PPC
static void
_pixel_op_cpu_altivec_test(void)
{    
   vector unsigned int zero;
   
   zero = vec_splat_u32(0);
}
#endif

int
pixel_op_init(void)
{
#ifdef ARCH_X86
   if (_pixel_op_cpu_test(_pixel_op_cpu_mmx_test))
     {
	cpu_flags = CPU_MMX;
	emms();
	if (_pixel_op_cpu_test(_pixel_op_cpu_sse_test))
	  {
	     cpu_flags = CPU_SSE;
	     emms();
	     if (_pixel_op_cpu_test(_pixel_op_cpu_sse2_test))
	       {
		  cpu_flags = CPU_SSE2;
		  emms();
	       }
	  }
     }
#endif   
#ifdef ARCH_PPC
   if (_pixel_op_cpu_test(_pixel_op_cpu_altivec_test))
     {
	cpu_flags = CPU_ALTIVEC;
     }
#endif   

   memset(op_table, 0, sizeof(op_table));
   
# define OP_REG
# include "op_copy_main_.c"
# include "op_blend_main_.c"
# include "op_mul_main_.c"
# ifdef ARCH_X86
#  include "op_copy_main_i386.c"
# include "op_blend_main_i386.c"
# include "op_mul_main_i386.c"
# endif   
# ifdef ARCH_PPC
#  include "op_copy_main_ppc.c"
# include "op_blend_main_ppc.c"
# include "op_mul_main_ppc.c"
# endif   
# undef OP_REG

   /* FIXME: we need to run performance tests on every routine to see what
    * is fast or slow */
   
   return 1;
}

int
pixel_op_shutdown(void)
{
   return 1;
}

inline void
pixel_op_params_init(Pixel_Op_Params *params, DATA32 v)
{
   if (!params) return;

   memset(params, 0, sizeof(Pixel_Op_Params));
   params->v = v;
   params->src.c = 0xffffffff;
}

Pixel_Op_Func
pixel_op_get(Pixel_Op_Params *params, int cpumode)
{
   DATA8 sp = SP_N, sm = SM_N, sc = SC;
   DATA8 dp = DP;
   Pixel_Op op = PIXEL_OP_COPY;
   
   if ((!params) || (!params->dst.p))
     return NULL;

   op = params->op;
   /* early check for an op for a cpu that doesnt support it */
   if (cpumode > cpu_flags) return NULL;
   
   /* early check for non-copy ops */
   if ((params->src.c == 0x00000000) && 
       ((params->op != PIXEL_OP_COPY) || (params->op != PIXEL_OP_MUL)))
     return NULL;
   
   /* set src.p related params */
   if ((params->src.p) && (params->src.pa < SP_LAST))
     sp = params->src.pa;
   
   /* set src.m related params */
   if ((params->src.m) && (params->src.ma < SM_LAST))
     sm = params->src.ma;
   
   /* set src.c related params */
   if (params->src.c == 0xffffffff)
     sc = SC_N;
   else if ((params->src.c & 0xff000000) == 0xff000000)
     sc = SC_AN;
   else if (params->src.c == ((params->src.c & 0xff000000) |
			      ((params->src.c >> 8) & 0xff0000) |
			      ((params->src.c >> 16) & 0xff00) |
			      ((params->src.c >> 24) & 0xff)))
     sc = SC_AA;
   if (params->src.c == 0x00000000)
     {
	sp = SP_N;
	sm = SM_N;
	op = PIXEL_OP_COPY;
     }

   /* set dst.p related params */
   if (params->dst.pa < DP_LAST)
     dp = params->dst.pa;
   
   return op_table[op][sp][sm][sc][dp][cpumode];
}

void
pixel_op(Pixel_Op_Params *params)
{
   Pixel_Op_Func func;
   
   func = pixel_op_get(params, CPU_C);
   func(params);
}

void
pixel_op_end(void)
{
#ifdef ARCH_X86
   if (did_mmx)
     {
	emms();
	did_mmx = 0;
     }
#endif   
}
