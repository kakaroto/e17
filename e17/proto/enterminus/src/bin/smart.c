/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>3
 */
#include "term.h"

/* Evas Smart Object API Wrapping methods */

static Evas_Smart *smart;

/* instantiate a new terminal */
Evas_Object *
term_new(Evas *evas)
{
   Evas_Object *term;
   term = evas_object_smart_add(evas, term_smart_get());
   TERM_EVENT_TITLE_CHANGE = ecore_event_type_new();
   return term;
}

/* knit smart object with its functions */
Evas_Smart *
term_smart_get()
{
   if (smart) return smart;
   smart = evas_smart_new("term",
			  term_smart_add,
			  term_smart_del,
			  term_smart_layer_set,
			  term_smart_raise,
			  term_smart_lower,
			  term_smart_stack_above,
			  term_smart_stack_below,
			  term_smart_move,
			  term_smart_resize,
			  term_smart_show,
			  term_smart_hide,
			  term_smart_color_set,
			  term_smart_clip_set,
			  term_smart_clip_unset,
			  NULL);
   return smart;
}

/* add the term object */
void
term_smart_add(Evas_Object *o)
{
   Term *data;

   data = term_init(o);
   if (!data) return;
   evas_object_smart_data_set(o, data);
}

/* delete the term object */
void
term_smart_del(Evas_Object *o)
{
}

/* we have 3 layers to deal with:
 * 1- evas text objects
 * 2- bg for evas text objects
 * 3- bg for entire termnal
 */
void
term_smart_layer_set(Evas_Object *o, int l)
{
   Term *t;
   int i, j;
   Term_EGlyph *gl;

   t = evas_object_smart_data_get(o);
   evas_object_layer_set(t->bg, l - 2);
   for (i = 0; i < t->tcanvas->rows; i++) {
      for (j = 0; j < t->tcanvas->cols; j++) {
	 gl = &t->grid[i][j];
	 evas_object_layer_set(gl->text, l);
	 /*
	 evas_object_layer_set(gl->bg, l - 1);
	 */
      }
   }
}

void
term_smart_raise(Evas_Object *o)
{
   Term *t;
   int i, j;
   Term_EGlyph *gl;

   t = evas_object_smart_data_get(o);
   evas_object_raise(t->bg);
   for (i = 0; i < t->tcanvas->rows; i++) {
      for (j = 0; j < t->tcanvas->cols; j++) {
	 gl = &t->grid[i][j];
	 evas_object_raise(gl->text);
	 /*
	 evas_object_raise(gl->bg);
	 */
      }
   }
}

void
term_smart_lower(Evas_Object *o)
{
   Term *t;
   int i, j;
   Term_EGlyph *gl;

   t = evas_object_smart_data_get(o);
   evas_object_lower(t->bg);
   for (i = 0; i < t->tcanvas->rows; i++) {
      for (j = 0; j < t->tcanvas->cols; j++) {
	 gl = &t->grid[i][j];
	 evas_object_lower(gl->text);
	 /*
	 evas_object_lower(gl->bg);
	 */
      }
   }
}

void
term_smart_stack_above(Evas_Object *o, Evas_Object *above)
{
   Term *t;
   int i, j;
   Term_EGlyph *gl;

   t = evas_object_smart_data_get(o);
   evas_object_stack_above(t->bg, above);
   for (i = 0; i < t->tcanvas->rows; i++) {
      for (j = 0; j < t->tcanvas->cols; j++) {
	 gl = &t->grid[i][j];
	 evas_object_stack_above(gl->text, above);
	 /*
	 evas_object_stack_above(gl->bg, above);
	 */
      }
   }
}

void
term_smart_stack_below(Evas_Object *o, Evas_Object *below)
{
   Term *t;
   int i, j;
   Term_EGlyph *gl;

   t = evas_object_smart_data_get(o);
   evas_object_stack_below(t->bg, below);
   for (i = 0; i < t->tcanvas->rows; i++) {
      for (j = 0; j < t->tcanvas->cols; j++) {
	 gl = &t->grid[i][j];
	 evas_object_stack_below(gl->text, below);
	 /*
	 evas_object_stack_below(gl->bg, below);
	 */
      }
   }
}

/* implement some sort of offset which will make moving easy */
void
term_smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
}

/* TODO:
 * We need to show evas objects and set their layers after a resize
 *
 * When we resize, if we dont clear the text (which is what we should
 * do, not clear the text) and we just call a normal redraw, then all
 * of out text will get shifted and we get a distorted looking term.
 * We need to copy the old are, resize, and render it onto the canvas
 * as it were without any distortions. We dont get that now because we
 * set all the area to '\0' in our current method, hence, clearing it.
 */
