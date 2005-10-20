#include <Ewl.h>
#include "entropy.h"
#include <dlfcn.h>
#include <Ecore.h>
#include <stdlib.h>

static Ewl_Widget* win;
static Ecore_List* components;

void layout_ewl_simple_config_create(entropy_core* core);


typedef struct entropy_layout_gui entropy_layout_gui;
struct entropy_layout_gui {
	entropy_gui_component_instance* iconbox_viewer;
	entropy_gui_component_instance* structure_viewer;	
	Ewl_Widget* tree;
	Ecore_List* current_folder;

	/*Random things*/
	Ewl_Widget* location_add_name;
	Ewl_Widget* location_add_path;
};


void filesystem_combo_cb(Ewl_Widget *item, void *ev_data, void *combo) {
	//printf("Value changed callback\n");
	//ewl_combo_selected_set(EWL_COMBO(combo), EWL_WIDGET(item));
	//ewl_menu_item_text_set(EWL_MENU_ITEM(combo), ewl_text_text_get(EWL_TEXT(item)));
}


void location_add_cancel_cb(Ewl_Widget *item, void *ev_data, void *combo) {
	ewl_widget_destroy(item);
}

void location_add_cb(Ewl_Widget *main_win, void *ev_data, void *user_data) {
	entropy_gui_component_instance* instance = (entropy_gui_component_instance*)user_data;
	
	Ewl_Widget* window;
	Ewl_Widget* label;
	Ewl_Widget* vbox;
	Ewl_Widget* hbox;
	Ewl_Widget* combo;
	Ewl_Widget* item;
	Ewl_Widget* button;

	
	window = ewl_window_new();
	//ewl_object_custom_size_set(EWL_OBJECT(window), 300, 400);
	ewl_window_title_set(EWL_WINDOW(window),"Add Location");

	vbox = ewl_vbox_new();
	ewl_widget_show(vbox);
	ewl_container_child_append(EWL_CONTAINER(window), vbox);
	

	/*-------*/
	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);
	
	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), "Filesystem Type");
	ewl_container_child_append(EWL_CONTAINER(hbox), label);
	ewl_widget_show(label);

	combo = ewl_combo_new("System");
	item = ewl_menu_item_new();
	ewl_menu_item_text_set(EWL_MENU_ITEM(item), "smb://");
	ewl_container_child_append(EWL_CONTAINER(combo), item);
	ewl_widget_show(item);
	
	ewl_container_child_append(EWL_CONTAINER(hbox), combo);	
	//ewl_callback_append(EWL_WIDGET(item), EWL_CALLBACK_MOUSE_DOWN, filesystem_combo_cb, combo);
	ewl_widget_show(combo);
	/*--------------------------*/



	/*-------*/
	/*"Name"*/
	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);
	
	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), "Name");
	ewl_container_child_append(EWL_CONTAINER(hbox), label);
	ewl_widget_show(label);

	((entropy_layout_gui*)instance->data)->location_add_name = ewl_entry_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), ((entropy_layout_gui*)instance->data)->location_add_name);
	ewl_widget_show(((entropy_layout_gui*)instance->data)->location_add_name);
	/*-----------------------*/

	/*-------*/
	/*"Path"*/
	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_widget_show(hbox);
	
	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), "Path");
	ewl_container_child_append(EWL_CONTAINER(hbox), label);
	ewl_widget_show(label);

	/*entry = ewl_entry_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), entry);
	ewl_widget_show(entry);*/
	/*-----------------------*/

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Cancel");
	ewl_container_child_append(EWL_CONTAINER(vbox), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, location_add_cancel_cb, window);
	ewl_widget_show(button);
	

	ewl_widget_show(window);
}


void layout_ewl_simple_config_create(entropy_core* core) {
	char* eg = calloc(255, sizeof(char)) ;

	//printf("Setting config..\n");

	sprintf(eg, "Computer;posix:///|Home;posix://%s|Samba Example (Don't use!);smb://username:password@/test/machine/folder", entropy_core_home_dir_get(core));

	//printf("Setting default config string..\n");
	entropy_config_str_set("layout_ewl_simple", "structure_bar", eg);

	free(eg);
}


