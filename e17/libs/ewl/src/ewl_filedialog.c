#include <Ewl.h>


/**
 * @param type: type of dialog to display
 * @param ok_cb: callback to be called when open/save button is pushed
 * @param cancel_cb: callback to be called when cancel button is pushed
 * @return Returns a new filedialog in success, NULL on failure.
 * @brief Create a new filedialog
 */
Ewl_Widget     *ewl_filedialog_new(Ewl_Filedialog_Type type,
				   Ewl_Callback_Function ok_cb,
				   Ewl_Callback_Function cancel_cb)
{
	Ewl_Filedialog *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fd = NEW(Ewl_Filedialog, 1);
	if (!fd)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_filedialog_init(fd, type, ok_cb, cancel_cb);

	DRETURN_PTR(EWL_WIDGET(fd), DLEVEL_STABLE);
}


/**
 * @param fd: the filedialog
 * @param type: the filedialog type
 * @param ok_cb: the callback to call when open/save button is pushed
 * @param cancel_cb: the callback to call when cancel button is pushed
 * @return Returns no value.
 * @brief Initialize a new filedialog
 */
void
ewl_filedialog_init(Ewl_Filedialog * fd, Ewl_Filedialog_Type type,
		Ewl_Callback_Function ok_cb, Ewl_Callback_Function cancel_cb)
{
	Ewl_Widget     *w;
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
	ewl_object_set_fill_policy(EWL_OBJECT(vbox), EWL_FLAG_FILL_SHRINK |
						     EWL_FLAG_FILL_FILL);
	ewl_container_append_child(EWL_CONTAINER(fd), vbox);
	ewl_widget_show (vbox);

	fd->path_label = ewl_text_new ("");
	ewl_container_append_child(EWL_CONTAINER(vbox), fd->path_label);
	ewl_object_set_padding(EWL_OBJECT(fd->path_label), 2, 2, 2, 2);
	ewl_widget_show (fd->path_label);

	fd->selector = ewl_fileselector_new(ok_cb);
	ewl_container_append_child(EWL_CONTAINER(vbox), fd->selector);
	ewl_callback_append (EWL_WIDGET (fd->selector),
			EWL_CALLBACK_VALUE_CHANGED,
			ewl_filedialog_change_labels, fd);
	ewl_callback_append (EWL_WIDGET (fd->selector),
			EWL_CALLBACK_CLICKED, ewl_filedialog_change_entry, fd);
	ewl_widget_show(fd->selector);

	fd->button_box = ewl_box_new(EWL_ORIENTATION_HORIZONTAL);
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
	ewl_container_append_child(EWL_CONTAINER(fd->button_box), fd->entry);
	ewl_callback_append (fd->entry, EWL_CALLBACK_VALUE_CHANGED,
			ewl_filedialog_change_path, fd);
	ewl_widget_show (fd->entry);


	if (type == EWL_FILEDIALOG_TYPE_OPEN)
		fd->ok = ewl_button_new("Open");
	else
		fd->ok = ewl_button_new("Save");

	ewl_object_set_fill_policy(EWL_OBJECT(fd->ok), EWL_FLAG_FILL_NONE);
	ewl_callback_append(fd->ok, EWL_CALLBACK_CLICKED,
			    ewl_filedialog_hide_cb, fd);
	if (ok_cb) {
		ewl_callback_append(fd->ok, EWL_CALLBACK_CLICKED, ok_cb,
				fd->selector);
	}
	ewl_container_append_child(EWL_CONTAINER(fd->button_box), fd->ok);
	ewl_widget_show(fd->ok);

	fd->cancel = ewl_button_new("Cancel");
	ewl_object_set_fill_policy(EWL_OBJECT(fd->cancel), EWL_FLAG_FILL_NONE);
	ewl_callback_append(fd->cancel, EWL_CALLBACK_CLICKED,
			    ewl_filedialog_hide_cb, fd);
	if (cancel_cb) {
		ewl_callback_append(fd->cancel, EWL_CALLBACK_CLICKED,
				    cancel_cb, fd->selector);
	}
	ewl_container_append_child(EWL_CONTAINER(fd->button_box), fd->cancel);
	ewl_widget_show(fd->cancel);


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_filedialog_change_labels (Ewl_Widget * w, void *ev_data, 
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

void ewl_filedialog_change_entry (Ewl_Widget * w, void *ev_data,
		void *user_data)
{
	Ewl_Filedialog *fd = user_data;
	Ewl_Fileselector *fs = EWL_FILESELECTOR (fd->selector);

	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_entry_set_text (EWL_ENTRY (fd->entry), fs->item);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void ewl_filedialog_change_path(Ewl_Widget * w, void *ev_data, void *user_data)
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

void ewl_filedialog_hide_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *fd = user_data;
	ewl_widget_hide(fd);
}
