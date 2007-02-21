#include "ephoto.h"

/*Ewl Callbacks*/
static void combo_changed(Ewl_Widget *w, void *event, void *data);
static void destroy(Ewl_Widget *w, void *event, void *data);
static void populate(Ewl_Widget *w, void *event, void *data);
static void window_fullscreen(Ewl_Widget *w, void *event, void *data);

/*Ephoto Create Callbacks*/
static Ewl_Widget *add_atree(Ewl_Widget *c);
static Ewl_Widget *add_vcombo(Ewl_Widget *c);

/*Ephoto MVC Callbacks*/
static Ewl_Widget *album_view_new(void);
static void album_view_assign(Ewl_Widget *w, void *data);
static Ewl_Widget *album_header_fetch(void *data, int column);
static void *album_data_fetch(void *data, unsigned int row, unsigned int column);
static int album_data_count(void *data);
static Ewl_Widget *views_header_fetch(void *data, int column);
static void *views_data_fetch(void *data, unsigned int row, unsigned int column);
static int views_data_count(void *data);

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
	Ewl_Widget *vbox, *spacer, *text, *entry, *hbox;
	Ewl_Widget *rvbox, *sp, *hsep, *vsep, *image;

	em = NULL;
	em = calloc(1, sizeof(Ephoto_Main));

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

	em->vcombo = add_vcombo(em->tbar);
	
	spacer = ewl_spacer_new();
	ewl_object_minimum_w_set(EWL_OBJECT(spacer), 100);
	ewl_object_maximum_w_set(EWL_OBJECT(spacer), 100);
	ewl_container_child_append(EWL_CONTAINER(em->tbar), spacer);
	ewl_widget_show(spacer);

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

	em->toolbar = ewl_toolbar_new();
	ewl_box_spacing_set(EWL_BOX(em->toolbar), 5);
	ewl_object_minimum_h_set(EWL_OBJECT(em->toolbar), 30);
	ewl_object_alignment_set(EWL_OBJECT(em->toolbar), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(em->toolbar), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(vbox), em->toolbar);
	ewl_widget_show(em->toolbar);

        image = add_image(em->toolbar, PACKAGE_DATA_DIR "/images/normal_view.png", 0, show_normal_view, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

        vsep = ewl_vseparator_new();
        ewl_container_child_append(EWL_CONTAINER(em->toolbar), vsep);
        ewl_widget_show(vsep);

        image = add_image(em->toolbar, PACKAGE_DATA_DIR "/images/list_view.png", 0, show_list_view, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);        

        vsep = ewl_vseparator_new();
        ewl_container_child_append(EWL_CONTAINER(em->toolbar), vsep);
        ewl_widget_show(vsep);

	image = add_image(em->toolbar, PACKAGE_DATA_DIR "/images/edit_view.png", 0, show_edit_view, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

	vsep = ewl_vseparator_new();
	ewl_container_child_append(EWL_CONTAINER(em->toolbar), vsep);
	ewl_widget_show(vsep);

        image = add_image(em->toolbar, PACKAGE_DATA_DIR "/images/get_exif.png", 0, display_exif_dialog, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

        vsep = ewl_vseparator_new();
        ewl_container_child_append(EWL_CONTAINER(em->toolbar), vsep);
        ewl_widget_show(vsep);

        image = add_image(em->toolbar, PACKAGE_DATA_DIR "/images/stock_fullscreen.png", 0, window_fullscreen, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

        vsep = ewl_vseparator_new();
        ewl_container_child_append(EWL_CONTAINER(em->toolbar), vsep);
        ewl_widget_show(vsep);

        image = add_image(em->toolbar, PACKAGE_DATA_DIR "/images/x-office-presentation.png", 0, NULL, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

        vsep = ewl_vseparator_new();
        ewl_container_child_append(EWL_CONTAINER(em->toolbar), vsep);
        ewl_widget_show(vsep);

	em->edit_tools = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(em->edit_tools), EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(em->toolbar), em->edit_tools);
	ewl_widget_show(em->edit_tools);

	add_edit_tools(em->edit_tools);
	ewl_widget_hide(em->edit_tools);

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
	char *album, *imagef;
	Ewl_Widget *shadow, *thumb;

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

	if (em->images)
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

/*Create the view combo*/
static Ewl_Widget *add_vcombo(Ewl_Widget *c)
{
	Ewl_Widget *combo;
	Ewl_Model *model;
	Ewl_View *view;

	em->views = calloc(3, sizeof(char *));
	em->views[0] = strdup("Normal");
	em->views[1] = strdup("List");
	em->views[2] = strdup("Edit");

	model = ewl_model_new();
	ewl_model_fetch_set(model, views_data_fetch);
	ewl_model_count_set(model, views_data_count);

	view = ewl_view_new();
	ewl_view_constructor_set(view, ewl_label_new);
	ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_label_text_set));
	ewl_view_header_fetch_set(view, views_header_fetch);

	combo = ewl_combo_new();
	ewl_mvc_model_set(EWL_MVC(combo), model);
	ewl_mvc_view_set(EWL_MVC(combo), view);
	ewl_mvc_data_set(EWL_MVC(combo), em->views);
	ewl_object_fill_policy_set(EWL_OBJECT(combo), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(c), combo);
	ewl_callback_append(combo, EWL_CALLBACK_VALUE_CHANGED, combo_changed, NULL);
	ewl_widget_show(combo);

	return combo;
}

/*Create a header for the view combo*/
static Ewl_Widget *views_header_fetch(void *data, int col)
{
	Ewl_Widget *header;

	header = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(header), "Select a View");
	ewl_widget_show(header);

	return header;
}

/*Get the data for the view combo*/
static void *views_data_fetch(void *data, unsigned int row, unsigned int col)
{
	if (row < 3) return em->views[row];
	else return NULL;
}

/*Show the number of items in the view*/
static int views_data_count(void *data)
{
	return 3;
}

/*Switch between views when the combo changes*/
static void combo_changed(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Selection_Idx *idx;

	idx = ewl_mvc_selected_get(EWL_MVC(w));

	if (!strcmp(em->views[idx->row], "Normal"))
	{
		show_normal_view(NULL, NULL, NULL);
	}
	else if (!strcmp(em->views[idx->row], "Edit"))
	{
		show_edit_view(NULL, NULL, NULL);
	}
	else if (!strcmp(em->views[idx->row], "List"))
	{
		show_list_view(NULL, NULL, NULL);
	}

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
	char *album;

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
	char *album;
	void *val = NULL;

	album = ecore_list_goto_index(em->albums, row);
	if (album)
	{
		val = album;
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

