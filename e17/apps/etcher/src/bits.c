#include "bits.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>

static int          __ebits_cache_zero = 0;
static int          __ebits_cache_size = 8;
static Evas_List    __bit_descriptions = NULL;

Ebits_Object_Bit_State _ebits_get_bit_class(Ebits_Object o, char *name);
static Ebits_Object_Description _ebits_find_description(char *file);
static char        *_ebits_get_file(Ebits_Object_Bit_Description d,
				    char *state);

#ifdef EDITOR
static int          _ebits_image_state_saved(Ebits_Object_Bit_State state,
					     char *s);
#endif
static void         _ebits_sync_bits(Ebits_Object_Bit_State state);
static void         _ebits_evaluate_fill(Ebits_Object_Bit_State state);
static void         _ebits_calculate(Ebits_Object_Bit_State state);
static void         _ebits_object_calculate(Ebits_Object o);
static void         _ebits_calculate_min_size(Ebits_Object o);

#define EBITS_FILE_REDIRECT "%s:/images/%s"
#define FREE(ptr) \
	{ \
		free(ptr); \
		ptr = NULL; \
	}
#define IF_FREE(ptr) { if (ptr) FREE(ptr); }

/* #define LENIENT 1 */

Ebits_Object_Bit_State
ebits_get_bit_name(Ebits_Object o, char *name)
{
   Evas_List           l;

   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	if ((state->description->name) && (name))
	  {
	     if (!strcmp(state->description->name, name))
		return state;
	  }
     }
   return NULL;
}

Ebits_Object_Bit_State
_ebits_get_bit_class(Ebits_Object o, char *class)
{
   Evas_List           l;

   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	if ((state->description->class) && (class))
	  {
	     if (!strcmp(state->description->class, class))
		return state;
	  }
     }
   return NULL;
}

static void
_ebits_evaluate_fill(Ebits_Object_Bit_State state)
{
   double              fill_w, fill_h;

   evas_set_image_border(state->o->state.evas, state->object,
			 state->description->border.l,
			 state->description->border.r,
			 state->description->border.t,
			 state->description->border.b);
   fill_w = state->w;
   if (state->description->tile.w == EBITS_FILL_TILE)
     {
	int                 im_w;

	evas_get_image_size(state->o->state.evas, state->object, &im_w, NULL);
	if (im_w > 0)
	   fill_w = im_w;
     }
   else if (state->description->tile.w == EBITS_FILL_FITTED_TILE)
     {
	int                 im_w;

	evas_get_image_size(state->o->state.evas, state->object, &im_w, NULL);
	if (im_w > 0)
	  {
	     int                 num;

	     num = (int)(state->w / (double)im_w);
	     if (num < 1)
		num = 1;
	     fill_w = state->w / (double)num;
	  }
     }
   fill_h = state->h;
   if (state->description->tile.h == EBITS_FILL_TILE)
     {
	int                 im_h;

	evas_get_image_size(state->o->state.evas, state->object, NULL, &im_h);
	if (im_h > 0)
	   fill_h = im_h;
     }
   else if (state->description->tile.h == EBITS_FILL_FITTED_TILE)
     {
	int                 im_h;

	evas_get_image_size(state->o->state.evas, state->object, NULL, &im_h);
	if (im_h > 0)
	  {
	     int                 num;

	     num = (int)(state->h / (double)im_h);
	     if (num < 1)
		num = 1;
	     fill_h = state->h / (double)num;
	  }
     }
   evas_set_image_fill(state->o->state.evas, state->object,
		       0, 0, fill_w, fill_h);
   evas_resize(state->o->state.evas, state->object, state->w, state->h);
}

static void
_ebits_object_calculate(Ebits_Object o)
{
   Evas_List           l;

   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	state->calculated = 0;
     }
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	_ebits_calculate(state);
	_ebits_evaluate_fill(state);
	evas_move(state->o->state.evas, state->object,
		  o->state.x + state->x, o->state.y + state->y);
	evas_resize(state->o->state.evas, state->object, state->w, state->h);
	if (state->func_move)
	   state->func_move(state->func_data,
			    o->state.x + state->x, o->state.y + state->y);
	if (state->func_resize)
	   state->func_resize(state->func_data, state->w, state->h);
     }
}

static void
_ebits_calculate(Ebits_Object_Bit_State state)
{
   int                 x1, y1, x2, y2;
   int                 rx, ry, rw, rh;
   int                 want_w, want_h;
   int                 minw, minh, maxw, maxh;

   if ((state->calculated) || (state->calc_pending))
      return;
   state->calc_pending = 1;
   if (state->description->rel1.name)
     {
	Ebits_Object_Bit_State state2;

	state2 = ebits_get_bit_name(state->o, state->description->rel1.name);
	if (state2)
	  {
	     _ebits_calculate(state2);
	     rx = state2->x;
	     ry = state2->y;
	     rw = state2->w;
	     rh = state2->h;
	  }
	else
	  {
	     rx = 0;
	     ry = 0;
	     rw = state->o->state.w;
	     rh = state->o->state.h;
	  }
     }
   else
     {
	rx = 0;
	ry = 0;
	rw = state->o->state.w;
	rh = state->o->state.h;
     }
   x1 = rx +
      ((rw +
	state->description->rel1.ax) * state->description->rel1.rx) +
      state->description->rel1.x;
   y1 = ry +
      ((rh +
	state->description->rel1.ay) * state->description->rel1.ry) +
      state->description->rel1.y;

   if (state->description->rel2.name)
     {
	Ebits_Object_Bit_State state2;

	state2 = ebits_get_bit_name(state->o, state->description->rel2.name);
	if (state2)
	  {
	     _ebits_calculate(state2);
	     rx = state2->x;
	     ry = state2->y;
	     rw = state2->w;
	     rh = state2->h;
	  }
	else
	  {
	     rx = 0;
	     ry = 0;
	     rw = state->o->state.w;
	     rh = state->o->state.h;
	  }
     }
   else
     {
	rx = 0;
	ry = 0;
	rw = state->o->state.w;
	rh = state->o->state.h;
     }
   x2 = rx +
      ((rw +
	state->description->rel2.ax) * state->description->rel2.rx) +
      state->description->rel2.x;
   y2 = ry +
      ((rh +
	state->description->rel2.ay) * state->description->rel2.ry) +
      state->description->rel2.y;

   state->x = x1;
   state->y = y1;

   state->want_w = want_w = state->w = x2 - x1 + 1;
   state->want_h = want_h = state->h = y2 - y1 + 1;

   state->w =
      (state->w / state->description->step.x) * state->description->step.x;
   state->h =
      (state->h / state->description->step.y) * state->description->step.y;

   if ((state->description->aspect.x > 0) && (state->description->aspect.y > 0))
     {
	int                 hh, ww;
	double              th, tw;

	tw = ((double)state->w / (double)state->description->aspect.x);
	th = ((double)state->h / (double)state->description->aspect.y);

	if (tw < th)
	  {
	     hh = (state->w * state->description->aspect.x) /
		state->description->aspect.y;
	     state->h = hh;
	  }
	else
	  {
	     ww = (state->h * state->description->aspect.x) /
		state->description->aspect.y;
	     state->w = ww;
	  }
     }
   minw = state->description->min.w;
   minh = state->description->min.h;
   maxw = state->description->max.w;
   maxh = state->description->max.h;
   if (state->func_get_min_size)
     {
	double              mw, mh;

	state->func_get_min_size(state->func_data, &mw, &mh);
	minw = mw;
	minh = mh;
     }
   if (state->func_get_max_size)
     {
	double              mw, mh;

	state->func_get_max_size(state->func_data, &mw, &mh);
	maxw = mw;
	maxh = mh;
     }
   if (state->w < minw)
      state->w = minw;
   if (state->h < minh)
      state->h = minh;
   if (state->description->max.w == 0)
     {
	/* if the bit has some other pre-defined size */
     }
   else
     {
	if (state->w > maxw)
	   state->w = maxw;
	state->x -= (state->w - want_w) * state->description->align.w;
     }
   if (state->description->max.h == 0)
     {
	/* if the bit has some other pre-defined size */
     }
   else
     {
	if (state->h > maxh)
	   state->h = maxh;
	state->y -= (state->h - want_h) * state->description->align.h;
     }
   state->calculated = 1;
   state->calc_pending = 0;
}

static void
_ebits_sync_bits(Ebits_Object_Bit_State state)
{
   Evas_List           l;

   state->syncing = 1;
   if (state->object)
     {
	char                buf[4096];
	char               *st = NULL;

#ifdef EDITOR
	if (!_ebits_image_state_saved(state, state->state))
	   snprintf(buf, sizeof(buf), "%s",
		    _ebits_get_file(state->description, state->state));
	else
#endif
	   st = _ebits_get_file(state->description, state->state);
	if (st && strlen(st) > 2)
	   snprintf(buf, sizeof(buf), EBITS_FILE_REDIRECT,
		    state->o->description->file, st);
#ifndef EDITOR
	else
	   strcpy(buf, "");
#endif

	evas_set_image_file(state->o->state.evas, state->object, buf);

	_ebits_evaluate_fill(state);
     }
   for (l = state->description->sync; l; l = l->next)
     {
	Ebits_Object_Bit_State state2;

	state2 = ebits_get_bit_name(state->o, l->data);
	if ((state2) && (state2 != state) && (!state2->syncing))
	  {
	     IF_FREE(state2->state);
	     state2->state = strdup(state->state);
	     _ebits_sync_bits(state2);
	  }
     }
   state->syncing = 0;
}

