#include "term.h"


#define COLOR0 255, 255, 255
#define COLOR1 200, 20, 20
#define COLOR2 20, 200, 20
#define COLOR3 150, 250, 20
#define COLOR4 20, 20, 200
#define COLOR5 46, 75, 100
#define COLOR6 98, 175, 200
#define COLOR7 200, 200, 200 
#define COLOR8 231, 105, 50

void term_window_init(Ecore_Evas *ee, Evas *evas) {

   ee = ecore_evas_software_x11_new(0, 0, 0, 0, 640, 480);
   evas =  ecore_evas_get(ee);
   ecore_evas_show(ee);
}

void term_term_bg_set(Term *term, char *img, Ecore_Evas *ee) {
   int w, h;
   Evas_Object *bg;

   w = term->tcanvas->cols*term_font_get_width(term);
   h = term->tcanvas->rows*term_font_get_height(term);
   
   ecore_evas_resize(ee, w, h);

   bg = evas_object_image_add(term->evas);
   evas_object_resize(bg, w, h);
   evas_object_image_file_set(bg, img, NULL);
   evas_object_layer_set(bg, 0);
   //evas_object_image_size_get(ob, &w, &h);
   evas_object_image_fill_set(bg, 0, 0, w, h);
   evas_object_focus_set(bg,1);
   evas_object_event_callback_add(bg, EVAS_CALLBACK_KEY_DOWN, term_cb_key_down,
				  term);
   //evas_object_event_callback_add(bg, EVAS_CALLBACK_KEY_UP, cb_key_up, term);
   evas_object_show(bg);

}

/* see what changed chars we have, redraw */
void term_redraw(void *data) {
   int i,j;
   int ig = 0, jg = 0;
   char c[2];
   Term *term = data;
   Evas_Object *ob;
   Term_EGlyph *gl;
   Term_TGlyph *tgl;   
   for(i = 0; i < term->tcanvas->rows; i++) {
      
      if(term->tcanvas->changed_rows[i] != 1) {
	 continue;
      }
      //printf("I is %d\n",i);
      ig++;
      for(j = 0; j < term->tcanvas->cols; j++) {
	 tgl = &term->tcanvas->grid[j + 
				    (term->tcanvas->cols * 
				     (i+ term->tcanvas->scroll_region_start)
				     )];
	 //printf("Rendering row %d col %d\n",i+ term->tcanvas->scroll_region_start,j);
	 if(tgl->changed != 1)
	   continue;
	 if(tgl->c == '\033') {
	    printf("Got escape in term_redraw()!\n");
	    continue;
	 }
	 jg++; /* TODO: see if we need those extra vars or not */
	 gl = &term->grid[j + (term->tcanvas->cols * i)];

	 //printf("Current location: [%d, %d]\n",j,i);
	 evas_object_text_font_set(gl->text, term->font.face, term->font.size);
	 c[0] = tgl->c;
	 c[1] = '\0';
	 evas_object_text_text_set(gl->text, c);
	 
	 
	 switch(tgl->fg) {
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
	 
	 
	 evas_object_layer_set(gl->text,1);
	 evas_object_move(gl->text,
			  j*term->font.width,
			  i*term->font.height);
	 evas_object_show(gl->text);
	 //printf("showing %c\n",tgl->c);
	 tgl->changed = 0;
      }
      term->tcanvas->changed_rows[i] = 0;
   }
}

/* Move cursor up n rows*/
int term_cursor_move_up(Term *term, int n) {
   term->tcanvas->cur_row -= n-1;
   if(term->tcanvas->cur_row < 0)
     term->tcanvas->cur_row = 0;
   return term->tcanvas->cur_row;
}

/* Move cursor down n rows */
int term_cursor_move_down(Term *term, int n) {
   term->tcanvas->cur_row += n-1;
   if(term->tcanvas->cur_row >= term->tcanvas->rows)
     term->tcanvas->cur_row = term->tcanvas->rows-1;
   return term->tcanvas->cur_row;
}

/* Move cursor left n cols */
int term_cursor_move_left(Term *term, int n) {
   term->tcanvas->cur_col -= n-1;
   if(term->tcanvas->cur_col < 0)
     term->tcanvas->cur_col = 0;
   return term->tcanvas->cur_col;
}

/* Move cursor right n cols */ 
int term_cursor_move_right(Term *term, int n) {
   term->tcanvas->cur_col += n-1;
   if(term->tcanvas->cur_col >= term->tcanvas->cols)
     term->tcanvas->cur_col = term->tcanvas->cols-1;
   return term->tcanvas->cur_col;
}

/* Move to a certain col */
int term_cursor_move_col(Term *term, int n) {
   term->tcanvas->cur_col = n-1;
   if(term->tcanvas->cur_col < 0)
     term->tcanvas->cur_col = 0;
   if(term->tcanvas->cur_col >= term->tcanvas->cols)
     term->tcanvas->cur_col = term->tcanvas->cols-1;
   return term->tcanvas->cur_col;
}

/* Move to a certain row */
int term_cursor_move_row(Term *term, int n) {
   term->tcanvas->cur_row = n-1;
   if(term->tcanvas->cur_row < 0)
     term->tcanvas->cur_row = 0;
   if(term->tcanvas->cur_row >= term->tcanvas->rows)
     term->tcanvas->cur_row = term->tcanvas->rows-1;
   return term->tcanvas->cur_row;
}

/* Move cursor to [x,y] */
void term_cursor_goto(Term *term, int x, int y) {
   term->tcanvas->cur_col = x-1;
   term->tcanvas->cur_row = y-1;
   if(term->tcanvas->cur_col < 0)
     term->tcanvas->cur_col = 0;
   if(term->tcanvas->cur_col >= term->tcanvas->cols)
     term->tcanvas->cur_col = term->tcanvas->cols-1;
   if(term->tcanvas->cur_row < 0)
     term->tcanvas->cur_row = 0;
   if(term->tcanvas->cur_row >=  term->tcanvas->rows)
     term->tcanvas->cur_row =  term->tcanvas->rows-1;
}

/* Move cursor again to last saved [x,y] */
void term_cursor_rego(Term *term) {
   term_cursor_goto(term, term->tcanvas->cur_col, term->tcanvas->cur_row);
}

/* Delete n rows starting from start */
void term_delete_rows(Term *term, int start, int n) {
   int i;
   
}

/* Add n rows starting from pos */
void term_add_rows(Term *term, int pos, int n) {
}

/* Save the current screen */
void term_tcanvas_save(Term *term) {
}

/* Restore the last saved screen */
void term_tcanvas_restore(Term *term) {
}

void term_clear_area(Term *term, int x1, int y1, int x2, int y2) {
   int i, j;  
   Term_TGlyph *tgl;
   /* TODO: Finalize this shit before shipping code out */
   //x1--;y1--;x2--;y2--;
   printf("Clearing: %d %d, %d %d\n",x1,y1,x2,y2);
   for(i = y1; i <= y2; i++) {      
      for(j = x1; j <= x2; j++) {
	 tgl = &term->tcanvas->grid[j + (term->tcanvas->cols * i)];
	 if(tgl->c != ' ' && tgl->c != '\0') {
	    tgl->c = '\0';
	    tgl->changed = 1;
	    term->tcanvas->changed_rows[i-1] = 1;
	 }
      }   
   }
}

void term_scroll_up(Term *term, int rows) {
   if(term->tcanvas->scroll_in_region) {
      
   } else {
      
   }
}

void term_scroll_down(Term *term, int rows) {
   if(term->tcanvas->scroll_in_region) {
      
   } else {
      
   }
}
