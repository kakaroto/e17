#include "term.h"

/* Evas Smart Object API Wrapping methods */

static Evas_Smart *smart;

/* instantiate a new terminal */
Evas_Object *term_new(Evas *evas) {
   Evas_Object *term;
   term = evas_object_smart_add(evas, term_smart_get());
   return term;
}

/* knit smart object with its functions */
Evas_Smart *term_smart_get() {   
   if(smart) return smart;
   smart = evas_smart_new ("term",
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
			   NULL
			   );
   return smart;
}

/* add the term object */
void term_smart_add(Evas_Object *o) {   
   Term *data;
   
   data = term_init(o);
   if(!data) return;   
   evas_object_smart_data_set(o, data);
}

/* delete the term object */
void term_smart_del(Evas_Object *o) {
}

/* we have 3 layers to deal with:
 * 1- evas text objects
 * 2- bg for evas text objects
 * 3- bg for entire termnal
 */
void term_smart_layer_set(Evas_Object *o, int l) {
   Term *t;
   int i;
   Term_EGlyph *gl;
   
   t = evas_object_smart_data_get(o);
   evas_object_layer_set(t->bg, l-2);
   for(i = 0; i < t->tcanvas->cols * t->tcanvas->rows; i++) {
      gl = &t->grid[i];
      evas_object_layer_set(gl->text, l);
      evas_object_layer_set(gl->bg, l-1);
   }   
}

void term_smart_raise(Evas_Object *o) {
   Term *t;
   int i;
   Term_EGlyph *gl;
   
   t = evas_object_smart_data_get(o);
   evas_object_raise(t->bg);
   for(i = 0; i < t->tcanvas->cols * t->tcanvas->rows; i++) {
      gl = &t->grid[i];
      evas_object_raise(gl->text);
      evas_object_raise(gl->bg);      
   }   
}

void term_smart_lower(Evas_Object *o) {
   Term *t;
   int i;
   Term_EGlyph *gl;
   
   t = evas_object_smart_data_get(o);
   evas_object_lower(t->bg);
   for(i = 0; i < t->tcanvas->cols * t->tcanvas->rows; i++) {
      gl = &t->grid[i];
      evas_object_lower(gl->text);
      evas_object_lower(gl->bg);      
   }      
}

void term_smart_stack_above(Evas_Object *o, Evas_Object *above) {
   Term *t;
   int i;
   Term_EGlyph *gl;
   
   t = evas_object_smart_data_get(o);
   evas_object_stack_above(t->bg, above);
   for(i = 0; i < t->tcanvas->cols * t->tcanvas->rows; i++) {
      gl = &t->grid[i];
      evas_object_stack_above(gl->text, above);
      evas_object_stack_above(gl->bg, above);      
   }
}

void term_smart_stack_below(Evas_Object *o, Evas_Object *below) {
   Term *t;
   int i;
   Term_EGlyph *gl;
   
   t = evas_object_smart_data_get(o);
   evas_object_stack_below(t->bg, below);
   for(i = 0; i < t->tcanvas->cols * t->tcanvas->rows; i++) {
      gl = &t->grid[i];
      evas_object_stack_below(gl->text, below);
      evas_object_stack_below(gl->bg, below);      
   }   
}

/* implement some sort of offset which will make moving easy */
void term_smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y) {
}

void term_smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h) {
   int x, y, old_size;
   int num_chars_w, num_chars_h;
   Term *term;
   Term_EGlyph *gl;
   Term_TGlyph *gt;
   
   /* check for w = 0 or h = 0 */
   if(w == 0)w = 1;
   if(h == 0)h = 1;
   
   term = evas_object_smart_data_get(o);
   term->w = w;
   term->h = h;
   num_chars_w = w/term->font.width;
   num_chars_h = h/term->font.height;

   if(term->tcanvas->cols == num_chars_w && term->tcanvas->rows == num_chars_h)
     return;
     
   /* TODO: Check if we're increasing or decreasing window size */   
   
   old_size = term->tcanvas->cols * term->tcanvas->rows;   
   
   term->tcanvas->cols = num_chars_w;
   term->tcanvas->rows = num_chars_h;
   
   term->tcanvas->scroll_region_start = 0;
   term->tcanvas->scroll_region_end = term->tcanvas->rows - 1;      
   
   if((term->tcanvas->grid = realloc(term->tcanvas->grid, 
				     term->tcanvas->cols * term->tcanvas->rows *
				     term->tcanvas->scroll_size *
				     sizeof(Term_TGlyph))) == NULL) {
      fprintf(stderr,"Fatal: Could not reallocate text grid!\n");
      exit(-1);
   }
   
   /* review this, do we need to subtract:
    * (term->tcanvas->cols * term->tcanvas->rows * term->tcanvas->scroll_size)
    */
   y = (term->tcanvas->cols * term->tcanvas->rows * term->tcanvas->scroll_size)
     - (old_size *  term->tcanvas->scroll_size);
   
   for(x = y ; 
       x <= term->tcanvas->cols * term->tcanvas->rows * term->tcanvas->scroll_size;
       x++) {
      gt = &term->tcanvas->grid[x];
      gt->c = '\0';
   }   
   
   if((term->tcanvas->changed_rows = realloc(term->tcanvas->changed_rows, 
					    term->tcanvas->rows *
					    term->tcanvas->scroll_size *
					    sizeof(int))) == NULL) {
      fprintf(stderr,"Fatal: Could not reallocate changed rows buffer!\n");
      exit(-1);      
   }
   
   for(x = 0; x <= term->tcanvas->rows * term->tcanvas->scroll_size; x++)
     term->tcanvas->changed_rows[x] = 0;
   
   if((term->grid = realloc(term->grid, term->tcanvas->cols * 
			    term->tcanvas->rows *
			    sizeof(Term_EGlyph))) == NULL) {
      fprintf(stderr,"Fatal: Couldnt not reallocate evas grid!\n");
      exit(-1);
   }
   
   y = term->tcanvas->cols * term->tcanvas->rows - 
     (term->tcanvas->cols * term->tcanvas->rows - (old_size));
   
   for(x = y ; x <= term->tcanvas->cols * term->tcanvas->rows; x++) {
      gl = &term->grid[x];
      gl->text = evas_object_text_add(term->evas);
   }
   
   if(ioctl(term->cmd_fd.sys, TIOCSWINSZ, get_font_dim(term)) < 0) {
      fprintf(stderr, "Couldn't set window size: %m\n");
   }
   
   term_term_bg_set(term, DATADIR"black.png");
      
}

void term_smart_show(Evas_Object *o) {
   Term *t;
   int i;
   Term_EGlyph *gl;
   
   t = evas_object_smart_data_get(o);
   evas_object_show(t->bg);
   for(i = 0; i < t->tcanvas->cols * t->tcanvas->rows; i++) {
      gl = &t->grid[i];
      evas_object_show(gl->text);
      evas_object_show(gl->bg);      
   }
}

void term_smart_hide(Evas_Object *o) {
   Term *t;
   int i;
   Term_EGlyph *gl;
   
   t = evas_object_smart_data_get(o);
   evas_object_hide(t->bg);
   for(i = 0; i < t->tcanvas->cols * t->tcanvas->rows; i++) {
      gl = &t->grid[i];
      evas_object_hide(gl->text);
      evas_object_hide(gl->bg);      
   }   
}

void term_smart_color_set(Evas_Object *o, int r, int g, int b, int a) {
}

void term_smart_clip_set(Evas_Object *o, Evas_Object *clip) {
}

void term_smart_clip_unset(Evas_Object *o) {
}
