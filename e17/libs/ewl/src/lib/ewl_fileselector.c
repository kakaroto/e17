#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

#define EWL_FS_TREE_WIDTH 500
#define EWL_FS_TREE_HEIGHT 300

/*
 * Internally used functions
 */
static void ewl_fileselector_tooltip_add(Ewl_Widget *w, 
					Ewl_Fileselector_Data *d);
static char *ewl_fileselector_str_append(const char *s1, const char *s2);
static char *ewl_fileselector_path_up_get(char *path);
static char *ewl_fileselector_path_home_get(void);
static char *ewl_fileselector_size_string_get(off_t st_size);
static char *ewl_fileselector_perm_string_get(mode_t st_mode);
static void ewl_fileselector_file_list_get(char *path, char *filter,
					   char *dfilter,
					   Ecore_List *flist,
					   Ecore_List *dlist);
static void ewl_fileselector_path_setup(Ewl_Fileselector *fs, const char *path);
static void ewl_fileselector_show_cb(Ewl_Widget *entry, void *ev_data,
					void *user_data);
static void ewl_fileselector_destroy_cb(Ewl_Widget *entry, void *ev_data,
					void *user_data);

static void ewl_fileselector_files_free_cb(void *data);

/**
 * @return Returns NULL on failure, or the new fileselector on success.
 * @brief Create a new fileselector
 */
Ewl_Widget *
ewl_fileselector_new(void)
{
	Ewl_Fileselector *fs;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fs = NEW(Ewl_Fileselector, 1);
	if (!fs) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_fileselector_init(fs)) {
		ewl_widget_destroy(EWL_WIDGET(fs));
		fs = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(fs), DLEVEL_STABLE);
}

/**
 * @param fs: the fileselector
 * @return Returns no value.
 * @brief Initalize the fileselector
 */
int
ewl_fileselector_init(Ewl_Fileselector *fs)
{
	char *tmp;
	Ewl_Widget *w, *hbox, *misc, *button;

	char *head_dirs[1] = { "Directories" };
	char *head_files[1] = { "Files" };

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fs", fs, FALSE);

	w = EWL_WIDGET(fs);

	if (!ewl_box_init(EWL_BOX(w)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_box_orientation_set(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_widget_appearance_set(w, EWL_FILESELECTOR_TYPE);
	ewl_widget_inherit(w, EWL_FILESELECTOR_TYPE);
	ewl_object_fill_policy_set(EWL_OBJECT(w),
				   EWL_FLAG_FILL_SHRINK | EWL_FLAG_FILL_FILL);
	ewl_object_minimum_size_set(EWL_OBJECT(w), EWL_FS_TREE_WIDTH,
				    EWL_FS_TREE_HEIGHT);

	ewl_callback_append(w, EWL_CALLBACK_SHOW,
			    ewl_fileselector_show_cb, NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
			    ewl_fileselector_destroy_cb, NULL);

	/* The entry for the current directory */
	/* and some icons */
	hbox = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(hbox),
				   EWL_FLAG_FILL_SHRINK | EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(fs), hbox);
	ewl_widget_show(hbox);

	fs->entry_dir = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(fs->entry_dir), NULL);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox),
				   EWL_FLAG_FILL_SHRINK | EWL_FLAG_FILL_HFILL);
	ewl_container_child_prepend(EWL_CONTAINER(hbox), fs->entry_dir);
	ewl_widget_show(fs->entry_dir);

	misc = ewl_spacer_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), misc);
	ewl_object_fill_policy_set(EWL_OBJECT(misc), EWL_FLAG_FILL_FILL);
	ewl_widget_show(misc);

	button = ewl_button_new();
	ewl_button_stock_type_set(EWL_BUTTON(button), EWL_STOCK_ARROW_UP);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED,
			    ewl_fileselector_go_up_cb, fs);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_widget_show(button);

	button = ewl_button_new();
	ewl_button_stock_type_set(EWL_BUTTON(button), EWL_STOCK_HOME);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED,
			    ewl_fileselector_go_home_cb, fs);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_widget_show(button);

	/* The lists for directories and files */
	hbox = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(hbox),
				   EWL_FLAG_FILL_SHRINK | EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(fs), hbox);
	ewl_widget_show(hbox);

	fs->list_dirs = ewl_tree_new(1);
	ewl_tree_headers_set(EWL_TREE(fs->list_dirs), head_dirs);
	ewl_object_fill_policy_set(EWL_OBJECT(fs->list_dirs),
				   EWL_FLAG_FILL_SHRINK | EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(hbox), fs->list_dirs);
	ewl_widget_show(fs->list_dirs);

	fs->list_files = ewl_tree_new(1);
	ewl_tree_headers_set(EWL_TREE(fs->list_files), head_files);
	ewl_object_fill_policy_set(EWL_OBJECT(fs->list_files),
				   EWL_FLAG_FILL_SHRINK |
				   EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(hbox),
				   fs->list_files);
	ewl_widget_show(fs->list_files);

	/* The file label and entry */
	hbox = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(hbox),
				   EWL_FLAG_FILL_SHRINK | EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(fs), hbox);
	ewl_widget_show(hbox);

	misc = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(misc), "File:");
	ewl_object_fill_policy_set(EWL_OBJECT(misc), EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append(EWL_CONTAINER(hbox), misc);
	ewl_widget_show(misc);

	fs->entry_file = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(fs->entry_file), NULL);
	ewl_container_child_append(EWL_CONTAINER(hbox), fs->entry_file);
	ewl_widget_show(fs->entry_file);

	/* The default filter values */
	fs->ffilter = strdup("^[^\\.]");
	fs->dfilter = strdup("^[^\\.]");

	tmp = getenv("HOME");
	fs->path = strdup((tmp ? tmp : "/"));

	fs->files = ecore_list_new();
	ecore_list_set_free_cb(fs->files, ewl_fileselector_files_free_cb);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fs: the fileselector
 * @return Returns the current path of fileselector
 * @brief Retrieve the current fileselector path
 */