static void
_ebits_handle_mouse_down(void *_data, Evas _e, Evas_Object _o, int _b,
			 int _x, int _y)
{
   Ebits_Object_Bit_State state;

   state = _data;
   if (!strncasecmp(state->state, "disabled", 8))
      return;
   IF_FREE(state->state);
   state->state = strdup("clicked");
   _ebits_sync_bits(state);
   if (state->callbacks)
     {
	Evas_List           l;

	for (l = state->callbacks; l; l = l->next)
	  {
	     Ebits_Callback      cb;

	     cb = l->data;
	     if (cb->type == CALLBACK_MOUSE_DOWN)
		cb->func(cb->data, state->o,
			 state->description->class, _b, _x, _y,
			 state->o->state.x + state->x,
			 state->o->state.y + state->y, state->w, state->h);
	  }
     }

   return;
   UN(_e);
   UN(_o);
}

static void
_ebits_handle_mouse_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
		       int _y)
{
   Ebits_Object_Bit_State state;

   state = _data;
   if (!strncasecmp(state->state, "disabled", 8))
      return;
   IF_FREE(state->state);
   if (state->mouse_in)
      state->state = strdup("hilited");
   else
      state->state = strdup("normal");
   _ebits_sync_bits(state);
   if (state->callbacks)
     {
	Evas_List           l;

	for (l = state->callbacks; l; l = l->next)
	  {
	     Ebits_Callback      cb;

	     cb = l->data;
	     if (cb->type == CALLBACK_MOUSE_UP)
		cb->func(cb->data, state->o,
			 state->description->class, _b, _x, _y,
			 state->o->state.x + state->x,
			 state->o->state.y + state->y, state->w, state->h);
	  }
     }

   return;
   UN(_e);
   UN(_o);
}

static void
_ebits_handle_mouse_move(void *_data, Evas _e, Evas_Object _o, int _b,
			 int _x, int _y)
{
   Ebits_Object_Bit_State state;

   state = _data;
   if (!strncasecmp(state->state, "disabled", 8))
      return;
   if (state->callbacks)
     {
	Evas_List           l;

	for (l = state->callbacks; l; l = l->next)
	  {
	     Ebits_Callback      cb;

	     cb = l->data;
	     if (cb->type == CALLBACK_MOUSE_MOVE)
		cb->func(cb->data, state->o,
			 state->description->class, _b, _x, _y,
			 state->o->state.x + state->x,
			 state->o->state.y + state->y, state->w, state->h);
	  }
     }

   return;
   UN(_e);
   UN(_o);
}

static void
_ebits_handle_mouse_in(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
		       int _y)
{
   Ebits_Object_Bit_State state;

   state = _data;
   if (!strncasecmp(state->state, "disabled", 8))
      return;
   state->mouse_in = 1;
   if (strncasecmp(state->state, "clicked", 7))
     {
	IF_FREE(state->state);
	state->state = strdup("hilited");
     }
   _ebits_sync_bits(state);
   if (state->callbacks)
     {
	Evas_List           l;

	for (l = state->callbacks; l; l = l->next)
	  {
	     Ebits_Callback      cb;

	     cb = l->data;
	     if (cb->type == CALLBACK_MOUSE_IN)
		cb->func(cb->data, state->o,
			 state->description->class, _b, _x, _y,
			 state->o->state.x + state->x,
			 state->o->state.y + state->y, state->w, state->h);
	  }
     }

   return;
   UN(_e);
   UN(_o);
}

static void
_ebits_handle_mouse_out(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
			int _y)
{
   Ebits_Object_Bit_State state;

   state = _data;
   if (!strncasecmp(state->state, "disabled", 8))
      return;
   state->mouse_in = 0;
   if (strncasecmp(state->state, "clicked", 7))
     {
	IF_FREE(state->state);
	state->state = strdup("normal");
     }
   _ebits_sync_bits(state);
   if (state->callbacks)
     {
	Evas_List           l;

	for (l = state->callbacks; l; l = l->next)
	  {
	     Ebits_Callback      cb;

	     cb = l->data;
	     if (cb->type == CALLBACK_MOUSE_OUT)
		cb->func(cb->data, state->o,
			 state->description->class, _b, _x, _y,
			 state->o->state.x + state->x,
			 state->o->state.y + state->y, state->w, state->h);
	  }
     }

   return;
   UN(_e);
   UN(_o);
}

#ifdef EDITOR

/* XXX */
static int
_ebits_image_state_saved(Ebits_Object_Bit_State state, char *s)
{
   Evas_List           l;
   int                 len;

   if (!s)
      return 0;

   len = strlen(s);

   if (!len)
      return 0;

   if (!strncasecmp(s, "normal", len))
     {
	if (state->description->normal.image)
	   return state->normal.saved;
	if (state->description->hilited.image)
	   return state->hilited.saved;
	if (state->description->clicked.image)
	   return state->clicked.saved;
	if (state->description->disabled.image)
	   return state->disabled.saved;

	for (l = state->state_source_description; l; l = l->next)
	  {
	     Ebits_State_Source_Description ss_d;

	     ss_d = l->data;

	     if (!strncasecmp(s, ss_d->name, len) && ss_d->image)
		return ss_d->saved;
	  }
     }
   else if (!strncasecmp(s, "hilited", len))
     {
	if (state->description->hilited.image)
	   return state->hilited.saved;
	if (state->description->normal.image)
	   return state->normal.saved;
	if (state->description->clicked.image)
	   return state->clicked.saved;
	if (state->description->disabled.image)
	   return state->disabled.saved;

	for (l = state->state_source_description; l; l = l->next)
	  {
	     Ebits_State_Source_Description ss_d;

	     ss_d = l->data;

	     if (!strncasecmp(s, ss_d->name, len) && ss_d->image)
		return ss_d->saved;
	  }
     }
   else if (!strncasecmp(s, "clicked", len))
     {
	if (state->description->clicked.image)
	   return state->clicked.saved;
	if (state->description->hilited.image)
	   return state->hilited.saved;
	if (state->description->normal.image)
	   return state->normal.saved;
	if (state->description->disabled.image)
	   return state->disabled.saved;

	for (l = state->state_source_description; l; l = l->next)
	  {
	     Ebits_State_Source_Description ss_d;

	     ss_d = l->data;

	     if (!strncasecmp(s, ss_d->name, len) && ss_d->image)
		return ss_d->saved;
	  }
     }
   else if (!strncasecmp(s, "disabled", len))
     {
	if (state->description->disabled.image)
	   return state->disabled.saved;
	if (state->description->normal.image)
	   return state->normal.saved;
	if (state->description->hilited.image)
	   return state->hilited.saved;
	if (state->description->clicked.image)
	   return state->clicked.saved;

	for (l = state->state_source_description; l; l = l->next)
	  {
	     Ebits_State_Source_Description ss_d;

	     ss_d = l->data;

	     if (!strncasecmp(s, ss_d->name, len) && ss_d->image)
		return ss_d->saved;
	  }
     }
   else
     {
	for (l = state->state_source_description; l; l = l->next)
	  {
	     Ebits_State_Source_Description ss_d;

	     ss_d = l->data;

	     if (!strncasecmp(s, ss_d->name, len) && ss_d->image)
		return ss_d->saved;
	  }

	if (state->description->normal.image)
	   return state->normal.saved;
	if (state->description->hilited.image)
	   return state->hilited.saved;
	if (state->description->clicked.image)
	   return state->clicked.saved;
	if (state->description->disabled.image)
	   return state->disabled.saved;
     }

   return 0;
}
#endif