void layout_ewl_simple_add_header(entropy_gui_component_instance* instance, char* name, char* uri) {
                          Ewl_Widget* hbox;
                          Ewl_Widget* image;
			  Ewl_Widget* row;
                          Ewl_Widget* children[2];
			  entropy_plugin* structure;
			 void* (*structure_plugin_init)(entropy_core* core, entropy_gui_component_instance*, void* data);
	

			  
			  entropy_layout_gui* gui = ((entropy_layout_gui*)instance->data);
                          Ewl_Widget* label = ewl_text_new();
			  Ewl_Widget* tree= gui->tree;
			  ewl_text_text_set(EWL_TEXT(label), name);
                          image = ewl_image_new();
			  ewl_image_file_set(EWL_IMAGE(image), "../icons/chardevice.png", NULL);
                          hbox = ewl_hbox_new();
                          ewl_container_child_append(EWL_CONTAINER(hbox), image);
                          ewl_container_child_append(EWL_CONTAINER(hbox), label);

                          ewl_widget_show(label);
                          ewl_widget_show(image);
                          ewl_widget_show(hbox);
                          children[0] = hbox;
                          children[1] = NULL;
                          row = ewl_tree_row_add(EWL_TREE(tree), NULL, children);
			  ewl_object_fill_policy_set(EWL_OBJECT(row), EWL_FLAG_FILL_VSHRINK);
			 	 
                          ewl_widget_show(row);


			  /*Now attach an object to it*/
			  structure = entropy_plugins_type_get_first(instance->core->plugin_list, ENTROPY_PLUGIN_GUI_COMPONENT, ENTROPY_PLUGIN_GUI_COMPONENT_STRUCTURE_VIEW);

			  if (structure) {
				Ewl_Widget* children[2];
				Ewl_Widget* visual;
				Ewl_Widget* srow;
				
				entropy_generic_file* file = entropy_core_parse_uri(uri);

				//printf("***** Adding structure viewer\n");

				/*Main drive viewer*/
				{
					structure_plugin_init = dlsym(structure->dl_ref, "entropy_plugin_init");
					gui->structure_viewer = (*structure_plugin_init)(instance->core,instance,file);
					gui->structure_viewer->plugin = structure;
					visual = EWL_WIDGET(gui->structure_viewer->gui_object);
					if (!visual) printf("Alert! - Visual component not found\n");
					 else {} // printf("Visual component found\n");
					children[0] = EWL_WIDGET(visual);
					children[1] = NULL;
					srow=	ewl_tree_row_add(EWL_TREE(tree), EWL_ROW(row), children);
					ewl_object_fill_policy_set(EWL_OBJECT(srow), EWL_FLAG_FILL_HFILL);
					ewl_object_fill_policy_set(EWL_OBJECT(visual), EWL_FLAG_FILL_HFILL);
					ewl_widget_show(srow);
				}				  
			 }
			  

}

Ecore_Hash* layout_ewl_simple_parse_config(entropy_gui_component_instance* instance, char* config) {
	Ecore_Hash* ret = ecore_hash_new(ecore_str_hash, ecore_str_compare);

	if (!strstr(config, "|")) {
		char* name;
		char* uri;
		
		//printf("Simple case - only one object...\n");

		name = strtok(config, ";");
		uri = strtok(NULL, ";");

		//printf("Name/uri is %s %s\n", name,uri);

		layout_ewl_simple_add_header(instance, name,uri);
		


		/*Cut the obj up by semicolon;*/
		
	} else {
		Ecore_List* objects = ecore_list_new();
		char* object;
		char* name; char* uri;
		
		//printf("Complex case, multiple objects...\n");

		object = strtok(config, "|");
		ecore_list_append(objects,strdup(object));
		while ( (object = strtok(NULL, "|"))) {
			ecore_list_append(objects, strdup(object));
		}

		ecore_list_goto_first(objects);
		while ((object = ecore_list_next(objects))) {
			name = strtok(object, ";");
			uri = strtok(NULL, ";");

			//printf("Name/uri is %s %s\n", name,uri);

			layout_ewl_simple_add_header(instance, name,uri);

			free(object);
		}
		ecore_list_destroy(objects);

		
		
	}

	return ret;
}


