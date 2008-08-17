#ifndef __ENUM_H__
#define __ENUM_H__


#include <Ecore.h>

#include "description.h"


/*
 * Enum item
 */

typedef struct _Ecrin_Enum_Item Ecrin_Enum_Item;
struct _Ecrin_Enum_Item
{
  char              *name;
  char              *value;
  Ecrin_Description *description;
  
};

Ecrin_Enum_Item *ecrin_enum_item_new  (char              *name,
                                       char              *value,
                                       Ecrin_Description *description);
void             ecrin_enum_item_free (Ecrin_Enum_Item *item);



/*
 * Enum
 */
typedef struct _Ecrin_Enum Ecrin_Enum;
struct _Ecrin_Enum
{
  char       *name;
  char        static_state;
  Ecore_List *items;
};

void        ecrin_enum_display   (Ecrin_Enum *e);

Ecrin_Enum *ecrin_enum_new       (void);

void        ecrin_enum_name_set  (Ecrin_Enum *e,
                                  char       *name);

void        ecrin_enum_state_set (Ecrin_Enum *e,
                                  char        static_state);

void        ecrin_enum_free      (Ecrin_Enum *e);

void        ecrin_enum_item_add  (Ecrin_Enum      *e,
                                  Ecrin_Enum_Item *item);


#endif /* __ENUM_H__ */
