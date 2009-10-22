#include "stickies.h"

/* FUNCTIONS WHICH HAVE PROTOTYPES DEFINED IN STICKIES.H.
 */
ESAPI void
_e_sticky_move(E_Sticky *s, int x, int y)
{
   evas_object_move(s->win, x, y);
   s->x = x;
   s->y = y;
}

ESAPI void
_e_sticky_resize(E_Sticky *s, int w, int h)
{
   evas_object_resize(s->win, s->w, s->h);
   s->w = w;
   s->h = h;
}

ESAPI void
_e_sticky_move_resize(E_Sticky *s)
{
   if (!s || !s->win) return;
   evas_object_resize(s->win, s->w, s->h);
   evas_object_move(s->win, s->x, s->y);
}

ESAPI void
_e_sticky_lock_toggle(E_Sticky *s)
{
   s->locked = s->lock_toggle_state;
   elm_entry_editable_set(s->textentry, !s->locked);
}

ESAPI void
_e_sticky_properties_set(E_Sticky *s)
{   
   elm_win_sticky_set(s->win, s->stick);
   s->lock_toggle_state = s->locked;
   if (s->lock_toggle_state)
     edje_object_signal_emit(s->sticky, "estickies,lock_toggle,on", "estickies");
   else
     edje_object_signal_emit(s->sticky, "estickies,lock_toggle,off", "estickies");
   _e_sticky_lock_toggle(s);
   if (ss->composite)
     {
	elm_win_alpha_set(s->win, 1);
	elm_win_shaped_set(s->win, 0);
     }
   else
     {
	elm_win_shaped_set(s->win, 1);
	elm_win_alpha_set(s->win, 0);
     }
}

