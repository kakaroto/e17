#include "bits.h"
#include "Edb.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static Evas_List __bit_descriptions = NULL;

static Ebits_Object_Description _ebits_find_description(char *file);
static char *_ebits_get_file(Ebits_Object_Bit_Description d, int state);
#ifdef EDITOR
static int _ebits_image_state_saved(Ebits_Object_Bit_State state, int s);
#endif
static void _ebits_sync_bits(Ebits_Object_Bit_State state);
static void _ebits_evaluate_fill(Ebits_Object_Bit_State state);
static void _ebits_calculate(Ebits_Object_Bit_State state);
static void _ebits_object_calculate(Ebits_Object o);

#define EBITS_FILE_REDIRECT "%s:/images/%s"
/* #define LENIENT 1 */

Ebits_Object_Bit_State ebits_get_bit_name(Ebits_Object o, char *name)
{
   Evas_List l;
   
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;
	
	state = l->data;
	if (!strcmp(state->description->name, name)) return state;
     }
   return NULL;
}

static void 
_ebits_evaluate_fill(Ebits_Object_Bit_State state)
{
   double fill_w, fill_h;
   
   evas_set_image_border(state->o->state.evas, state->object,
			 state->description->border.l,
			 state->description->border.r,
			 state->description->border.t,
			 state->description->border.b);
   fill_w = state->w;
   if (state->description->tile.w == 1) 
     {
	int im_w;
	
	evas_get_image_size(state->o->state.evas, state->object, &im_w, NULL);
	if (im_w > 0) fill_w = im_w;
     }
   else if (state->description->tile.w == 2)
     {
	int im_w;
	
	evas_get_image_size(state->o->state.evas, state->object, &im_w, NULL);
	if (im_w > 0)
	  {
	     int num;
	     
	     num = (int)(state->w / (double)im_w);
	     if (num < 1) num = 1;
	     fill_w = state->w / (double)num;
	  }
     }
   fill_h = state->h;
   if (state->description->tile.h == 1) 
     {
	int im_h;
	
	evas_get_image_size(state->o->state.evas, state->object, NULL, &im_h);
	if (im_h > 0) fill_h = im_h;
     }
   else if (state->description->tile.h == 2)
     {
	int im_h;
	
	evas_get_image_size(state->o->state.evas, state->object, NULL, &im_h);
	if (im_h > 0)
	  {
	     int num;
	     
	     num = (int)(state->h / (double)im_h);
	     if (num < 1) num = 1;
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
   Evas_List l;
   
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
	evas_resize(state->o->state.evas, state->object, 
		    state->w, state->h);
     }
}

static void
_ebits_calculate(Ebits_Object_Bit_State state)
{
   int                 x1, y1, x2, y2;
   int                 rx, ry, rw, rh;
   int                 want_w, want_h;
   
   
   if ((state->calculated) || (state->calc_pending))
      return;
   state->calc_pending = 1;
   if (state->description->rel1.name)
     {
	Ebits_Object_Bit_State  state2;
	
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
   x1 = rx + ((rw + state->description->rel1.ax) * state->description->rel1.rx) + state->description->rel1.x;
   y1 = ry + ((rh + state->description->rel1.ay) * state->description->rel1.ry) + state->description->rel1.y;

   if (state->description->rel2.name)
     {
	Ebits_Object_Bit_State  state2;
	
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
   x2 = rx + ((rw + state->description->rel2.ax) * state->description->rel2.rx) + state->description->rel2.x;
   y2 = ry + ((rh + state->description->rel2.ay) * state->description->rel2.ry) + state->description->rel2.y;
   
   state->x = x1;
   state->y = y1;
   
   want_w = state->w = x2 - x1 + 1;
   want_h = state->h = y2 - y1 + 1;
   
   state->w = (state->w / state->description->step.x) * state->description->step.x;
   state->h = (state->h / state->description->step.y) * state->description->step.y;
   
   if ((state->description->aspect.x > 0) && (state->description->aspect.y > 0))
     {
	int                 hh, ww;
	double              th, tw;
	
	tw = ((double)state->w / (double)state->description->aspect.x);
	th = ((double)state->h / (double)state->description->aspect.y);
	
	if (tw < th)
	  {
	     hh = (state->w * state->description->aspect.x) / state->description->aspect.y;
	     state->h = hh;
	  }
	else
	  {
	     ww = (state->h * state->description->aspect.x) / state->description->aspect.y;
	     state->w = ww;
	  }
     }
   if (state->w < state->description->min.w)
      state->w = state->description->min.w;
   if (state->h < state->description->min.h)
      state->h = state->description->min.h;
   if (state->description->max.w == 0)
     {
	/* if the bit has some other pre-defined size */
     }
   else
     {
	if (state->w > state->description->max.w)
	   state->w = state->description->max.w;
	state->x -= (state->w - want_w) * state->description->align.w;
     }
   if (state->description->max.h == 0)
     {
	/* if the bit has some other pre-defined size */
     }
   else
     {
	if (state->h > state->description->max.h)
	   state->h = state->description->max.h;
	state->y -= (state->h - want_h) * state->description->align.h;
     }
   state->calculated = 1;
   state->calc_pending = 0;
}

static void
_ebits_sync_bits(Ebits_Object_Bit_State state)
{
   Evas_List l;
   
   state->syncing = 1;
   if (state->object)
     {
	char buf[4096];

#ifdef EDITOR	
	if (!_ebits_image_state_saved(state, state->state))
	   snprintf(buf, sizeof(buf), "%s", 
		    _ebits_get_file(state->description, state->state));
	else
#endif	   
	   snprintf(buf, sizeof(buf), EBITS_FILE_REDIRECT, 
		    state->o->description->file, 
		    _ebits_get_file(state->description, state->state));
	evas_set_image_file(state->o->state.evas, state->object, buf);
	_ebits_evaluate_fill(state);
     }
   for (l = state->description->sync; l; l = l->next)
     {
	Ebits_Object_Bit_State state2;

	state2 = ebits_get_bit_name(state->o, l->data);
	if ((state2) && (state2 != state) && (!state2->syncing))
	  {
	     state2->state = state->state;
	     _ebits_sync_bits(state2);
	  }
     }
   state->syncing = 0;
}

static void
_ebits_handle_mouse_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   Ebits_Object_Bit_State state;
   
   state = _data;
   if (state->state == 3) return;
   state->state = 2;
   _ebits_sync_bits(state);
}

static void
_ebits_handle_mouse_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   Ebits_Object_Bit_State state;
   
   state = _data;
   if (state->state == 3) return;
   if (state->mouse_in) state->state = 1;
   else state->state = 0;
   _ebits_sync_bits(state);
}

static void
_ebits_handle_mouse_move (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   Ebits_Object_Bit_State state;
   
   state = _data;
   if (state->state == 3) return;
}

static void
_ebits_handle_mouse_in (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   Ebits_Object_Bit_State state;
   
   state = _data;
   if (state->state == 3) return;
   state->mouse_in = 1;
   if (state->state != 2) state->state = 1;
   _ebits_sync_bits(state);
}

static void
_ebits_handle_mouse_out (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   Ebits_Object_Bit_State state;
   
   state = _data;
   if (state->state == 3) return;
   state->mouse_in = 0;
   if (state->state != 2) state->state = 0;
   _ebits_sync_bits(state);
}

#ifdef EDITOR
static int
_ebits_image_state_saved(Ebits_Object_Bit_State state, int s)
{
   if (s == 0)
     {
	if (state->description->normal.image) return state->normal.saved;
	if (state->description->hilited.image) return state->hilited.saved;
	if (state->description->clicked.image) return state->clicked.saved;
	if (state->description->disabled.image) return state->disabled.saved;
	if (state->description->selected.image) return state->selected.saved;
     }
   if (s == 1)
     {
	if (state->description->hilited.image) return state->hilited.saved;
	if (state->description->clicked.image) return state->clicked.saved;
	if (state->description->normal.image) return state->normal.saved;
	if (state->description->disabled.image) return state->disabled.saved;
	if (state->description->selected.image) return state->selected.saved;
     }
   if (s == 2)
     {
	if (state->description->clicked.image) return state->clicked.saved;
	if (state->description->hilited.image) return state->hilited.saved;
	if (state->description->normal.image) return state->normal.saved;
	if (state->description->disabled.image) return state->disabled.saved;
	if (state->description->selected.image) return state->selected.saved;
     }
   if (s == 3)
     {
	if (state->description->disabled.image) return state->disabled.saved;
	if (state->description->normal.image) return state->normal.saved;
	if (state->description->hilited.image) return state->hilited.saved;
	if (state->description->clicked.image) return state->clicked.saved;
	if (state->description->selected.image) return state->selected.saved;
     }
   if (s == 4)
     {
        if (state->description->selected.image) return state->selected.saved;
        if (state->description->disabled.image) return state->disabled.saved;
        if (state->description->normal.image) return state->normal.saved;
        if (state->description->hilited.image) return state->hilited.saved;
        if (state->description->clicked.image) return state->clicked.saved;
     }
   return 0;
}
#endif

static char *
_ebits_get_file(Ebits_Object_Bit_Description d, int state)
{
   if (state == 0)
     {
	if (d->normal.image) return d->normal.image;
	if (d->hilited.image) return d->hilited.image;
	if (d->clicked.image) return d->clicked.image;
	if (d->disabled.image) return d->disabled.image;
	if (d->selected.image) return d->selected.image;
     }
   if (state == 1)
     {
	if (d->hilited.image) return d->hilited.image;
	if (d->clicked.image) return d->clicked.image;
	if (d->normal.image) return d->normal.image;
	if (d->disabled.image) return d->disabled.image;
	if (d->selected.image) return d->selected.image;
     }
   if (state == 2)
     {
	if (d->clicked.image) return d->clicked.image;
	if (d->hilited.image) return d->hilited.image;
	if (d->normal.image) return d->normal.image;
	if (d->disabled.image) return d->disabled.image;
	if (d->selected.image) return d->selected.image;
     }
   if (state == 3)
     {
	if (d->disabled.image) return d->disabled.image;
	if (d->normal.image) return d->normal.image;
	if (d->hilited.image) return d->hilited.image;
	if (d->clicked.image) return d->clicked.image;
	if (d->selected.image) return d->selected.image;
     }
   if (state == 4)
     {
        if (d->selected.image) return d->selected.image;
        if (d->disabled.image) return d->disabled.image;
        if (d->normal.image) return d->normal.image;
        if (d->hilited.image) return d->hilited.image;
        if (d->clicked.image) return d->clicked.image;
     }
   return "";
}

static Ebits_Object_Description
_ebits_find_description(char *file)
{
   Ebits_Object_Description d = NULL;
   Evas_List l;
   E_DB_File *db;
   int version;
   
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
	     d->references++;
	     return d;
	   }
     }
   /* open db */
   db = e_db_open_read(file);
   /* no db - return NULL */
   if (!db) return NULL;
   
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
   d->file = strdup(file);
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
   
   if (d->step.x <= 0) d->step.x = 1;
   if (d->step.y <= 0) d->step.y = 1;

   /* all the bits */
     {
	int num_bits = 0, i;
	
	e_db_int_get(db, "/bits/count", &(num_bits));
	for (i = 0; i < num_bits; i++)
	  {
	     Ebits_Object_Bit_Description bit;
	     char key[4096];
	     float f;
	     int num_sync, j;
	     
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
	     snprintf(key, sizeof(key), "/bits/bit/%i/selected/image", i);
	     bit->selected.image = e_db_str_get(db, key);
	     
	     snprintf(key, sizeof(key), "/bits/bit/%i/border/l", i);
	     e_db_int_get(db, key, &(bit->border.l));
	     snprintf(key, sizeof(key), "/bits/bit/%i/border/r", i);
	     e_db_int_get(db, key, &(bit->border.r));
	     snprintf(key, sizeof(key), "/bits/bit/%i/border/t", i);
	     e_db_int_get(db, key, &(bit->border.t));
	     snprintf(key, sizeof(key), "/bits/bit/%i/border/b", i);
	     e_db_int_get(db, key, &(bit->border.b));

	     snprintf(key, sizeof(key), "/bits/bit/%i/tile/w", i);
	     e_db_int_get(db, key, &(bit->tile.w));
	     snprintf(key, sizeof(key), "/bits/bit/%i/tile/h", i);
	     e_db_int_get(db, key, &(bit->tile.h));
	     
	     snprintf(key, sizeof(key), "/bits/bit/%i/rel1/name", i);
	     bit->rel1.name = e_db_str_get(db, key);
	     snprintf(key, sizeof(key), "/bits/bit/%i/rel1/x", i);
	     e_db_int_get(db, key, &(bit->rel1.x));
	     snprintf(key, sizeof(key), "/bits/bit/%i/rel1/y", i);
	     e_db_int_get(db, key, &(bit->rel1.y));
	     snprintf(key, sizeof(key), "/bits/bit/%i/rel1/rx", i);
	     e_db_float_get(db, key, &(f)); bit->rel1.rx = f;
	     snprintf(key, sizeof(key), "/bits/bit/%i/rel1/ry", i);
	     e_db_float_get(db, key, &(f)); bit->rel1.ry = f;
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
	     e_db_float_get(db, key, &(f)); bit->rel2.rx = f;
	     snprintf(key, sizeof(key), "/bits/bit/%i/rel2/ry", i);
	     e_db_float_get(db, key, &(f)); bit->rel2.ry = f;
	     snprintf(key, sizeof(key), "/bits/bit/%i/rel2/ax", i);
	     e_db_int_get(db, key, &(bit->rel2.ax));
	     snprintf(key, sizeof(key), "/bits/bit/%i/rel2/ay", i);
	     e_db_int_get(db, key, &(bit->rel2.ay));

	     snprintf(key, sizeof(key), "/bits/bit/%i/align/w", i);
	     e_db_float_get(db, key, &(f)); bit->align.w = f;
	     snprintf(key, sizeof(key), "/bits/bit/%i/align/h", i);
	     e_db_float_get(db, key, &(f)); bit->align.h = f;

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

	     if (bit->step.x <= 0) bit->step.x = 1;
	     if (bit->step.y <= 0) bit->step.y = 1;
	     	     
	     for (j = 0; j < num_sync; j++)
	       {
		  char *s;
		  
		  snprintf(key, sizeof(key), "/bits/bit/%i/sync/%i", i, j);
		  s = e_db_str_get(db, key);
		  if (s) bit->sync = evas_list_append(bit->sync, s);
	       }
	     
	     d->bits = evas_list_append(d->bits, bit);
	  }
     }

   e_db_close(db);
   return d;
}

