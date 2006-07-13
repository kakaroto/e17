#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <Ecore.h>
#include <Ecore_X.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Ecore_Event_Handler *ewl_dnd_enter_handler = NULL;
Ecore_Event_Handler *ewl_dnd_position_handler = NULL;
Ecore_Event_Handler *ewl_dnd_status_handler = NULL;
Ecore_Event_Handler *ewl_dnd_leave_handler = NULL;
Ecore_Event_Handler *ewl_dnd_drop_handler = NULL;
Ecore_Event_Handler *ewl_dnd_finished_handler = NULL;

static int create_test(Ewl_Container *box);

static void ewl_dnd_snoop_cb_realize(Ewl_Widget *w, void *event, void *data);

static int ewl_dnd_snoop_cb_enter(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_position(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_status(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_leave(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_drop(void *data, int type, void *ev);
static int ewl_dnd_snoop_cb_finished(void *data, int type, void *ev);

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

	Ewl_Widget *scroll, *o;

	/* Register DND handlers */
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

	o = ewl_entry_new();
	ewl_container_child_append(EWL_CONTAINER(box), o);
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
	ewl_callback_append(scroll, EWL_CALLBACK_REALIZE,
			ewl_dnd_snoop_cb_realize, NULL);
	ewl_widget_show(scroll);

	text = ewl_text_new();
	ewl_container_child_append(EWL_CONTAINER(scroll), text);
	ewl_widget_show(text);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Clear");
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
					ewl_dnd_snoop_cb_clear, NULL);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
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
ewl_dnd_snoop_cb_realize(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Embed *embed;
	embed = ewl_embed_widget_find(w);
	ecore_x_dnd_aware_set((Ecore_X_Window)embed->evas_window, TRUE);
}

static int
ewl_dnd_snoop_cb_enter(void *data, int type, void *ev)
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
ewl_dnd_snoop_cb_position(void *data, int type, void *ev)
{
	char buf[1024];
	Ecore_X_Event_Xdnd_Position *event = ev;

	snprintf(buf, sizeof(buf), "\nXdnd Position\n");
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tWindow: %d\n\tSource: %d\n", event->win, 
								event->source);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tPosition: %d,%d\n", event->position.x, 
							event->position.y);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tAction: %s\n", 
					XGetAtomName(ecore_x_display_get(), 
					event->action));
	ewl_dnd_snoop_output(buf);

	return 1;
}

static int
ewl_dnd_snoop_cb_status(void *data, int type, void *ev)
{
	char buf[1024];
	Ecore_X_Event_Xdnd_Status *event = ev;

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

	snprintf(buf, sizeof(buf), "\tAction: %s\n", 
					XGetAtomName(ecore_x_display_get(), 
					event->action));
	ewl_dnd_snoop_output(buf);

	return 1;
}

static int
ewl_dnd_snoop_cb_leave(void *data, int type, void *ev)
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
ewl_dnd_snoop_cb_drop(void *data, int type, void *ev)
{
	char buf[1024];
	Ecore_X_Event_Xdnd_Drop *event = ev;

	snprintf(buf, sizeof(buf), "\nXdnd Drop\n");
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tWindow: %d\n\tSource: %d\n", 
						event->win, event->source);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tAction: %s\n", 
					XGetAtomName(ecore_x_display_get(), 
					event->action));
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tPosition: %d,%d\n", 
					event->position.x, event->position.y);
	ewl_dnd_snoop_output(buf);

	return 1;
}

static int
ewl_dnd_snoop_cb_finished(void *data, int type, void *ev)
{
	char buf[1024];
	Ecore_X_Event_Xdnd_Finished *event = ev;

	snprintf(buf, sizeof(buf), "\nXdnd Finished\n");
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tWindow: %d\n\tTarget: %d\n", 
						event->win, event->target);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tCompleted: %d\n", event->completed);
	ewl_dnd_snoop_output(buf);

	snprintf(buf, sizeof(buf), "\tAction: %s\n", 
					XGetAtomName(ecore_x_display_get(), 
					event->action));
	ewl_dnd_snoop_output(buf);

	return 1;
}

static void
ewl_dnd_snoop_cb_clear(Ewl_Widget *w, void *ev, void *data)
{
	ewl_text_clear(EWL_TEXT(text));
}


