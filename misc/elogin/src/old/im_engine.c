#include "first.h"
#include "util.h"
#include "mem.h"
#include "str.h"
#include "x.h"
#include <Imlib2.h>
#include "events.h"
#include "ev_x.h"
#include "im_engine.h"
#include "db.h"

Efm_Bit            *
efm_bit_raise(Efm_Bit * bbit, Efm_Bit * b)
{
   Efm_Bit            *bb;

   bb = efm_bit_remove(bbit, b);
   if (!bb)
      bb = b;
   else
      efm_bit_append(bb, b);
   return bb;
}

Efm_Bit            *
efm_bit_lower(Efm_Bit * bbit, Efm_Bit * b)
{
   Efm_Bit            *bb;

   bb = efm_bit_remove(bbit, b);
   if (!bb)
      bb = b;
   else
      b->next = bb;
   return b;
}

Efm_Bit            *
efm_bit_remove(Efm_Bit * bbit, Efm_Bit * b)
{
   Efm_Bit            *bb, *pb = NULL;

   bb = bbit;
   while (bb)
     {
	if (bb == b)
	  {
	     if (pb)
	       {
		  pb->next = b->next;
		  b->next = NULL;
		  return bbit;
	       }
	     else
	       {
		  pb = b->next;
		  b->next = NULL;
		  return pb;
	       }
	  }
	pb = bb;
	bb = bb->next;
     }
   return bbit;
}

void
efm_bit_free(Efm_Bit * b)
{
   IF_FREE(b->name);
   IF_FREE(b->class);
   IF_FREE(b->rel1);
   IF_FREE(b->rel2);
   IF_FREE(b->state);
   if (b->sync)
      e_string_free_list(b->sync, b->sync_num);
   IF_FREE(b->text);
   free(b);
}

Efm_Bit            *
efm_bit_find_named(Efm_Bit * bbit, char *name)
{
   Efm_Bit            *b;

   for (b = bbit; b; b = b->next)
     {
	if (e_string_cmp(name, b->name))
	   return b;
     }
   return NULL;
}

Efm_Bit            *
efm_bit_find_classed(Efm_Bit * bbit, char *class)
{
   Efm_Bit            *b;

   for (b = bbit; b; b = b->next)
     {
	if (e_string_cmp(class, b->class))
	   return b;
     }
   return NULL;
}

void
efm_bit_calculate(Efm_Bit * bbit, Efm_Bit * bit, int w, int h)
{
   int                 x1, y1, x2, y2;
   int                 rx, ry, rw, rh;
   int                 want_w, want_h;

   if (bit->calc_pending)
      return;
   bit->calc_pending = 1;
   if (bit->rel1)
     {
	Efm_Bit            *b;

	b = efm_bit_find_named(bbit, bit->rel1);
	if (b)
	  {
	     efm_bit_calculate(bbit, b, w, h);
	     rx = b->x;
	     ry = b->y;
	     rw = b->w;
	     rh = b->h;
	  }
	else
	  {
	     rx = 0;
	     ry = 0;
	     rw = w;
	     rh = h;
	  }
     }
   else
     {
	rx = 0;
	ry = 0;
	rw = w;
	rh = h;
     }
   x1 = rx + ((rw + bit->rax1) * bit->rx1) + bit->x1;
   y1 = ry + ((rh + bit->ray1) * bit->ry1) + bit->y1;
   if (bit->rel2)
     {
	Efm_Bit            *b;

	b = efm_bit_find_named(bbit, bit->rel2);
	if (b)
	  {
	     efm_bit_calculate(bbit, b, w, h);
	     rx = b->x;
	     ry = b->y;
	     rw = b->w;
	     rh = b->h;
	  }
	else
	  {
	     rx = 0;
	     ry = 0;
	     rw = w;
	     rh = h;
	  }
     }
   else
     {
	rx = 0;
	ry = 0;
	rw = w;
	rh = h;
     }
   x2 = rx + ((rw + bit->rax2) * bit->rx2) + bit->x2;
   y2 = ry + ((rh + bit->ray2) * bit->ry2) + bit->y2;

   bit->x = x1;
   bit->y = y1;

   want_w = bit->w = x2 - x1 + 1;
   want_h = bit->h = y2 - y1 + 1;

   bit->w = (bit->w / bit->step_w) * bit->step_w;
   bit->h = (bit->h / bit->step_h) * bit->step_h;

   if ((bit->aspect_h > 0) && (bit->aspect_v > 0))
     {
        int hh, ww;
	
	hh = (bit->w * bit->aspect_v) / bit->aspect_h;
	if (hh > bit->h)
	  {
	     ww = (bit->h * bit->aspect_h) / bit->aspect_v;
	     bit->w = ww;
	  }
	else
	   bit->h = hh;
     }

   if (bit->w < bit->min_w)
      bit->w = bit->min_w;
   if (bit->h < bit->min_h)
      bit->h = bit->min_h;
   if (bit->max_w == 0)
     {
	int                 ww = 0, hh = 0;

	if (bit->icon)
	  {
	     imlib_context_set_image(bit->icon);
	     ww = imlib_image_get_width();
	     hh = imlib_image_get_height();
	  }
	else if ((bit->tdef) && (bit->text))
	  {
	     Efm_T              *t;
	     Imlib_Font          fn;

	     for (t = bit->tdef->txt; t; t = t->next)
	       {
		  if (e_string_cmp(bit->state, t->state))
		     break;
	       }
	     if (!t)
		t = bit->tdef->txt;
	     fn = imlib_load_font(t->font);
	     if (fn)
	       {
		  imlib_context_set_font(fn);
		  imlib_context_set_direction(t->dir);
		  imlib_get_text_size(bit->text, &ww, &hh);
		  imlib_free_font();
	       }
	  }
	bit->x += (bit->w - ww) * bit->align_h;
	bit->w = ww;
     }
   else
     {
	if (bit->w > bit->max_w)
	   bit->w = bit->max_w;
	bit->x -= (bit->w - want_w) * bit->align_h;
     }
   if (bit->max_h == 0)
     {
	int                 ww = 0, hh = 0;

	if (bit->icon)
	  {
	     imlib_context_set_image(bit->icon);
	     ww = imlib_image_get_width();
	     hh = imlib_image_get_height();
	  }
	else if ((bit->tdef) && (bit->text))
	  {
	     Efm_T              *t;
	     Imlib_Font          fn;

	     for (t = bit->tdef->txt; t; t = t->next)
	       {
		  if (e_string_cmp(bit->state, t->state))
		     break;
	       }
	     if (!t)
		t = bit->tdef->txt;
	     fn = imlib_load_font(t->font);
	     if (fn)
	       {
		  imlib_context_set_font(fn);
		  imlib_context_set_direction(t->dir);
		  imlib_get_text_size(bit->text, &ww, &hh);
		  imlib_free_font();
	       }
	  }
	bit->y += (bit->h - hh) * bit->align_v;
	bit->h = hh;
     }
   else
     {
	if (bit->h > bit->max_h)
	   bit->h = bit->max_h;
	bit->y -= (bit->h - want_h) * bit->align_v;
     }
   bit->calculated = 1;
   bit->calc_pending = 0;
}

