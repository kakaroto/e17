#include "Ewl_Test.h"
#include "ewl_test_private.h"

static int create_test(Ewl_Container *box);
static void ewl_cb_load(Ewl_Widget *w, void *event, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Histogram";
	test->tip = "Defines an image histogram.";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_SIMPLE;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *hist;

	hist = ewl_histogram_new();
	ewl_histogram_color_set(EWL_HISTOGRAM(hist), 64, 64, 64, 128);
	ewl_widget_name_set(hist, "histogram");
	ewl_container_child_append(EWL_CONTAINER(box), hist);
	ewl_object_fill_policy_set(EWL_OBJECT(hist), EWL_FLAG_FILL_FILL);
	ewl_widget_show(hist);

	ewl_callback_append(EWL_WIDGET(hist), EWL_CALLBACK_REALIZE, ewl_cb_load,
			PACKAGE_DATA_DIR "/images/entrance.png");

	return 1;
}

static void
ewl_cb_load(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Embed *e;
	Ewl_Widget *hist;
	Evas_Object *img;
	int width = 0, height = 0;

	e = ewl_embed_widget_find(w);

	hist = ewl_widget_name_find("histogram");
	img = evas_object_image_add(e->evas);
	evas_object_image_file_set(img, (const char *)data, NULL);
	evas_object_image_size_get(img, &width, &height);

	ewl_histogram_data_set(EWL_HISTOGRAM(hist), evas_object_image_data_get(img, 0), width, height);
}
