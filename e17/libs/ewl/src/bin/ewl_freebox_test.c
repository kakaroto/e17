#include "ewl_test.h"
#include <stdlib.h>

static Ewl_Widget *fb_button;
static Ewl_Widget *sort_fb;

static void
ewl_freebox_cb_icon_change(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
								void *data)
{
	Ewl_Freebox *fb;

	fb = data;
	ewl_freebox_resort(EWL_FREEBOX(fb));
}

static int
ewl_freebox_cb_compare(Ewl_Widget *a, Ewl_Widget *b)
{
	const char *a_txt, *b_txt;

	a_txt = ewl_icon_label_get(EWL_ICON(a));
	b_txt = ewl_icon_label_get(EWL_ICON(b));

	return strcmp(a_txt, b_txt);
}

static void
ewl_freebox_cb_add(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
						void *data)
{
	Ewl_Freebox *fb;
	Ewl_Widget *i;
	int t;

	struct 
	{ 
		char *name;
		char *path;
	} files[] = {
		{"Draw", PACKAGE_DATA_DIR "/images/Draw.png"},
		{"End", PACKAGE_DATA_DIR "/images/End.png"},
		{"Card", PACKAGE_DATA_DIR "/images/NewBCard.png"},
		{"Open", PACKAGE_DATA_DIR "/images/Open.png"},
		{"Package", PACKAGE_DATA_DIR "/images/Package.png"},
		{"World", PACKAGE_DATA_DIR "/images/World.png"},
		{NULL, NULL}
	};

	fb = data;

	for (t = 0; files[t].name != NULL; t++)
	{
		long width, height;

		width = (rand() % 30) + 30;
		height = (rand() % 30) + 30;

		i = ewl_icon_new();
		ewl_container_child_append(EWL_CONTAINER(fb), i);
		ewl_icon_label_set(EWL_ICON(i), files[t].name);
		ewl_icon_image_set(EWL_ICON(i), files[t].path, NULL);
		ewl_object_fill_policy_set(EWL_OBJECT(i),
						EWL_FLAG_FILL_FILL);
		ewl_object_minimum_size_set(EWL_OBJECT(i), (int)width,
							(int)height);

		if (fb == EWL_FREEBOX(sort_fb))
		{
			ewl_icon_editable_set(EWL_ICON(i), TRUE);
			ewl_callback_append(i, EWL_CALLBACK_VALUE_CHANGED,
					ewl_freebox_cb_icon_change, fb);
		}
		ewl_widget_show(i);
	}
}

static void 
cb_destroy_win(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(fb_button, EWL_CALLBACK_CLICKED,
				__create_freebox_test_window, NULL);
}

void
__create_freebox_test_window(Ewl_Widget *w, void *ev __UNUSED__,
					void *data __UNUSED__)
{
	Ewl_Widget *win, *box, *hbox, *fb, *pane, *o;

	fb_button = w;

	srand(time(NULL));

	win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(win), "Freebox Test");
	ewl_window_name_set(EWL_WINDOW(win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(win), "EFL Test Application");
	ewl_object_size_request(EWL_OBJECT(win), 400, 600);

	if (w)
	{
		ewl_callback_del(w, EWL_CALLBACK_CLICKED,
					__create_freebox_test_window);
		ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
					cb_destroy_win, NULL);
	}
	else
		ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(win);

	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(win), box);
	ewl_widget_show(box);

	/* the manual box */
	o = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(o), "Manual Placement");
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), hbox);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_FILL);
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_TOP);
	ewl_widget_show(hbox);

	fb = ewl_freebox_new();

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Add items");
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_TOP);
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
				ewl_freebox_cb_add, fb);
	ewl_widget_show(o);

	pane = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), pane);
	ewl_widget_show(pane);

	ewl_freebox_layout_type_set(EWL_FREEBOX(fb),
					EWL_FREEBOX_LAYOUT_MANUAL);
	ewl_container_child_append(EWL_CONTAINER(pane), fb);
	ewl_widget_show(fb);

	/* the auto box */
	o = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(o), "Auto Placement");
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), hbox);
	ewl_widget_show(hbox);

	fb = ewl_freebox_new();

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Add items");
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_TOP);
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
				ewl_freebox_cb_add, fb);
	ewl_widget_show(o);

	pane = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), pane);
	ewl_widget_show(pane);

	ewl_freebox_layout_type_set(EWL_FREEBOX(fb),
					EWL_FREEBOX_LAYOUT_AUTO);
	ewl_container_child_append(EWL_CONTAINER(pane), fb);
	ewl_widget_show(fb);

	/* the comparator box */
	o = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(o), "Comparator Placement (by name)");
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), hbox);
	ewl_widget_show(hbox);

	fb = ewl_freebox_new();

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Add items");
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_TOP);
	ewl_container_child_append(EWL_CONTAINER(hbox), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
				ewl_freebox_cb_add, fb);
	ewl_widget_show(o);

	pane = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), pane);
	ewl_widget_show(pane);

	ewl_freebox_layout_type_set(EWL_FREEBOX(fb),
					EWL_FREEBOX_LAYOUT_COMPARATOR);
	ewl_freebox_comparator_set(EWL_FREEBOX(fb), 
					ewl_freebox_cb_compare);
	ewl_container_child_append(EWL_CONTAINER(pane), fb);
	ewl_widget_show(fb);
	sort_fb = fb;
}

