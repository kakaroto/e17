#ifndef __EWL_FILESELECTOR_H__
#define __EWL_FILESELECTOR_H__

/**
 * @defgroup Ewl_Fileselector Fileselector: a fileselector
 * @{
 */

/**
 * The Ewl_Fileselector_Row provides data for a single row
 */
typedef struct _ewl_fileselector_row Ewl_Fileselector_Row;

/**
 * @def EWL_FILESELECTOR_ROW(fd) 
 * Typecasts a pointer to an Ewl_Fileselector_Row pointer.
 */
#define EWL_FILESELECTOR_ROW(di) ((Ewl_Fileselector_Row *))

/**
 * @struct Ewl_Fileselector_Row
 * Internally used struct for storing the filename as a 
 * datapointer inside each tree row
 */
struct _ewl_fileselector_row {
	char            *name;	/* directory name */
	char            *path;	/* path to directory */
};

/**
 * The Ewl_Fileselector provides a fileselector
 */
typedef struct _ewl_fileselector Ewl_Fileselector;

/**
 * @def EWL_FILESELECTOR(fd) 
 * Typecasts a pointer to an Ewl_Fileselector pointer.
 */
#define EWL_FILESELECTOR(fs) ((Ewl_Fileselector *) fs)

/**
 * @struct Ewl_Fileselector
 * Creates a fileselector with one tree for dirs and one for files
 */
struct _ewl_fileselector {
	Ewl_Box         box;   /* the vbox containing the trees */
	Ewl_Widget     *dirs;	 /* directory table */
	Ewl_Widget     *files; /* file table */

	char           *path;  /* current fileselector path */
	char           *item;  /* current selected item in the fileselector */

	Ewl_Callback_Function file_clicked; /* use callback for open file */
};


Ewl_Widget *ewl_fileselector_new(Ewl_Callback_Function file_clicked);

void ewl_fileselector_init(Ewl_Fileselector * fs,
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
void ewl_fileselector_file_clicked_cb(Ewl_Widget * w, void *ev_data, 
		void *user_data);
void ewl_fileselector_directory_clicked_single_cb(Ewl_Widget * w, 
		void *ev_data, void *user_data);
void ewl_fileselector_directory_clicked_cb(Ewl_Widget * w, void *ev_data, 
		void *user_data);
char *ewl_fileselector_directory_adjust (Ewl_Fileselector *fs,
		Ewl_Fileselector_Row *d_info);
char *ewl_fileselector_path_down (char *dir);
int ewl_fileselector_alphasort(const void *a, const void *b);


/**
 * @}
 */

#endif				/* __EWL_FILESELECTOR_H__ */