void
efm_bits_calculate(Efm_Bit * bit, int w, int h)
{
   Efm_Bit            *b;

   for (b = bit; b; b = b->next) b->calculated = 0;
   for (b = bit; b; b = b->next)
     {
	if (!b->calculated)
	   efm_bit_calculate(bit, b, w, h);
     }
}

void
efm_idef_draw(Efm_Imagedef * idef, char *state, Imlib_Image * dst,
	      int x, int y, int w, int h)
{
   Imlib_Image        *im;
   Efm_I              *i;
   int                 iw, ih;

   for (i = idef->img; i; i = i->next)
     {
	if (e_string_cmp(state, i->state))
	   break;
     }
   if (!i)
      i = idef->img;
   if ((i->file) && (i->file[0] != '/'))
     {
	char *f;
	
	f = i->file;
	i->file = e_path_find(path_skins, i->file);
	FREE(f);
     }
   if (!i->file)
       return;
   if (i->file[0] != '/')
     {
	char *str;
	
	str = i->file;
	i->file = e_path_find(path_skins, str);
	FREE(str);
     }
   im = imlib_load_image(i->file);
   if (!im)
      return;
   imlib_context_set_image(im);
   iw = imlib_image_get_width();
   ih = imlib_image_get_height();
   if ((i->border.left > 0) || (i->border.right > 0) ||
       (i->border.top > 0) || (i->border.bottom > 0))
     {
	imlib_image_set_irrelevant_border(1);
	imlib_image_set_border(&(i->border));
     }
   if (i->fill_flag == ID_SCALED)
     {
	imlib_context_set_anti_alias(1);
	imlib_context_set_blend(1);
	imlib_context_set_dither(0);
	imlib_context_set_image(dst);
	imlib_blend_image_onto_image(im, 0, 0, 0, iw, ih, x, y, w, h);
     }
   else if (i->fill_flag == ID_TILED)
     {
	int tx, ty, tw, th;
	
	imlib_context_set_anti_alias(1);
	imlib_context_set_blend(1);
	imlib_context_set_dither(0);
	imlib_context_set_image(dst);
	for (tx = x; tx < (x + w); tx += iw)
	  {
	     for (ty = y; ty < (y + h); ty += ih)
	       {
		  tw = x + w - tx;
		  if (tw > iw)
		     tw = iw;
		  th = y + h - ty;
		  if (th > ih)
		     th = ih;
		  imlib_blend_image_onto_image(im, 0, 0, 0, tw, th, tx, ty, tw, th);
	       }
	  }
     }
   else if (i->fill_flag == ID_INT_TILED)
     {
	imlib_context_set_anti_alias(1);
	imlib_context_set_blend(1);
	imlib_context_set_dither(0);
	imlib_context_set_image(dst);
	/* FIXME - impliment */
     }
   imlib_context_set_image(im);
   imlib_free_image();
}