/* XXX */
static char        *
_ebits_get_file(Ebits_Object_Bit_Description d, char *state)
{
   int                 len, len2;

   if (!state)
      return "";

   len = strlen(state);

   if (!len)
      return "";

   if (!strncasecmp(state, "normal", len))
     {
	Evas_List           l;

	if (d->normal.image)
	   return d->normal.image;
	if (d->hilited.image)
	   return d->hilited.image;
	if (d->clicked.image)
	   return d->clicked.image;
	if (d->disabled.image)
	   return d->disabled.image;

	for (l = d->state_description; l; l = l->next)
	  {
	     Ebits_State_Description state_d;

	     state_d = l->data;

	     len2 = strlen(state_d->name);

	     if (len != len2)
		continue;

	     if (!strncasecmp(state, state_d->name, len))
		return state_d->image;
	  }
     }
   else if (!strncasecmp(state, "hilited", len))
     {
	Evas_List           l;

	if (d->hilited.image)
	   return d->hilited.image;
	if (d->normal.image)
	   return d->normal.image;
	if (d->clicked.image)
	   return d->clicked.image;
	if (d->disabled.image)
	   return d->disabled.image;

	for (l = d->state_description; l; l = l->next)
	  {
	     Ebits_State_Description state_d;

	     state_d = l->data;

	     len2 = strlen(state_d->name);

	     if (len != len2)
		continue;

	     if (!strncasecmp(state, state_d->name, len))
		return state_d->image;
	  }
     }
   else if (!strncasecmp(state, "clicked", len))
     {
	Evas_List           l;

	if (d->clicked.image)
	   return d->clicked.image;
	if (d->hilited.image)
	   return d->hilited.image;
	if (d->normal.image)
	   return d->normal.image;
	if (d->disabled.image)
	   return d->disabled.image;

	for (l = d->state_description; l; l = l->next)
	  {
	     Ebits_State_Description state_d;

	     state_d = l->data;

	     len2 = strlen(state_d->name);

	     if (len != len2)
		continue;

	     if (!strncasecmp(state, state_d->name, len))
		return state_d->image;
	  }
     }
   else if (!strncasecmp(state, "disabled", len))
     {
	Evas_List           l;

	if (d->disabled.image)
	   return d->disabled.image;
	if (d->normal.image)
	   return d->normal.image;
	if (d->hilited.image)
	   return d->hilited.image;
	if (d->clicked.image)
	   return d->clicked.image;

	for (l = d->state_description; l; l = l->next)
	  {
	     Ebits_State_Description state_d;

	     state_d = l->data;

	     len2 = strlen(state_d->name);

	     if (len != len2)
		continue;

	     if (!strncasecmp(state, state_d->name, len))
		return state_d->image;
	  }
     }
   else
     {
	Evas_List           l;

	for (l = d->state_description; l; l = l->next)
	  {
	     Ebits_State_Description state_d;

	     state_d = l->data;

	     len2 = strlen(state_d->name);

	     if (len != len2)
		continue;

	     if (!strncasecmp(state, state_d->name, len))
		return state_d->image;
	  }

	if (d->normal.image)
	   return d->normal.image;
	if (d->hilited.image)
	   return d->hilited.image;
	if (d->clicked.image)
	   return d->clicked.image;
	if (d->disabled.image)
	   return d->disabled.image;
     }

   return "";
}

static              Ebits_Object_Description
_ebits_find_description(char *file)
{
   Ebits_Object_Description d = NULL;
   Evas_List           l;
   E_DB_File          *db;
   int                 version;

   /* find bit description in cache */
   for (l = __bit_descriptions; l; l = l->next)
     {
	d = l->data;
	if (!strcmp(d->file, file))
	  {
	     if (l != __bit_descriptions)
	       {
		  __bit_descriptions = evas_list_remove(__bit_descriptions, d);
		  __bit_descriptions = evas_list_prepend(__bit_descriptions, d);
	       }
	     if (d->references <= 0)
		__ebits_cache_zero--;
	     d->references++;
	     return d;
	  }
     }
   /* open db */
   db = e_db_open_read(file);
   /* no db - return NULL */
   if (!db)
      return NULL;

   /* check version of file (and magic key/number) */
   version = 0;
   e_db_int_get(db, "/type/bits", &(version));
   /* got either no key or version != 1 */
   if (version != 1)
     {
	e_db_close(db);
	return NULL;
     }
   /* new description */
   d = malloc(sizeof(struct _Ebits_Object_Description));
   d = memset(d, 0, sizeof(struct _Ebits_Object_Description));
   __bit_descriptions = evas_list_prepend(__bit_descriptions, d);
   d->file = malloc(strlen(file) + 1);
   strcpy(d->file, file);
   d->references = 1;

   d->min.w = 0;
   d->min.h = 0;
   d->max.w = 999999;
   d->max.h = 999999;
   d->step.x = 1;
   d->step.y = 1;

   /* basic bit info */
   e_db_int_get(db, "/base/min/w", &(d->min.w));
   e_db_int_get(db, "/base/min/h", &(d->min.h));

   e_db_int_get(db, "/base/max/w", &(d->max.w));
   e_db_int_get(db, "/base/max/h", &(d->max.h));

   e_db_int_get(db, "/base/step/x", &(d->step.x));
   e_db_int_get(db, "/base/step/y", &(d->step.y));

   e_db_int_get(db, "/base/padding/l", &(d->padding.l));
   e_db_int_get(db, "/base/padding/r", &(d->padding.r));
   e_db_int_get(db, "/base/padding/t", &(d->padding.t));
   e_db_int_get(db, "/base/padding/b", &(d->padding.b));

   e_db_int_get(db, "/base/inset/l", &(d->inset.l));
   e_db_int_get(db, "/base/inset/r", &(d->inset.r));
   e_db_int_get(db, "/base/inset/t", &(d->inset.t));
   e_db_int_get(db, "/base/inset/b", &(d->inset.b));

   if (d->step.x <= 0)
      d->step.x = 1;
   if (d->step.y <= 0)
      d->step.y = 1;

   /* XXX */
   {
      int                 i, num_states = 0;

      e_db_int_get(db, "/base/states/count", &(num_states));

      for (i = 0; i < num_states; i++)
	{
	   char                key[4096];
	   char               *name;

	   snprintf(key, 4096, "/base/states/%i/name", i);
	   name = e_db_str_get(db, key);

	   d->state_names = evas_list_append(d->state_names, name);
	}

   }

   /* all the bits */
   {
      int                 num_bits = 0, i;

      e_db_int_get(db, "/bits/count", &(num_bits));
      for (i = 0; i < num_bits; i++)
	{
	   Ebits_Object_Bit_Description bit;
	   char                key[4096];
	   float               f;
	   int                 num_sync, j;

	   bit = malloc(sizeof(struct _Ebits_Object_Bit_Description));
	   bit = memset(bit, 0, sizeof(struct _Ebits_Object_Bit_Description));

	   snprintf(key, sizeof(key), "/bits/bit/%i/name", i);
	   bit->name = e_db_str_get(db, key);
	   snprintf(key, sizeof(key), "/bits/bit/%i/class", i);
	   bit->class = e_db_str_get(db, key);
	   snprintf(key, sizeof(key), "/bits/bit/%i/color_class", i);
	   bit->color_class = e_db_str_get(db, key);
	   snprintf(key, sizeof(key), "/bits/bit/%i/normal/image", i);
	   bit->normal.image = e_db_str_get(db, key);
	   snprintf(key, sizeof(key), "/bits/bit/%i/hilited/image", i);
	   bit->hilited.image = e_db_str_get(db, key);
	   snprintf(key, sizeof(key), "/bits/bit/%i/clicked/image", i);
	   bit->clicked.image = e_db_str_get(db, key);
	   snprintf(key, sizeof(key), "/bits/bit/%i/disabled/image", i);
	   bit->disabled.image = e_db_str_get(db, key);

	   snprintf(key, sizeof(key), "/bits/bit/%i/border/l", i);
	   e_db_int_get(db, key, &(bit->border.l));
	   snprintf(key, sizeof(key), "/bits/bit/%i/border/r", i);
	   e_db_int_get(db, key, &(bit->border.r));
	   snprintf(key, sizeof(key), "/bits/bit/%i/border/t", i);
	   e_db_int_get(db, key, &(bit->border.t));
	   snprintf(key, sizeof(key), "/bits/bit/%i/border/b", i);
	   e_db_int_get(db, key, &(bit->border.b));

	   snprintf(key, sizeof(key), "/bits/bit/%i/tile/w", i);
	   e_db_int_get(db, key, (int *)&(bit->tile.w));
	   snprintf(key, sizeof(key), "/bits/bit/%i/tile/h", i);
	   e_db_int_get(db, key, (int *)&(bit->tile.h));

	   snprintf(key, sizeof(key), "/bits/bit/%i/rel1/name", i);
	   bit->rel1.name = e_db_str_get(db, key);
	   snprintf(key, sizeof(key), "/bits/bit/%i/rel1/x", i);
	   e_db_int_get(db, key, &(bit->rel1.x));
	   snprintf(key, sizeof(key), "/bits/bit/%i/rel1/y", i);
	   e_db_int_get(db, key, &(bit->rel1.y));
	   snprintf(key, sizeof(key), "/bits/bit/%i/rel1/rx", i);
	   e_db_float_get(db, key, &(f));
	   bit->rel1.rx = f;
	   snprintf(key, sizeof(key), "/bits/bit/%i/rel1/ry", i);
	   e_db_float_get(db, key, &(f));
	   bit->rel1.ry = f;
	   snprintf(key, sizeof(key), "/bits/bit/%i/rel1/ax", i);
	   e_db_int_get(db, key, &(bit->rel1.ax));
	   snprintf(key, sizeof(key), "/bits/bit/%i/rel1/ay", i);
	   e_db_int_get(db, key, &(bit->rel1.ay));

	   snprintf(key, sizeof(key), "/bits/bit/%i/rel2/name", i);
	   bit->rel2.name = e_db_str_get(db, key);
	   snprintf(key, sizeof(key), "/bits/bit/%i/rel2/x", i);
	   e_db_int_get(db, key, &(bit->rel2.x));
	   snprintf(key, sizeof(key), "/bits/bit/%i/rel2/y", i);
	   e_db_int_get(db, key, &(bit->rel2.y));
	   snprintf(key, sizeof(key), "/bits/bit/%i/rel2/rx", i);
	   e_db_float_get(db, key, &(f));
	   bit->rel2.rx = f;
	   snprintf(key, sizeof(key), "/bits/bit/%i/rel2/ry", i);
	   e_db_float_get(db, key, &(f));
	   bit->rel2.ry = f;
	   snprintf(key, sizeof(key), "/bits/bit/%i/rel2/ax", i);
	   e_db_int_get(db, key, &(bit->rel2.ax));
	   snprintf(key, sizeof(key), "/bits/bit/%i/rel2/ay", i);
	   e_db_int_get(db, key, &(bit->rel2.ay));

	   snprintf(key, sizeof(key), "/bits/bit/%i/align/w", i);
	   e_db_float_get(db, key, &(f));
	   bit->align.w = f;
	   snprintf(key, sizeof(key), "/bits/bit/%i/align/h", i);
	   e_db_float_get(db, key, &(f));
	   bit->align.h = f;

	   snprintf(key, sizeof(key), "/bits/bit/%i/aspect/x", i);
	   e_db_int_get(db, key, &(bit->aspect.x));
	   snprintf(key, sizeof(key), "/bits/bit/%i/aspect/y", i);
	   e_db_int_get(db, key, &(bit->aspect.y));
	   snprintf(key, sizeof(key), "/bits/bit/%i/step/x", i);
	   e_db_int_get(db, key, &(bit->step.x));
	   snprintf(key, sizeof(key), "/bits/bit/%i/step/y", i);
	   e_db_int_get(db, key, &(bit->step.y));

	   snprintf(key, sizeof(key), "/bits/bit/%i/min/w", i);
	   e_db_int_get(db, key, &(bit->min.w));
	   snprintf(key, sizeof(key), "/bits/bit/%i/min/h", i);
	   e_db_int_get(db, key, &(bit->min.h));
	   snprintf(key, sizeof(key), "/bits/bit/%i/max/w", i);
	   e_db_int_get(db, key, &(bit->max.w));
	   snprintf(key, sizeof(key), "/bits/bit/%i/max/h", i);
	   e_db_int_get(db, key, &(bit->max.h));

	   num_sync = 0;
	   snprintf(key, sizeof(key), "/bits/bit/%i/sync/count", i);
	   e_db_int_get(db, key, &(num_sync));

	   if (bit->step.x <= 0)
	      bit->step.x = 1;
	   if (bit->step.y <= 0)
	      bit->step.y = 1;

	   for (j = 0; j < num_sync; j++)
	     {
		char               *s;

		snprintf(key, sizeof(key), "/bits/bit/%i/sync/%i", i, j);
		s = e_db_str_get(db, key);
		if (s)
		   bit->sync = evas_list_append(bit->sync, s);
	     }

	   {
	      Ebits_State_Description state_d;

	      /* XXX */
	      for (l = d->state_names; l; l = l->next)
		{
		   state_d = malloc(sizeof(struct _Ebits_State_Description));
		   state_d =
		      memset(state_d, 0,
			     sizeof(struct _Ebits_State_Description));

		   state_d->name = strdup(l->data);
		   snprintf(key, sizeof(key),
			    "/bits/bit/%i/%s/image", i, state_d->name);

		   state_d->image = e_db_str_get(db, key);

		   if (!state_d->image)
		     {
			FREE(state_d->name);
			FREE(state_d);
			continue;
		     }

		   bit->state_description =
		      evas_list_append(bit->state_description, state_d);
		}
	   }

	   d->bits = evas_list_append(d->bits, bit);
	}
   }

   e_db_close(db);
   return d;
}

