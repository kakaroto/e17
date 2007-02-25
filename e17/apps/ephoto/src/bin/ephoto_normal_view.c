#include "ephoto.h"

/*Add the normal view*/
Ewl_Widget *add_normal_view(Ewl_Widget *c)
{
	Ewl_Widget *sp;

        em->fbox_vbox = ewl_vbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(em->fbox_vbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(c), em->fbox_vbox);
        ewl_widget_show(em->fbox_vbox);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), em->fbox_vbox, "Normal");

        sp = ewl_scrollpane_new();
        ewl_object_fill_policy_set(EWL_OBJECT(sp), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(em->fbox_vbox), sp);
        ewl_widget_show(sp);

        em->fbox = ewl_hfreebox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(em->fbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(sp), em->fbox);
        ewl_widget_show(em->fbox);

	em->ilabel = add_label(em->fbox_vbox, "Image Information");

	return em->fbox_vbox;
}

/*Show the normal view*/
void show_normal_view(Ewl_Widget *w, void *event, void *data)
{
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->view_box), em->fbox_vbox);
	ewl_widget_disable(em->edit_tools);
        ewl_widget_show(em->atree);
        ewl_widget_show(em->tbar);
        ewl_widget_show(em->ilabel);
}

/*Set the info that is in the info label on normal view*/
void set_info(Ewl_Widget *w, void *event, void *data)
{
	const char *path, *pixels, *size;
	char info[PATH_MAX];
	time_t modtime;

	if (ewl_widget_type_is(w, EWL_IMAGE_TYPE))
	{
		if (em->currentf) ewl_widget_state_set(em->currentf, "unselected", EWL_STATE_PERSISTENT);
		em->currentf = w;
		ewl_widget_state_set(em->currentf, "selected", EWL_STATE_PERSISTENT);
	}
	
	path = ewl_widget_name_get(w);
	pixels = image_pixels_string_get(path);
	size = file_size_get(ecore_file_size(path));
	modtime = ecore_file_mod_time(path);
	snprintf(info, PATH_MAX, "%s - %s - %s", basename((char *)path), pixels, size);

	ewl_label_text_set(EWL_LABEL(em->ilabel), info);
	ewl_widget_reparent(em->ilabel);

	return;
}

