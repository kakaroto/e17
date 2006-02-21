#include "entropy.h"
#include "entropy_gui.h"
#include <dlfcn.h>
#include <limits.h>
#include <Etk.h>

static int etk_callback_setup = 0;
static Ecore_Hash* instance_map_hash;

typedef struct entropy_etk_file_structure_viewer entropy_etk_file_structure_viewer;
struct entropy_etk_file_structure_viewer
{
  entropy_core *ecore;		/*A reference to the core object passed from init */
  //Etk_Row *current_row;
  Etk_Widget *tree;
  Etk_Tree_Col* tree_col1;
  Etk_Widget* parent_visual; 

  Ecore_Hash* row_hash;

  Ecore_List *gui_events;
  Ecore_List *files;		/*The entropy_generic_file references we copy. */

  Ecore_Hash *loaded_dirs;	/*A hash of the directories we have already loaded directories for - mostly for cleanup */
  Ecore_Hash *row_folder_hash;

  Etk_Widget *last_selected_label;
};

typedef struct event_file_core event_file_core;
struct event_file_core
{
  entropy_generic_file *file;
  entropy_gui_component_instance *instance;
  void *data;
};

int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_GUI_COMPONENT;
}

int
entropy_plugin_sub_type_get ()
{
  return ENTROPY_PLUGIN_GUI_COMPONENT_STRUCTURE_VIEW;
}

char *
entropy_plugin_identify ()
{
  return (char *) "File system tree structure viewer";
}

char*
entropy_plugin_toolkit_get() 
{
	return ENTROPY_TOOLKIT_ETK;
}


static void _etk_structure_viewer_xdnd_drag_drop_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Selection_Request *ev;   
   Etk_Selection_Data_Files *files;
   Etk_Tree* tree;
   Etk_Tree_Row* row;
   int i;
   entropy_etk_file_structure_viewer* viewer;
   entropy_gui_component_instance* instance;
   event_file_core* e_event;
   
   ev = event;
   files = ev->data;
   tree = ETK_TREE(object);
   row = etk_tree_selected_row_get(tree);

   instance = ecore_hash_get(instance_map_hash, row);
   if (instance) {
	   viewer = instance->data;
	   e_event = ecore_hash_get(viewer->row_hash, row);
   
	   if(ev->content != ETK_SELECTION_CONTENT_FILES) {
	     printf("Drop wasn't files!\n");
	     return;
	   }

	   printf("We received %d files\n", files->num_files);
	   for (i = 0; i < files->num_files; i++)
	     {
		     entropy_generic_file* file = entropy_core_uri_generic_file_retrieve(files->files[i]);
	     
		     if (!file) {
			     entropy_file_listener* listener = entropy_malloc(sizeof(entropy_file_listener));
			     file = entropy_core_parse_uri(files->files[i]);
			     listener->file = file;
			     listener->count = 1;
			     entropy_core_file_cache_add(file->md5, listener);
		     }
		     printf("File is '%s' ---> %p\n", files->files[i], file);
		     printf("Destination: %s\n", e_event->file->uri);

		     entropy_plugin_filesystem_file_copy(file, e_event->file->uri, instance);
	     }
   } else {
	   printf("Could not get instance for dropped row!\n");
   }
}

static void _etk_structure_viewer_row_clicked(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up_Down *event, void *data)
{
   entropy_gui_component_instance* instance;
   entropy_etk_file_structure_viewer* viewer;
   entropy_gui_event *gui_event;
   event_file_core* e_event;
   
   
   instance = ecore_hash_get(instance_map_hash, row);
   if (instance) {
	   viewer = instance->data;
	   e_event = ecore_hash_get(viewer->row_hash, row);
	
   /*printf(_("Row clicked %p. Button: %d. "), row, event->button);
   if (event->flags & EVAS_BUTTON_TRIPLE_CLICK)
      printf(_("Triple Click\n"));
   else if (event->flags & EVAS_BUTTON_DOUBLE_CLICK)
      printf(_("Double Click\n"));
   else
      printf(_("Single Click\n"));*/

  //printf("Received row: %p, %p: %s\n", e_event, instance, e_event->file->path);
  
	  if (e_event) {
		  gui_event = entropy_malloc (sizeof (entropy_gui_event));
		  gui_event->event_type =
		    entropy_core_gui_event_get (ENTROPY_GUI_EVENT_ACTION_FILE);
		  gui_event->data = e_event->file;
		  entropy_core_layout_notify_event (e_event->instance, gui_event, ENTROPY_EVENT_GLOBAL);
	   }	
     }

   
}



