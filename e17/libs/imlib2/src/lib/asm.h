#ifndef __ASM_H
#define __ASM_H

#ifdef __EMX__
/* Due to strange behaviour of as.exe we use this macros */
/* For all OS/2 coders - please use PGCC to compile this code */
#define PR_(foo) ___##foo
#define PT_(foo,func) ___##foo,##func
#define FN_(foo) \
.globl PR_(foo);			       \
        .type PT_(foo,@function)
#define SIZE(sym)                              \
	.___end_##sym:;                        \
	.size ___##sym,.___end_##sym-___##sym; \
	.align 8;
#else /* not __EMX__ */
#define PR_(foo) __##foo
#define PT_(foo,func) __##foo,##func
#ifdef USE_HIDDEN_FUNCTION_ATTRIBUTE
#define FN_(foo) \
.globl PR_(foo);			    \
        .hidden PR_(foo);		    \
        .type PT_(foo,@function)
#else /* not USE_HIDDEN... */
#define FN_(foo) \
.globl PR_(foo);			    \
        .type PT_(foo,@function)
#endif /* USE_HIDDEN... */
#define SIZE(sym)                           \
	.__end_##sym:;                      \
	.size __##sym,.__end_##sym-__##sym; \
	.align 8;
#endif

#endif /* __ASM_H */
