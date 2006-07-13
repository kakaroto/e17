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

void 
test_info(Ewl_Test *test)
{
	test->name = "DND Snoop";
	test->tip = "Watches for Xdnd events and prints information to the console.";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_ADVANCED;
}

static int
create_test(Ewl_Container *box)
{

	Ewl_Widget *hbox, *scroll, *o;

	/* Register DND handlers */
	ewl_dnd_enter_handler = ecore_event_handler_add(ECORE_X_EVENT_XDND_ENTER, ewl_dnd_snoop_cb_enter, NULL);
	ewl_dnd_position_handler = ecore_event_handler_add(ECORE_X_EVENT_XDND_POSITION, ewl_dnd_snoop_cb_position, NULL);
	ewl_dnd_status_handler = ecore_event_handler_add(ECORE_X_EVENT_XDND_STATUS, ewl_dnd_snoop_cb_status, NULL);
	ewl_dnd_leave_handler = ecore_event_handler_add(ECORE_X_EVENT_XDND_LEAVE, ewl_dnd_snoop_cb_leave, NULL);
	ewl_dnd_drop_handler = ecore_event_handler_add(ECORE_X_EVENT_XDND_DROP, ewl_dnd_snoop_cb_drop, NULL);
	ewl_dnd_finished_handler = ecore_event_handler_add(ECORE_X_EVENT_XDND_FINISHED, ewl_dnd_snoop_cb_finished, NULL);

	scroll = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(box), scroll);
	ewl_callback_append(scroll, EWL_CALLBACK_REALIZE,
			ewl_dnd_snoop_cb_realize, NULL);
	ewl_widget_show(scroll);

	o = ewl_entry_new();
	ewl_container_child_append(EWL_CONTAINER(scroll), o);
	ewl_widget_name_set(o, "entry");
	ewl_entry_multiline_set(EWL_ENTRY(o), TRUE);
	ewl_text_wrap_set(EWL_TEXT(o), TRUE);
	ewl_text_font_size_set(EWL_TEXT(o), 12);

	ewl_text_underline_color_set(EWL_TEXT(o), 0, 0, 0, 255);
	ewl_text_double_underline_color_set(EWL_TEXT(o), 50, 50, 50, 255);
	ewl_text_shadow_color_set(EWL_TEXT(o), 128, 128, 128, 128);
	ewl_text_outline_color_set(EWL_TEXT(o), 200, 200, 200, 200);
	ewl_text_strikethrough_color_set(EWL_TEXT(o), 0, 0, 0, 255);
	ewl_text_glow_color_set(EWL_TEXT(o), 0, 255, 0, 128);

	ewl_object_fill_policy_set(EWL_OBJECT(o), 
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VFILL);
	ewl_widget_show(o);

	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), hbox);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_NONE);
	ewl_widget_show(hbox);

	return 1;
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
	Ecore_X_Event_Xdnd_Enter *event = ev;

	printf("\nXdnd Enter\n");
	printf("\tWindow: %d\nSource: %d\n", event->win, event->source);
	for (i = 0; i < event->num_types; i++) {
		printf("\tType: %s\n", event->types[i]);
	}
	return 1;
}

static int
ewl_dnd_snoop_cb_position(void *data, int type, void *ev)
{
	Ecore_X_Event_Xdnd_Position *event = ev;

	printf("\nXdnd Position\n");
	printf("\tWindow: %d\nSource: %d\n", event->win, event->source);
	printf("\tPosition: %d,%d\n", event->position.x, event->position.y);
	printf("\tAction: %s\n", XGetAtomName(ecore_x_display_get(), event->action));
	return 1;
}

static int
ewl_dnd_snoop_cb_status(void *data, int type, void *ev)
{
	Ecore_X_Event_Xdnd_Status *event = ev;

	printf("\nXdnd Status\n");
	printf("\tWindow: %d\nTarget: %d\n", event->win, event->target);
	printf("\tAccepts: %d\n", event->will_accept);
	printf("\tRegion: %d,%d %dx%d\n", event->rectangle.x, event->rectangle.y,
			event->rectangle.width, event->rectangle.height);
	printf("\tAction: %s\n", XGetAtomName(ecore_x_display_get(), event->action));
	return 1;
}

static int
ewl_dnd_snoop_cb_leave(void *data, int type, void *ev)
{
	Ecore_X_Event_Xdnd_Leave *event = ev;

	printf("\nXdnd Leave\n");
	printf("\tWindow: %d\nSource: %d\n", event->win, event->source);
	return 1;
}

static int
ewl_dnd_snoop_cb_drop(void *data, int type, void *ev)
{
	Ecore_X_Event_Xdnd_Drop *event = ev;

	printf("\nXdnd Drop\n");
	printf("\tWindow: %d\nSource: %d\n", event->win, event->source);
	printf("\tAction: %s\n", XGetAtomName(ecore_x_display_get(), event->action));
	printf("\tPosition: %d,%d\n", event->position.x, event->position.y);
	return 1;
}

static int
ewl_dnd_snoop_cb_finished(void *data, int type, void *ev)
{
	Ecore_X_Event_Xdnd_Finished *event = ev;

	printf("\nXdnd Finished\n");
	printf("\tWindow: %d\nTarget: %d\n", event->win, event->target);
	printf("\tCompleted: %d\n", event->completed);
	printf("\tAction: %s\n", XGetAtomName(ecore_x_display_get(), event->action));
	return 1;
}
