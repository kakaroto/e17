#include "ephoto.h"

/*Destroy Boot/Create Main Window*/
static int destroy_boot(void *data);
static void create_main_gui(void);

/*Ewl Callbacks*/
static void destroy(Ewl_Widget *w, void *event, void *data);
static void populate_files(Ewl_Widget *w, void *event, void *data);
static void view_catalog(Ewl_Widget *w, void *event, void *data);
static void view_image(Ewl_Widget *w, void *event, void *data);

/*Ephoto Widget Creation Callbacks*/
static void add_button(Ewl_Widget *c, char *txt, char *img, void *cb);
static Ewl_Widget *add_menu(Ewl_Widget *c, char *txt);
static void add_menu_item(Ewl_Widget *c, char *txt, char *img, void *cb);
static Ewl_Widget *add_tree(Ewl_Widget *c);

/*Ephoto MVC Callbacks*/
static Ewl_Widget *directory_view_new(void);
static void directory_view_assign(Ewl_Widget *w, void *data);
static Ewl_Widget *directory_header_fetch(void *data, int column);
static void *directory_data_fetch(void *data, unsigned int row, unsigned int column);
static int directory_data_count(void *data);

/*Ephoto Image Manipulation*/
static void flip_image_horizontal(Ewl_Widget *w, void *event, void *data);
static void flip_image_vertical(Ewl_Widget *w, void *event, void *data);
static void rotate_image_left(Ewl_Widget *w, void *event, void *data);
static void rotate_image_right(Ewl_Widget *w, void *event, void *data);

/*Ephoto Widget Global Variables*/
static Ewl_Widget *ftree, *atree, *fbox, *vnb;
static Ewl_Widget *cvbox, *ivbox, *vimage, *progress;

/*Ephoto Ecore Global Variables*/
static Ecore_Timer *timer;
static Ecore_List *directories, *images;

/*Ephoto Current Directory*/
static char *current_directory;

/*Destroy Boot Splash*/
int destroy_boot(void *data)
{
	Ewl_Widget *win;
	double val, new_val;

	val = ewl_range_value_get(EWL_RANGE(progress));
	new_val = val + 20;
	ewl_range_value_set(EWL_RANGE(progress), new_val);	

	if (new_val == 100)
	{
		win = data;
		ewl_widget_destroy(win);
		ecore_timer_del(timer);
		create_main_gui();
	}
	return 1;
}

/*Create the Boot Splash and Start its Timer*/
void init_gui(void)
{
	Ewl_Widget *win, *vbox, *image, *text;

	win = ewl_window_new();
        ewl_window_title_set(EWL_WINDOW(win), "Ephoto!");
        ewl_window_name_set(EWL_WINDOW(win), "Ephoto!");
        ewl_window_override_set(EWL_WINDOW(win), 1);
	ewl_object_size_request(EWL_OBJECT(win), 325, 240);
	ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, destroy, NULL);
        ewl_widget_state_set(win, "splash", EWL_STATE_PERSISTENT);
	ewl_widget_show(win);

        vbox = ewl_vbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(win), vbox);
        ewl_widget_show(vbox);

	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), VERSION);
	ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(vbox), text);
	ewl_widget_show(text);

	image = ewl_image_new();
	ewl_image_file_set(EWL_IMAGE(image), 
			   PACKAGE_DATA_DIR "/images/logo.png", NULL);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(vbox), image);
	ewl_widget_show(image);

	text = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(text), "          Ephoto    \n"
					  "By Stephen Houston");
	ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(vbox), text);
	ewl_widget_show(text);

	progress = ewl_progressbar_new();
	ewl_range_value_set(EWL_RANGE(progress), 0);
	ewl_object_alignment_set(EWL_OBJECT(progress), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(vbox), progress);
	ewl_object_maximum_size_set(EWL_OBJECT(progress), 200, 20);
	ewl_widget_show(progress);

	timer = ecore_timer_add(.2, destroy_boot, win);

	ewl_main();
}

/*Destroy the Main Window*/
static void destroy(Ewl_Widget *w, void *event, void *data)
{
        ewl_widget_destroy(w);
        ewl_main_quit();
}

/*Create and Add a Button the Container c*/
static void add_button(Ewl_Widget *c, char *txt, char *img, void *cb)
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
		ewl_callback_append(button, EWL_CALLBACK_CLICKED, cb, NULL);
	}
	ewl_widget_show(button);

	return;
}

