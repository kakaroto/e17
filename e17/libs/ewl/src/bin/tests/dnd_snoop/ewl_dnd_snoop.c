/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef ENABLE_EWL_SOFTWARE_X11
#include <Ecore.h>
#include <Ecore_X.h>
#include <X11/Xlib.h>
#endif

/**
 * @addtogroup Ewl_Dnd
 * @section dnd_tut Tutorial
 *
 * In order to make DND handling easier for the end programmer, we've added a
 * simplification API to EWL. It currently supports receiving drops and some
 * limited dragging of widgets. Getting started is fairly simple, even for some
 * rather complicated widgets. As an example, I'll run through the DND support
 * added to the entry widget.
 * 
 * The first step to setup DND support on a widget is to decide which MIME types
 * are allowed and understood by the widget. The entry displays text, so
 * accepting the type "text/plain" is a safe choice. A NULL terminated list of
 * type strings is passed to the ewl_dnd_accepted_types_set, which enables DND
 * responses for the widget and helps to negotiate whether a drop will be
 * accepted at a given position.
 * 
 * @code
 * const char *text_types[] = { "text/plain", NULL };
 * 
 * ewl_dnd_accepted_types_set(EWL_WIDGET(e), text_types);
 * @endcode
 * 
 * One key feature for DND support in the entry widget was to allow dragging
 * text to arbitrary positions within the visible text area. This is
 * accomplished by registering a callback for the EWL_CALLBACK_DND_POSITION
 * event on the entry widget.
 * 
 * @code
 * ewl_callback_append(w, EWL_CALLBACK_DND_POSITION, ewl_entry_cb_dnd_position, NULL);
 * @endcode
 * 
 * When the mouse moves during a DND event over the specified entry w the
 * ewl_entry_cb_dnd_position function will be called. This function prototype
 * looks like all other EWL callback prototypes:
 * 
 * @code
 * void ewl_entry_cb_dnd_position(Ewl_Widget *w, void *ev, void *data);
 * @endcode
 * 
 * In this case, the void *ev parameter points to a Ewl_Event_Dnd_Position
 * struct, which contains more detailed information about the event. We can use
 * the coordinates from the event to position the cursor within our entry to
 * receive the dropped data. Since the entry widget inherits from the text
 * widget, the text calls are used directly on the widget to alter the entry
 * contents. The code to accomplish this is rather small when the extra
 * debugging information is removed:
 * 
 * @code
 * void
 * ewl_entry_cb_dnd_position(Ewl_Widget *w, void *ev, void *data)
 * {
 *         Ewl_Event_Dnd_Position *event;
 *         Ewl_Text *txt;
 * 
 *         event = ev;
 *         txt = EWL_TEXT(w);
 * 
 *         if (EWL_ENTRY(w)->editable && !DISABLED(w)) {
 *                 ewl_widget_focus_send(w);
 *                 ewl_text_cursor_position_set(txt, ewl_text_coord_index_map(txt, event->x, event->y));
 *        }
 * }
 * @endcode
 * 
 * Once the cursor has been positioned, the only event we care about is
 * receiving the data from the drop. This is accomplished by using the
 * EWL_CALLBACK_DND_DATA callback which should also be placed on the entry
 * widget.
 * 
 * @code
 * ewl_callback_append(w, EWL_CALLBACK_DND_DATA, ewl_entry_cb_dnd_data, NULL);
 * @endcode
 * 
 * The function prototype for ewl_entry_cb_dnd_data is identical to
 * ewl_entry_cb_dnd_position, but the void *ev parameter is of type
 * Ewl_Event_Dnd_Data. Since we only registered to receive plain text data
 * dropped on the entry, we can insert the event data directly into the entry at
 * the current cursor position.
 * 
 * @code
 * void
 * ewl_entry_cb_dnd_data(Ewl_Widget *w, void *ev, void *data)
 * {
 *         Ewl_Event_Dnd_Data *event;
 *         Ewl_Text *txt;
 * 
 *         event = ev;
 *         txt = EWL_TEXT(w);
 * 
 *         if (EWL_ENTRY(w)->editable && !DISABLED(w)) {
 *                 ewl_text_text_insert(txt, event->data,
 *                                         ewl_text_cursor_position_get(txt));
 *         }
 * }
 * @endcode
 * 
 * Considering the complicated nature of the Xdnd protocol, we are able to
 * accomplish a considerable amount of work in very few lines of code. While
 * some flexibility is sacrificed to achieve this, almost all of the protocol
 * events are available for widgets to override as they please.
 * 
 * Check back for followup information to handle drag events on widgets.
 */

