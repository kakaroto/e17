#include "Esmart_Textarea.h"
#include "esmart_textarea_private.h"

/* this file contains textarea's public API, other than the smart functions */
/* functions here wrap the internal API and provide access to Esmart_Text_Area
 * objects via Evas_Object calls
 */

/* create a new textarea */
EAPI Evas_Object *
esmart_textarea_add(Evas *evas)
{
   Evas_Object *t;
   t = evas_object_smart_add(evas, esmart_textarea_smart_get());
   return t;
}

/* update cursor location automatically */
EAPI void 
esmart_textarea_cursor_goto_cursor(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_cursor_goto_cursor(t);
}

/* move cursor to the left 1 char */
EAPI void
esmart_textarea_cursor_move_left(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_cursor_move_left(t);
}
   
/* move cursor to the right 1 char */
EAPI void
esmart_textarea_cursor_move_right(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_cursor_move_right(t);
}

/* move cursor down 1 char */
EAPI void
esmart_textarea_cursor_move_down(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_cursor_move_down(t);
}

/* move cursor up 1 char */
EAPI void
esmart_textarea_cursor_move_up(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_cursor_move_up(t);
}

/* move cursor to home position */
EAPI void
esmart_textarea_cursor_move_home(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_cursor_move_home(t);
}

/* move cursor to end position */
EAPI void
esmart_textarea_cursor_move_end(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_cursor_move_end(t);
}

/* delete 1 char from the right */
EAPI void
esmart_textarea_cursor_delete_right(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_cursor_delete_right(t);
}

/* delete 1 char from the left */
EAPI void
esmart_textarea_cursor_delete_left(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_cursor_delete_left(t);
}

/* focus / unfocus textarea */
EAPI void
esmart_textarea_focus_set(Evas_Object *o, Evas_Bool focus)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);   
   _esmart_textarea_focus_set(t, focus);     
}
    

/* override default background with an Evas_Object */
EAPI void
esmart_textarea_bg_set(Evas_Object *o, Evas_Object *bg) 
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);   
   _esmart_textarea_bg_set(t, bg);
}

EAPI Evas_Object *
esmart_textarea_bg_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);   
   return _esmart_textarea_bg_get(t);
}


/* override default cursor with an Evas_Object */
EAPI void
esmart_textarea_cursor_set(Evas_Object *o, Evas_Object *c)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_cursor_set(t, c);
}

/* insert text into the text area at current cursor */
EAPI void
esmart_textarea_text_insert(Evas_Object *o, const char *text)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_text_insert(t, text);
}

/* clear the textarea */
EAPI void
esmart_textarea_clear(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_clear(t);
   _esmart_textarea_cursor_goto_cursor(t);
}

/* set the cursor's position */
EAPI void
esmart_textarea_cursor_pos_set(Evas_Object *o, int pos)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_cursor_pos_set(t, pos);
   _esmart_textarea_cursor_goto_cursor(t);   
}

EAPI int
esmart_textarea_cursor_pos_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_cursor_pos_get(t);
}

EAPI int
esmart_textarea_length_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_length_get(t);
}

EAPI int
esmart_textarea_cursor_line_get(Evas_Object *o)
{    
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_cursor_line_get(t);
}

EAPI int
esmart_textarea_lines_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_lines_get(t);
}

EAPI int
esmart_textarea_line_start_pos_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_line_start_pos_get(t);
}

EAPI int
esmart_textarea_line_end_pos_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_line_end_pos_get(t);
}

EAPI Evas_Bool
esmart_textarea_line_get(Evas_Object *o, int line, Evas_Coord *lx, 
			  Evas_Coord *ly, Evas_Coord *lw, Evas_Coord *lh)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_line_get(t, line, lx, ly, lw, lh);
}

EAPI Evas_Bool
esmart_textarea_char_pos_get(Evas_Object *o, int pos, Evas_Coord *cx, 
			      Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_char_pos_get(t, pos, cx, cy, cw, ch);
}

EAPI int
esmart_textarea_char_coords_get(Evas_Object *o, Evas_Coord x, Evas_Coord y, 
				Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw,
				Evas_Coord *ch)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_char_coords_get(t, x, y, cx, cy, cw, ch);
}



EAPI char *
esmart_textarea_text_get(Evas_Object *o, int len)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_text_get(t, len);
}

EAPI void
esmart_textarea_text_del(Evas_Object *o, int len)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_text_del(t, len);
}  
  
EAPI void
esmart_textarea_format_insert(Evas_Object *o, const char *format)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_format_insert(t, format);
}

EAPI int
esmart_textarea_format_next_pos_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_format_next_pos_get(t);
}

EAPI int
esmart_textarea_format_next_count_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_format_next_count_get(t);
}

EAPI const char *
esmart_textarea_format_next_get(Evas_Object *o, int n)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_format_next_get(t, n);
}

EAPI void
esmart_textarea_format_next_del(Evas_Object *o, int n)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_format_next_del(t, n);
}

EAPI int
esmart_textarea_format_prev_pos_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_format_prev_pos_get(t);
}

EAPI int
esmart_textarea_format_prev_count_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_format_prev_count_get(t);
}

EAPI const char *
esmart_textarea_format_prev_get(Evas_Object *o, int n)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_format_prev_get(t, n);
}

EAPI void
esmart_textarea_format_prev_del(Evas_Object *o, int n)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_format_prev_del(t, n);
}

EAPI char *
esmart_textarea_format_current_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_format_current_get(t);
}

EAPI void
esmart_textarea_format_size_get(Evas_Object *o, Evas_Coord *w, 
				 Evas_Coord *h)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_format_size_get(t, w, h);
}

EAPI void
esmart_textarea_native_size_get(Evas_Object *o, Evas_Coord *w,
				 Evas_Coord *h)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   _esmart_textarea_native_size_get(t, w, h);
}

EAPI int
esmart_textarea_native_lines_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_native_lines_get(t);
}

EAPI Esmart_Text_Area_Format *
esmart_textarea_format_get(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   return _esmart_textarea_format_get(t);
}
    
