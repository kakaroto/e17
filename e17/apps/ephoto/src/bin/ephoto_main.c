#include "ephoto.h"

/*Ewl Callbacks*/
static void destroy(Ewl_Widget *w, void *event, void *data);
static void populate(Ewl_Widget *w, void *event, void *data);
static void window_fullscreen(Ewl_Widget *w, void *event, void *data);

/*Ephoto Create Callbacks*/
static Ewl_Widget *add_atree(Ewl_Widget *c);

/*Ephoto MVC Callbacks*/
static Ewl_Widget *album_view_new(void);
static void album_view_assign(Ewl_Widget *w, void *data);
static Ewl_Widget *album_header_fetch(void *data, int column);
static void *album_data_fetch(void *data, unsigned int row, unsigned int column);
static int album_data_count(void *data);

/*Ephoto Global Variables*/
Ephoto_Main *em;

/*Destroy the Main Window*/
static void destroy(Ewl_Widget *w, void *event, void *data)
{
	ephoto_db_close(em->db);
        ewl_widget_destroy(em->win);
	ecore_list_destroy(em->albums);
	ecore_list_destroy(em->images);
	free(em->views);
	free(em);
	ewl_main_quit();
	return;
}

static void window_fullscreen(Ewl_Widget *w, void *event, void *data)
{
	int fullscreen;

	fullscreen = ewl_window_fullscreen_get(EWL_WINDOW(em->win));
	if(!fullscreen)
	{
		ewl_window_fullscreen_set(EWL_WINDOW(em->win), 1);
	}
	else
	{
		ewl_window_fullscreen_set(EWL_WINDOW(em->win), 0);
	}
	return;
}

/*Create the Main Ephoto Window*/
void create_main_gui(void)
{
	Ewl_Widget *vbox, *entry, *hbox;
	Ewl_Widget *rvbox, *hsep, *vsep, *button;

	em = NULL;
	em = calloc(1, sizeof(Ephoto_Main));
	em->albums = ecore_list_new();
	em->images = ecore_dlist_new();

	em->win = ewl_window_new();
        ewl_window_title_set(EWL_WINDOW(em->win), "Ephoto!");
        ewl_window_name_set(EWL_WINDOW(em->win), "Ephoto!");
        ewl_object_size_request(EWL_OBJECT(em->win), 777, 542);
        ewl_callback_append(em->win, EWL_CALLBACK_DELETE_WINDOW, destroy, NULL);
	ewl_widget_show(em->win);

	vbox = ewl_vbox_new();
	ewl_box_spacing_set(EWL_BOX(vbox), 5);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(em->win), vbox);
	ewl_widget_show(vbox);

	em->tbar = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(em->tbar), EWL_FLAG_FILL_SHRINK);
	ewl_object_alignment_set(EWL_OBJECT(em->tbar), EWL_FLAG_ALIGN_RIGHT);
	ewl_container_child_append(EWL_CONTAINER(vbox), em->tbar);
	ewl_widget_show(em->tbar);

	entry = ewl_entry_new();
	ewl_object_minimum_size_set(EWL_OBJECT(entry), 157, 22);
	ewl_object_maximum_size_set(EWL_OBJECT(entry), 157, 22);
	ewl_container_child_append(EWL_CONTAINER(em->tbar), entry);
	ewl_widget_show(entry);

	add_button(em->tbar, "Find", NULL, NULL, NULL);

	hbox = ewl_hbox_new();
	ewl_box_spacing_set(EWL_BOX(hbox), 2);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	em->atree = add_atree(hbox);
	ewl_object_maximum_w_set(EWL_OBJECT(em->atree), 172);

	rvbox = ewl_vbox_new();
	ewl_box_spacing_set(EWL_BOX(rvbox), 1);
	ewl_object_fill_policy_set(EWL_OBJECT(rvbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(hbox), rvbox);
	ewl_widget_show(rvbox);

	em->view_box = ewl_notebook_new();
	ewl_notebook_tabbar_visible_set(EWL_NOTEBOOK(em->view_box), 0);
	ewl_object_fill_policy_set(EWL_OBJECT(em->view_box), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(rvbox), em->view_box);
	ewl_widget_show(em->view_box);

	add_normal_view(em->view_box);

	add_edit_view(em->view_box);

	add_list_view(em->view_box);	
	
	hsep = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hsep);
	ewl_widget_show(hsep);

	em->toolbar = ewl_htoolbar_new();
	ewl_box_spacing_set(EWL_BOX(em->toolbar), 5);
	ewl_object_minimum_h_set(EWL_OBJECT(em->toolbar), 30);
	ewl_object_alignment_set(EWL_OBJECT(em->toolbar), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(em->toolbar), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(vbox), em->toolbar);
	ewl_widget_show(em->toolbar);

        button = add_button(em->toolbar, NULL, PACKAGE_DATA_DIR "/images/normal_view.png", show_normal_view, NULL);
        ewl_image_size_set(EWL_IMAGE(EWL_BUTTON(button)->image_object), 30, 30);
	ewl_attach_tooltip_text_set(button, "Normal View");

        button = add_button(em->toolbar, NULL, PACKAGE_DATA_DIR "/images/list_view.png", show_list_view, NULL);
        ewl_image_size_set(EWL_IMAGE(EWL_BUTTON(button)->image_object), 30, 30);
	ewl_attach_tooltip_text_set(button, "List View");      

	button = add_button(em->toolbar, NULL, PACKAGE_DATA_DIR "/images/edit_view.png", show_edit_view, NULL);
        ewl_image_size_set(EWL_IMAGE(EWL_BUTTON(button)->image_object), 30, 30);
	ewl_attach_tooltip_text_set(button, "Single View");

	vsep = ewl_vseparator_new();
	ewl_container_child_append(EWL_CONTAINER(em->toolbar), vsep);
	ewl_widget_show(vsep);

        button = add_button(em->toolbar, NULL, PACKAGE_DATA_DIR "/images/get_exif.png", NULL, NULL);
        ewl_image_size_set(EWL_IMAGE(EWL_BUTTON(button)->image_object), 30, 30);
	ewl_attach_tooltip_text_set(button, "You do not have libexif 0.6.13");
