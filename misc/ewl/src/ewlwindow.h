#ifndef _EWL_WINDOW_H_
#define _EWL_WINDOW_H_

#include "ewlcore.h"
#include "ewlcontainer.h"

/* ALL THIS SHIT WILL BE COMING FROM THE DB EVENTUALLY */
#define EWL_DEFAULT_WINDOW_TYPE	   EWL_WINDOW_TOPLEVEL
#define EWL_DEFAULT_WINDOW_TITLE   "Untitled EwlWindow"
#define EWL_DEFAULT_WINDOW_X   0
#define EWL_DEFAULT_WINDOW_Y   0
#define EWL_DEFAULT_WINDOW_WIDTH   320
#define EWL_DEFAULT_WINDOW_HEIGHT  240
#define EWL_DEFAULT_WINDOW_NAME_H  "Untitled EwlWindow" /*ewl_db_get("EwlWindow.db","default/name_hint")*/
#define EWL_DEFAULT_WINDOW_CLASS_H "Untitled EwlWindow"
#define EWL_DEFAULT_WINDOW_DECOR_H TRUE

#define EWL_WINDOW_MAX_WIDTH  4096
#define EWL_WINDOW_MAX_HEIGHT 4096

typedef struct _EwlWindow EwlWindow;

enum _EwlWindowType	{
	EWL_WINDOW_TOPLEVEL,
/* heh change this please i couldnt think of anything else ;) */
	EWL_WINDOW_TRANSIENT
};

typedef enum _EwlWindowType EwlWindowType;

struct _EwlWindow	{
	EwlContainer     container;
	
	/* window properties */
	XSetWindowAttributes	attr;
	EwlWindowType    type;
	char            *title;
	char            *name_hint;
	char            *class_hint;
	EwlBool          decoration_hint;				 

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
	
	XClassHint      *xclass_hint;
	MWMHints         mwmhints;

	Evas             evas;
};

EwlWidget   *ewl_window_new(EwlWindowType type);
EwlWidget   *ewl_window_new_with_values(EwlWindowType type, char *title, 
										int w, int h);
void         ewl_window_init(EwlWindow *win, EwlWindowType type, char *title, 
							 int w, int h);

void         ewl_window_pack(EwlWidget *window, EwlWidget *child);

EwlWidget   *ewl_window_find_by_xwin(Window xwin);

/* property helper functions */
void	 ewl_window_set_title(EwlWidget *widget, char *title);
char    *ewl_window_get_title(EwlWidget *widget);

void     ewl_window_set_class_hints(EwlWidget *widget,
                                   char *name, char *klass);
void	 ewl_window_set_decoration_hint(EwlWidget *widget, 
                                        EwlBool decor_hint);
char    *ewl_window_get_name_hint(EwlWidget *widget);
char    *ewl_window_get_class_hint(EwlWidget *widget);
EwlBool  ewl_window_get_decoration_hint(EwlWidget *widget);

void     ewl_window_move(EwlWidget *widget,
                         int x, int y);
void     ewl_window_resize(EwlWidget *widget,
                           int w, int h);
void     ewl_window_moveresize(EwlWidget *widget,
                               int x, int y, int w, int h);
EwlRect *ewl_window_get_rect(EwlWidget *widget);

Evas     ewl_window_get_evas(EwlWidget *widget);
void     ewl_window_set_evas(EwlWidget *widget, Evas evas);

/* private */
void	 ewl_window_set_render_context(EwlWidget *widget);
void	 ewl_window_realize(EwlWidget *widget);
void	 ewl_window_unrealize(EwlWidget *widget);

EwlBool _cb_ewl_window_event_handler(EwlWidget *widget, EwlEvent *ev,
                                     EwlData *data);

#endif /* _EWL_WINDOW_H_ */
