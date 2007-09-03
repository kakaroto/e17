#include "entropy.h"
#include <Etk.h>
#include "etk_location_add_dialog.h"

typedef enum evfs_filesystem_properties {
	REQUIRES_AUTH = 0x1,
	REQUIRES_HOST = 0x2,
	
} evfs_filesystem_properties;

typedef struct _evfs_filesystem evfs_filesystem;
struct _evfs_filesystem {
	char* name;
	char* uri;
	evfs_filesystem_properties properties;
};

static Ecore_Hash* entropy_evfs_filesystems = NULL;
static int location_add_init = 0;

static Etk_Bool 
_location_add_window_delete (Etk_Object * object, void *data)
{
	entropy_etk_location_add_dialog* dialog = data;

	free(dialog);

	return ETK_TRUE;
}


void _entropy_etk_location_cancel_cb(Etk_Object *object, void *data) {
	entropy_etk_location_add_dialog* dialog = data;
	
	etk_object_destroy(ETK_OBJECT(dialog->window));
}

void _entropy_etk_location_radio_cb(Etk_Object *object, void *data)
{
	entropy_etk_location_add_dialog* dialog = data;
	
	/*Get the uri for this button*/
	dialog->selected_uri = (char*)etk_object_data_get(ETK_OBJECT(object), "uri");
	printf("Selected uri becomes '%s'\n", dialog->selected_uri);
	
}

void _location_add_next_cb(Etk_Object *obj, void *data) 
{
	entropy_etk_location_add_dialog* dialog = data;

	evfs_filesystem* system = ecore_hash_get(entropy_evfs_filesystems, dialog->selected_uri);
	
	if (dialog->screen_id == 0) {
		dialog->screen_id++;
		etk_widget_hide_all(dialog->vbox1);

		etk_widget_show_all(dialog->vbox2);

		/*Check if we need to display host/username/password*/
		if (system) {
			printf("We have a system..\n");
			if (system->properties & REQUIRES_AUTH) {
				printf("Showing widgets..\n");
				etk_widget_show(dialog->username_widget_label);
				etk_widget_show(dialog->username_widget_entry);
				etk_widget_show(dialog->password_widget_label);
				etk_widget_show(dialog->password_widget_entry);
			} else  {
				printf("Hiding widgets..\n");
				etk_widget_hide(dialog->username_widget_label);
				etk_widget_hide(dialog->username_widget_entry);
				etk_widget_hide(dialog->password_widget_label);
				etk_widget_hide(dialog->password_widget_entry);				
			}

			if (system->properties & REQUIRES_HOST) {
				printf("Showing widgets..\n");
				etk_widget_show(dialog->host_widget_label);
				etk_widget_show(dialog->host_widget_entry);
			} else  {
				printf("Hiding widgets..\n");
				etk_widget_hide(dialog->host_widget_label);
				etk_widget_hide(dialog->host_widget_entry);
			}
		}
		
		
		etk_container_add(ETK_CONTAINER(dialog->frame), dialog->vbox2);
	} else if (dialog->screen_id == 1) {
		/*Finish!*/
		const char *host= NULL, *path = NULL, *name = NULL, *username = NULL, *password = NULL;
		char buffer[PATH_MAX];
		Entropy_Config_Structure* structure;

		name = etk_entry_text_get(ETK_ENTRY(dialog->name_entry));
		host = etk_entry_text_get(ETK_ENTRY(dialog->host_widget_entry));
		username = etk_entry_text_get(ETK_ENTRY(dialog->username_widget_entry));
		password = etk_entry_text_get(ETK_ENTRY(dialog->password_widget_entry));
		path = etk_entry_text_get(ETK_ENTRY(dialog->path_entry));

		printf("Adding location '%s'\n", name);

		bzero(buffer, PATH_MAX);

		snprintf(buffer, PATH_MAX, "%s://", dialog->selected_uri);

		if (system->properties & REQUIRES_AUTH) {
			strcat(buffer, username);
			strcat(buffer, ":");
			strcat(buffer, password);
			strcat(buffer, "@");
		}
		
		if (system->properties & REQUIRES_HOST) {
			strcat(buffer, "/");
			strcat(buffer, host);
		}

		strcat(buffer, path);		
	
		printf("Final URI: '%s'\n", buffer);

		structure = entropy_config_standard_structures_add((char*)name, buffer);
		(*dialog->add_callback)(dialog->instance, structure);

		etk_object_destroy(ETK_OBJECT(dialog->window));
	}
}


evfs_filesystem* filesystem_init_and_add(char* name, char* uri, evfs_filesystem_properties props)
{
	evfs_filesystem* system = entropy_malloc(sizeof(evfs_filesystem));
	system->name = strdup(name);
	system->properties = props;
	system->uri = strdup(uri);

	printf("Added filesystem with name '%s' for uri '%s'\n", system->name, system->uri);

	ecore_hash_set(entropy_evfs_filesystems, uri, system);
	
	return system;
}

static void location_add_initialise() {
	entropy_evfs_filesystems = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	
	
	/*Add 'file'*/
	filesystem_init_and_add("Local Filesystem", "file", 0);
	filesystem_init_and_add("Windows File Share", "smb", REQUIRES_AUTH | REQUIRES_HOST);
	filesystem_init_and_add("Secure FTP Host", "sftp", REQUIRES_HOST);
	filesystem_init_and_add("Virtual Folder", "vfolder", 0);

	location_add_init = 1;
}


