/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "e.h"
#include "trivials.h"
#include <stdarg.h>

#define TILING_DEBUG

void
change_window_border(E_Border *bd, char *bordername)
{
   if (bd->bordername)
     evas_stringshare_del(bd->bordername);
   bd->bordername = evas_stringshare_add(bordername);
   bd->client.border.changed = 1;
   bd->changed = 1;
}

void
move_resize(E_Border *bd, int x, int y, int w, int h)
{
   e_border_move_resize(bd, (bd->zone ? bd->zone->x : 0) + x, (bd->zone ? bd->zone->y : 0) + y, w, h);
}

void
recursively_set_disabled(Evas_Object *obj, int disabled)
{
   if (!obj) return;
   E_Widget_Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), "e_widget"))) return;

   Evas_List *l;
   for (l = sd->subobjs; l; l = l->next)
     {
	Evas_Object *o = l->data;
	if (!o) continue;
	recursively_set_disabled(o, disabled);
     }

   e_widget_disabled_set(obj, disabled);
}

void
DBG(const char *fmt, ...)
{
#ifdef TILING_DEBUG
   va_list ap;
   va_start(ap, fmt);
   char buf[4096];
   snprintf(buf, 4096, "TILING_DEBUG: %s", fmt);
   vprintf(buf, ap);
   va_end(ap);
#endif
}

/* Returns the bigger one. Needed to avoid SIGFPE */
int
max(int a, int b)
{
   return (a < b ? b : a);
}

/* Returns the smaller one *doh* */
int
min(int a, int b)
{
   return (a < b ? a : b);
}

/* Returns true if value is between or equal to minimum and maximum */
int
between(int value, int minimum, int maximum)
{
   return (value >= minimum && value <= maximum);
}

/* I wonder why noone has implemented the following one yet? */
E_Desk*
get_current_desk()
{
   E_Manager *m = e_manager_current_get();
   E_Container *c = e_container_current_get(m);
   E_Zone *z = e_zone_current_get(c);
   return e_desk_current_get(z);
}

/* Returns 1 if the given shelf is visible on the given desk */
int
shelf_show_on_desk(E_Shelf *sh, E_Desk *desk)
{
   if (!sh || !desk) return 0;
   E_Config_Shelf *cf = sh->cfg;
   if (!cf) return 0;
   if (!cf->desk_show_mode) return 1;
   Evas_List *l;
   for (l = cf->desk_list; l; l = l->next)
     {
	E_Config_Shelf_Desk *sd = l->data;
	if (sd && sd->x == desk->x && sd->y == desk->y)
	  return 1;
     }
   return 0;
}


