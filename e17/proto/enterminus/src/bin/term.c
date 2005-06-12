/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>3
 */
#include "term.h"

void
term_tcanvas_fg_color_set(Term *term, int c)
{
   Term_TGlyph *gl;
   int pos;

   pos = term->tcanvas->pos + term->cur_row;
   if (pos >= term->tcanvas->size)
      pos -= term->tcanvas->size;

   gl = &term->tcanvas->grid[pos][term->cur_col];
   gl->changed = 1;
   gl->fg = c;
   term->tcanvas->cur_fg = c;
   term->tcanvas->changed_rows[pos] = 1;
}

void
term_tcanvas_bg_color_set(Term *term, int c)
{
   Term_TGlyph *gl;
   int pos;

   pos = term->tcanvas->pos + term->cur_row;
   if (pos >= term->tcanvas->size)
      pos -= term->tcanvas->size;

   gl = &term->tcanvas->grid[pos][term->cur_col];
   gl->changed = 1;
   gl->bg = c;
   term->tcanvas->cur_bg = c;
   term->tcanvas->changed_rows[pos] = 1;
}

void
term_tcanvas_glyph_push(Term *term, char c)
{
   Term_TGlyph *gl;
   int j;
   int pos;

   pos = term->tcanvas->pos + term->cur_row;
   if (pos >= term->tcanvas->size)
      pos -= term->tcanvas->size;

   gl = &term->tcanvas->grid[pos][term->cur_col];
   gl->changed = 1;
   gl->c = c;
   gl->fg = term->tcanvas->cur_fg;
   gl->bg = term->tcanvas->cur_bg;
   term->tcanvas->changed_rows[pos] = 1;
   term->cur_col++;

   if (term->cur_col >= term->cols) {
      term->cur_col = 0;
      term->cur_row++;
      if (term->cur_row >= term->rows) {
	 term_scroll_up(term, 1);
	 term->cur_row = term->rows - 1;
      }
   }

   return;
}

char
term_tcanvas_data_pop(Term *term)
{
   if (term->data_ptr >= term->data_len)
      return 0;
   return term->data[term->data_ptr++];
}

/* look for new characters on the terminal device */
int
term_tcanvas_data(void *data, Ecore_Fd_Handler *fd_handler)
{
   char c;
   Term *term;

   term = data;
   term->data_ptr = 0;	      /* THIS  | WAS NOT HERE */
   term->data_len = read(term->cmd_fd.sys, &term->data[0], sizeof(term->data));
   if (term->data_len > 0) {
      while ((c = term_tcanvas_data_pop(term))) {
	 //printf("%c",c);
	 switch(c) {
	    case '\007': /* Bell */
	       break;
	    case '\010': /* backspace */
	       term->cur_col--;
	       /* FIXME!!! */
	       if (term->cur_col < 0)
		  term->cur_col = 0;
	       //term_tcanvas_glyph_push(term, ' ');
	       break;
	    case '\011': /* tab */
	       break;
	    case '\033': /* escape */
	       term_handler_escape_seq(term);
	       break;
	    case '\n': /* newline */
	       term->cur_col = 0;
	       term->cur_row++;
	       if (term->cur_row >= term->rows) {
		  term_scroll_up(term, 1);
		  term->cur_row = term->rows - 1;
	       }
	       {
		  Term_TGlyph *gl;
		  int j;
		  int pos;

		  pos = term->tcanvas->pos + term->cur_row;
		  if (pos >= term->tcanvas->size)
		     pos -= term->tcanvas->size;

		  for (j = 0; j < term->cols; j++) {
		     gl = &term->tcanvas->grid[pos][j];
		     gl->c = ' ';
		     gl->changed = 1;
		  }
	       }
	       break;
	    case '\r': /* carriage return */
	       term->cur_col = 0;
	       break;
	    default:
	       term_tcanvas_glyph_push(term, c);
	       break;
	 }
      }
   }
   //printf("Exiting TCanvas Data handler\n");
   return 1;
}

