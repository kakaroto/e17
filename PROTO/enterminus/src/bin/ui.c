/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>3
 */
#include "term.h"

#define COLOR0 255, 255, 255
#define COLOR1 200, 20, 20
#define COLOR2 20, 200, 20
#define COLOR3 150, 250, 20
#define COLOR4 20, 20, 200
#define COLOR5 46, 75, 100
#define COLOR6 98, 175, 200
#define COLOR7 250, 250, 250
#define COLOR8 231, 105, 50

/* set the bg */
/* TODO: This should now be here, what we need to have is a rectangle
 * that will simply capture keys. This should be set by the application
 * that uses the smart object
 */
void
term_term_bg_set(Term *term, char *img)
{

   if (!term->bg) {
      term->bg = evas_object_image_add(term->evas);
      evas_object_event_callback_add(term->bg, EVAS_CALLBACK_KEY_DOWN,
				     term_cb_key_down,
				     term);
   }

   evas_object_resize(term->bg, term->w, term->h);
   evas_object_image_file_set(term->bg, img, NULL);
   evas_object_layer_set(term->bg, 0);
   evas_object_image_fill_set(term->bg, 0, 0, term->w, term->h);
   evas_object_focus_set(term->bg, 1);
   evas_object_show(term->bg);
}

void
term_update_glyph(Term *term, Term_TGlyph *tgl, Term_EGlyph *gl, int i, int j)
{
   char *cc=NULL;

   /*
   c[0] = tgl->c;
   c[1] = '\0';
   */
   if(tgl->uc)
   {
      int ord;
      cc = (char*)malloc((tgl->nbc + 1) * sizeof(char));
      cc[tgl->nbc]='\0';
      for(ord=0;ord < tgl->nbc;ord++)
      {
	 cc[ord]=tgl->uc[ord];
      }      
   }
   evas_object_text_font_set(gl->text, term->font.face, term->font.size);
   evas_object_text_text_set(gl->text, cc);

   /*TODO break this out to the api */
   evas_object_text_style_set(gl->text, EVAS_TEXT_STYLE_SOFT_SHADOW);
   evas_object_text_shadow_color_set (gl->text, 100, 100, 100, 150);
   
   /* this is just temp, move it into its own function later */
   switch (tgl->fg) {
      case 0:
	 evas_object_color_set(gl->text, COLOR0, 255);
	 break;
      case 1:
	 evas_object_color_set(gl->text, COLOR1, 255);
	 break;
      case 2:
	 evas_object_color_set(gl->text, COLOR2, 255);
	 break;
      case 3:
	 evas_object_color_set(gl->text, COLOR3, 255);
	 break;
      case 4:
	 evas_object_color_set(gl->text, COLOR4, 255);
	 break;
      case 5:
	 evas_object_color_set(gl->text, COLOR5, 255);
	 break;
      case 6:
	 evas_object_color_set(gl->text, COLOR6, 255);
	 break;
      case 7:
	 evas_object_color_set(gl->text, COLOR7, 255);
	 break;
      case 8:
	 evas_object_color_set(gl->text, COLOR8, 255);
	 break;
   }
   /* The Layer setting and showing functions need to go away */
   evas_object_move(gl->text, j * term->font.width, i * term->font.height);
   tgl->changed = 0;
}

/* see what changed chars we have, redraw */
int
term_redraw(void *data) {
   int i1, i2, j;
   Term *term;
   Term_EGlyph *gl;
   Term_TGlyph *tgl;

   term = data;

   /* loop over all rows, see what has changed */
   /* the general idea is to only inspect as many rows from the tcanvas
    * as we have in the term->grid. We loop that man times, which is
    * term->rows times, and look for changed flags in each row.
    * Since i (0 -> rows) cant be used to subscript the current rows, we
    * start it from scroll_region_start.
    */
   for (i1 = 0, i2 = term->tcanvas->pos;
	i1 < term->rows; i1++, i2++) {

      if (i2 >= term->tcanvas->size)
	 i2 = 0;

      if (term->tcanvas->changed_rows[i2] != 1)
	 continue;

      /* fetch the text glyph */
      for (j = 0; j < term->cols; j++) {
	 tgl = &term->tcanvas->grid[i2][j];
	 gl = &term->grid[i1][j];
	 if (tgl->changed != 1)
	    continue;

	 /* unsure as to why this is here, I dont think we need it */
	 if ((tgl->nbc > 0) && (tgl->uc[0] == '\033')) {
	    printf("Got escape in term_redraw()!\n");
	    continue;
	 }

	 term_update_glyph(term, tgl, gl, i1, j);
      }
      term->tcanvas->changed_rows[i2] = 0;
   }
   /* display cursor, note: this is still sort of a hack */
   evas_object_move(term->cursor.shape,
		    term->cur_col * term->font.width,
		    term->cur_row * term->font.height);
   return 1;
}

