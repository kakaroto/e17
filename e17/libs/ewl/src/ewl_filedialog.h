#ifndef __EWL_FILEDIALOG_H__
#define __EWL_FILEDIALOG_H__

typedef struct Ewl_Filedialog Ewl_Filedialog;

#define EWL_FILEDIALOG(fd) ((Ewl_Filedialog *) fd)
struct Ewl_Filedialog
{
	Ewl_Floater     box;

	Ewl_Filedialog_Type type;

	Ewl_Widget     *selector;	/* Ewl_Fileselector */
	Ewl_Widget     *path_label; /* label to display current path */
	Ewl_Widget     *entry; /* entry for manual input or current selected */
	void           *dialog;	/* open or save dialog */
};

Ewl_Widget *ewl_filedialog_new(Ewl_Widget * follows, Ewl_Filedialog_Type type,
			       Ewl_Callback_Function cb);
void        ewl_filedialog_init(Ewl_Filedialog * fd, Ewl_Widget * follows,
				Ewl_Filedialog_Type type,
				Ewl_Callback_Function cb);

/*
 * Internally used callback, override at your own risk.
 */
void ewl_filedialog_destroy_cb(Ewl_Widget * w, void *ev_data, void *user_data);

#endif
