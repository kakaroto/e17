#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>

static int create_test(Ewl_Container *box);

void 
test_info(Ewl_Test *test)
{
	test->name = "Icon";
	test->tip = "";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_ADVANCED;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *o, *o2;

	o = ewl_icon_new();
	ewl_icon_image_set(EWL_ICON(o),
				PACKAGE_DATA_DIR "/images/Draw.png", NULL);
	ewl_icon_label_set(EWL_ICON(o), "Draw (Editable)");
	ewl_icon_editable_set(EWL_ICON(o), TRUE);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	o = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	o2 = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(o2), "This icon has\nextended data\n set "
					"on it.\n\n That data is just \n"
					"text, but could\nbe any widget.");
	ewl_widget_show(o2);

	o = ewl_icon_new();
	ewl_icon_image_set(EWL_ICON(o),
				PACKAGE_DATA_DIR "/images/World.png", NULL);
	ewl_icon_extended_data_set(EWL_ICON(o), o2);
	ewl_icon_label_set(EWL_ICON(o), "World");
	ewl_icon_type_set(EWL_ICON(o), EWL_ICON_TYPE_LONG);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	o = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	o = ewl_icon_new();
	ewl_box_orientation_set(EWL_BOX(o), EWL_ORIENTATION_HORIZONTAL);
	ewl_icon_image_set(EWL_ICON(o),
				PACKAGE_DATA_DIR "/images/Draw.png", NULL);
	ewl_icon_label_set(EWL_ICON(o), "Draw");
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);
	
	o = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	o = ewl_icon_new();
	ewl_icon_image_set(EWL_ICON(o),
				PACKAGE_DATA_DIR "/images/Draw.png", NULL);
	ewl_icon_label_set(EWL_ICON(o), "This is a long title that is compressed.");
	ewl_icon_label_compressed_set(EWL_ICON(o), TRUE);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	o = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	o = ewl_icon_new();
	ewl_icon_image_set(EWL_ICON(o),
				PACKAGE_DATA_DIR "/images/Draw.png", NULL);
	ewl_icon_label_set(EWL_ICON(o), "This is a long title that is compressed.");
	ewl_icon_label_compressed_set(EWL_ICON(o), TRUE);
	ewl_icon_editable_set(EWL_ICON(o), TRUE);
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_widget_show(o);

	return 1;
}

