#ifndef __EWL_FILESELECTOR_H__
#define __EWL_FILESELECTOR_H__

/**
 * @defgroup Ewl_Fileselector Fileselector: Basic File And Directory Listing
 *
 * Displays a list of directories and a list of files for the user to choose.
 * The selector triggers a EWL_CALLBACK_VALUE_CHANGED callback when either a
 * click causes the selected file to change, when a double click causes the
 * currently displayed directory to change, or when a double click on a file
 * occurs to signal an open. The event data passed to the callback is NULL,
 * unless the double click event occurred on a file, in which case it is the
 * file's path.
 *
 * @{
 */

/**
 * @themekey /fileselector/file
 * @themekey /fileselector/group
 */

/**
 * The Ewl_Fileselector provides a fileselector
 */
typedef struct Ewl_Fileselector Ewl_Fileselector;

/**
 * @def EWL_FILESELECTOR(fd) 
 * Typecasts a pointer to an Ewl_Fileselector pointer.
 */
#define EWL_FILESELECTOR(fs) ((Ewl_Fileselector *) fs)

/**
 * @struct Ewl_Fileselector
 * Creates a fileselector with one tree for dirs and one for files
 */
struct Ewl_Fileselector
{
	Ewl_Box         box;   /* the vbox containing the trees */
	Ewl_Widget     *dirs;	 /* directory table */
	Ewl_Widget     *files; /* file table */

	char           *path;  /* current fileselector path */
	char           *item;  /* current selected item in the fileselector */
};


Ewl_Widget *ewl_fileselector_new();

void ewl_fileselector_init(Ewl_Fileselector * fs);
void ewl_fileselector_set_directory(Ewl_Fileselector * fs, char *path);

char *ewl_fileselector_get_filename (Ewl_Fileselector *fs);
char *ewl_fileselector_get_path (Ewl_Fileselector *fs);


/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_fileselector_realize_cb(Ewl_Widget * w, void *ev_data,
		void *user_data);
void ewl_fileselector_configure_cb(Ewl_Widget * w, void *ev_data,
		void *user_data);
void ewl_fileselector_file_clicked_cb(Ewl_Widget * w, void *ev_data, 
		void *user_data);
void ewl_fileselector_file_open_cb(Ewl_Widget * w, void *ev_data,
		void *user_data);
void ewl_fileselector_directory_clicked_single_cb(Ewl_Widget * w, 
		void *ev_data, void *user_data);
void ewl_fileselector_directory_clicked_cb(Ewl_Widget * w, void *ev_data, 
		void *user_data);

/**
 * @}
 */

#endif				/* __EWL_FILESELECTOR_H__ */