#ifdef EDITOR
void
ebits_set_state(Ebits_Object o, int st)
{
   Evas_List l;

   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;
	
	state = l->data;
	state->state = st;
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
   if (state->description->normal.image)
      state->normal.image = imlib_load_image(state->description->normal.image);
   if (state->description->hilited.image)
      state->hilited.image = imlib_load_image(state->description->hilited.image);
   if (state->description->clicked.image)
      state->clicked.image = imlib_load_image(state->description->clicked.image);
   if (state->description->disabled.image)
      state->disabled.image = imlib_load_image(state->description->disabled.image);
   if (state->description->selected.image)
         state->selected.image = imlib_load_image(state->description->selected.image);
   _ebits_object_calculate(state->o);
}

void
ebits_del_bit(Ebits_Object o, Ebits_Object_Bit_State state)
{
   Evas_List l;
   
   if (state->object) evas_del_object(o->state.evas, state->object);
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
   if (state->selected.image)
     {
        imlib_context_set_image(state->selected.image);
        imlib_free_image_and_decache();
     }
   if (state->description->name) free(state->description->name);
   if (state->description->class) free(state->description->class);
   if (state->description->color_class) free(state->description->color_class);
   if (state->description->normal.image) free(state->description->normal.image);
   if (state->description->hilited.image) free(state->description->hilited.image);
   if (state->description->clicked.image) free(state->description->clicked.image);
   if (state->description->disabled.image) free(state->description->disabled.image);
   if (state->description->selected.image) free(state->description->selected.image);
   if (state->description->rel1.name) free(state->description->rel1.name);
   if (state->description->rel2.name) free(state->description->rel2.name);
   for (l = state->description->sync; l; l = l->next) free(l->data);
   evas_list_free(state->description->sync);
   o->bits = evas_list_remove(o->bits, state);
   o->description->bits = evas_list_remove(o->description->bits, state->description);
   free(state->description);
   free(state);
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
   
   bit->normal.image = strdup(file);
   state->object = evas_add_image_from_file(o->state.evas, _ebits_get_file(state->description, state->state));
   evas_callback_add(o->state.evas, state->object, CALLBACK_MOUSE_DOWN, _ebits_handle_mouse_down, state);
   evas_callback_add(o->state.evas, state->object, CALLBACK_MOUSE_UP, _ebits_handle_mouse_up, state);
   evas_callback_add(o->state.evas, state->object, CALLBACK_MOUSE_MOVE, _ebits_handle_mouse_move, state);
   evas_callback_add(o->state.evas, state->object, CALLBACK_MOUSE_IN, _ebits_handle_mouse_in, state);
   evas_callback_add(o->state.evas, state->object, CALLBACK_MOUSE_OUT, _ebits_handle_mouse_out, state);

   evas_set_layer(o->state.evas, state->object, o->state.layer);
   if (o->state.visible) evas_show(o->state.evas, state->object);
     {
	int w, h;
	
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

Ebits_Object ebits_load(char *file)
{
   Ebits_Object o;
   Ebits_Object_Description d;   
   Evas_List l;
   
   d = _ebits_find_description(file);
   if (!d) return NULL;
   
   o = ebits_new();
   o->description = d;
   
   for (l = d->bits; l; l = l->next)
     {
	Ebits_Object_Bit_Description bit;
	Ebits_Object_Bit_State state;
#ifdef EDITOR
	char image[4096];
#endif
	
	bit = l->data;
	state = malloc(sizeof(struct _Ebits_Object_Bit_State));
	state = memset(state, 0, sizeof(struct _Ebits_Object_Bit_State));
	o->bits = evas_list_append(o->bits, state);
	state->o = o;
	state->description = bit;
#ifdef EDITOR
	if (bit->normal.image)
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT, file, bit->normal.image);
	     state->normal.image = imlib_load_image(image);
#ifdef LENIENT
	     if (!state->normal.image)
		state->normal.image = imlib_load_image(bit->normal.image);
#endif
	  }
	if (bit->hilited.image)
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT, file, bit->hilited.image);
	     state->hilited.image = imlib_load_image(image);
#ifdef LENIENT
	     if (!state->hilited.image)
		state->hilited.image = imlib_load_image(bit->hilited.image);
#endif
	  }
	if (bit->clicked.image)
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT, file, bit->clicked.image);
	     state->clicked.image = imlib_load_image(image);
