#include "ephoto.h"

/*Ephoto Image Manipulation*/
static void flip_image_horizontal(Ewl_Widget *w, void *event, void *data);
static void flip_image_vertical(Ewl_Widget *w, void *event, void *data);
static void rotate_image_left(Ewl_Widget *w, void *event, void *data);
static void rotate_image_right(Ewl_Widget *w, void *event, void *data);

/*Show the edit view*/
void show_edit_view(Ewl_Widget *w, void *event, void *data)
{
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(view_box), edit_vbox);
        ewl_widget_hide(atree);
        ewl_widget_hide(tbar);
        ewl_widget_hide(ilabel);
        ewl_widget_reparent(eimage);
}

/*Flip the image 180 degrees horizontally*/
static void flip_image_horizontal(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;

	evas_object_image_size_get(EWL_IMAGE(eimage)->image, &nw, &nh);
	image_data = flip_horizontal(eimage);
	update_image(eimage, nw, nh, image_data);
	ewl_widget_configure(eimage);
	ewl_widget_configure(eimage->parent);

	return;
}

/*Flip the image 180 degrees vertically*/
static void flip_image_vertical(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;

	evas_object_image_size_get(EWL_IMAGE(eimage)->image, &nw, &nh);
	image_data = flip_vertical(eimage);
	update_image(eimage, nw, nh, image_data);
	ewl_widget_configure(eimage);
	ewl_widget_configure(eimage->parent);

	return;
}

/*Rotate the image 90 degrees to the left*/
static void rotate_image_left(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;

	evas_object_image_size_get(EWL_IMAGE(eimage)->image, &nh, &nw);
	image_data = rotate_left(eimage);
	update_image(eimage, nw, nh, image_data);
	ewl_widget_configure(eimage);
	ewl_widget_configure(eimage->parent);

	return;
}

/*Rotate the image 90 degrees to the right*/
static void rotate_image_right(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;

	evas_object_image_size_get(EWL_IMAGE(eimage)->image, &nh, &nw);
	image_data = rotate_right(eimage);
	update_image(eimage, nw, nh, image_data);
	ewl_widget_configure(eimage);
	ewl_widget_configure(eimage->parent);

	return;
}