#ifdef EDITOR

void
ebits_set_state(Ebits_Object o, char *st)
{
   Evas_List           l;

   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;

	IF_FREE(state->state);

	state->state = strdup(st);

	_ebits_sync_bits(state);
     }
}

Ebits_Object_Description
ebits_new_description(void)
{
   Ebits_Object_Description d = NULL;

   /* new description */
   d = malloc(sizeof(struct _Ebits_Object_Description));
   d = memset(d, 0, sizeof(struct _Ebits_Object_Description));
   d->step.x = 1;
   d->step.y = 1;
   d->min.w = 0;
   d->min.h = 0;
   d->max.w = 99999;
   d->max.h = 99999;
   d->references = 1;
   return d;
}

void
_ebits_evaluate(Ebits_Object_Bit_State state)
{
   Evas_List           l;

   if (state->description->normal.image && !state->normal.image)
      state->normal.image = imlib_load_image(state->description->normal.image);
   if (state->description->hilited.image && !state->hilited.image)
      state->hilited.image =
	 imlib_load_image(state->description->hilited.image);
   if (state->description->clicked.image && !state->clicked.image)
      state->clicked.image =
	 imlib_load_image(state->description->clicked.image);
   if (state->description->disabled.image && !state->disabled.image)
      state->disabled.image =
	 imlib_load_image(state->description->disabled.image);

   /* XXX */
   for (l = state->description->state_description; l; l = l->next)
     {
	Ebits_State_Description state_d;
	int                 np = 0;

	state_d = l->data;

	if (!state_d->ss_d)
	  {
	     np = 1;
	     state_d->ss_d =
		malloc(sizeof(struct _Ebits_State_Source_Description));
	     state_d->ss_d =
		memset(state_d->ss_d, 0,
		       sizeof(struct _Ebits_State_Source_Description));
	  }

	IF_FREE(state_d->ss_d->name);

	state_d->ss_d->name = strdup(state_d->name);
	state_d->ss_d->image = imlib_load_image(state_d->image);
	state_d->ss_d->state_d = state_d;

	if (!np)
	   state->state_source_description =
	      evas_list_append(state->state_source_description, state_d->ss_d);
     }

   _ebits_object_calculate(state->o);
}

void
ebits_del_bit(Ebits_Object o, Ebits_Object_Bit_State state)
{
   Evas_List           l;

   if (state->object)
      evas_del_object(o->state.evas, state->object);
   if (state->normal.image)
     {
	imlib_context_set_image(state->normal.image);
	imlib_free_image_and_decache();
     }
   if (state->hilited.image)
     {
	imlib_context_set_image(state->hilited.image);
	imlib_free_image_and_decache();
     }
   if (state->clicked.image)
     {
	imlib_context_set_image(state->clicked.image);
	imlib_free_image_and_decache();
     }
   if (state->disabled.image)
     {
	imlib_context_set_image(state->disabled.image);
	imlib_free_image_and_decache();
     }
   /* XXX */
   for (l = state->state_source_description; l; l = l->next)
     {
	Ebits_State_Source_Description ss_d;

	ss_d = l->data;

	imlib_context_set_image(ss_d->image);
	imlib_free_image_and_decache();

	FREE(ss_d->name);
	FREE(ss_d);
     }

   evas_list_free(state->state_source_description);

   IF_FREE(state->description->name);
   IF_FREE(state->description->class);
   IF_FREE(state->description->color_class);
   IF_FREE(state->description->normal.image);
   IF_FREE(state->description->hilited.image);
   IF_FREE(state->description->clicked.image);
   IF_FREE(state->description->disabled.image);

   /* XXX */
   for (l = state->description->state_description; l; l = l->next)
     {
	Ebits_State_Description state_d;

	state_d = l->data;

	FREE(state_d->name);
	FREE(state_d->image);
	FREE(state_d);
     }

   evas_list_free(state->description->state_description);

   IF_FREE(state->description->rel1.name);
   IF_FREE(state->description->rel2.name);
   for (l = state->description->sync; l; l = l->next)
      FREE(l->data);
   evas_list_free(state->description->sync);
   o->bits = evas_list_remove(o->bits, state);
   o->description->bits =
      evas_list_remove(o->description->bits, state->description);
   FREE(state->description);
   FREE(state);
}

