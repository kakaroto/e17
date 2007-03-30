#include "ephoto.h"

/*Add the normal view*/
Ewl_Widget *add_normal_view(Ewl_Widget *c)
{
	Ewl_Widget *sp;

        em->fbox_vbox = add_box(c, EWL_ORIENTATION_VERTICAL, 5);
        ewl_object_fill_policy_set(EWL_OBJECT(em->fbox_vbox), EWL_FLAG_FILL_ALL);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(c), em->fbox_vbox, "Normal");

        sp = ewl_scrollpane_new();
        ewl_object_fill_policy_set(EWL_OBJECT(sp), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(em->fbox_vbox), sp);
        ewl_widget_show(sp);

        em->fbox = ewl_hfreebox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(em->fbox), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(sp), em->fbox);
        ewl_widget_show(em->fbox);

	return em->fbox_vbox;
}

/*Show the normal view*/
void show_normal_view(Ewl_Widget *w, void *event, void *data)
{
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(em->view_box), em->fbox_vbox);
}

/*Action when an image is clicked*/
void freebox_image_clicked(Ewl_Widget *w, void *event, void *data)
{
	const char *path;

	path = ewl_widget_name_get(w);
//	show_edit_view(NULL, NULL, strdup(path));

	return;
}

