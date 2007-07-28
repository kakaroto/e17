#include "entropy.h"
#include "entropy_gui.h"
#include <dlfcn.h>
#include <limits.h>
#include <Etk.h>
#include "etk_progress_dialog.h"
#include "entropy_etk_context_menu.h"

static int etk_callback_setup = 0;
static Ecore_Hash* instance_map_hash = NULL;
static Ecore_Hash* tree_map_hash = NULL;

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

   /*FIXME - DND is disabled during the transition to tree*/
   return;
   
   ev = event;
   files = ev->data;
   tree = ETK_TREE(object);
   //row = etk_tree_selected_row_get(tree);

   instance = ecore_hash_get(instance_map_hash, row);
   if (instance) {
	   Ecore_List* files_copy;
	   
	   viewer = instance->data;
	   e_event = ecore_hash_get(viewer->row_hash, row);
   
	   if (e_event) {
		   if(ev->content != ETK_SELECTION_CONTENT_FILES) {
		     printf("Drop wasn't files!\n");
		     return;
		   }

		   printf("We received %d files\n", files->num_files);

		   files_copy = ecore_list_new();
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

			     ecore_list_append(files_copy, file);
	     
		     }
		   entropy_plugin_filesystem_file_copy_multi(files_copy, e_event->file->uri, instance);
		   ecore_list_destroy(files_copy);
	    }
   } else {
	   printf("Could not get instance for dropped row!\n");
   }
}

static void _etk_structure_viewer_row_clicked(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Down *event, void *data)
{
   entropy_gui_component_instance* instance;
   entropy_etk_file_structure_viewer* viewer;
   entropy_gui_event *gui_event;
   event_file_core* e_event;

   printf("Structure event: %d\n", event->button);

   if ( (!(event->button == 1 || event->button == 3)) || event->flags & ETK_MOUSE_DOUBLE_CLICK
	|| event->flags & ETK_MOUSE_TRIPLE_CLICK )
	   return;

   printf("Post\n");
   
   instance = ecore_hash_get(instance_map_hash, row);
   etk_tree_row_select(row);
   if (instance) {
	   viewer = instance->data;
	   e_event = ecore_hash_get(viewer->row_hash, row);

	  if (e_event) {
		  if (event->button == 1) {
		  	gui_event = entropy_malloc (sizeof (entropy_gui_event));
			  gui_event->event_type =
			    entropy_core_gui_event_get (ENTROPY_GUI_EVENT_ACTION_FILE);
			  gui_event->data = e_event->file;
			  entropy_core_layout_notify_event (e_event->instance, gui_event, ENTROPY_EVENT_GLOBAL);
		  } else if (event->button == 3) {
			entropy_etk_context_menu_popup(instance, e_event->file);
		  }
	  }
     }

   
}



