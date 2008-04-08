#include "ephoto.h"

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
	if (ec->requested_album)
		free(ec->requested_album);
	if (ec->requested_directory)
		free(ec->requested_directory);
	if (ec->requested_image)
		free(ec->requested_image);
	free(ec);
	ewl_main_quit();
	return;
}

void 
create_main(void)
{
	const char *type;
	char buf[PATH_MAX];
	Ewl_Widget *hbox, *vbox;

	if (ec->requested_image)
	{
		type = efreet_mime_type_get(ec->requested_image);
		if ((ecore_hash_get(em->types, type)) != "image")
			ec->requested_image = NULL;
	}

	if (ec->requested_album)
		em->current_album = strdup(ec->requested_album);
	else
		em->current_album = strdup("Complete Library");
	em->db = ephoto_db_init();

	if (ec->requested_directory)
		em->current_directory = strdup(ec->requested_directory);
	else
	{
		getcwd(buf, PATH_MAX);
		em->current_directory = strdup(buf);
	}

	em->win = add_window("Ephoto!", 715, 500, destroy, NULL);

	vbox = add_box(em->win, EWL_ORIENTATION_VERTICAL, 5);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);

	hbox = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 5);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);

	em->view = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 1);
	ewl_object_fill_policy_set(EWL_OBJECT(em->view), EWL_FLAG_FILL_ALL);

	em->view_box = ewl_notebook_new();
	ewl_notebook_tabbar_visible_set(EWL_NOTEBOOK(em->view_box), 0);
	ewl_object_fill_policy_set(EWL_OBJECT(em->view_box), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(em->view), em->view_box);
	ewl_widget_show(em->view_box);

	if (ec->requested_image)
	{
		add_single_view(em->view_box);
		add_normal_view(em->view_box);
	}
	else
	{
		add_normal_view(em->view_box);
		add_single_view(em->view_box);
	}

	return;
}