/*Create and Add a Menu to the Container c.*/
static Ewl_Widget *add_menu(Ewl_Widget *c, char *txt)
{
	Ewl_Widget *menu;

	menu = ewl_menu_new();
	if (txt)
	{
		ewl_button_label_set(EWL_BUTTON(menu), S_(txt));
	}
	ewl_object_fill_policy_set(EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
	ewl_container_child_append(EWL_CONTAINER(c), menu);
	ewl_widget_show(menu);

	return menu;
}

/*Create and Add a Menu Item to the Menu c*/
static void add_menu_item(Ewl_Widget *c, char *txt, char *img, void *cb)
{
	Ewl_Widget *menu_item;
	
	menu_item = ewl_menu_item_new();
	if (img)
	{
		ewl_button_image_set(EWL_BUTTON(menu_item), img, NULL);
	}
	if (txt)
	{
		ewl_button_label_set(EWL_BUTTON(menu_item), S_(txt));
	}
	ewl_object_alignment_set(EWL_OBJECT(menu_item), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(menu_item), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(c), menu_item);
	if (cb)
	{
		ewl_callback_append(menu_item, EWL_CALLBACK_CLICKED, cb, NULL);
	}
	ewl_widget_show(menu_item);

	return;
}

/*Create and Add a Tree to the Container c*/
static Ewl_Widget *add_tree(Ewl_Widget *c)
{
	Ewl_Widget *tree;
	Ewl_Model *model;
	Ewl_View *view;

	model = ewl_model_new();
	ewl_model_fetch_set(model, directory_data_fetch);
	ewl_model_count_set(model, directory_data_count);

	tree = ewl_tree2_new();
	ewl_mvc_selection_mode_set(EWL_MVC(tree), EWL_SELECTION_MODE_SINGLE);
	ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(c), tree);
	ewl_widget_show(tree);

	view = ewl_view_new();
	ewl_view_constructor_set(view, directory_view_new);
	ewl_view_assign_set(view, directory_view_assign);
	ewl_view_header_fetch_set(view, directory_header_fetch);

	ewl_tree2_column_append(EWL_TREE2(tree), model, view);

	return tree;
}


/* The view of the users directories */
static Ewl_Widget *directory_view_new(void)
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
static void directory_view_assign(Ewl_Widget *w, void *data)
{
	char *file;

	file = data;
	ewl_icon_image_set(EWL_ICON(w), PACKAGE_DATA_DIR "/images/folder.png", NULL);
	ewl_icon_label_set(EWL_ICON(w), basename(file));
	ewl_icon_constrain_set(EWL_ICON(w), 25);
	if (strcmp(file, ".."))
	{
		ewl_widget_name_set(w, file);
	}
	else
	{
		ewl_widget_name_set(w, dirname(current_directory));
	}
	ewl_callback_append(w, EWL_CALLBACK_CLICKED, populate_files, NULL);
}	

/* The header for the tree */
static Ewl_Widget *directory_header_fetch(void *data, int column)
{
	Ewl_Widget *label;

	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), "Browser");
	ewl_widget_show(label);

	return label;
}

/* The directories that will be displayed*/ 
static void *directory_data_fetch(void *data, unsigned int row, unsigned int column)
{
	char *file;
	void *val = NULL;

	file = ecore_list_goto_index(directories, row);
	if (file)
	{
		val = file;
	}

	return val;
}

/* The number of directories the view is displaying */
static int directory_data_count(void *data)
{
	int val;

	val = ecore_list_nodes(directories);

	return val;
}

/*Switch to the Image List*/
static void view_catalog(Ewl_Widget *w, void *event, void *data)
{
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(vnb), cvbox);
}

/*Switch to the Image Viewer*/
static void view_image(Ewl_Widget *w, void *event, void *data)
{
	char *file;

	file = data;
	ewl_image_file_set(EWL_IMAGE(vimage), file, NULL);
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(vnb), ivbox);
}

/*Update the Directory List and Image List*/
static void populate_files(Ewl_Widget *w, void *event, void *data)
{
	char *cdir, *imagef;
	Ewl_Widget *thumb;

	if (w)
	{
		cdir = (char *)ewl_widget_name_get(w);
	}
	else
	{
		cdir = data;
	}

	current_directory = cdir;

	if (!ecore_list_is_empty(directories))
	{
		ecore_list_destroy(directories);
		directories = ecore_list_new();
	}
	if (!ecore_list_is_empty(images))
	{
		ecore_list_destroy(images);
		images = ecore_list_new();
	}

	directories = get_directories(cdir);
	images = get_images(cdir);

	ewl_mvc_data_set(EWL_MVC(ftree), directories);

	ewl_container_reset(EWL_CONTAINER(fbox));
	while (!ecore_list_is_empty(images))
	{
		imagef = ecore_list_remove_first(images);

		thumb = ewl_image_thumbnail_new();
		ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(thumb), imagef);
		ewl_image_proportional_set(EWL_IMAGE(thumb), TRUE);
		ewl_image_constrain_set(EWL_IMAGE(thumb), 100);
		ewl_container_child_append(EWL_CONTAINER(fbox), thumb);
                ewl_callback_append(thumb, EWL_CALLBACK_CLICKED, view_image, imagef);
		ewl_widget_show(thumb);
	}
	ewl_widget_configure(fbox);
	ewl_widget_configure(fbox->parent);
}

