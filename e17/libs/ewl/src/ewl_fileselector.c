#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

static void ewl_filedialog_process_directory(Ewl_Fileselector * fs, char *dir);
static int ewl_fileselector_alphasort(const void *a, const void *b);



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
 * @param clicked: callback for file clicked event
 * @return Returns NULL on failure, or a pointer to the new fileselector on success.
 * @brief create a new fileselector
 */
Ewl_Widget     *ewl_fileselector_new(Ewl_Callback_Function clicked)
{
	Ewl_Fileselector *fs;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fs = NEW(Ewl_Fileselector, 1);
	if (!fs)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_fileselector_init(fs, clicked);

	DRETURN_PTR(EWL_WIDGET(fs), DLEVEL_STABLE);
}

/**
 * @param fs: the fileselector
 * @param fc: your select file callback function
 * @return Returns no value.
 * @brief Initalize the fileselector
 */
void ewl_fileselector_init(Ewl_Fileselector * fs, Ewl_Callback_Function fc)
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
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_SHRINK |
				                  EWL_FLAG_FILL_FILL);

	/* 
	 * Create the vbox that should contain the directories tree
	 */
	fs->dbox = ewl_vbox_new();
	ewl_object_set_fill_policy(EWL_OBJECT(fs->dbox), EWL_FLAG_FILL_SHRINK |
							 EWL_FLAG_FILL_FILL);
	ewl_container_append_child(EWL_CONTAINER(w), fs->dbox);
	ewl_widget_show(fs->dbox);

	fs->dirs = ewl_tree_new (1);
	ewl_tree_set_headers (EWL_TREE (fs->dirs), head_dirs);
	ewl_container_append_child(EWL_CONTAINER (fs->dbox), fs->dirs);
	ewl_object_set_minimum_size (EWL_OBJECT (fs->dirs), 100, 50);
	ewl_widget_show (fs->dirs);

	/*
	 * Create the vbox that should contain the files tree
	 */
	fs->fbox = ewl_vbox_new();
	ewl_object_set_fill_policy(EWL_OBJECT(fs->fbox), EWL_FLAG_FILL_FILL);
	ewl_container_append_child(EWL_CONTAINER(w), fs->fbox);
	ewl_widget_show(fs->fbox);

	fs->files = ewl_tree_new (1);
	ewl_tree_set_headers (EWL_TREE (fs->files), head_files);
	ewl_container_append_child(EWL_CONTAINER (fs->fbox), fs->files);
	ewl_widget_show (fs->files);

	/* Set what callback the user has defined */
	fs->file_clicked = fc;

	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    ewl_fileselector_realize_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_fileselector_configure_cb, NULL);


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param row: the fileselector
 * @return Returns the selected filename including its path
 * @brief Retrieve the selected filename
 */
char *ewl_fileselector_get_filename (Ewl_Fileselector *fs) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(fs->item, DLEVEL_STABLE);
}

/**
 * @param fs: the fileselector
 * @return Returns the current path of fileselector
 * @brief Retrieve the current fileselector path
 */
char *ewl_fileselector_get_path (Ewl_Fileselector *fs)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(fs->path, DLEVEL_STABLE);
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
		ewl_filedialog_process_directory(fs, home);

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

