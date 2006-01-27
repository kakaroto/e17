#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include "pixel_op.h"
#include "mmx.h"

static Pixel_Op_Func op_table
  [PIXEL_OP_LAST]
  [2]/*src p*/ [2]/*src m*/ [2]/*src c*/ [3]/*src alpha or sparse alpha*/
  [2]/*dst p or m*/[2]/*dst alpha */
  [5]/*CPU*/
  ;

#define S_PN 0
#define S_P1 1
#define S_MN 0
#define S_M1 1
#define S_CN 0
#define S_C1 1
#define S_AN 0
#define S_A1 1
#define S_AS 2
#define D_P  0
#define D_M  1
#define D_AN 0
#define D_A1 1
#define OPFN(n) static void n(Pixel_Op_Params *p)
#define OPREG(n, op, sp, sm, sc, sa, dpm, da, cpu) op_table[op][sp][sm][sc][sa][dpm][da][cpu] = n

#if defined(i386) || defined(__x86__) || defined(__x86_64__) || defined(__i386__) || defined(__386__) || defined(__i386)
# define ARCH_X86

# define CPU_C    0
# define CPU_OS   1
# define CPU_MMX  2
# define CPU_SSE  3
# define CPU_SSE2 4

static int did_mmx = 0;

# define OP_FN
# include "pixel_op_copy.c"
# include "pixel_op_copy_i386.c"
# undef OP_FN

#endif

#if (defined(__POWERPC__) || defined(__powerpc__)) && defined(__VEC__)
# define ARCH_PPC

# define CPU_C       0
# define CPU_OS      1
# define CPU_ALTIVEC 2

# define OP_FN
# include "pixel_op_copy.c"
# include "pixel_op_copy_ppc.c"
# undef OP_FN

#endif

static DATA32 cpu_flags = CPU_C;
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
   movdqa_r2r(xmm1, xmm1);
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
# include "pixel_op_copy.c"
# ifdef ARCH_X86
#  include "pixel_op_copy_i386.c"
# endif   
# ifdef ARCH_PPC
#  include "pixel_op_copy_ppc.c"
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
   memset(params, 0, sizeof(Pixel_Op_Params));
   params->v = v;
   params->src.c = 0xffffffff;
}

Pixel_Op_Func
pixel_op_get(Pixel_Op_Params *params, int cpumode)
{
   DATA8 sp = S_PN, sm = S_MN, sc = S_C1, sa = S_AN;
   DATA8 dpm = D_P, da = D_AN;
   
   if (params->src.c == 0xffffffff) sc = S_CN;
   else if ((params->src.c & 0xff) == 0) return NULL;
   if (params->src.p) sp = S_P1;
   if (params->src.m) sm = S_M1;
   sa += params->src.alpha;
   sa += params->src.sparse;
   if (params->dst.m) dpm = D_M;
   da = params->dst.alpha;
   return op_table[params->op][sp][sm][sc][sa][dpm][da][cpumode];
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
