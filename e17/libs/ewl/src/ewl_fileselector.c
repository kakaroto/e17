#include <Ewl.h>

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <regex.h>

/*
 * Internally used functions
 */

static void ewl_fileselector_tooltip_add(Ewl_Widget * w, Ewl_Fileselector_Data * d);
static char *ewl_fileselector_str_append(char *s1, char *s2);
static char *ewl_fileselector_path_up_get(char *path);
static char *ewl_fileselector_path_home_get(void);
static char* ewl_fileselector_size_string_get(off_t st_size);
static char *ewl_fileselector_perm_string_get(mode_t st_mode);
static void  ewl_fileselector_file_list_get(char *path, char *filter, Ecore_List *flist, Ecore_List *dlist);


/**
 * @return Returns NULL on failure, or the new fileselector on success.
 * @brief Create a new fileselector
 */
Ewl_Widget *ewl_fileselector_new()
{
	Ewl_Fileselector *fs;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fs = NEW(Ewl_Fileselector, 1);
	if (!fs)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_fileselector_init(fs);

	DRETURN_PTR(EWL_WIDGET(fs), DLEVEL_STABLE);
}

/**
 * @param fs: the fileselector
 * @return Returns no value.
 * @brief Initalize the fileselector
 */
void ewl_fileselector_init(Ewl_Fileselector * fs)
{
	char *tmp;
	Ewl_Widget *w;
	Ewl_Widget *hbox;
	Ewl_Widget *misc;
	Ewl_Widget *button;

	char *head_dirs[1] = {
		"Directories"
	};
	char *head_files[1] = {
		"Files"
	};

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fs", fs);

	w = EWL_WIDGET(fs);

	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_object_size_request(EWL_OBJECT(w), 500, 450);
	ewl_object_fill_policy_set(EWL_OBJECT(w),
				   EWL_FLAG_FILL_SHRINK |
				   EWL_FLAG_FILL_HFILL);

	ewl_widget_inherit(w, "fileselector");

	/* The entry for the current directory */
	/* and some icons */
	hbox = ewl_hbox_new();
	if (hbox) {
		ewl_object_fill_policy_set(EWL_OBJECT(hbox),
					   EWL_FLAG_FILL_SHRINK |
					   EWL_FLAG_FILL_HFILL);

		fs->entry_dir = ewl_entry_new(NULL);
		ewl_object_fill_policy_set(EWL_OBJECT(hbox),
					   EWL_FLAG_FILL_SHRINK |
					   EWL_FLAG_FILL_HFILL);
		ewl_container_child_prepend(EWL_CONTAINER(hbox),
					    fs->entry_dir);
		ewl_widget_show(fs->entry_dir);
		// ewl_object_custom_w_set(EWL_OBJECT(fs->entry_dir), 200);

		misc = ewl_spacer_new();
		ewl_container_child_append(EWL_CONTAINER(hbox), misc);
		ewl_object_fill_policy_set(EWL_OBJECT(misc),
					   EWL_FLAG_FILL_FILL);
		ewl_widget_show(misc);

		button = ewl_button_stock_with_id_new(EWL_STOCK_ARROW_UP,
							EWL_RESPONSE_ARROW_UP);
		ewl_callback_append(button, EWL_CALLBACK_CLICKED,
				    EWL_CALLBACK_FUNCTION(ewl_fileselector_go_up_cb),
				    fs);
		ewl_container_child_append(EWL_CONTAINER(hbox), button);
		ewl_widget_show(button);

		button = ewl_button_stock_with_id_new(EWL_STOCK_HOME,
							EWL_RESPONSE_HOME);
		ewl_callback_append(button, EWL_CALLBACK_CLICKED,
				    EWL_CALLBACK_FUNCTION
				    (ewl_fileselector_go_home_cb), fs);
		ewl_container_child_append(EWL_CONTAINER(hbox), button);
		ewl_widget_show(button);

		ewl_container_child_append(EWL_CONTAINER(fs), hbox);
		ewl_widget_show(hbox);
	}

	/* The lists for directories and files */
	hbox = ewl_hbox_new();
	if (hbox) {
		ewl_object_fill_policy_set(EWL_OBJECT(hbox),
					   EWL_FLAG_FILL_SHRINK |
					   EWL_FLAG_FILL_FILL);

		fs->list_dirs = ewl_tree_new(1);
		ewl_tree_headers_set(EWL_TREE(fs->list_dirs), head_dirs);
		ewl_object_fill_policy_set(EWL_OBJECT(fs->list_dirs),
					   EWL_FLAG_FILL_SHRINK |
					   EWL_FLAG_FILL_FILL);
		ewl_container_child_append(EWL_CONTAINER(hbox),
					   fs->list_dirs);
		ewl_widget_show(fs->list_dirs);

		fs->list_files = ewl_tree_new(1);
		ewl_tree_headers_set(EWL_TREE(fs->list_files), head_files);
		ewl_object_fill_policy_set(EWL_OBJECT(fs->list_files),
					   EWL_FLAG_FILL_SHRINK |
					   EWL_FLAG_FILL_FILL);
		ewl_container_child_append(EWL_CONTAINER(hbox),
					   fs->list_files);
		ewl_widget_show(fs->list_files);

		ewl_container_child_append(EWL_CONTAINER(fs), hbox);
		ewl_widget_show(hbox);
	}

	/* The file label and entry */
	hbox = ewl_hbox_new();
	if (hbox) {
		ewl_object_fill_policy_set(EWL_OBJECT(hbox),
					   EWL_FLAG_FILL_SHRINK |
					   EWL_FLAG_FILL_HFILL);

		misc = ewl_text_new("File:");
		ewl_container_child_append(EWL_CONTAINER(hbox), misc);
		ewl_widget_show(misc);

		fs->entry_file = ewl_entry_new(NULL);
		ewl_container_child_append(EWL_CONTAINER(hbox),
					   fs->entry_file);
		ewl_widget_show(fs->entry_file);

		ewl_container_child_append(EWL_CONTAINER(fs), hbox);
		ewl_widget_show(hbox);
	}

	/* The filter label and entry */
	hbox = ewl_hbox_new();
	if (hbox) {
		ewl_object_fill_policy_set(EWL_OBJECT(hbox),
					   EWL_FLAG_FILL_SHRINK |
					   EWL_FLAG_FILL_HFILL);

		misc = ewl_text_new("Filter:");
		ewl_object_size_request(EWL_OBJECT(misc), 10, 50);
		ewl_container_child_append(EWL_CONTAINER(hbox), misc);
		ewl_widget_show(misc);

		fs->entry_filter = ewl_entry_new("^[^\\.]");
		ewl_callback_append(fs->entry_filter,
				EWL_CALLBACK_VALUE_CHANGED, ewl_fileselector_filter_cb,
				fs);
		ewl_container_child_append(EWL_CONTAINER(hbox),
					   fs->entry_filter);
		ewl_widget_show(fs->entry_filter);

		ewl_container_child_append(EWL_CONTAINER(fs), hbox);
		ewl_widget_show(hbox);
	}

	tmp = getenv("HOME");
	ewl_fileselector_configure_cb(fs, (tmp ? tmp : "/"));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fs: the fileselector
 * @return Returns the current path of fileselector
 * @brief Retrieve the current fileselector path
 */
char *ewl_fileselector_path_get(Ewl_Fileselector * fs)
{
	return strdup(fs->path);
}

/**
 * @param fs: the fileselector
 * @return Returns the selected filename including its path
 * @brief Retrieve the selected filename
 */
char *ewl_fileselector_file_get(Ewl_Fileselector * fs)
{
	char *entry_file;

	entry_file = ewl_entry_text_get(EWL_ENTRY(fs->entry_file));
	if (!fs->file || (fs->file && strcmp(fs->file, entry_file))) {
		IF_FREE(fs->file);
		fs->file = entry_file;
	}
	if (!fs->file || !fs->path)
		return NULL;

	return ewl_fileselector_str_append(fs->path, fs->file);
}

/**
 * @param fs: The fileselector
 * @param path: the directory to process
 * @return Returns nothing
 * @brief Prosess the given directory / change the fileselector dir
 */
void ewl_fileselector_path_set(Ewl_Fileselector * fs, char *path)
{
	ewl_fileselector_configure_cb(fs, path);
}

/*
 * Internally used functions
 */

/* if non NULL, result must be freed */
static char *ewl_fileselector_str_append(char *s1, char *s2)
{
	char *s;
	int l;
 
	/* if either the path or the file is null return null */
	if (!s1 || !s2)
		return NULL;

	l = strlen(s1) + strlen(s2) + 1;

	s = (char *) malloc(sizeof(char) * l);
	s = memcpy(s, s1, strlen(s1));
	memcpy(s + strlen(s1), s2, strlen(s2));
	s[l - 1] = '\0';

	return s;
}

/* if non NULL, result must be freed */
static char *ewl_fileselector_path_up_get(char *path)
{
	char *new_path;
	int l;

	if (!path || (l = strlen(path)) == 0)
		return NULL;

	l--;
	if (path[l] == '/')
		l--;

	if (l < 0)
		return strdup("/");

	while ((l >= 0) && (path[l] != '/'))
		l--;

	if (l < 0)
		return strdup("/");
	else {
		l++;
		new_path = (char *) malloc(sizeof(char) * (l + 1));
		new_path = memcpy(new_path, path, l);
		new_path[l] = '\0';
		return new_path;
	}
}

/* if non NULL, result must be freed */
static char *ewl_fileselector_path_home_get()
{
	char *path;
	char *new_path;

	path = getenv("HOME");
	if ((!path) || (strlen(path) == 0))
		path = strdup("/");

	if (path[strlen(path)] != '/')
		new_path = ewl_fileselector_str_append(path, "/");
	else
		new_path = path;

	return new_path;
}

static char *ewl_fileselector_size_string_get(off_t st_size)
{
	double dsize;
	char size[1024];

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

	return strdup(size);
}

static char *ewl_fileselector_perm_string_get(mode_t st_mode)
{
	char *perm;
	int i;

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

	return perm;
}

static void ewl_fileselector_file_list_get(char *path, char *filter, 
				Ecore_List * flist, Ecore_List * dlist)
{
	regex_t preg;
	Ewl_Fileselector_Data *d;
	struct dirent *lecture;
	DIR *rep;
	struct stat buf;
	char *name;
	char *path2;
	int len;

	if (filter) {
		if (regcomp(&preg, filter, REG_NOSUB | REG_EXTENDED))
			filter = NULL;
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

	rep = opendir(path2);
	if (!rep) {
		free(path2);
		DRETURN(DLEVEL_STABLE);
	}

	while ((lecture = readdir(rep))) {
		int match = 0;

		if (!(strcmp(lecture->d_name, "..")))
			match = 1;
		else if (strcmp(lecture->d_name, ".")) {
			if (filter && !regexec(&preg, lecture->d_name, 0,
						NULL, 0))
				match = 1;
		}

		if (match) {
			len = strlen(path2) + strlen(lecture->d_name) + 1;
			name = (char *) malloc(sizeof(char) * len);
			memcpy(name, path2, strlen(path2));
			memcpy(name + strlen(path2),
			       lecture->d_name, strlen(lecture->d_name));
			name[len - 1] = '\0';
			stat(name, &buf);
			if (S_ISDIR(buf.st_mode) && dlist) {
				d = ewl_fileselector_data_new(lecture->d_name,
						    buf.st_size,
						    buf.st_mtime,
						    buf.st_mode);
				ecore_list_append(dlist, d);
			} else if (flist) {
				d = ewl_fileselector_data_new(lecture->d_name,
						    buf.st_size,
						    buf.st_mtime,
						    buf.st_mode);
				ecore_list_append(flist, d);
			}

			free(name);
		}
	}
	closedir(rep);

	if (filter)
		regfree(&preg);
	free(path2);

	return;
}

/*
 * Internally used callbacks, override at your own risk.
 */
void
ewl_fileselector_tooltip_destroy_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	if (user_data)
		ewl_widget_destroy(EWL_WIDGET(user_data));
}

void ewl_fileselector_select_file_cb(Ewl_Widget * w, void *ev_data, Ewl_Fileselector * fs)
{
	Ewl_Fileselector_Data *d = NULL;

	d = (Ewl_Fileselector_Data *) ewl_widget_data_get(EWL_WIDGET(w), "FILE");
	IF_FREE(fs->file);
	fs->file = strdup( d->name );
	ewl_entry_text_set(EWL_ENTRY(fs->entry_file), d->name);
}

void ewl_fileselector_select_dir_cb(Ewl_Widget * w, void *ev_data, Ewl_Fileselector * fs)
{
	char *path;
	char *new_path;

	path = (char *) ewl_widget_data_get(EWL_WIDGET(w), "DIR");
	if (!strcmp(path, ".."))
		new_path = ewl_fileselector_path_up_get(fs->path);
	else
		new_path = ewl_fileselector_str_append(fs->path, path);
	path = ewl_fileselector_str_append(new_path, "/");
	free(new_path);
	ewl_fileselector_configure_cb(EWL_FILESELECTOR(fs), path);
}

void ewl_fileselector_go_up_cb(Ewl_Widget * w, void *ev_data, Ewl_Fileselector * fs)
{
	char *path;

	path = ewl_fileselector_path_up_get(fs->path);
	ewl_fileselector_configure_cb(EWL_FILESELECTOR(fs), path);
}

void ewl_fileselector_go_home_cb(Ewl_Widget * w, void *ev_data, Ewl_Fileselector * fs)
{
	char *path;

	path = ewl_fileselector_path_home_get();
	ewl_fileselector_configure_cb(EWL_FILESELECTOR(fs), path);
}

void ewl_fileselector_configure_cb(Ewl_Fileselector * fs, char *path)
{
	char *filter;
	Ewl_Fileselector_Data *d;
	Ewl_Widget *parent_win;
	Ewl_Container *cont;
	Ewl_Widget *prow = NULL;
	Ecore_List *dirs, *files;
	char *path2;
	char *title;

	ewl_container_reset(EWL_CONTAINER(fs->list_dirs));
	ewl_container_reset(EWL_CONTAINER(fs->list_files));
	ewl_entry_text_set(EWL_ENTRY(fs->entry_file), "");

	/*
	 * Setup a regex for matching files.
	 */
	filter = ewl_entry_text_get(EWL_ENTRY(fs->entry_filter));

	if (path[strlen(path) - 1] == '/')
		path2 = strdup(path);
	else {
		path2 = (char *) malloc(sizeof(char) * (strlen(path) + 2));
		memcpy(path2, path, strlen(path));
		path2[strlen(path)] = '/';
		path2[strlen(path) + 1] = '\0';
	}

	fs->path = path2;
	fs->file = NULL;
	ewl_entry_text_set(EWL_ENTRY(fs->entry_dir), path2);

	files = ecore_list_new();
	dirs = ecore_list_new();
	ewl_fileselector_file_list_get(path2, filter, files, dirs);

	parent_win = EWL_WIDGET(ewl_embed_widget_find(EWL_WIDGET(fs)));
	cont = ewl_container_redirect_get(EWL_CONTAINER(parent_win));
	if (cont) {
		ewl_container_redirect_set(EWL_CONTAINER(parent_win), NULL);
	}

	title = malloc(PATH_MAX);
	if (!title)
		DRETURN(DLEVEL_STABLE);

	snprintf(title, PATH_MAX, "Files (%d)", ecore_list_nodes(files));
	ewl_tree_headers_set(EWL_TREE(fs->list_files), &title);

	ecore_list_goto_first(files);
	while ((d = ecore_list_current(files))) {
		prow = ewl_tree_text_row_add(EWL_TREE(fs->list_files),
					     NULL, &d->name);

		ewl_widget_data_set(EWL_WIDGET(prow), "FILE", d);

		ewl_fileselector_tooltip_add(EWL_WIDGET(prow), d);

		ewl_callback_append(EWL_WIDGET(prow),
				    EWL_CALLBACK_CLICKED,
				    EWL_CALLBACK_FUNCTION
				    (ewl_fileselector_select_file_cb), fs);

		ecore_list_next(files);
	}

	snprintf(title, PATH_MAX, "Directories (%d)", ecore_list_nodes(dirs));
	ewl_tree_headers_set(EWL_TREE(fs->list_dirs), &title);

	ecore_list_goto_first(dirs);
	while ((d = ecore_list_current(dirs))) {
		Ewl_Widget *hbox;
		Ewl_Widget *label;

		hbox = ewl_hbox_new();
		ewl_box_spacing_set(EWL_BOX(hbox), 5);
		label = ewl_text_new(d->name);
		ewl_object_fill_policy_set(EWL_OBJECT(label),
					   EWL_FLAG_FILL_SHRINK);
		ewl_container_child_append(EWL_CONTAINER(hbox), label);
		ewl_widget_show(label);
		ewl_widget_show(hbox);

		prow =
		    ewl_tree_row_add(EWL_TREE(fs->list_dirs), NULL, &hbox);
		ewl_widget_data_set(EWL_WIDGET(prow), "DIR", d->name);

		ewl_fileselector_tooltip_add(EWL_WIDGET(prow),
				   ecore_list_current(dirs));

		ewl_callback_append(EWL_WIDGET(prow), EWL_CALLBACK_CLICKED,
				    EWL_CALLBACK_FUNCTION
				    (ewl_fileselector_select_dir_cb), fs);

		ecore_list_next(dirs);
	}

	if (cont)
		ewl_container_redirect_set(EWL_CONTAINER(parent_win),
					   cont);

	ecore_list_destroy(files);
	ecore_list_destroy(dirs);

}

void ewl_fileselector_filter_cb(Ewl_Widget * entry, void *ev_data, void *user_data)
{
	Ewl_Fileselector *fs = user_data;
	ewl_fileselector_configure_cb(fs, ewl_fileselector_path_get(fs));
}


/* Private: data for a file */

/* Allocate a new data. Must be freed after used */
Ewl_Fileselector_Data
*ewl_fileselector_data_new(const char *name, off_t size, time_t time, mode_t mode)
{
	Ewl_Fileselector_Data *d;
	char *n;

	n = strdup(name);

	d = (Ewl_Fileselector_Data *) malloc(sizeof(Ewl_Fileselector_Data));
	d->name = n;
	d->size = size;
	d->time = time;
	d->mode = mode;

	return d;
}

/* Free an allocated data */
void ewl_fileselector_data_free(Ewl_Fileselector_Data * d)
{
	if (d) {
		if (d->name)
			free(d->name);
		free(d);
	}
}

static void ewl_fileselector_tooltip_add(Ewl_Widget * w, Ewl_Fileselector_Data * d)
{
	Ewl_Widget *parent_win;
	Ewl_Widget *tooltip;
	char *str;
	char *name, *size, *perm;

	parent_win = EWL_WIDGET(ewl_embed_widget_find(w));

	tooltip = ewl_tooltip_new(w);
	ewl_tooltip_delay_set(EWL_TOOLTIP(tooltip), 1.0);
	ewl_container_child_append(EWL_CONTAINER(parent_win), tooltip);

	name = d->name;
	size = ewl_fileselector_size_string_get(d->size);
	perm = ewl_fileselector_perm_string_get(d->mode);
	str = (char *) malloc(sizeof(char) * (strlen(name) +
					      strlen(size) +
					      strlen(perm) + 3));
	str = memcpy(str, name, strlen(name));
	str[strlen(name)] = '\n';
	memcpy(str + strlen(name) + 1, size, strlen(size));
	str[strlen(name) + strlen(size) + 1] = '\n';
	memcpy(str + strlen(name) + strlen(size) + 2, perm, strlen(perm));
	str[strlen(name) + strlen(size) + strlen(perm) + 2] = '\0';
	ewl_tooltip_text_set(EWL_TOOLTIP(tooltip), str);

	/* destroy tooltip when the row is destroyed */
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_DESTROY,
			    EWL_CALLBACK_FUNCTION
			    (ewl_fileselector_tooltip_destroy_cb), tooltip);

	free(size);
	free(perm);
}
