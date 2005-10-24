#include <Ewl.h>
#include "entropy.h"
#include "entropy_gui.h"
#include <dlfcn.h>


typedef struct entropy_file_structure_viewer entropy_file_structure_viewer;
struct entropy_file_structure_viewer {
	entropy_core* ecore; /*A reference to the core object passed from init */
	Ewl_Row* current_row;
	Ewl_Widget* tree;

	Ecore_List* gui_events;
	Ecore_List* files; /*The entropy_generic_file references we copy.*/

	Ecore_Hash* loaded_dirs; /*A hash of the directories we have already loaded directories for - mostly for cleanup*/
	Ecore_Hash* row_folder_hash;

	Ewl_Widget* last_selected_label;
};

typedef struct event_file_core event_file_core;
struct event_file_core {
	entropy_generic_file* file;
	entropy_gui_component_instance* instance;
	void* data;
};

void structure_viewer_add_row(entropy_gui_component_instance* instance, entropy_generic_file* file, Ewl_Row* prow);


/*void entropy_gui_component_instance_destroy(entropy_gui_component* comp) {
}*/

int entropy_plugin_type_get() {
        return ENTROPY_PLUGIN_GUI_COMPONENT;
}

int entropy_plugin_sub_type_get() {
	return ENTROPY_PLUGIN_GUI_COMPONENT_STRUCTURE_VIEW;
}

char* entropy_plugin_identify() {
	        return (char*)"File system tree structure viewer";
}

/*Ewl_Widget* entropy_plugin_gui_component_visual_get() {
	return itree;
}*/

void gui_event_callback(entropy_notify_event* eevent, void* requestor, void* el, entropy_gui_component_instance* comp) {
   entropy_file_structure_viewer* viewer = (entropy_file_structure_viewer*)comp->data;

   switch (eevent->event_type) {
       case ENTROPY_NOTIFY_FILE_REMOVE_DIRECTORY: {
		entropy_generic_file* event_file = (entropy_generic_file*)el;
       
		printf("Received a remove directory notify at structure viewer\n");
		
		Ewl_Row* row = ecore_hash_get(viewer->row_folder_hash, event_file);
		if (row) {
			ewl_tree_row_destroy(viewer->tree, row);
		}
	}
	break;
   
      case ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL:
      case ENTROPY_NOTIFY_FILELIST_REQUEST: {
							  
	entropy_generic_file* file;
	

	/*We only want folder events from outselves, so leave otherwise..*/
	//if (requestor != comp) {
	//	return;
	//}


	entropy_generic_file* event_file = ((entropy_file_request*)eevent->data)->file;
	//printf("Looking for row for file %p\n", event_file);
	
	Ewl_Row* row = ecore_hash_get(viewer->row_folder_hash, event_file);
	//printf ("   Got %p\n", row);

	/*If we don't own this row, forget about doing something - we don't know about this*/
	if (row && !ecore_hash_get(viewer->loaded_dirs, row)) {


			ecore_list_goto_first(el);
			while ( (file = ecore_list_next(el)) ) {
			
				if (file->filetype == FILE_FOLDER) {
					char *c = entropy_malloc(sizeof(char));
					*c = 1;

					/*Tell the core we're watching this file*/
					entropy_core_file_cache_add_reference(comp->core, file->md5);
					structure_viewer_add_row(comp, file, row);
					ecore_hash_set(viewer->loaded_dirs, row, c);
				}
			}
		/*ecore_list_destroy(el);*/

		ewl_tree_row_expand_set(row, EWL_TREE_NODE_EXPANDED);
		
	} else {
		//printf ("We don't own this row\n");
		/*printf("---------------------------------------------> This row already has children!!\n");*/
	}
      }
      break;
   }

}

void row_clicked_callback(Ewl_Widget *main_win, void *ev_data, void *user_data) {
	event_file_core* event = (event_file_core*)user_data;
	entropy_file_structure_viewer* viewer = (entropy_file_structure_viewer*)event->instance->data;
	entropy_gui_event* gui_event;
	

	//printf("Clicked on %s%s\n", event->file->path, event->file->filename);

	/*-----------*/
	/*Send an event to the core*/
	gui_event = entropy_malloc(sizeof(entropy_gui_event));
	gui_event->event_type = entropy_core_gui_event_get(ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS);
	gui_event->data = event->file;
	entropy_core_layout_notify_event(event->instance, gui_event, ENTROPY_EVENT_LOCAL); 

	if (viewer->last_selected_label) {
		ewl_text_cursor_position_set(EWL_TEXT(viewer->last_selected_label), 0);

		/*TODO theme this color stuff*/
		ewl_text_color_apply(EWL_TEXT(viewer->last_selected_label), 0, 0, 0, 255, ewl_text_length_get(EWL_TEXT(viewer->last_selected_label)));		
	}

	/*Highlight this row*/
	ewl_text_cursor_position_set(EWL_TEXT(event->data), 0);
	ewl_text_color_apply(EWL_TEXT(event->data), 0, 0, 255, 255, ewl_text_length_get(EWL_TEXT(event->data)));
	viewer->last_selected_label = event->data;
}

