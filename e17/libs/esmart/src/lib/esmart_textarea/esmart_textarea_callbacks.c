#include "Esmart_Textarea.h"
#include "esmart_textarea_private.h"

/* callback when a key is released, used for modifiers mainly */
void 
_esmart_textarea_cb_key_up(void *data, Evas *e, Evas_Object *obj,
			   void *event_info) 
{
   Evas_Event_Key_Down *ev = event_info;
   Esmart_Text_Area *t = data;
   if (strstr(ev->keyname, "Shift_")) {
      t->key_modifiers &= ~ESMART_TEXTAREA_KEY_MODIFIER_SHIFT;
      t->in_selection = 0;
   }
}

/* callback when a key is pressed, all keys + modifiers */
void
_esmart_textarea_cb_key_down(void *data, Evas *e, Evas_Object *obj, 
			     void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;
   Esmart_Text_Area *t = data;
   if (strstr(ev->keyname, "Shift_"))
     t->key_modifiers |= ESMART_TEXTAREA_KEY_MODIFIER_SHIFT;
   else if (!strcmp(ev->keyname, "Left"))
     _esmart_textarea_cursor_move_left(t);
   else if (!strcmp(ev->keyname, "Right"))
     _esmart_textarea_cursor_move_right(t);
   else if (!strcmp(ev->keyname, "Down"))
     _esmart_textarea_cursor_move_down(t);
   else if (!strcmp(ev->keyname, "Up"))
     _esmart_textarea_cursor_move_up(t);
   else if (!strcmp(ev->keyname, "Home"))
     _esmart_textarea_cursor_move_home(t);
   else if (!strcmp(ev->keyname, "End"))
     _esmart_textarea_cursor_move_end(t);
   else if (!strcmp(ev->keyname, "BackSpace"))
     _esmart_textarea_cursor_delete_left(t);
   else if (!strcmp(ev->keyname, "Delete"))
     _esmart_textarea_cursor_delete_right(t);
   else if (!strcmp(ev->keyname, "Return") ||
	    !strcmp(ev->keyname, "KP_Return") ||
	    !strcmp(ev->keyname, "Enter") ||
	    !strcmp(ev->keyname, "KP_Enter")) {
      //evas_object_textblock_text_insert(t->text, "\0");
      evas_object_textblock_format_insert(t->text, "\n");
   } else {
      evas_object_textblock_text_insert(t->text, ev->string);
   }
   _esmart_textarea_cursor_goto_cursor(t);
}

/* callback for when the mouse moves, for selection */
void
_esmart_textarea_cb_mouse_move(void *data, Evas *e, Evas_Object *obj,
				void *event_info) {
   Evas_Event_Mouse_Move *ev = event_info;
   Esmart_Text_Area *t = data;
      
   if((t->mouse_modifiers & ESMART_TEXTAREA_MOUSE_MODIFIER_LEFT))
     {
	int pos, cx, cy;
	evas_object_geometry_get(t->text, &cx, &cy, NULL, NULL);
	cx = ev->cur.canvas.x - cx;
	cy = ev->cur.canvas.y - cy;
	pos = evas_object_textblock_char_coords_get(t->text,cx,cy,NULL,NULL,
						    NULL,NULL);
	if(pos > t->sel_start.pos) // moved right or down
	  {
	     printf("moving right / down\n");
	  }
	else if(pos < t->sel_start.pos) // moved left or up
	  {
	     printf("moving left / up\n");
	  }
	else // stayed on same char
	  {
	     printf("still in place!\n");
	  }
     }   
}

/* callback for when a mouse button is pressed, cursor movement + selection */
void
_esmart_textarea_cb_mouse_down(void *data, Evas *e, Evas_Object *obj,
			       void *event_info) {
   Evas_Event_Mouse_Down *ev = event_info;
   Esmart_Text_Area *t = data;
   int cx, cy, index;
   
   /* set mouse down modifier, needed for selection / dragging */
   switch(ev->button)
     {
      case 1:
	t->mouse_modifiers |= ESMART_TEXTAREA_MOUSE_MODIFIER_LEFT;
		
	evas_object_geometry_get(t->text, &cx, &cy, NULL, NULL);
	cx = ev->canvas.x - cx;
	cy = ev->canvas.y - cy;
	index = evas_object_textblock_char_coords_get(t->text,
						      cx,cy,NULL,NULL,
						      NULL,NULL);
	evas_object_textblock_cursor_pos_set(t->text, index);
	_esmart_textarea_cursor_goto_cursor(t);
	
	/* save current coord for selection */
	t->sel_start.x = ev->canvas.x;
	t->sel_start.y = ev->canvas.y;
	t->sel_start.pos = index;
	break;
      case 2:
	t->mouse_modifiers |= ESMART_TEXTAREA_MOUSE_MODIFIER_MIDDLE;	
	break;
	
      case 3:
	t->mouse_modifiers |= ESMART_TEXTAREA_MOUSE_MODIFIER_RIGHT;
	break;
     }
}

void
_esmart_textarea_cb_mouse_up(void *data, Evas *e, Evas_Object *obj,
				  void *event_info) {
   Evas_Event_Mouse_Down *ev = event_info;
   Esmart_Text_Area *t = data;
   
   /* unset mouse down modifier, needed for selection / dragging */
   switch(ev->button) {
    case 1:
      t->mouse_modifiers &= ~ESMART_TEXTAREA_MOUSE_MODIFIER_LEFT;
      break;
    case 2:
      t->mouse_modifiers &= ~ESMART_TEXTAREA_MOUSE_MODIFIER_RIGHT;
      break;
    case 3:
      t->mouse_modifiers &= ~ESMART_TEXTAREA_MOUSE_MODIFIER_MIDDLE;
      break;
   }
}