Ebits_Object_Bit_State
ebits_new_bit(Ebits_Object o, char *file)
{
   Ebits_Object_Bit_Description bit;
   Ebits_Object_Bit_State state;

   bit = malloc(sizeof(struct _Ebits_Object_Bit_Description));
   bit = memset(bit, 0, sizeof(struct _Ebits_Object_Bit_Description));
   state = malloc(sizeof(struct _Ebits_Object_Bit_State));
   state = memset(state, 0, sizeof(struct _Ebits_Object_Bit_State));

   o->bits = evas_list_append(o->bits, state);
   o->description->bits = evas_list_append(o->description->bits, bit);
   state->description = bit;
   state->o = o;

   state->r = 255;
   state->g = 255;
   state->b = 255;
   state->a = 255;

   state->state = strdup("normal");

   bit->normal.image = malloc(strlen(file) + 1);
   strcpy(bit->normal.image, file);
   state->object =
      evas_add_image_from_file(o->state.evas,
			       _ebits_get_file(state->description,
					       state->state));
   {
      evas_callback_add(o->state.evas, state->object,
			CALLBACK_MOUSE_DOWN, _ebits_handle_mouse_down, state);
      evas_callback_add(o->state.evas, state->object,
			CALLBACK_MOUSE_UP, _ebits_handle_mouse_up, state);
      evas_callback_add(o->state.evas, state->object,
			CALLBACK_MOUSE_MOVE, _ebits_handle_mouse_move, state);
      evas_callback_add(o->state.evas, state->object,
			CALLBACK_MOUSE_IN, _ebits_handle_mouse_in, state);
      evas_callback_add(o->state.evas, state->object,
			CALLBACK_MOUSE_OUT, _ebits_handle_mouse_out, state);
   }
   if ((o->state.evas) && (state->object))
      evas_set_color(o->state.evas, state->object,
		     state->r, state->g, state->b, state->a);

   evas_set_layer(o->state.evas, state->object, o->state.layer);
   if (o->state.visible)
      evas_show(o->state.evas, state->object);

   {
      int                 w, h;

      evas_get_image_size(o->state.evas, state->object, &w, &h);
      bit->rel1.x = 0;
      bit->rel1.y = 0;
      bit->rel1.rx = 0.0;
      bit->rel1.ry = 0.0;
      bit->rel1.ax = 0;
      bit->rel1.ay = 0;
      bit->rel2.x = -1;
      bit->rel2.y = -1;
      bit->rel2.rx = 1.0;
      bit->rel2.ry = 1.0;
      bit->rel2.ax = 0;
      bit->rel2.ay = 0;
      bit->align.w = 0.5;
      bit->align.h = 0.5;
      bit->step.x = 1;
      bit->step.y = 1;
      bit->min.w = 0;
      bit->min.h = 0;
      bit->max.w = 99999;
      bit->max.h = 99999;
   }

   _ebits_evaluate(state);

   return state;
}

#endif

static void
_ebits_cache_flush(void)
{
   while (__ebits_cache_zero > __ebits_cache_size)
     {
	Evas_List           l;
	Ebits_Object_Description d, del;

	del = NULL;
	for (l = __bit_descriptions; l; l = l->next)
	  {
	     d = l->data;
	     if (d->references == 0)
		del = d;
	  }
	d = del;
	if (d)
	  {
	     __ebits_cache_zero--;
	     IF_FREE(d->file);
	     __bit_descriptions = evas_list_remove(__bit_descriptions, d);
	     if (d->bits)
	       {
		  for (l = d->bits; l; l = l->next)
		    {
		       Ebits_Object_Bit_Description bit;
		       Evas_List           ll;

		       bit = l->data;
		       IF_FREE(bit->name);
		       IF_FREE(bit->class);
		       IF_FREE(bit->color_class);
		       IF_FREE(bit->normal.image);
		       IF_FREE(bit->hilited.image);
		       IF_FREE(bit->clicked.image);
		       IF_FREE(bit->disabled.image);

		       /* XXX */
		       for (ll = bit->state_description; ll; ll = ll->next)
			 {
			    Ebits_State_Description state_d;

			    state_d = ll->data;

			    FREE(state_d->name);
			    FREE(state_d->image);
			    FREE(state_d);
			 }
		       evas_list_free(bit->state_description);

		       IF_FREE(bit->rel1.name);
		       IF_FREE(bit->rel2.name);
		       for (ll = bit->sync; ll; ll = ll->next)
			  FREE(ll->data);
		       evas_list_free(bit->sync);
		    }
		  evas_list_free(d->bits);
	       }
	     FREE(d);
	  }
     }
}

static void
_ebits_calculate_min_size(Ebits_Object o)
{
   int                 pw, ph;
   Evas_List           l;
   Ebits_Object_Bit_State state;
   int                 maxw, maxh, mw, mh;

   pw = o->state.w;
   ph = o->state.h;
   mw = maxw = 0;
   mh = maxh = 0;

   do
     {
	mw += maxw;
	mh += maxh;
	o->state.w = mw;
	o->state.h = mh;

	for (l = o->bits; l; l = l->next)
	  {
	     state = l->data;
	     state->calculated = 0;
	  }
	for (l = o->bits; l; l = l->next)
	  {
	     int                 w, h;

	     state = l->data;
	     _ebits_calculate(state);
	     w = state->w - state->want_w;
	     h = state->h - state->want_h;
	     if ((w > 0) && (w > maxw))
		maxw = w;
	     if ((h > 0) && (h > maxh))
		maxh = h;
	  }
     }
   while ((maxw != mw) || (maxh != mh));

   o->state.w = pw;
   o->state.h = ph;
   for (l = o->bits; l; l = l->next)
     {
	state = l->data;
	state->calculated = 0;
     }
   for (l = o->bits; l; l = l->next)
     {
	state = l->data;
	_ebits_calculate(state);
     }

   o->description->real_min_size.w = maxw;
   o->description->real_min_size.h = maxh;
   o->description->real_min_size.caculated = 1;
}

void
ebits_set_cache(int count)
{
   __ebits_cache_size = count;
   _ebits_cache_flush();
}

int
ebits_get_cache(void)
{
   return __ebits_cache_size;
}

void
ebits_flush(void)
{
   int                 c;

   c = ebits_get_cache();
   ebits_set_cache(0);
   ebits_set_cache(c);
}

Ebits_Object
ebits_load(char *file)
{
   Ebits_Object        o;
   Ebits_Object_Description d;
   Evas_List           l;
   char                realf[PATH_MAX];

   if (!realpath(file, realf))
      strcpy(realf, file);
   d = _ebits_find_description(realf);
   if (!d)
      return NULL;

   o = ebits_new();
   o->description = d;

   for (l = d->bits; l; l = l->next)
     {
	Ebits_Object_Bit_Description bit;
	Ebits_Object_Bit_State state;

#ifdef EDITOR
	Evas_List           ll;
	char                image[4096];
#endif

	bit = l->data;
	state = malloc(sizeof(struct _Ebits_Object_Bit_State));
	state = memset(state, 0, sizeof(struct _Ebits_Object_Bit_State));

	state->r = 255;
	state->g = 255;
	state->b = 255;
	state->a = 255;

	state->state = strdup("normal");

	o->bits = evas_list_append(o->bits, state);
	state->o = o;
	state->description = bit;
#ifdef EDITOR
	if (bit->normal.image)
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT,
		      realf, bit->normal.image);
	     state->normal.image = imlib_load_image(image);
#ifdef LENIENT
	     if (!state->normal.image)
		state->normal.image = imlib_load_image(bit->normal.image);
#endif
	  }
	if (bit->hilited.image)
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT,
		      realf, bit->hilited.image);
	     state->hilited.image = imlib_load_image(image);
#ifdef LENIENT
	     if (!state->hilited.image)
		state->hilited.image = imlib_load_image(bit->hilited.image);
#endif
	  }
	if (bit->clicked.image)
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT,
		      realf, bit->clicked.image);
	     state->clicked.image = imlib_load_image(image);
#ifdef LENIENT
	     if (!state->clicked.image)
		state->clicked.image = imlib_load_image(bit->clicked.image);
#endif
	  }
	if (bit->disabled.image)
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT,
		      realf, bit->disabled.image);
	     state->disabled.image = imlib_load_image(image);
#ifdef LENIENT
	     if (!state->disabled.image)
		state->disabled.image = imlib_load_image(bit->disabled.image);
#endif
	  }
	/* XXX */
	for (ll = bit->state_description; ll; ll = ll->next)
	  {
	     Ebits_State_Description state_d;
	     Ebits_State_Source_Description ss_d;

	     state_d = ll->data;

	     ss_d = malloc(sizeof(struct _Ebits_State_Source_Description));
	     ss_d = memset(ss_d, 0,
			   sizeof(struct _Ebits_State_Source_Description));

	     ss_d->name = strdup(state_d->name);
	     ss_d->state_d = state_d;

	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT,
		      realf, state_d->image);
	     ss_d->image = imlib_load_image(image);

#ifdef LENIENT
	     if (!ss_d->image)
		ss_d->image = imlib_load_image(state_d->image);
#endif
	     if (!ss_d->image)
	       {
		  FREE(ss_d->name);
		  FREE(ss_d);
	       }
	     else
		state->state_source_description =
		   evas_list_append(state->state_source_description, ss_d);

	  }
#endif
     }
   return o;
}

