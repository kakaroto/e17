#include "Ewl_Test.h"
#include <stdlib.h>
#include <string.h>

static int create_test(Ewl_Container *box);
static void ewl_freebox_cb_icon_change(Ewl_Widget *w, void *ev, void *data);
static int ewl_freebox_cb_compare(Ewl_Widget *a, Ewl_Widget *b);
static void ewl_freebox_cb_add(Ewl_Widget *w, void *ev, void *data);

static Ewl_Widget *sort_fb;

void 
test_info(Ewl_Test *test)
{
	test->name = "Freebox";
	test->tip = "The freebox is used for a more free layout system\n"
					"then the other containers",
	test->filename = "ewl_freebox.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *hbox, *fb, *pane, *o;

	srand(time(NULL));

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

	return 1;
}

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