void structure_viewer_add_row(entropy_gui_component_instance* instance, entropy_generic_file* file, Ewl_Row* prow) {
		Ewl_Widget* row;
		Ewl_Widget* image;
		Ewl_Widget* hbox = ewl_hbox_new();
		Ewl_Widget* children[2];
		Ewl_Widget* label = ewl_text_new();
		ewl_text_text_set(EWL_TEXT(label), file->filename);
		event_file_core* event;
		entropy_file_structure_viewer* viewer = (entropy_file_structure_viewer*)instance->data;

		image = ewl_image_new();
		ewl_image_file_set(EWL_IMAGE(image), PACKAGE_DATA_DIR "/icons/folder_structure.png", NULL);
		ewl_widget_show(image);
		
		/*printf ("  Added %s'\n", file->filename);*/
		
		ewl_container_child_append(EWL_CONTAINER(hbox), image);
		ewl_container_child_append(EWL_CONTAINER(hbox), label);

		ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
		ewl_object_fill_policy_set(EWL_OBJECT(label), EWL_FLAG_FILL_HFILL);

		ewl_widget_show(label);
		ewl_widget_show(hbox);

		children[0] = hbox;
		children[1] = NULL;

		/*printf("Adding row %s to existing row\n", file->filename);*/
		row = ewl_tree_row_add(EWL_TREE(viewer->tree), prow, children);

		ewl_object_fill_policy_set(EWL_OBJECT(row), EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
		ewl_widget_show(row);

		event = entropy_malloc(sizeof(event_file_core)); 
		event->file = file; /*Create a clone of this file, and add it to the event */
		event->instance = instance;
		event->data = label; /*So we can highlight the current directory*/
		
		ecore_list_append(viewer->files, event->file); /*Save this file in this list of files we're responsible for */
		

		ewl_callback_append(row, EWL_CALLBACK_CLICKED, row_clicked_callback, event);

		
		ecore_list_append(viewer->gui_events, event);

		/*Add this row to our map of files -> rows */
		/*printf ("Adding row to hash %p, file %p\n", row, file);*/
		ecore_hash_set(viewer->row_folder_hash, file, row);

}



void entropy_plugin_destroy(entropy_gui_component_instance* comp) {
	//printf("Destroying structure viewer...\n");
}



entropy_gui_component_instance* entropy_plugin_init(entropy_core* core, entropy_gui_component_instance* layout, void* data) {
	entropy_gui_component_instance* instance;
	entropy_file_structure_viewer* viewer;
	Ewl_Widget* child;
	

	 /*entropy_file_request* file_request = entropy_malloc(sizeof(entropy_file_request));*/

	 instance = entropy_malloc(sizeof(entropy_gui_component_instance));
	 viewer = entropy_malloc(sizeof(entropy_file_structure_viewer));
	 instance->data = viewer;

	instance->layout_parent = layout;

	/*Register out interest in receiving folder notifications*/
	entropy_core_component_event_register(core, instance, entropy_core_gui_event_get(ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS));
	entropy_core_component_event_register(core, instance, entropy_core_gui_event_get(ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS_EXTERNAL));
	entropy_core_component_event_register(core, instance, entropy_core_gui_event_get(ENTROPY_GUI_EVENT_FILE_REMOVE_DIRECTORY));



	viewer->gui_events = ecore_list_new();
	viewer->files = ecore_list_new();
	viewer->ecore = core;
	instance->core = core;
	
	viewer->tree = ewl_tree_new(1);	
	ewl_tree_headers_visible_set(EWL_TREE(viewer->tree),0);
	viewer->loaded_dirs = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
	viewer->row_folder_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
	
	ewl_object_fill_policy_set(EWL_OBJECT(EWL_TREE(viewer->tree)->scrollarea), EWL_FLAG_FILL_VFILL | EWL_FLAG_FILL_HFILL);
	
	instance->gui_object = viewer->tree;

	structure_viewer_add_row(instance, (entropy_generic_file*)data, NULL);

	/*Prevent expand/collpase of root node*/
	child =ewl_container_child_get(EWL_CONTAINER(viewer->tree), 0);
	ewl_callback_del_type(EWL_TREE_NODE(child)->handle, EWL_CALLBACK_VALUE_CHANGED);
	
	
	//printf("..done\n");

	ewl_widget_show(viewer->tree);

	//printf("Returning instance..\n");

	

	//printf("Doing..\n");
	return instance;
}