void
ebits_free(Ebits_Object o)
{
   o->description->references--;
   if (o->description->references <= 0)
      __ebits_cache_zero++;
   _ebits_cache_flush();
   if (o->bits)
     {
	Evas_List           l;

	for (l = o->bits; l; l = l->next)
	  {
	     Ebits_Object_Bit_State state;
	     Evas_List           ll;

	     state = l->data;
	     if ((state->object) && (o->state.evas))
		evas_del_object(o->state.evas, state->object);
	     if (state->callbacks)
	       {
		  for (ll = state->callbacks; ll; ll = ll->next)
		     FREE(ll->data);
		  evas_list_free(state->callbacks);
	       }
#ifdef EDITOR
	     if (state->normal.image)
	       {
		  imlib_context_set_image(state->normal.image);
		  imlib_free_image_and_decache();
	       }
	     if (state->hilited.image)
	       {
		  imlib_context_set_image(state->hilited.image);
		  imlib_free_image_and_decache();
	       }
	     if (state->clicked.image)
	       {
		  imlib_context_set_image(state->clicked.image);
		  imlib_free_image_and_decache();
	       }
	     if (state->disabled.image)
	       {
		  imlib_context_set_image(state->disabled.image);
		  imlib_free_image_and_decache();
	       }
	     /* XXX */
	     for (ll = state->state_source_description; ll; ll = ll->next)
	       {
		  Ebits_State_Source_Description ss_d;

		  ss_d = ll->data;

		  imlib_context_set_image(ss_d->image);
		  imlib_free_image_and_decache();

		  FREE(ss_d->name);
		  FREE(ss_d);
	       }

	     evas_list_free(state->state_source_description);
#endif
	     FREE(state);
	  }
	evas_list_free(o->bits);
     }
   FREE(o);
}

void
ebits_add_to_evas(Ebits_Object o, Evas e)
{
   Evas_List           l;

   o->state.evas = e;
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;
	Evas_List           ll;
	char                buf[4096];
	char               *st;

	state = l->data;
	st = _ebits_get_file(state->description, state->state);
	if (strlen(st) > 0)
	  {
	     snprintf(buf, sizeof(buf), EBITS_FILE_REDIRECT,
		      state->o->description->file, st);
	  }
	else
	   strcpy(buf, "");
	if ((state->description->normal.image) &&
	    (!strcmp(state->description->normal.image, "-")))
	  {
	     state->object = evas_add_rectangle(o->state.evas);
	     if ((state->description->class) &&
		 (!strcmp(state->description->class, "Decoration")))
		evas_set_pass_events(o->state.evas, state->object, 1);
	     evas_set_color(o->state.evas, state->object, 255, 255, 255, 0);
	  }
	else
	  {
	     state->object = evas_add_image_from_file(o->state.evas, buf);
	     if ((state->description->class) &&
		 (!strcmp(state->description->class, "Decoration")))
		evas_set_pass_events(o->state.evas, state->object, 1);
	  }
	if ((state->o->state.evas) && (state->object))
	   evas_set_color(state->o->state.evas, state->object,
			  state->r, state->g, state->b, state->a);
#ifdef EDITOR
	if (state->normal.image)
	   state->normal.saved = 1;
	if (state->hilited.image)
	   state->hilited.saved = 1;
	if (state->clicked.image)
	   state->clicked.saved = 1;
	if (state->disabled.image)
	   state->disabled.saved = 1;
	for (ll = state->state_source_description; ll; ll = ll->next)
	  {
	     Ebits_State_Source_Description ss_d;

	     ss_d = ll->data;

	     ss_d->saved = 1;
	  }
#endif
	evas_callback_add(o->state.evas, state->object,
			  CALLBACK_MOUSE_DOWN, _ebits_handle_mouse_down, state);
	evas_callback_add(o->state.evas, state->object,
			  CALLBACK_MOUSE_UP, _ebits_handle_mouse_up, state);
	evas_callback_add(o->state.evas, state->object,
			  CALLBACK_MOUSE_MOVE, _ebits_handle_mouse_move, state);
	evas_callback_add(o->state.evas, state->object,
			  CALLBACK_MOUSE_IN, _ebits_handle_mouse_in, state);
	evas_callback_add(o->state.evas, state->object,
			  CALLBACK_MOUSE_OUT, _ebits_handle_mouse_out, state);
     }
}

void
ebits_show(Ebits_Object o)
{
   Evas_List           l;

   if (o->state.visible)
      return;
   o->state.visible = 1;
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	_ebits_sync_bits(state);
	evas_show(state->o->state.evas, state->object);
	if (state->func_show)
	   state->func_show(state->func_data);
     }
}

void
ebits_hide(Ebits_Object o)
{
   Evas_List           l;

   if (!o->state.visible)
      return;
   o->state.visible = 0;
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	evas_hide(state->o->state.evas, state->object);
	if (state->func_hide)
	   state->func_hide(state->func_data);
     }
}

void
ebits_set_layer(Ebits_Object o, int layer)
{
   Evas_List           l;

   if (layer == o->state.layer)
      return;
   o->state.layer = layer;
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	evas_set_layer(state->o->state.evas, state->object, layer);
	if (state->func_set_layer)
	   state->func_set_layer(state->func_data, layer);
     }
}

void
ebits_raise(Ebits_Object o)
{
   Evas_List           l;

   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	evas_raise(state->o->state.evas, state->object);
	if (state->func_raise)
	   state->func_raise(state->func_data);
     }
}

void
ebits_lower(Ebits_Object o)
{
   Evas_List           l;

   for (l = o->bits; l->next; l = l->next);
   for (; l; l = l->prev)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	evas_lower(state->o->state.evas, state->object);
	if (state->func_lower)
	   state->func_lower(state->func_data);
     }
}

void
ebits_move(Ebits_Object o, double x, double y)
{
   Evas_List           l;

   o->state.x = x;
   o->state.y = y;
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	if (state->func_move)
	   state->func_move(state->func_data,
			    o->state.x + state->x, o->state.y + state->y);
	evas_move(state->o->state.evas, state->object,
		  o->state.x + state->x, o->state.y + state->y);
     }
}

void
ebits_set_clip(Ebits_Object o, Evas_Object clip)
{
   Evas_List           l;

   o->state.clip = clip;
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	evas_set_clip(state->o->state.evas, state->object, clip);
	if (state->func_set_clip)
	   state->func_set_clip(state->func_data, clip);
     }
}

void
ebits_unset_clip(Ebits_Object o)
{
   Evas_List           l;

   o->state.clip = NULL;
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	evas_unset_clip(state->o->state.evas, state->object);
	if (state->func_set_clip)
	   state->func_set_clip(state->func_data, NULL);
     }
}

void
ebits_resize(Ebits_Object o, double w, double h)
{
   w = (double)(((int)w / o->description->step.x) * o->description->step.x);
   h = (double)(((int)h / o->description->step.y) * o->description->step.y);
   if (w > o->description->max.w)
      w = o->description->max.w;
   else if (w < o->description->min.w)
      w = o->description->min.w;
   if (h > o->description->max.h)
      h = o->description->max.h;
   else if (h < o->description->min.h)
      h = o->description->min.h;
   o->state.w = w;
   o->state.h = h;
   _ebits_object_calculate(o);
}

void
ebits_get_padding(Ebits_Object o, int *l, int *r, int *t, int *b)
{
   if (l)
      *l = o->description->padding.l;
   if (r)
      *r = o->description->padding.r;
   if (t)
      *t = o->description->padding.t;
   if (b)
      *b = o->description->padding.b;
}

void
ebits_get_insets(Ebits_Object o, int *l, int *r, int *t, int *b)
{
   if (l)
      *l = o->description->inset.l;
   if (r)
      *r = o->description->inset.r;
   if (t)
      *t = o->description->inset.t;
   if (b)
      *b = o->description->inset.b;
}

void
ebits_get_min_size(Ebits_Object o, int *w, int *h)
{
   if (w)
      *w = o->description->min.w;
   if (h)
      *h = o->description->min.h;
}

void
ebits_get_max_size(Ebits_Object o, int *w, int *h)
{
   if (w)
      *w = o->description->max.w;
   if (h)
      *h = o->description->max.h;
}

void
ebits_get_real_min_size(Ebits_Object o, int *w, int *h)
{
   _ebits_calculate_min_size(o);
   if (w)
      *w = o->description->real_min_size.w;
   if (h)
      *h = o->description->real_min_size.h;
}

void
ebits_get_size_step(Ebits_Object o, int *x, int *y)
{
   if (x)
      *x = o->description->step.x;
   if (y)
      *y = o->description->step.y;
}

void
ebits_get_named_bit_geometry(Ebits_Object o, char *c, double *x, double *y,
			     double *w, double *h)
{
   Ebits_Object_Bit_State state;

   state = ebits_get_bit_name(o, c);
   if (state)
     {
	if (x)
	   *x = (double)state->x;
	if (y)
	   *y = (double)state->y;
	if (w)
	   *w = (double)state->w;
	if (h)
	   *h = (double)state->h;
     }
   else
     {
	if (x)
	   *x = 0;
	if (y)
	   *y = 0;
	if (w)
	   *w = -1;
	if (h)
	   *h = -1;
     }
}