#ifdef LENIENT
	     if (!state->clicked.image)
		state->clicked.image = imlib_load_image(bit->clicked.image);
#endif
	  }
	if (bit->disabled.image)
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT, file, bit->disabled.image);
	     state->disabled.image = imlib_load_image(image);
#ifdef LENIENT
	     if (!state->disabled.image)
		state->disabled.image = imlib_load_image(bit->disabled.image);
#endif
	  }
        if (bit->selected.image)
          {
             snprintf(image, sizeof(image), EBITS_FILE_REDIRECT, file, bit->selected.image);
             state->selected.image = imlib_load_image(image);
#ifdef LENIENT
             if (!state->selected.image)
                state->selected.image = imlib_load_image(bit->selected.image);
#endif
          }
#endif
     }
   return o;
}

void ebits_free(Ebits_Object o)
{
   Evas_List l;
   
   o->description->references--;
   if (o->description->references <= 0)
     {
	if (o->description->file) free(o->description->file);
	__bit_descriptions = evas_list_remove(__bit_descriptions, o->description);
	if (o->description->bits)
	  {
	     for (l = o->description->bits; l; l = l->next)
	       {
		  Ebits_Object_Bit_Description bit;
		  Evas_List ll;
		  
		  bit = l->data;
		  if (bit->name) free(bit->name);
		  if (bit->class) free(bit->class);
		  if (bit->color_class) free(bit->color_class);
		  if (bit->normal.image) free(bit->normal.image);
		  if (bit->hilited.image) free(bit->hilited.image);
		  if (bit->clicked.image) free(bit->clicked.image);
		  if (bit->disabled.image) free(bit->disabled.image);
		  if (bit->selected.image) free(bit->selected.image);
		  if (bit->rel1.name) free(bit->rel1.name);
		  if (bit->rel2.name) free(bit->rel2.name);
		  for (ll = bit->sync; ll; ll = ll->next) free(ll->data);
		  evas_list_free(bit->sync);
	       }
	     evas_list_free(o->description->bits);
	  }
	free(o->description);
     }
   if (o->bits)
     {
	for (l = o->bits; l; l = l->next)
	  {
	     Ebits_Object_Bit_State state;
	     
	     state = l->data;
	     if ((state->object) && (o->state.evas))
		evas_del_object(o->state.evas, state->object);
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
             if (state->selected.image)
               {
                  imlib_context_set_image(state->selected.image);
                  imlib_free_image_and_decache();
               }
#endif
	     free(state);
	  }
	evas_list_free(o->bits);
     }
   free(o);
}

