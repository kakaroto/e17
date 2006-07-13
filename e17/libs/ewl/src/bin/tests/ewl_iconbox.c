#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>

static int create_test(Ewl_Container *box);
static void add_icons_cb(Ewl_Widget *w, void *ev, void *data); 
static void icon_click_cb(Ewl_Widget *w, void *ev, void *data); 

void 
test_info(Ewl_Test *test)
{
	test->name = "Iconbox";
	test->tip = "Provides a widget for displaying icons\n"
			"in an arranged grid, or freeform,\n"
	                "with callback and label editing facilities.";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget * button, *ib;

	button = ewl_button_new();
	ewl_container_child_append(box, button);
	ewl_button_label_set(EWL_BUTTON(button), "Add Icons");
	ewl_object_maximum_size_set(EWL_OBJECT(button), 50, 50);
	ewl_callback_append(button, EWL_CALLBACK_MOUSE_DOWN,
					add_icons_cb, NULL);
	ewl_widget_show(button);

	ib = ewl_iconbox_new();
	ewl_widget_name_set(ib, "ib");
	ewl_iconbox_editable_set(EWL_ICONBOX(ib), TRUE);
	ewl_container_child_append(EWL_CONTAINER(box), ib);
	ewl_widget_show(ib);

	return 1;
}

static void
add_icons_cb(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
					void *data __UNUSED__) 
{
	Ewl_Widget *ib;
	Ewl_Iconbox_Icon* icon;

	ib = ewl_widget_name_find("ib");

	icon = ewl_iconbox_icon_add(EWL_ICONBOX(ib), "Draw", 
				PACKAGE_DATA_DIR "/images/Draw.png");
	ewl_callback_prepend(EWL_WIDGET(icon), EWL_CALLBACK_CLICKED, 
					icon_click_cb, NULL);

	ewl_iconbox_icon_arrange(EWL_ICONBOX(ib));

	ewl_iconbox_icon_add(EWL_ICONBOX(ib),"End", 
				PACKAGE_DATA_DIR "/images/End.png");
	ewl_iconbox_icon_add(EWL_ICONBOX(ib),"Card", 
				PACKAGE_DATA_DIR "/images/NewBCard.png");
	ewl_iconbox_icon_add(EWL_ICONBOX(ib),"Open", 
				PACKAGE_DATA_DIR "/images/Open.png");
	ewl_iconbox_icon_add(EWL_ICONBOX(ib),"Package", 
				PACKAGE_DATA_DIR "/images/Package.png");
	ewl_iconbox_icon_add(EWL_ICONBOX(ib),"World", 
				PACKAGE_DATA_DIR "/images/World.png");
}

static void 
icon_click_cb(Ewl_Widget *w __UNUSED__, void *ev, void *data __UNUSED__) 
{
	Ewl_Event_Mouse_Down *e;
	
	e = ev;
	if (e->clicks > 1)
		printf("Icon clicked!\n");
}