void
ebits_set_classed_bit_callback(Ebits_Object o, char *c,
			       Evas_Callback_Type type,
			       void (*func) (void *_data, Ebits_Object _o,
					     char *_c, int _b, int _x,
					     int _y, int _ox, int _oy,
					     int _ow, int _oh), void *data)
{
   Evas_List           l;

   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	if ((state->description->class) && (c))
	  {
	     if (!strcmp(state->description->class, c))
	       {
		  Ebits_Callback      cb;

		  cb = malloc(sizeof(struct _Ebits_Callback));
		  cb = memset(cb, 0, sizeof(struct _Ebits_Callback));
		  cb->type = type;
		  cb->func = func;
		  cb->data = data;
		  state->callbacks = evas_list_append(state->callbacks, cb);
	       }
	  }
     }
}

void
ebits_set_color_class(Ebits_Object o, char *cc, int r, int g, int b, int a)
{
   Evas_List           l;

   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	if ((state->description->color_class) && (cc))
	  {
	     if (!strcmp(state->description->color_class, cc))
	       {
		  state->r = r;
		  state->g = g;
		  state->b = b;
		  state->a = a;
		  if ((state->o->state.evas) && (state->object))
		     evas_set_color(state->o->state.evas,
				    state->object, state->r,
				    state->g, state->b, state->a);
	       }
	  }
	if (state->func_set_color_class)
	   state->func_set_color_class(state->func_data, cc, r, g, b, a);
     }
}

Ebits_Object
ebits_new(void)
{
   Ebits_Object        o;

   o = malloc(sizeof(struct _Ebits_Object));
   o = memset(o, 0, sizeof(struct _Ebits_Object));
   o->description = NULL;
   o->state.x = 0;
   o->state.y = 0;
   o->state.w = 32;
   o->state.h = 32;
   return o;
}

#ifdef EDITOR
void
ebits_save(Ebits_Object o, char *file)
{
   Ebits_Object_Description d;
   Evas_List           l;
   E_DB_File          *db;
   int                 i, count;

   d = o->description;
   /* delete the original */
   unlink(file);
   /* open it now */
   db = e_db_open(file);
   if (!db)
      return;

   e_db_int_set(db, "/type/bits", 1);

   e_db_int_set(db, "/base/min/w", d->min.w);
   e_db_int_set(db, "/base/min/h", d->min.h);

   e_db_int_set(db, "/base/max/w", d->max.w);
   e_db_int_set(db, "/base/max/h", d->max.h);

   e_db_int_set(db, "/base/step/x", d->step.x);
   e_db_int_set(db, "/base/step/y", d->step.y);

   e_db_int_set(db, "/base/padding/l", d->padding.l);
   e_db_int_set(db, "/base/padding/r", d->padding.r);
   e_db_int_set(db, "/base/padding/t", d->padding.t);
   e_db_int_set(db, "/base/padding/b", d->padding.b);

   e_db_int_set(db, "/base/inset/l", d->inset.l);
   e_db_int_set(db, "/base/inset/r", d->inset.r);
   e_db_int_set(db, "/base/inset/t", d->inset.t);
   e_db_int_set(db, "/base/inset/b", d->inset.b);

   for (count = 0, l = d->state_names; l; l = l->next, count++);

   e_db_int_set(db, "/base/states/count", count);

   for (i = 0, l = d->state_names; l; l = l->next, i++)
     {
	char                key[4096];

	snprintf(key, sizeof(key), "/base/states/%i/name", i);
	e_db_str_set(db, key, l->data);
     }

   for (count = 0, l = d->bits; l; l = l->next, count++);

   e_db_int_set(db, "/bits/count", count);

   /* save the images */
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_Description bit;
	Ebits_Object_Bit_State state;
	Evas_List           ll;
	char                image[4096];

	state = l->data;
	bit = state->description;

	if ((state->normal.image) && (bit->normal.image))
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT,
		      file, bit->normal.image);
	     imlib_context_set_image(state->normal.image);
	     imlib_image_set_format("db");
	     imlib_save_image(image);
	     state->normal.saved = 1;
	  }
	if ((state->hilited.image) && (bit->hilited.image))
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT,
		      file, bit->hilited.image);
	     imlib_context_set_image(state->hilited.image);
	     imlib_image_set_format("db");
	     imlib_save_image(image);
	     state->hilited.saved = 1;
	  }
	if ((state->clicked.image) && (bit->clicked.image))
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT,
		      file, bit->clicked.image);
	     imlib_context_set_image(state->clicked.image);
	     imlib_image_set_format("db");
	     imlib_save_image(image);
	     state->clicked.saved = 1;
	  }
	if ((state->disabled.image) && (bit->disabled.image))
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT,
		      file, bit->disabled.image);
	     imlib_context_set_image(state->disabled.image);
	     imlib_image_set_format("db");
	     imlib_save_image(image);
	     state->disabled.saved = 1;
	  }
	/* XXX */
	for (ll = state->description->state_description; ll; ll = ll->next)
	  {
	     Ebits_State_Description state_d;

	     state_d = ll->data;

	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT,
		      file, state_d->image);
	     imlib_context_set_image(state_d->ss_d->image);
	     imlib_image_set_format("db");
	     imlib_save_image(image);
	     state_d->ss_d->saved = 1;
	  }
     }
   /* save bit info */
   for (i = 0, l = d->bits; l; l = l->next, i++)
     {
	Ebits_Object_Bit_Description bit;
	Evas_List           ll;
	char                key[4096];
	int                 j, sync_count;

	bit = l->data;

	snprintf(key, sizeof(key), "/bits/bit/%i/name", i);
	if (bit->name)
	   e_db_str_set(db, key, bit->name);
	snprintf(key, sizeof(key), "/bits/bit/%i/class", i);
	if (bit->class)
	   e_db_str_set(db, key, bit->class);
	snprintf(key, sizeof(key), "/bits/bit/%i/color_class", i);
	if (bit->color_class)
	   e_db_str_set(db, key, bit->color_class);
	snprintf(key, sizeof(key), "/bits/bit/%i/normal/image", i);
	if (bit->normal.image)
	   e_db_str_set(db, key, bit->normal.image);
	snprintf(key, sizeof(key), "/bits/bit/%i/hilited/image", i);
	if (bit->hilited.image)
	   e_db_str_set(db, key, bit->hilited.image);
	snprintf(key, sizeof(key), "/bits/bit/%i/clicked/image", i);
	if (bit->clicked.image)
	   e_db_str_set(db, key, bit->clicked.image);
	snprintf(key, sizeof(key), "/bits/bit/%i/disabled/image", i);
	if (bit->disabled.image)
	   e_db_str_set(db, key, bit->disabled.image);

	for (ll = bit->state_description; ll; ll = ll->next)
	  {
	     Ebits_State_Description state_d;

	     state_d = ll->data;

	     snprintf(key, sizeof(key), "/bits/bit/%i/%s/image",
		      i, state_d->name);
	     e_db_str_set(db, key, state_d->image);
	  }

	snprintf(key, sizeof(key), "/bits/bit/%i/border/l", i);
	e_db_int_set(db, key, bit->border.l);
	snprintf(key, sizeof(key), "/bits/bit/%i/border/r", i);
	e_db_int_set(db, key, bit->border.r);
	snprintf(key, sizeof(key), "/bits/bit/%i/border/t", i);
	e_db_int_set(db, key, bit->border.t);
	snprintf(key, sizeof(key), "/bits/bit/%i/border/b", i);
	e_db_int_set(db, key, bit->border.b);

	snprintf(key, sizeof(key), "/bits/bit/%i/tile/w", i);
	e_db_int_set(db, key, bit->tile.w);
	snprintf(key, sizeof(key), "/bits/bit/%i/tile/h", i);
	e_db_int_set(db, key, bit->tile.h);

	snprintf(key, sizeof(key), "/bits/bit/%i/rel1/name", i);
	if (bit->rel1.name)
	   e_db_str_set(db, key, bit->rel1.name);
	snprintf(key, sizeof(key), "/bits/bit/%i/rel1/x", i);
	e_db_int_set(db, key, bit->rel1.x);
	snprintf(key, sizeof(key), "/bits/bit/%i/rel1/y", i);
	e_db_int_set(db, key, bit->rel1.y);
	snprintf(key, sizeof(key), "/bits/bit/%i/rel1/rx", i);
	e_db_float_set(db, key, bit->rel1.rx);
	snprintf(key, sizeof(key), "/bits/bit/%i/rel1/ry", i);
	e_db_float_set(db, key, bit->rel1.ry);
	snprintf(key, sizeof(key), "/bits/bit/%i/rel1/ax", i);
	e_db_int_set(db, key, bit->rel1.ax);
	snprintf(key, sizeof(key), "/bits/bit/%i/rel1/ay", i);
	e_db_int_set(db, key, bit->rel1.ay);

	snprintf(key, sizeof(key), "/bits/bit/%i/rel2/name", i);
	if (bit->rel2.name)
	   e_db_str_set(db, key, bit->rel2.name);
	snprintf(key, sizeof(key), "/bits/bit/%i/rel2/x", i);
	e_db_int_set(db, key, bit->rel2.x);
	snprintf(key, sizeof(key), "/bits/bit/%i/rel2/y", i);
	e_db_int_set(db, key, bit->rel2.y);
	snprintf(key, sizeof(key), "/bits/bit/%i/rel2/rx", i);
	e_db_float_set(db, key, bit->rel2.rx);
	snprintf(key, sizeof(key), "/bits/bit/%i/rel2/ry", i);
	e_db_float_set(db, key, bit->rel2.ry);
	snprintf(key, sizeof(key), "/bits/bit/%i/rel2/ax", i);
	e_db_int_set(db, key, bit->rel2.ax);
	snprintf(key, sizeof(key), "/bits/bit/%i/rel2/ay", i);
	e_db_int_set(db, key, bit->rel2.ay);

	snprintf(key, sizeof(key), "/bits/bit/%i/align/w", i);
	e_db_float_set(db, key, bit->align.w);
	snprintf(key, sizeof(key), "/bits/bit/%i/align/h", i);
	e_db_float_set(db, key, bit->align.h);

	snprintf(key, sizeof(key), "/bits/bit/%i/aspect/x", i);
	e_db_int_set(db, key, bit->aspect.x);
	snprintf(key, sizeof(key), "/bits/bit/%i/aspect/y", i);
	e_db_int_set(db, key, bit->aspect.y);
	snprintf(key, sizeof(key), "/bits/bit/%i/step/x", i);
	e_db_int_set(db, key, bit->step.x);
	snprintf(key, sizeof(key), "/bits/bit/%i/step/y", i);
	e_db_int_set(db, key, bit->step.y);

	snprintf(key, sizeof(key), "/bits/bit/%i/min/w", i);
	e_db_int_set(db, key, bit->min.w);
	snprintf(key, sizeof(key), "/bits/bit/%i/min/h", i);
	e_db_int_set(db, key, bit->min.h);
	snprintf(key, sizeof(key), "/bits/bit/%i/max/w", i);
	e_db_int_set(db, key, bit->max.w);
	snprintf(key, sizeof(key), "/bits/bit/%i/max/h", i);
	e_db_int_set(db, key, bit->max.h);

	for (sync_count = 0, ll = bit->sync; ll; ll = ll->next, sync_count++);
	snprintf(key, sizeof(key), "/bits/bit/%i/sync/count", i);
	e_db_int_set(db, key, sync_count);
	for (j = 0, ll = bit->sync; ll; ll = ll->next, j++)
	  {
	     snprintf(key, sizeof(key), "/bits/bit/%i/sync/%i", i, j);
	     e_db_str_set(db, key, ll->data);
	  }
     }

   e_db_close(db);
   e_db_flush();
}
#endif