#ifdef BUILD_EXIF_SUPPORT
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, display_exif_dialog, NULL);
	ewl_attach_tooltip_text_set(button, "View Exif Data");
#endif

	button = add_button(em->toolbar, NULL, PACKAGE_DATA_DIR "/images/stock_fullscreen.png", window_fullscreen, NULL);
        ewl_image_size_set(EWL_IMAGE(EWL_BUTTON(button)->image_object), 30, 30);
	ewl_attach_tooltip_text_set(button, "Toggle Fullscreen");

        button = add_button(em->toolbar, NULL, PACKAGE_DATA_DIR "/images/x-office-presentation.png", start_slideshow, NULL);
	ewl_image_size_set(EWL_IMAGE(EWL_BUTTON(button)->image_object), 30, 30);
	ewl_attach_tooltip_text_set(button, "Start a Slideshow");

	em->albums = ecore_list_new();
	em->db = ephoto_db_init();
	em->albums = ephoto_db_list_albums(em->db);

	ewl_mvc_data_set(EWL_MVC(em->atree), em->albums);
	
	populate(NULL, NULL, "Complete Library");

	ewl_main();

	return;
}

/*Update the Image List*/
static void populate(Ewl_Widget *w, void *event, void *data)
{
	const char *album;
	char *imagef;
	Ewl_Widget *thumb;

	if (w)
	{
		album = (char *)ewl_widget_name_get(w);
		if (em->currenta) 
		{
			ewl_widget_state_set(em->currenta, "unselected", EWL_STATE_PERSISTENT);
			ewl_widget_state_set(EWL_ICON(em->currenta)->label, "default", EWL_STATE_PERSISTENT);
		}
		em->currenta = w;
		ewl_widget_state_set(em->currenta, "selected", EWL_STATE_PERSISTENT);
		ewl_widget_state_set(EWL_ICON(em->currenta)->label, "blue", EWL_STATE_PERSISTENT);
	}
	else
	{
		album = data;
	}

	if (!ecore_list_is_empty(em->images))
	{
		ecore_dlist_destroy(em->images);
	}

	em->images = ecore_dlist_new();
	em->images = ephoto_db_list_images(em->db, album);

	ecore_dlist_goto_first(em->images);
	ewl_container_reset(EWL_CONTAINER(em->fbox));
        while (ecore_dlist_current(em->images))
        {
                imagef = ecore_dlist_current(em->images);

                thumb = add_image(em->fbox, imagef, 1, set_info, NULL);
		ewl_image_constrain_set(EWL_IMAGE(thumb), 64);
		ewl_object_alignment_set(EWL_OBJECT(thumb), EWL_FLAG_ALIGN_CENTER);
		ewl_widget_name_set(thumb, imagef);

		ecore_dlist_next(em->images);
        }

	ecore_dlist_goto_first(em->images);

        ewl_mvc_data_set(EWL_MVC(em->ltree), em->images);
	ewl_mvc_dirty_set(EWL_MVC(em->ltree), 1);
	
	ewl_image_file_set(EWL_IMAGE(em->eimage), ecore_dlist_current(em->images), NULL);

	return;
}