/* Move cursor up n rows*/
int
term_cursor_move_up(Term *term, int n)
{
   term->cur_row -= n;
   if (term->cur_row < 0)
      term->cur_row = 0;
   return term->cur_row;
}

/* Move cursor down n rows */
int
term_cursor_move_down(Term *term, int n)
{
   term->cur_row += n;
   if (term->cur_row >= term->rows)
      term->cur_row = term->rows - 1;
   return term->cur_row;
}

/* Move cursor left n cols */
int
term_cursor_move_left(Term *term, int n)
{
   term->cur_col -= n - 1;
   if (term->cur_col < 0)
      term->cur_col = 0;
   return term->cur_col;
}

/* Move cursor right n cols */
int
term_cursor_move_right(Term *term, int n)
{
   term->cur_col += n - 1;
   if(term->cur_col >= term->cols)
      term->cur_col = term->cols - 1;
   return term->cur_col;
}

/* Move to a certain col */
int
term_cursor_move_col(Term *term, int n)
{
   term->cur_col = n - 1;
   if (term->cur_col < 0)
      term->cur_col = 0;
   if (term->cur_col >= term->cols)
      term->cur_col = term->cols - 1;
   return term->cur_col;
}

/* Move to a certain row */
int
term_cursor_move_row(Term *term, int n)
{
   term->cur_row = n - 1;
   if (term->cur_row < 0)
      term->cur_row = 0;
   if (term->cur_row >= term->rows)
      term->cur_row = term->rows - 1;
   return term->cur_row;
}

/* Move cursor to [x,y] */
void
term_cursor_goto(Term *term, int x, int y)
{
   term->cur_col = x - 1;
   term->cur_row = y - 1;
   if (term->cur_col < 0)
      term->cur_col = 0;
   if (term->cur_col >= term->cols)
      term->cur_col = term->cols - 1;
   if (term->cur_row < 0)
      term->cur_row = 0;
   if (term->cur_row >= term->rows)
      term->cur_row = term->rows - 1;
}

/* Move cursor again to last saved [x,y] */
void
term_cursor_rego(Term *term)
{
   term_cursor_goto(term, term->cur_col, term->cur_row);
}

/* Delete n rows starting from start */
void
term_delete_rows(Term *term, int start, int n)
{
}

/* Add n rows starting from pos */
void
term_add_rows(Term *term, int pos, int n)
{
}

/* Save the current screen */
void
term_tcanvas_save(Term *term)
{
}

/* Restore the last saved screen */
void
term_tcanvas_restore(Term *term)
{
}

/* clear a certain part of the screen */
void
term_clear_area(Term *term, int x1, int y1, int x2, int y2)
{
   int i, j, x;
   Term_TGlyph *tgl;
   /* Points are given in 1-indexed numbers, we work with 0-indexed */
   x1--; y1--; x2--; y2--;

   if (x1 < 0) x1 = 0;
   if (x1 >= term->cols) x1 = term->cols - 1;
   if (y1 < 0) y1 = 0;
   if (y1 >= term->rows) y1 = term->rows - 1;
   if (x2 < 0) x2 = 0;
   if (x2 >= term->cols) x2 = term->cols - 1;
   if (y2 < 0) y2 = 0;
   if (y2 >= term->rows) y2 = term->rows - 1;

   DPRINT((stderr, "Clearing: %d %d, %d %d\n", x1, y1, x2, y2));
   for (i = y1, x = y1 + term->tcanvas->pos; i <= y2; i++, x++) {
      for (j = x1; j <= x2; j++) {
	 if (x >= term->tcanvas->size)
	    x = 0;
	 tgl = &term->tcanvas->grid[x][j];
	 if ((tgl->nbc >= 1) && (tgl->uc[0] != ' ')) {
	    tgl->uc = (char*)realloc(tgl->uc ,1);
	    tgl->uc[0] = ' ';
	    tgl->nbc = 1;
	    tgl->changed = 1;
	    term->tcanvas->changed_rows[x] = 1;
	 }
      }
   }
}

