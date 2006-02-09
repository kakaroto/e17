#ifndef _FDO_MENUS_H
# define _FDO_MENUS_H

#include "dumb_list.h"

# ifdef __cplusplus
extern "C"
{
# endif

   Dumb_List *fdo_menus_get(char *file, Dumb_List *xml);

# ifdef __cplusplus
}
# endif

#endif
