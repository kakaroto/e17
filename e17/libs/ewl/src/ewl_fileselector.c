
#include <Ewl.h>


void            __ewl_fileselector_realize(Ewl_Widget * w, void *ev_data,
					   void *user_data);
void            __ewl_fileselector_configure(Ewl_Widget * w, void *ev_data,
					     void *user_data);
void            __directory_clicked(Ewl_Widget * w, void *ev_data,
				    void *user_data);
void            __process_directory(Ewl_Fileselector * fs, char *dir);

/**
 * ewl_fileselector_new - create a new fileselector
 * @file_clicked: callback for file clicked event
 *
 * Returns a pointer to a newly allocated fileselector on success, NULL
 * on failure.
 */
Ewl_Widget     *
ewl_fileselector_new(Ewl_Callback_Function file_clicked)
{
	Ewl_Fileselector *fs;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fs = NEW(Ewl_Fileselector, 1);
	if (!fs)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	memset(fs, 0, sizeof(Ewl_Fileselector));

	ewl_fileselector_init(fs, file_clicked);

	DRETURN_PTR(EWL_WIDGET(fs), DLEVEL_STABLE);
}

/**
 * ewl_fileselector_init - initialize a new fileselector
 * @fs: the fileselector
 *
 * Returns nothing. Initialize the fileselector to default values
 */
void
ewl_fileselector_init(Ewl_Fileselector * fs, Ewl_Callback_Function fc)
{
	Ewl_Widget     *w;


	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fs", fs);

	w = EWL_WIDGET(fs);

	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_HORIZONTAL);
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_FILL);

	fs->dbox = ewl_vbox_new();
	ewl_object_set_fill_policy(EWL_OBJECT(fs->dbox), EWL_FILL_POLICY_FILL);
	ewl_container_append_child(EWL_CONTAINER(w), fs->dbox);
	ewl_widget_show(fs->dbox);

	fs->fbox = ewl_vbox_new();
	ewl_object_set_fill_policy(EWL_OBJECT(fs->fbox), EWL_FILL_POLICY_FILL);
	ewl_container_append_child(EWL_CONTAINER(w), fs->fbox);
	ewl_widget_show(fs->fbox);

	fs->file_clicked = fc;

	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_fileselector_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_fileselector_configure, NULL);


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
__ewl_fileselector_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Fileselector *fs;
	char           *home;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	fs = EWL_FILESELECTOR(w);
	home = getenv("HOME");

	__process_directory(fs, home);


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
__ewl_fileselector_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Fileselector *fs;

	DENTER_FUNCTION(DLEVEL_STABLE);


	fs = EWL_FILESELECTOR(w);

	ewl_widget_configure(fs->files);
	ewl_widget_configure(fs->dirs);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
__directory_clicked(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char            dir[PATH_MAX];
	int             index;
	Ewl_Fileselector *fs;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);


	printf("in dir clicked\n");
	fs = EWL_FILESELECTOR(user_data);
	index = (int) ewl_widget_get_data(w, (void *) w->parent);

	sprintf(dir, "%s/%s", fs->d_info[index].path, fs->d_info[index].name);

	__process_directory(EWL_FILESELECTOR(user_data), dir);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
