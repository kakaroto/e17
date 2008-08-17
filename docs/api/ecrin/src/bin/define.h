#ifndef __DEFINE_H__
#define __DEFINE_H__


#include "description.h"

/*
 * Defines
 */

typedef struct _Ecrin_Define Ecrin_Define;
struct _Ecrin_Define
{
  char              *name;
  char              *value;
  char               static_state;
  Ecrin_Description *description;
};

void          ecrin_define_display (Ecrin_Define *define);

Ecrin_Define *ecrin_define_new     (char              *name,
                                    char              *value,
                                    char               static_value,
                                    Ecrin_Description *description);

void          ecrin_define_free    (Ecrin_Define *define);



#endif /* __DEFINE_H__ */
