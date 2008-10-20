#ifndef E_MOD_MACRO_H
#define E_MOD_MACRO_H

#include <eina_types.h>

#define D_(str) dgettext(PACKAGE, str)

#define MOD_CFG_FILE_EPOCH 0x0002
#define MOD_CFG_FILE_GENERATION 0x0004
#define MOD_CFG_FILE_VERSION \
   ((MOD_CFG_FILE_EPOCH << 16) | MOD_CFG_FILE_GENERATION)

#undef  MIN_LEN
#define MIN_LEN(str1, str2) strlen(str1) < strlen(str2) ? strlen(str1) : strlen(str2)

#undef  MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#endif

