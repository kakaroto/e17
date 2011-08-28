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
   Eina_List *children;
   const char *name;
   void *ptr;
   Eina_Bool is_obj;
   Eina_Bool is_clipper;
   Eina_Bool is_visible;
};
#endif