__process_directory(Ewl_Fileselector * fs, char *directory)
{
	char          **dir_head, **file_head;
	char            file[PATH_MAX];
	struct dirent **dentries;
	struct stat     statBuffer;
	int             num, i, f_count, d_count, result;
	char            dir[PATH_MAX];

	Ewl_Table_Child *tc;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fs", fs);

	dir_head = file_head = NULL;
	num = i = f_count = d_count = result = 0;
	tc = NULL;

	strncpy(dir, directory, PATH_MAX);

	if ((num = scandir(dir, &dentries, 0, alphasort)) < 0) {
		perror("__process_directory - scandir");
		return;
	}

	--num;

	/**
	 * count the number of regular files and number of directories
	 */
	for (i = num; i >= 0; i--) {
		if (dentries[i]->d_name[0] != '.') {
			sprintf(file, "%s/%s", dir, dentries[i]->d_name);
			result = stat(file, &statBuffer);
			if (result == -1) {
				perror("__process_directory - stat 1");
				continue;
			}

			if (S_ISDIR(statBuffer.st_mode))
				d_count++;
			else if (S_ISREG(statBuffer.st_mode))
				f_count++;
			else
				continue;
		}
	}
	d_count += 3;		/* add 3 to d_count for the dir header,  '.' and '..' */
	f_count++;		/* add 1 to f_count for the file header */


	/**
	 * time to create the tables to hold the files and directories,
	 * and also initialize the arrays to hold the dir/file info
	 */
	IF_FREE(fs->f_info);
	IF_FREE(fs->d_info);

	fs->f_info = NEW(Ewl_Fileinfo, f_count);
	fs->d_info = NEW(Ewl_Dirinfo, d_count);

	dir_head = NEW(char *, 1);

	dir_head[0] = "Directory";
	file_head = NEW(char *, 1);

	file_head[0] = "File Name";

	if (fs->dirs)
		ewl_table_reset(EWL_TABLE(fs->dirs), 1, d_count, dir_head);
	else {
		fs->dirs = ewl_table_new(1, d_count, dir_head);
		ewl_container_append_child(EWL_CONTAINER(fs->dbox), fs->dirs);
		ewl_widget_show(fs->dirs);
	}

	if (fs->files)
		ewl_table_reset(EWL_TABLE(fs->files), 1, f_count, file_head);
	else {
		fs->files = ewl_table_new(1, f_count, file_head);
		ewl_container_append_child(EWL_CONTAINER(fs->fbox), fs->files);
		ewl_widget_show(fs->files);
	}


	/**
	 * ready to start filling in the tables with directory and file
	 * information
	 */
	result = 0;
	for (i = num; i >= 0; i--) {

		/* make sure there exists non-hidden files/directories
		 */
		if (d_count == 0 && f_count == 0)
			break;

		/* if the current file is not hidden (start with .), add
		 * it to the table
		 */
		if (dentries[i]->d_name[0] != '.') {
			/* stat the file */
			sprintf(file, "%s/%s", dir, dentries[i]->d_name);
			result = stat(file, &statBuffer);
			if (result == -1) {
				perror("__process_directory - stat 2: ");
				continue;
			}

			/* if the file is a directory add it to the dir
			 * table
			 */
			if (S_ISDIR(statBuffer.st_mode)) {
				printf("dir: %s\n", file);
				d_count--;
				strncpy(fs->d_info[d_count].name,
					(const char *) dentries[i]->d_name,
					PATH_MAX);
				strncpy(fs->d_info[d_count].path,
					(const char *) dir, PATH_MAX);
				fs->d_info[d_count].dirEntry = dentries[i];

				tc = (Ewl_Table_Child *)
					ewl_table_add_return(EWL_TABLE
							     (fs->dirs),
							     dentries[i]->
							     d_name, 1, 1,
							     d_count + 1,
							     d_count + 1);

				ewl_widget_show(EWL_WIDGET(tc));

				ewl_callback_append(EWL_WIDGET(tc->widget),
						    EWL_CALLBACK_MOUSE_UP,
						    __directory_clicked, fs);

				ewl_widget_set_data(tc->widget, (void *) tc,
						    (int *) d_count);

			}
			/* else if it is a regulare file add it to the
			 * file table
			 */
			else if (S_ISREG(statBuffer.st_mode)) {
				printf("file: %s\n", file);
				f_count--;
				strncpy(fs->f_info[f_count].name,
					(const char *) dentries[i]->d_name,
					PATH_MAX);
				strncpy(fs->f_info[f_count].path,
					(const char *) dir, PATH_MAX);
				fs->f_info[f_count].status = statBuffer;

				tc = (Ewl_Table_Child *)
					ewl_table_add_return(EWL_TABLE
							     (fs->files),
							     dentries[i]->
							     d_name, 1, 1,
							     f_count + 1,
							     f_count + 1);

				ewl_widget_show(EWL_WIDGET(tc));

				if (fs->file_clicked != NULL) {
					ewl_callback_append(EWL_WIDGET
							    (tc->widget),
							    EWL_CALLBACK_MOUSE_UP,
							    fs->file_clicked,
							    fs);
				}

				ewl_widget_set_data(tc->widget, (void *) tc,
						    (int *) f_count);
			}

			else
				continue;

		}
	}

	/**
	 * add directories '.' and '..' to the directory table
	 */
	d_count--;
	strcpy(fs->d_info[d_count].name, "..");
	strcpy(fs->d_info[d_count].path, dir);

	tc = (Ewl_Table_Child *) ewl_table_add_return(EWL_TABLE(fs->dirs),
						      "..", 1, 1, d_count + 1,
						      d_count + 1);
	ewl_widget_show(EWL_WIDGET(tc));
	ewl_callback_append(EWL_WIDGET(tc), EWL_CALLBACK_MOUSE_UP,
			    __directory_clicked, fs);
	ewl_widget_set_data(tc->widget, (void *) tc, (int *) d_count);


	d_count--;
	strcpy(fs->d_info[d_count].name, ".");
	strcpy(fs->d_info[d_count].path, dir);

	tc = (Ewl_Table_Child *) ewl_table_add_return(EWL_TABLE(fs->dirs),
						      ".", 1, 1, d_count + 1,
						      d_count + 1);
	ewl_widget_show(EWL_WIDGET(tc));
	ewl_callback_append(EWL_WIDGET(tc), EWL_CALLBACK_MOUSE_UP,
			    __directory_clicked, fs);
	ewl_widget_set_data(tc->widget, (void *) tc, (int *) d_count);



	ewl_widget_configure(EWL_WIDGET(fs));


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
