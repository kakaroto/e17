#include "Esmart_Textarea.h"
#include "esmart_textarea_private.h"

/* make the cursor follow textblock's current cursor location */
void _esmart_textarea_cursor_goto_cursor(Esmart_Text_Area *t) {
   int pos,x,y,w,h,x2,y2;   
   pos = evas_object_textblock_cursor_pos_get(t->text);
   if(pos == 0) return;
   pos--;
   evas_object_textblock_char_pos_get(t->text, pos ,&x,&y,&w,&h);
   evas_object_geometry_get(t->text, &x2, &y2, NULL, NULL);
   evas_object_move(t->cursor, x+w+x2, y+y2);
}
 
/* TODO: bug when we reach first char */
/* move cursor one position to the left */
void _esmart_textarea_cursor_move_left(Esmart_Text_Area *ta) {
   int pos;

   if((ta->key_modifiers & ESMART_TEXTAREA_KEY_MODIFIER_SHIFT)) {
      pos = evas_object_textblock_cursor_pos_get(ta->text);      
      if(!ta->in_selection) {
	 evas_object_textblock_cursor_pos_set(ta->text, pos);
	 evas_object_textblock_format_insert(ta->text, "backing=off");
	 ta->in_selection = 1;
      }
      evas_object_textblock_cursor_pos_set(ta->text, pos - 1);      
      evas_object_textblock_format_insert(ta->text, "backing=on backing_color=#cccccc");
   } else {
      pos = evas_object_textblock_cursor_pos_get(ta->text);
   }
   if (--pos < 0) pos = 1;
   evas_object_textblock_cursor_pos_set(ta->text, pos);
   _esmart_textarea_cursor_goto_cursor(ta);   
}

/* move cursor one position to the right */
void _esmart_textarea_cursor_move_right(Esmart_Text_Area *ta) {
   int pos,len;

   if((ta->key_modifiers & ESMART_TEXTAREA_KEY_MODIFIER_SHIFT)) {
      pos = evas_object_textblock_cursor_pos_get(ta->text);
      evas_object_textblock_cursor_pos_set(ta->text, pos + 1);
      evas_object_textblock_format_insert(ta->text, "backing=off");
      evas_object_textblock_cursor_pos_set(ta->text, pos);      
      evas_object_textblock_format_insert(ta->text, "backing=on backing_color=#cccccc");
   }
   pos = evas_object_textblock_cursor_pos_get(ta->text);
   len = evas_object_textblock_line_end_pos_get(ta->text);
   if (++pos > len) pos = len;   
   evas_object_textblock_cursor_pos_set(ta->text, pos);
   _esmart_textarea_cursor_goto_cursor(ta);   
}

/* move cursor one position down: go one line down, reach current x coord */
void _esmart_textarea_cursor_move_down(Esmart_Text_Area *ta) {
   int cur_line, line, pos, len;
   Evas_Coord cx,cy,cw,ch,i;
   
   i = 0;
   pos = evas_object_textblock_cursor_pos_get(ta->text);
   
   len = evas_object_textblock_length_get(ta->text);   
   
   /* get coords of current cursor position */
   evas_object_textblock_char_pos_get(ta->text,pos,&cx,&cy,&cw,&ch);
   
   /* go down one line */
   line = cur_line = evas_object_textblock_cursor_line_get(ta->text);
   while(line == cur_line && pos <= len ) {
      pos++;
      evas_object_textblock_cursor_pos_set(ta->text, pos);
      cur_line = evas_object_textblock_cursor_line_get(ta->text);
   }
   
   /* keep moving until we hit the required x coord */
   evas_object_textblock_char_pos_get(ta->text,pos,&i,&cy,&cw,&ch);   
   while(cx != i && pos <= len) {
      pos++;
      evas_object_textblock_cursor_pos_set(ta->text, pos);      
      evas_object_textblock_char_pos_get(ta->text,pos,&i,&cy,&cw,&ch);
      
   }   
}


/* move cursor one position up: go one line up, reach current x coord */
void _esmart_textarea_cursor_move_up(Esmart_Text_Area *ta) {
   int cur_line, line, pos;
   Evas_Coord cx,cy,cw,ch,i;
   
   i = 0;
   pos = evas_object_textblock_cursor_pos_get(ta->text);
   
   /* get coords of current cursor position */
   evas_object_textblock_char_pos_get(ta->text,pos,&cx,&cy,&cw,&ch);
   
   /* go up one line */
   line = cur_line = evas_object_textblock_cursor_line_get(ta->text);
   while(line == cur_line && pos >= 0) {
      pos--;
      evas_object_textblock_cursor_pos_set(ta->text, pos);
      cur_line = evas_object_textblock_cursor_line_get(ta->text);
   }
   
   /* keep moving until we hit the required x coord */
   evas_object_textblock_char_pos_get(ta->text,pos,&i,&cy,&cw,&ch);   
   while(cx != i && pos >= 0) {      
      pos--;
      evas_object_textblock_cursor_pos_set(ta->text, pos);      
      evas_object_textblock_char_pos_get(ta->text,pos,&i,&cy,&cw,&ch);
   }   
}


/* move cursor to home position, usually, start of line */
void _esmart_textarea_cursor_move_home(Esmart_Text_Area *ta) {
}

/* move cursor to end position, usually, end of line */
void _esmart_textarea_cursor_move_end(Esmart_Text_Area *ta) {
}

/* delete one character from current cursor location */
/* TODO: make sure that the last pos = len, and not len - 1 */
void _esmart_textarea_cursor_delete_right(Esmart_Text_Area *ta) {
   int pos, len;
   pos = evas_object_textblock_cursor_pos_get(ta->text);
   len = evas_object_textblock_length_get(ta->text);
   if(pos == len) return;
   evas_object_textblock_text_del(ta->text, 1);
}

/* delete left, backspace, one character from current location */
void _esmart_textarea_cursor_delete_left(Esmart_Text_Area *ta) {
   int pos;
   pos = evas_object_textblock_cursor_pos_get(ta->text);
   if(pos == 0) return;
   evas_object_textblock_cursor_pos_set(ta->text, pos - 1);
   evas_object_textblock_text_del(ta->text, 1);   
}

/* override the default cursor */
void
_esmart_textarea_cursor_set(Esmart_Text_Area *t, Evas_Object *o) {
   int x,y,w,h,l;
   evas_object_geometry_get(t->cursor, &x,&y,&w,&h);
   l = evas_object_layer_get(t->cursor);
   evas_object_del(t->cursor);
   t->cursor = o;
   evas_object_move(t->cursor, x, y);
   evas_object_layer_set(t->cursor, l);
   evas_object_show(t->cursor);   
}