Ecore_Event_Handler *ewl_dnd_enter_handler = NULL;
Ecore_Event_Handler *ewl_dnd_position_handler = NULL;
Ecore_Event_Handler *ewl_dnd_status_handler = NULL;
Ecore_Event_Handler *ewl_dnd_leave_handler = NULL;
Ecore_Event_Handler *ewl_dnd_drop_handler = NULL;
Ecore_Event_Handler *ewl_dnd_finished_handler = NULL;
Ecore_Event_Handler *ewl_dnd_selection_clear_handler = NULL;
Ecore_Event_Handler *ewl_dnd_selection_request_handler = NULL;
Ecore_Event_Handler *ewl_dnd_selection_notify_handler = NULL;
Ecore_Event_Handler *ewl_dnd_client_message_handler = NULL;

static int create_test(Ewl_Container *box);

static void ewl_dnd_snoop_cb_dnd_position(Ewl_Widget *w, void *event, void *data);
static void ewl_dnd_snoop_cb_dnd_drop(Ewl_Widget *w, void *event, void *data);
static void ewl_dnd_snoop_cb_dnd_data(Ewl_Widget *w, void *event, void *data);
static void ewl_dnd_snoop_cb_dnd_data_request(Ewl_Widget *w, void *event, void *data);

#ifdef ENABLE_EWL_SOFTWARE_X11
static int ewl_dnd_snoop_cb_enter(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_position(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_status(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_leave(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_drop(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_finished(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_selection_clear(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_selection_notify(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_selection_request(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_client_message(void *data, int type, void *ev);
#endif

static void ewl_dnd_snoop_cb_clear(Ewl_Widget *w, void *ev, void *data);

static Ewl_Widget *text = NULL;

void 
test_info(Ewl_Test *test)
{
	test->name = "DND Snoop";
	test->tip = "Watches for Xdnd events and prints information "
			"to the console.";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_ADVANCED;
}

static int
create_test(Ewl_Container *box)
{
	const char *text_types[] = { "UTF8_STRING", "text/plain", NULL };
	Ewl_Widget *scroll, *o;

	/* Register DND handlers */
#ifdef ENABLE_EWL_SOFTWARE_X11
	ewl_dnd_enter_handler = ecore_event_handler_add(
					ECORE_X_EVENT_XDND_ENTER, 
					ewl_dnd_snoop_cb_enter, NULL);
	ewl_dnd_position_handler = ecore_event_handler_add(
					ECORE_X_EVENT_XDND_POSITION, 
					ewl_dnd_snoop_cb_position, NULL);
	ewl_dnd_status_handler = ecore_event_handler_add(
					ECORE_X_EVENT_XDND_STATUS, 
					ewl_dnd_snoop_cb_status, NULL);
	ewl_dnd_leave_handler = ecore_event_handler_add(
					ECORE_X_EVENT_XDND_LEAVE, 
					ewl_dnd_snoop_cb_leave, NULL);
	ewl_dnd_drop_handler = ecore_event_handler_add(
					ECORE_X_EVENT_XDND_DROP, 
					ewl_dnd_snoop_cb_drop, NULL);
	ewl_dnd_finished_handler = ecore_event_handler_add(
					ECORE_X_EVENT_XDND_FINISHED, 
					ewl_dnd_snoop_cb_finished, NULL);
	ewl_dnd_selection_clear_handler = ecore_event_handler_add(
					ECORE_X_EVENT_SELECTION_CLEAR,
					ewl_dnd_snoop_cb_selection_clear, NULL);
	ewl_dnd_selection_request_handler = ecore_event_handler_add(
					ECORE_X_EVENT_SELECTION_REQUEST,
					ewl_dnd_snoop_cb_selection_request, NULL);
	ewl_dnd_selection_notify_handler = ecore_event_handler_add(
					ECORE_X_EVENT_SELECTION_NOTIFY,
					ewl_dnd_snoop_cb_selection_notify, NULL);
	ewl_dnd_client_message_handler = ecore_event_handler_add(
					ECORE_X_EVENT_CLIENT_MESSAGE,
					ewl_dnd_snoop_cb_client_message, NULL);
#endif

	o = ewl_entry_new();
	ewl_dnd_accepted_types_set(o, text_types);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_callback_append(o, EWL_CALLBACK_DND_POSITION, ewl_dnd_snoop_cb_dnd_position, NULL);
	ewl_callback_append(o, EWL_CALLBACK_DND_DROP, ewl_dnd_snoop_cb_dnd_drop, NULL);
	ewl_callback_append(o, EWL_CALLBACK_DND_DATA_RECEIVED, ewl_dnd_snoop_cb_dnd_data, NULL);
	ewl_widget_name_set(o, "entry");
	ewl_entry_multiline_set(EWL_ENTRY(o), TRUE);
	ewl_text_wrap_set(EWL_TEXT(o), TRUE);
	ewl_text_font_size_set(EWL_TEXT(o), 12);
	ewl_text_text_set(EWL_TEXT(o), "Drop Here");

	ewl_text_underline_color_set(EWL_TEXT(o), 0, 0, 0, 255);
	ewl_text_double_underline_color_set(EWL_TEXT(o), 50, 50, 50, 255);
	ewl_text_shadow_color_set(EWL_TEXT(o), 128, 128, 128, 128);
	ewl_text_outline_color_set(EWL_TEXT(o), 200, 200, 200, 200);
	ewl_text_strikethrough_color_set(EWL_TEXT(o), 0, 0, 0, 255);
	ewl_text_glow_color_set(EWL_TEXT(o), 0, 255, 0, 128);

	ewl_object_fill_policy_set(EWL_OBJECT(o), 
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);
	ewl_widget_show(o);

	scroll = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(box), scroll);
	ewl_widget_show(scroll);

	text = ewl_text_new();
	ewl_container_child_append(EWL_CONTAINER(scroll), text);
	ewl_widget_show(text);

	o = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
	ewl_container_child_append(box, o);
	ewl_widget_show(o);

	box = EWL_CONTAINER(o);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Clear");
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
					ewl_dnd_snoop_cb_clear, NULL);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Drag This");
	ewl_callback_append(o, EWL_CALLBACK_DND_DATA_REQUEST, ewl_dnd_snoop_cb_dnd_data_request, NULL);
	ewl_dnd_provided_types_set(o, text_types);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
	ewl_container_child_append(box, o);
	ewl_widget_show(o);

	return 1;
}

static void
ewl_dnd_snoop_output(char *buf)
{
	printf(buf);
	ewl_text_text_append(EWL_TEXT(text), buf);
}

static void
ewl_dnd_snoop_cb_dnd_position(Ewl_Widget *w, void *event, 
						void *data __UNUSED__)
{
	char buf[PATH_MAX];
	Ewl_Event_Mouse *ev = event;
	snprintf(buf, PATH_MAX, "Position event on widget %p: %d %d\n", w, ev->x, ev->y);
	ewl_dnd_snoop_output(buf);
}

static void
ewl_dnd_snoop_cb_dnd_drop(Ewl_Widget *w, void *event, 
						void *data __UNUSED__)
{
	char buf[PATH_MAX];
	Ewl_Event_Dnd_Drop *ev = event;
	snprintf(buf, PATH_MAX, "Drop event on widget %p: %d %d %p\n", w, ev->x, ev->y, ev->data);
	ewl_dnd_snoop_output(buf);
}

static void
ewl_dnd_snoop_cb_dnd_data(Ewl_Widget *w, void *event, 
						void *data __UNUSED__)
{
	char buf[PATH_MAX];
	Ewl_Event_Dnd_Data_Received *ev = event;
	snprintf(buf, PATH_MAX, "Data event on widget %p: %p length %d\n", w, ev->data, ev->len);
	ewl_dnd_snoop_output(buf);
}

static void
ewl_dnd_snoop_cb_dnd_data_request(Ewl_Widget *w, void *event, 
						void *data __UNUSED__)
{
	char buf[PATH_MAX];
	Ewl_Embed *emb;
	Ewl_Event_Dnd_Data_Request *ev = event;

	snprintf(buf, PATH_MAX, "Data request on widget %p: type %s\n", w, ev->type);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, PATH_MAX, "\tSending response: %s\n", w->inheritance);
	ewl_dnd_snoop_output(buf);
	emb = ewl_embed_widget_find(w);
	if (emb)
		ewl_engine_embed_dnd_drag_data_send(emb, ev->handle,
				(void *)w->inheritance,
				strlen(w->inheritance) + 1);
}

#ifdef ENABLE_EWL_SOFTWARE_X11
static int
ewl_dnd_snoop_cb_enter(void *data __UNUSED__, int type __UNUSED__, void *ev)
{
	int i;
	char buf[1024];
	Ecore_X_Event_Xdnd_Enter *event = ev;

	snprintf(buf, sizeof(buf), "\nXdnd Enter\n");
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tWindow: %d\n\tSource: %d\n", 
					event->win, event->source);
	ewl_dnd_snoop_output(buf);

	for (i = 0; i < event->num_types; i++) {
		snprintf(buf, sizeof(buf), "\tType: %s\n", event->types[i]);
		ewl_dnd_snoop_output(buf);
	}
	return 1;
}

static int
ewl_dnd_snoop_cb_position(void *data __UNUSED__, int type __UNUSED__, void *ev)
{
	char buf[1024];
	Ecore_X_Event_Xdnd_Position *event = ev;
	char *name;

	snprintf(buf, sizeof(buf), "\nXdnd Position\n");
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tWindow: %d\n\tSource: %d\n", event->win, 
								event->source);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tPosition: %d,%d\n", event->position.x, 
							event->position.y);
	ewl_dnd_snoop_output(buf);

	name = XGetAtomName(ecore_x_display_get(), event->action);
	snprintf(buf, sizeof(buf), "\tAction: %s\n", name);
	XFree(name);
	ewl_dnd_snoop_output(buf);

	return 1;
}

static int
ewl_dnd_snoop_cb_status(void *data __UNUSED__, int type __UNUSED__, void *ev)
{
	char buf[1024];
	Ecore_X_Event_Xdnd_Status *event = ev;
	char *name;

	snprintf(buf, sizeof(buf), "\nXdnd Status\n");
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tWindow: %d\n\tTarget: %d\n", 
						event->win, event->target);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tAccepts: %d\n", event->will_accept);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tRegion: %d,%d %dx%d\n", 
						event->rectangle.x,
						event->rectangle.y,
						event->rectangle.width, 
						event->rectangle.height);
	ewl_dnd_snoop_output(buf);

	name = XGetAtomName(ecore_x_display_get(), event->action);
	snprintf(buf, sizeof(buf), "\tAction: %s\n", name);
	XFree(name);
	ewl_dnd_snoop_output(buf);

	return 1;
}

static int
ewl_dnd_snoop_cb_leave(void *data __UNUSED__, int type __UNUSED__, void *ev)
{
	char buf[1024];
	Ecore_X_Event_Xdnd_Leave *event = ev;

	snprintf(buf, sizeof(buf), "\nXdnd Leave\n");
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tWindow: %d\n\tSource: %d\n", 
						event->win, event->source);
	ewl_dnd_snoop_output(buf);

	return 1;
}

