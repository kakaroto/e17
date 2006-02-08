#include "ewl_test.h"

static Ewl_Widget *colordialog_button = NULL;

static void
__colordialog_cb_value_changed(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
  	Ewl_Dialog_Event *cd_ev;
  
	cd_ev = ev;
	if (cd_ev->response == EWL_STOCK_OK)
	{
		Ewl_Widget *o;
		unsigned int r, g, b, a;

		o = ewl_widget_name_find("colour_preview");

		ewl_colordialog_current_rgb_get(EWL_COLORDIALOG(w), &r, &g, &b);
		a = ewl_colordialog_alpha_get(EWL_COLORDIALOG(w));

		ewl_widget_color_set(o, r, g, b, a);
	}
	ewl_widget_destroy(w);
}

static void
__colordialog_cb_launch(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Widget *o;
	int r, g, b, a;

	o = ewl_widget_name_find("colour_preview");
	ewl_widget_color_get(o, &r, &g, &b, &a);

	o = ewl_colordialog_new();
	ewl_colordialog_previous_rgb_set(EWL_COLORDIALOG(o), r, g, b);
	ewl_colordialog_alpha_set(EWL_COLORDIALOG(o), a);
	ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED, 
				__colordialog_cb_value_changed, NULL);
	ewl_widget_show(o);
}

static void 
__destroy_colordialog_test_window(Ewl_Widget *w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(colordialog_button, EWL_CALLBACK_CLICKED,
			    __create_colordialog_test_window, NULL);
}

void
__create_colordialog_test_window(Ewl_Widget *w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	Ewl_Widget *colordialog_win;
	Ewl_Widget *box, *o;

	colordialog_button = w;

	colordialog_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(colordialog_win), "Colordialog Test");
	ewl_window_name_set(EWL_WINDOW(colordialog_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(colordialog_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
				__create_colordialog_test_window);
		ewl_callback_append(colordialog_win, EWL_CALLBACK_DELETE_WINDOW,
				    __destroy_colordialog_test_window, NULL);
	} else
		ewl_callback_append(colordialog_win, EWL_CALLBACK_DELETE_WINDOW,
				__close_main_window, NULL);
	ewl_widget_show(colordialog_win);

	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(colordialog_win), box);
	ewl_widget_show(box);

	o = NEW(Ewl_Widget, 1);
	ewl_widget_init(o);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_object_minimum_size_set(EWL_OBJECT(o), 150, 20);
	ewl_widget_name_set(o, "colour_preview");
	ewl_widget_color_set(o, 255, 255, 255, 255);
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_button_label_set(EWL_BUTTON(o), "Launch Colour Dialog");
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, __colordialog_cb_launch, NULL);
	ewl_widget_show(o);
}