void
efm_tdef_draw(Efm_Textdef * tdef, char *state, Imlib_Image * dst,
	      int x, int y, int w, int h, char *text)
{
   Imlib_Font          fn;
   Efm_T              *t;
   int                 tw, th;
   char               *text2 = NULL;

   for (t = tdef->txt; t; t = t->next)
     {
	if (e_string_cmp(state, t->state))
	   break;
     }
   if (!t)
      t = tdef->txt;
   fn = imlib_load_font(t->font);
   if (!fn)
      return;
   imlib_context_set_font(fn);
   imlib_context_set_direction(t->dir);
   imlib_get_text_size(text, &tw, &th);
   switch (t->dir)
     {
     case IMLIB_TEXT_TO_RIGHT:
     case IMLIB_TEXT_TO_LEFT:
	if (tw <= w)
	   x += t->h_just * (w - tw);
	else
	  {
	     int                 tl, tr;	/*  i; */

	     text2 = e_string_dup(text);
	     tl = e_string_length(text);
	     tr = 0;
	     while (tw > w)
	       {
		  tr++;
		  if (tr > tl)
		    {
		       e_string_clear(text2);
		       e_string_cat_n(text2, text, 0, 1);
		       break;
		    }
		  e_string_clear(text2);
		  e_string_cat_n(text2, text, 0, (tl - tr) / 2);
		  e_string_cat(text2, "...");
		  e_string_cat(text2, &(text[((tl - tr) / 2) + tr]));
		  imlib_get_text_size(text2, &tw, &th);
		  tr++;
	       }
	     x += t->h_just * (w - tw);
	     text = text2;
	  }
	y += t->v_just * (h - th);
	break;
     case IMLIB_TEXT_TO_DOWN:
     case IMLIB_TEXT_TO_UP:
	if (th <= h)
	   y += t->v_just * (h - th);
	else
	  {
	     int                 tl, tr;	/* i; */

	     text2 = e_string_dup(text);
	     tl = e_string_length(text);
	     tr = 0;
	     while (th > h)
	       {
		  tr++;
		  if (tr > tl)
		    {
		       e_string_clear(text2);
		       e_string_cat_n(text2, text, 0, 1);
		       break;
		    }
		  e_string_clear(text2);
		  e_string_cat_n(text2, text, 0, (tl - tr) / 2);
		  e_string_cat(text2, "...");
		  e_string_cat(text2, &(text[((tl - tr) / 2) + tr]));
		  imlib_get_text_size(text2, &tw, &th);
		  tr++;
	       }
	     y += t->v_just * (h - th);
	     text = text2;
	  }
	x += t->h_just * (w - tw);
	break;
     default:
	break;
     }
   if (t->effect == 0)
     {
	imlib_context_set_image(dst);
	imlib_context_set_color(t->col1.red, t->col1.green, t->col1.blue,
				t->col1.alpha);
	imlib_text_draw(x, y, text);
     }
   imlib_free_font();
   IF_FREE(text2);
}

void
efm_bits_draw(Efm_Bit * bit, Imlib_Image * dst, int ox, int oy)
{
   Efm_Bit            *b;

   for (b = bit; b; b = b->next)
     {
	if ((b->type != BT_NO_DRAW) && (b->visible))
	  {
	     if (b->idef)
	       {
		  efm_idef_draw(b->idef, b->state, dst,
				b->x - ox, b->y - oy, b->w, b->h);
	       }
	     else if (b->icon)
	       {
		  int                 w, h;

		  imlib_context_set_image(b->icon);
		  w = imlib_image_get_width();
		  h = imlib_image_get_height();
		  imlib_context_set_image(dst);
		  imlib_context_set_anti_alias(1);
		  imlib_context_set_blend(1);
		  imlib_context_set_dither(0);
		  imlib_blend_image_onto_image(b->icon, 0,
					       0, 0, w, h,
					       b->x - ox, b->y - oy, b->w,
					       b->h);
	       }
	     else if ((b->text) && (b->tdef))
		efm_tdef_draw(b->tdef, b->state, dst,
			      b->x - ox, b->y - oy, b->w, b->h, b->text);
	  }
     }
}

Efm_Bit            *
efm_bits_get_at_xy(Efm_Bit * bbit, int x, int y)
{
   Efm_Bit            *b, *bb = NULL;

   for (b = bbit; b; b = b->next)
     {
	if ((b->type != BT_DECOR_ONLY) && (b->type != BT_NO_DRAW))
	  {
	     if (XY_IN_RECT(x, y, b->x, b->y, b->w, b->h))
		bb = b;
	  }
     }
   return bb;
}

