#include "stickies.h"

/* FUNCTIONS WHICH HAVE PROTOTYPES DEFINED IN STICKIES.H.
 */
ESAPI E_Sticky *
_e_sticky_new(void)
{  
   E_Sticky *s = E_NEW(1, E_Sticky);

   s->theme = NULL;
   s->stick = EINA_FALSE;
   s->locked = EINA_FALSE;
   _e_sticky_window_add(s);

   return s;
}

ESAPI E_Sticky *
_e_sticky_new_show_append(void)
{
   E_Sticky *s = _e_sticky_new();

   ss->stickies = eina_list_append(ss->stickies, s);
   _e_sticky_properties_set(s);
   _e_sticky_show(s);

   return s;
}

ESAPI void
_e_sticky_destroy(E_Sticky *s)
{
   evas_object_del(s->win);
   E_FREE(s);
}

ESAPI void
_e_sticky_delete(E_Sticky *s)
{
   if (eina_list_count(ss->stickies) == 1)
     {
	ss->stickies = eina_list_remove(ss->stickies, s);
	ecore_event_handler_del(s->evhandler);
	ecore_x_window_free(s->xwin);
	evas_object_del(s->win);
	s = _e_sticky_new();
	ss->stickies = eina_list_append(ss->stickies, s);
	_e_sticky_properties_set(s);
	_e_sticky_show(s);
	return;
     }
   ss->stickies = eina_list_remove(ss->stickies, s);
   ecore_event_handler_del(s->evhandler);
   ecore_x_window_free(s->xwin);
   evas_object_del(s->win);

   if (!ss->stickies || eina_list_count(ss->stickies) == 0)
     elm_exit();
}