void ebits_add_to_evas(Ebits_Object o, Evas e)
{
   Evas_List l;
   
   o->state.evas = e;
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;
	char buf[4096];
	
	state = l->data;
	snprintf(buf, sizeof(buf), EBITS_FILE_REDIRECT, 
		 state->o->description->file, 
		 _ebits_get_file(state->description, state->state));
	state->object = evas_add_image_from_file(o->state.evas, buf);
#ifdef EDITOR
	if (state->normal.image) state->normal.saved = 1;
	if (state->hilited.image) state->hilited.saved = 1;
	if (state->clicked.image) state->clicked.saved = 1;
	if (state->disabled.image) state->disabled.saved = 1;
	if (state->selected.image) state->selected.saved = 1;
#endif	
	evas_callback_add(o->state.evas, state->object, CALLBACK_MOUSE_DOWN, _ebits_handle_mouse_down, state);
	evas_callback_add(o->state.evas, state->object, CALLBACK_MOUSE_UP, _ebits_handle_mouse_up, state);
	evas_callback_add(o->state.evas, state->object, CALLBACK_MOUSE_MOVE, _ebits_handle_mouse_move, state);
	evas_callback_add(o->state.evas, state->object, CALLBACK_MOUSE_IN, _ebits_handle_mouse_in, state);
	evas_callback_add(o->state.evas, state->object, CALLBACK_MOUSE_OUT, _ebits_handle_mouse_out, state);
     }
}