char *
ewl_fileselector_path_get(Ewl_Fileselector *fs)
{
	char *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fs", fs, NULL);
	DCHECK_TYPE_RET("fs", fs, EWL_FILESELECTOR_TYPE, NULL);

	s = strdup(fs->path);

	DRETURN_PTR(s, DLEVEL_STABLE);
}

/**
 * @param fs: the fileselector
 * @return Returns the selected filename including its path
 * @brief Retrieve the selected filename
 */
char *
ewl_fileselector_file_get(Ewl_Fileselector *fs)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fs", fs, NULL);
	DCHECK_TYPE_RET("fs", fs, EWL_FILESELECTOR_TYPE, NULL);

	if (ecore_list_is_empty(fs->files)) {
		char *entry_file;

		entry_file = ewl_text_text_get(EWL_TEXT(fs->entry_file));
		if (entry_file && fs->path) {
			char *f2;

			f2 = ewl_fileselector_str_append(fs->path, entry_file);
			ecore_list_append(fs->files, f2);
			FREE(entry_file);
		}
	}

	if (ecore_list_is_empty(fs->files)) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(ecore_list_goto_first(fs->files), DLEVEL_STABLE);
}

/**
 * @param fs: The fileselector
 * @param path: the directory to process
 * @return Returns nothing
 * @brief Prosess the given directory / change the fileselector dir
 */
void
ewl_fileselector_path_set(Ewl_Fileselector *fs, const char *path)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fs", fs);
	DCHECK_PARAM_PTR("path", path);
	DCHECK_TYPE("fs", fs, EWL_FILESELECTOR_TYPE);

	if (REALIZED(fs))
		ewl_fileselector_path_setup(fs, path);
	else
		fs->path = strdup(path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fs: The fileselector
 * @param val: Set selector multiselect (0|1)
 * @return Returns no value
 * @brief Sets the selector multi or single select
 */
void
ewl_fileselector_multiselect_set(Ewl_Fileselector *fs, unsigned int val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fs", fs);
	DCHECK_TYPE("fs", fs, EWL_FILESELECTOR_TYPE);

	if (val > 1) val = 1;
	fs->multi_select = val;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fs: The fileselector
 * @return Returns the multiselect status of the selector
 * @brief Gets the multiselect status of the selector
 */
unsigned int
ewl_fileselector_multiselect_get(Ewl_Fileselector *fs)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fs", fs, 0);
	DCHECK_TYPE_RET("fs", fs, EWL_FILESELECTOR_TYPE, 0);

	DRETURN_INT(fs->multi_select, DLEVEL_STABLE);
}

/**
 * @param fs: The fileselector
 * @return Returns the selections
 * @brief Returns the files selected in the selector
 */
Ecore_List *
ewl_fileselector_select_list_get(Ewl_Fileselector *fs)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fs", fs, NULL);
	DCHECK_TYPE_RET("fs", fs, EWL_FILESELECTOR_TYPE, NULL);

	DRETURN_PTR(fs->files, DLEVEL_STABLE);
}

