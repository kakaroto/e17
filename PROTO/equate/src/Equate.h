
#define DEBUG 1

#define E(lvl,fmt,args...)                      \
  do {                                          \
     if(DEBUG>=(lvl))                           \
       fprintf(stderr,fmt, args);               \
  } while(0)


#include <Elementary.h>

#include "config.h"
#include "calc.h"

#define BUFLEN 100

#define EQ_EXIT 1
#define OP_CLR 2
#define OP_DIV 3
#define OP_MUT 4
#define OP_ADD 5
#define OP_SUB 6
#define OP_EQU 7
#define OP_DEC 8
#define OP_SIN 9
#define OP_COS 10
#define OP_TAN 11
#define OP_ROOT 12
#define OP_POW 13
#define OP_LOG 14
#define OP_LN 15
#define OP_EXP 16
#define OP_OBRAK 17
#define OP_CBRAK 18

#define NUM_0 20
#define NUM_1 21
#define NUM_2 22
#define NUM_3 23
#define NUM_4 24
#define NUM_5 25
#define NUM_6 26
#define NUM_7 27
#define NUM_8 28
#define NUM_9 29
#define NUM_PI 30