Efm_Bit            *
efm_bit_new(void)
{
   Efm_Bit            *b;

   b = NEW(Efm_Bit, 1);
   b->name = NULL;
   b->class = NULL;
   b->type = BT_DECORATION;
   b->rel1 = NULL;
   b->rel2 = NULL;
   b->x1 = 0;
   b->y1 = 0;
   b->x2 = -1;
   b->y2 = -1;
   b->rx1 = 0.0;
   b->ry1 = 0.0;
   b->rx2 = 1.0;
   b->ry2 = 1.0;
   b->rax1 = 0;
   b->ray1 = 0;
   b->rax2 = 0;
   b->ray2 = 0;
   b->min_w = 0;
   b->min_h = 0;
   b->max_w = 9999;
   b->max_h = 9999;
   b->step_w = 1;
   b->step_h = 1;
   b->align_h = 0.5;
   b->align_v = 0.5;
   b->aspect_h = 0;
   b->aspect_v = 0;
   b->state = e_string_dup("Normal");
   b->sync_num = 0;
   b->sync = NULL;
   b->text = NULL;
   b->icon = NULL;
   b->visible = 1;
   b->idef = NULL;
   b->tdef = NULL;
   b->data = NULL;
   b->next = NULL;
   b->flags = BF_SHOW_WHEN_SHADED | BF_SHOW_WHEN_NOT_SHADED |
      BF_SHOW_WHEN_ACTIVE | BF_SHOW_WHEN_NOT_ACTIVE |
      BF_SHOW_WHEN_STICKY | BF_SHOW_WHEN_NOT_STICKY |
      BF_SHOW_WHEN_MAXIMIZED | BF_SHOW_WHEN_NOT_MAXIMIZED |
      BF_SHOW_WHEN_GROUPED | BF_SHOW_WHEN_NOT_GROUPED;
   b->x = 0;
   b->y = 0;
   b->w = -1;
   b->h = -1;
   b->calculated = 0;
   b->calc_pending = 0;
   return b;
}

void
efm_bit_append(Efm_Bit * bbit, Efm_Bit * bit)
{
   Efm_Bit            *b;

   for (b = bbit; b; b = b->next)
     {
	if (!b->next)
	  {
	     b->next = bit;
	     return;
	  }
     }
}

void
efm_bit_set_name(Efm_Bit * bit, char *name)
{
   IF_FREE(bit->name);
   bit->name = e_string_dup(name);
}

void
efm_bit_set_class(Efm_Bit * bit, char *class)
{
   IF_FREE(bit->class);
   bit->class = e_string_dup(class);
}

void
efm_bits_free(Efm_Bit * bit)
{
   IF_FREE(bit->name);
   IF_FREE(bit->rel1);
   IF_FREE(bit->rel2);
   IF_FREE(bit->state);
   IF_FREE(bit->text);
   if (bit->sync)
      e_string_free_list(bit->sync, bit->sync_num);
   if (bit->icon)
     {
	imlib_context_set_image(bit->icon);
	imlib_free_image();
     }
   if (bit->next)
      efm_bits_free(bit->next);
   FREE(bit);
}

void
efm_bit_set_text(Efm_Bit * bit, char *text)
{
   IF_FREE(bit->text);
   bit->text = e_string_dup(text);
}

void
efm_bit_set_realtive_1(Efm_Bit * bit, char *name)
{
   IF_FREE(bit->rel1);
   bit->rel1 = e_string_dup(name);
}

void
efm_bit_set_realtive_2(Efm_Bit * bit, char *name)
{
   IF_FREE(bit->rel2);
   bit->rel2 = e_string_dup(name);
}

void
efm_bit_set_state(Efm_Bit * bit, char *state)
{
   int i;
   
   IF_FREE(bit->state);
   bit->state = e_string_dup(state);

   for (i = 0; i < bit->sync_num; i++)
     {
	Efm_Bit            *bb;
	
	bb = efm_bit_find_named(bit, bit->sync[i]);
	if (bb)
	  {
	     IF_FREE(bb->state);
	     bb->state = e_string_dup(bit->state);
	  }
     }
}

void
efm_bit_add_sync(Efm_Bit * bit, char *name)
{
   bit->sync_num++;
   REALLOC_PTR(bit->sync, bit->sync_num);
   bit->sync[bit->sync_num - 1] = e_string_dup(name);
}

Efm_Imagedef       *
efm_imagedef_new(void)
{
   Efm_Imagedef       *idef;

   idef = NEW(Efm_Imagedef, 1);
   idef->name = NULL;
   idef->img = NULL;
   idef->next = NULL;
   return idef;
}

void
efm_imagedef_set_name(Efm_Imagedef * idef, char *name)
{
   IF_FREE(idef->name);
   idef->name = e_string_dup(name);
}

void
efm_imagedef_add_i(Efm_Imagedef * idef, Efm_I * i)
{
   i->next = idef->img;
   idef->img = i;
}

void
efm_imagedef_free(Efm_Imagedef * idef)
{
   IF_FREE(idef->name);
   if (idef->img)
      efm_i_free(idef->img);
   FREE(idef);
}

Efm_Imagedef       *
efm_imagedef_find_by_name(Efm_Imagedef * bidef, char *name)
{
   Efm_Imagedef       *id;

   for (id = bidef; id; id = id->next)
     {
	if (e_string_cmp(id->name, name))
	   return id;
     }
   return NULL;
}

Efm_Imagedef       *
efm_imagedef_remove(Efm_Imagedef * bidef, Efm_Imagedef * idef)
{
   Efm_Imagedef       *id, *pid;

   if (bidef == idef)
     {
	bidef = idef->next;
	idef->next = NULL;
	return bidef;
     }
   for (id = bidef->next, pid = bidef; id; pid = id, id = id->next)
     {
	if (id == idef)
	  {
	     pid->next = id->next;
	     id->next = NULL;
	     return id;
	  }
     }
   return NULL;
}

