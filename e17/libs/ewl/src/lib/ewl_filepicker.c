/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "ewl_base.h"
#include "ewl_filepicker.h"
#include "ewl_filelist.h"
#include "ewl_button.h"
#include "ewl_combo.h"
#include "ewl_entry.h"
#include "ewl_label.h"
#include "ewl_model.h"
#include "ewl_scrollpane.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

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

static void *ewl_filepicker_cb_type_fetch(void *data, unsigned int row,
						unsigned int col);
static unsigned int ewl_filepicker_cb_type_count(void *data);
static Ewl_Widget *ewl_filepicker_cb_type_header(void *data, unsigned int col);

static void ewl_filepicker_filter_free_cb(Ewl_Filelist_Filter *filter);

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

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(fp, FALSE);

	if (!ewl_box_init(EWL_BOX(fp)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_box_orientation_set(EWL_BOX(fp), EWL_ORIENTATION_VERTICAL);
	ewl_widget_inherit(EWL_WIDGET(fp), EWL_FILEPICKER_TYPE);
	ewl_widget_appearance_set(EWL_WIDGET(fp), EWL_FILEPICKER_TYPE);

	ewl_callback_prepend(EWL_WIDGET(fp), EWL_CALLBACK_DESTROY,
				ewl_filepicker_cb_destroy, NULL);

	ewl_object_preferred_inner_size_set(EWL_OBJECT(fp), 400, 300);

	fp->mvc_path.model = ewl_model_ecore_list_get();

	fp->mvc_path.view = ewl_label_view_get();

	fp->path = ecore_list_new();
	ecore_list_free_cb_set(fp->path, ECORE_FREE_CB(free));

	fp->mvc_path.combo = ewl_combo_new();
	ewl_container_child_append(EWL_CONTAINER(fp),
					fp->mvc_path.combo);
	ewl_mvc_model_set(EWL_MVC(fp->mvc_path.combo),
						fp->mvc_path.model);
	ewl_mvc_view_set(EWL_MVC(fp->mvc_path.combo),
						fp->mvc_path.view);
	ewl_mvc_data_set(EWL_MVC(fp->mvc_path.combo), fp->path);
	ewl_callback_append(fp->mvc_path.combo,
				EWL_CALLBACK_VALUE_CHANGED,
				ewl_filepicker_cb_path_change, fp);
	ewl_object_fill_policy_set(EWL_OBJECT(fp->mvc_path.combo),
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);
	ewl_widget_show(fp->mvc_path.combo);

	o = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(fp), o);
	ewl_widget_show(o);

	fp->favorites_box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(o), fp->favorites_box);
	ewl_object_fill_policy_set(EWL_OBJECT(fp->favorites_box),
				EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VFILL);
	ewl_filepicker_favorites_populate(fp);
	ewl_filepicker_show_favorites_set(fp, FALSE);
	
	fp->file_list = ewl_filelist_new();
	ewl_container_child_append(EWL_CONTAINER(o), fp->file_list);
	ewl_callback_append(EWL_WIDGET(fp->file_list),
				EWL_CALLBACK_VALUE_CHANGED,
				ewl_filepicker_cb_list_value_changed,
				fp);
	ewl_widget_show(fp->file_list);

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
	ecore_list_free_cb_set(fp->filters,
			ECORE_FREE_CB(ewl_filepicker_filter_free_cb));

	fp->mvc_filters.model = ewl_model_new();
	ewl_model_data_fetch_set(fp->mvc_filters.model,
				ewl_filepicker_cb_type_fetch);
	ewl_model_data_count_set(fp->mvc_filters.model,
				ewl_filepicker_cb_type_count);

	fp->mvc_filters.view = ewl_label_view_get();
	ewl_view_header_fetch_set(fp->mvc_filters.view,
				ewl_filepicker_cb_type_header);

	fp->mvc_filters.combo = ewl_combo_new();
	ewl_mvc_model_set(EWL_MVC(fp->mvc_filters.combo),
				fp->mvc_filters.model);
	ewl_mvc_view_set(EWL_MVC(fp->mvc_filters.combo),
				fp->mvc_filters.view);
	ewl_mvc_data_set(EWL_MVC(fp->mvc_filters.combo), fp);
	ewl_combo_editable_set(EWL_COMBO(fp->mvc_filters.combo), TRUE);
	ewl_callback_append(fp->mvc_filters.combo,
				EWL_CALLBACK_VALUE_CHANGED,
				ewl_filepicker_cb_type_change, fp);
	ewl_container_child_append(EWL_CONTAINER(box),
				fp->mvc_filters.combo);
	ewl_object_fill_policy_set(EWL_OBJECT(fp->mvc_filters.combo),
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);
	ewl_widget_show(fp->mvc_filters.combo);

	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(o), box);
	ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show(box);

	o = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_OK);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
				ewl_filepicker_cb_button_clicked, fp);
	ewl_object_fill_policy_set(EWL_OBJECT(o),
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_CANCEL);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
				ewl_filepicker_cb_button_clicked, fp);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show(o);
	
	/* Set up filelist */
	ewl_filepicker_filter_add(fp, "All files", NULL, NULL);
	ewl_filepicker_directory_set(fp, NULL);
	ewl_mvc_selected_set(EWL_MVC(fp->mvc_filters.combo),
			NULL, fp, 0, 0);

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
	DCHECK_PARAM_PTR(fp);
	DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

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
	DCHECK_PARAM_PTR_RET(fp, 0);
	DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, 0);

	DRETURN_INT(fp->show_favorites, DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to change
 * @param view: The view to use for the file list
 * @return Returns no value
 * @brief Sets the given view to be used to display the file list
 */
void
ewl_filepicker_list_view_set(Ewl_Filepicker *fp, Ewl_Filelist_View view)
{
	Ewl_Filelist *temp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(fp);
	DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);
	
	temp = EWL_FILELIST(fp->file_list);
	
	if (temp->view_flag == view)
		DRETURN(DLEVEL_STABLE);
	ewl_filelist_view_set(EWL_FILELIST(fp->file_list), view);
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the value from
 * @return Returns the current view set on the filepicker
 * @brief Retrieves the current view used for the file list
 */
Ewl_Filelist_View *
ewl_filepicker_list_view_get(Ewl_Filepicker *fp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(fp, NULL);
	DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, NULL);

	DRETURN_PTR(ewl_filelist_view_get(EWL_FILELIST(fp->file_list)),
							DLEVEL_STABLE);
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
	DCHECK_PARAM_PTR(fp);
	DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

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
	DCHECK_PARAM_PTR_RET(fp, NULL);
	DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, NULL);

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
ewl_filepicker_filter_set(Ewl_Filepicker *fp, Ewl_Filelist_Filter *filter)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(fp);
	DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

	ewl_filelist_filter_set(EWL_FILELIST(fp->file_list), filter);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fp: The filepicker to get the filter from
 * @return Returns the current filter used in the filepicker
 * @brief Retrieves the current filter used in the filepicker
 */