Etk_Tree_Row*
structure_viewer_add_row (entropy_gui_component_instance * instance,
			  entropy_generic_file * file, Etk_Tree_Row * prow)
{
  Etk_Tree_Row* new_row;
  entropy_etk_file_structure_viewer* viewer;
  event_file_core *event;
  Etk_Tree_Col* col;
  Etk_Tree_Row* parent;
  char* thumbnail_filename;
  char* thumbnail_key;

  viewer = instance->data;
  parent = (Etk_Tree_Row*)viewer->parent_visual;
  
  col = etk_tree_nth_col_get(ETK_TREE(parent->tree), 0);
  etk_tree_freeze(ETK_TREE(viewer->tree));

  /* [TODO] Make ETK_STOCK_BIG changable from options */
  thumbnail_filename= etk_theme_icon_path_get();
  thumbnail_key = etk_stock_key_get(ETK_STOCK_PLACES_FOLDER, ETK_STOCK_BIG);
  
  if (!prow) {
	  new_row = etk_tree_row_append( ((Etk_Tree_Row*)viewer->parent_visual)->tree, (Etk_Tree_Row*)viewer->parent_visual, col,
      thumbnail_filename, thumbnail_key, _(file->filename), NULL);
  } else {
	  new_row = etk_tree_row_append( ((Etk_Tree_Row*)prow)->tree, (Etk_Tree_Row*)prow, col, 
      thumbnail_filename, thumbnail_key, _(file->filename), NULL);
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

  return new_row;
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

	      /*If we don't have a file's row-parent, try traversing up the tree
	       * to find it*/
	      if (!row) {
		      entropy_generic_file* traverse_file = event_file;
		      Ecore_List* traverse_stack;
		      int hit = 0;

		      traverse_stack = ecore_list_new();
		      /*Attempt to traverse up the tree...*/
		      while ( (traverse_file = entropy_core_parent_folder_file_get(traverse_file))) {
			      if (ecore_hash_get(viewer->row_folder_hash, traverse_file)) {
				      row = ecore_hash_get(viewer->row_folder_hash, traverse_file);
				      hit = 1;
				      goto done;
			      }

			      ecore_list_prepend(traverse_stack, traverse_file);
		      }

		      done:
		      if (hit) {
			    while ((traverse_file = ecore_list_first_remove(traverse_stack))) {
				    row = structure_viewer_add_row(comp, traverse_file, row);
				    entropy_core_file_cache_add_reference (traverse_file->md5);
			    }

			    row = structure_viewer_add_row(comp, event_file, row);
			    entropy_core_file_cache_add_reference (event_file->md5);
		      }

		      ecore_list_destroy(traverse_stack);
	      }
	      
	      if (row)
		      etk_tree_row_select(row);
	      
	      if (row) {
			ecore_list_first_goto (el);
			while ((file = ecore_list_next (el))) {

			  if (!ecore_hash_get(viewer->row_folder_hash, file)) {
				  /*We need the file's mime type, 
				   * so get it here if it's not here already...*/
				  /*printf("STRUCTURE REFERENCES FILE: %p\n", file);*/
				  if (!strlen (file->mime_type)) {
				    entropy_mime_file_identify (file);
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
				  etk_tree_row_unfold(row);
			   }
			 }
			  

	      }
	  }
	  break;

         case ENTROPY_NOTIFY_FILE_CREATE:{
						 
		entropy_generic_file* file = el;
		entropy_generic_file* parent_file = NULL;
		Etk_Tree_Row* row = NULL;

		parent_file = entropy_core_parent_folder_file_get(file);

		/*If we have a parent file..*/
		if (parent_file && (file->filetype == FILE_FOLDER || 
			(entropy_core_descent_for_mime_get (comp->core,
						 file->mime_type)))) {
			
			row = ecore_hash_get (viewer->row_folder_hash, parent_file);
			if (row) {
			      entropy_core_file_cache_add_reference (file->md5);
			      structure_viewer_add_row (comp, file, row);
			}
		}
         }
         break;


	 case ENTROPY_NOTIFY_FILE_REMOVE_DIRECTORY: {
		entropy_generic_file* file = el;
		Etk_Tree_Row* row = NULL;
	
		row = ecore_hash_get (viewer->row_folder_hash, file);
		if (row) {
			etk_tree_row_delete(row);
			ecore_hash_remove(viewer->row_folder_hash, file);
		}
		
	 }
	 break;
						     
	      
	default: break;
  }

}


Entropy_Plugin*
entropy_plugin_init (entropy_core * core)
{
  Entropy_Plugin_Gui* plugin;
  Entropy_Plugin* base;
	
  plugin = entropy_malloc(sizeof(Entropy_Plugin_Gui));
  base = ENTROPY_PLUGIN(plugin);
  
  return ENTROPY_PLUGIN(plugin);
}


entropy_gui_component_instance *
entropy_plugin_gui_instance_new (entropy_core * core,
		     entropy_gui_component_instance * layout, void* parent_visual, void *data)
{	
  entropy_gui_component_instance *instance;	
  entropy_etk_file_structure_viewer *viewer;

    
  instance = entropy_gui_component_instance_new ();
  viewer = entropy_malloc (sizeof (entropy_etk_file_structure_viewer));

  viewer->files = ecore_list_new();
  viewer->row_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  viewer->row_folder_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  viewer->loaded_dirs = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  viewer->parent_visual = parent_visual;
  
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

  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_CREATE));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_REMOVE_DIRECTORY));
  

  if (!etk_callback_setup) {
	  printf("ETK stuff setup! *******\n");
	  
	  instance_map_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
	  tree_map_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

	  etk_callback_setup = 1;
  }

  if (!ecore_hash_get(tree_map_hash, ((Etk_Tree_Row*)parent_visual)->tree)) {
	  etk_signal_connect("row-clicked", ETK_OBJECT( ((Etk_Tree_Row*)parent_visual)->tree  ), 
		  ETK_CALLBACK(_etk_structure_viewer_row_clicked), NULL);



	  /*Accept drops*/
	   const char  *dnd_types[] = { "text/uri-list" };
	   int          dnd_types_num; 

	   dnd_types_num = 1;
	   etk_widget_dnd_types_set(  ETK_WIDGET(((Etk_Tree_Row*)parent_visual)->tree), 
	   			dnd_types, dnd_types_num);
	   etk_widget_dnd_dest_set( ETK_WIDGET(((Etk_Tree_Row*)parent_visual)->tree)  , ETK_TRUE);
	   
	   
	   /*FIXME :: Disabled until migration to tree2 is done*/
	   /*etk_signal_connect("drag_drop", ETK_OBJECT( ((Etk_Tree_Row*)parent_visual)->tree  ), 
	   			ETK_CALLBACK(_etk_structure_viewer_xdnd_drag_drop_cb), NULL);*/

	   ecore_hash_set(tree_map_hash, ((Etk_Tree_Row*)parent_visual)->tree, (int*)1);
 
  }

  structure_viewer_add_row (instance, (entropy_generic_file *) data, NULL);

  printf("Initialising ETK structure viewer...%p\n", instance);


  return instance;

}