Efm_Imagedef       *
efm_imagedef_append(Efm_Imagedef * bidef, Efm_Imagedef * idef)
{
   idef->next = bidef;
   return idef;
}

void
efm_imagedef_clean(Efm_Imagedef * idef)
{
   Efm_I              *i, *pi;

   for (i = idef->img, pi = NULL; i;)
     {
	if ((!i->file) || (!i->state) || ((i->file) && (i->file[0] == 0)))
	  {
	     if (pi)
	       {
		  pi->next = i->next;
		  i->next = NULL;
		  efm_i_free(i);
		  i = pi->next;
	       }
	     else
	       {
		  idef->img = i->next;
		  i->next = NULL;
		  efm_i_free(i);
		  i = idef->img;
	       }
	  }
	else
	   i = i->next;
     }
}

Efm_I              *
efm_imagedef_find_i_by_name(Efm_Imagedef * idef, char *name)
{
   Efm_I              *i;

   for (i = idef->img; i; i = i->next)
     {
	if (e_string_cmp(name, i->state))
	   return i;
     }
   return NULL;
}

Efm_I              *
efm_i_new(void)
{
   Efm_I              *i;

   i = NEW(Efm_I, 1);
   i->file = NULL;
   i->state = NULL;
   i->border.left = 0;
   i->border.right = 0;
   i->border.top = 0;
   i->border.bottom = 0;
   i->fill_flag = 0;
   i->next = NULL;
   return i;
}

void
efm_i_set_file(Efm_I * i, char *file)
{
   IF_FREE(i->file);
   i->file = e_string_dup(file);
}

void
efm_i_set_state(Efm_I * i, char *state)
{
   IF_FREE(i->state);
   i->state = e_string_dup(state);
}

void
efm_i_free(Efm_I * i)
{
   IF_FREE(i->file);
   IF_FREE(i->state);
   if (i->next)
      efm_i_free(i->next);
   FREE(i);
}

Efm_Textdef        *
efm_textdef_new(void)
{
   Efm_Textdef        *tdef;

   tdef = NEW(Efm_Textdef, 1);
   tdef->name = NULL;
   tdef->txt = NULL;
   return tdef;
}

void
efm_textdef_free(Efm_Textdef * tdef)
{
   IF_FREE(tdef->name);
   if (tdef->txt)
      efm_t_free(tdef->txt);
   FREE(tdef);
}

void
efm_textdef_set_name(Efm_Textdef * tdef, char *name)
{
   IF_FREE(tdef->name);
   tdef->name = e_string_dup(name);
}

void
efm_textdef_add_t(Efm_Textdef * tdef, Efm_T * t)
{
   t->next = tdef->txt;
   tdef->txt = t;
}

Efm_T              *
efm_t_new(void)
{
   Efm_T              *t;

   t = NEW(Efm_T, 1);
   t->font = NULL;
   t->state = NULL;
   t->col1.red = 255;
   t->col1.green = 255;
   t->col1.blue = 255;
   t->col1.red = 255;
   t->col2.red = 0;
   t->col2.green = 0;
   t->col2.blue = 0;
   t->col2.red = 127;
   t->effect = 0;
   t->h_just = 0.0;
   t->v_just = 0.5;
   t->dir = IMLIB_TEXT_TO_RIGHT;
   t->next = NULL;
   return t;
}

void
efm_t_set_font(Efm_T * t, char *font)
{
   IF_FREE(t->font);
   t->font = e_string_dup(font);
}

void
efm_t_set_state(Efm_T * t, char *state)
{
   IF_FREE(t->state);
   t->state = e_string_dup(state);
}

void
efm_t_free(Efm_T * t)
{
   IF_FREE(t->font);
   IF_FREE(t->state);
   if (t->next)
      efm_t_free(t->next);
   FREE(t);
}

