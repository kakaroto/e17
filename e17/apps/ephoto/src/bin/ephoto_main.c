#include "ephoto.h"

/*Ephoto Databasing Global Variables*/
static sqlite3 *db;

/*Ephoto Ecore Global Variables*/
static Ecore_List *albums;

/*Ephoto Layout Global Variable*/
static char **views;

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
Ewl_Widget *atree, *tbar, *vcombo, *view_box, *fbox_vbox, *edit_vbox, *list_vbox; 
Ewl_Widget *fbox, *eimage, *ltree, *ilabel, *currenta, *currentf, *currenti;
Ecore_List *images;


/*Destroy the Main Window*/
static void destroy(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Widget *window;

	window = data;
	ephoto_db_close(db);
        ewl_widget_destroy(window);
        ewl_main_quit();
	return;
}

static void window_fullscreen(Ewl_Widget *w, void *event, void *data)
{
	int fullscreen;
	Ewl_Widget *window;
	
	window = data;
	fullscreen = ewl_window_fullscreen_get(EWL_WINDOW(window));
	if(!fullscreen)
	{
		ewl_window_fullscreen_set(EWL_WINDOW(window), 1);
	}
	else
	{
		ewl_window_fullscreen_set(EWL_WINDOW(window), 0);
	}
	return;
}

