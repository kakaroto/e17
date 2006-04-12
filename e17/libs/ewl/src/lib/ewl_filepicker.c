#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

typedef struct Ewl_Filepicker_Filter Ewl_Filepicker_Filter;
struct Ewl_Filepicker_Filter
{
	char *name;
	char *filter;
};

static void ewl_filepicker_favorites_populate(Ewl_Filepicker *fp);
static void ewl_filepicker_cb_list_value_changed(Ewl_Widget *w, void *ev,
								void *data);
static void ewl_filepicker_cb_button_clicked(Ewl_Widget *w, void *ev, 
							void *data);
static void ewl_filepicker_cb_destroy(Ewl_Widget *w, void *ev, void *data);

static void ewl_filepicker_path_populate(Ewl_Filepicker *fp, char *path);
static void ewl_filepicker_cb_path_change(Ewl_Widget *w, void *ev, 
							void *data);
static void ewl_filepicker_cb_filter_change(Ewl_Widget *w, void *ev, 
							void *data);
static void ewl_filepicker_cb_type_change(Ewl_Widget *w, void *ev, 
							void *data);

static void *ewl_filepicker_cb_path_fetch(void *data, unsigned int row,
						unsigned int col);
static int ewl_filepicker_cb_path_count(void *data);

static void *ewl_filepicker_cb_type_fetch(void *data, unsigned int row,
						unsigned int col);
static int ewl_filepicker_cb_type_count(void *data);
static Ewl_Widget *ewl_filepicker_cb_type_header(void *data, int col);

/**
 * @return Returns a new Ewl_Filepicker widget or NULL on failure
 * @brief Creates a new Ewl_Filepicker widget
 */