/*Create and Add a Tree to the Container c*/
static Ewl_Widget *add_atree(Ewl_Widget *c)
{
	Ewl_Widget *tree;
	Ewl_Model *model;
	Ewl_View *view;

	model = ewl_model_new();
	ewl_model_fetch_set(model, album_data_fetch);
	ewl_model_count_set(model, album_data_count);

	tree = ewl_tree2_new();
	ewl_tree2_headers_visible_set(EWL_TREE2(tree), 0);
	ewl_tree2_fixed_rows_set(EWL_TREE2(tree), 1);
	ewl_mvc_model_set(EWL_MVC(tree), model);
	ewl_mvc_selection_mode_set(EWL_MVC(tree), EWL_SELECTION_MODE_SINGLE);
	ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(c), tree);
	ewl_widget_show(tree);

	view = ewl_view_new();
	ewl_view_constructor_set(view, album_view_new);
	ewl_view_assign_set(view, album_view_assign);
	ewl_view_header_fetch_set(view, album_header_fetch);
	ewl_tree2_column_append(EWL_TREE2(tree), view, FALSE);

	return tree;
}


/* The view of the users albums */
static Ewl_Widget *album_view_new(void)
{
	Ewl_Widget *icon;

	icon = ewl_icon_new();
	ewl_icon_thumbnailing_set(EWL_ICON(icon), FALSE);
	ewl_box_orientation_set(EWL_BOX(icon), EWL_ORIENTATION_HORIZONTAL);
	ewl_object_alignment_set(EWL_OBJECT(icon), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(icon), EWL_FLAG_FILL_ALL);
	ewl_widget_show(icon);

	return icon;
}

/*The row that is added to the tree*/
static void album_view_assign(Ewl_Widget *w, void *data)
{
	const char *album;

	album = data;
	ewl_icon_image_set(EWL_ICON(w), PACKAGE_DATA_DIR "/images/image.png", NULL);
	ewl_icon_label_set(EWL_ICON(w), album);
	ewl_icon_constrain_set(EWL_ICON(w), 25);
	ewl_widget_name_set(w, album);
	ewl_callback_append(w, EWL_CALLBACK_CLICKED, populate, NULL);

	return;
}	

/* The header for the tree */
static Ewl_Widget *album_header_fetch(void *data, int column)
{
	Ewl_Widget *label;

	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), "Browser");
	ewl_widget_show(label);

	return label;
}

/* The albums that will be displayed*/ 
static void *album_data_fetch(void *data, unsigned int row, unsigned int column)
{
	const char *album;
	void *val = NULL;

	album = ecore_list_goto_index(em->albums, row);
	if (album)
	{
		val = (void *)album;
	}

	return val;
}

/* The number of albums the view is displaying */
static int album_data_count(void *data)
{
	int val;

	val = ecore_list_nodes(em->albums);

	return val;
}

