/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "ewl_base.h"
#include "ewl_filelist_view.h"
#include "ewl_filelist.h"
#include "ewl_icon_theme.h"
#include "ewl_icon.h"
#include "ewl_label.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

Ewl_Widget *ewl_filelist_view_widget_fetch(void *data,
			unsigned int row __UNUSED__, unsigned int column)
{
	Ewl_Widget *ret;
	const char *img, *stock, *filename;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(data, NULL);

	/* Create icon */
	ret = ewl_icon_simple_new();
	ewl_icon_constrain_set(EWL_ICON(ret), 32);
	ewl_box_orientation_set(EWL_BOX(ret),
			EWL_ORIENTATION_HORIZONTAL);
	ewl_object_alignment_set(EWL_OBJECT(ret), EWL_FLAG_ALIGN_LEFT);

	/* Get and set data into icon */
	if (column == 0)
	{
		stock = ewl_filelist_stock_icon_get(data);
		img = ewl_icon_theme_icon_path_get(stock, 0);
		if (img) ewl_icon_image_set(EWL_ICON(ret), 
						img, stock);

		filename = ecore_file_file_get(data);
		ewl_icon_label_set(EWL_ICON(ret), filename);
	}
	else
	{
		ewl_icon_label_set(EWL_ICON(ret), data);
	}
	
	FREE(data);
	ewl_widget_show(ret);

	DRETURN_PTR(ret, DLEVEL_STABLE);
}

Ewl_Widget *ewl_filelist_view_header_fetch(void *data __UNUSED__,
						unsigned int column)
{
	Ewl_Widget *l;
	char *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (column == 0) t = "Filename";
	else if (column == 1) t = "Size";
	else if (column == 2) t = "Permissions";
	else if (column == 3) t = "Username";
	else if (column == 4) t = "Groupname";
	else if (column == 5) t = "Modified";
	else t = "N/A";

	l = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(l), t);
	ewl_widget_show(l);

	DRETURN_PTR(l, DLEVEL_STABLE);
}
