#ifndef _EVENT_TRANSLATORS_H_
#define _EVENT_TRANSLATORS_H_ 1

#include "includes.h"
#include "ll.h"
#include "event.h"
#include "layout.h"
#include "widget.h"

#define EWL_MAX_EVENT_TRANSLATORS LASTEvent /* thanks! giblet */

/* defined in state.h */
extern EwlWidget *ewl_widget_grabbed_get();
extern EwlWidget *ewl_widget_focused_get();
/* defined in ewlwindow.h */
extern EwlWidget *ewl_window_find_by_xwin(Window xwin);

typedef struct _EwlEventTranslator EwlEventTranslator;
typedef EwlEvent *(*EwlTranslatorCallback)(EwlEventTranslator *t, XEvent *xev);
struct _EwlEventTranslator	{
	EwlEventType           type;
	char                  *desc;
	EwlTranslatorCallback  cb;
};

EwlEventTranslator *ewl_translator_new(EwlTranslatorCallback  cb,
                                       char                  *desc,
                                       EwlEventType           type);
EwlWidget          *ewl_widget_find_by_xwin(Window xwin);

EwlEvent *cb_ewl_event_keypress_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_keyrelease_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_buttonpress_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_buttonrelease_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_motion_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_enter_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_leave_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_focusin_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_focusout_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_expose_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_visibility_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_create_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_destroy_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_unmapnotify_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_mapnotify_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_maprequest_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_reparentnotify_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_configurenotify_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_configurerequest_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_circulatenotify_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_circulaterequest_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_property_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_colormap_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_clientmessage_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_selectionnotify_translate(EwlEventTranslator *t, XEvent *xev);
EwlEvent *cb_ewl_event_selectionrequest_translate(EwlEventTranslator *t, XEvent *xev);

#endif /* _EVENT_TRANSLATORS_H_ */

