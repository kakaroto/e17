#include <Ewl.h>


/**
 * @param follows: the widget this dialog follows
 * @param cb: callback to be called when open/save button is pushed
 * @return Returns a new filedialog in success, NULL on failure.
 * @brief Create a new filedialog
 */
Ewl_Widget     *ewl_filedialog_new(Ewl_Widget * follows,
				   Ewl_Filedialog_Type type,
				   Ewl_Callback_Function cb)
{
	Ewl_Filedialog *fd;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fd = NEW(Ewl_Filedialog, 1);
	if (!fd)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_filedialog_init(fd, follows, type, cb);

	DRETURN_PTR(EWL_WIDGET(fd), DLEVEL_STABLE);
}


/**
 * ewl_filedialog_init - initialize a new filedialog
 * @fd: the filedialog
 * @follows: widget to follow for the floater
 * @type: the filedialog type
 * @cb: the callback to call when open/save button is pushed
 *
 * Returns nothing. Iinitialize the filedialog to default values.
 */
void
ewl_filedialog_init(Ewl_Filedialog * fd, Ewl_Widget * follows,
		    Ewl_Filedialog_Type type, Ewl_Callback_Function cb)
{
	Ewl_Widget     *w;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR ("fd", fd);

	w = EWL_WIDGET(fd);
	
	ewl_floater_init(EWL_FLOATER(w), follows);
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_SHRINK |
						  EWL_FLAG_FILL_FILL);
	fd->type = type;

	ewl_widget_set_appearance (EWL_WIDGET (w), "filedialog");

	if (type == EWL_FILEDIALOG_TYPE_OPEN)
		ewl_filedialog_open_init(EWL_FILEDIALOG (fd), cb);
	else
		ewl_filedialog_save_init(EWL_FILEDIALOG (fd), cb);


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


void ewl_filedialog_open_init(Ewl_Filedialog * fd, Ewl_Callback_Function cb)
{
	Open_Dialog    *od;
	Ewl_Widget     *vbox;

	DENTER_FUNCTION(DLEVEL_STABLE);

	od = NEW(Open_Dialog, 1);

	if (!od)
		return;

	vbox = ewl_vbox_new ();
	ewl_object_set_fill_policy(EWL_OBJECT(vbox), EWL_FLAG_FILL_SHRINK |
						     EWL_FLAG_FILL_FILL);
	ewl_container_append_child(EWL_CONTAINER(fd), vbox);
	ewl_widget_show (vbox);

	fd->path_label = ewl_text_new ("");
	ewl_container_append_child(EWL_CONTAINER(vbox), fd->path_label);
	ewl_object_set_padding(EWL_OBJECT(fd->path_label), 2, 2, 2, 2);
	ewl_widget_show (fd->path_label);

	fd->selector = ewl_fileselector_new(cb);
	ewl_container_append_child(EWL_CONTAINER(vbox), fd->selector);
	ewl_callback_append (EWL_WIDGET (fd->selector),
			EWL_CALLBACK_VALUE_CHANGED, ewl_filedialog_change_labels, fd);
	ewl_callback_append (EWL_WIDGET (fd->selector),
			EWL_CALLBACK_CLICKED, ewl_filedialog_change_entry, fd);
	ewl_widget_show(fd->selector);

	od->box = ewl_box_new(EWL_ORIENTATION_HORIZONTAL);
	ewl_object_set_fill_policy(EWL_OBJECT(od->box), EWL_FLAG_FILL_HFILL |
							EWL_FLAG_FILL_HSHRINK);
	ewl_box_set_spacing(EWL_BOX(od->box), 4);
	ewl_object_set_padding(EWL_OBJECT(od->box), 10, 10, 10, 10);
	/*
	ewl_object_set_fill_policy(EWL_OBJECT(od->box), EWL_FLAG_FILL_NONE);
	*/
	ewl_object_set_alignment(EWL_OBJECT(od->box), EWL_FLAG_ALIGN_RIGHT);
	ewl_container_append_child(EWL_CONTAINER(vbox), od->box);
	ewl_widget_show(od->box);

	fd->entry = ewl_entry_new ("");
	ewl_container_append_child(EWL_CONTAINER(od->box), fd->entry);
	ewl_callback_append (fd->entry, EWL_CALLBACK_VALUE_CHANGED,
			ewl_filedialog_change_path, fd);
	ewl_widget_show (fd->entry);

	od->open = ewl_button_new("Open");
	ewl_object_set_fill_policy(EWL_OBJECT(od->open), EWL_FLAG_FILL_NONE);
	ewl_callback_append(od->open, EWL_CALLBACK_CLICKED, cb, fd->selector);
	ewl_container_append_child(EWL_CONTAINER(od->box), od->open);
	ewl_widget_show(od->open);

	od->cancel = ewl_button_new("Cancel");
	ewl_object_set_fill_policy(EWL_OBJECT(od->cancel), EWL_FLAG_FILL_NONE);
	ewl_callback_append(od->cancel, EWL_CALLBACK_CLICKED,
			    ewl_filedialog_destroy_cb, NULL);
	ewl_container_append_child(EWL_CONTAINER(od->box), od->cancel);
	ewl_widget_show(od->cancel);

	fd->dialog = (void *) od;

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

void ewl_filedialog_save_init(Ewl_Filedialog * fd, Ewl_Callback_Function cb)
{
	Save_Dialog    *sd;

	DENTER_FUNCTION(DLEVEL_STABLE);

	sd = NEW(Save_Dialog, 1);
	if (!sd)
		return;

	fd->selector = ewl_fileselector_new(cb);
	ewl_container_append_child(EWL_CONTAINER(fd), fd->selector);


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_filedialog_destroy_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *fd;

	/* Destroy the filedialog */
	fd = w->parent->parent;
	ewl_widget_destroy(fd);
}
