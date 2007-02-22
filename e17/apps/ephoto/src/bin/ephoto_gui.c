#include "ephoto.h"

/*Create and Add a Button to the Container c*/
Ewl_Widget *add_button(Ewl_Widget *c, const char *txt, const char *img, void *cb, void *data)
{
	Ewl_Widget *button;

	button = ewl_button_new();
	if (img)
	{
		ewl_button_image_set(EWL_BUTTON(button), img, NULL);
	}
	if (txt)
	{
		ewl_button_label_set(EWL_BUTTON(button), _(txt));
	}
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(c), button);
	if (cb)
	{
		ewl_callback_append(button, EWL_CALLBACK_CLICKED, cb, data);
	}
	ewl_widget_show(button);

	return button;
}

/*Create and Add an Image to the Container c*/
Ewl_Widget *add_image(Ewl_Widget *c, const char *img, int thumbnail, void *cb, void *data)
{
	Ewl_Widget *image;
	const char *thumb;
	int w, h, pid;

	if(!thumbnail)
	{
		image = ewl_image_new();
		if (img)
		{
			ewl_image_file_path_set(EWL_IMAGE(image), img);
		}

	}
	else
	{
		image_pixels_int_get(img, &w, &h);
		if(w > 75 || h > 75)
		{
			image = ewl_image_thumbnail_new();
			ewl_image_file_path_set(EWL_IMAGE(image), PACKAGE_DATA_DIR "/images/image.png");
			if (img)
			{
				ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(image), img);
			}
		}
		else
		{
			image = ewl_image_new();
			if (img)
			{
				ewl_image_file_path_set(EWL_IMAGE(image), img);
			}
		}
		ewl_image_constrain_set(EWL_IMAGE(image), 48);
	}
	ewl_image_proportional_set(EWL_IMAGE(image), TRUE);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append(EWL_CONTAINER(c), image);
	if (cb)
	{
		ewl_callback_append(image, EWL_CALLBACK_CLICKED, cb, data);
	}
	ewl_widget_show(image);

	return image;
}

/*Add a label to the container c, with the text lbl, and whether you want it blue*/
Ewl_Widget *add_label(Ewl_Widget *c, const char *lbl, int blue)
{
	Ewl_Widget *label;

	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), lbl);
	ewl_object_alignment_set(EWL_OBJECT(label), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(label), EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(c), label);
	if(blue)
	{
		ewl_widget_state_set(label, "blue", EWL_STATE_PERSISTENT);
	}
	ewl_widget_show(label);

	return label;
}

/*Add a shadow to the container c*/
Ewl_Widget *add_shadow(Ewl_Widget *c)
{
	Ewl_Widget *shadow;

	shadow = ewl_shadow_new();
	ewl_container_child_append(EWL_CONTAINER(c), shadow);
	ewl_widget_show(shadow);

	return shadow;
}