void ebits_show(Ebits_Object o)
{
   Evas_List l;

   if (o->state.visible) return;
   o->state.visible = 1;
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;
	
	state = l->data;
	_ebits_sync_bits(state);
	evas_show(state->o->state.evas, state->object);
     }
}

void ebits_hide(Ebits_Object o)
{
   Evas_List l;

   if (!o->state.visible) return;
   o->state.visible = 0;
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;
	
	state = l->data;
	evas_hide(state->o->state.evas, state->object);
     }
}

void ebits_set_layer(Ebits_Object o, int layer)
{
   Evas_List l;

   if (layer == o->state.layer) return;
   o->state.layer = layer;
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;
	
	state = l->data;
	evas_set_layer(state->o->state.evas, state->object, layer);
     }
}

void ebits_raise(Ebits_Object o)
{
   Evas_List l;

   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;
	
	state = l->data;
	evas_raise(state->o->state.evas, state->object);
     }
}

void ebits_lower(Ebits_Object o)
{
   Evas_List l;

   for (l = o->bits; l->next; l = l->next);
   for (; l; l = l->prev)
     {
	Ebits_Object_Bit_State state;
	
	state = l->data;
	evas_lower(state->o->state.evas, state->object);
     }
}

void ebits_move(Ebits_Object o, double x, double y)
{
   Evas_List l;
   
   o->state.x = x;
   o->state.y = y;
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;
	
	state = l->data;
	evas_move(state->o->state.evas, state->object, 
		  o->state.x + state->x, o->state.y + state->y);
     }
}