static int
ewl_dnd_snoop_cb_drop(void *data __UNUSED__, int type __UNUSED__, void *ev)
{
	char buf[1024];
	Ecore_X_Event_Xdnd_Drop *event = ev;
	char *name;

	snprintf(buf, sizeof(buf), "\nXdnd Drop\n");
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tWindow: %d\n\tSource: %d\n", 
						event->win, event->source);
	ewl_dnd_snoop_output(buf);

	name = XGetAtomName(ecore_x_display_get(), event->action);
	snprintf(buf, sizeof(buf), "\tAction: %s\n", name);
	XFree(name);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tPosition: %d,%d\n", 
					event->position.x, event->position.y);
	ewl_dnd_snoop_output(buf);

	return 1;
}

static int
ewl_dnd_snoop_cb_finished(void *data __UNUSED__, int type __UNUSED__, void *ev)
{
	char buf[1024];
	Ecore_X_Event_Xdnd_Finished *event = ev;
	char *name;

	snprintf(buf, sizeof(buf), "\nXdnd Finished\n");
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tWindow: %d\n\tTarget: %d\n", 
						event->win, event->target);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tCompleted: %d\n", event->completed);
	ewl_dnd_snoop_output(buf);

	name = XGetAtomName(ecore_x_display_get(), event->action);
	snprintf(buf, sizeof(buf), "\tAction: %s\n", name);
	XFree(name);
	ewl_dnd_snoop_output(buf);

	return 1;
}

