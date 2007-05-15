#include "ephoto.h"

/*Create and Add a box to container c, with orientation and spacing*/
Ewl_Widget *add_box(Ewl_Widget *c, Ewl_Orientation orientation, int spacing)
{
	Ewl_Widget *box;

	box = ewl_box_new();
	if(orientation) ewl_box_orientation_set(EWL_BOX(box), orientation);
	if(spacing) ewl_box_spacing_set(EWL_BOX(box), spacing);
	if(c) ewl_container_child_append(EWL_CONTAINER(c), box);
	ewl_widget_show(box);

	return box;
}

/*Create and Add a Button to the Container c*/
Ewl_Widget *add_button(Ewl_Widget *c, const char *txt, const char *img, void *cb, void *data)
{
	Ewl_Widget *button;

	button = ewl_button_new();
	if(img) ewl_button_image_set(EWL_BUTTON(button), img, NULL);
	if(txt) ewl_button_label_set(EWL_BUTTON(button), _(txt));
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	if(c) ewl_container_child_append(EWL_CONTAINER(c), button);
	if(cb)	ewl_callback_append(button, EWL_CALLBACK_CLICKED, cb, data);
	ewl_widget_show(button);

	return button;
}

/*Create and Add an Entry to Container c With Text txt and the Callback cb*/
Ewl_Widget *add_entry(Ewl_Widget *c, const char *txt, void *cb, void *data)
{
	Ewl_Widget *entry;

	entry = ewl_entry_new();
	if(txt) ewl_text_text_set(EWL_TEXT(entry), _(txt));
	if(c) ewl_container_child_append(EWL_CONTAINER(c), entry);
	if(cb) ewl_callback_append(entry, EWL_CALLBACK_VALUE_CHANGED, cb, data);
	ewl_widget_show(entry);

	return entry;
}

/*Create and add an icon to the container c*/
Ewl_Widget *add_icon(Ewl_Widget *c, const char *lbl, const char *img, int thumbnail, void *cb, void *data)
{
	Ewl_Widget *icon;
	
	icon = ewl_icon_new();
	if(!thumbnail) ewl_icon_thumbnailing_set(EWL_ICON(icon), FALSE);
	if (lbl) ewl_icon_label_set(EWL_ICON(icon), _(lbl));
	if (img) ewl_icon_image_set(EWL_ICON(icon), img, NULL);
	if (c) ewl_container_child_append(EWL_CONTAINER(c), icon);
	if (cb) ewl_callback_append(icon, EWL_CALLBACK_CLICKED, cb, data);
	ewl_widget_show(icon);

	return icon;
}

/*Create and Add an Image to the Container c*/
Ewl_Widget *add_image(Ewl_Widget *c, const char *img, int thumbnail, void *cb, void *data)
{
	Ewl_Widget *image;
	int w, h;

	if(!thumbnail)
	{
		image = ewl_image_new();
		if(img) ewl_image_file_path_set(EWL_IMAGE(image), img);
	}
	else
	{
		image_pixels_int_get(img, &w, &h);
		if(w > 75 || h > 75)
		{
			image = ewl_image_thumbnail_new();
			ewl_image_file_path_set(EWL_IMAGE(image), 
					PACKAGE_DATA_DIR "/images/image.png");
			if(img) ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(image), img);
		}
		else
		{
			image = ewl_image_new();
			if(img) ewl_image_file_path_set(EWL_IMAGE(image), img);
		}
		ewl_image_constrain_set(EWL_IMAGE(image), 48);
	}
	ewl_image_proportional_set(EWL_IMAGE(image), TRUE);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_LEFT);
	if(c) ewl_container_child_append(EWL_CONTAINER(c), image);
	if(cb) ewl_callback_append(image, EWL_CALLBACK_CLICKED, cb, data);
	ewl_widget_show(image);

	return image;
}

/*Create and Add a Label to the Container c, With the Text lbl*/
Ewl_Widget *add_label(Ewl_Widget *c, const char *lbl)
{
	Ewl_Widget *label;

	label = ewl_label_new();
	if(lbl) ewl_label_text_set(EWL_LABEL(label), _(lbl));
	ewl_object_alignment_set(EWL_OBJECT(label), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(label), EWL_FLAG_FILL_SHRINK);
	if(c) ewl_container_child_append(EWL_CONTAINER(c), label);
	ewl_widget_show(label);

	return label;
}

/*Create and Add a Menubar to the Container c*/
Ewl_Widget *add_menubar(Ewl_Widget *c)
{
	Ewl_Widget *mb;

	mb = ewl_hmenubar_new();
	ewl_object_fill_policy_set(EWL_OBJECT(mb), EWL_FLAG_FILL_HFILL);
	if(c) ewl_container_child_append(EWL_CONTAINER(c), mb);
	ewl_widget_show(mb);

	return mb;
}

/*Create and Add a Menu to the Container c*/
Ewl_Widget *add_menu(Ewl_Widget *c, const char *lbl)
{
	Ewl_Widget *menu;
	
	menu = ewl_menu_new();
	if(lbl) ewl_button_label_set(EWL_BUTTON(menu), _(lbl));
	ewl_object_fill_policy_set(EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
	if(c) ewl_container_child_append(EWL_CONTAINER(c), menu);
	ewl_widget_show(menu);

	return menu;
}

/*Create and Add a Menu Item to the Container c*/
Ewl_Widget *add_menu_item(Ewl_Widget *c, const char *lbl, const char *img, void *cb, void *data)
{
	Ewl_Widget *mi;

	mi = ewl_menu_item_new();
	if(img) ewl_button_image_set(EWL_BUTTON(mi), img, NULL);
	if(lbl) ewl_button_label_set(EWL_BUTTON(mi), _(lbl));
	ewl_object_alignment_set(EWL_OBJECT(mi), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(mi), EWL_FLAG_FILL_ALL);
	if(c) ewl_container_child_append(EWL_CONTAINER(c), mi);
	if(cb) ewl_callback_append(mi, EWL_CALLBACK_CLICKED, cb, data);
	ewl_widget_show(mi);

	return mi;
}

/*Create and Add a Shadow to the Container c*/
Ewl_Widget *add_shadow(Ewl_Widget *c)
{
        Ewl_Widget *shadow;

        shadow = ewl_shadow_new();
        if(c) ewl_container_child_append(EWL_CONTAINER(c), shadow);
        ewl_widget_show(shadow);

        return shadow;
}

/*Create and Add a Text Widget to the Container c*/
Ewl_Widget *add_text(Ewl_Widget *c, const char *txt)
{
	Ewl_Widget *text;
	
	text = ewl_text_new();
	if(txt) ewl_text_text_set(EWL_TEXT(text), _(txt));
        ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_SHRINK);
        if(c) ewl_container_child_append(EWL_CONTAINER(c), text);
        ewl_widget_show(text);

        return text;
}

/*Create and Add a Window*/
Ewl_Widget *add_window(const char *name, int width, int height, void *cb, void *data)
{
	Ewl_Widget *win;

        win = ewl_window_new();
	if(name)
	{
        	ewl_window_title_set(EWL_WINDOW(win), _(name));
        	ewl_window_name_set(EWL_WINDOW(win), _(name));
        }
	if(width && height) ewl_object_size_request(EWL_OBJECT(win), width, 
								     height);
        if(cb) ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, cb, data);
        ewl_widget_show(win);

	return win;
}

