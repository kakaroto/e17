
#define __EWL_FILEDIALOG_H__
#define __EWL_FILEDIALOG_H__

typedef struct _ewl_filedialog Ewl_Filedialog;

#define EWL_FILEDIALOG(fd) ((Ewl_Filedialog *) fd)
struct _ewl_filedialog {
	Ewl_Floater     box;

	Ewl_Filedialog_Type type;

	Ewl_Widget     *selector;	/* Ewl_Fileselector */
	void           *dialog;	/* open or save dialog */
};


Ewl_Widget     *ewl_filedialog_new(Ewl_Widget * follows,
				   Ewl_Filedialog_Type type,
				   Ewl_Callback_Function cb);
void            ewl_filedialog_init(Ewl_Filedialog * fd, Ewl_Widget * follows,
				    Ewl_Filedialog_Type type,
				    Ewl_Callback_Function cb);