/*
 * Internally used functions
 */

/* if non NULL, result must be freed */
static char *
ewl_fileselector_str_append(const char *s1, const char *s2)
{
	char *s;
	int l;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s1", s1, NULL);
	DCHECK_PARAM_PTR_RET("s2", s2, NULL);

	l = strlen(s1) + strlen(s2) + 1;

	s = NEW(char, l);
	s = memcpy(s, s1, strlen(s1));
	memcpy(s + strlen(s1), s2, strlen(s2));
	s[l - 1] = '\0';

	DRETURN_PTR(s, DLEVEL_STABLE);;
}

/* if non NULL, result must be freed */
static char *ewl_fileselector_path_up_get(char *path)
{
	char *new_path;
	int l;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("path", path, NULL);

	l = strlen(path);
	if (l == 0)
		DRETURN_PTR(NULL, DLEVEL_STABLE);;

	l--;
	if (path[l] == '/')
		l--;

	while ((l >= 0) && (path[l] != '/'))
		l--;

	if (l < 0)  {
		new_path = strdup("/");
	} else {
		l++;
		new_path = (char *) malloc(sizeof(char) * (l + 1));
		new_path = memcpy(new_path, path, l);
		new_path[l] = '\0';
	}

	DRETURN_PTR(new_path, DLEVEL_STABLE);
}

/* if non NULL, result must be freed */
static char *
ewl_fileselector_path_home_get(void)
{
	char *path;
	char *new_path;

	DENTER_FUNCTION(DLEVEL_STABLE);

	path = getenv("HOME");
	if ((!path) || (strlen(path) == 0))
		path = strdup("/");

	if (path[strlen(path)] != '/')
		new_path = ewl_fileselector_str_append(path, "/");
	else
		new_path = path;

	DRETURN_PTR(new_path, DLEVEL_STABLE);
}

static char *
ewl_fileselector_size_string_get(off_t st_size)
{
	double dsize;
	char size[1024];

	DENTER_FUNCTION(DLEVEL_STABLE);

	dsize = (double) st_size;
	if (dsize < 1024)
		sprintf(size, "%'.0f B", dsize);
	else {
		dsize /= 1024.0;
		if (dsize < 1024)
			sprintf(size, "%'.1f KB", dsize);
		else {
			dsize /= 1024.0;
			if (dsize < 1024)
				sprintf(size, "%'.1f MB", dsize);
			else {
				dsize /= 1024.0;
				sprintf(size, "%'.1f GB", dsize);
			}
		}
	}

	DRETURN_PTR(strdup(size), DLEVEL_STABLE);
}

static char *
ewl_fileselector_perm_string_get(mode_t st_mode)
{
	char *perm;
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);

	perm = (char *) malloc(sizeof(char) * 10);

	for (i = 0; i < 9; i++)
		perm[i] = '-';
	perm[9] = '\0';

	if ((S_IRUSR & st_mode) == S_IRUSR)
		perm[0] = 'r';
	if ((S_IWUSR & st_mode) == S_IWUSR)
		perm[1] = 'w';
	if ((S_IXUSR & st_mode) == S_IXUSR)
		perm[2] = 'x';

	if ((S_IRGRP & st_mode) == S_IRGRP)
		perm[3] = 'r';
	if ((S_IWGRP & st_mode) == S_IWGRP)
		perm[4] = 'w';
	if ((S_IXGRP & st_mode) == S_IXGRP)
		perm[5] = 'x';

	if ((S_IROTH & st_mode) == S_IROTH)
		perm[6] = 'r';
	if ((S_IWOTH & st_mode) == S_IWOTH)
		perm[7] = 'w';
	if ((S_IXOTH & st_mode) == S_IXOTH)
		perm[8] = 'x';

	DRETURN_PTR(perm, DLEVEL_STABLE);
}

