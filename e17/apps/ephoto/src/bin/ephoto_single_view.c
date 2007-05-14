#include "ephoto.h"

/*Ephoto Image Changing*/
static void previous_image(Ewl_Widget *w, void *event, void *data);
static void next_image(Ewl_Widget *w, void *event, void *data);

/*Add the single view*/
Ewl_Widget *add_single_view(Ewl_Widget *c)
{
	Ewl_Widget *hbox, *ibox, *button;

        em->single_vbox = add_box(c, EWL_ORIENTATION_VERTICAL, 5);
        ewl_object_fill_policy_set(EWL_OBJECT(em->single_vbox), EWL_FLAG_FILL_ALL);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), em->single_vbox, "Single");

	ibox = ewl_cell_new();
	ewl_object_fill_policy_set(EWL_OBJECT(ibox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(em->single_vbox), ibox);
	ewl_widget_show(ibox);

        em->simage = add_image(ibox, NULL, 0, NULL, NULL);
        ewl_object_alignment_set(EWL_OBJECT(em->simage), EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(em->simage), EWL_FLAG_FILL_SHRINK);

	hbox = add_box(em->single_vbox, EWL_ORIENTATION_HORIZONTAL, 2);
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);

	button = add_button(hbox, NULL, PACKAGE_DATA_DIR "/images/media-seek-backward.png", previous_image, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);
        ewl_attach_tooltip_text_set(button, _("Previous Image"));

        button = add_button(hbox, NULL, PACKAGE_DATA_DIR "/images/media-seek-forward.png", next_image, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);
        ewl_attach_tooltip_text_set(button, _("Next Image"));

	return em->single_vbox;
}

/*Show the single view*/
void show_single_view(Ewl_Widget *w, void *event, void *data)
{
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->main_nb), em->main_vbox);
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->view_box), em->single_vbox);
	ewl_image_file_path_set(EWL_IMAGE(em->simage), ecore_dlist_current(em->images));

	return;
}

/*Go to the previous image*/
static void previous_image(Ewl_Widget *w, void *event, void *data)
{
        char *image;

        ecore_dlist_previous(em->images);
        image = ecore_dlist_current(em->images);
	if(!image)
	{
		ecore_dlist_goto_last(em->images);
		image = ecore_dlist_current(em->images);
	}
        ewl_image_file_path_set(EWL_IMAGE(em->simage), image);
        ewl_widget_configure(em->simage->parent);

        return;
}


/*Go to the next image*/
static void next_image(Ewl_Widget *w, void *event, void *data)
{
	char *image;

	ecore_dlist_next(em->images);
	image = ecore_dlist_current(em->images);
	if(!image)
	{
		ecore_dlist_goto_first(em->images);
		image = ecore_dlist_current(em->images);
	}
	ewl_image_file_path_set(EWL_IMAGE(em->simage), image);
	ewl_widget_configure(em->simage->parent);

	return;
}