Ewl_Widget *
ewl_filepicker_new(void)
{
	Ewl_Widget *w = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Filepicker, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_filepicker_init(EWL_FILEPICKER(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialize an Ewl_Filepicker to default values
 */
int
ewl_filepicker_init(Ewl_Filepicker *fp)
{
	Ewl_Widget *o, *box;
	Ewl_Model *model;
	Ewl_View *view;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fp", fp, FALSE);

	if (!ewl_box_init(EWL_BOX(fp)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_box_orientation_set(EWL_BOX(fp), EWL_ORIENTATION_VERTICAL);
	ewl_widget_inherit(EWL_WIDGET(fp), EWL_FILEPICKER_TYPE);
	ewl_widget_appearance_set(EWL_WIDGET(fp), EWL_FILEPICKER_TYPE);

	ewl_callback_prepend(EWL_WIDGET(fp), EWL_CALLBACK_DESTROY,
				ewl_filepicker_cb_destroy, NULL);

	ewl_object_minimum_size_set(EWL_OBJECT(fp), 400, 300);

	model = ewl_model_new();
	ewl_model_fetch_set(model, ewl_filepicker_cb_path_fetch);
	ewl_model_count_set(model, ewl_filepicker_cb_path_count);

	view = ewl_view_new();
	ewl_view_constructor_set(view, ewl_label_new);
	ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_label_text_set));

	fp->path = ecore_list_new();

	fp->path_combo = ewl_combo_new();
	ewl_container_child_append(EWL_CONTAINER(fp), fp->path_combo);
	ewl_combo_model_set(EWL_COMBO(fp->path_combo), model);
	ewl_combo_view_set(EWL_COMBO(fp->path_combo), view);
	ewl_combo_data_set(EWL_COMBO(fp->path_combo), fp->path);
	ewl_callback_append(fp->path_combo, EWL_CALLBACK_VALUE_CHANGED,
					ewl_filepicker_cb_path_change, fp);
	ewl_widget_show(fp->path_combo);

	o = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(fp), o);
	ewl_widget_show(o);

	fp->favorites_box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(o), fp->favorites_box);
	ewl_object_fill_policy_set(EWL_OBJECT(fp->favorites_box),
				EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VFILL);
	ewl_filepicker_favorites_populate(fp);
	ewl_filepicker_show_favorites_set(fp, FALSE);

	fp->file_list_box = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(o), fp->file_list_box);
	ewl_widget_show(fp->file_list_box);

	o = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(fp), o);
	ewl_widget_show(o);

	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(o), box);
	ewl_object_fill_policy_set(EWL_OBJECT(o), 
			EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);
	ewl_widget_show(box);

	fp->file_entry = ewl_entry_new();
	ewl_container_child_append(EWL_CONTAINER(box), fp->file_entry);

	ewl_widget_show(fp->file_entry);

	fp->filters = ecore_list_new();
	ewl_filepicker_filter_add(fp, "All files", NULL);

	model = ewl_model_new();
	ewl_model_fetch_set(model, ewl_filepicker_cb_type_fetch);
	ewl_model_count_set(model, ewl_filepicker_cb_type_count);

	view = ewl_view_new();
	ewl_view_constructor_set(view, ewl_label_new);
	ewl_view_assign_set(view, EWL_VIEW_ASSIGN(ewl_label_text_set));
	ewl_view_header_fetch_set(view, ewl_filepicker_cb_type_header);

	fp->type_combo = ewl_combo_new();
	ewl_combo_model_set(EWL_COMBO(fp->type_combo), model);
	ewl_combo_view_set(EWL_COMBO(fp->type_combo), view);
	ewl_combo_data_set(EWL_COMBO(fp->type_combo), fp);
	ewl_combo_editable_set(EWL_COMBO(fp->type_combo), TRUE);
	ewl_callback_append(fp->type_combo, EWL_CALLBACK_VALUE_CHANGED,
					ewl_filepicker_cb_type_change, fp);
	ewl_container_child_append(EWL_CONTAINER(box), fp->type_combo);
	ewl_combo_selected_set(EWL_COMBO(fp->type_combo), 0);
	ewl_widget_show(fp->type_combo);

	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(o), box);
	ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show(box);

	o = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_button_stock_type_set(EWL_BUTTON(o), EWL_STOCK_OK);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
				ewl_filepicker_cb_button_clicked, fp);
	ewl_object_fill_policy_set(EWL_OBJECT(o), 
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_button_stock_type_set(EWL_BUTTON(o), EWL_STOCK_CANCEL);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
				ewl_filepicker_cb_button_clicked, fp);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show(o);

	/* do this last so all the other widgets are set correctly */
	ewl_filepicker_list_view_set(fp, ewl_filelist_icon_view_get());
	ewl_filepicker_directory_set(fp, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to change
 * @param show: The show favorites setting to apply
 * @return Returns no value
 * @brief Sets the show favorites setting of the file picker 
 */
void
ewl_filepicker_show_favorites_set(Ewl_Filepicker *fp, unsigned int show)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fp", fp);
	DCHECK_TYPE("fp", fp, EWL_FILEPICKER_TYPE);

	if (show == fp->show_favorites)
		DRETURN(DLEVEL_STABLE);

	fp->show_favorites = !!show;
	if (fp->show_favorites)
		ewl_widget_show(fp->favorites_box);
	else
		ewl_widget_hide(fp->favorites_box);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the show favorites value from
 * @return Returns the current show favorites setting of the filepicker
 * @brief Retrieves the current show favorites setting of the filepicker
 */
unsigned int
ewl_filepicker_show_favorites_get(Ewl_Filepicker *fp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fp", fp, 0);
	DCHECK_TYPE_RET("fp", fp, EWL_FILEPICKER_TYPE, 0);

	DRETURN_INT(fp->show_favorites, DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to change
 * @param view: The view to use for the file list
 * @return Returns no value
 * @brief Sets the given view to be used to display the file list
 */
void
ewl_filepicker_list_view_set(Ewl_Filepicker *fp, Ewl_View *view)
{
	Ewl_Filelist *old_fl;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fp", fp);
	DCHECK_PARAM_PTR("view", view);
	DCHECK_TYPE("fp", fp, EWL_FILEPICKER_TYPE);

	if (view == fp->view)
		DRETURN(DLEVEL_STABLE);

	fp->view = view;
	old_fl = EWL_FILELIST(fp->file_list);

	/* set the new view */
	fp->file_list = view->construct();
	ewl_container_child_append(EWL_CONTAINER(fp->file_list_box),
							fp->file_list);
	ewl_callback_append(EWL_WIDGET(fp->file_list), 
				EWL_CALLBACK_VALUE_CHANGED,
				ewl_filepicker_cb_list_value_changed, fp);

	/* load new view from old view values */
	if (old_fl)
	{
		ewl_filepicker_directory_set(fp, 
					ewl_filelist_directory_get(old_fl));
		ewl_filepicker_filter_set(fp, 
					ewl_filelist_filter_get(old_fl));
		ewl_filepicker_multiselect_set(fp, 
					ewl_filelist_multiselect_get(old_fl));
		ewl_filepicker_show_dot_files_set(fp, 
					ewl_filelist_show_dot_files_get(old_fl));
		ewl_filepicker_selected_files_set(fp, 
					ewl_filelist_selected_files_get(old_fl));
		ewl_widget_destroy(EWL_WIDGET(old_fl));
	}
	ewl_widget_show(fp->file_list);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the value from
 * @return Returns the current view set on the filepicker
 * @brief Retrieves the current view used for the file list 
 */
Ewl_View *
ewl_filepicker_list_view_get(Ewl_Filepicker *fp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fp", fp, NULL);
	DCHECK_TYPE_RET("fp", fp, EWL_FILEPICKER_TYPE, NULL);

	DRETURN_PTR(fp->view, DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to set the directory on
 * @param dir: The directory to set
 * @return Returns no value.
 * @brief Set the current directory to be displayed in the filepicker
 */
void
ewl_filepicker_directory_set(Ewl_Filepicker *fp, const char *dir)
{
	char *tmp;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fp", fp);
	DCHECK_TYPE("fp", fp, EWL_FILEPICKER_TYPE);

	/* if no directory is given try to default to the HOME directory,
	 * and if that failes try /tmp and if that fails try / */
	if (!dir)
	{
		char *t;

		t = getenv("HOME");
		if (t) tmp = strdup(t);
		else tmp = strdup("/tmp");

		if (!ecore_file_exists(tmp))
		{
			FREE(tmp);
			tmp = strdup("/");
		}
	}
	else
		tmp = strdup(dir);

	ewl_filepicker_path_populate(fp, tmp);
	ewl_filelist_directory_set(EWL_FILELIST(fp->file_list), tmp);
	FREE(tmp);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the directory from
 * @return Returns the current directory displayed in the file dialog
 * @brief Retrieve the current directory from the filepicker
 */
const char *
ewl_filepicker_directory_get(Ewl_Filepicker *fp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fp", fp, NULL);
	DCHECK_TYPE_RET("fp", fp, EWL_FILEPICKER_TYPE, NULL);

	DRETURN_PTR(ewl_filelist_directory_get(EWL_FILELIST(fp->file_list)),
							DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to set the filter on
 * @param filter: The filter to set on the filepicker
 * @return Returns no value.
 * @brief Set the given filter into the file picker 
 */
void
ewl_filepicker_filter_set(Ewl_Filepicker *fp, const char *filter)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fp", fp);
	DCHECK_TYPE("fp", fp, EWL_FILEPICKER_TYPE);

	ewl_filelist_filter_set(EWL_FILELIST(fp->file_list), filter);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the filter from
 * @return Returns the current filter used in the filepicker
 * @brief Retrieves the current filter used in the filepicker
 */
const char *
ewl_filepicker_filter_get(Ewl_Filepicker *fp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fp", fp, NULL);
	DCHECK_TYPE_RET("fp", fp, EWL_FILEPICKER_TYPE, NULL);

	DRETURN_PTR(ewl_filelist_filter_get(EWL_FILELIST(fp->file_list)),
							DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to set the multi-select flag
 * @param ms: The multi-select setting to place on the filepicker
 * @return Returns no value.
 * @brief Sets the multiselect state of the filepicker to the given value
 */
void
ewl_filepicker_multiselect_set(Ewl_Filepicker *fp, unsigned int ms)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fp", fp);
	DCHECK_TYPE("fp", fp, EWL_FILEPICKER_TYPE);

	ewl_filelist_multiselect_set(EWL_FILELIST(fp->file_list), ms);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the multiselect value from
 * @return Returns the current multi-select value of the filepicker
 * @brief Retrieves the current multi-select value from the filepicker
 */
unsigned int
ewl_filepicker_multiselect_get(Ewl_Filepicker *fp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fp", fp, 0);
	DCHECK_TYPE_RET("fp", fp, EWL_FILEPICKER_TYPE, 0);

	DRETURN_INT(ewl_filelist_multiselect_get(EWL_FILELIST(fp->file_list)),
								DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to set the value into
 * @param dot: The show dot files setting to set.
 * @return Returns no value.
 * @brief Set the show dot files setting of the filepicker to the given
 * value 
 */
void
ewl_filepicker_show_dot_files_set(Ewl_Filepicker *fp, unsigned int dot)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fp", fp);
	DCHECK_TYPE("fp", fp, EWL_FILEPICKER_TYPE);

	ewl_filelist_show_dot_files_set(EWL_FILELIST(fp->file_list), dot);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the showdot files setting from
 * @return Returns the current show dot files setting of the filepicker
 * @brief Retrieves the current show dot files setting of the filepicker
 */
unsigned int
ewl_filepicker_show_dot_files_get(Ewl_Filepicker *fp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fp", fp, 0);
	DCHECK_TYPE_RET("fp", fp, EWL_FILEPICKER_TYPE, 0);

	DRETURN_INT(ewl_filelist_show_dot_files_get(EWL_FILELIST(fp->file_list)),
								DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to set the selected file into
 * @param file: The file to set selected in the filepicker
 * @return Returns no value
 * @brief Sets the given file as selected in the filepicker
 */
void
ewl_filepicker_selected_file_set(Ewl_Filepicker *fp, const char *file)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fp", fp);
	DCHECK_TYPE("fp", fp, EWL_FILEPICKER_TYPE);

	ewl_filelist_selected_file_set(EWL_FILELIST(fp->file_list), file);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the selected file from
 * @return Returns the currently selected file in the filepicker
 * @brief Retrieves the currently selected file from the filepicker
 */
char *
ewl_filepicker_selected_file_get(Ewl_Filepicker *fp)
{
	char *file;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fp", fp, NULL);
	DCHECK_TYPE_RET("fp", fp, EWL_FILEPICKER_TYPE, NULL);

	/* we try to grab whats in the text entry first as the user may have
	 * changed the selected name. Otherwise we try to grab whats in the
	 * selected array from the file list */
	file = ewl_text_text_get(EWL_TEXT(fp->file_entry));
	if (!file) 
		file = ewl_filelist_selected_file_get(EWL_FILELIST(fp->file_list));

	DRETURN_PTR(file, DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to set the selected files into
 * @param files: The Ecore_List of selected files to set into the filepicker
 * @return Returns no value
 * @brief Sets the given files as selected in the filepicker
 */
void
ewl_filepicker_selected_files_set(Ewl_Filepicker *fp, Ecore_List *files)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fp", fp);
	DCHECK_TYPE("fp", fp, EWL_FILEPICKER_TYPE);

	ewl_filelist_selected_files_set(EWL_FILELIST(fp->file_list), files);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to work with
 * @return Returns the Ecore_List of selected files in the filepicker
 * @brief Retrieves the list of selected files in the filepicker
 */
Ecore_List *
ewl_filepicker_selected_files_get(Ewl_Filepicker *fp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fp", fp, NULL);
	DCHECK_TYPE_RET("fp", fp, EWL_FILEPICKER_TYPE, NULL);

	DRETURN_PTR(ewl_filelist_selected_files_get(EWL_FILELIST(fp->file_list)),
								DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to work with
 * @param name: The name to display for the filter
 * @param filter: The actual regular expression for the filter
 * @return Returns no value
 * @brief Add the filter named @a name to the combo box in the filepicker.
 */
void
ewl_filepicker_filter_add(Ewl_Filepicker *fp, const char *name,
						const char *filter)
{
	Ewl_Filepicker_Filter *f;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fp", fp);
	DCHECK_PARAM_PTR("name", name);
	DCHECK_TYPE("fp", fp, EWL_FILEPICKER_TYPE);

	f= NEW(Ewl_Filepicker_Filter, 1);
	f->name = strdup(name);
	f->filter = (filter ? strdup(filter) : NULL);

	ecore_list_prepend(fp->filters, f);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_list_value_changed(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Filepicker *fp;
	Ewl_Filelist *fl;
	Ewl_Filelist_Event *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("data", data);

	fl = EWL_FILELIST(w);
	fp = data;
	e = ev;

	/* XXX need to correctly deal with multiselect files in here */
	if (e->type == EWL_FILELIST_EVENT_TYPE_DIR_CHANGE)
	{
		char *dir;

		dir = strdup(ewl_filelist_directory_get(fl));
		ewl_filepicker_path_populate(fp, dir);
		ewl_text_text_set(EWL_TEXT(fp->file_entry), NULL);

		FREE(dir);
	}
	else if (e->type == EWL_FILELIST_EVENT_TYPE_SELECTION_CHANGE)
		ewl_text_text_set(EWL_TEXT(fp->file_entry), 
				ewl_filelist_selected_file_get(fl));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_button_clicked(Ewl_Widget *w, void *ev __UNUSED__, 
							void *data)
{
	Ewl_Filepicker *fp;
	Ewl_Button *b;
	Ewl_Filepicker_Event e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fp = data;
	b = EWL_BUTTON(w);

	e.response = ewl_button_stock_type_get(b);

	if (e.response == EWL_STOCK_CANCEL)
	{
		ewl_text_text_set(EWL_TEXT(fp->file_entry), NULL);
		ewl_filepicker_selected_file_set(fp, NULL);
	}
	ewl_callback_call_with_event_data(EWL_WIDGET(fp),
					EWL_CALLBACK_VALUE_CHANGED, &e);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_favorites_populate(Ewl_Filepicker *fp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fp", fp);
	DCHECK_TYPE("fp", fp, EWL_FILEPICKER_TYPE);

	/* XXX Write me */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void *
ewl_filepicker_cb_path_fetch(void *data, unsigned int row, 
				unsigned int col __UNUSED__)
{
	Ecore_List *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, NULL);

	list = data;
	ecore_list_goto_index(list, row);

	DRETURN_PTR(ecore_list_current(list), DLEVEL_STABLE);
}

static int
ewl_filepicker_cb_path_count(void *data)
{
	Ecore_List *list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, 0);

	list = data;

	DRETURN_INT(ecore_list_nodes(list), DLEVEL_STABLE);
}

static void *
ewl_filepicker_cb_type_fetch(void *data, unsigned int row, 
				unsigned int col __UNUSED__)
{
	Ewl_Filepicker *fp;
	Ewl_Filepicker_Filter *filter;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, NULL);

	fp = data;
	ecore_list_goto_index(fp->filters, row);
	filter = ecore_list_current(fp->filters);

	DRETURN_PTR(filter->name, DLEVEL_STABLE);
}

static int
ewl_filepicker_cb_type_count(void *data)
{
	Ewl_Filepicker *fp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, 0);

	fp = data;

	DRETURN_INT(ecore_list_nodes(fp->filters), DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_filepicker_cb_type_header(void *data, int col)
{
	Ewl_Filepicker *fp;
	Ewl_Filepicker_Filter *filter;
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, NULL);

	fp = data;
	ecore_list_goto_index(fp->filters, col);
	filter = ecore_list_current(fp->filters);

	w = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(w), filter->name);
	ewl_callback_append(w, EWL_CALLBACK_VALUE_CHANGED,
				ewl_filepicker_cb_filter_change, fp);
	ewl_widget_show(w);

	DRETURN_PTR(w, DLEVEL_STABLE);
}

static void
ewl_filepicker_path_populate(Ewl_Filepicker *fp, char *path)
{
	char *p, *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fp", fp);
	DCHECK_PARAM_PTR("path", path);
	DCHECK_TYPE("fp", fp, EWL_FILEPICKER_TYPE);

	ecore_list_clear(fp->path);
	ecore_list_append(fp->path, strdup("/"));

	p = path;
	while ((t = strchr((p + 1), '/')))
	{
		*t = '\0';

		ecore_list_prepend(fp->path, strdup(path));

		*t = '/';
		p = t;
	}

	/* do we have more stuff? if so, make sure we get the last path */
	if (p && (*(p + 1) != '\0'))
		ecore_list_prepend(fp->path, strdup(path));

	ewl_combo_dirty_set(EWL_COMBO(fp->path_combo), TRUE);
	ewl_combo_selected_set(EWL_COMBO(fp->path_combo), 0);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_path_change(Ewl_Widget *w, void *ev __UNUSED__, 
							void *data)
{
	Ewl_Filepicker *fp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fp = data;
	ecore_list_goto_index(fp->path,
			ewl_combo_selected_get(EWL_COMBO(w)));
	ewl_filepicker_directory_set(fp, ecore_list_current(fp->path));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_filter_change(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Filepicker *fp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fp = data;
	ewl_filepicker_filter_set(fp, ewl_text_text_get(EWL_TEXT(w)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_type_change(Ewl_Widget *w, void *ev __UNUSED__, 
							void *data)
{
	Ewl_Filepicker *fp;
	Ewl_Filepicker_Filter *filter;
	int idx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fp = data;
	idx = ewl_combo_selected_get(EWL_COMBO(w));
	if (idx > -1)
	{
		ecore_list_goto_index(fp->filters, idx);
		filter = ecore_list_current(fp->filters);
		ewl_filepicker_filter_set(fp, filter->filter);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_destroy(Ewl_Widget *w, void *ev, void *data)
{
	char *file;
	Ewl_Filepicker *fp;
	Ewl_Filepicker_Filter *filter;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fp = EWL_FILEPICKER(w);

	while ((file = ecore_list_remove_first(fp->path)))
		FREE(file);

	while ((filter = ecore_list_remove_first(fp->filters)))
	{
		FREE(filter->name);
		IF_FREE(filter->filter);
		FREE(filter);
	}

	ecore_list_destroy(fp->path);
	ecore_list_destroy(fp->filters);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