void
 __resize_homedir(Ewl_Widget *obj, void *ev_data, void *user_data)
 {
	int h;


	 

	 //ewl_callback_del_type(EWL_WIDGET(obj), EWL_CALLBACK_CONFIGURE);

	 h = ewl_object_current_h_get(EWL_OBJECT(obj));
	//ewl_object_custom_h_set(EWL_WIDGET(user_data), h);	
	

	//ewl_callback_append(EWL_WIDGET(obj), EWL_CALLBACK_CONFIGURE, __resize_homedir, EWL_WIDGET(user_data));
	//printf("\n\n\n\n\nResize called, set to %d\n\n\n\n\n", h);
}


void
 __destroy_main_window(Ewl_Widget *main_win, void *ev_data, void *user_data)
 {
	entropy_core* core = (entropy_core*)user_data;
	 
        ewl_widget_destroy(main_win);
        ewl_main_quit();

	entropy_core_destroy(core);
	

 	exit(0);
 }

void
contract_cb(Ewl_Widget *main_win, void *ev_data, void *user_data)
 {
	Ewl_Box* box = EWL_BOX(user_data);

	ewl_object_maximum_w_set(EWL_OBJECT(box), 15);
 }

void
expand_cb(Ewl_Widget *main_win, void *ev_data, void *user_data)
 {
	Ewl_Box* box = EWL_BOX(user_data);

	ewl_object_minimum_w_set(EWL_OBJECT(box), 150);
 }

int entropy_plugin_type_get() {
        return ENTROPY_PLUGIN_GUI_LAYOUT;
}

char* entropy_plugin_identify() {
	        return (char*)"Simple EWL layout container";
}

void entropy_plugin_layout_main() {
	ewl_widget_show(win);
	ewl_main();
}


void entropy_plugin_destroy(entropy_gui_component_instance* comp) {
	//printf("Destroying layout...\n");
}


void entropy_plugin_init(entropy_core* core) {
	int i =0;
	char **c = NULL;
	/*Init ewl*/
	components = ecore_list_new();
	ewl_init(&i, c);
	//ewl_theme_name_set("e17");
}

void entropy_delete_current_folder(Ecore_List* el) {
	ecore_list_destroy(el);
}