Efm_Image_Object *
efm_bits_load(char *file)
{
   E_DB_File *db;
   Efm_Image_Object *ob;
   char *real;

   real = e_path_find(path_geometry, file);
   if (!real)
      return NULL;
   db = e_db_open_read(real);
   FREE(real);
   if (db)
     {
	int num_tdef = 0, num_idef = 0, num_bit = 0, n, n2;
	Efm_Imagedef *idef;
	Efm_I        *i;
	Efm_Bit      *bit;
	char         *s, buf[4096];
	
	ob = NEW(Efm_Image_Object, 1);
	ob->bits  = NULL;
	ob->idefs = NULL;
	ob->tdefs = NULL;
	ob->x = 0;
	ob->y = 0;
	ob->w = 0;
	ob->h = 0;
	ob->need_recalc = 1;
	ob->need_redraw = 0;
	ob->last_mouseover_bit = NULL;
	ob->clicked = 0;
	ob->visible = 0;
	e_db_int_get(db, "textdef/count",  &num_tdef);
	e_db_int_get(db, "imagedef/count", &num_idef);
	e_db_int_get(db, "bit/count",      &num_bit);
	
	for (n = 0; n < num_idef; n++)
	  {
	     int i_count;
	     
	     i_count = 0;
	     idef = efm_imagedef_new();
	     sprintf(buf, "imagedef/%i/name", n);
	     idef->name = e_db_str_get(db, buf);
	     sprintf(buf, "imagedef/%i/img/count", n);
	     e_db_int_get(db, buf, &i_count);
	     for (n2 = 0; n2 < i_count; n2++)
	       {
		  i = efm_i_new();
		  sprintf(buf, "imagedef/%i/img/%i/file", n, n2);
		  i->file = e_db_str_get(db, buf);
		  sprintf(buf, "imagedef/%i/img/%i/state", n, n2);
		  i->state = e_db_str_get(db, buf);
		  sprintf(buf, "imagedef/%i/img/%i/border/left", n, n2);
		  e_db_int_get(db, buf, &(i->border.left));
		  sprintf(buf, "imagedef/%i/img/%i/border/right", n, n2);
		  e_db_int_get(db, buf, &(i->border.right));
		  sprintf(buf, "imagedef/%i/img/%i/border/top", n, n2);
		  e_db_int_get(db, buf, &(i->border.top));
		  sprintf(buf, "imagedef/%i/img/%i/border/bottom", n, n2);
		  e_db_int_get(db, buf, &(i->border.bottom));
		  sprintf(buf, "imagedef/%i/img/%i/fill_flag", n, n2);
		  e_db_int_get(db, buf, &(i->fill_flag));
		  efm_imagedef_add_i(idef, i);
	       }
	     ob->idefs = efm_imagedef_append(ob->idefs, idef);
	  }
	for (n = 0; n < num_bit; n++)
	  {
	     char *str;
	     int   num;
	     
	     bit = efm_bit_new();
	     if (ob->bits)
		efm_bit_append(ob->bits, bit);
	     else
		ob->bits = bit;
	     sprintf(buf, "bit/%i/name", n);
	     bit->name = e_db_str_get(db, buf);
	     sprintf(buf, "bit/%i/class", n);
	     bit->class = e_db_str_get(db, buf);
	     sprintf(buf, "bit/%i/type", n);
	     e_db_int_get(db, buf, &(bit->type));
	     sprintf(buf, "bit/%i/rel1", n);
	     bit->rel1 = e_db_str_get(db, buf);
	     sprintf(buf, "bit/%i/rel2", n);
	     bit->rel2 = e_db_str_get(db, buf);
	     sprintf(buf, "bit/%i/x1", n);e_db_int_get(db, buf, &(bit->x1));
	     sprintf(buf, "bit/%i/y1", n);e_db_int_get(db, buf, &(bit->y1));
	     sprintf(buf, "bit/%i/x2", n);e_db_int_get(db, buf, &(bit->x2));
	     sprintf(buf, "bit/%i/y2", n);e_db_int_get(db, buf, &(bit->y2));
	     sprintf(buf, "bit/%i/rx1", n);e_db_float_get(db, buf, &(bit->rx1));
	     sprintf(buf, "bit/%i/ry1", n);e_db_float_get(db, buf, &(bit->ry1));
	     sprintf(buf, "bit/%i/rx2", n);e_db_float_get(db, buf, &(bit->rx2));
	     sprintf(buf, "bit/%i/ry2", n);e_db_float_get(db, buf, &(bit->ry2));
	     sprintf(buf, "bit/%i/rax1", n);e_db_int_get(db, buf, &(bit->rax1));
	     sprintf(buf, "bit/%i/ray1", n);e_db_int_get(db, buf, &(bit->ray1));
	     sprintf(buf, "bit/%i/rax2", n);e_db_int_get(db, buf, &(bit->rax2));
	     sprintf(buf, "bit/%i/ray2", n);e_db_int_get(db, buf, &(bit->ray2));
	     sprintf(buf, "bit/%i/min_w", n);e_db_int_get(db, buf, &(bit->min_w));
	     sprintf(buf, "bit/%i/min_h", n);e_db_int_get(db, buf, &(bit->min_h));
	     sprintf(buf, "bit/%i/max_w", n);e_db_int_get(db, buf, &(bit->max_w));
	     sprintf(buf, "bit/%i/max_h", n);e_db_int_get(db, buf, &(bit->max_h));
	     sprintf(buf, "bit/%i/step_w", n);e_db_int_get(db, buf, &(bit->step_w));
	     sprintf(buf, "bit/%i/step_h", n);e_db_int_get(db, buf, &(bit->step_h));
	     sprintf(buf, "bit/%i/align_h", n);e_db_float_get(db, buf, &(bit->align_h));
	     sprintf(buf, "bit/%i/align_v", n);e_db_float_get(db, buf, &(bit->align_v));
	     sprintf(buf, "bit/%i/aspect_h", n);e_db_int_get(db, buf, &(bit->aspect_h));
	     sprintf(buf, "bit/%i/aspect_v", n);e_db_int_get(db, buf, &(bit->aspect_v));
	     sprintf(buf, "bit/%i/flags", n);e_db_int_get(db, buf, &(bit->flags));
	     num = 0;
	     sprintf(buf, "bit/%i/sync_num", n);e_db_int_get(db, buf, &(num));
	     for (n2 = 0; n2 < num; n2++)
	       {
		  sprintf(buf, "bit/%i/sync/%i/name", n, n2);
		  str = e_db_str_get(db, buf);
		  if (str)
		    {
		       efm_bit_add_sync(bit, str);
		       FREE(str);
		    }
	       }
	     sprintf(buf, "bit/%i/idef", n);
	     str = e_db_str_get(db, buf);
	     bit->idef = efm_imagedef_find_by_name(ob->idefs, str);
	     sprintf(buf, "bit/%i/tdef", n);
	     str = e_db_str_get(db, buf);
	     /*           bit->tdef = efm_imagedef_find_by_name(tdefs, str);*/
	     FREE(str);
	  }
	e_db_close(db);
	return ob;
     }
   return NULL;
}