void
structure_viewer_add_row (entropy_gui_component_instance * instance,
			  entropy_generic_file * file, Etk_Tree_Row * prow)
{
  Etk_Tree_Row* new_row;
  entropy_etk_file_structure_viewer* viewer;
  event_file_core *event;
  Etk_Tree_Col* col;
  Etk_Tree_Row* parent;

  viewer = instance->data;
  parent = (Etk_Tree_Row*)viewer->parent_visual;
  
  col = etk_tree_nth_col_get(ETK_TREE(parent->tree), 0);
  etk_tree_freeze(ETK_TREE(viewer->tree));
  
  if (!prow) {
	  new_row = etk_tree_append_to_row((Etk_Tree_Row*)viewer->parent_visual, col, 
			  etk_theme_icon_theme_get(), "places/user-home_16", _(file->filename), NULL);
  } else {
	  new_row = etk_tree_append_to_row(prow, col, 
			  etk_theme_icon_theme_get(), "places/user-home_16", _(file->filename), NULL);
  }

  ecore_hash_set(instance_map_hash, new_row, instance);
  
  event = entropy_malloc (sizeof (event_file_core));
  event->file = file;		/*Create a clone of this file, and add it to the event */
  event->instance = instance;
  
  ecore_hash_set(viewer->row_hash, new_row, event);
  ecore_hash_set(viewer->row_folder_hash, file, new_row);

  /*Save this file in this list of files we're responsible for */
  ecore_list_append (viewer->files, event->file);

  etk_tree_thaw(ETK_TREE(viewer->tree));
}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor,
		    void *el, entropy_gui_component_instance * comp)
{

  entropy_etk_file_structure_viewer *viewer =
    (entropy_etk_file_structure_viewer *) comp->data;

  switch (eevent->event_type) {
  	  case ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL:
	  case ENTROPY_NOTIFY_FILELIST_REQUEST:{
	      Etk_Tree_Row* row = NULL;
	      entropy_generic_file *file;
	      entropy_generic_file *event_file =
		((entropy_file_request *) eevent->data)->file;

	      row = ecore_hash_get (viewer->row_folder_hash, event_file);
	      if (row && !ecore_hash_get (viewer->loaded_dirs, row)) {
			ecore_list_goto_first (el);
			while ((file = ecore_list_next (el))) {

			  /*We need the file's mime type, 
			   * so get it here if it's not here already...*/
			  if (!strlen (file->mime_type)) {
			    entropy_mime_file_identify (comp->core->mime_plugins, file);
			  }

			  if (file->filetype == FILE_FOLDER ||
			      entropy_core_descent_for_mime_get (comp->core,
						 file->mime_type)) {
			      /*Tell the core we're watching 
			       * this file*/
			      entropy_core_file_cache_add_reference (file->md5);
			      structure_viewer_add_row (comp, file, row);
			      ecore_hash_set (viewer->loaded_dirs, row, (int*)1);
			  }
			  etk_tree_row_expand(row);
		}

	      }
	}
	      

  }

}



entropy_gui_component_instance *
entropy_plugin_init (entropy_core * core,
		     entropy_gui_component_instance * layout, void* parent_visual, void *data)
{	
  entropy_gui_component_instance *instance;	
  entropy_etk_file_structure_viewer *viewer;
   char      **dnd_types;
   int         dnd_types_num; 

    
  instance = entropy_gui_component_instance_new ();
  viewer = entropy_malloc (sizeof (entropy_etk_file_structure_viewer));

  viewer->files = ecore_list_new();
  viewer->row_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  viewer->row_folder_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  viewer->loaded_dirs = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  viewer->parent_visual = parent_visual;
  
  /*viewer->tree = etk_tree_new(); 
  etk_tree_mode_set(ETK_TREE(viewer->tree), ETK_TREE_MODE_TREE);
  viewer->tree_col1 = etk_tree_col_new(ETK_TREE(viewer->tree), _("Files"), 
		  etk_tree_model_icon_text_new(ETK_TREE(viewer->tree), ETK_TREE_FROM_EDJE), 60);
  etk_tree_col_expand_set(viewer->tree_col1, ETK_TRUE);
  etk_tree_build(ETK_TREE(viewer->tree));*/

  instance->data = viewer;
  instance->core = core;
  instance->gui_object = viewer->tree;

  instance->layout_parent = layout;

  /*Register out interest in receiving folder notifications */
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS_EXTERNAL));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_REMOVE_DIRECTORY));

  if (!etk_callback_setup) {
	  printf("ETK stuff setup! *******\n");
	  
	  etk_signal_connect("row_clicked", ETK_OBJECT( ((Etk_Tree_Row*)parent_visual)->tree  ), 
		  ETK_CALLBACK(_etk_structure_viewer_row_clicked), NULL);

	  /*Accept drops*/
	   dnd_types_num = 1;
	   dnd_types = calloc(dnd_types_num, sizeof(char*));
	   dnd_types[0] = strdup("text/uri-list");  
	   etk_widget_dnd_types_set(  ((Etk_Tree_Row*)parent_visual)->tree, 
	   			dnd_types, dnd_types_num);
	   etk_widget_dnd_dest_set( ((Etk_Tree_Row*)parent_visual)->tree  , ETK_TRUE);
	   etk_signal_connect("drag_drop", ETK_OBJECT( ((Etk_Tree_Row*)parent_visual)->tree  ), 
	   			ETK_CALLBACK(_etk_structure_viewer_xdnd_drag_drop_cb), NULL);

	  instance_map_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

	  etk_callback_setup = 1;
  }

  structure_viewer_add_row (instance, (entropy_generic_file *) data, NULL);

  printf("Initialising ETK structure viewer...%p\n", instance);


  return instance;

}

