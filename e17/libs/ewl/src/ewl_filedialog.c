#include <Ewl.h>


/**
 * @param type: type of dialog to display
 * @return Returns a new filedialog in success, NULL on failure.
 * @brief Create a new filedialog
 */
Ewl_Widget     *ewl_filedialog_new(Ewl_Filedialog_Type type)
{
	Ewl_Filedialog *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fd = NEW(Ewl_Filedialog, 1);
	if (!fd)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_filedialog_init(fd, type);

	DRETURN_PTR(EWL_WIDGET(fd), DLEVEL_STABLE);
}


/**
 * @param fd: the filedialog
 * @param type: the filedialog type
 * @return Returns no value.
 * @brief Initialize a new filedialog
 */
void
ewl_filedialog_init(Ewl_Filedialog * fd, Ewl_Filedialog_Type type)
{
	Ewl_Widget     *w;
	Ewl_Widget     *hbox;
	Ewl_Widget     *vbox;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR ("fd", fd);

	w = EWL_WIDGET(fd);

	/*
	 * Initialize the widget to have the necessary box and filedialog
	 * attributes.
	 */
	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_SHRINK |
						  EWL_FLAG_FILL_FILL);
	ewl_widget_set_appearance (EWL_WIDGET (w), "filedialog");
	fd->type = type;

	/*
	 * Setup the internally used layout and display widgets.
	 */
	vbox = ewl_vbox_new ();
	ewl_widget_set_internal(vbox, TRUE);
	ewl_object_set_fill_policy(EWL_OBJECT(vbox), EWL_FLAG_FILL_SHRINK |
						     EWL_FLAG_FILL_FILL);
	ewl_container_append_child(EWL_CONTAINER(fd), vbox);
	ewl_widget_show (vbox);

	fd->path_label = ewl_text_new ("");
	ewl_widget_set_internal(fd->path_label, TRUE);
	ewl_container_append_child(EWL_CONTAINER(vbox), fd->path_label);
	ewl_object_set_padding(EWL_OBJECT(fd->path_label), 2, 2, 2, 2);
	ewl_widget_show (fd->path_label);

	hbox = ewl_hbox_new();
	ewl_box_set_homogeneous(EWL_BOX(hbox), FALSE);
	ewl_widget_set_internal(hbox, TRUE);
	ewl_container_append_child(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show (hbox);

	/*
	 * This box will be redirected to, to allow users to pack additional
	 * buttons and widgets inside the filedialog.
	 */
	fd->decor_box = ewl_vbox_new();
	ewl_widget_set_internal(fd->decor_box, TRUE);
	ewl_object_set_fill_policy(EWL_OBJECT(fd->decor_box),
			EWL_FLAG_FILL_VFILL);
	ewl_box_set_spacing(EWL_BOX(fd->decor_box), 4);
	ewl_object_set_padding(EWL_OBJECT(fd->decor_box), 10, 10, 10, 10);
	ewl_object_set_alignment(EWL_OBJECT(fd->decor_box), EWL_FLAG_ALIGN_RIGHT);
	ewl_container_append_child(EWL_CONTAINER(hbox), fd->decor_box);
	ewl_widget_show(fd->decor_box);

	/*
	 * Display the lists of directories and files.
	 */
	fd->selector = ewl_fileselector_new();
	ewl_widget_set_internal(fd->selector, TRUE);
	ewl_container_append_child(EWL_CONTAINER(hbox), fd->selector);
	ewl_callback_append (EWL_WIDGET (fd->selector),
			EWL_CALLBACK_VALUE_CHANGED,
			ewl_filedialog_change_labels_cb, fd);
	ewl_widget_show(fd->selector);

	/*
	 * Here we provide an entry for typing in filenames, and buttons for
	 * accepting or canceling the selection.
	 */
	fd->button_box = ewl_hbox_new();
	ewl_widget_set_internal(fd->button_box, TRUE);
	ewl_object_set_fill_policy(EWL_OBJECT(fd->button_box),
			EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
	ewl_box_set_spacing(EWL_BOX(fd->button_box), 4);
	ewl_object_set_padding(EWL_OBJECT(fd->button_box), 10, 10, 10, 10);
	ewl_object_set_alignment(EWL_OBJECT(fd->button_box), EWL_FLAG_ALIGN_RIGHT);
	ewl_container_append_child(EWL_CONTAINER(vbox), fd->button_box);
	ewl_widget_show(fd->button_box);

	fd->entry = ewl_entry_new ("");
	ewl_widget_set_internal(fd->entry, TRUE);
	ewl_container_append_child(EWL_CONTAINER(fd->button_box), fd->entry);
	ewl_callback_append (fd->entry, EWL_CALLBACK_VALUE_CHANGED,
			ewl_filedialog_change_path_cb, fd);
	ewl_widget_show (fd->entry);


	if (type == EWL_FILEDIALOG_TYPE_OPEN)
		fd->ok = ewl_button_new("Open");
	else
		fd->ok = ewl_button_new("Save");
	ewl_widget_set_internal(fd->ok, TRUE);
	ewl_object_set_fill_policy(EWL_OBJECT(fd->ok), EWL_FLAG_FILL_NONE);
	ewl_callback_append(fd->ok, EWL_CALLBACK_CLICKED, ewl_filedialog_ok_cb,
			    fd);
	ewl_container_append_child(EWL_CONTAINER(fd->button_box), fd->ok);
	ewl_widget_show(fd->ok);

	fd->cancel = ewl_button_new("Cancel");
	ewl_widget_set_internal(fd->cancel, TRUE);
	ewl_object_set_fill_policy(EWL_OBJECT(fd->cancel), EWL_FLAG_FILL_NONE);
	ewl_callback_append(fd->cancel, EWL_CALLBACK_CLICKED,
				    ewl_filedialog_cancel_cb, fd);
	ewl_container_append_child(EWL_CONTAINER(fd->button_box), fd->cancel);
	ewl_widget_show(fd->cancel);

	/*
	 * Redirect incoming widgets to the decoration box to allow for
	 * special purpose widgets along the left side.
	 */
	ewl_container_set_redirect(EWL_CONTAINER(fd),
				   EWL_CONTAINER(fd->decor_box));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fd: the filedialog to change paths
 * @param path: the new path used for the filedialog
 * @return Returns no value.
 * @brief Changes the current path of a filedialog.
 */
void ewl_filedialog_set_directory(Ewl_Filedialog *fd, char *path)
{
	struct stat          statbuf;
	Ewl_Fileselector *fs = EWL_FILESELECTOR (fd->selector);
	int i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fd", fd);
	DCHECK_PARAM_PTR("path", path);

	i = stat (path, &statbuf);
	if (!i && S_ISDIR(statbuf.st_mode)) {
		printf ("Changing path to: %s\n", path);
		ewl_fileselector_set_directory (EWL_FILESELECTOR (fs), path);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_filedialog_change_labels_cb (Ewl_Widget * w, void *ev_data, 
		void *user_data) 
{
	char *path, *ptr = NULL;
	char str[PATH_MAX + 50];
	Ewl_Filedialog *fd = user_data; 

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ev_data) {
		path = ewl_fileselector_get_path (EWL_FILESELECTOR (fd->selector));

		snprintf (str, sizeof (str), "Current dir: %s", path);
		free(path);
		ewl_text_set_text (EWL_TEXT (fd->path_label), str);
 
		path = ewl_fileselector_get_filename(EWL_FILESELECTOR(fd->selector));
		if (!path) return;
		ptr = strrchr(path, '/');
		/* if we have a file */
		if (ptr)
		{
		  ptr++;
		  ewl_entry_set_text (EWL_ENTRY(fd->entry), ptr);
		}
		/* if we just changed dirs, then clear it out */
		else
		  ewl_entry_set_text (EWL_ENTRY(fd->entry), "");
		free(path);
	}
	else {
		ewl_filedialog_ok_cb(w, NULL, fd);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
ewl_filedialog_change_path_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char *dir;
	Ewl_Filedialog *fd = user_data;

	DENTER_FUNCTION(DLEVEL_STABLE);

	dir = ewl_entry_get_text (EWL_ENTRY (w));
	if (dir)
		ewl_filedialog_set_directory(fd, dir);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_filedialog_ok_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char *path1, *path2;
	char tmp[PATH_MAX];
	Ewl_Filedialog *fd = user_data;

	path1 = ewl_fileselector_get_path(EWL_FILESELECTOR(fd->selector));
	path2 = ewl_entry_get_text(EWL_ENTRY(fd->entry));
	snprintf(tmp, PATH_MAX, "%s/%s", path1, path2);
	free(path1);
	free(path2);
	path1 = strdup(tmp);
	ewl_callback_call_with_event_data(EWL_WIDGET(fd),
					  EWL_CALLBACK_VALUE_CHANGED, path1);
	free(path1);
}

void
ewl_filedialog_cancel_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Widget *fd = user_data;
	ewl_callback_call(fd, EWL_CALLBACK_VALUE_CHANGED);
}