static void
ewl_filedialog_process_directory(Ewl_Fileselector * fs, char *directory)
{
	struct dirent        **dentries;
	int                  num, i;
	char                 dir[PATH_MAX];
	char                 file[PATH_MAX];
	struct stat          statbuf;
	Ewl_Widget           *items[1];
	Ewl_Widget           *row = NULL;
	Ewl_Fileselector_Row *f_row;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fs", fs);

	strncpy (dir, directory, PATH_MAX);

	fs->path = strdup (directory);
	ewl_callback_call(EWL_WIDGET(fs), EWL_CALLBACK_VALUE_CHANGED);

	if ((num = scandir(dir, &dentries, 0, ewl_fileselector_alphasort)) < 0) {
		perror("ewl_filedialog_process_directory - scandir");
		return;
	}
	
	printf ("Prosessing %d elements in %s\n", num, dir);
	
	while (num--) {
		snprintf(file, PATH_MAX, "%s/%s", dir, dentries[num]->d_name);

		i = stat (file, &statbuf);
		if (i == -1) {
			perror("ewl_filedialog_process_directory - stat 1");
			continue;
		}

		if (dentries[num]->d_name[0] == '.' && 
				(strlen(dentries[num]->d_name) > 2 || 
					dentries[num]->d_name[1] != '.'))
			continue;
		
		printf ("%s\n", dentries[num]->d_name);

		items[0] = ewl_text_new (dentries[num]->d_name);
		ewl_widget_show (items[0]);
		
		if (S_ISDIR(statbuf.st_mode)) {
			row = ewl_tree_add_row (EWL_TREE (fs->dirs), NULL, items);
			
			ewl_callback_append(row, EWL_CALLBACK_DOUBLE_CLICKED,
					ewl_filedialog_directory_clicked_cb, fs);
			ewl_callback_append(row, EWL_CALLBACK_CLICKED,
					ewl_filedialog_directory_clicked_single_cb, fs);
			
		} else if (S_ISREG(statbuf.st_mode)) {
			row = ewl_tree_add_row (EWL_TREE (fs->files), NULL, items);
			
			ewl_callback_append (row, EWL_CALLBACK_DOUBLE_CLICKED, 
					fs->file_clicked, fs);
			ewl_callback_append(row, EWL_CALLBACK_CLICKED,
					ewl_filedialog_file_clicked_cb, fs);
			
		}
		
		f_row = NEW (Ewl_Fileselector_Row, 1);
		f_row->name = strdup (dentries[num]->d_name);
		f_row->path = strdup (dir);
		ewl_widget_set_data (EWL_WIDGET (row), "filedialog_info", f_row);
	}
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_filedialog_file_clicked_cb(Ewl_Widget * w, void *ev_data,
		void *user_data)
{
	Ewl_Fileselector *fs;
	char *file;
	Ewl_Fileselector_Row *f_info = ewl_widget_get_data (EWL_WIDGET (w), 
			"filedialog_info");
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	fs = EWL_FILESELECTOR (user_data);

	file = malloc (PATH_MAX);
	snprintf (file, PATH_MAX, "%s/%s", f_info->path, f_info->name);
	printf ("Single click: %s\n", file);

	fs->item = strdup (file);
	ewl_callback_call(EWL_WIDGET(fs), EWL_CALLBACK_CLICKED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_filedialog_directory_clicked_single_cb(Ewl_Widget * w, void *ev_data,
		            void *user_data)
{
	char dir[PATH_MAX];
	Ewl_Fileselector *fs;
	Ewl_Fileselector_Row *d_info = ewl_widget_get_data (EWL_WIDGET (w), 
			"filedialog_info"); 
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	fs = EWL_FILESELECTOR (user_data);

	snprintf (dir, PATH_MAX, "%s/%s", d_info->path, d_info->name);
	fs->item = strdup (dir);
	ewl_callback_call(EWL_WIDGET(fs), EWL_CALLBACK_CLICKED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_filedialog_directory_clicked_cb(Ewl_Widget * w, void *ev_data,
				    void *user_data)
{
	char dir[PATH_MAX];
	Ewl_Fileselector *fs;
	Ewl_Fileselector_Row *d_info = ewl_widget_get_data (EWL_WIDGET (w), 
			"filedialog_info");

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	fs = EWL_FILESELECTOR (user_data);

	snprintf (dir, PATH_MAX, "%s/%s", d_info->path, d_info->name);
	fs->item = strdup (dir);
	ewl_callback_call(EWL_WIDGET(fs), EWL_CALLBACK_CLICKED);
	
	printf ("DIR clicked: %s\n", dir);

	ewl_container_reset (EWL_CONTAINER (fs->dirs));
	ewl_container_reset (EWL_CONTAINER (fs->files));
	ewl_filedialog_process_directory(EWL_FILESELECTOR(user_data), dir);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int ewl_fileselector_alphasort (const void *a, const void *b)
{
  struct dirent **ad = (struct dirent **)a;
  struct dirent **bd = (struct dirent **)b;
  return (strcmp((*bd)->d_name, (*ad)->d_name));
}

