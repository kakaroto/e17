#include "term.h"

void term_handler_xterm_seq(int op, Term *term) {
   char buf[512];
   int len;
   int buflen;
   unsigned char c;
   
   c = term_tcanvas_data_pop(term);
   for(len = 0; c != '\007'; len++) {
      
      buf[len] = c;
      
      c = term_tcanvas_data_pop(term);
   }
   buf[len] = 0;   
   
   switch(op) {
    case 0: /* set window and icon title */
    case 1: /* set icon title */
    case 2: /* set window title */      
      ecore_x_window_prop_title_set(ecore_evas_software_x11_window_get(term->ee),
				    buf);
      break;
   }
}


int term_handler_escape_seq(Term *term) {
   int len;
   int pos;
   int args[NPAR];
   int narg = 0;
   int digit;
   int rows, cols;
   int i;
   int questionmark;
   unsigned char c;

   for(pos = 0; pos < NPAR; pos++)
     args[pos] = 0;

   c = term_tcanvas_data_pop(term);
   
   switch(c) {
    case '[': /* CSI */
      digit = 0;
      questionmark = 0;
      c = term_tcanvas_data_pop(term);
      while(isdigit(c) || c == ';' || c == '?')	{
	 if(c == ';') {
	    args[narg] = 0;
	    digit = 0;
	 }
	 else {
	    if(c == '?')
	      questionmark = 1;
	    else {
	       if(!digit)
		 narg++;
	       digit = 1;
	       args[narg-1] *= 10;
	       args[narg-1] += c - '0';
	    }
	 }
	 c = term_tcanvas_data_pop(term);
      }

      switch(c) {
       case 'A':
	 term_cursor_move_up(term, narg ? args[0] : 1);
	 break;
       case 'B':
	 term_cursor_move_down(term, narg ? args[0] : 1);
	 break;
       case 'C':
	 term_cursor_move_right(term, narg ? args[0] : 1);
	 break;
       case 'D':
	 term_cursor_move_left(term, narg ? args[0] : 1);
	 break;
       case 'G':
	 term_cursor_move_col(term, narg ? args[0] : 1);
	 break;
       case 'H':
	 term_cursor_goto(term, args[1] ? args[1] : 1, args[0] ? args[0] : 1);
	 break;
       case 'J':
	 if(narg) {
	    if(args[0] == 1) {
	       /* erase from start to cursor */
	       term_clear_area(term, 1, 1, 
			       term->tcanvas->cols, term->tcanvas->cur_row);
	    }
	    if(args[0] == 2) {
	       /* erase whole display */
	       term_clear_area(term, 1, 1, 
			       term->tcanvas->cols, term->tcanvas->rows);
	    }
	 }
	 else {
	    /* erase from cursor to end of display */
	    term_clear_area(term, 1, term->tcanvas->cur_row,
		       term->tcanvas->cols, term->tcanvas->rows);	    
	 }
	 break;
       case 'K':
	 if(narg) {
	    if(args[0] == 1) {
	       /* erase from start of line to cursor */
	       term_clear_area(term, 1, term->tcanvas->cur_row, 
			       term->tcanvas->cur_col, term->tcanvas->cur_row);
	    }
	    if(args[0] == 2) {
	       /* erase whole line */
	       term_clear_area(term, 1, term->tcanvas->cur_row, 
			       term->tcanvas->cols, term->tcanvas->cur_row);
	    }
	 }
	 else {
	    /* erase from cursor to end of line */
	    term_clear_area(term, term->tcanvas->cur_col, 
			    term->tcanvas->cur_row, 
			    term->tcanvas->cols, term->tcanvas->cur_row);
	 }
	 break;
       case 'L': /* Insert lines */
	 //insert_lines(narg ? args[0] : 1);
	 break;
       case 'M': /* Delete lines */
	 //delete_lines(narg ? args[0] : 1);
	 break;
       case 'd': /* line position absolute */
	 term_cursor_move_row(term, narg ? args[0] : 1);
	 break;
       case 'P': /* clear # of characters */
	 term_clear_area(term, term->tcanvas->cur_col, 
			 term->tcanvas->cur_row, 
			 term->tcanvas->cur_col + args[0], 
			 term->tcanvas->cur_row);
	 break;
       case 'h': /* set mode */
	 switch(args[0]) {
	  case 1:
	    if(questionmark) {
		/* DEC CKM mode */
	      //decckm_mode = 1;
	    }
	    break;
	  case 4:
	    if(!questionmark) {
		/* insert mode */
	      //insert_mode = 1;
	    }
	    break;
	  case 7:
	    if(questionmark) {
	      //wraparound_mode = 1;
	    }
	    break;
	  case 25:
	    if(questionmark) {
	      //cursor_visible = 1;
	    }
	    break;
	  case 47:
	    if(questionmark) {
	       //using_alternate_screen = 1;
	       //save_current_screen();
	    }
	    break;
	  default:
	    fprintf(stderr, "Unsupported ESC [%s h mode %d\n",
		    questionmark?" ?":"",
		    args[0]);
	    break;
	 }
	 break;
       case 'l': /* reset mode */
	 switch(args[0]) {
	  case 1:
	    if(questionmark) {
		/* DEC CKM mode */
	      //decckm_mode = 0;
	    }
	    break;
	  case 4: /* insert mode */
	    //insert_mode = 0;
	    break;
	  case 7:
	    if(questionmark) {
	      //wraparound_mode = 0;
	    }
	    break;
	  case 25:
	    if(questionmark) {
	      //cursor_visible = 0;
	    }
	    break;
	  case 47:
	    if(questionmark) {
	       //using_alternate_screen = 0;
	       //restore_saved_screen();
	    }
	    break;
	  default:
	    fprintf(stderr, "Unsupported ESC [%s l mode %d\n",
		    questionmark?" ?":"",
		    args[0]);
	    break;
	 }
	 break;
       case 'm':
	  /* reset attrs */
	 if(!narg) {
	    //set_buffer_attrs(0);
	    term_tcanvas_fg_color_set(term, 7);
	    //set_buffer_reverse(0);
	    term_tcanvas_bg_color_set(term, 0);
	 }
	 for(i = 0; i < narg; i++) {
	    if(args[i] == 0) {
	       //set_buffer_attrs(0);
	       term_tcanvas_fg_color_set(term, 7);
	    }
	    else if(args[i] == 1) {
	      //set_buffer_attrs(1);
	    }
	    else if(args[i] == 7) {
	      //set_buffer_reverse(1);
	    }
	    else if(args[i] == 27) {
	      //set_buffer_reverse(0);
	    }
	    else if(args[i] >= 30 && args[i] <= 37) {
	      term_tcanvas_fg_color_set(term, args[i] - 30);
	    }
	    else if(args[i] >= 40 && args[i] <= 47) {
	      term_tcanvas_bg_color_set(term, args[i] - 40);
	    }
	    else if(args[i] == 39) {
	      term_tcanvas_fg_color_set(term, 7);
	    }
	    else if(args[i] == 49) {
	      term_tcanvas_bg_color_set(term, 0);
	    }
	    else
	      fprintf(stderr, "Unsupported mode %d\n", args[i]);
	 }
	 break;
       case 'n':
	  /* status report */
	   {
	      char buf[20];

	      switch(args[0])
		{
		 case 6:
		   /* cursor position */
		   
		    snprintf(buf, sizeof(buf), "\033[%d;%dR",
		    term->tcanvas->cur_row, term->tcanvas->cur_col);
		    write(term->cmd_fd.sys, buf, strlen(buf));
		    
		   //cmd_write(buf, strlen(buf));
		   break;
		 default:
		   fprintf(stderr, "Unknown status request id %d\n", args[0]);
		}
	   }
	 break;
       case 'r': /* set scrolling region */
	 term->tcanvas->scroll_region_start = args[0] ? args[0] : 1;
	 term->tcanvas->scroll_region_end = args[1] ? args[1] : term->tcanvas->rows;
	 if(!narg) {
	    /* Reset scroll region */
	   term->tcanvas->scroll_in_region = 0;
	 }

	 else if(args[0] == 1 && args[1] == term->tcanvas->rows)
	   term->tcanvas->scroll_in_region = 0;
	 else
	   term->tcanvas->scroll_in_region = 1;

	 break;
       case '[': /* echoed function key */
	 term_tcanvas_data_pop(term);
	 break;
       default:
	 fprintf(stderr, "Unsupported CSI sequence ESC [");
	 if(questionmark)
	   fprintf(stderr, " ?");
	 for(i = 0; i < narg; i++)
	   fprintf(stderr, " %d", args[i]);
	 fprintf(stderr, " %c\n", c);
      }
      break;
    case ']': /* xterm sequence */

      digit = 0;
      c = term_tcanvas_data_pop(term);
      while(isdigit(c)) {
	 if(!digit)
	   narg++;
	 digit = 1;
	 args[narg-1] *= 10;
	 args[narg-1] += c - '0';
	 c = term_tcanvas_data_pop(term);
      }
      if(c != ';' || !narg) {
	 fprintf(stderr, "Invalid xterm sequence\n");
	 break;
      }

      term_handler_xterm_seq(args[0], term);
      break;
    case '7': /* save cursor position */
      term->tcanvas->saved_cursor_x = term->tcanvas->cur_col;
      term->tcanvas->saved_cursor_y = term->tcanvas->cur_row;
      break;
    case '8': /* restore cursor position */
      term->tcanvas->cur_col = term->tcanvas->saved_cursor_x;
      term->tcanvas->cur_row = term->tcanvas->saved_cursor_y;
      break;
    case '=': /* set application keypad mode */
      term->tcanvas->app_keypad_mode = 1;
      break;
    case '>': /* set numeric keypad mode */
      term->tcanvas->app_keypad_mode = 0;
      break;
    case 'M': /* reverse linefeed */
      term->tcanvas->cur_row--;
      if(term->tcanvas->cur_row < term->tcanvas->scroll_region_start) {
	 term->tcanvas->cur_row = term->tcanvas->scroll_region_start;
	 term_scroll_down(term, 1);
      }
      break;
    default:
      fprintf(stderr, "Unsupported ESC sequence ESC %c\n", c);
      break;
   }
   return 1;
}

