#ifndef _WIDGET_H_
#define _WIDGET_H_ 1

#include "includes.h"
#include "ll.h"
#include "layout.h"
#include "imlib.h"
#include "image.h"
#include "event.h"

typedef struct _EwlEvCbLL EwlEvCbLL;
typedef struct _EwlWidget EwlWidget;


/* from theme.h */
extern char    *ewl_theme_get_string(char *key);
extern EwlBool  ewl_theme_get_int(char *key, int *val);
extern void     ewl_widget_get_theme(EwlWidget *wid, char *key);

/* defined in state.h */
extern char  ewl_state_render_dithered_get();
extern char  ewl_state_render_antialiased_get();
extern void  ewl_widget_add(EwlWidget *widget);
extern void  ewl_event_queue(EwlEvent *ev);

/* defined in ewlcontainer.h */
extern void ewl_container_render_children(EwlWidget *w);


struct _EwlEvCbLL	{
	EwlLL                ll;
	EwlEventType         type;
	char                 (*cb)(EwlWidget *widget,
	                           EwlEvent *ev,
	                           EwlData *data);
	EwlData             *data;
	EwlEvCbLL           *next;
};

enum _EwlPaddingEnum	{
	EWL_PAD_LEFT,
	EWL_PAD_TOP,
	EWL_PAD_RIGHT,
	EWL_PAD_BOTTOM
};

enum _EwlWidgetFlags	{
	VISIBLE                = 0x00000001, /* 1<<0, */
	FOCUSED                = 0x00000002, /* 1<<1, */
	GRABBED                = 0x00000004, /* 1<<2, */
	RENDER_ALPHA           = 0x00000008, /* 1<<3, */
	HAS_CHILDREN           = 0x0000000F, /* 1<<4, */

	NEEDS_REFRESH          = 0x00000010, /* 1<<5, */
	NEEDS_RESIZE           = 0x00000020, /* 1<<6, */
	CAN_RESIZE             = 0x00000040, /* 1<<7, */

	DONT_PROPAGATE_RESIZE  = 0x00000080, /* 1<<8, */
	DONT_PROPAGATE_REFRESH = 0x000000F0, /* 1<<9, */

	CONTAINER_FILL         = 0x00000100  /* 1<<10 */
};

/*typedef struct _EwlWidget EwlWidget;*/
struct _EwlWidget	{
	char           *name;
	EwlWidgetType   type;
	EwlWidget      *parent;
	EwlWidget      *root;  

	EwlFlag         flags;
	EwlWidgetState  state;

	/* layout */
	EwlImLayer     *layers;
	int             padding[4];
	EwlLayout      *layout; /* current, min, max, and requested rects */
	EwlRLayout     *rlayout;

	/* event mask and callbacks */
	EwlEvCbLL      *event_callbacks;

	void (*render)(EwlWidget *wid, EwlData *data);
	Imlib_Image *rendered;
	Imlib_Image *bg;
	int          rendered_w, rendered_h; /* depricated */
};

/* allocation functions */
EwlWidget       *ewl_widget_new();
void             ewl_widget_init(EwlWidget *w);
void             ewl_widget_free(EwlWidget *w);

/* flag functions */
void             ewl_widget_set_flags(EwlWidget *w, EwlFlag flags);
void             ewl_widget_set_flag(EwlWidget *w, EwlFlag f, EwlBool v);
EwlBool          ewl_widget_get_flag(EwlWidget *w, EwlFlag f);

void             ewl_widget_set_state(EwlWidget *w, EwlWidgetState state);
EwlWidgetState   ewl_widget_get_state(EwlWidget *w);

void             ewl_widget_set_type(EwlWidget *w, EwlWidgetType state);
EwlWidgetType    ewl_widget_get_type(EwlWidget *w);
char            *ewl_widget_get_type_string(EwlWidget *w);

char             ewl_widget_is_visible(EwlWidget *w);
char             ewl_widget_needs_resize(EwlWidget *w);
char             ewl_widget_needs_refresh(EwlWidget *w);
void             ewl_widget_set_needs_resize(EwlWidget *w);
void             ewl_widget_set_needs_refresh(EwlWidget *w);
char             ewl_widget_can_resize(EwlWidget *w);

/* public widget resize functions */
void             ewl_widget_moveresize(EwlWidget *widget,
                                       int x, int y, int w, int h);
void             ewl_widget_move(EwlWidget *widget, int x, int y);
void             ewl_widget_resize(EwlWidget *widget, int w, int h);

EwlRect         *ewl_widget_get_rect(EwlWidget *widget);

/* private widget resize functions */
void             ewl_widget_set_rect(EwlWidget *widget,
                                     int *x, int *y, int *w, int *h);

/* public event functions */
void             ewl_callback_add(EwlWidget *w, EwlEventType t, 
	                              char   (*cb)(EwlWidget *widget,
	                                           EwlEvent *ev,
	                                           EwlData *data),
                                  EwlData *data);
/* private event functiosn */
void             ewl_widget_callback_add(EwlWidget *w,
                                         EwlEventType t, 
	                                     char   (*cb)(EwlWidget *widget,
	                                                  EwlEvent *ev,
	                                                  EwlData *data),
                                         EwlData *data);
char             ewl_widget_handle_event(EwlWidget *w,
                                         EwlEvent *ev,
                                         EwlData *d);
char             cb_ewl_widget_event_handler(EwlWidget *w,
                                             EwlEvent *ev,
                                             EwlData *d);

/* public rendering functions */
void             ewl_widget_render(EwlWidget *w);
void             ewl_widget_render_onto_parent(EwlWidget *w);

/* padding funcs */
int             *ewl_widget_get_padding(EwlWidget *w);
void             ewl_widget_set_padding(EwlWidget *w, int *left, int *top,
                                        int *right, int *bottom);

/* widget imlayer handling functions */
void             ewl_widget_set_background(EwlWidget *w, Imlib_Image im);
void             ewl_widget_imlayer_insert(EwlWidget *w, EwlImLayer *l);
void             ewl_widget_imlayer_remove(EwlWidget *w, EwlImLayer *l);
void             ewl_widget_imlayer_push(EwlWidget *w, EwlImLayer *l);
EwlImLayer      *ewl_widget_imlayer_pop(EwlWidget *w);
void             ewl_widget_imlayer_foreach(EwlWidget *widget,
                                            EwlBool  (*cb)(EwlImLayer *layer,
                                                           EwlData    *data),
                                            EwlData   *data);

/* debugging functions */
void             ewl_widget_dump(EwlWidget *w);

/* PRIVATE */
typedef struct _EwlImLayerForeachData EwlImLayerForeachData;
struct _EwlImLayerForeachData {
	EwlBool  (*cb)(EwlImLayer *layer,
	               EwlData    *data);
	EwlData   *data;
};

#endif  /*_WIDGET_H_*/