void
ebits_set_named_bit_replace(Ebits_Object o, char *c,
			    void (*func_show) (void *_data),
			    void (*func_hide) (void *_data),
			    void (*func_move) (void *_data, double x,
					       double y),
			    void (*func_resize) (void *_data, double w,
						 double h),
			    void (*func_raise) (void *_data),
			    void (*func_lower) (void *_data),
			    void (*func_set_layer) (void *_data, int l),
			    void (*func_set_clip) (void *_data,
						   Evas_Object clip),
			    void (*func_set_color_class) (void *_data,
							  char *cc, int r,
							  int g, int b,
							  int a),
			    void (*func_get_min_size) (void *_data,
						       double *w, double *h),
			    void (*func_get_max_size) (void *_data,
						       double *w, double *h),
			    void *data)
{
   Evas_List           l;

   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;

	state = l->data;
	if (!strcmp(state->description->name, c))
	  {
	     state->func_data = data;
	     state->func_show = func_show;
	     state->func_hide = func_hide;
	     state->func_move = func_move;
	     state->func_resize = func_resize;
	     state->func_raise = func_raise;
	     state->func_lower = func_lower;
	     state->func_set_layer = func_set_layer;
	     state->func_set_clip = func_set_clip;
	     state->func_get_min_size = func_get_min_size;
	     state->func_get_max_size = func_get_max_size;
	     state->func_set_color_class = func_set_color_class;
	     if (state->func_set_clip)
	       {
		  state->func_set_clip(state->func_data, state->object);
		  evas_set_color(o->state.evas, state->object,
				 255, 255, 255, 255);
	       }
	     if ((o->state.visible) && (func_show))
		func_show(data);
	     else if ((!o->state.visible) && (func_hide))
		func_hide(data);
	     if ((state->description->color_class) && (func_set_color_class))
		func_set_color_class(data,
				     state->description->
				     color_class, state->r,
				     state->g, state->b, state->a);
	     if (func_set_layer)
		func_set_layer(data, o->state.layer);
	     if (func_set_clip)
		func_set_clip(data, o->state.clip);
	     _ebits_object_calculate(o);
	     ebits_raise(o);
	     return;
	  }
     }
}

void
ebits_set_named_bit_state(Ebits_Object o, char *c, char *state)
{
   Evas_List           l;

   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State st;

	st = l->data;
	if (!strcmp(st->description->name, c))
	  {
	     IF_FREE(st->state);

	     st->state = strdup(state);
	     _ebits_sync_bits(st);
	  }
     }
}

Evas_List
ebits_get_bit_names(Ebits_Object o)
{
   Evas_List           l, name_list;

   name_list = NULL;

   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State st;

	st = l->data;
	name_list = evas_list_append(name_list, strdup(st->description->name));
     }

   return name_list;
}

Evas_List
ebits_get_state_names(Ebits_Object o)
{
   if (!o)
      return NULL;

   return o->description->state_names;
}

#ifdef EDITOR
void
ebits_add_state_name(Ebits_Object o, char *name)
{
   Evas_List           l;
   int                 i, len;

   if (!o || !name || !(len = strlen(name)))
      return;

   for (l = o->description->state_names; l; l = l->next)
      if (!strncasecmp(l->data, name, len))
	 return;

   for (i = 0; i < len; i++)
      name[i] = tolower(name[i]);

   o->description->state_names =
      evas_list_append(o->description->state_names, name);
}

void
ebits_del_state_name(Ebits_Object o, char *name)
{
   Evas_List           l;
   int                 len;

   if (!o || !name || !(len = strlen(name)))
      return;

   len = strlen(name);

   for (l = o->description->state_names; l; l = l->next)
     {
	if (!strncasecmp(l->data, name, len))
	  {
	     FREE(l->data);
	     o->description->state_names =
		evas_list_remove(o->description->state_names, l->data);
	     break;
	  }
     }
}

void
ebits_add_bit_state(Ebits_Object_Bit_State bit, char *state, char *image)
{
   Evas_List           l;
   int                 found = 0, len, len2;

   if (!bit || !state)
      return;

   len = strlen(state);

   for (l = ebits_get_state_names(bit->o); l; l = l->next)
     {
	len2 = strlen(l->data);

	if (len != len2)
	   continue;

	if (!strncasecmp(state, (char *)l->data, len))
	  {
	     found = 1;
	     state = l->data;
	  }
     }

   if (!found)
      return;

   found = 0;

   for (l = bit->state_source_description; l; l = l->next)
     {
	Ebits_State_Source_Description ss_d;

	ss_d = l->data;

	len2 = strlen(ss_d->name);

	if (!strncasecmp(state, ss_d->name, len) && len == len2)
	  {
	     found = 1;

	     if (!image || !strlen(image))
	       {
		  evas_list_remove(bit->state_source_description, ss_d);
		  evas_list_remove(bit->description->
				   state_description, ss_d->state_d);

		  if (ss_d->image)
		    {
		       imlib_context_set_image(ss_d->image);
		       imlib_free_image_and_decache();
		       ss_d->image = NULL;
		    }

		  FREE(ss_d->state_d->image);
		  FREE(ss_d->name);
		  FREE(ss_d->state_d->name);

		  FREE(ss_d->state_d);
		  FREE(ss_d);
		  return;
	       }

	     if (!strcmp(ss_d->state_d->image, image))
		return;

	     FREE(ss_d->state_d->image);

	     ss_d->state_d->image = strdup(image);
	     ss_d->saved = 0;
	  }
     }

   if (!found)
     {
	Ebits_State_Description state_d;

	state_d = malloc(sizeof(struct _Ebits_State_Description));
	state_d = memset(state_d, 0, sizeof(struct _Ebits_State_Description));

	state_d->name = strdup(state);
	state_d->image = strdup(image);

	bit->description->state_description =
	   evas_list_append(bit->description->state_description, state_d);
     }
}
#endif