static int
ewl_dnd_snoop_cb_selection_clear(void *data __UNUSED__, int type __UNUSED__, 
								void *ev)
{
	char buf[1024];
	Ecore_X_Event_Selection_Clear *event = ev;

	snprintf(buf, sizeof(buf), "\nSelection Clear\n");
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tWindow: %d\n\tSelection%d\n", 
						event->win, event->selection);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tTime: %d\n", event->time);
	ewl_dnd_snoop_output(buf);

	return 1;

}

static int
ewl_dnd_snoop_cb_selection_request(void *data __UNUSED__, int type __UNUSED__, 
								void *ev)
{
	char buf[1024];
	Ecore_X_Event_Selection_Request *event = ev;
	char *name;

	snprintf(buf, sizeof(buf), "\nSelection Request\n");
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tOwner: %d\n", event->owner);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tRequestor: %d\n", event->requestor);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tTime: %d\n", event->time);
	ewl_dnd_snoop_output(buf);

	name = XGetAtomName(ecore_x_display_get(), event->selection);
	snprintf(buf, sizeof(buf), "\tSelection: %s\n", name);
	XFree(name);
	ewl_dnd_snoop_output(buf);

	name = XGetAtomName(ecore_x_display_get(), event->target);
	snprintf(buf, sizeof(buf), "\tTarget: %s\n", name);
	XFree(name);
	ewl_dnd_snoop_output(buf);

	name = XGetAtomName(ecore_x_display_get(), event->property);
	snprintf(buf, sizeof(buf), "\tProperty: %s\n", name);
	XFree(name);
	ewl_dnd_snoop_output(buf);

	return 1;

}

