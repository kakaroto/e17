#include <Ewl.h>

void ewl_fileselector_directory_adjust (Ewl_Fileselector *fs, char *path);
void ewl_fileselector_path_down (char *dir);
int ewl_fileselector_alphasort(const void *a, const void *b);

/** Addition for Solaris compatablity in scandir dep. -benr **/
/** Code written originally by Joerg-R. Hill for Viewmol used with permission and covered by GPL **/
#ifdef HAVE_SUN

#include<dirent.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>

int scandir(const char *dir, struct dirent ***namelist,
            int (*select)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **))
{
  DIR *d;
  struct dirent *entry;
  register int i=0;
  size_t entrysize;

  if ((d=opendir(dir)) == NULL)
     return(-1);

  *namelist=NULL;
  while ((entry=readdir(d)) != NULL)
  {
    if (select == NULL || (select != NULL && (*select)(entry)))
    {
      *namelist=(struct dirent **)realloc((void *)(*namelist),
                 (size_t)((i+1)*sizeof(struct dirent *)));
        if (*namelist == NULL) return(-1);
        entrysize=sizeof(struct dirent)-sizeof(entry->d_name)+strlen(entry->d_name)+1;
        (*namelist)[i]=(struct dirent *)malloc(entrysize);
        if ((*namelist)[i] == NULL) return(-1);
        memcpy((*namelist)[i], entry, entrysize);
        i++;
    }
  }
  if (closedir(d)) return(-1);
  if (i == 0) return(-1);
  if (compar != NULL)
    qsort((void *)(*namelist), (size_t)i, sizeof(struct dirent *), compar);

  return(i);
}


#endif
/* *************************************** */



/**
 * @return Returns NULL on failure, or the new fileselector on success.
 * @brief Create a new fileselector
 */
Ewl_Widget     *ewl_fileselector_new()
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
	Ewl_Widget     *w;
	char *head_dirs[1] = {
		"Directories"
	};
	char *head_files[1] = {
		"Files"
	};

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fs", fs);

	w = EWL_WIDGET(fs);

	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_set_appearance(w, "fileselector");
	ewl_box_set_homogeneous(EWL_BOX(w), TRUE);
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_SHRINK |
				                  EWL_FLAG_FILL_FILL);

	fs->dirs = ewl_tree_new (1);
	ewl_tree_set_headers (EWL_TREE (fs->dirs), head_dirs);
	ewl_container_append_child(EWL_CONTAINER(w), fs->dirs);
	ewl_object_set_padding(EWL_OBJECT(fs->dirs), 2, 2, 2, 2);
	ewl_widget_show (fs->dirs);

	fs->files = ewl_tree_new (1);
	ewl_tree_set_headers (EWL_TREE (fs->files), head_files);
	ewl_container_append_child(EWL_CONTAINER(w), fs->files);
	ewl_object_set_padding(EWL_OBJECT(fs->files), 2, 2, 2, 2);
	ewl_widget_show (fs->files);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    ewl_fileselector_realize_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_fileselector_configure_cb, NULL);


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fs: the fileselector
 * @return Returns the selected filename including its path
 * @brief Retrieve the selected filename
 */
char *ewl_fileselector_get_filename (Ewl_Fileselector *fs) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DRETURN_PTR(fs->item ? strdup(fs->item) : NULL, DLEVEL_STABLE);
}

/**
 * @param fs: the fileselector
 * @return Returns the current path of fileselector
 * @brief Retrieve the current fileselector path
 */
char *ewl_fileselector_get_path (Ewl_Fileselector *fs)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DRETURN_PTR(fs->path ? strdup(fs->path) : NULL, DLEVEL_STABLE);
}


void ewl_fileselector_realize_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Fileselector *fs;
	char           *home;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	fs = EWL_FILESELECTOR(w);
	home = getenv("HOME");

	if (home)
		ewl_fileselector_set_directory(fs, home);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