/*Create the Main Ephoto Window*/
void create_main_gui(void)
{
	Ecore_List *images;

	Ewl_Widget *win, *vbox, *spacer, *text, *entry, *hbox;
	Ewl_Widget *rvbox, *sp, *hsep, *toolbar, *vsep, *image;
	Ewl_Widget *shbox, *seeker;

	win = ewl_window_new();
        ewl_window_title_set(EWL_WINDOW(win), "Ephoto!");
        ewl_window_name_set(EWL_WINDOW(win), "Ephoto!");
        ewl_object_size_request(EWL_OBJECT(win), 777, 542);
        ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, destroy, win);
	ewl_widget_show(win);

	vbox = ewl_vbox_new();
	ewl_box_spacing_set(EWL_BOX(vbox), 5);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(win), vbox);
	ewl_widget_show(vbox);

	tbar = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(tbar), EWL_FLAG_FILL_SHRINK);
	ewl_object_alignment_set(EWL_OBJECT(tbar), EWL_FLAG_ALIGN_RIGHT);
	ewl_container_child_append(EWL_CONTAINER(vbox), tbar);
	ewl_widget_show(tbar);

	vcombo = add_vcombo(tbar);
	
	spacer = ewl_spacer_new();
	ewl_object_minimum_w_set(EWL_OBJECT(spacer), 100);
	ewl_object_maximum_w_set(EWL_OBJECT(spacer), 100);
	ewl_container_child_append(EWL_CONTAINER(tbar), spacer);
	ewl_widget_show(spacer);

	entry = ewl_entry_new();
	ewl_object_minimum_size_set(EWL_OBJECT(entry), 157, 22);
	ewl_object_maximum_size_set(EWL_OBJECT(entry), 157, 22);
	ewl_container_child_append(EWL_CONTAINER(tbar), entry);
	ewl_widget_show(entry);

	add_button(tbar, "Find", NULL, NULL, NULL);

	hbox = ewl_hbox_new();
	ewl_box_spacing_set(EWL_BOX(hbox), 2);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);

	atree = add_atree(hbox);
	ewl_object_maximum_w_set(EWL_OBJECT(atree), 172);

	rvbox = ewl_vbox_new();
	ewl_box_spacing_set(EWL_BOX(rvbox), 1);
	ewl_object_fill_policy_set(EWL_OBJECT(rvbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(hbox), rvbox);
	ewl_widget_show(rvbox);

	view_box = ewl_notebook_new();
	ewl_notebook_tabbar_visible_set(EWL_NOTEBOOK(view_box), 0);
	ewl_object_fill_policy_set(EWL_OBJECT(view_box), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(rvbox), view_box);
	ewl_widget_show(view_box);

	fbox_vbox = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(fbox_vbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(view_box), fbox_vbox);
	ewl_widget_show(fbox_vbox);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(view_box), fbox_vbox, "Normal");

	sp = ewl_scrollpane_new();
	ewl_object_fill_policy_set(EWL_OBJECT(sp), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(fbox_vbox), sp);
	ewl_widget_show(sp);

        fbox = ewl_hfreebox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(fbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(sp), fbox);
	ewl_widget_show(fbox);

        edit_vbox = ewl_vbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(edit_vbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(view_box), edit_vbox);
        ewl_widget_show(edit_vbox);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(view_box), edit_vbox, "Edit");
	
	eimage = add_image(edit_vbox, NULL, 0, NULL, NULL);

	list_vbox = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(list_vbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(view_box), list_vbox);
	ewl_widget_show(list_vbox);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(view_box), list_vbox, "List");

	ltree = add_ltree(list_vbox);	

	ilabel = add_label(rvbox, "Image Information", 1);

	/*shbox = ewl_hbox_new();
	ewl_object_alignment_set(EWL_OBJECT(shbox), EWL_FLAG_ALIGN_RIGHT);
	ewl_object_fill_policy_set(EWL_OBJECT(shbox), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(vbox), shbox);
	ewl_widget_show(shbox);

	seeker = ewl_hseeker_new();
	ewl_object_maximum_w_set(EWL_OBJECT(seeker), 150);
	ewl_object_alignment_set(EWL_OBJECT(seeker), EWL_FLAG_ALIGN_RIGHT);
	ewl_container_child_append(EWL_CONTAINER(shbox), seeker);
	ewl_widget_show(seeker);*/

	hsep = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hsep);
	ewl_widget_show(hsep);

	toolbar = ewl_toolbar_new();
	ewl_box_spacing_set(EWL_BOX(toolbar), 5);
	ewl_object_minimum_h_set(EWL_OBJECT(toolbar), 30);
	ewl_object_alignment_set(EWL_OBJECT(toolbar), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(toolbar), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(vbox), toolbar);
	ewl_widget_show(toolbar);

        image = add_image(toolbar, PACKAGE_DATA_DIR "/images/normal_view.png", 0, show_normal_view, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

        image = add_image(toolbar, PACKAGE_DATA_DIR "/images/edit_view.png", 0, show_edit_view, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

        image = add_image(toolbar, PACKAGE_DATA_DIR "/images/list_view.png", 0, show_list_view, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

	vsep = ewl_vseparator_new();
	ewl_container_child_append(EWL_CONTAINER(toolbar), vsep);
	ewl_widget_show(vsep);

        image = add_image(toolbar, PACKAGE_DATA_DIR "/images/get_exif.png", 0, display_exif_dialog, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

        vsep = ewl_vseparator_new();
        ewl_container_child_append(EWL_CONTAINER(toolbar), vsep);
        ewl_widget_show(vsep);

        image = add_image(toolbar, PACKAGE_DATA_DIR "/images/stock_fullscreen.png", 0, window_fullscreen, win);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

        image = add_image(toolbar, PACKAGE_DATA_DIR "/images/x-office-presentation.png", 0, NULL, NULL);
        ewl_image_constrain_set(EWL_IMAGE(image), 30);

	albums = ecore_list_new();
	db = ephoto_db_init();
	albums = ephoto_db_list_albums(db);

	ewl_mvc_data_set(EWL_MVC(atree), albums);
	
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
		if (currenta) 
		{
			ewl_widget_state_set(currenta, "unselected", EWL_STATE_PERSISTENT);
			ewl_widget_state_set(EWL_ICON(currenta)->label, "default", EWL_STATE_PERSISTENT);
		}
		currenta = w;
		ewl_widget_state_set(currenta, "selected", EWL_STATE_PERSISTENT);
		ewl_widget_state_set(EWL_ICON(currenta)->label, "blue", EWL_STATE_PERSISTENT);
	}
	else
	{
		album = data;
	}

	if (images)
	{
		ecore_list_destroy(images);
	}

	images = ecore_list_new();
	images = ephoto_db_list_images(db, album);

	ecore_list_goto_first(images);
	ewl_container_reset(EWL_CONTAINER(fbox));
        while (ecore_list_current(images))
        {
                imagef = ecore_list_current(images);

                thumb = add_image(fbox, imagef, 1, set_info, NULL);
		ewl_object_alignment_set(EWL_OBJECT(thumb), EWL_FLAG_ALIGN_CENTER);
		ewl_widget_name_set(thumb, imagef);

		ecore_list_next(images);
        }

	ecore_list_goto_first(images);

        ewl_mvc_data_set(EWL_MVC(ltree), images);
	ewl_mvc_dirty_set(EWL_MVC(ltree), 1);
	
	ewl_image_file_set(EWL_IMAGE(eimage), ecore_list_current(images), NULL);

	return;
}

/*Create the view combo*/
static Ewl_Widget *add_vcombo(Ewl_Widget *c)
{
	Ewl_Widget *combo;
	Ewl_Model *model;
	Ewl_View *view;

	views = calloc(3, sizeof(char *));
	views[0] = strdup("Normal");
	views[1] = strdup("Edit");
	views[2] = strdup("List");

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
	ewl_mvc_data_set(EWL_MVC(combo), views);
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
	if (row < 3) return views[row];
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

	if (!strcmp(views[idx->row], "Normal"))
	{
		show_normal_view(NULL, NULL, NULL);
	}
	else if (!strcmp(views[idx->row], "Edit"))
	{
		show_edit_view(NULL, NULL, NULL);
	}
	else if (!strcmp(views[idx->row], "List"))
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

	album = ecore_list_goto_index(albums, row);
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

	val = ecore_list_nodes(albums);

	return val;
}

