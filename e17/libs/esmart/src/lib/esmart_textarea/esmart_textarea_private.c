#include "Esmart_Textarea.h"
#include "esmart_textarea_private.h"

/* private core functions that are used by the smart calls */

/* init the text object, add textblock, create cursor, create bg, add menus
 * defaults: VeraMono, black, size 10 (layer 2)
 *           white bg (layer 1)
 *           black cursor (layer 2)
 * those values can be set by the user using too
 * 
 * TODO: dont add menus always, support pages */
Esmart_Text_Area *
_esmart_textarea_init(Evas_Object *o) {
   
   Esmart_Text_Area *t;
   Evas *evas;
   int x,y,w,h;
   
   t = malloc(sizeof(Esmart_Text_Area));
   
   evas = evas_object_evas_get(o);   
   evas_font_path_append(evas, DATADIR);
   t->text = evas_object_textblock_add(evas);
   t->key_modifiers = 0;
   t->in_selection = 0;
   t->mouse_modifiers = 0;
   
   evas_object_textblock_format_insert(t->text, "color=#000000 size=10 font=VeraMono");
   evas_object_textblock_text_insert(t->text, " ");
   evas_object_textblock_cursor_pos_set(t->text, 0);
   evas_object_textblock_char_pos_get(t->text, 1,&x,&y,&w,&h);   
   //evas_object_textblock_text_del(t->text, 1); // is this needed?
   evas_object_textblock_cursor_pos_set(t->text, 0);   
      
   evas_object_focus_set(t->text, 1);
   evas_object_event_callback_add(t->text, EVAS_CALLBACK_KEY_DOWN, 
				  _esmart_textarea_cb_key_down, t);
   evas_object_event_callback_add(t->text, EVAS_CALLBACK_KEY_UP,
				  _esmart_textarea_cb_key_up, t);   
   evas_object_event_callback_add(t->text, EVAS_CALLBACK_MOUSE_DOWN, 
				  _esmart_textarea_cb_mouse_down, t);
   evas_object_event_callback_add(t->text, EVAS_CALLBACK_MOUSE_UP,
				  _esmart_textarea_cb_mouse_up, t);
   evas_object_event_callback_add(t->text, EVAS_CALLBACK_MOUSE_MOVE,
				  _esmart_textarea_cb_mouse_move, t);

   t->bg = evas_object_rectangle_add(evas);
   evas_object_color_set(t->bg, 255, 255, 255, 255);
   evas_object_stack_below(t->bg, t->text);
   
   t->cursor = evas_object_rectangle_add(evas);
   evas_object_color_set(t->cursor, 0,0,0,255);
   if( h > 1000) h = 10;
   evas_object_resize(t->cursor, 1, h);
   evas_object_stack_above(t->cursor, t->text);   

   return t;
}

/* focus / unfocus (to accept key strokes) textarea */
void
_esmart_textarea_focus_set(Esmart_Text_Area *t, Evas_Bool focus)
{
   evas_object_focus_set(t->text, focus);
}

/* override the default bg */
void 
_esmart_textarea_bg_set(Esmart_Text_Area *t, Evas_Object *o)
{
   int x,y,w,h;
   evas_object_geometry_get(t->text, &x,&y,&w,&h);
   evas_object_del(t->bg);
   t->bg = o;
   evas_object_move(t->bg, x, y);
   evas_object_resize(t->bg, w, h);
   evas_object_stack_below(t->bg, t->text);
   evas_object_show(t->bg);
}

Evas_Object * 
_esmart_textarea_bg_get(Esmart_Text_Area *t)
{
   return t->bg;
}

/* clear the textarea */
void
_esmart_textarea_clear(Esmart_Text_Area *t)
{
   evas_object_textblock_clear(t->text);
   _esmart_textarea_cursor_goto_cursor(t);
}

/* set the cursor's position */
void
_esmart_textarea_cursor_pos_set(Esmart_Text_Area *t, int pos)
{
   evas_object_textblock_cursor_pos_set(t->text, pos);
   _esmart_textarea_cursor_goto_cursor(t);   
}

int
_esmart_textarea_cursor_pos_get(Esmart_Text_Area *t)
{
   return evas_object_textblock_cursor_pos_get(t->text);
}

int
_esmart_textarea_length_get(Esmart_Text_Area *t)
{
   return evas_object_textblock_length_get(t->text);
}

int
_esmart_textarea_cursor_line_get(Esmart_Text_Area *t)
{    
   return evas_object_textblock_cursor_line_get(t->text);
}

int
_esmart_textarea_lines_get(Esmart_Text_Area *t)
{
   return evas_object_textblock_lines_get(t->text);
}

int
_esmart_textarea_line_start_pos_get(Esmart_Text_Area *t)
{
   return evas_object_textblock_line_start_pos_get(t->text);
}

int
_esmart_textarea_line_end_pos_get(Esmart_Text_Area *t)
{
   return evas_object_textblock_line_end_pos_get(t->text);
}

