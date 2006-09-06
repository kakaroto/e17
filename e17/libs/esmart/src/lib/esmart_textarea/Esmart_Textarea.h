#ifndef ESMART_TEXTAREA_H
#define ESMART_TEXTAREA_H

#ifdef EAPI
#undef EAPI
#endif
#ifdef WIN32
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#include <Evas.h>

enum Esmart_Textarea_Key_Modifiers           /* several key modifiers */
{
     ESMART_TEXTAREA_KEY_MODIFIER_SHIFT = 0x1,
     ESMART_TEXTAREA_KEY_MODIFIER_CTRL = 0x2,
     ESMART_TEXTAREA_KEY_MODIFIER_ALT = 0x4,
     ESMART_TEXTAREA_KEY_MODIFIER_MOD = 0x8,
     ESMART_TEXTAREA_KEY_MODIFIER_WIN = 0x10,
};

enum Esmart_Textarea_Mouse_Modifiers        /* several mouse modifiers */
{
     ESMART_TEXTAREA_MOUSE_MODIFIER_LEFT = 0x1,
     ESMART_TEXTAREA_MOUSE_MODIFIER_MIDDLE = 0x2,
     ESMART_TEXTAREA_MOUSE_MODIFIER_RIGHT = 0x4,
};

struct _Esmart_Text_Area_Coord {            /* a coord, x, y and char pos */
   Evas_Coord x;                         
   Evas_Coord y;
   int pos;
};

struct _Esmart_Text_Area_Format {           /* formatting information */
   char *font;
   char *size;
   char *color;
   char *underline;
   char *underline_color;
   char *double_underline;
   char *double_underline_color;
   char *outline;
   char *outline_color;
   char *shadow;
   char *shadow_color;
   char *glow;
   char *glow_color;
   char *outer_glow;
   char *outer_glow_color;
   char *backing;
   char *backing_color;
   char *strikethrough;
   char *strikethrough_color;      
};

typedef struct _Esmart_Text_Area_Format Esmart_Text_Area_Format;

typedef struct _Esmart_Text_Area_Coord Esmart_Text_Area_Coord;

struct _Esmart_Text_Area {                  /* our typical text area */   
   Evas_Object  *text;
   Evas_Object  *bg;
   Evas_Object  *cursor;      
   unsigned int  key_modifiers;
   unsigned int  in_selection;
   unsigned int  mouse_modifiers;
   Esmart_Text_Area_Coord sel_start;
   Esmart_Text_Area_Coord sel_end;   
};

typedef struct _Esmart_Text_Area Esmart_Text_Area;

/* text area public api */
EAPI Evas_Object    *esmart_textarea_add(Evas *evas);
EAPI void            esmart_textarea_cursor_goto_cursor(Evas_Object *o);
EAPI void            esmart_textarea_cursor_move_left(Evas_Object *o);
EAPI void            esmart_textarea_cursor_move_right(Evas_Object *o);
EAPI void            esmart_textarea_cursor_move_down(Evas_Object *o);
EAPI void            esmart_textarea_cursor_move_up(Evas_Object *o);
EAPI void            esmart_textarea_cursor_move_home(Evas_Object *o);
EAPI void            esmart_textarea_cursor_move_end(Evas_Object *o);
EAPI void            esmart_textarea_cursor_delete_right(Evas_Object *o);
EAPI void            esmart_textarea_cursor_delete_left(Evas_Object *o);
EAPI void            esmart_textarea_focus_set(Evas_Object *o, Evas_Bool focus);  
EAPI void            esmart_textarea_bg_set(Evas_Object *o, Evas_Object *bg);
EAPI Evas_Object    *esmart_textarea_bg_get(Evas_Object *o);
EAPI void            esmart_textarea_text_insert(Evas_Object *o, const char *text);
EAPI void            esmart_textarea_cursor_set(Evas_Object *o, Evas_Object *c);
EAPI void            esmart_textarea_clear(Evas_Object *o);
EAPI void            esmart_textarea_cursor_pos_set(Evas_Object *o, int pos);
EAPI int             esmart_textarea_cursor_pos_get(Evas_Object *o);
EAPI int             esmart_textarea_length_get(Evas_Object *o);
EAPI int             esmart_textarea_cursor_line_get(Evas_Object *o);
EAPI int             esmart_textarea_lines_get(Evas_Object *o);
EAPI int             esmart_textarea_line_start_pos_get(Evas_Object *o);
EAPI int             esmart_textarea_line_end_pos_get(Evas_Object *o);
EAPI Evas_Bool       esmart_textarea_line_get(Evas_Object *o, int line, Evas_Coord *lx, Evas_Coord *ly, Evas_Coord *lw, Evas_Coord *lh);
EAPI Evas_Bool       esmart_textarea_char_pos_get(Evas_Object *o, int pos, Evas_Coord *lx, Evas_Coord *ly, Evas_Coord *lw, Evas_Coord *lh);
EAPI int             esmart_textarea_char_coords_get(Evas_Object *o, Evas_Coord x, Evas_Coord y, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch);
EAPI void            esmart_textarea_text_insert(Evas_Object *o, const char *text);
EAPI char           *esmart_textarea_text_get(Evas_Object *o, int len);
EAPI void            esmart_textarea_text_del(Evas_Object *o, int len);
EAPI void            esmart_textarea_format_insert(Evas_Object *o, const char *format);
EAPI int             esmart_textarea_format_next_pos_get(Evas_Object *o);
EAPI int             esmart_textarea_format_next_count_get(Evas_Object *o);
EAPI const char     *esmart_textarea_format_next_get(Evas_Object *o, int n);
EAPI void            esmart_textarea_format_next_del(Evas_Object *o, int n);
EAPI int             esmart_textarea_format_prev_pos_get(Evas_Object *o);
EAPI int             esmart_textarea_format_prev_count_get(Evas_Object *o);
EAPI const char     *esmart_textarea_format_prev_get(Evas_Object *o, int n);
EAPI void            esmart_textarea_format_prev_del(Evas_Object *o, int n);
EAPI char           *esmart_textarea_format_current_get(Evas_Object *o);
EAPI void            esmart_textarea_format_size_get(Evas_Object *o, Evas_Coord *w, Evas_Coord *h);
EAPI void            esmart_textarea_native_size_get(Evas_Object *o, Evas_Coord *w, Evas_Coord *h);
EAPI int             esmart_textarea_native_lines_get(Evas_Object *o);

EAPI Esmart_Text_Area_Format *esmart_textarea_format_get(Evas_Object *o);
    
#endif
