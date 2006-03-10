#include "Ewl_Test.h"
#include <stdio.h>
#include <string.h>

static int create_test(Ewl_Container *win);
static void combo_value_changed(Ewl_Widget *w, void *ev, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Combo";
	test->tip = "Defines a combo box used internally.\n"
		"The contents of the box are not drawn\n"
		"outside of the evas.";
	test->filename = "ewl_combo.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_MISC;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *combo1, *item;

	combo1 = ewl_combo_new("test menu");
	ewl_container_child_append(EWL_CONTAINER(box), combo1);
	ewl_callback_append(combo1, EWL_CALLBACK_VALUE_CHANGED,
					combo_value_changed, NULL);
	ewl_widget_show(combo1);

	item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item), "dia");
	ewl_button_image_set(EWL_BUTTON(item),
				PACKAGE_DATA_DIR "/images/Draw.png", NULL);
	ewl_container_child_append(EWL_CONTAINER(combo1), item);
	ewl_widget_show(item);

	item = EWL_WIDGET(ewl_separator_new());
	ewl_container_child_append(EWL_CONTAINER(combo1), item);
	ewl_widget_show(item);

	item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item), "gimp");
	ewl_button_image_set(EWL_BUTTON(item),
				PACKAGE_DATA_DIR "/images/World.png", NULL);
	ewl_container_child_append(EWL_CONTAINER(combo1), item);
	ewl_widget_show(item);

	item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item), "button");
	ewl_widget_data_set(item, "dummy", "data");
	ewl_container_child_append(EWL_CONTAINER(combo1), item);
	ewl_widget_show(item);

	return 1;
}

static void
combo_value_changed(Ewl_Widget *w __UNUSED__, void *ev,
					void *data __UNUSED__)
{
	Ewl_Widget *entry;
	const char *text;

	entry = EWL_WIDGET(ev);
	text = ewl_button_label_get(EWL_BUTTON(entry));

	printf("value changed to %s\n", text);
	if (!strcmp(text, "button"))
	{
		char *t2;
		t2 = ewl_widget_data_get(entry, "dummy");
		printf("with data: %s\n", t2);
	}
}