ewl_fileselector_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Fileselector *fs;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fs = EWL_FILESELECTOR(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fs: The fileselector
 * @param path: the directory to process
 * @return Returns nothing
 * @brief Prosess the given directory / change the fileselector dir
 */
void ewl_fileselector_set_directory(Ewl_Fileselector * fs, char *path)
{
	struct dirent        **dentries;
	int                  num, i;
	char                 file[PATH_MAX];
	struct stat          statbuf;
	Ewl_Widget           *items[1];
	Ewl_Widget           *row = NULL;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fs", fs);
	DCHECK_PARAM_PTR("path", path);

	if (fs->path && !strcmp(fs->path, path))
		DRETURN(DLEVEL_STABLE);

	if ((num = scandir(path, &dentries, 0, ewl_fileselector_alphasort)) < 0) {
		perror("ewl_fileselector_set_directory - scandir");
		return;
	}

	/* 
	 * Empty the trees before adding data.
	 */
	ewl_container_reset (EWL_CONTAINER (fs->dirs));
	ewl_container_reset (EWL_CONTAINER (fs->files));

	fs->path = strdup (path);
	
	while (num--) {
		snprintf(file, PATH_MAX, "%s/%s", path, dentries[num]->d_name);

		i = stat (file, &statbuf);
		if (i == -1) {
			perror("ewl_fileselector_set_directory - stat 1");
			continue;
		}

		if (dentries[num]->d_name[0] == '.' && 
				(strlen(dentries[num]->d_name) > 2 || 
					dentries[num]->d_name[1] != '.'))
			continue;
		
		items[0] = ewl_text_new (dentries[num]->d_name);
		ewl_object_set_fill_policy(EWL_OBJECT(items[0]),
					   EWL_FLAG_FILL_HFILL);
		ewl_widget_show (items[0]);
		
		if (S_ISDIR(statbuf.st_mode)) {
			ewl_callback_append(items[0], EWL_CALLBACK_CLICKED,
					    ewl_fileselector_directory_clicked_single_cb,
					    fs);
			row = ewl_tree_add_row (EWL_TREE (fs->dirs), NULL,
						items);
			ewl_callback_append(items[0],
					    EWL_CALLBACK_DOUBLE_CLICKED,
					    ewl_fileselector_directory_clicked_cb,
					    fs);
		}
		else if (S_ISREG(statbuf.st_mode)) {
			ewl_callback_append(items[0],
					    EWL_CALLBACK_DOUBLE_CLICKED,
					    ewl_fileselector_file_open_cb,
					    fs);
			ewl_callback_append(items[0], EWL_CALLBACK_CLICKED,
					    ewl_fileselector_file_clicked_cb,
					    fs);
			row = ewl_tree_add_row (EWL_TREE (fs->files), NULL, items);
		}

		if (!row)
			continue;
	}

	ewl_callback_call(EWL_WIDGET(fs), EWL_CALLBACK_VALUE_CHANGED);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_fileselector_file_clicked_cb(Ewl_Widget * w, void *ev_data,
		void *user_data)
{
	char *tmp;
	Ewl_Fileselector *fs;
	char file[PATH_MAX];
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	fs = EWL_FILESELECTOR (user_data);

	tmp = ewl_text_get_text(EWL_TEXT(w));
	if (!strcmp (tmp, "/"))
		snprintf (file, PATH_MAX, "/%s", tmp);
	else
		snprintf (file, PATH_MAX, "%s/%s", fs->path, tmp);

	fs->item = strdup (file);
	ewl_callback_call(EWL_WIDGET(fs), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_fileselector_file_open_cb(Ewl_Widget * w, void *ev_data,
		void *user_data)
{
	char *tmp;
	Ewl_Fileselector *fs;
	char file[PATH_MAX];
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	fs = EWL_FILESELECTOR (user_data);

	tmp = ewl_text_get_text(EWL_TEXT(w));
	if (!strcmp (tmp, "/"))
		snprintf (file, PATH_MAX, "/%s", tmp);
	else
		snprintf (file, PATH_MAX, "%s/%s", fs->path, tmp);

	fs->item = strdup (file);
	ewl_callback_call_with_event_data(EWL_WIDGET(fs),
			EWL_CALLBACK_VALUE_CHANGED, fs->item);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_fileselector_directory_adjust (Ewl_Fileselector *fs, char *path)
{
	char *ptr;
	char dir[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);

	dir[0] = '\0';
	ptr = strrchr(path, '/');
	if (ptr) {
		ptr++;
		if (!strcmp (ptr, "..")) {
			ptr--;
			*ptr = '\0';
			ptr = strrchr(path, '/');
			ptr++;
			*ptr = '\0';

			if (strcmp(path, "/")) {
				ptr--;
				*ptr = '\0';
			}
		}
		snprintf (dir, PATH_MAX, "%s", path);
	}

	IF_FREE(fs->item);
	fs->item = strdup (dir);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_fileselector_directory_clicked_single_cb(Ewl_Widget * w, 
		void *ev_data, void *user_data)
{
	Ewl_Fileselector *fs;
	char *format;
	char path[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	fs = EWL_FILESELECTOR (user_data);

	if (!strcmp(fs->path, "/"))
		format = "%s%s";
	else
		format = "%s/%s";

	snprintf(path, PATH_MAX, format, fs->path,
		 ewl_text_get_text(EWL_TEXT(w)));

	ewl_fileselector_directory_adjust (fs, path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_fileselector_directory_clicked_cb(Ewl_Widget * w, void *ev_data,
				    void *user_data)
{
	char *dir;
	char path[PATH_MAX];
	char *format;
	Ewl_Fileselector *fs;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	
	fs = EWL_FILESELECTOR (user_data);

	dir = ewl_text_get_text(EWL_TEXT(w));

	if (!strcmp(fs->path, "/"))
		format = "%s%s";
	else
		format = "%s/%s";
	snprintf(path, PATH_MAX, format, fs->path, dir);

	IF_FREE(dir);
	ewl_fileselector_directory_adjust (fs, path);
	ewl_fileselector_set_directory (fs, path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int ewl_fileselector_alphasort (const void *a, const void *b)
{
  struct dirent **ad = (struct dirent **)a;
  struct dirent **bd = (struct dirent **)b;
  return (strcmp((*bd)->d_name, (*ad)->d_name));
}

