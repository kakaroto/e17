
#define DEBUG 1

#define E(lvl,fmt,args...) \
 do {\
   if(DEBUG>=(lvl)) \
     fprintf(stderr,fmt, args); \
 } while(0)

#include "gui.h"

#include "config.h"

#define BUFLEN 100