entropy_gui_component_instance* entropy_plugin_layout_create(entropy_core* core) {
	entropy_gui_component_instance* layout;
	entropy_layout_gui* gui;
	char* tmp = NULL;
	
	
        /*EWL Stuff -----------------*/

	void* (*entropy_plugin_init)(entropy_core* core, entropy_gui_component_instance*);
	void* (*structure_plugin_init)(entropy_core* core, entropy_gui_component_instance*, void* data);
	
         Ewl_Widget* paned;
         Ewl_Widget* box;
         Ewl_Widget* tree;
         Ewl_Widget* vbox;
	 Ewl_Widget* hbox;
	 Ewl_Widget* contract_button;
	 Ewl_Widget* expand_button;
	 Ewl_Widget* scrollpane;
	 Ewl_Widget* add_button;
	 
	 entropy_plugin* plugin;
	 Ewl_Widget* iconbox;

	layout = entropy_malloc(sizeof(entropy_gui_component_instance));
	gui = entropy_malloc(sizeof(entropy_layout_gui));
	gui->current_folder = NULL;
	layout->data = gui;
	layout->core = core;

	/*Register this layout container with the core, so our children can get events*/
	entropy_core_layout_register(core, layout);

        /*---------------------------*/
	 /*HACK - get the iconbox - this should be configurable */
	plugin = entropy_plugins_type_get_first(core->plugin_list,  ENTROPY_PLUGIN_GUI_COMPONENT, ENTROPY_PLUGIN_GUI_COMPONENT_LOCAL_VIEW);
	if (plugin) {
		//printf("Plugin: %s\n", plugin->filename);
		entropy_plugin_init = dlsym(plugin->dl_ref, "entropy_plugin_init");
		gui->iconbox_viewer = (*entropy_plugin_init)(core,layout);
		gui->iconbox_viewer->plugin = plugin;
		iconbox = EWL_WIDGET(gui->iconbox_viewer->gui_object);
	} else {
		//printf("No visual component found! *****\n");
	}

	
	//printf("Initialising simple EWL layout manager...\n");

        /*EWL Setup*/
        win= ewl_window_new();
        box = ewl_vbox_new();
        vbox = ewl_vbox_new();
	hbox = ewl_hbox_new();
	
	scrollpane = ewl_scrollpane_new();
	contract_button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(contract_button), "<");
	expand_button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(expand_button), ">");
	

        tree = ewl_tree_new(1);
	gui->tree = tree;

        paned = ewl_hpaned_new();
        ewl_window_title_set(EWL_WINDOW(win), "Entropy");
        ewl_window_name_set(EWL_WINDOW(win), "Entropy");
        ewl_window_class_set(EWL_WINDOW(win), "Entropy");
        ewl_object_size_request(EWL_OBJECT(win), 800,600);

	ewl_object_maximum_size_set(EWL_OBJECT(contract_button), 20, 10);
	ewl_object_maximum_size_set(EWL_OBJECT(expand_button), 20, 10);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_VSHRINK);


	
	ewl_container_child_append(EWL_CONTAINER(hbox), expand_button);
	ewl_container_child_append(EWL_CONTAINER(hbox), contract_button);

	ewl_callback_append(EWL_WIDGET(contract_button), EWL_CALLBACK_CLICKED, contract_cb, EWL_PANED(paned)->first);
	ewl_callback_append(EWL_WIDGET(expand_button), EWL_CALLBACK_CLICKED, expand_cb, EWL_PANED(paned)->first);


	/*Set up the paned*/

	/*Button to add locations*/
	ewl_paned_active_area_set(EWL_PANED(paned), EWL_POSITION_LEFT);
	add_button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(add_button), "Add Location");
	ewl_container_child_append(EWL_CONTAINER(paned), add_button);
	
	ewl_object_maximum_h_set(EWL_OBJECT(add_button), 20);
	ewl_object_minimum_w_set(EWL_OBJECT(add_button), 250);

	
	ewl_callback_append(EWL_WIDGET(add_button), EWL_CALLBACK_CLICKED, location_add_cb, layout);
	ewl_widget_show(add_button);
	/*--------------------------*/
	
        ewl_container_child_append(EWL_CONTAINER(box),paned);
	//ewl_container_child_append(EWL_CONTAINER(scrollpane), tree);
	ewl_container_child_append(EWL_CONTAINER(paned), tree);
	ewl_paned_active_area_set(EWL_PANED(paned), EWL_POSITION_RIGHT);
	ewl_container_child_append(EWL_CONTAINER(paned), iconbox);


	/*This is cheating - break OO convention by accessing the internals of the struct.. but it doesn't work without this*/
        ewl_object_fill_policy_set(EWL_OBJECT(EWL_PANED(paned)->second), EWL_FLAG_FILL_NORMAL);
        ewl_object_fill_policy_set(EWL_OBJECT(EWL_PANED(paned)->first), EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VFILL );
	//ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VFILL);
        ewl_object_fill_policy_set(EWL_OBJECT(paned), EWL_FLAG_FILL_NORMAL);
        //ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_FILL | EWL_FLAG_FILL_HSHRINK);
	//ewl_object_fill_policy_set(EWL_OBJECT(scrollpane), EWL_FLAG_FILL_NORMAL);

	
	if (!(tmp = entropy_config_str_get("layout_ewl_simple", "structure_bar"))) {
		//printf ("Creating initial view for ewl_simple layout..\n");
		layout_ewl_simple_config_create(core);

		tmp = entropy_config_str_get("layout_ewl_simple", "structure_bar");
	}

	//printf("Config for layout is: '%s'\n", tmp);
	layout_ewl_simple_parse_config(layout, tmp);


	

		/*ewl_tree_node_expand(row);*/

		//printf("Showing widgets..\n");
                
                ewl_widget_show(box);
                ewl_widget_show(vbox);
		ewl_widget_show(hbox);
		ewl_widget_show(contract_button);
		ewl_widget_show(expand_button);
		ewl_widget_show(scrollpane);
                
                ewl_widget_show(paned);
                ewl_widget_show(tree);

		ewl_object_fill_policy_set(EWL_OBJECT(win), EWL_FLAG_FILL_ALL);
                ewl_container_child_append(EWL_CONTAINER(win), box);

	
	
                 ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
                            __destroy_main_window, core);


		ewl_container_child_resize(EWL_WIDGET(EWL_PANED(paned)->first), 80, EWL_ORIENTATION_HORIZONTAL);


		 layout->gui_object = win;
		 return layout;
}
