#ifndef _FDO_MENUS_H
# define _FDO_MENUS_H

#include "dumb_tree.h"

# ifdef __cplusplus
extern "C"
{
# endif

   Dumb_Tree *fdo_menus_get(char *file, Dumb_Tree *merge_stack, int level);

# ifdef __cplusplus
}
# endif

#endif
