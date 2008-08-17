#include "ephoto.h"

static Ewl_Widget *add_atree(Ewl_Widget *c);
static Ewl_Widget *album_view_new(void *data, unsigned int row, 
					unsigned int column);
static Ewl_Widget *album_header_fetch(void *data, 
					unsigned int column);
static void *album_data_fetch(void *data, unsigned int row, 
					unsigned int column);
static unsigned int album_data_count(void *data);
static void add_rc_menu(Ewl_Widget *w, void *event, void *data);
static void album_clicked(Ewl_Widget *w, void *event, void *data);
static void populate_images(Ewl_Widget *w, void *event, void *data);

void 
show_albums(Ewl_Widget *c)
{
	em->atree = add_atree(c);
       	ewl_object_maximum_w_set(EWL_OBJECT(em->atree), 180);

	populate_albums(NULL, NULL, NULL);
}

static unsigned int
highlight_cb(void *data, unsigned int row)
{
	return 0;
}

static Ewl_Widget *
add_atree(Ewl_Widget *c)
{
	Ewl_Widget *tree;
	Ewl_Model *model;
	Ewl_View *view;

	model = ewl_model_new();
	ewl_model_data_fetch_set(model, album_data_fetch);
	ewl_model_data_count_set(model, album_data_count);
	ewl_model_data_highlight_set(model, highlight_cb);

        view = ewl_view_new();
        ewl_view_widget_fetch_set(view, album_view_new);
        ewl_view_header_fetch_set(view, album_header_fetch);

	tree = ewl_tree_new();
	ewl_tree_headers_visible_set(EWL_TREE(tree), TRUE);
	ewl_tree_fixed_rows_set(EWL_TREE(tree), TRUE);
	ewl_tree_column_count_set(EWL_TREE(tree), 1);
	ewl_mvc_model_set(EWL_MVC(tree), model);
	ewl_mvc_view_set(EWL_MVC(tree), view);
	ewl_mvc_selection_mode_set(EWL_MVC(tree), EWL_SELECTION_MODE_SINGLE);
	ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_FILL);
	ewl_object_minimum_w_set(EWL_OBJECT(tree), 180);
	ewl_container_child_prepend(EWL_CONTAINER(c), tree);
	ewl_widget_show(tree);

	return tree;
}

static Ewl_Widget *
album_view_new(void *data, unsigned int row, unsigned int column)
{
	char *album;
	Ewl_Widget *icon;

	album = data;

	icon = add_icon(NULL, album, PACKAGE_DATA_DIR "/images/image.png", 0, 
							album_clicked, NULL);
        ewl_icon_constrain_set(EWL_ICON(icon), 25);
	ewl_box_orientation_set(EWL_BOX(icon), EWL_ORIENTATION_HORIZONTAL);
	ewl_object_alignment_set(EWL_OBJECT(icon), EWL_FLAG_ALIGN_LEFT);
	ewl_object_fill_policy_set(EWL_OBJECT(icon), EWL_FLAG_FILL_ALL);
	ewl_widget_name_set(icon, album);
	ewl_callback_append(icon, EWL_CALLBACK_SHOW, add_rc_menu, NULL);

	return icon;
}

static Ewl_Widget *
album_header_fetch(void *data, unsigned int column)
{
	Ewl_Widget *label;

	label = add_label(NULL, "Albums");

	return label;
}

static void *
album_data_fetch(void *data, unsigned int row, unsigned int column)
{
	const char *album;
	void *val = NULL;

	album = ecore_list_index_goto(em->albums, row);
	if (album)
	{
		val = (void *)album;
	}

	return val;
}

static unsigned int 
album_data_count(void *data)
{
	int val;

	val = ecore_list_count(em->albums);

	return val;
}

static void album_clicked(Ewl_Widget *w, void *event, void *data)
{
	em->current_album = strdup(ewl_icon_label_get(EWL_ICON(w)));
	populate_images(NULL, NULL, NULL);
}

static void 
rc_remove(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Widget *icon;

	icon = data;
	ephoto_db_delete_album(em->db, ewl_icon_label_get(EWL_ICON(icon)));
	populate_albums(NULL, NULL, NULL);
}

static void 
add_rc_menu(Ewl_Widget *w, void *event, void *data)
{
        Ewl_Widget *context;

        context = ewl_context_menu_new();
        ewl_context_menu_attach(EWL_CONTEXT_MENU(context), w);

        add_menu_item(context, "Remove Album",
					PACKAGE_DATA_DIR "/images/remove.png", 
					rc_remove, w);
}

void 
populate_albums(Ewl_Widget *w, void *event, void *data)
{
	const char *album;
	pthread_t album_worker;
	int ret1;
	
	album = NULL;

	if (w)
	{
		album = ewl_widget_name_get(w);
		em->current_album = strdup(album);
	}

	if (!ecore_list_empty_is(em->albums))
		ecore_list_destroy(em->albums);

	em->albums = ecore_list_new();
	
	ret1 = pthread_create(&album_worker, NULL, get_albums_pre, NULL);
	if (ret1)
	{
		printf("ERROR: Couldn't create thread!\n");
		return;
	}
	pthread_join(album_worker, NULL);

	populate_images(NULL, NULL, NULL);

	return;
}

static void
populate_images(Ewl_Widget *w, void *event, void *data)
{
	pthread_t image_worker;
	int ret1;

	if (!ecore_list_empty_is(em->images))
		ecore_dlist_destroy(em->images);

	em->images = ecore_dlist_new();
	if (em->fbox) ewl_container_reset(EWL_CONTAINER(em->fbox));

	ret1 = pthread_create(&image_worker, NULL, get_aimages_pre, NULL);
	if (ret1)
	{	printf("ERROR: Couldn't create thread!\n");
		return;
	}
	pthread_join(image_worker, NULL);

	ewl_mvc_data_set(EWL_MVC(em->fbox), em->images);

	return;
}

void *
get_albums_pre()
{
        em->albums = ephoto_db_list_albums(em->db);
        ecore_dlist_first_goto(em->albums);
        ewl_mvc_data_set(EWL_MVC(em->atree), em->albums);
        pthread_exit(NULL);
}

void *
get_aimages_pre()
{
        em->images = ephoto_db_list_images(em->db, em->current_album);
        ecore_dlist_first_goto(em->images);
        pthread_exit(NULL);
}