void 
efm_object_bits_free(Efm_Image_Object *ob)
{
   Efm_Imagedef *id;
   Efm_Textdef *td;
   
   if (ob->bits)
      efm_bits_free(ob->bits);
   id = ob->idefs;
   while (id)
     {
	Efm_Imagedef *id2;
	
	id2 = id;
	id = id->next;
	efm_imagedef_free(id2);
     }
   td = ob->tdefs;
   while (td)
     {
	Efm_Textdef *td2;
	
	td2 = td;
	td = td->next;
	efm_textdef_free(td2);
     }
   FREE(ob);
}

void
efm_object_bits_show(Efm_Image_Object *ob)
{
   if ((!ob) || (!ob->bits))
      return;
   if (!ob->visible)
      ob->need_redraw = 1;
   ob->visible = 1;
}

void
efm_object_bits_hide(Efm_Image_Object *ob)
{
   if ((!ob) || (!ob->bits))
      return;
   if (!ob->visible)
      ob->need_redraw = 1;
   ob->visible = 0;
}

void
efm_object_bits_calculate(Efm_Image_Object *ob)
{
   if ((!ob) || (!ob->bits) || (!ob->need_recalc))
      return;
   efm_bits_calculate(ob->bits, ob->w, ob->h);
}

void
efm_object_bits_move_resize(Efm_Image_Object *ob, int x, int y, int w, int h)
{
   if (!ob)
      return;
   if ((ob->x != x) && (ob->visible))
      ob->need_redraw = 1;
   if ((ob->y != y) && (ob->visible))
      ob->need_redraw = 1;
   if (ob->w != w)
     {
	ob->need_recalc = 1;
	if (ob->visible)
	   ob->need_redraw = 1;
     }
   if (ob->h != h)
     {
	ob->need_recalc = 1;
	if (ob->visible)
	   ob->need_redraw = 1;
     }
   ob->x = x;
   ob->y = y;
   ob->w = w;
   ob->h = h;
}

void
efm_object_bits_draw(Efm_Image_Object *ob, Imlib_Image *dst, int ox, int oy)
{
   if ((!ob) || (!ob->bits))
      return;
   efm_bits_draw(ob->bits, dst, -ob->x + ox, -ob->y + oy);   
   ob->px = ob->x;
   ob->py = ob->y;
   ob->pw = ob->w;
   ob->ph = ob->h;
   ob->need_redraw = 0;
}

void
efm_object_bits_handle_event(Efm_Image_Object *ob, Eevent *ev)
{
   Efm_Bit *bit;
   
   if ((!ob) || (!ob->bits))
      return;
   switch (ev->type)
     {
     case EV_MOUSE_MOVE:
	  {
	     Ev_Mouse_Move *e;
	     
	     e = ev->event;
	     if (!ob->clicked)
	       {
		  bit = efm_bits_get_at_xy(ob->bits, e->x - ob->x, e->y - ob->y);
		  if (bit != ob->last_mouseover_bit)
		    {
		       if (ob->last_mouseover_bit)
			 {
			    /* unhilite bit */
			    efm_bit_set_state(ob->last_mouseover_bit, "Normal");
			    ob->need_redraw = 1;
			 }
		       ob->last_mouseover_bit = bit;
		       if (ob->last_mouseover_bit)
			 {
			    /* hilite bit */
			    efm_bit_set_state(ob->last_mouseover_bit, "Hilited");
			    ob->need_redraw = 1;
			 }
		    }
	       }
	  }
	break;
     case EV_MOUSE_DOWN:
	  {
	     Ev_Mouse_Down *e;
	     
	     e = ev->event;
	     if (e->button == 1)
	       {
		  bit = efm_bits_get_at_xy(ob->bits, e->x - ob->x, e->y - ob->y);
		  ob->last_mouseover_bit = bit;
		  if (ob->last_mouseover_bit)
		    {
		       /* set bit to clicked */
		       efm_bit_set_state(ob->last_mouseover_bit, "Clicked");
		       ob->need_redraw = 1;
		    }
		  ob->clicked = 1;
	       }
	  }
	break;
     case EV_MOUSE_UP:
	  {
	     Ev_Mouse_Up *e;
	     
	     e = ev->event;
	     if (e->button == 1)
	       {
		  if (ob->last_mouseover_bit)
		    {
		       /* unset clicked state */
		       efm_bit_set_state(ob->last_mouseover_bit, "Hilited");
		       ob->need_redraw = 1;
		    }
		  bit = efm_bits_get_at_xy(ob->bits, e->x - ob->x, e->y - ob->y);
		  if ((!bit) && (ob->last_mouseover_bit))
		    {
		       /* unhilite bit */
		       efm_bit_set_state(ob->last_mouseover_bit, "Normal");
		       ob->need_redraw = 1;
		    }
		  ob->last_mouseover_bit = bit;
		  ob->clicked = 0;
	       }
	  }
	break;
     case EV_MOUSE_IN:
	  {
	     Ev_Window_Enter *e;
	     
	     e = ev->event;
	     if (!ob->clicked)
	       {
		  bit = efm_bits_get_at_xy(ob->bits, e->x - ob->x, e->y - ob->y);
		  ob->last_mouseover_bit = bit;
		  if (ob->last_mouseover_bit)
		    {
		       /* hilite bit */
		       efm_bit_set_state(ob->last_mouseover_bit, "Hilited");
		       ob->need_redraw = 1;
		    }
	       }
	  }
	break;
     case EV_MOUSE_OUT:
	  {
	     Ev_Window_Leave *e;
	     
	     e = ev->event;
	     if (!ob->clicked)
	       {
		  if (ob->last_mouseover_bit)
		    {
		       /* unhilite bit */
		       efm_bit_set_state(ob->last_mouseover_bit, "Normal");
		       ob->need_redraw = 1;
		    }
		  ob->last_mouseover_bit = NULL;
	       }
	  }
	break;
     default:
	break;
     }
}

