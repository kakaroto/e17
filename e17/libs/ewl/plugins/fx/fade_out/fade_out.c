
#include <Ewl.h>


void fx_start(Ewl_Widget * w);
void fx_cont(Ewl_FX_Timer * t);
void fx_stop(Ewl_Widget * w);
void create_about_dialog(void);
void create_settings_dialog(void);
static void close_about_dialog(Ewl_Widget * w, void *ev_data,
			       void *user_data);
static void close_settings_dialog(Ewl_Widget * w, void *ev_data,
				  void *user_data);


static double fps = 50;
static double timeout = 1.5;
static double start_val = 255.0;
static double end_val = 0.0;


Ewl_FX_Proto fade_out = {
	fx_start,
	fx_cont,
	fx_stop,
	EWL_FX_MODIFIES_ALPHA_CHANNEL,
	create_about_dialog,
	create_settings_dialog,
	"fade_out",
	NULL
};

struct
{
	Ewl_Widget *window;
	Ewl_Widget *text;
	Ewl_Widget *separator;
	Ewl_Widget *button_close;
}
about_dialog;

struct
{
	Ewl_Widget *window;
	Ewl_Widget *fps_label, *fps_spinner;
	Ewl_Widget *timeout_label, *timeout_spinner;
	Ewl_Widget *separator;
	Ewl_Widget *button_close, *button_cancel;
}
settings_dialog;





Ewl_FX_Proto *
load(void)
{
	return &fade_out;
}

void
fx_start(Ewl_Widget * w)
{
	double interval;
	double step;
	int r, g, b;
	int hits;

	ewl_fx_clip_box_get_color(w, &r, &g, &b, NULL);
	ewl_fx_clip_box_set_color(w, r, g, b, 255);

	interval = ewl_fx_calculate_interval(fps, timeout);
	step = ewl_fx_calculate_step(start_val, end_val, fps, timeout);
	hits = start_val - end_val / (int) (ceil(step));

	ewl_fx_timer_add(w, fade_out.name, interval, step, hits, NULL);
}


void
fx_cont(Ewl_FX_Timer * t)
{
	int r, g, b, a;

	ewl_fx_clip_box_get_color(t->widget, &r, &g, &b, &a);

	a -= (int) (ceil(t->step));

	if (a < 0)
		a = 0;

	ewl_fx_clip_box_set_color(t->widget, r, g, b, a);
}


void
fx_stop(Ewl_Widget * w)
{
	int r, g, b;

	ewl_fx_clip_box_get_color(w, &r, &g, &b, NULL);
	ewl_fx_clip_box_set_color(w, r, g, b, 255);
}





void
create_about_dialog(void)
{
	about_dialog.window = ewl_window_new();
	ewl_window_resize(about_dialog.window, 370, 150);
	ewl_window_set_title(about_dialog.window,
			     "EWL Fade Out Plugin: About");
	ewl_object_set_custom_size(EWL_OBJECT(about_dialog.window), 370, 150);
	ewl_callback_append(about_dialog.window, EWL_CALLBACK_DELETE_WINDOW,
			    close_about_dialog, NULL);
	ewl_widget_realize(about_dialog.window);

	about_dialog.text = ewl_text_new();
	ewl_text_set_text(about_dialog.text,
			  "           EWL Fade Out Plugin\n\n"
			  "Created by Christopher 'smugg' Rosendahl");
	ewl_object_set_alignment(EWL_OBJECT(about_dialog.text),
				 EWL_ALIGNMENT_CENTER);
	ewl_object_set_padding(EWL_OBJECT(about_dialog.text), 0, 0, 20, 0);
	ewl_container_append_child(EWL_CONTAINER(about_dialog.window),
				   about_dialog.text);
	ewl_widget_realize(about_dialog.text);

	about_dialog.separator = ewl_vseparator_new();
	ewl_object_set_padding(EWL_OBJECT(about_dialog.separator),
			       20, 20, 20, 20);
	ewl_container_append_child(EWL_CONTAINER(about_dialog.window),
				   about_dialog.separator);
	ewl_widget_realize(about_dialog.separator);

	about_dialog.button_close = ewl_button_new("Close");
	ewl_object_set_custom_size(EWL_OBJECT(about_dialog.button_close), 100,
				   17);
	ewl_object_set_alignment(EWL_OBJECT(about_dialog.button_close),
				 EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(about_dialog.window),
				   about_dialog.button_close);
	ewl_callback_append(about_dialog.button_close, EWL_CALLBACK_CLICKED,
			    close_about_dialog, NULL);
	ewl_widget_realize(about_dialog.button_close);

	ewl_widget_configure(about_dialog.window);
}





void
create_settings_dialog(void)
{

}






void
unload(void)
{
	fade_out.fx_start = NULL;
	fade_out.fx_cont = NULL;
	fade_out.fx_stop = NULL;
	fade_out.modifies = EWL_FX_MODIFIES_NONE;
	fade_out.name = NULL;
}





static void
close_about_dialog(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(about_dialog.window);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

static void
close_settings_dialog(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(settings_dialog.window);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