void etk_location_add_dialog_create(entropy_gui_component_instance* instance, 
		void (*add_callback)(entropy_gui_component_instance*, Entropy_Config_Structure*) )
{
	Ecore_List* filesystems;
	evfs_filesystem* system;
	char* key;
	Etk_Widget* dialog_button;
	Etk_Widget* hbox;
	Etk_Widget* outer_vbox;
	Etk_Widget* first_button =  NULL;

	Etk_Widget* label;

	entropy_etk_location_add_dialog* dialog = 
		entropy_malloc(sizeof(entropy_etk_location_add_dialog));

	if (!location_add_init) 
		location_add_initialise();

	dialog->instance = instance;
	dialog->add_callback = add_callback;

	dialog->window = etk_window_new();
	
	etk_window_title_set(ETK_WINDOW(dialog->window), "Add location..");
	etk_window_wmclass_set(ETK_WINDOW(dialog->window), "locationadd", "locationadd");

	etk_signal_connect ("delete_event", ETK_OBJECT (dialog->window),
		      ETK_CALLBACK (_location_add_window_delete), dialog);

	outer_vbox = etk_vbox_new(ETK_FALSE,0);
	etk_container_add(ETK_CONTAINER(dialog->window), outer_vbox);

	dialog->frame = etk_frame_new("Add Location..");
	etk_box_append(ETK_BOX(outer_vbox), dialog->frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 10);

	dialog->vbox1 = etk_vbox_new(ETK_FALSE,0);
	etk_container_add(ETK_CONTAINER(dialog->frame), dialog->vbox1);


	filesystems = ecore_hash_keys(entropy_evfs_filesystems);
	while ( (key = ecore_list_first_remove(filesystems)) ) {
		Etk_Widget* button;
		system = ecore_hash_get(entropy_evfs_filesystems, key);

		printf("Adding radio button for '%s'...\n", system->name);
		
		if (!first_button) {
			button = etk_radio_button_new_with_label(system->name, NULL);
			first_button = button;
		} else {
			button = etk_radio_button_new_with_label_from_widget(system->name, 
				ETK_RADIO_BUTTON(first_button));
		}

		/*Select the first item*/
		if (!dialog->selected_uri)
			dialog->selected_uri = key;

	      etk_object_data_set(ETK_OBJECT(button), "uri", key);
	      etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(_entropy_etk_location_radio_cb), dialog );

		etk_box_append(ETK_BOX(dialog->vbox1), button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 10);
	}
	ecore_list_destroy(filesystems);



	/*Setup screen 2*/
	dialog->vbox2 = etk_table_new(2,5,ETK_FALSE);
	label = etk_label_new("Location Name");
	etk_table_attach(ETK_TABLE(dialog->vbox2), label,0,0,0,0,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	dialog->name_entry = etk_entry_new();
	etk_entry_text_set(ETK_ENTRY(dialog->name_entry), _("New Location"));
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->name_entry,1,1,0,0,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	label = etk_label_new("Path");
	etk_table_attach(ETK_TABLE(dialog->vbox2), label,0,0,1,1,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	dialog->path_entry = etk_entry_new();
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->path_entry,1,1,1,1,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);


	/*Optional Elements*/
	dialog->host_widget_label = etk_label_new("Host");
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->host_widget_label,0,0,2,2,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	dialog->host_widget_entry = etk_entry_new();
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->host_widget_entry,1,1,2,2,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	dialog->username_widget_label = etk_label_new("Username");
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->username_widget_label,0,0,3,3,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	dialog->username_widget_entry = etk_entry_new();
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->username_widget_entry,1,1,3,3,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);


	dialog->password_widget_label = etk_label_new("Password");
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->password_widget_label,0,0,4,4,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	dialog->password_widget_entry = etk_entry_new();
	etk_entry_password_mode_set(ETK_ENTRY(dialog->password_widget_entry), ETK_TRUE);
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->password_widget_entry,1,1,4,4,
				0,0,
				ETK_TABLE_HFILL | ETK_TABLE_VFILL | ETK_TABLE_HEXPAND);

	

	/*Setup the dialog buttons*/
	hbox = etk_hbox_new(ETK_FALSE,0);
	
	dialog_button = etk_button_new_with_label("Back");
	etk_box_append(ETK_BOX(hbox), dialog_button, ETK_BOX_START, ETK_BOX_NONE, 5);
	

	dialog_button = etk_button_new_with_label("Next >");
	etk_box_append(ETK_BOX(hbox), dialog_button, ETK_BOX_START, ETK_BOX_NONE, 5);
	etk_signal_connect("clicked", ETK_OBJECT(dialog_button), ETK_CALLBACK(_location_add_next_cb), dialog);

	dialog_button = etk_button_new_with_label("Cancel");
	etk_signal_connect("clicked", ETK_OBJECT(dialog_button), ETK_CALLBACK(_entropy_etk_location_cancel_cb), dialog);
	etk_box_append(ETK_BOX(hbox), dialog_button, ETK_FALSE, ETK_FALSE, 5);
	

	etk_box_append(ETK_BOX(outer_vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 5);
	
	etk_widget_show_all(dialog->window);
}