static int
ewl_dnd_snoop_cb_selection_notify(void *data __UNUSED__, int type __UNUSED__, 
								void *ev)
{
	char buf[1024];
	Ecore_X_Event_Selection_Notify *event = ev;

	snprintf(buf, sizeof(buf), "\nSelection Notify\n");
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tWindow: %d\n\tSelection%d\n", 
						event->win, event->selection);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tTime: %d\n", event->time);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tTarget: %s\n", event->target);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tData: %p\n", event->data);
	ewl_dnd_snoop_output(buf);

	return 1;

}

static int
ewl_dnd_snoop_cb_client_message(void *data __UNUSED__, int type __UNUSED__, 
								void *ev)
{
	char buf[1024];
	Ecore_X_Event_Client_Message *event = ev;
	char *name;

	snprintf(buf, sizeof(buf), "\nClient Message\n");
	ewl_dnd_snoop_output(buf);

	name = XGetAtomName(ecore_x_display_get(), event->message_type);
	snprintf(buf, sizeof(buf), "\tWindow: %d\n\tType: %s\n", 
						event->win,
						name);
	XFree(name);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tTime: %d\n", event->time);
	ewl_dnd_snoop_output(buf);

	return 1;

}
#endif

static void
ewl_dnd_snoop_cb_clear(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	ewl_text_clear(EWL_TEXT(text));
}


