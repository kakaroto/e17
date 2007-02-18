#include "ephoto.h"

/*Show the normal view*/
void show_normal_view(Ewl_Widget *w, void *event, void *data)
{
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(view_box), fbox_vbox);
        ewl_widget_show(atree);
        ewl_widget_show(tbar);
        ewl_widget_show(ilabel);
	ewl_widget_hide(edit_tools);
        ewl_widget_configure(fbox);
}

/*Set the info that is in the info label on normal view*/
void set_info(Ewl_Widget *w, void *event, void *data)
{
	char *path, *pixels, *size;
	char info[PATH_MAX];
	time_t modtime;

	if (ewl_widget_type_is(w, EWL_IMAGE_TYPE))
	{
		if (currentf) ewl_widget_state_set(currentf, "unselected", EWL_STATE_PERSISTENT);
		currentf = w;
		ewl_widget_state_set(currentf, "selected", EWL_STATE_PERSISTENT);
	}
	else
	{
		if (currenti)
		{
			ewl_widget_state_set(currenti, "unselected", EWL_STATE_PERSISTENT);
		}
                currenti = w;
                ewl_widget_state_set(currenti, "selected", EWL_STATE_PERSISTENT);
	}
	path = (char *)ewl_widget_name_get(w);
	pixels = image_pixels_string_get(path);
	size = file_size_get(ecore_file_size(path));
	modtime = ecore_file_mod_time(path);
	snprintf(info, PATH_MAX, "%s - %s - %s", basename(path), pixels, size);

	ewl_label_text_set(EWL_LABEL(ilabel), info);
	ewl_widget_reparent(ilabel);

	return;
}
