#include "ephoto.h"

Ephoto_Main *em;
static void destroy(Ewl_Widget *w, void *event, void *data);

static void 
destroy(Ewl_Widget *w, void *event, void *data)
{
        ewl_widget_destroy(em->win);
	ecore_hash_destroy(em->types);
	ecore_list_destroy(em->albums);
	ecore_list_destroy(em->fsystem);
	ecore_dlist_destroy(em->images);
	ephoto_db_close(em->db);
	free(em->current_album);
	free(em->current_directory);
	free(em);
	ewl_main_quit();
	return;
}

void 
create_main(void)
{
	char buf[PATH_MAX];
	Ewl_Widget *hbox, *mbar, *menu, *vbox;

	em = NULL;
        em = calloc(1, sizeof(Ephoto_Main));

        em->albums = ecore_list_new();
        em->fsystem = ecore_list_new();
        em->images = ecore_dlist_new();

        em->types = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        ecore_hash_set(em->types, "image/gif", "image");
        ecore_hash_set(em->types, "image/jpeg", "image");
        ecore_hash_set(em->types, "image/png", "image");
        ecore_hash_set(em->types, "image/svg+xml", "image");
        ecore_hash_set(em->types, "image/tiff", "image");

	em->current_album = strdup("Complete Library");
	em->db = ephoto_db_init();

	getcwd(buf, PATH_MAX);
	em->current_directory = strdup(buf);

	em->win = add_window("Ephoto - Photo Management", 600, 400, destroy, NULL);

	vbox = add_box(em->win, EWL_ORIENTATION_VERTICAL, 5);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);

	mbar = add_menubar(vbox);

	menu = add_menu(mbar, "File");
	add_menu_item(menu, "Exit",
			PACKAGE_DATA_DIR "/images/exit.png", destroy, NULL);
	menu = add_menu(mbar, "Help");
	add_menu_item(menu, "About",
			PACKAGE_DATA_DIR "/images/help.png", about_dialog, NULL);

	hbox = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 5);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);

	em->view = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 1);
	ewl_object_fill_policy_set(EWL_OBJECT(em->view), EWL_FLAG_FILL_ALL);

	em->view_box = ewl_notebook_new();
	ewl_notebook_tabbar_visible_set(EWL_NOTEBOOK(em->view_box), 0);
	ewl_object_fill_policy_set(EWL_OBJECT(em->view_box), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(em->view), em->view_box);
	ewl_widget_show(em->view_box);

	add_normal_view(em->view_box);
	add_single_view(em->view_box);

	return;
}

