#ifndef _EWL_WINDOW_H_
#define _EWL_WINDOW_H_

#include "ewlcore.h"
#include "ewlcontainer.h"

/* ALL THIS SHIT WILL BE COMING FROM THE DB EVENTUALLY */
#define EWL_DEFAULT_WINDOW_TYPE	   EWL_WINDOW_TOPLEVEL
#define EWL_DEFAULT_WINDOW_TITLE   "Untitled EwlWindow"
#define EWL_DEFAULT_WINDOW_WIDTH   320
#define EWL_DEFAULT_WINDOW_HEIGHT  240
#define EWL_DEFAULT_WINDOW_X   0
#define EWL_DEFAULT_WINDOW_Y   0
#define EWL_DEFAULT_WINDOW_NAME_H  "Untitled EwlWindow" /*ewl_db_get("EwlWindow.db","default/name_hint")*/
#define EWL_DEFAULT_WINDOW_CLASS_H "Untitled EwlWindow"

typedef struct _EwlWindow EwlWindow;

enum _EwlWinType	{
	EWL_WINDOW_TOPLEVEL
};

typedef enum _EwlWinType EwlWinType;

struct _EwlWindow	{
	EwlContainer     container;
	
	/* x junk */
	Screen          *screen;
	int              depth;
	Visual          *vis;
	Colormap         cm;
	Window           root;
	XContext         xid;
	Window           xwin;
	GC               gc;
	Pixmap           pmap; /* everything is rendered into this pixmap */
	
	/* window properties below here */
	EwlWinType	 	 type;
	char			*title;
	int				 x, y;
	int				 w, h;
	char            *name_hint;
	char            *class_hint;
	EwlBool			 decorations;				 
};

EwlWidget   *ewl_window_new(EwlWinType type);
EwlWidget   *ewl_window_new_with_values(char *title, int w, int h,
                                        char *name_hint, char *class_hint);
void         ewl_window_init(EwlWindow *win, char *title, int w, int h,
                             char *name_hint, char *class_hint);

void         ewl_window_pack(EwlWidget *window, EwlWidget *child);

EwlWidget   *ewl_window_find_by_xwin(Window xwin);
void         ewl_window_set_render_context(EwlWidget *widget);
/* private */
EwlBool _cb_ewl_window_event_handler(EwlWidget *widget, EwlEvent *ev,
                                     EwlData *data);

#endif /* _EWL_WINDOW_H_ */
