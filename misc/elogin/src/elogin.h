#ifndef HAVE_ELOGIN_H
#define HAVE_ELOGIN_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ebits.h>


/* entry stuffs */
typedef struct _E_Entry E_Entry;

struct _E_Entry
{
   Evas  evas;
   char *buffer;
   int   cursor_pos;
   struct {
      int start, length, down;
   } select;
   int   mouse_down;
   int   visible;
   int   focused;
   int   scroll_pos;
   int   x, y, w, h;
   int   min_size;
   Ebits_Object obj_base;
   Ebits_Object obj_cursor;
   Ebits_Object obj_selection;
   Evas_Object event_box;
   Evas_Object clip_box;
   Evas_Object text;
   Window paste_win;
   Window selection_win;
   int end_width;
   void (*func_changed) (E_Entry *entry, void *data);
   void *data_changed;
   void (*func_enter) (E_Entry *entry, void *data);
   void *data_enter;
   void (*func_focus_in) (E_Entry *entry, void *data);
   void *data_focus_in;
   void (*func_focus_out) (E_Entry *entry, void *data);
   void *data_focus_out;
};

/* greeter */
typedef struct _Elogin_Greeter Elogin_Greeter;

struct _Elogin_Greeter
{
   /* gui stuffs */
   Evas_Object greet_win;
   E_Entry *user_input;
   E_Entry *pass_input;
   int w, h;

   /* other goodies */
   char *user;
   char *password;
   int attempts;
};

char *elogin_greeter_msg(char *str);


void e_entry_init(void);
void e_entry_free(E_Entry *entry);
E_Entry *e_entry_new(void);
void e_entry_handle_keypress(E_Entry *entry, Ev_Key_Down *e);
void e_entry_set_evas(E_Entry *entry, Evas evas);
void e_entry_show(E_Entry *entry);
void e_entry_hide(E_Entry *entry);
void e_entry_raise(E_Entry *entry);
void e_entry_lower(E_Entry *entry);
void e_entry_set_layer(E_Entry *entry, int l);
void e_entry_set_clip(E_Entry *entry, Evas_Object clip);
void e_entry_unset_clip(E_Entry *entry);
void e_entry_move(E_Entry *entry, int x, int y);
void e_entry_resize(E_Entry *entry, int w, int h);
void e_entry_query_max_size(E_Entry *entry, int *w, int *h);
void e_entry_max_size(E_Entry *entry, int *w, int *h);
void e_entry_min_size(E_Entry *entry, int *w, int *h);
void e_entry_set_size(E_Entry *entry, int w, int h);
void e_entry_set_focus(E_Entry *entry, int focused);
void e_entry_set_text(E_Entry *entry, const char *text);
const char *e_entry_get_text(E_Entry *entry);
void e_entry_set_cursor(E_Entry *entry, int cursor_pos);
int e_entry_get_cursor(E_Entry *entry);
void e_entry_set_changed_callback(E_Entry *entry, void (*func) (E_Entry *_entry,
	  void *_data), void *data);
void e_entry_set_enter_callback(E_Entry *entry, void (*func) (E_Entry *_entry, 
	 void *_data), void *data);
void e_entry_set_focus_in_callback(E_Entry *entry, void (*func) (E_Entry *_entry, 
	 void *_data), void *data);
void e_entry_set_focus_out_callback(E_Entry *entry, void (*func) (E_Entry *_entry, 
	 void *_data), void *data);
void e_entry_insert_text(E_Entry *entry, char *text);
void e_entry_clear_selection(E_Entry *entry);
void e_entry_delete_to_left(E_Entry *entry);
void e_entry_delete_to_right(E_Entry *entry);
char *e_entry_get_selection(E_Entry *entry);

#endif
