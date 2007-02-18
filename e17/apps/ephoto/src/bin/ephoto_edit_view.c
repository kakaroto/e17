#include "ephoto.h"

/*Ephoto Image Manipulation*/
static void previous_image(Ewl_Widget *w, void *event, void *data);
static void next_image(Ewl_Widget *w, void *event, void *data);
static void flip_image_horizontal(Ewl_Widget *w, void *event, void *data);
static void flip_image_vertical(Ewl_Widget *w, void *event, void *data);
static void rotate_image_left(Ewl_Widget *w, void *event, void *data);
static void rotate_image_right(Ewl_Widget *w, void *event, void *data);

/*Show the edit view*/
void show_edit_view(Ewl_Widget *w, void *event, void *data)
{
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(view_box), edit_vbox);
	ecore_dlist_goto_first(images);
	ewl_widget_show(edit_tools);
        ewl_widget_hide(atree);
        ewl_widget_hide(tbar);
        ewl_widget_hide(ilabel);
        ewl_widget_reparent(eimage);
}

/*Add edit tools to container c*/
void add_edit_tools(Ewl_Widget *c)
{
	Ewl_Widget *image, *sep;

	image = add_image(c, PACKAGE_DATA_DIR "/images/undo.png", 0, rotate_image_left, NULL);
	ewl_image_constrain_set(EWL_IMAGE(image), 30);

        image = add_image(c, PACKAGE_DATA_DIR "/images/redo.png", 0, rotate_image_right, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

        image = add_image(c, PACKAGE_DATA_DIR "/images/go-next.png", 0, flip_image_horizontal, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

        image = add_image(c, PACKAGE_DATA_DIR "/images/go-down.png", 0, flip_image_vertical, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

	sep = ewl_vseparator_new();
	ewl_container_child_append(EWL_CONTAINER(c), sep);
	ewl_widget_show(sep);

        image = add_image(c, PACKAGE_DATA_DIR "/images/media-seek-backward.png", 0, previous_image, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_RIGHT);

        image = add_image(c, PACKAGE_DATA_DIR "/images/media-seek-forward.png", 0, next_image, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_RIGHT);

	return;
}

/*Go to the previous image*/
static void previous_image(Ewl_Widget *w, void *event, void *data)
{
        char *image;

        ecore_dlist_previous(images);
        image = ecore_dlist_current(images);
	if(!image)
	{
		ecore_dlist_goto_last(images);
		image = ecore_dlist_current(images);
	}
        ewl_image_file_path_set(EWL_IMAGE(eimage), image);
        ewl_widget_configure(eimage->parent);

        return;
}


/*Go to the next image*/
static void next_image(Ewl_Widget *w, void *event, void *data)
{
	char *image;

	ecore_dlist_next(images);
	image = ecore_dlist_current(images);
	if(!image)
	{
		ecore_dlist_goto_first(images);
		image = ecore_dlist_current(images);
	}
	ewl_image_file_path_set(EWL_IMAGE(eimage), image);
	ewl_widget_configure(eimage->parent);

	return;
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