static void
ewl_fileselector_file_list_get(char *path, char *filter,
				char *dfilter, Ecore_List *flist,
				Ecore_List *dlist)
{
	regex_t freg, dreg;
	Ewl_Fileselector_Data *d;
	struct stat buf;
	char *name;
	char *path2;
	int len;
	Ecore_List *file_list = NULL;
	char *listing = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("path", path);
	DCHECK_PARAM_PTR("filter", filter);
	DCHECK_PARAM_PTR("dfilter", dfilter);
	DCHECK_PARAM_PTR("flist", flist);
	DCHECK_PARAM_PTR("dlist", dlist);

	if (filter) {
		if (regcomp(&freg, filter, REG_NOSUB | REG_EXTENDED))
			filter = NULL;
	}

	if (dfilter) {
		if (regcomp(&dreg, dfilter, REG_NOSUB | REG_EXTENDED))
			dfilter = NULL;
	}

	/* Check if path is finished by a / and add it if there's none */
	if (path[strlen(path) - 1] == '/')
		path2 = strdup(path);
	else {
		path2 = (char *) malloc(sizeof(char) * (strlen(path) + 2));
		memcpy(path2, path, strlen(path));
		path2[strlen(path)] = '/';
		path2[strlen(path) + 1] = '\0';
	}
	
	/* Get directory listing, store, and move to beginning of list */
	file_list = ecore_file_ls(path2);
	ecore_list_goto_first(file_list);

	/* Loop over all listings in the directory to build both lists */
	while ( (!ecore_list_is_empty(file_list)) &&
		(!(ecore_list_index(file_list) >= (ecore_list_nodes(file_list)))) && 
		(listing = strdup((char *)ecore_list_next(file_list))) ) {
		
		int match = 0;
		Ecore_List *add = NULL;
		regex_t *reg;
		
		/* Setup a ful path copy to the listing */
		len = strlen(path2) + strlen(listing) + 1; 	
		name = (char *) malloc(sizeof(char) * len); 	
		memcpy(name, path2, strlen(path2)); 
		memcpy(name + strlen(path2),
	        listing, strlen(listing)); 
		name[len - 1] = '\0'; 

		/* Set to NULL before determining the type of the listing */
		reg = NULL;
		
		/* Determine type of listing */
		if(stat(name, &buf) == 0) {
			if(ecore_file_is_dir(name)) {
				add = dlist;
				if (dfilter)
					reg = &dreg;
			}
			else {
				add = flist;
				if (filter)
					reg = &freg;
			}
		}

		/* Determine if this item should be listed */
		if (!strcmp(listing, ".."))
			match = 1;
		else if (reg && !regexec(reg, listing, 0, NULL, 0))
			match = 1;
	
		/* File matches so add it to the listing */
		if (match && add) {
			d = ewl_fileselector_data_new(listing,
						      buf.st_size,
						      buf.st_mtime,
						      buf.st_mode);
			ecore_list_append(add, d);
		}

		free(name);
		free(listing);
	}
	
	/* Clean up temporary variables */
	if (filter)
		regfree(&freg);
	if (dfilter)
		regfree(&dreg);
	free(path2);	
	path2 = NULL;
	
	ecore_list_destroy(file_list);
	file_list = NULL;	
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_fileselector_set_filter(Ewl_Fileselector *fs,
					char *filter)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fs", fs);
	DCHECK_PARAM_PTR("filter", filter);
	DCHECK_TYPE("fs", fs, EWL_FILESELECTOR_TYPE);

	fs->dfilter = filter;
	ewl_fileselector_path_setup(fs, ewl_fileselector_path_get(fs));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Internally used callbacks, override at your own risk.
 */
void
ewl_fileselector_show_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Fileselector *fs;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fs = EWL_FILESELECTOR(w);
	ewl_fileselector_path_setup(fs, fs->path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_fileselector_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Fileselector *fs;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fs = EWL_FILESELECTOR(w);
	IF_FREE(fs->path);

	ecore_list_destroy(fs->files);
	IF_FREE(fs->dfilter);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_fileselector_select_file_cb(Ewl_Widget *w,
					void *ev_data, void *data)
{
	Ewl_Container *row;
	Ewl_Event_Mouse_Up *ev;
	Ewl_Fileselector *fs;
	Ewl_Widget *label = NULL;
	const char *name = NULL;
	char *full_name = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	row = EWL_CONTAINER(w);
	ev = ev_data;
	fs = data;

	/*
	 * Retrieve the file name from the label.
	 */
	ewl_container_child_iterate_begin(row);
	if ((label = ewl_container_child_next(row))) {
		name = ewl_label_text_get(EWL_LABEL(label));
	}

	full_name = ewl_fileselector_str_append(fs->path, name);
	if (!full_name) {
		DLEAVE_FUNCTION(DLEVEL_STABLE);
	}

	if ((fs->multi_select) && (ev->modifiers & EWL_KEY_MODIFIER_CTRL)) {
		int i, found = 0;

		for (i = 0; i < ecore_list_nodes(fs->files); i++) {
			char *cur;

			/* see if this selection is already in the list and
			 * remove if so */
			cur = ecore_list_goto_index(fs->files, i);
			if (!strcmp(cur, full_name))
			{
				ecore_list_remove(fs->files);
				found = 1;
				break;
			}
		}
		if (!found) ecore_list_append(fs->files, full_name);
	} else {
		ecore_list_clear(fs->files);
		ecore_list_append(fs->files, full_name);
	}

	/* only set the name if there is a single selection */
	if (ecore_list_nodes(fs->files) == 1)
		ewl_text_text_set(EWL_TEXT(fs->entry_file), name);
	else
		ewl_text_text_set(EWL_TEXT(fs->entry_file), "");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_fileselector_select_dir_cb(Ewl_Widget *w, void *ev_data, void *data)
{
	Ewl_Container *row;
	Ewl_Fileselector *fs;
	const char *path = NULL;
	char *new_path;
	Ewl_Event_Mouse_Down *ev;
	Ewl_Widget *label = NULL;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ev = ev_data;
	if (ev->clicks < 2)
		DRETURN(DLEVEL_STABLE);

	row = EWL_CONTAINER(w);
	fs = data;

	/*
	 * Retrieve the file name from the label.
	 */
	ewl_container_child_iterate_begin(row);
	if ((label = ewl_container_child_next(row))) {
		path = ewl_label_text_get(EWL_LABEL(label));
	}

	if (!strcmp(path, "..")) {
		new_path = ewl_fileselector_path_up_get(fs->path);
		path = strdup(new_path);
	} else {
		new_path = ewl_fileselector_str_append(fs->path, path);
		path = ewl_fileselector_str_append(new_path, "/");
	}
	FREE(new_path);

	ewl_fileselector_path_setup(fs, path);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_fileselector_go_up_cb(Ewl_Widget *w __UNUSED__,
				void *ev_data __UNUSED__, void *data)
{
	Ewl_Fileselector *fs;
	char *path;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	fs = data;
	path = ewl_fileselector_path_up_get(fs->path);
	ewl_fileselector_path_setup(EWL_FILESELECTOR(fs), path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_fileselector_go_home_cb(Ewl_Widget *w __UNUSED__,
				void *ev_data __UNUSED__, void *data)
{
	Ewl_Fileselector *fs;
	char *path;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	fs = data;
	path = ewl_fileselector_path_home_get();
	ewl_fileselector_path_setup(EWL_FILESELECTOR(fs), path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_fileselector_path_setup(Ewl_Fileselector *fs, const char *path)
{
	Ewl_Fileselector_Data *d;
	Ewl_Widget *parent_win;
	Ewl_Container *cont;
	Ewl_Widget *prow = NULL;
	Ecore_List *dirs, *files;
	char *path2;
	char *headers[1];
	char title[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fs", fs);
	DCHECK_PARAM_PTR("path", path);
	DCHECK_TYPE("fs", fs, EWL_FILESELECTOR_TYPE);

	/* FIXME: clearing the lists is maybe not sufficient/correct */
        ewl_tree_selected_clear(EWL_TREE(fs->list_dirs));
        ewl_tree_selected_clear(EWL_TREE(fs->list_files));
	ewl_container_reset(EWL_CONTAINER(fs->list_dirs));
	ewl_container_reset(EWL_CONTAINER(fs->list_files));
	ewl_text_text_set(EWL_TEXT(fs->entry_file), "");

	/* clear the selection list */
	ecore_list_clear(fs->files);

	/*
	 * Determine if it's an absolute path or relative path.
	 */
	if (path[strlen(path) - 1] == '/')
		path2 = strdup(path);
	else {
		path2 = (char *) malloc(sizeof(char) * (strlen(path) + 2));
		memcpy(path2, path, strlen(path));
		path2[strlen(path)] = '/';
		path2[strlen(path) + 1] = '\0';
	}

	fs->path = path2;
	ewl_text_text_set(EWL_TEXT(fs->entry_dir), path2);

	files = ecore_list_new();
	dirs = ecore_list_new();
	ewl_fileselector_file_list_get(path2, fs->ffilter, fs->dfilter, files,
				       dirs);

	parent_win = EWL_WIDGET(ewl_embed_widget_find(EWL_WIDGET(fs)));
	cont = ewl_container_redirect_get(EWL_CONTAINER(parent_win));
	if (cont) 
		ewl_container_redirect_set(EWL_CONTAINER(parent_win), NULL);

	snprintf(title, PATH_MAX, "Files (%d)", ecore_list_nodes(files));
	headers[0] = title;
	ewl_tree_headers_set(EWL_TREE(fs->list_files), headers);

	ecore_list_goto_first(files);
	while ((d = ecore_list_current(files))) {
		Ewl_Widget *label;

		prow = ewl_row_new();
		ewl_container_child_append(EWL_CONTAINER(fs->list_files), prow);
		ewl_widget_show(prow);

		label = ewl_label_new();
		ewl_label_text_set(EWL_LABEL(label), d->name);
		ewl_object_fill_policy_set(EWL_OBJECT(label),
					   EWL_FLAG_FILL_SHRINK);
		ewl_container_child_append(EWL_CONTAINER(prow), label);
		ewl_widget_show(label);

		ewl_fileselector_tooltip_add(prow, d);
		ewl_fileselector_data_free(d);

		ewl_callback_append(prow, EWL_CALLBACK_CLICKED,
				    ewl_fileselector_select_file_cb, fs);

		ecore_list_next(files);
	}

	snprintf(title, PATH_MAX, "Directories (%d)", ecore_list_nodes(dirs));
	headers[0] = title;
	ewl_tree_headers_set(EWL_TREE(fs->list_dirs), headers);

	ecore_list_goto_first(dirs);
	while ((d = ecore_list_current(dirs))) {
		Ewl_Widget *label;

		prow = ewl_row_new();
		ewl_container_child_append(EWL_CONTAINER(fs->list_dirs), prow);
		ewl_widget_show(prow);

		label = ewl_label_new();
		ewl_label_text_set(EWL_LABEL(label), d->name);
		ewl_object_fill_policy_set(EWL_OBJECT(label),
					   EWL_FLAG_FILL_SHRINK);
		ewl_container_child_append(EWL_CONTAINER(prow), label);
		ewl_widget_show(label);

		ewl_fileselector_tooltip_add(prow, ecore_list_current(dirs));
		ewl_fileselector_data_free(d);

		ewl_callback_append(prow, EWL_CALLBACK_CLICKED,
				    ewl_fileselector_select_dir_cb, fs);

		ecore_list_next(dirs);
	}

	if (cont)
		ewl_container_redirect_set(EWL_CONTAINER(parent_win), cont);

	ecore_list_destroy(files);
	ecore_list_destroy(dirs);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_fileselector_filter_cb(Ewl_Widget *entry __UNUSED__, 
				void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Fileselector *fs;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	fs = user_data;
	ewl_fileselector_path_setup(fs, ewl_fileselector_path_get(fs));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* Private: data for a file */

/* Allocate a new data. Must be freed after used */
Ewl_Fileselector_Data *
ewl_fileselector_data_new(const char *name, off_t size, time_t time, mode_t mode)
{
	Ewl_Fileselector_Data *d;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, NULL);

	d = malloc(sizeof(Ewl_Fileselector_Data));
	d->name = strdup(name);
	d->size = size;
	d->time = time;
	d->mode = mode;

	DRETURN_PTR(d, DLEVEL_STABLE);
}

/* Free an allocated data */
void
ewl_fileselector_data_free(Ewl_Fileselector_Data *d)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("d", d);

	IF_FREE(d->name);
	FREE(d);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_fileselector_tooltip_add(Ewl_Widget *w, Ewl_Fileselector_Data *d)
{
	char *str;
	char *name, *size, *perm;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("d", d);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	name = d->name;
	size = ewl_fileselector_size_string_get(d->size);
	perm = ewl_fileselector_perm_string_get(d->mode);
	str = (char *) malloc(sizeof(char) * (strlen(name) +
					      strlen(size) +
					      strlen(perm) + 3));
	memcpy(str, name, strlen(name));
	str[strlen(name)] = '\n';

	memcpy(str + strlen(name) + 1, size, strlen(size));
	str[strlen(name) + strlen(size) + 1] = '\n';

	memcpy(str + strlen(name) + strlen(size) + 2, perm, strlen(perm));
	str[strlen(name) + strlen(size) + strlen(perm) + 2] = '\0';

	ewl_attach_tooltip_text_set(w, str);

	FREE(str);
	FREE(size);
	FREE(perm);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_fileselector_files_free_cb(void *data)
{
	char *v;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	v = data;
	IF_FREE(v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