Ewl_Filelist_Filter *
ewl_filepicker_filter_get(Ewl_Filepicker *fp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(fp, NULL);
	DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, NULL);

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
	DCHECK_PARAM_PTR(fp);
	DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

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
	DCHECK_PARAM_PTR_RET(fp, FALSE);
	DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, FALSE);

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
	DCHECK_PARAM_PTR(fp);
	DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

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
	DCHECK_PARAM_PTR_RET(fp, FALSE);
	DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, FALSE);

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
	DCHECK_PARAM_PTR(fp);
	DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

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
	DCHECK_PARAM_PTR_RET(fp, NULL);
	DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, NULL);

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
	DCHECK_PARAM_PTR(fp);
	DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

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
	DCHECK_PARAM_PTR_RET(fp, NULL);
	DCHECK_TYPE_RET(fp, EWL_FILEPICKER_TYPE, NULL);

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
						const char *extension,
						Ecore_List *mime_types)
{
	Ewl_Filelist_Filter *f;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(fp);
	DCHECK_PARAM_PTR(name);
	DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

	f= NEW(Ewl_Filelist_Filter, 1);
	f->name = strdup(name);
	if (extension) f->extension = strdup(extension);
	else f->extension = NULL;
	if (mime_types) f->mime_list = mime_types;
	else f->mime_list = NULL;

	ecore_list_prepend(fp->filters, f);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_list_value_changed(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Filepicker *fp;
	Ewl_Filelist *fl;
	Ewl_Event_Action_Response *e;
	char *file;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(w);
	DCHECK_PARAM_PTR(data);

	fl = EWL_FILELIST(w);
	fp = data;
	e = ev;

	/* clear the text and get the selected file */
	ewl_text_clear(EWL_TEXT(fp->file_entry));
	file = ewl_filelist_selected_file_get(fl);

	if (e->response == EWL_FILELIST_EVENT_DIR_CHANGE)
	{
		char *dir;

		dir = strdup(ewl_filelist_directory_get(fl));
		ewl_filepicker_path_populate(fp, dir);
		FREE(dir);
	}
	else if ((e->response == EWL_FILELIST_EVENT_SELECTION_CHANGE) &&
					(!fl->multiselect))
		ewl_text_text_set(EWL_TEXT(fp->file_entry), file);

	else if (e->response == EWL_FILELIST_EVENT_MULTI_TRUE)
		ewl_widget_disable(EWL_WIDGET(fp->file_entry));

	else if (e->response == EWL_FILELIST_EVENT_MULTI_FALSE)
	{
		ewl_widget_enable(EWL_WIDGET(fp->file_entry));
		ewl_text_text_set(EWL_TEXT(fp->file_entry), file);
	}

	FREE(file);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_button_clicked(Ewl_Widget *w, void *ev __UNUSED__,
							void *data)
{
	Ewl_Filepicker *fp;
	Ewl_Stock *s;
	Ewl_Event_Action_Response e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(w);
	DCHECK_PARAM_PTR(data);
	DCHECK_TYPE(w, EWL_WIDGET_TYPE);

	fp = data;
	s = EWL_STOCK(w);

	e.response = ewl_stock_type_get(s);

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
	DCHECK_PARAM_PTR(fp);
	DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

	/* XXX Write me */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void *
ewl_filepicker_cb_type_fetch(void *data, unsigned int row,
				unsigned int col __UNUSED__)
{
	Ewl_Filepicker *fp;
	Ewl_Filelist_Filter *filter;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(data, NULL);

	fp = data;
	ecore_list_index_goto(fp->filters, row);
	filter = ecore_list_current(fp->filters);

	DRETURN_PTR(filter->name, DLEVEL_STABLE);
}

static unsigned int
ewl_filepicker_cb_type_count(void *data)
{
	Ewl_Filepicker *fp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(data, FALSE);

	fp = data;

	DRETURN_INT(ecore_list_count(fp->filters), DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_filepicker_cb_type_header(void *data, unsigned int col)
{
	Ewl_Filepicker *fp;
	Ewl_Filelist_Filter *filter;
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(data, NULL);

	fp = data;
	ecore_list_index_goto(fp->filters, col);
	filter = ecore_list_current(fp->filters);

	w = ewl_entry_new();
	if (filter)
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
	DCHECK_PARAM_PTR(fp);
	DCHECK_PARAM_PTR(path);
	DCHECK_TYPE(fp, EWL_FILEPICKER_TYPE);

	/* Clear the list and free the data first */
	while ((p = ecore_list_first_remove(fp->path)))
		FREE(p);

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

	ewl_mvc_dirty_set(EWL_MVC(fp->mvc_path.combo), TRUE);
	ewl_mvc_selected_set(EWL_MVC(fp->mvc_path.combo), NULL, fp->path, 0, 0);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_path_change(Ewl_Widget *w, void *ev __UNUSED__,
							void *data)
{
	Ewl_Filepicker *fp;
	Ewl_Selection_Idx *idx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(w);
	DCHECK_PARAM_PTR(data);
	DCHECK_TYPE(w, EWL_WIDGET_TYPE);

	fp = data;
	idx = ewl_mvc_selected_get(EWL_MVC(w));

	if (idx->row == 0)
	{
		free(idx);
		DRETURN(DLEVEL_STABLE);
	}

	ecore_list_index_goto(fp->path, idx->row);
	ewl_filepicker_directory_set(fp, ecore_list_current(fp->path));
	free(idx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_filter_change(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	Ewl_Filepicker *fp;
	char *name;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(w);
	DCHECK_PARAM_PTR(data);
	DCHECK_TYPE(w, EWL_WIDGET_TYPE);

	fp = data;
	name = ewl_text_text_get(EWL_TEXT(w));
	
	if (!name)
		DRETURN(DLEVEL_STABLE);
	
	ewl_filepicker_filter_add(fp, name, name, NULL);
	ewl_filepicker_cb_type_change(fp->mvc_filters.combo, NULL, fp);
	FREE(name);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_type_change(Ewl_Widget *w, void *ev __UNUSED__,
							void *data)
{
	Ewl_Filepicker *fp;
	Ewl_Filelist_Filter *filter;
	Ewl_Selection_Idx *idx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(w);
	DCHECK_PARAM_PTR(data);
	DCHECK_TYPE(w, EWL_WIDGET_TYPE);

	fp = data;
	idx = ewl_mvc_selected_get(EWL_MVC(w));
	if (idx)
	{
		ecore_list_index_goto(fp->filters, idx->row);
		filter = ecore_list_current(fp->filters);
		ewl_filepicker_filter_set(fp, filter);
	}
	
	FREE(idx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__,
					void *data __UNUSED__)
{
	Ewl_Filepicker *fp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(w);
	DCHECK_TYPE(w, EWL_WIDGET_TYPE);

	fp = EWL_FILEPICKER(w);

	IF_FREE(fp->mvc_path.model);
	IF_FREE(fp->mvc_path.view);
	IF_FREE(fp->mvc_filters.model);
	IF_FREE(fp->mvc_filters.view);
	IF_FREE_LIST(fp->path);
	IF_FREE_LIST(fp->filters);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_filepicker_filter_free_cb(Ewl_Filelist_Filter *filter)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!filter) DRETURN(DLEVEL_STABLE);

	IF_FREE(filter->name);
	IF_FREE(filter->extension);
	IF_FREE_LIST(filter->mime_list);

	FREE(filter);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



