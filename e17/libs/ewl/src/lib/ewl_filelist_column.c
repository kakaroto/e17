#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static Ewl_View *ewl_filelist_column_view = NULL;

static void ewl_filelist_column_cb_dir_clicked(Ewl_Widget *w, void *event, 
							void *data);
static void ewl_filelist_column_cb_file_clicked(Ewl_Widget *w, void *event, 
							void *data);
static void ewl_filelist_column_row_add(Ewl_Filelist *fl, const char *dir, 
							char *file);

/**
 * @return Returns the Ewl_View needed to display the filelist_column
 * @brief Retrieves the needed Ewl_View to use the column view
 */
Ewl_View *
ewl_filelist_column_view_get(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_filelist_column_view)
	{
		ewl_filelist_column_view = ewl_view_new();
		ewl_view_constructor_set(ewl_filelist_column_view, 
					ewl_filelist_column_new);
	}

	DRETURN_PTR(ewl_filelist_column_view, DLEVEL_STABLE);
}

/**
 * @return Returns a new Ewl_Filelist_Column or NULL on failure
 * @brief Creates a new Ewl_Filelist_Column widget
 */
Ewl_Widget *
ewl_filelist_column_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Filelist_Column, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_filelist_column_init(EWL_FILELIST_COLUMN(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Filelist_Column to default values
 */
int
ewl_filelist_column_init(Ewl_Filelist_Column *fl)
{
	Ewl_Filelist *list;
	Ewl_Widget *p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fl", fl, FALSE);

	if (!ewl_filelist_init(EWL_FILELIST(fl)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(fl), EWL_FILELIST_COLUMN_TYPE);
	ewl_widget_inherit(EWL_WIDGET(fl), EWL_FILELIST_COLUMN_TYPE);

	list = EWL_FILELIST(fl);
	list->dir_change = ewl_filelist_column_dir_change;
	list->filter_change = ewl_filelist_column_dir_change;

	p = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(fl), p);
	ewl_widget_show(p);

	fl->hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(p), fl->hbox);
	ewl_widget_show(fl->hbox);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fl: The filelist to work with
 * @return Returns no value
 * @brief The callback for a directory change
 */
void
ewl_filelist_column_dir_change(Ewl_Filelist *fl)
{
	const char *path;
	Ewl_Filelist_Column *list;
	Ewl_Container *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_COLUMN_TYPE);

	list = EWL_FILELIST_COLUMN(fl);
	c = EWL_CONTAINER(list->hbox);
	path = ewl_filelist_directory_get(EWL_FILELIST(fl));

	if (list->tree != NULL) 
	{
		Ewl_Widget *tree;
		int index;

		tree = ewl_widget_name_find(path);
		if (tree)
		{
			index = ewl_container_child_index_get(c, tree);

			/*
			 * Destroy columns following the current column.
			 */
			while ((tree = ewl_container_child_get(c, index + 1))) 
				ewl_widget_destroy(tree);
		}
	}

	list->tree = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(list->hbox), list->tree);
	ewl_widget_show(list->tree);

	ewl_filelist_directory_read(fl, TRUE, ewl_filelist_column_row_add);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_column_cb_dir_clicked(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Filelist_Column *fl;
	Ewl_Event_Mouse_Down *event;
	const char *dir;
	char *path;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev", ev);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fl = data;
	event = ev;

	/* only trigger on lmb */
	if (event->button != 1)
		DRETURN(DLEVEL_STABLE);

	dir = ewl_icon_label_get(EWL_ICON(w));
	path = ewl_filelist_expand_path(EWL_FILELIST(fl), dir);
	ewl_filelist_directory_set(EWL_FILELIST(fl), path);

	FREE(path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_column_cb_file_clicked(Ewl_Widget *w, void *ev, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev", ev);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_filelist_handle_click(EWL_FILELIST(data), w, ev, 
					"icon,select", "icon,unselect");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filelist_column_row_add(Ewl_Filelist *fl, const char *dir, char *file)
{
	Ewl_Filelist_Column *list;
	Ewl_Widget *icon;
	char *stock, path[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fl", fl);
	DCHECK_PARAM_PTR("dir", dir);
	DCHECK_PARAM_PTR("file", file);
	DCHECK_TYPE("fl", fl, EWL_FILELIST_TYPE);

	list = EWL_FILELIST_COLUMN(fl);
	snprintf(path, PATH_MAX, "%s/%s", dir, file);
	ewl_widget_name_set(list->tree, strdup(path));

	icon = ewl_icon_new();
	ewl_box_orientation_set(EWL_BOX(icon),
			EWL_ORIENTATION_HORIZONTAL);
	ewl_object_fill_policy_set(EWL_OBJECT(icon), EWL_FLAG_FILL_VSHRINK);
	ewl_icon_label_set(EWL_ICON(icon), file);

	if (ecore_file_is_dir(path))
	{
		stock = "/stock/open";
		ewl_callback_append(icon, EWL_CALLBACK_CLICKED,
				ewl_filelist_column_cb_dir_clicked, fl);
	}
	else
	{
		stock = "/stock/arrow/right";
		ewl_callback_append(icon, EWL_CALLBACK_CLICKED,
				ewl_filelist_column_cb_file_clicked, fl);
	}

	ewl_icon_image_set(EWL_ICON(icon), ewl_theme_path_get(),
			ewl_theme_data_str_get(EWL_WIDGET(icon), stock));

	ewl_container_child_append(EWL_CONTAINER(list->tree), icon);
	ewl_widget_show(icon);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

