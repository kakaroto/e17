#include "ephoto.h"

/*Ewl Callbacks*/
static void destroy(Ewl_Widget *w, void *event, void *data);
static void populate(Ewl_Widget *w, void *event, void *data);
static void window_fullscreen(Ewl_Widget *w, void *event, void *data);

/*Ephoto Create Callbacks*/
static Ewl_Widget *add_atree(Ewl_Widget *c);

/*Ephoto MVC Callbacks*/
static Ewl_Widget *album_view_new(void *data, int column, int row);
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
	Ewl_Widget *vbox, *hbox, *hsep, *vsep, *button;
	Ewl_Widget *mb, *menu, *mi;

	em = NULL;
	em = calloc(1, sizeof(Ephoto_Main));
	em->albums = ecore_list_new();
	em->images = ecore_dlist_new();

	em->win = add_window("Ephoto!", 775, 540, destroy, NULL);

	vbox = add_box(em->win, EWL_ORIENTATION_VERTICAL, 5);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);

	mb = add_menubar(vbox);
	menu = add_menu(mb, "File");
	mi = add_menu_item(menu, "Exit", PACKAGE_DATA_DIR "/images/exit.png", destroy, NULL);

	hbox = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 2);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);

	em->atree = add_atree(hbox);
	ewl_object_maximum_w_set(EWL_OBJECT(em->atree), 172);

	em->view_box = ewl_notebook_new();
	ewl_notebook_tabbar_visible_set(EWL_NOTEBOOK(em->view_box), 0);
	ewl_object_fill_policy_set(EWL_OBJECT(em->view_box), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(hbox), em->view_box);
	ewl_widget_show(em->view_box);

	add_normal_view(em->view_box);

	add_edit_view(em->view_box);

	add_list_view(em->view_box);	
	
	hsep = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hsep);
	ewl_widget_show(hsep);

	em->toolbar = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 1);
	ewl_object_minimum_h_set(EWL_OBJECT(em->toolbar), 30);
	ewl_object_alignment_set(EWL_OBJECT(em->toolbar), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(em->toolbar), EWL_FLAG_FILL_SHRINK);

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

                thumb = add_image(em->fbox, imagef, 1, freebox_image_clicked, NULL);
		ewl_image_constrain_set(EWL_IMAGE(thumb), 64);
		ewl_object_alignment_set(EWL_OBJECT(thumb), EWL_FLAG_ALIGN_CENTER);
		ewl_widget_name_set(thumb, imagef);

		ecore_dlist_next(em->images);
        }
	ecore_dlist_goto_first(em->images);

        ewl_mvc_data_set(EWL_MVC(em->ltree), em->images);
	ewl_mvc_dirty_set(EWL_MVC(em->ltree), 1);

	return;
}

/*Create and Add a Tree to the Container c*/
static Ewl_Widget *add_atree(Ewl_Widget *c)
{
	Ewl_Widget *tree;
	Ewl_Model *model;
	Ewl_View *view;

	model = ewl_model_new();
	ewl_model_data_fetch_set(model, album_data_fetch);
	ewl_model_data_count_set(model, album_data_count);

        view = ewl_view_new();
        ewl_view_widget_fetch_set(view, album_view_new);
        ewl_view_header_fetch_set(view, album_header_fetch);

	tree = ewl_tree2_new();
	ewl_tree2_headers_visible_set(EWL_TREE2(tree), 0);
	ewl_tree2_fixed_rows_set(EWL_TREE2(tree), 1);
	ewl_tree2_column_count_set(EWL_TREE2(tree), 1);
	ewl_mvc_model_set(EWL_MVC(tree), model);
	ewl_mvc_view_set(EWL_MVC(tree), view);
	ewl_mvc_selection_mode_set(EWL_MVC(tree), EWL_SELECTION_MODE_SINGLE);
	ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(c), tree);
	ewl_widget_show(tree);

	return tree;
}


/* The view of the users albums */
static Ewl_Widget *album_view_new(void *data, int column, int row)
{
	const char *album;
	Ewl_Widget *icon;

	album = data;

	icon = ewl_icon_new();
	ewl_icon_thumbnailing_set(EWL_ICON(icon), FALSE);
        ewl_icon_image_set(EWL_ICON(icon), PACKAGE_DATA_DIR "/images/image.png", NULL);
        ewl_icon_label_set(EWL_ICON(icon), album);
        ewl_icon_constrain_set(EWL_ICON(icon), 25);
	ewl_box_orientation_set(EWL_BOX(icon), EWL_ORIENTATION_HORIZONTAL);
	ewl_object_alignment_set(EWL_OBJECT(icon), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(icon), EWL_FLAG_FILL_ALL);
        ewl_callback_append(icon, EWL_CALLBACK_CLICKED, populate, NULL);
	ewl_widget_name_set(icon, album);
	ewl_widget_show(icon);

	return icon;
}

/* The header for the tree */
static Ewl_Widget *album_header_fetch(void *data, int column)
{
	Ewl_Widget *label;

	label = add_label(NULL, "Browser");

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