/* scroll window / region upwards */
void
term_scroll_up(Term *term, int rows)
{
   int i, j, x, y;
   Term_TGlyph *tgl;
   Term_EGlyph *gl;

   if (term->scroll_in_region) {
      /* TODO: implement this */
      DPRINT((stderr,"Scrolling: in region between %d and %d\n",
	      term->scroll_region_start,
	      term->scroll_region_end));
      for (i = term->scroll_region_start, x = term->tcanvas->pos + term->scroll_region_start;
	   i < term->scroll_region_end; i++, x++) {
	 if (x >= term->tcanvas->size)
	    x = 0;
	 y = x + rows;
	 if (y >= term->tcanvas->size)
	    y = 0;
	 for (j = 0; j < term->cols; j++) {
	    term->tcanvas->grid[x][j] = term->tcanvas->grid[y][j];
	    tgl = &term->tcanvas->grid[x][j];
	    tgl->changed = 1;
	 }
      }
   } else {
      void *data;

      DPRINT((stderr, "Scrolling: window\n"));
      term->tcanvas->pos += rows;
      if (term->tcanvas->pos >= term->tcanvas->size) {
	 /* Going past the virtual scroll buffer, we need to wrap  */
	 DPRINT((stderr, "End gone past max scroll buffer, wrapping\n"));
	 term->tcanvas->pos -= term->tcanvas->size;
	 /* we're going back to the top, clear the rows we want to overwrite */
	 for (i = 0; i < term->tcanvas->pos; i++) {
	    term->tcanvas->changed_rows[i] = 1;
	    for (j = 0; j < term->cols; j++) {
	       tgl = &term->tcanvas->grid[i][j];
	       tgl->uc = (char*)realloc(tgl->uc ,1); /* Is this EVIL? */
	       tgl->uc[0] = ' ';
	       tgl->changed = 1;
	    }
	 }
      }

      /* rotate screen */
      data = malloc(rows * term->cols * sizeof(Term_EGlyph));
      memcpy(data, term->grid[0], rows * term->cols * sizeof(Term_EGlyph));
      memmove(term->grid[0], term->grid[rows],
	      (term->rows - rows) * term->cols * sizeof(Term_EGlyph));
      memcpy(term->grid[term->rows - rows], data, rows * term->cols * sizeof(Term_EGlyph));
      free(data);
      /* update positions */
      for (i = 0; i < term->rows; i++) {
	 for (j = 0; j < term->cols; j++) {
	    gl = &term->grid[i][j];
	    evas_object_move(gl->text, j * term->font.width, i * term->font.height);
	 }
      }
      /* set changed flags on chars */
      for (i = term->rows - rows, x = term->tcanvas->pos; i < term->rows; i++, x++) {
	 if (x >= term->tcanvas->size)
	    x = 0;
	 term->tcanvas->changed_rows[x] = 1;
	 for (j = 0; j < term->cols; j++) {
	    tgl = &term->tcanvas->grid[x][j];
	    tgl->changed = 1;
	 }
      }
#if 0
      /* set changed flags on chars */
      for (i = 0, x = term->tcanvas->pos; i < term->rows; i++, x++) {
	 if (x >= term->tcanvas->size)
	    x = 0;
	 term->tcanvas->changed_rows[x] = 1;
	 for (j = 0; j < term->cols; j++) {
	    gl = &term->tcanvas->grid[x][j];
	    gl->changed = 1;
	 }
      }
#endif
   }
}

/* scroll window / region down */
void
term_scroll_down(Term *term, int rows)
{
   if (term->scroll_in_region) {

   } else {

   }
}

int term_cursor_anim(Term *term) {
   int a;
   a = 162 + 73 * cos((ecore_time_get() - term->cursor.last_reset) * 2);
   evas_object_color_set(term->cursor.shape, 100, 100, 100, a);
   return 1;
}

/* FIXME!!! */
void term_delete_lines(Term *term, int lines) {
#if 0
   int a, b;
   a = term->scroll_region_start;
   b = term->scroll_in_region;
   term->scroll_region_start = term->cur_row;
   term->scroll_in_region = 0;
   term_scroll_up(term, lines);
   term->scroll_region_start = a;
   term->scroll_in_region = b;
#endif
}
