#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "layout.h"
#include "object.h"

#define EWL_WIDGET(a) ((EwlWidget*)a)

typedef struct _EwlWidget EwlWidget;

/* from window.h */
extern Evas ewl_window_get_evas(EwlWidget *widget);
extern EwlObject *ewl_get_state();

/* from theme.h */
extern void ewl_widget_get_theme(EwlWidget *widget, char *key);

struct _EwlWidget {
	EwlObject    object;
	Evas_Object  background;
};

/* WIDGET GRAB/FOCUS FUNCTIONS */
EwlWidget   *ewl_get_grabbed();
void         ewl_grab(EwlWidget *widget);

EwlWidget   *ewl_get_focused();
void         ewl_focus(EwlWidget *widget);

/* WIDGET NEW/FREE FUNCTIONS */
EwlWidget   *ewl_widget_new();
void        ewl_widget_init(EwlWidget *widget);
void        ewl_widget_free(EwlWidget *widget);

/* WIDGET REALIZE/UNREALIZE FUNCTIONS */
void        ewl_widget_realize(EwlWidget *widget);
void        ewl_widget_unrealize(EwlWidget *widget);

/* WIDGET EVENT CALLBACKS */
void        ewl_widget_realize_callback(void     *object,
                                        EwlEvent *event,
                                        void     *data);
void        ewl_widget_unrealize_callback(void     *object,
                                          EwlEvent *event,
                                          void     *data);

/* WIDGET HANDLERS */
void        ewl_widget_show_handler(void    *object,
                                    char    *type,
                                    EwlHash *params);
void        ewl_widget_hide_handler(void    *object,
                                    char    *type,
                                    EwlHash *params);
void        ewl_widget_resize_handler(void    *object,
                                      char    *type,
                                      EwlHash *params);

/* WIDGET SHOW/HIDE/RESIZE FUNCTIONS */
void        ewl_widget_show(EwlWidget *widget);
void        ewl_widget_hide(EwlWidget *widget);
void        ewl_widget_resize(EwlWidget *widget);

/* WIDGET RECT/LAYOUT FUNCTIONS */

EwlRequisition *ewl_widget_get_requisition(EwlWidget *widget);

EwlRect    *ewl_widget_get_rect(EwlWidget *widget);
void        ewl_widget_set_rect(EwlWidget *widget, EwlRect *rect);

EwlRect    *ewl_widget_get_requested_rect(EwlWidget *widget);
void        ewl_widget_set_requested_rect(EwlWidget *widget, EwlRect *rect);

EwlRect    *ewl_widget_get_min_rect(EwlWidget *widget);
void        ewl_widget_set_min_rect(EwlWidget *widget, EwlRect *rect);

EwlRect    *ewl_widget_get_max_rect(EwlWidget *widget);
void        ewl_widget_set_max_rect(EwlWidget *widget, EwlRect *rect);

EwlRect    *ewl_widget_get_resize_rect(EwlWidget *widget);
void        ewl_widget_set_resize_rect(EwlWidget *widget, EwlRect *rect);

EwlRect    *ewl_widget_get_previous_rect(EwlWidget *widget);
void        ewl_widget_set_previous_rect(EwlWidget *widget, EwlRect *rect);

/* WIDGET PADDING FUNCTIONS */
int        *ewl_widget_get_padding(EwlWidget *widget);
void        ewl_widget_set_padding(EwlWidget *widget,
                                   int l, int t, int b, int r);

/* WIDGET FLAG FUNCTIONS */
char        ewl_widget_is_realized(EwlWidget *widget);
char        ewl_widget_is_visible(EwlWidget *widget);
char        ewl_widget_is_focused(EwlWidget *widget);
char        ewl_widget_is_grabbed(EwlWidget *widget);
char        ewl_widget_is_container(EwlWidget *widget);
char        ewl_widget_can_resize(EwlWidget *widget);

void        ewl_widget_set_flag(EwlWidget *widget, char *flag, char value);
char        ewl_widget_get_flag(EwlWidget *widget, char *flag);

/* WIDGET EVAS/EVAS_OBJECT FUNCTIONS */
Evas        ewl_widget_get_evas(EwlWidget *widget);
void        ewl_widget_set_background(EwlWidget *w,
                                      Evas_Object im, char tiled);
Evas_Object ewl_widget_get_background(EwlWidget *w);

int         ewl_widget_get_layer(EwlWidget *widget);
void        ewl_widget_set_layer(EwlWidget *widget, 
                                 int        stacking_layer);

EwlWidget  *ewl_widget_get_parent(EwlWidget *widget);
void        ewl_widget_set_parent(EwlWidget *widget, EwlWidget *parent);

#endif /* _WIDGET_H_ */
