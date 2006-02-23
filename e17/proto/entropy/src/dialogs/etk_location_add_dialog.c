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
	evfs_filesystem* filesystem;
	entropy_evfs_filesystems = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	
	
	/*Add 'file'*/
	filesystem_init_and_add("Local Filesystem", "file", 0);
	filesystem_init_and_add("Windows File Share", "smb", REQUIRES_AUTH | REQUIRES_HOST);
	filesystem_init_and_add("Secure FTP Host", "sftp", REQUIRES_HOST);

	location_add_init = 1;
}


void etk_location_add_dialog_create(entropy_gui_component_instance* instance)
{
	Ecore_List* filesystems;
	evfs_filesystem* system;
	char* key;
	Etk_Widget* vbox;
	Etk_Widget* dialog_button;
	Etk_Widget* hbox;
	Etk_Widget* outer_vbox;
	Etk_Widget* table;
	Evas_List* button_group = NULL;
	Etk_Widget* first_button =  NULL;

	Etk_Widget* entry;
	Etk_Widget* label;

	entropy_etk_location_add_dialog* dialog = 
		entropy_malloc(sizeof(entropy_etk_location_add_dialog));

	if (!location_add_init) 
		location_add_initialise();

	dialog->window = etk_window_new();
	
	etk_window_title_set(ETK_WINDOW(dialog->window), "Add location..");
	etk_window_wmclass_set(ETK_WINDOW(dialog->window), "locationadd", "locationadd");

	etk_signal_connect ("delete_event", ETK_OBJECT (dialog->window),
		      ETK_CALLBACK (_location_add_window_delete), dialog);

	outer_vbox = etk_vbox_new(ETK_FALSE,0);
	etk_container_add(ETK_CONTAINER(dialog->window), outer_vbox);

	dialog->frame = etk_frame_new("Add Location..");
	etk_container_add(ETK_CONTAINER(outer_vbox), dialog->frame);

	dialog->vbox1 = etk_vbox_new(ETK_FALSE,0);
	etk_container_add(ETK_CONTAINER(dialog->frame), dialog->vbox1);


	filesystems = ecore_hash_keys(entropy_evfs_filesystems);
	while ( (key = ecore_list_remove_first(filesystems)) ) {
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

		etk_box_pack_start(ETK_BOX(dialog->vbox1), button, ETK_TRUE, ETK_TRUE, 10);
	}
	ecore_list_destroy(filesystems);



	/*Setup screen 2*/
	dialog->vbox2 = etk_table_new(2,5,ETK_FALSE);
	label = etk_label_new("Location Name");
	etk_table_attach(ETK_TABLE(dialog->vbox2), label,0,0,0,0,
				0,0,
				ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL | ETK_FILL_POLICY_HEXPAND);

	dialog->name_entry = etk_entry_new();
	etk_entry_text_set(ETK_ENTRY(entry), "New Location");
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->name_entry,1,1,0,0,
				0,0,
				ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL | ETK_FILL_POLICY_HEXPAND);

	label = etk_label_new("Path");
	etk_table_attach(ETK_TABLE(dialog->vbox2), label,0,0,1,1,
				0,0,
				ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL | ETK_FILL_POLICY_HEXPAND);

	dialog->path_entry = etk_entry_new();
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->path_entry,1,1,1,1,
				0,0,
				ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL | ETK_FILL_POLICY_HEXPAND);


	/*Optional Elements*/
	dialog->host_widget_label = etk_label_new("Host");
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->host_widget_label,0,0,2,2,
				0,0,
				ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL | ETK_FILL_POLICY_HEXPAND);

	dialog->host_widget_entry = etk_entry_new();
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->host_widget_entry,1,1,2,2,
				0,0,
				ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL | ETK_FILL_POLICY_HEXPAND);

	dialog->username_widget_label = etk_label_new("Username");
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->username_widget_label,0,0,3,3,
				0,0,
				ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL | ETK_FILL_POLICY_HEXPAND);

	dialog->username_widget_entry = etk_entry_new();
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->username_widget_entry,1,1,3,3,
				0,0,
				ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL | ETK_FILL_POLICY_HEXPAND);


	dialog->password_widget_label = etk_label_new("Password");
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->password_widget_label,0,0,4,4,
				0,0,
				ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL | ETK_FILL_POLICY_HEXPAND);

	dialog->password_widget_entry = etk_entry_new();
	etk_table_attach(ETK_TABLE(dialog->vbox2), dialog->password_widget_entry,1,1,4,4,
				0,0,
				ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL | ETK_FILL_POLICY_HEXPAND);

	

	/*Setup the dialog buttons*/
	hbox = etk_hbox_new(ETK_FALSE,0);
	
	dialog_button = etk_button_new_with_label("Back");
	etk_box_pack_start(ETK_BOX(hbox), dialog_button, ETK_TRUE, ETK_TRUE, 5);
	

	dialog_button = etk_button_new_with_label("Next >");
	etk_box_pack_start(ETK_BOX(hbox), dialog_button, ETK_TRUE, ETK_TRUE, 5);
	etk_signal_connect("clicked", ETK_OBJECT(dialog_button), ETK_CALLBACK(_location_add_next_cb), dialog);

	dialog_button = etk_button_new_with_label("Cancel");
	etk_box_pack_start(ETK_BOX(hbox), dialog_button, ETK_TRUE, ETK_TRUE, 5);

	etk_container_add(ETK_CONTAINER(outer_vbox), hbox);

	etk_widget_show_all(dialog->window);
}



