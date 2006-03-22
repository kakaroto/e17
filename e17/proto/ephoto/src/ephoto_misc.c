#include "ephoto.h"

/****************CALLBACKS*******************/
void
destroy_cb(Ewl_Widget * w, void *event, void *data)
{
	/****Shutdown Ewl Completely(Main Window close)****/
	ewl_widget_destroy(w);
	ewl_main_quit();
	return;
	event = NULL;
	data = NULL;
	/*************************************************/
}

void
destroys_cb(Ewl_Widget * w, void *event, void *data)
{
	/****Shutdown the slideshow****/
	ewl_widget_destroy(w);
	ecore_timer_del(s->timer);
	wino = 0;
	if ( mainwin == 0 ) {
		ewl_main_quit();
	}
	return;
	data = NULL;
	/*****************************/
}

void
destroyp_cb(Ewl_Widget * w, void *event, void *data)
{
	/****Shut down the presentation****/
	ewl_widget_destroy(s->wins);
	wino = 0;
	if ( mainwin == 0 ) {
		ewl_main_quit();
	}
	return;
	data = NULL;
	/**********************************/
}

void
rad_cb(Ewl_Widget * w, void *event, void *data)
{
	/****Setup Callbacks for the radio buttons for size/length****/
	if ( w == m->slidetime ) {
		ewl_checkbutton_checked_set(EWL_CHECKBUTTON(m->audiolen), FALSE);
	}
	if ( w == m->fullrad ) {
		ewl_text_text_set(EWL_TEXT(m->wsize), "Full");
		ewl_text_text_set(EWL_TEXT(m->hsize), "Full");
		ewl_entry_editable_set(EWL_ENTRY(m->wsize), 0);
		ewl_entry_editable_set(EWL_ENTRY(m->hsize), 0);
	}
	if ( w == m->rad4 ) {
		ewl_text_text_set(EWL_TEXT(m->wsize), argwidth);
		ewl_text_text_set(EWL_TEXT(m->hsize), argheight);
		ewl_entry_editable_set(EWL_ENTRY(m->wsize), 1);
		ewl_entry_editable_set(EWL_ENTRY(m->hsize), 1);
	}
	/****************************************************/
}
