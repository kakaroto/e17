
#ifndef __EVASWIN_H__
#define __EVASWIN_H__ 1

#include <Ecore.h>

#include <Evas.h>

typedef enum {
  TOOL_NEW_ITEM = 1,
  TOOL_NEW_SUBMENU,
  TOOL_NEW_SEP
} med_tool_type;



void
evaswin_new( int w, int h, const char *title );

void
evaswin_free(void);

Evas *
evaswin_get_e(void);

Display *
evaswin_get_disp(void);

Window
evaswin_get_win(void);

void
med_tool_mouse_up(void *_data, Evas * _e, Evas_Object * _o, void *_event);
void
med_tool_mouse_down(void *_data, Evas * _e, Evas_Object * _o, void *_event);
void
med_tool_mouse_move(void *_data, Evas * _e, Evas_Object * _o, void *_event);

void
med_show_commit_tool(void);
void
med_hide_commit_tool(void);
void
med_indicate_entries_dirty(void);

#endif /* __EVASWIN_H__ */