/*Flip the image 180 degrees horizontally*/
static void flip_image_horizontal(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;

	evas_object_image_size_get(EWL_IMAGE(vimage)->image, &nw, &nh);
	image_data = flip_horizontal(vimage);
	update_image(vimage, nw, nh, image_data);
	ewl_widget_configure(vimage);
	ewl_widget_configure(vimage->parent);
}

/*Flip the image 180 degrees vertically*/
static void flip_image_vertical(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;

	evas_object_image_size_get(EWL_IMAGE(vimage)->image, &nw, &nh);
	image_data = flip_vertical(vimage);
	update_image(vimage, nw, nh, image_data);
	ewl_widget_configure(vimage);
	ewl_widget_configure(vimage->parent);
}

/*Rotate the image 90 degrees to the left*/
static void rotate_image_left(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;

	evas_object_image_size_get(EWL_IMAGE(vimage)->image, &nh, &nw);
	image_data = rotate_left(vimage);
	update_image(vimage, nw, nh, image_data);
	ewl_widget_configure(vimage);
	ewl_widget_configure(vimage->parent);
}

/*Rotate the image 90 degrees to the right*/
static void rotate_image_right(Ewl_Widget *w, void *event, void *data)
{
	unsigned int *image_data;
	int nw, nh;

	evas_object_image_size_get(EWL_IMAGE(vimage)->image, &nh, &nw);
	image_data = rotate_right(vimage);
	update_image(vimage, nw, nh, image_data);
	ewl_widget_configure(vimage);
	ewl_widget_configure(vimage->parent);
}

/*Create the Main Ephoto Window*/
static void create_main_gui(void)
{
	Ewl_Widget *win, *vbox, *menu_bar, *menu, *paned;
	Ewl_Widget *ihbox, *sp;

	win = ewl_window_new();
        ewl_window_title_set(EWL_WINDOW(win), "Ephoto!");
        ewl_window_name_set(EWL_WINDOW(win), "Ephoto!");
        ewl_object_size_request(EWL_OBJECT(win), 600, 400);
        ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, destroy, NULL);
	ewl_widget_show(win);

	vbox = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(win), vbox);
	ewl_widget_show(vbox);

	menu_bar = ewl_hmenubar_new();
	ewl_object_fill_policy_set(EWL_OBJECT(menu_bar), EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(vbox), menu_bar);
	ewl_widget_show(menu_bar);

	menu = add_menu(menu_bar, "Menu|File");
	add_menu_item(menu, "Menu|File|Exit", PACKAGE_DATA_DIR 
		"/images/exit.png", destroy);

	menu = add_menu(menu_bar, "Menu|Help");
	add_menu_item(menu, "Menu|Help|About", NULL, NULL);

	paned = ewl_hpaned_new();
	ewl_object_fill_policy_set(EWL_OBJECT(paned), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(vbox), paned);
	ewl_widget_show(paned);

	ftree = add_tree(paned);

	vnb = ewl_notebook_new();
	ewl_notebook_tabbar_visible_set(EWL_NOTEBOOK(vnb), 0);
	ewl_object_fill_policy_set(EWL_OBJECT(vnb), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(paned), vnb);
	ewl_widget_show(vnb);

	cvbox = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(cvbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(vnb), cvbox);
	ewl_widget_show(cvbox);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(vnb), cvbox, "Catalog");

	sp = ewl_scrollpane_new();
	ewl_object_fill_policy_set(EWL_OBJECT(sp), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(cvbox), sp);
	ewl_widget_show(sp);

	fbox = ewl_hfreebox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(fbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(sp), fbox);
	ewl_widget_show(fbox);

	ivbox = ewl_vbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(ivbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(vnb), ivbox);
        ewl_widget_show(ivbox);

        sp = ewl_scrollpane_new();
        ewl_object_fill_policy_set(EWL_OBJECT(sp), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(ivbox), sp);
        ewl_widget_show(sp);

	vimage = ewl_image_new();
	ewl_image_proportional_set(EWL_IMAGE(vimage), TRUE);
	ewl_object_alignment_set(EWL_OBJECT(vimage), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(vimage), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(sp), vimage);
	ewl_widget_show(vimage);

	ihbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(ivbox), ihbox);
	ewl_object_alignment_set(EWL_OBJECT(ihbox), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(ihbox), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show(ihbox);
	
	add_button(ihbox, "Flip Horizontal", NULL, flip_image_horizontal);
	add_button(ihbox, "Flip Vertical", NULL, flip_image_vertical);
	add_button(ihbox, "Rotate Left", NULL, rotate_image_left);
	add_button(ihbox, "Rotate Right", NULL, rotate_image_right);
	add_button(ihbox, "Return to Catalog", NULL, view_catalog);

	directories = ecore_list_new();
	images = ecore_list_new();
	populate_files(NULL, NULL, getenv("HOME"));

	return;
}