void ebits_resize(Ebits_Object o, double w, double h)
{
   w = (double)(((int)w / o->description->step.x) * o->description->step.x);
   h = (double)(((int)h / o->description->step.y) * o->description->step.y);
   if (w > o->description->max.w)      w = o->description->max.w;
   else if (w < o->description->min.w) w = o->description->min.w;
   if (h > o->description->max.h)      h = o->description->max.h;
   else if (h < o->description->min.h) h = o->description->min.h;
   o->state.w = w;
   o->state.h = h;
   _ebits_object_calculate(o);
}

void ebits_get_padding(Ebits_Object o, int *l, int *r, int *t, int *b){}
void ebits_get_insets(Ebits_Object o, int *l, int *r, int *t, int *b){}
void ebits_get_min_size(Ebits_Object o, int *w, int *h){}
void ebits_get_max_size(Ebits_Object o, int *w, int *h){}
void ebits_get_size_step(Ebits_Object o, int *x, int *y){}
void ebits_get_bit_geometry(Ebits_Object o, char *c, double *x, double *y, double *w, double *h){}

Ebits_Object ebits_new(void)
{
   Ebits_Object o;
   
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
void ebits_save(Ebits_Object o, char *file)
{
   Ebits_Object_Description d;
   Evas_List l;
   E_DB_File *db;
   int i, count;
   
   d = o->description;
   /* delete the original */
   unlink(file);
   /* open it now */
   db = e_db_open(file);
   if (!db) return;
   
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
   
   for (count = 0, l = d->bits; l; l = l->next, count++);
   
   e_db_int_set(db, "/bits/count", count);

   /* save the images */
   for (l = o->bits; l; l = l->next)
     {
	Ebits_Object_Bit_Description bit;
	Ebits_Object_Bit_State state;
	char image[4096];
	
	state = l->data;
	bit = state->description;

	if ((state->normal.image) && (bit->normal.image))
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT, file, bit->normal.image);
	     imlib_context_set_image(state->normal.image);
	     imlib_image_set_format("db");
	     imlib_save_image(image);
	     state->normal.saved = 1;
	  }
	if ((state->hilited.image) && (bit->hilited.image))
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT, file, bit->hilited.image);
	     imlib_context_set_image(state->hilited.image);
	     imlib_image_set_format("db");
	     imlib_save_image(image);
	     state->hilited.saved = 1;
	  }
	if ((state->clicked.image) && (bit->clicked.image))
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT, file, bit->clicked.image);
	     imlib_context_set_image(state->clicked.image);
	     imlib_image_set_format("db");
	     imlib_save_image(image);
	     state->clicked.saved = 1;
	  }
	if ((state->disabled.image) && (bit->disabled.image))
	  {
	     snprintf(image, sizeof(image), EBITS_FILE_REDIRECT, file, bit->disabled.image);
	     imlib_context_set_image(state->disabled.image);
	     imlib_image_set_format("db");
	     imlib_save_image(image);
	     state->disabled.saved = 1;
	  }
        if ((state->selected.image) && (bit->selected.image))
          {
             snprintf(image, sizeof(image), EBITS_FILE_REDIRECT, file, bit->selected.image);
             imlib_context_set_image(state->selected.image);
             imlib_image_set_format("db");
             imlib_save_image(image);
             state->selected.saved = 1;
          }
     }
   /* save bit info */
   for (i = 0, l = d->bits; l; l = l->next, i++)
     {
	Ebits_Object_Bit_Description bit;
	Evas_List ll;
	char key[4096];
	int j, sync_count;
	
	bit = l->data;
	
	snprintf(key, sizeof(key), "/bits/bit/%i/name", i);
	if (bit->name) e_db_str_set(db, key, bit->name);
	snprintf(key, sizeof(key), "/bits/bit/%i/class", i);
	if (bit->class) e_db_str_set(db, key, bit->class);
	snprintf(key, sizeof(key), "/bits/bit/%i/color_class", i);
	if (bit->color_class) e_db_str_set(db, key, bit->color_class);
	snprintf(key, sizeof(key), "/bits/bit/%i/normal/image", i);
	if (bit->normal.image) e_db_str_set(db, key, bit->normal.image);
	snprintf(key, sizeof(key), "/bits/bit/%i/hilited/image", i);
	if (bit->hilited.image) e_db_str_set(db, key, bit->hilited.image);
	snprintf(key, sizeof(key), "/bits/bit/%i/clicked/image", i);
	if (bit->clicked.image) e_db_str_set(db, key, bit->clicked.image);
	snprintf(key, sizeof(key), "/bits/bit/%i/disabled/image", i);
	if (bit->disabled.image) e_db_str_set(db, key, bit->disabled.image);
        snprintf(key, sizeof(key), "/bits/bit/%i/selected/image", i);
        if (bit->selected.image) e_db_str_set(db, key, bit->selected.image);
	
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
	if (bit->rel1.name) e_db_str_set(db, key, bit->rel1.name);
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
	if (bit->rel2.name) e_db_str_set(db, key, bit->rel2.name);
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
