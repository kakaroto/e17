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
	
	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_SHRINK |
						  EWL_FLAG_FILL_FILL);
	fd->type = type;

	ewl_widget_set_appearance (EWL_WIDGET (w), "filedialog");

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
	/*
	ewl_object_set_fill_policy(EWL_OBJECT(fd->decor_box), EWL_FLAG_FILL_NONE);
	*/
	ewl_object_set_alignment(EWL_OBJECT(fd->decor_box), EWL_FLAG_ALIGN_RIGHT);
	ewl_container_append_child(EWL_CONTAINER(hbox), fd->decor_box);
	ewl_widget_show(fd->decor_box);

	fd->selector = ewl_fileselector_new(ewl_filedialog_fs_ok_cb);
	ewl_widget_set_internal(fd->selector, TRUE);
	ewl_container_append_child(EWL_CONTAINER(hbox), fd->selector);
	ewl_callback_append (EWL_WIDGET (fd->selector),
			EWL_CALLBACK_VALUE_CHANGED,
			ewl_filedialog_change_labels_cb, fd);
	ewl_callback_append (EWL_WIDGET (fd->selector),
			EWL_CALLBACK_CLICKED, ewl_filedialog_change_entry_cb,
			fd);
	ewl_widget_show(fd->selector);

	fd->button_box = ewl_hbox_new();
	ewl_widget_set_internal(fd->button_box, TRUE);
	ewl_object_set_fill_policy(EWL_OBJECT(fd->button_box),
			EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
	ewl_box_set_spacing(EWL_BOX(fd->button_box), 4);
	ewl_object_set_padding(EWL_OBJECT(fd->button_box), 10, 10, 10, 10);
	/*
	ewl_object_set_fill_policy(EWL_OBJECT(fd->button_box), EWL_FLAG_FILL_NONE);
	*/
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

	ewl_container_set_redirect(EWL_CONTAINER(fd),
				   EWL_CONTAINER(fd->decor_box));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_filedialog_change_labels_cb (Ewl_Widget * w, void *ev_data, 
		void *user_data) 
{
	char *ptr;
	char str[PATH_MAX + 50];
	Ewl_Filedialog *fd = user_data; 

	DENTER_FUNCTION(DLEVEL_STABLE);
	
	ptr = ewl_fileselector_get_path (EWL_FILESELECTOR (fd->selector));

	snprintf (str, sizeof (str), "Current dir: %s", ptr);

	ewl_text_set_text (EWL_TEXT (fd->path_label), str);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_filedialog_change_entry_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Filedialog *fd = user_data;
	Ewl_Fileselector *fs = EWL_FILESELECTOR (fd->selector);

	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_entry_set_text (EWL_ENTRY (fd->entry), fs->item);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
ewl_filedialog_change_path_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	struct stat          statbuf;
	Ewl_Filedialog *fd = user_data;
	Ewl_Fileselector *fs = EWL_FILESELECTOR (fd->selector);
	char *dir;
	int i;

	dir = ewl_entry_get_text (EWL_ENTRY (w));
	
	i = stat (dir, &statbuf);
	if (S_ISDIR(statbuf.st_mode)) {
		printf ("Changing path to: %s\n", dir);
		ewl_fileselector_process_directory (EWL_FILESELECTOR (fs), dir);
	}
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
	ewl_callback_call_with_event_data(fd, EWL_CALLBACK_VALUE_CHANGED,
					  path1);
	free(path1);
}

void
ewl_filedialog_cancel_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Widget *fd = user_data;
	ewl_callback_call(fd, EWL_CALLBACK_VALUE_CHANGED);
}

void
ewl_filedialog_fs_ok_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *fs = user_data;
	Ewl_Widget *fd = fs->parent->parent;

	ewl_filedialog_ok_cb(w, ev_data, fd);
}