int
efm_object_bits_get_classed_coords(Efm_Image_Object *ob, char *class, int *x, int *y, int *w, int *h)
{
   Efm_Bit *bit;
   
   if ((!ob) || (!ob->bits))
      return 0;
   bit = efm_bit_find_classed(ob->bits, class);
   if (!bit)
     {
	if (x) *x = 0;
	if (y) *y = 0;
	if (w) *w = 0;
	if (h) *h = 0;
	return 0;
     }
   if (x) *x = ob->x + bit->x;
   if (y) *y = ob->y + bit->y;
   if (w) *w = bit->w;
   if (h) *h = bit->h;
   return 1;
}

Imlib_Updates
efm_object_bits_get_update(Efm_Image_Object *ob, Imlib_Updates updates)
{
   if ((!ob) || (!ob->bits) || (!ob->need_redraw))
      return updates;
   updates = imlib_update_append_rect(updates, ob->px, ob->py, ob->pw, ob->ph);
   updates = imlib_update_append_rect(updates, ob->x, ob->y, ob->w, ob->h);
   ob->px = ob->x;
   ob->py = ob->y;
   ob->pw = ob->w;
   ob->ph = ob->h;
   return updates;
}

int 
efm_object_bits_draw_pending(Efm_Image_Object *ob)
{
   if ((!ob) || (!ob->bits) || (!ob->need_redraw))
      return 0;
   return 1;
}

int
efm_object_bits_in_classed(Efm_Image_Object *ob, char *class, int x,int y)
{
   Efm_Bit *bit;
   
   if ((!ob) || (!ob->bits))
      return 0;
   bit = efm_bits_get_at_xy(ob->bits, x - ob->x, y - ob->y);
   if (!bit)
      return 0;
   if (e_string_cmp(bit->class, class))
      return 1;
   return 0;       
}

int
efm_object_bits_get_minmax(Efm_Image_Object *ob, int *minw, int *minh, int *maxh, int *maxw)
{
   Efm_Bit *bit;
   
   if ((!ob) || (!ob->bits))
      return 0;   
   bit = efm_bit_find_classed(ob->bits, "Geometry_Information");
   if (!bit)
      return 0;
   if (minw)
      *minw = bit->min_w;
   if (minh)
      *minh = bit->min_h;
   if (maxw)
      *maxw = bit->max_w;
   if (maxh)
      *maxh = bit->max_h;
   return 1;
}

int
efm_object_bits_get_insets(Efm_Image_Object *ob, int *padl, int *padr, int *padt, int *padb)
{
   Efm_Bit *bit;
   
   if ((!ob) || (!ob->bits))
      return 0;   
   bit = efm_bit_find_classed(ob->bits, "Geometry_Information");
   if (!bit)
      return 0;
   if (padl)
      *padl = bit->x1;
   if (padr)
      *padr = -1 - bit->x2;
   if (padt)
      *padt = bit->y1;
   if (padb)
      *padb = -1 - bit->y2;
   return 1;
}

void
efm_object_bits_set_entire_state(Efm_Image_Object *ob, char *state)
{
   Efm_Bit *b;
   
   if ((!ob) || (!ob->bits))
      return;
   
   for (b = ob->bits; b; b = b->next)
      efm_bit_set_state(b, state);   
   ob->need_redraw = 1;   
}

void
efm_object_draw_at(Efm_Image_Object *ob, int x, int y, int w, int h, Imlib_Image dst, int ox, int oy)
{
   if ((!ob) || (!ob->bits))
      return;
   
   efm_object_bits_show(ob);
   efm_object_bits_move_resize(ob, x, y, w, h); 
   efm_object_bits_calculate(ob);
   efm_object_bits_draw(ob, dst, ox, oy);
}