Evas_Bool
_esmart_textarea_line_get(Esmart_Text_Area *t, int line, Evas_Coord *lx, 
			  Evas_Coord *ly, Evas_Coord *lw, Evas_Coord *lh)
{
   return evas_object_textblock_line_get(t->text, line, lx, ly, lw, lh);
}

Evas_Bool
_esmart_textarea_char_pos_get(Esmart_Text_Area *t, int pos, Evas_Coord *cx, 
			      Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch)
{
   return evas_object_textblock_char_pos_get(t->text, pos, cx, cy, cw, ch);
}

int
_esmart_textarea_char_coords_get(Esmart_Text_Area *t, Evas_Coord x, 
				 Evas_Coord y, Evas_Coord *cx, Evas_Coord *cy,
				 Evas_Coord *cw,  Evas_Coord *ch)
{
   return evas_object_textblock_char_coords_get(t->text, x, y, cx, cy, cw, ch);
}

/* insert text at current cursor */
void
_esmart_textarea_text_insert(Esmart_Text_Area *t, const char *text)
{
   evas_object_textblock_text_insert(t->text, text);
}

char *
_esmart_textarea_text_get(Esmart_Text_Area *t, int len)
{
   return evas_object_textblock_text_get(t->text, len);
}

void
_esmart_textarea_text_del(Esmart_Text_Area *t, int len)
{
   return evas_object_textblock_text_del(t->text, len);
}  
  
void
_esmart_textarea_format_insert(Esmart_Text_Area *t, const char *format)
{
   evas_object_textblock_format_insert(t->text, format);
}

int
_esmart_textarea_format_next_pos_get(Esmart_Text_Area *t)
{
   return evas_object_textblock_format_next_pos_get(t->text);
}

int
_esmart_textarea_format_next_count_get(Esmart_Text_Area *t)
{
   return evas_object_textblock_format_next_count_get(t->text);
}

const char *
_esmart_textarea_format_next_get(Esmart_Text_Area *t, int n)
{
   return evas_object_textblock_format_next_get(t->text, n);
}

void
_esmart_textarea_format_next_del(Esmart_Text_Area *t, int n)
{
   evas_object_textblock_format_next_del(t->text, n);
}

int
_esmart_textarea_format_prev_pos_get(Esmart_Text_Area *t)
{
   return evas_object_textblock_format_prev_pos_get(t->text);
}

int
_esmart_textarea_format_prev_count_get(Esmart_Text_Area *t)
{
   return evas_object_textblock_format_prev_count_get(t->text);
}

const char *
_esmart_textarea_format_prev_get(Esmart_Text_Area *t, int n)
{
   return evas_object_textblock_format_prev_get(t->text, n);
}

void
_esmart_textarea_format_prev_del(Esmart_Text_Area *t, int n)
{
   evas_object_textblock_format_prev_del(t->text, n);
}

char *
_esmart_textarea_format_current_get(Esmart_Text_Area *t)
{
   return evas_object_textblock_format_current_get(t->text);
}

void
_esmart_textarea_format_size_get(Esmart_Text_Area *t, Evas_Coord *w, 
				 Evas_Coord *h)
{
   evas_object_textblock_format_size_get(t->text, w, h);
}

void
_esmart_textarea_native_size_get(Esmart_Text_Area *t, Evas_Coord *w,
				 Evas_Coord *h)
{
   evas_object_textblock_native_size_get(t->text, w, h);
}

int
_esmart_textarea_native_lines_get(Esmart_Text_Area *t)
{
   return evas_object_textblock_native_lines_get(t->text);
}

/* this is an experimental function that gets formatting at a current
 * location and uses the Esmart_Text_Area_Format struct as a placeholder
 */
Esmart_Text_Area_Format *
_esmart_textarea_format_get(Esmart_Text_Area *t)
{
   Esmart_Text_Area_Format *format;
   char *s, *tok;
   
   format = malloc(sizeof(Esmart_Text_Area_Format));
   s = strdup(evas_object_textblock_format_current_get(t->text));
              
#define FORMAT_FILL(op) \
   if(!strcmp(tok,#op)) \
       { \
	  if((tok = strtok(NULL," ="))) \
	    format->op = strdup(tok); \
       }
   
   tok = strtok(s," =");
   if(!tok) return NULL;
   do {
      FORMAT_FILL(font);
      FORMAT_FILL(size);
      FORMAT_FILL(color);
      FORMAT_FILL(underline);
      FORMAT_FILL(underline_color);
      FORMAT_FILL(double_underline);
      FORMAT_FILL(double_underline_color);
      FORMAT_FILL(outline);
      FORMAT_FILL(outline_color);
      FORMAT_FILL(shadow);
      FORMAT_FILL(shadow_color);
      FORMAT_FILL(glow);
      FORMAT_FILL(glow_color);
      FORMAT_FILL(outer_glow);
      FORMAT_FILL(outer_glow_color);
      FORMAT_FILL(backing);
      FORMAT_FILL(backing_color);
      FORMAT_FILL(strikethrough);
      FORMAT_FILL(strikethrough_color);      
   } while((tok = strtok(NULL," =")));
   return format;   
}