void term_cb_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info){
   Evas_Event_Key_Down *ev = event_info;
   unsigned int key_modifiers = 0;
   char *keyname = strdup(ev->keyname);
   Term *term = data;
   char *buf = NULL;
   
   /* TODO: improve on this code because its stupid */
   if (!strcmp(ev->keyname, "Left")) {
      buf = malloc(7);
      snprintf(buf, sizeof(buf), "\033[D");
   }
   else if (!strcmp(ev->keyname, "Right")) {
      buf = malloc(7);      
      snprintf(buf, sizeof(buf), "\033[C");
   }
   else if (!strcmp(ev->keyname, "Down")) {
      buf = malloc(7);      
      snprintf(buf, sizeof(buf), "\033[B");
   }
   else if (!strcmp(ev->keyname, "Up")) {
      buf = malloc(7);      
      snprintf(buf, sizeof(buf), "\033[A");
   }
   else if (!strcmp(ev->keyname, "Home")) {
   }
   else if (!strcmp(ev->keyname, "End")) {
   }
   else if (!strcmp(ev->keyname, "BackSpace")) {
   }
   else if (!strcmp(ev->keyname, "Delete")) {
   }
   
   if(buf) {
      write(term->cmd_fd.sys, buf, strlen(buf));
   } else {
      if( write(term->cmd_fd.sys, ev->string, 1) < 0) {
	 fprintf(stderr, "Error writing to process: %m\n");
	 //exit(2);
      }
   }
     
   return;

   /* extra stuff, clean up later */
   
   if (evas_key_modifier_is_set_get(ev->modifiers, "Shift"))
     key_modifiers |= TERM_KEY_MODIFIER_SHIFT;
   else if (evas_key_modifier_is_set_get(ev->modifiers, "Alt"))
     key_modifiers |= TERM_KEY_MODIFIER_ALT;
   else if (evas_key_modifier_is_set_get(ev->modifiers, "Control"))
     key_modifiers |= TERM_KEY_MODIFIER_CTRL;
   else if (evas_key_modifier_is_set_get(ev->modifiers, "Meta"))
     key_modifiers |= TERM_KEY_MODIFIER_MOD;
   else if (evas_key_modifier_is_set_get(ev->modifiers, "Super"))
     key_modifiers |= TERM_KEY_MODIFIER_WIN;
   else if (evas_key_modifier_is_set_get(ev->modifiers, "Hyper"))
     key_modifiers |= TERM_KEY_MODIFIER_WIN;

   /* fixup the space char */
   if (!strncmp(keyname, "space", 5))
     {
	free(keyname);
	keyname = strdup(" ");
     }
   /* fixup the return char */
   else if (!strncmp(keyname, "Return", 6))
     {
	free(keyname);
	keyname = (char*)'\n';
	write(term->cmd_fd.sys, "\n", 1);
	return;
     }
   /* fixup upper case chars */
   else if (key_modifiers & TERM_KEY_MODIFIER_SHIFT)
     strupper(keyname);
#if 0
   if (!strcmp(ev->keyname, "Left"))
     term_entry_cursor_left_move(evas_object_smart_data_get(data));
   else if (!strcmp(ev->keyname, "Right"))
     term_entry_cursor_right_move(evas_object_smart_data_get(data));
   else if (!strcmp(ev->keyname, "Down"))
     term_entry_cursor_down_move(evas_object_smart_data_get(data));
   else if (!strcmp(ev->keyname, "Up"))
     term_entry_cursor_up_move(evas_object_smart_data_get(data));
   else if (!strcmp(ev->keyname, "Home"))
     term_entry_cursor_home_move(evas_object_smart_data_get(data));
   else if (!strcmp(ev->keyname, "End"))
     term_entry_cursor_end_move(evas_object_smart_data_get(data));
   else if (!strcmp(ev->keyname, "BackSpace"))
     term_entry_left_delete(evas_object_smart_data_get(data));
   else if (!strcmp(ev->keyname, "Delete"))
     term_entry_right_delete(evas_object_smart_data_get(data));
   else
#endif
     if (!strcmp(ev->keyname, "Return") ||
	 !strcmp(ev->keyname, "KP_Return") ||
	 !strcmp(ev->keyname, "Enter") ||
	 !strcmp(ev->keyname, "KP_Enter")) {
     }
   else if ((ev->keyname && strlen(ev->keyname) == 1) ||
	    !strcmp(keyname, " ")) {
      char *tmp = ev->string;
      //term_key_down(evas_object_smart_data_get(data),tmp[0]);
      //write(term->cmd_fd.sys, tmp, 1);

   }
}

struct winsize *get_font_dim(Term *term)
{
   static struct winsize w;
   w.ws_row = term->tcanvas->rows;
   w.ws_col = term->tcanvas->cols;
   w.ws_xpixel = w.ws_ypixel = 0;
   return &w;
}


void term_cb_resize(Ecore_Evas *ee) {
   int x, y, w, h, w_char, h_char, 
     num_chars_w, num_chars_h, old_size;
   Term *term;
   Term_EGlyph *gl;
   Term_TGlyph *gt;
   
   term = (Term*)ecore_evas_data_get(ee, "term");
   
   ecore_evas_geometry_get(term->ee, &x, &y, &w, &h);
   
   w_char = term_font_get_width(term);
   h_char = term_font_get_height(term);   
   
   num_chars_w = (int)((float)w/(float)w_char);
   num_chars_h = (int)((float)h/(float)h_char);
   
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
   
   term_term_bg_set(term, DATADIR"white.png");
   
}
