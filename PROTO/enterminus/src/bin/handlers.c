/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>3
 */
#include "term.h"

void
term_handler_xterm_seq(int op, Term *term)
{
   char buf[512];
   int len;
   unsigned char c;
   Term_Char *c2;
   Term_Event_Title_Change *e;
   Ecore_Event *event;

   if((c2 = term_tcanvas_data_pop(term)))
      c = c2->car[0];/* FIXME we'll wrap c for now, the code will be reworked*/
   else
      c = 0;
   for (len = 0; c != '\007' && len < 512; len++) {
      buf[len] = c;
      if((c2 = term_tcanvas_data_pop(term)))
	 c = c2->car[0];/* FIXME we'll wrap c for now, the code will be reworked*/
      else
	 c = 0;
   }
   buf[len] = 0;

   switch(op) {
      case 0: /* set window and icon title */
      case 1: /* set icon title */
      case 2: /* set window title */
	 if (term->title) free(term->title);
	 term->title = strdup(buf);
	 e = malloc(sizeof(Term_Event_Title_Change));
	 e->title = strdup(term->title);
	 event = ecore_event_add(TERM_EVENT_TITLE_CHANGE, e,
				 term_event_title_change_free, NULL);
	 break;
   }
}

int
term_handler_escape_seq(Term *term) {
   int pos;
   int args[NPAR];
   int narg = 0;
   int digit;
   int i;
   int questionmark;
   unsigned char c;
   Term_Char *c2;

   for (pos = 0; pos < NPAR; pos++)
     args[pos] = 0;

   if((c2 = term_tcanvas_data_pop(term)))
      c = c2->car[0];
   else
      c = 0;
   switch(c) {
      case '[': /* CSI */
	 digit = 0;
	 questionmark = 0;
	 if((c2 = term_tcanvas_data_pop(term)))
	     c = c2->car[0];
	 else
	    c = 0;
	 while (isdigit(c) || c == ';' || c == '?')	{
	    if (c == ';') {
	       args[narg] = 0;
	       digit = 0;
	    }
	    else {
	       if (c == '?')
		  questionmark = 1;
	       else {
		  if(!digit)
		     narg++;
		  digit = 1;
		  args[narg - 1] *= 10;
		  args[narg - 1] += c - '0';
	       }
	    }
	    if((c2 = term_tcanvas_data_pop(term)))
	       c = c2->car[0];
	    else
	       c = 0;
	 }

	 switch(c) {
	    case 'A':
	       /* ESC [ [ n ] A  move up n times */
	       DPRINT((stderr, "ESC [ [ n ] A  move up n times\n"));
	       term_cursor_move_up(term, narg ? args[0] : 1);
	       break;
	    case 'B':
	       /* ESC [ [ n ] B  move down n times */
	       DPRINT((stderr, "ESC [ [ n ] B  move down n times\n"));
	       term_cursor_move_down(term, narg ? args[0] : 1);
	       break;
	    case 'C':
	       /* ESC [ [ n ] C  move forward n times */
	       DPRINT((stderr, "ESC [ [ n ] C  move forward n times\n"));
	       term_cursor_move_right(term, narg ? args[0] : 1);
	       break;
	    case 'D':
	       /* ESC [ [ n ] D  move backward n times */
	       DPRINT((stderr, "ESC [ [ n ] D  move backward n times\n"));
	       term_cursor_move_left(term, narg ? args[0] : 1);
	       break;
	    case 'E':
	       /* ESC [ [ n ] E  Cursor Down n times and to first col, default 1 */
	       DPRINT((stderr, "ESC [ [ n ] E  Cursor Down n times and to first col\n"));
	       term_cursor_move_down(term, narg ? args[0] : 1);
	       term_cursor_move_col(term, 1);
	       break;
	    case 'F':
	       /* ESC [ [ n ] F  Cursor Up n times and to first col, default 1 */
	       DPRINT((stderr, "ESC [ [ n ] F  Cursor Up n times and to first col\n"));
	       term_cursor_move_up(term, narg ? args[0] : 1);
	       term_cursor_move_col(term, 1);
	       break;
	    case 'G':
	       /* ESC [ [ n ] G  Cursor to Column n (HPA) */
	       DPRINT((stderr, "ESC [ [ n ] G  Cursor to Column n (HPA)\n"));
	       term_cursor_move_col(term, narg ? args[0] : 1);
	       break;
	    case 'H':
	       /* ESC [ [ r ; c ] H  Cursor Pos [row;column] (CUP), default 1;1 */
	       DPRINT((stderr, "ESC [ [ r ; c ] H  Cursor Pos [row;column] (CUP)\n"));
	       term_cursor_goto(term, args[1] ? args[1] : 1, args[0] ? args[0] : 1);
	       break;
	    case 'I':
	       /* ESC [ [ n ] I  Move forward n tab stops, default 1 */
	       DPRINT((stderr, "ESC [ [ n ] I  Move forward n tab stops\n"));
	       break;
	    case 'J':
	       /* ESC [ [ n ] J  Erase in Display (ED)
		* n == 0: Clear Below (default)
		* n == 1: Clear Above
		* n == 2: Clear All
		*/
	       DPRINT((stderr, "ESC [ [ n ] J  Erase in Display (ED)\n"));
	       if (narg) {
		  if (args[0] == 1) {
		     /* erase from start to cursor */
		     term_clear_area(term, 1, 1, term->cols,
				     term->cur_row + 1);
		  }
		  else if (args[0] == 2) {
		     /* erase whole display */
		     term_clear_area(term, 1, 1, term->cols,
				     term->rows + 1);
		  }
	       }
	       else {
		  /* erase from cursor to end of display */
		  term_clear_area(term, 1, term->cur_row + 1,
				  term->cols, term->rows + 1);
	       }
	       break;
	    case 'K':
	       /* ESC [ [ n ] K  Erase in Line (EL)
		* n == 0: Clear to Right (default)
		* n == 1: Clear to Left (EL1)
		* n == 2: Clear All
		*/
	       DPRINT((stderr, "ESC [ [ n ] K  Erase in Line (EL) %d %d\n", term->cur_col, term->cur_row));
	       if (narg) {
		  if (args[0] == 1) {
		     /* erase from start of line to cursor */
		     term_clear_area(term, 1, term->cur_row + 1,
			   term->cur_col, term->cur_row + 1);
		  }
		  else if (args[0] == 2) {
		     /* erase whole line */
		     term_clear_area(term, 1, term->cur_row + 1,
			   term->cols, term->cur_row + 1);
		  }
	       }
	       else {
		  /* erase from cursor to end of line */
		  term_clear_area(term, term->cur_col + 1, term->cur_row + 1,
			term->cols, term->cur_row + 1);
	       }
	       break;
	    case 'L':
	       /* ESC [ [ n ] L  Insert n lines (IL), default 1 */
	       DPRINT((stderr, "ESC [ [ n ] L  Insert n lines (IL)\n"));
	       //insert_lines(narg ? args[0] : 1);
	       break;
	    case 'M':
	       /* ESC [ [ n ] M  Delete n lines (DL), default 1  */
	       DPRINT((stderr, "ESC [ [ n ] M  Delete n lines (DL)\n"));
	       term_delete_lines(term, narg ? args[0] : 1);
	       break;
	    case 'P':
	       /* ESC [ [ n ] P  Delete n characters (DCH), default 1 */
	       DPRINT((stderr, "ESC [ [ n ] P  Delete n characters (DCH)\n"));
	       term_clear_area(term, term->cur_col,
			       term->cur_row + 1,
			       term->cur_col + args[0],
			       term->cur_row + 1);
	       break;
	    case 'W':
	       /* ESC [ [ n ] W  Tabulator functions
		* n == 0: Tab Set (HTS)
		* n == 2: Tab Clear (TBC), clear current column
		* n == 5: Tab Clear (TBC), clear all
		*/
	       DPRINT((stderr, "ESC [ [ n ] W  Tabulator functions\n"));
	       break;
	    case 'X':
	       /* ESC [ [ n ] X  Erase n characters (ECH), default 1 */
	       DPRINT((stderr, "ESC [ [ n ] X  Erase n characters (ECH)\n"));
	       break;
	    case 'Z':
	       /* ESC [ [ n ] Z  Move backward n tabstops, default 1 */
	       DPRINT((stderr, "ESC [ [ n ] Z  Move backward n tabstops\n"));
	       break;
	    case '`':
	       /* ESC [ [ n ]   Same as ESC [ n G (HPA) */
	       DPRINT((stderr, "ESC [ [ n ]   Same as ESC [ n G (HPA)\n"));
	       break;
	    case 'a':
	       /* ESC [ [ n ] a  Same as ESC [ n C (CUF) */
	       DPRINT((stderr, "ESC [ [ n ] a  Same as ESC [ n C (CUF)\n"));
	       break;
	    case 'c':
	       /* ESC [ [ n ] c  Send Device Attributes (DA),
		* default of 0 returns "ESC[?1;2c" indicating
		* a VT100 with advanced video option
		*/
	       DPRINT((stderr, "ESC [ [ n ] c  Send Device Attributes (DA)\n"));
	       break;
	    case 'd':
	       /* ESC [ [ n ] d  Cursor to line n (VPA) */
	       DPRINT((stderr, "ESC [ [ n ] d  Cursor to line n (VPA)\n"));
	       term_cursor_move_row(term, narg ? args[0] : 1);
	       break;
	    case 'e':
	       /* ESC [ [ n ] e  Same as ESC [ n A (CUU) */
	       DPRINT((stderr, "ESC [ [ n ] e  Same as ESC [ n A (CUU)\n"));
	       break;
	    case 'f':
	       /* ESC [ [ r ; c ] f  Horizontal and Vertical
		* Position (HVP), default 1;1
		*/
	       DPRINT((stderr, "ESC [ [ r ; c ] f  Horizontal and Vertical Position (HVP)\n"));
	       break;
	    case 'g':
	       /* ESC [ [ n ] g  Tab Clear
		* n == 0: Tab Clear (TBC), clear current column (default)
		* n == 3: Tab Clear (TBC), clear all
		*/
	       DPRINT((stderr, "ESC [ [ n ] g  Tab Clear\n"));
	       break;
	    case 'i':
	       /* ESC [ [ n ] i  Printing
		* n == 4: Disable transparent print mode (MC4)
		* n == 5: Enable transparent print mode (MC5)
		*/
	       DPRINT((stderr, "ESC [ [ n ] i  Printing\n"));
	       break;

	       /* ESC [ n [ ; n ... ] { h | l }
		*
		* h: Set Mode (SM)
		* l: Reset Mode (RM)
		*
		* n == 4: Insert Mode (SMIR)/Replace Mode (RMIR)
		* n == 20: Automatic Newline/Normal Linefeed (LNM)
		*/
	    case 'h': /* set mode */
	       DPRINT((stderr, "ESC [ n [ ; n ... ] h\n"));
	       switch (args[0]) {
		  case 1:
		     if (questionmark) {
			/* DEC CKM mode */
			//decckm_mode = 1;
		     }
		     break;
		  case 4:
		     if (!questionmark) {
			/* insert mode */
			//insert_mode = 1;
		     }
		     break;
		  case 7:
		     if (questionmark) {
			//wraparound_mode = 1;
		     }
		     break;
		  case 25:
		     if (questionmark) {
			//cursor_visible = 1;
		     }
		     break;
		  case 47:
		     if (questionmark) {
			//using_alternate_screen = 1;
			//save_current_screen();
		     }
		     break;
		  default:
		     DPRINT((stderr, "Unsupported ESC [%s h mode %d\n",
			      questionmark?" ?":"",
			      args[0]));
		     break;
	       }
	       break;
	    case 'l': /* reset mode */
	       DPRINT((stderr, "ESC [ n [ ; n ... ] l\n"));
	       switch (args[0]) {
		  case 1:
		     if (questionmark) {
			/* DEC CKM mode */
			//decckm_mode = 0;
		     }
		     break;
		  case 4: /* insert mode */
		     //insert_mode = 0;
		     break;
		  case 7:
		     if (questionmark) {
			//wraparound_mode = 0;
		     }
		     break;
		  case 25:
		     if (questionmark) {
			//cursor_visible = 0;
		     }
		     break;
		  case 47:
		     if (questionmark) {
			//using_alternate_screen = 0;
			//restore_saved_screen();
		     }
		     break;
		  default:
		     DPRINT((stderr, "Unsupported ESC [%s l mode %d\n",
			      questionmark?" ?":"",
			      args[0]));
		     break;
	       }
	       break;
	    case 'm':
	       DPRINT((stderr, "ESC [ n [ ; n ... ] m  Character Attributes (SGR)\n"));
	       /* ESC [ n [ ; n ... ] m  Character Attributes (SGR)
		*
		* n == 0: Normal (default)
		* n == 1/22: Turn bold (bright fg) on/off
		* n == 4/24: Turn underline on/off
		* n == 5/25: Turn "blink" (bright bg) on/off
		* n == 7/27: Turn inverse on/off
		* n == 30/40: foreground/background black
		* n == 31/41: foreground/background red
		* n == 32/42: foreground/background green
		* n == 33/43: foreground/background yellow
		* n == 34/44: foreground/background blue
		* n == 35/45: foreground/background magenta
		* n == 36/46: foreground/background cyan
		* n == 37/47: foreground/background white
		* n == 39/49: foreground/background default
		*/
	       /* reset attrs */
	       if (!narg) {
		  //set_buffer_attrs(0);
		  term_tcanvas_fg_color_set(term, 7);
		  //set_buffer_reverse(0);
		  term_tcanvas_bg_color_set(term, 0);
	       }
	       for (i = 0; i < narg; i++) {
		  if (args[i] == 0) {
		     //set_buffer_attrs(0);
		     term_tcanvas_fg_color_set(term, 7);
		  }
		  else if (args[i] == 1) {
		     //set_buffer_attrs(1);
		  }
		  else if (args[i] == 7) {
		     //set_buffer_reverse(1);
		  }
		  else if (args[i] == 27) {
		     //set_buffer_reverse(0);
		  }
		  else if (args[i] >= 30 && args[i] <= 37) {
		     term_tcanvas_fg_color_set(term, args[i] - 30);
		  }
		  else if (args[i] >= 40 && args[i] <= 47) {
		     term_tcanvas_bg_color_set(term, args[i] - 40);
		  }
		  else if (args[i] == 39) {
		     term_tcanvas_fg_color_set(term, 7);
		  }
		  else if (args[i] == 49) {
		     term_tcanvas_bg_color_set(term, 0);
		  }
		  else
		     DPRINT((stderr, "Unsupported mode %d\n", args[i]));
	       }
	       break;
	    case 'n':
	       /* ESC [ [ n ] n  Device Status Report (DSR)
		*
		* n == 5: Status Report, returns "ESC[0n" ("OK")
		* n == 6: Report Cursor Position (CPR) as "ESC[r;cR"
		* n == 7: Request display name (ignored by default for
		*         security reasons)
		* n == 8: Request version number in window title
		* n == 9: Display pixmap/transparency status in window title
		*/
	       DPRINT((stderr, "ESC [ [ n ] n  Device Status Report (DSR)\n"));
	       /* status report */
	       {
		  char buf[20];

		  switch (args[0])
		  {
		     case 6:
			/* cursor position */

			snprintf(buf, sizeof(buf), "\033[%d;%dR",
				 term->cur_row, term->cur_col);
			write(term->cmd_fd.sys, buf, strlen(buf));
			break;
		     default:
			DPRINT((stderr, "Unknown status request id %d\n", args[0]));
		  }
	       }
	       break;
	    case 'r':
	       /* ESC [ [ t ; b ] r  Set Scrolling Region (CSR),
		* where t is the top row and b is the bottom row,
		* defaults to the full screen
		*/
	       DPRINT((stderr, "ESC [ [ t ; b ] r  Set Scrolling Region (CSR)\n"));
	       /* scroll_region_start should be 0-indexed */
	       term->scroll_region_start = args[0] ? args[0] - 1 : 0;
	       if (term->scroll_region_start < 0)
		  term->scroll_region_start = 0;
	       /* scroll_region_end should be one after the wanted row */
	       term->scroll_region_end = args[1] ? args[1] - 1 : term->rows - 1;
	       if (term->scroll_region_end >= term->rows)
		  term->scroll_region_end = term->rows - 1;

	       if (!narg) {
		  /* Reset scroll region */
		  term->scroll_in_region = 0;
	       }

	       else if ((args[0] == 1) && (args[1] == term->rows))
		  term->scroll_in_region = 0;
	       else
		  term->scroll_in_region = 1;

	       break;
	    case '[': /* echoed function key */
	       term_tcanvas_data_pop(term);
	       break;
	    default:
	       DPRINT((stderr, "Unsupported CSI sequence ESC ["));
	       if (questionmark)
		  DPRINT((stderr, " ?"));
	       for (i = 0; i < narg; i++)
		  DPRINT((stderr, " %d", args[i]));
	       DPRINT((stderr, " %c\n", c));
	 }
	 break;
      case ']': /* xterm sequence */
	 digit = 0;
	 if((c2 = term_tcanvas_data_pop(term)))
	    c = c2->car[0];
	 else
	    c = 0;
	 while (isdigit(c)) {
	    if (!digit)
	       narg++;
	    digit = 1;
	    args[narg - 1] *= 10;
	    args[narg - 1] += c - '0';
	    if((c2 = term_tcanvas_data_pop(term)))
	       c = c2->car[0];
	    else
	       c = 0;
	 }
	 if (c != ';' || !narg) {
	    DPRINT((stderr, "Invalid xterm sequence\n"));
	    break;
	 }

	 term_handler_xterm_seq(args[0], term);
	 break;
      case '7': /* ESC 7 (save cursor position) */
	 DPRINT((stderr, "ESC 7 (save cursor pos)\n"));
	 term->saved_cursor_x = term->cur_col;
	 term->saved_cursor_y = term->cur_row;
	 break;
      case '8': /* ESC 8 (restore cursor position) */
	 DPRINT((stderr, "ESC 8 (restore cursor pos)\n"));
	 term->cur_col = term->saved_cursor_x;
	 term->cur_row = term->saved_cursor_y;
	 break;
      case '=': /* ESC = (set application keypad mode) */
	 DPRINT((stderr, "ESC = (set application keypad mode)\n"));
	 term->tcanvas->app_keypad_mode = 1;
	 break;
      case '>': /* ESC > (set numeric keypad mode) */
	 DPRINT((stderr, "ESC = (set numeric keypad mode)\n"));
	 term->tcanvas->app_keypad_mode = 0;
	 break;
      case 'M': /* ESC M (reverse linefeed) */
	 DPRINT((stderr, "ESC = (reverse linefeed)\n"));
	 term->cur_row--;
	 if (term->cur_row < 0) {
	    /* We moved over the top! Scroll up! */
	    term_scroll_down(term, 1);
	    term->cur_row = 0;
	 }
	 break;
      default:
	 DPRINT((stderr, "Unsupported ESC sequence ESC %c\n", c));
	 break;
   }
   return 1;
}

