
#ifndef __EWL_FILESELECTOR_H__
#define __EWL_FILESELECTOR_H__


typedef struct _ewl_fileselector_row Ewl_Fileselector_Row;

#define EWL_FILESELECTOR_ROW(di) ((Ewl_Dirinfo *))
struct _ewl_fileselector_row {
	char            *name;	/* directory name */
	char            *path;	/* path to directory */
};


typedef struct _ewl_fileselector Ewl_Fileselector;

#define EWL_FILESELECTOR(fs) ((Ewl_Fileselector *) fs)
struct _ewl_fileselector {
	Ewl_Box         box;

	Ewl_Widget     *dbox;	/* vbox to display directories */
	Ewl_Widget     *fbox;	/* vbox to display files */

	Ewl_Widget     *dirs;	/* directory table */
	Ewl_Widget     *files;	/* file table */

	char           *path; /* current fileselector path */
	char           *item; /* current selected item in the fileselector */

	Ewl_Callback_Function file_clicked;
};


Ewl_Widget     *ewl_fileselector_new(Ewl_Callback_Function file_clicked);
void            ewl_fileselector_init(Ewl_Fileselector * fs,
				      Ewl_Callback_Function fc);

void ewl_fileselector_process_directory(Ewl_Fileselector * fs, char *path);

char *ewl_fileselector_get_filename (Ewl_Fileselector *fs);
char *ewl_fileselector_get_path (Ewl_Fileselector *fs);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_fileselector_realize_cb(Ewl_Widget * w, void *ev_data,
				 void *user_data);
void ewl_fileselector_configure_cb(Ewl_Widget * w, void *ev_data,
				   void *user_data);
void ewl_fileselector_directory_clicked(Ewl_Widget * w, void *ev_data,
					void *user_data);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_filedialog_file_clicked_cb(Ewl_Widget * w, void *ev_data, 
				    void *user_data);
void ewl_filedialog_directory_clicked_single_cb(Ewl_Widget * w, void *ev_data,
		                void *user_data);
void ewl_filedialog_directory_clicked_cb(Ewl_Widget * w, void *ev_data, 
					 void *user_data);


#endif				/* __EWL_FILESELECTOR_H__ */
