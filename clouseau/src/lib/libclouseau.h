#ifndef _LIBCLOUSEAU_H
#define _LIBCLOUSEAU_H
#include <Eina.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Elementary.h>

#define ELM_INTERNAL_API_ARGESFSDFEFC
#include <elm_widget.h>

#include "config.h"

#ifdef HAVE___ATTRIBUTE__
# define __UNUSED__ __attribute__((unused))
#else
# define __UNUSED__
#endif

typedef struct _Tree_Item Tree_Item;
struct _Tree_Item
{
   Tree_Item *parent;
   Eina_List *children;
   union {
        Ecore_Evas *ee;
        Evas_Object *obj;
   } data;
};
#endif