/* Create a new text canvas */
Term_TCanvas *
term_tcanvas_new(Term *term)
{

   int i, j;
   Term_TGlyph *gl;
   Term_TCanvas *canvas = malloc(sizeof(Term_TCanvas));

   canvas->canvas_id = 1; /* change later */
   canvas->size = 500; /* number of rows to keep */
   canvas->grid = calloc(canvas->size, sizeof(Term_TGlyph *));
   canvas->changed_rows = calloc(canvas->size, sizeof(int));

   for (i = 0; i < canvas->size; i++) {
      canvas->grid[i] = calloc(term->cols, sizeof(Term_TGlyph));
      for (j = 0; j < term->cols; j++) {
	 gl = &canvas->grid[i][j];
	 gl->c = '\0';
	 gl->changed = 0;
      }
   }

   canvas->pos = 0;

   if (canvas->grid == NULL || canvas->changed_rows == NULL) {
      fprintf(stderr, "Could not allocate memory for grid!");
      exit(-1);
   }

   return canvas;
}

/*
 * get the max width this font at this size could have
 */
int
term_font_get_width(Term *term)
{
   int x, y, w, h;
   Evas_Object *ob;
   ob = evas_object_text_add(term->evas);
   evas_font_path_append(term->evas, term->font.path);
   evas_object_text_font_set(ob, term->font.face, term->font.size);
   evas_object_text_text_set(ob, "W");
   evas_object_geometry_get(ob, &x, &y, &w, &h);
   evas_object_del(ob);
   return w;
}

/*
 * get the max height this font at this size could have
 */
int
term_font_get_height(Term *term)
{
   int x, y, w, h;
   Evas_Object *ob;
   ob = evas_object_text_add(term->evas);
   evas_font_path_append(term->evas, term->font.path);
   evas_object_text_font_set(ob, term->font.face, term->font.size);
   evas_object_text_text_set(ob, "W");
   evas_object_geometry_get(ob, &x, &y, &w, &h);
   evas_object_del(ob);
   return h;
}

Term
*term_init(Evas_Object *o)
{
   int x, y;
   Evas *evas;
   Term_EGlyph *gl;
   Term *term;

   term = malloc(sizeof(Term));
   evas = evas_object_evas_get(o);
   term->term_id = 0;
   term->evas = evas;
   term->rows = 24;
   term->cols = 80;
   term->cur_row = 0;
   term->cur_col = 0;
   term->tcanvas = term_tcanvas_new(term);

   term->grid = calloc(term->rows, sizeof(Term_EGlyph *));
   term->grid[0] = calloc(term->rows * term->cols, sizeof(Term_EGlyph));
   for (x = 0; x < term->rows; x++) {
      if (x > 0)
	 term->grid[x] = &term->grid[x - 1][term->cols];
      for (y = 0; y < term->cols; y++) {
	 gl = &term->grid[x][y];
	 gl->text = evas_object_text_add(term->evas);
	 evas_object_layer_set(gl->text, 2);
	 /*
	 gl->bg = evas_object_rectangle_add(term->evas);
	 evas_object_layer_set(gl->bg, 1);
	 */
      }
   }

   term->bg = NULL;
   strcpy(term->font.path, DATADIR);
   strcpy(term->font.face, "VeraMono");
   term->font.size = 10;
   term->data_ptr = 0;
   term->font.width = term_font_get_width(term);
   term->font.height = term_font_get_height(term);
   term->title = NULL;

   evas_font_path_append(term->evas, term->font.path);
   ecore_timer_add(0.01, term_redraw, term);
   ecore_timer_add(0.095, term_cursor_anim, term);
   execute_command(term);//, argc, argv);
   term->cursor.shape = evas_object_rectangle_add(term->evas);
   evas_object_resize(term->cursor.shape, term->font.width, term->font.height);
   evas_object_color_set(term->cursor.shape, 100, 100, 100, 255);
   evas_object_layer_set(term->cursor.shape, 5);
   evas_object_show(term->cursor.shape);
   term->cursor.last_reset = ecore_time_get();
   term->cmd_fd.ecore =  ecore_main_fd_handler_add(term->cmd_fd.sys,
						   ECORE_FD_READ,
						   term_tcanvas_data, term,
						   NULL, NULL);
   term->w = term->font.width * term->cols;
   term->h = term->font.height * term->rows;
   term_term_bg_set(term, DATADIR"black.png");

   return term;
}
