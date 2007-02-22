#include "ephoto.h"

/*Ephoto Image Manipulation*/
static void previous_image(Ewl_Widget *w, void *event, void *data);
static void next_image(Ewl_Widget *w, void *event, void *data);
static void flip_image_horizontal(Ewl_Widget *w, void *event, void *data);
static void flip_image_vertical(Ewl_Widget *w, void *event, void *data);
static void rotate_image_left(Ewl_Widget *w, void *event, void *data);
static void rotate_image_right(Ewl_Widget *w, void *event, void *data);

/*Add the edit view*/
Ewl_Widget *add_edit_view(Ewl_Widget *c)
{
        em->edit_vbox = ewl_vbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(em->edit_vbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(c), em->edit_vbox);
        ewl_widget_show(em->edit_vbox);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), em->edit_vbox, "Edit");

        em->eimage = add_image(em->edit_vbox, NULL, 0, NULL, NULL);
        ewl_object_alignment_set(EWL_OBJECT(em->eimage), EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(em->eimage), EWL_FLAG_FILL_SHRINK);

	return em->edit_vbox;
}

/*Show the edit view*/
void show_edit_view(Ewl_Widget *w, void *event, void *data)
{
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->view_box), em->edit_vbox);
	ecore_dlist_goto_first(em->images);
	ewl_widget_enable(em->edit_tools);
        ewl_widget_hide(em->atree);
        ewl_widget_hide(em->tbar);
        ewl_widget_hide(em->ilabel);
}

/*Add edit tools to container c*/
void add_edit_tools(Ewl_Widget *c)
{
	Ewl_Widget *image, *sep;

	image = add_image(c, PACKAGE_DATA_DIR "/images/undo.png", 0, rotate_image_left, NULL);
	ewl_image_constrain_set(EWL_IMAGE(image), 30);
	ewl_attach_tooltip_text_set(image, "Rotate Image Left");

        image = add_image(c, PACKAGE_DATA_DIR "/images/redo.png", 0, rotate_image_right, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);
	ewl_attach_tooltip_text_set(image, "Rotate Image Right");

        image = add_image(c, PACKAGE_DATA_DIR "/images/go-next.png", 0, flip_image_horizontal, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);
	ewl_attach_tooltip_text_set(image, "Flip Image Horizontally");

        image = add_image(c, PACKAGE_DATA_DIR "/images/go-down.png", 0, flip_image_vertical, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);
	ewl_attach_tooltip_text_set(image, "Flip Image Vertically");

	sep = ewl_vseparator_new();
	ewl_container_child_append(EWL_CONTAINER(c), sep);
	ewl_widget_show(sep);

        image = add_image(c, PACKAGE_DATA_DIR "/images/media-seek-backward.png", 0, previous_image, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_RIGHT);
	ewl_attach_tooltip_text_set(image, "Previous Image");

        image = add_image(c, PACKAGE_DATA_DIR "/images/media-seek-forward.png", 0, next_image, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_RIGHT);
	ewl_attach_tooltip_text_set(image, "Next Image");

	return;
}

/*Go to the previous image*/
static void previous_image(Ewl_Widget *w, void *event, void *data)
{
        const char *image;

        ecore_dlist_previous(em->images);
        image = ecore_dlist_current(em->images);
	if(!image)
	{
		ecore_dlist_goto_last(em->images);
		image = ecore_dlist_current(em->images);
	}
        ewl_image_file_path_set(EWL_IMAGE(em->eimage), image);
        ewl_widget_configure(em->eimage->parent);

        return;
}


/*Go to the next image*/
static void next_image(Ewl_Widget *w, void *event, void *data)
{
	const char *image;

	ecore_dlist_next(em->images);
	image = ecore_dlist_current(em->images);
	if(!image)
	{
		ecore_dlist_goto_first(em->images);
		image = ecore_dlist_current(em->images);
	}
	ewl_image_file_path_set(EWL_IMAGE(em->eimage), image);
	ewl_widget_configure(em->eimage->parent);

	return;
}



/*Flip the image 180 degrees horizontally*/
static void flip_image_horizontal(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Image *image;

	evas_object_image_size_get(EWL_IMAGE(em->eimage)->image, &nw, &nh);
	image_data = flip_horizontal(em->eimage);
	update_image(em->eimage, nw, nh, image_data);
	ewl_image_size_set(EWL_IMAGE(em->eimage), nh, nw);
	image = (Ewl_Image *)em->eimage;
        image->ow = nw;
        image->oh = nh;
	ewl_object_preferred_inner_size_set(EWL_OBJECT(em->eimage), nw, nh);
	ewl_widget_configure(em->eimage);
	ewl_widget_configure(em->eimage->parent);

	return;
}

/*Flip the image 180 degrees vertically*/
static void flip_image_vertical(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Image *image;

	evas_object_image_size_get(EWL_IMAGE(em->eimage)->image, &nw, &nh);
	image_data = flip_vertical(em->eimage);
	update_image(em->eimage, nw, nh, image_data);
	ewl_image_size_set(EWL_IMAGE(em->eimage), nh, nw);
	image = (Ewl_Image *)em->eimage;
        image->ow = nw;
        image->oh = nh;
	ewl_object_preferred_inner_size_set(EWL_OBJECT(em->eimage), nh, nw);
	ewl_widget_configure(em->eimage);
	ewl_widget_configure(em->eimage->parent);

	return;
}

/*Rotate the image 90 degrees to the left*/
static void rotate_image_left(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Image *image;

	evas_object_image_size_get(EWL_IMAGE(em->eimage)->image, &nh, &nw);
	image_data = rotate_left(em->eimage);
	update_image(em->eimage, nw, nh, image_data);
	ewl_image_size_set(EWL_IMAGE(em->eimage), nh, nw);
	image = (Ewl_Image *)em->eimage;
        image->ow = nw;
        image->oh = nh;
        ewl_object_preferred_inner_size_set(EWL_OBJECT(em->eimage), nw, nh);
	ewl_widget_configure(em->eimage);
	ewl_widget_configure(em->eimage->parent);

	return;
}

/*Rotate the image 90 degrees to the right*/
static void rotate_image_right(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;
	Ewl_Image *image;

	evas_object_image_size_get(EWL_IMAGE(em->eimage)->image, &nh, &nw);
	image_data = rotate_right(em->eimage);
	update_image(em->eimage, nw, nh, image_data);
	image = (Ewl_Image *)em->eimage;
        image->ow = nw;
        image->oh = nh;
        ewl_object_preferred_inner_size_set(EWL_OBJECT(em->eimage), nw, nh);
	ewl_widget_configure(em->eimage);
	ewl_widget_configure(em->eimage->parent);

	return;
}