void
term_smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   int x, y, size;
   int num_chars_w, num_chars_h;
   Term *term;
   Term_EGlyph *gl;
   Term_TGlyph *gt;

   /* check for w = 0 or h = 0 */
   if (w == 0) w = 1;
   if (h == 0) h = 1;

   term = evas_object_smart_data_get(o);
   num_chars_w = w / term->font.width;
   num_chars_h = h / term->font.height;

   if ((term->tcanvas->cols == num_chars_w) && (term->tcanvas->rows == num_chars_h))
      return;

   size = term->tcanvas->rows - num_chars_h;
   if (term->cur_row < (term->tcanvas->rows / 2)) {
      /* Change at the bottom */
      term->tcanvas->scroll_region_end -= size;
      if (term->tcanvas->scroll_region_end < 0)
	 term->tcanvas->scroll_region_end += term->tcanvas->scroll_size;
      if (term->tcanvas->scroll_region_end >= term->tcanvas->scroll_size)
	 term->tcanvas->scroll_region_end -= term->tcanvas->scroll_size;
   } else {
      /* Change at the top */
      term->tcanvas->scroll_region_start += size;
      if (term->tcanvas->scroll_region_start < 0)
	 term->tcanvas->scroll_region_start += term->tcanvas->scroll_size;
      else if (term->tcanvas->scroll_region_start >= term->tcanvas->scroll_size)
	 term->tcanvas->scroll_region_start -= term->tcanvas->scroll_size;
   }

   term->tcanvas->grid[0] = realloc(term->tcanvas->grid[0],
				    num_chars_w * term->tcanvas->scroll_size
				    * sizeof(Term_TGlyph));
   for (x = 1; x < term->tcanvas->scroll_size; x++)
      term->tcanvas->grid[x] = &term->tcanvas->grid[x - 1][num_chars_w];
   memset(term->tcanvas->grid[0], 0, num_chars_w * term->tcanvas->scroll_size * sizeof(Term_TGlyph));
   /* FIXME: Initialize new characters if we get bigger */

   /* Mark all visible characters changed */
   if (term->tcanvas->scroll_region_start < term->tcanvas->scroll_region_end) {
      for (x = term->tcanvas->scroll_region_start; x < term->tcanvas->scroll_region_end; x++) {
	 term->tcanvas->changed_rows[x] = 1;
	 for (y = 0; y < num_chars_w; y++) {
	    gt = &term->tcanvas->grid[x][y];
	    gt->changed = 1;
	 }
      }
   } else {
      for (x = term->tcanvas->scroll_region_start; x < term->tcanvas->scroll_size; x++) {
	 term->tcanvas->changed_rows[x] = 1;
	 for (y = 0; y < num_chars_w; y++) {
	    gt = &term->tcanvas->grid[x][y];
	    gt->changed = 1;
	 }
      }

      for (x = 0; x < term->tcanvas->scroll_region_end; x++) {
	 term->tcanvas->changed_rows[x] = 1;
	 for (y = 0; y < num_chars_w; y++) {
	    gt = &term->tcanvas->grid[x][y];
	    gt->changed = 1;
	 }
      }
   }

   size = (num_chars_h * num_chars_w) - (term->tcanvas->cols * term->tcanvas->rows);
   /* Free grid */
   if (size < 0) {
      int start, end;
      start = (num_chars_h * num_chars_w);
      end = (term->tcanvas->cols * term->tcanvas->rows);
      for (x = start; x < end; x++) {
	 gl = &term->grid[0][x];
	 if (gl->text) evas_object_del(gl->text);
	 /*
	    if (gl->bg) evas_object_del(gl->bg);
	    */
      }
   }
   if ((term->grid = realloc(term->grid,
			     num_chars_h
			     * sizeof(Term_EGlyph *))) == NULL) {
      fprintf(stderr, "Fatal: Couldn't not reallocate evas grid!\n");
      exit(-1);
   }

   if ((term->grid[0] = realloc(term->grid[0],
				num_chars_h * num_chars_w
			       	* sizeof(Term_EGlyph))) == NULL) {
      fprintf(stderr, "Fatal: Couldn't not reallocate evas grid!\n");
      exit(-1);
   }
   for (x = 1; x < num_chars_h; x++)
      term->grid[x] = &term->grid[x - 1][num_chars_w];

   /* Init grid */
   if (size > 0) {
      int start, end;
      start = (term->tcanvas->cols * term->tcanvas->rows);
      end = (num_chars_h * num_chars_w);
      for (x = start; x < end; x++) {
	 gl = &term->grid[0][x];
	 gl->text = evas_object_text_add(term->evas);
	 evas_object_layer_set(gl->text, 2);
	 evas_object_show(gl->text);
      }
   }

#if 0
   gl->bg = evas_object_rectangle_add(term->evas);
   evas_object_resize(gl->bg, term->font.width, term->font.height);
   evas_object_color_set(gl->bg, 100, 50, 50, 150);
   evas_object_layer_set(gl->bg, 1);
#endif

   if (ioctl(term->cmd_fd.sys, TIOCSWINSZ, get_font_dim(term)) < 0) {
      fprintf(stderr, "Couldn't set window size: %m\n");
   }

   term->tcanvas->cols = num_chars_w;
   term->tcanvas->rows = num_chars_h;

   term->w = term->font.width * term->tcanvas->cols;
   term->h = term->font.height * term->tcanvas->rows;

   term_term_bg_set(term, DATADIR"black.png");
}

void
term_smart_show(Evas_Object *o)
{
   Term *t;
   int i, j;
   Term_EGlyph *gl;

   t = evas_object_smart_data_get(o);
   evas_object_show(t->bg);
   for (i = 0; i < t->tcanvas->rows; i++) {
      for (j = 0; j < t->tcanvas->cols; j++) {
	 gl = &t->grid[i][j];
	 evas_object_show(gl->text);
	 /* Enabling this isnt really wise at this point, uber slowness */
	 //evas_object_show(gl->bg);
      }
   }
}

void
term_smart_hide(Evas_Object *o)
{
   Term *t;
   int i, j;
   Term_EGlyph *gl;

   t = evas_object_smart_data_get(o);
   evas_object_hide(t->bg);
   for (i = 0; i < t->tcanvas->rows; i++) {
      for (j = 0; j < t->tcanvas->cols; j++) {
	 gl = &t->grid[i][j];
	 evas_object_hide(gl->text);
	 /*
	 evas_object_hide(gl->bg);
	 */
      }
   }
}

void
term_smart_color_set(Evas_Object *o, int r, int g, int b, int a)
{
}

void
term_smart_clip_set(Evas_Object *o, Evas_Object *clip)
{
}

void term_smart_clip_unset(Evas_Object *o)
{
}
