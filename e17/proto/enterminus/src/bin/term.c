#include "term.h"


void term_tcanvas_fg_color_set(Term *term, int c) {
   Term_TGlyph *gl;
   gl = &term->tcanvas->grid[term->tcanvas->cur_col + (term->tcanvas->cur_row * term->tcanvas->cols)];
   gl->changed = 1;
   gl->fg = c;
   term->tcanvas->cur_fg = c;   
   term->tcanvas->changed_rows[term->tcanvas->cur_row] = 1;
}

void term_tcanvas_bg_color_set(Term *term, int c) {
   Term_TGlyph *gl;
   gl = &term->tcanvas->grid[term->tcanvas->cur_col + (term->tcanvas->cur_row * term->tcanvas->cols)];
   gl->changed = 1;
   gl->bg = c;
   term->tcanvas->cur_bg = c;
   term->tcanvas->changed_rows[term->tcanvas->cur_row] = 1;
}

void term_tcanvas_glyph_push(Term *term, char c) {

   Term_TGlyph *gl;
   int j;
   gl = &term->tcanvas->grid[term->tcanvas->cur_col + (term->tcanvas->cur_row * term->tcanvas->cols)];
   gl->changed = 1;
   gl->c = c;
   gl->fg = term->tcanvas->cur_fg;
   gl->bg = term->tcanvas->cur_bg;   
   term->tcanvas->changed_rows[term->tcanvas->cur_row] = 1;
   term->tcanvas->cur_col++;  
   
   if(term->tcanvas->cur_col > term->tcanvas->cols) {
      term->tcanvas->cur_col = 0;
      term->tcanvas->cur_row++;
      for(j = 0; j <= term->tcanvas->cols; j++) {
	 gl = & term->tcanvas->grid[j + (term->tcanvas->cols * term->tcanvas->cur_row)];
	 gl->c = ' ';
	 gl->changed = 1;
      }
   }
   
   return;   
}

char term_tcanvas_data_pop(Term *term) {
   if(term->data_ptr >= term->data_len)
     return 0;
   return term->data[term->data_ptr++];
}

/* look for new characters on the terminal device */
int term_tcanvas_data(void *data) {
   char c;
   Term *term = data;
   term->data_ptr = 0;	      /* THIS  | WAS NOT HERE */
   term->data_len = read(term->cmd_fd.sys, &term->data[0], sizeof(term->data));
   if( term->data_len > 0 ) {
      while ( (c = term_tcanvas_data_pop(term)) ) {
	 //printf("%c",c);
	 switch(c) {
	  case '\007': /* Bell */
	    
	    break;
	  case '\010': /* backspace */
	    term->tcanvas->cur_col--;
	    //term_tcanvas_glyph_push(term, ' ');
	    
	    break;
	  case '\011': /* tab */
	    
	    break;
	  case '\033': /* escape */
	    term_handler_escape_seq(term);
	    break;
	  case '\n': /* newline */
	    term->tcanvas->cur_col = 0;
	    term->tcanvas->cur_row++;
	    /* TODO: Remember to scroll */
	    
	      {
		 int j;
		 Term_TGlyph *gl;
		 for(j = 0; j <= term->tcanvas->cols; j++) {
		    gl = & term->tcanvas->grid[j + (term->tcanvas->cols * term->tcanvas->cur_row)];
		    gl->c = ' ';
		    gl->changed = 1;
		 }
	      }	    
	    
	    if(term->tcanvas->cur_row >= term->tcanvas->scroll_region_end) {
	       term_scroll_up(term, term->tcanvas->cur_row - term->tcanvas->scroll_region_end);
	       term->tcanvas->cur_row = term->tcanvas->scroll_region_end;
	    }
	    break;
	  case '\r': /* carriage return */
	    term->tcanvas->cur_col = 0;	    
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
Term_TCanvas *term_tcanvas_new() {

   int i;
   Term_TGlyph *gl;
   Term_TCanvas *canvas = malloc(sizeof(Term_TCanvas));

   canvas->canvas_id = 1; /* change later */
   canvas->rows = 24; /* multiply by a number or scrollback */  
   canvas->cols = 80;
   canvas->scroll_size = 30; /* this means rows * 3 total rows */
   canvas->cur_row = 0;   /* between 0 and rows-1 */
   canvas->cur_col = 0;
   canvas->grid = calloc(canvas->cols * 
			 canvas->rows *
			 canvas->scroll_size, sizeof(Term_TGlyph));
   canvas->changed_rows = malloc(canvas->rows * canvas->scroll_size * 
				 sizeof(int));
   
   canvas->scroll_region_start = 0;
   canvas->scroll_region_end = canvas->rows -1;
   
   for(i = 0; i < canvas->rows * canvas->scroll_size; i++)
     canvas->changed_rows[i] = 0;
   
   if(canvas->grid == NULL || canvas->changed_rows == NULL) {
      fprintf(stderr, "Could not allocate memory for grid!");
      exit(-1);
   }
   
   for(i = 0; i < canvas->cols * canvas->rows * canvas->scroll_size; i++) {
      gl = &canvas->grid[i];
      gl->c = '\0';
      gl->changed = 0;            
   }
   
   return canvas;
}

/*
 * get the max width this font at this size could have
 */
int term_font_get_width(Term *term) {
   int x, y, w, h;
   Evas_Object *ob;
   ob = evas_object_text_add(term->evas);
   evas_font_path_append(term->evas, term->font.path);
   evas_object_text_font_set(ob, term->font.face, term->font.size);
   evas_object_text_text_set(ob, "W");
   evas_object_geometry_get(ob,&x,&y,&w,&h);
   evas_object_del(ob);
   return w;
}

/*
 * get the max height this font at this size could have
 */
int term_font_get_height(Term *term) {
   int x, y, w, h;
   Evas_Object *ob;
   ob = evas_object_text_add(term->evas);
   evas_font_path_append(term->evas, term->font.path);
   evas_object_text_font_set(ob, term->font.face, term->font.size);
   evas_object_text_text_set(ob, "W");
   evas_object_geometry_get(ob,&x,&y,&w,&h);
   evas_object_del(ob);
   return h;
}

Term *term_new(Ecore_Evas *ee) {
   int i, j;
   Term_EGlyph *gl;
   Term *term = malloc(sizeof(Term));   
   term->term_id = 0;
   term->ee = ee;
   term->evas = ecore_evas_get(ee);   
   term->tcanvas = term_tcanvas_new();
   term->grid = calloc(term->tcanvas->cols * term->tcanvas->rows,
		       sizeof(Term_EGlyph));
         
   for(i = 0; 
       i < term->tcanvas->cols * term->tcanvas->rows; i++) {
      gl = &term->grid[i];
      gl->text = evas_object_text_add(term->evas);      
   }   
   
   term->bg = NULL;
   strcpy(term->font.path, DATADIR);
   strcpy(term->font.face, "VeraMono");
   term->font.size = 10;
   term->data_ptr = 0;   
   term->font.width = term_font_get_width(term);
   term->font.height = term_font_get_height(term);
   evas_font_path_append(term->evas, term->font.path);
   ecore_timer_add(0.1, term_timers, term);   
   
   ecore_x_window_prop_step_size_set(ecore_evas_software_x11_window_get(term->ee),
				      term_font_get_width(term),
				      term_font_get_height(term));
   
   ecore_evas_resize(term->ee, 
		     term->tcanvas->cols*term_font_get_width(term), 
		     term->tcanvas->rows*term_font_get_height(term));
   
   ecore_evas_data_set(term->ee, "term", term);
   
   return term;
}