void
term_cb_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
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

   if (buf) {
      write(term->cmd_fd.sys, buf, strlen(buf));
   } else {
      unsigned char *st = NULL;
      int size = 1;
      
      if(ev->string)
      {
	 st = strdup(ev->string);
	 /* Find the size of data to send borrowed from evas!!*/
	 if(st[0] < 0x80)
	    size = 1;
	 else if((st[0] & 0xe0) == 0xc0)
	    size = 2;
	 else if((st[0] & 0xf0) == 0xe0)
	    size = 3;	 
	 else
	    size = 4;
      }
      
      if (write(term->cmd_fd.sys, st, size) < 0) {
	    DPRINT((stderr, "Error writing to process: %m\n"));
	    //exit(2);
	    }
   }
   return;

   /* extra stuff, clean up later */

   if (evas_key_modifier_is_set(ev->modifiers, "Shift"))
     key_modifiers |= TERM_KEY_MODIFIER_SHIFT;
   else if (evas_key_modifier_is_set(ev->modifiers, "Alt"))
     key_modifiers |= TERM_KEY_MODIFIER_ALT;
   else if (evas_key_modifier_is_set(ev->modifiers, "Control"))
     key_modifiers |= TERM_KEY_MODIFIER_CTRL;
   else if (evas_key_modifier_is_set(ev->modifiers, "Meta"))
     key_modifiers |= TERM_KEY_MODIFIER_MOD;
   else if (evas_key_modifier_is_set(ev->modifiers, "Super"))
     key_modifiers |= TERM_KEY_MODIFIER_WIN;
   else if (evas_key_modifier_is_set(ev->modifiers, "Hyper"))
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
#if 0
   else if (key_modifiers & TERM_KEY_MODIFIER_SHIFT)
     strupper(keyname);
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
      //char *tmp = ev->string;
      //term_key_down(evas_object_smart_data_get(data),tmp[0]);
      //write(term->cmd_fd.sys, tmp, 1);

   }
}

void
term_event_title_change_free(void *data, void *ev)
{
   Term_Event_Title_Change *e;

   e = ev;
   free(e->title);
   free(e);
}
