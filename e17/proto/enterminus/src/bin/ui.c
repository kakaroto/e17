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

void term_term_bg_set(Term *term, char *img) {

   if(!term->bg) {
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

}

/* see what changed chars we have, redraw */
void term_redraw(void *data) {
   int i,i2,j;
   int ig = 0;
   char c[2];
   Term *term = data;
   Evas_Object *ob;
   Term_EGlyph *gl;
   Term_TGlyph *tgl;   
      
   i2 = term->tcanvas->scroll_region_start;
   
   for(i = 0; i < term->tcanvas->rows; i++) {
      if(term->tcanvas->changed_rows[i2] != 1) {
	 i2++;
	 continue;
      }
      /* printf("Rendering c-row %d  g-row %d\n",i2,i); */
      for(j = 0; j < term->tcanvas->cols; j++) {
	 tgl = &term->tcanvas->grid[j + 
				    (term->tcanvas->cols * 
				     (i2)
				     )];	 
	 if(tgl->changed != 1) {
	    continue;
	 }
	 if(tgl->c == '\033') {
	    printf("Got escape in term_redraw()!\n");
	    continue;
	 }
	 
	 if(i + term->tcanvas->scroll_region_start <= (term->tcanvas->rows - 1)*term->tcanvas->scroll_size) {
	    gl = &term->grid[j + (term->tcanvas->cols * i)];
	 } else {
	    printf("Overflowing: [cur_row=%d] [start: %d, end: %d] [ig=%d]\n",term->tcanvas->cur_row,term->tcanvas->scroll_region_start,term->tcanvas->scroll_region_end,ig);	    
	    gl = &term->grid[j + (term->tcanvas->cols * ig)];
	 }
	 
	 evas_object_text_font_set(gl->text, term->font.face, term->font.size);
	 c[0] = tgl->c;
	 c[1] = '\0';
	 evas_object_text_text_set(gl->text, c);
	 	
	 /* this is just temp, move it into its own function later */
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
	 
	 evas_object_move(gl->text, j*term->font.width, i*term->font.height);
	 tgl->changed = 0;
      }
      if(i + term->tcanvas->scroll_region_start > (term->tcanvas->rows - 1)*term->tcanvas->scroll_size) {	 
	 ig++;
      }
      i2++;
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
   x1--;y1--;x2--;y2--;
   if(x1 < 0) x1 = 0; if(x1 > term->tcanvas->cols) x1 = term->tcanvas->cols;
   if(y1 < 0) y1 = 0; if(y1 > term->tcanvas->rows) y1 = term->tcanvas->rows;
   if(x2 < 0) x2 = 0; if(x2 > term->tcanvas->cols) x2 = term->tcanvas->cols;
   if(y2 < 0) y2 = 0; if(y2 > term->tcanvas->rows) y2 = term->tcanvas->rows;  
   printf("Clearing: %d %d, %d %d\n",x1,y1+term->tcanvas->scroll_region_start,x2,y2+term->tcanvas->scroll_region_start);
   for(i = y1; i <= y2; i++) {      
      for(j = x1; j <= x2; j++) {
	 tgl = &term->tcanvas->grid[j + (term->tcanvas->cols * (i + term->tcanvas->scroll_region_start))];
	 if(tgl->c != ' ' && tgl->c != '\0') {
	    tgl->c = '\0';
	    tgl->changed = 1;
	    term->tcanvas->changed_rows[i + term->tcanvas->scroll_region_start] = 1;
	 }
      }   
   }
}

void term_scroll_up(Term *term, int rows) {

   
   int i, i2, j;
   int x,y;
   Term_TGlyph *gl;   
      
   if(term->tcanvas->scroll_region_end + rows >
      (term->tcanvas->rows-1) * term->tcanvas->scroll_size) {
      printf("End gone past max scroll buffer, wrapping\n");                  
      term->tcanvas->scroll_region_end = rows - (((term->tcanvas->rows-1) * 
	term->tcanvas->scroll_size) -  term->tcanvas->scroll_region_end);
      /* we're going back to the top, clear the rows we want to overwrite */
      for(i = 0; i <= term->tcanvas->scroll_region_end; i++) {
	 term->tcanvas->changed_rows[i] = 1;
	 for(j = 0; j <= term->tcanvas->cols; j++) {
	    gl = & term->tcanvas->grid[j + (term->tcanvas->cols * i)];
	    gl->c = ' ';
	    gl->changed = 1;
	 }
      }
   } else {
      term->tcanvas->scroll_region_end+=rows;
      
   }
   
   if(term->tcanvas->scroll_region_start + rows >
      (term->tcanvas->rows-1) * term->tcanvas->scroll_size) {
      printf("Start gone past scroll area max, going back to start\n");      
      term->tcanvas->scroll_region_start = rows - (((term->tcanvas->rows-1) * 
	term->tcanvas->scroll_size) -  term->tcanvas->scroll_region_start);
   } else {
      term->tcanvas->scroll_region_start+= rows;
   }
            
   /* fix this and make it set changed flags properly */

   if(term->tcanvas->scroll_region_start < term->tcanvas->scroll_region_end)
     i2 = term->tcanvas->scroll_region_end;
   else {
      for(i = 0; i <= term->tcanvas->scroll_region_end; i++) {
	 term->tcanvas->changed_rows[i] = 1;
	 for(j = 0; j <= term->tcanvas->cols; j++) {
	    gl = & term->tcanvas->grid[j + (term->tcanvas->cols * i)];
	    gl->changed = 1;
	 }	 
      }
      i2 = (term->tcanvas->rows - 1) * term->tcanvas->scroll_size;
   }
   
   for(i = term->tcanvas->scroll_region_start;
       i <= i2;
       i++) {
      term->tcanvas->changed_rows[i] = 1;
      for(j = 0; j <= term->tcanvas->cols; j++) {
	 gl = & term->tcanvas->grid[j + (term->tcanvas->cols * i)];
	 gl->changed = 1;
      }
   }


   return;
   
   if(term->tcanvas->scroll_in_region) {
      printf("SCROLL IN REGION!!!!!!!!!!!!!!!!!!!!\n");
   } else {
      
   }
}

void term_scroll_down(Term *term, int rows) {
   if(term->tcanvas->scroll_in_region) {
      
   } else {
      
   }
}
