#include "entropy.h"
#include "entropy_gui.h"
#include <dlfcn.h>
#include <limits.h>
#include <time.h>
#include <Etk.h>
#include "etk_progress_dialog.h"

static int etk_callback_setup = 0;
static Ecore_Hash* row_hash;

typedef struct entropy_etk_file_list_viewer entropy_etk_file_list_viewer;
struct entropy_etk_file_list_viewer
{
  entropy_core *ecore;		/*A reference to the core object passed from init */
  //Etk_Row *current_row;
  Etk_Widget *tree;
  Etk_Tree_Col* tree_col1;
  Etk_Widget* parent_visual; 

  Ecore_Hash* row_hash;
  Ecore_Hash* gui_hash;

  Ecore_List *gui_events;
  Ecore_List *files;		/*The entropy_generic_file references we copy. */

  entropy_file_progress_window* progress;
  
  Etk_Widget *last_selected_label;
};

typedef struct event_file_core event_file_core;
struct event_file_core
{
  entropy_generic_file *file;
  entropy_gui_component_instance *instance;
  void *data;
};

typedef struct gui_file gui_file;
struct gui_file
{
  entropy_generic_file *file;
  entropy_thumbnail *thumbnail;
  entropy_gui_component_instance *instance;
  Etk_Tree_Row *icon;
};


void
gui_file_destroy (gui_file * file)
{
  entropy_free (file);

}


int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_GUI_COMPONENT;
}

int
entropy_plugin_sub_type_get ()
{
  return ENTROPY_PLUGIN_GUI_COMPONENT_LOCAL_VIEW;
}

char *
entropy_plugin_identify ()
{
  return (char *) "ETK File system list viewer";
}

char*
entropy_plugin_toolkit_get() 
{
	return ENTROPY_TOOLKIT_ETK;
}



static void _entropy_etk_list_viewer_drag_begin_cb(Etk_Object *object, void *data)
{
   Etk_Tree *tree;
   const char **types;
   unsigned int num_types;
   void *drag_data;
   Etk_Drag *drag;
   Etk_Widget *image;
   entropy_gui_component_instance* instance;
   entropy_etk_file_list_viewer* viewer;
   int i=0;

   char buffer[8192]; /* Um - help - what do we size this to? */
   
   int count = 0;

   Evas_List* rows;

   instance = data;
   viewer = instance->data;

   tree = ETK_TREE(object);
   rows = etk_tree_selected_rows_get(tree);
   drag = (ETK_WIDGET(tree))->drag;

   count = evas_list_count(rows);
   bzero(buffer,8192);
   for (; rows; rows = rows->next ) {
	   printf("Row %p resolves to %p:%s!\n", rows->data, ecore_hash_get(row_hash, rows->data),
			   ((gui_file*)ecore_hash_get(row_hash, rows->data))->file->uri );
	   strcat(buffer, ((gui_file*)ecore_hash_get(row_hash, rows->data))->file->uri);
	   strcat(buffer, "\r\n");
   }
  

   types = calloc(1, sizeof(char*));
   num_types = 1;
   types[0] = strdup("text/uri-list");
    

   printf("Drag buffer: %s\n", buffer);
   
   etk_drag_types_set(drag, types, num_types);
   etk_drag_data_set(drag, buffer, strlen(buffer)+1);



   
   /*image = etk_image_new_from_file(icol1_string);
   etk_image_keep_aspect_set(ETK_IMAGE(image), ETK_TRUE);
   etk_widget_size_request_set(image, 96, 96);
   etk_container_add(ETK_CONTAINER(drag), image);*/

   evas_list_free(rows);
}


void
gui_object_destroy_and_free (entropy_gui_component_instance * comp,
			     Ecore_Hash * gui_hash)
{

  Ecore_List *list;
  entropy_generic_file *obj;
  gui_file *freeobj;
  Etk_Tree_Row* row;
  entropy_etk_file_list_viewer *view = comp->data;

  /*Temporarily stop callbacks, we don't want to clobber an in-op process */
  entropy_notify_lock_loop (comp->core->notify);

  list = ecore_hash_keys (gui_hash);

  ecore_list_goto_first (list);
  while ((obj = ecore_list_next (list))) {


    freeobj = ecore_hash_get (gui_hash, obj);
    if (freeobj) {
      /*De-Associate this icon with this file in the core, so DND works */
      entropy_core_object_file_disassociate (freeobj->icon);

      gui_file_destroy (freeobj);
    }

    /*Tell the core we no longer need this file - it might free it now */
    entropy_core_file_cache_remove_reference (obj->md5);
  }
  ecore_hash_destroy (gui_hash);
  view->gui_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  ecore_list_destroy (list);


  ecore_list_goto_first(view->files);
  while ((row = ecore_list_remove_first(view->files))) {
	  ecore_hash_remove(row_hash, row);
  }

  entropy_notify_unlock_loop (comp->core->notify);


}


static void _etk_list_viewer_row_clicked(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up_Down *event, void *data)
{
   entropy_gui_component_instance* instance;
   entropy_etk_file_list_viewer* viewer;
   entropy_gui_event *gui_event;
   gui_file* file;
   
  
   file = ecore_hash_get(row_hash, row);
   instance = file->instance;
   viewer = instance->data;
	

	  
   if (event->flags & EVAS_BUTTON_DOUBLE_CLICK) {
	   printf("Row clicked, file is: %s\n", file->file->filename); 

	  gui_event = entropy_malloc (sizeof (entropy_gui_event));
	  gui_event->event_type =
	    entropy_core_gui_event_get (ENTROPY_GUI_EVENT_ACTION_FILE);
	  gui_event->data = file->file;
	  entropy_core_layout_notify_event (file->instance, gui_event, ENTROPY_EVENT_GLOBAL);
   }
}



void
list_viewer_add_row (entropy_gui_component_instance * instance,
			  entropy_generic_file * file)
{
  Etk_Tree_Row* new_row;
  entropy_etk_file_list_viewer* viewer;
  gui_file *e_file;
  Etk_Tree_Col* col1;
  Etk_Tree_Col* col2;
  Etk_Tree_Col* col3;
  Etk_Tree_Col* col4;
  Etk_Tree_Col* col5;
  char buffer[50];


  viewer = instance->data;
  
  col1 = etk_tree_nth_col_get(ETK_TREE(viewer->tree), 0);
  col2 = etk_tree_nth_col_get(ETK_TREE(viewer->tree), 1);
  col3 = etk_tree_nth_col_get(ETK_TREE(viewer->tree), 2);
  col4 = etk_tree_nth_col_get(ETK_TREE(viewer->tree), 3);
  col5 = etk_tree_nth_col_get(ETK_TREE(viewer->tree), 4);
  
  
  etk_tree_freeze(ETK_TREE(viewer->tree));
  
  if (!file->retrieved_stat) {
	  new_row = etk_tree_append(ETK_TREE(viewer->tree), 
		  col1, PACKAGE_DATA_DIR "/icons/default.png", 
		  col2,   file->filename, NULL);
  } else {
	 // time_t stime = file->properties.st_mtime
	  
	  snprintf(buffer,50, "%d Kb", file->properties.st_size / 1024);
	  new_row = etk_tree_append(ETK_TREE(viewer->tree), 
		  col1, PACKAGE_DATA_DIR "/icons/default.png", 
		  col2,   file->filename,
		  col3,   buffer,
		  col4,   file->mime_type,
		  col5,   ctime(&file->properties.st_mtime),
		  NULL);
	  
  }

  e_file = entropy_malloc(sizeof(gui_file));
  e_file->file = file;		/*Create a clone of this file, and add it to the event */
  e_file->instance = instance;
  e_file->icon=new_row;

  ecore_hash_set(viewer->gui_hash, file, e_file);

  ecore_hash_set(row_hash, new_row, e_file);

  /*Save this file in this list of files we're responsible for */
  ecore_list_append (viewer->files, new_row);

  etk_tree_thaw(ETK_TREE(viewer->tree));
}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor,
		    void *el, entropy_gui_component_instance * comp)
{
  entropy_etk_file_list_viewer *viewer =
    (entropy_etk_file_list_viewer *) comp->data;

  switch (eevent->event_type) {
  	  case ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL:
	  case ENTROPY_NOTIFY_FILELIST_REQUEST:{
	      entropy_generic_file *file;

	      gui_object_destroy_and_free(comp, viewer->gui_hash);


	      etk_tree_clear(ETK_TREE(viewer->tree));

		ecore_list_goto_first (el);
		while ((file = ecore_list_next (el))) {

		  /*We need the file's mime type, 
		   * so get it here if it's not here already...*/
		  if (!strlen (file->mime_type)) {
		    entropy_mime_file_identify (comp->core->mime_plugins, file);
		  }

		  if (file->mime_type) {
		    entropy_plugin* thumb = entropy_thumbnailer_retrieve (file->mime_type);
		    if (thumb) {
				entropy_thumbnail_request *request = entropy_thumbnail_request_new ();
				request->file = file;
				request->instance = comp;

				entropy_notify_event *ev =
				  entropy_notify_request_register (comp->core->notify, comp,
					   ENTROPY_NOTIFY_THUMBNAIL_REQUEST,
					   thumb,
					   "entropy_thumbnailer_thumbnail_get",
					   request, NULL);

				entropy_notify_event_callback_add (ev, (void *) gui_event_callback,
					   comp);
				entropy_notify_event_cleanup_add (ev, request);

				entropy_notify_event_commit (comp->core->notify, ev);
		    }
		  }

		      /*Tell the core we're watching 
		       * this file*/
		      entropy_core_file_cache_add_reference (file->md5);
		      list_viewer_add_row (comp, file);
		}

	      }
	      break;

     case ENTROPY_NOTIFY_FILE_CREATE:{
      //printf ("Received file create event at icon viewer for file %s \n", ((entropy_generic_file*)ret)->filename);
      list_viewer_add_row (comp, (entropy_generic_file *) el);				      
     }
     break;	  

    case ENTROPY_NOTIFY_FILE_PROGRESS:{
   	entropy_etk_file_list_viewer *view = comp->data;
	entropy_file_progress *progress = el;

	if (!view->progress)
		view->progress = entropy_etk_progress_window_create();

	entropy_etk_progress_dialog_show(view->progress);
	entropy_etk_progress_dialog_set_file_from_to(view->progress, progress->file_from, progress->file_to);
	entropy_etk_progress_dialog_set_progress_pct(view->progress, &progress->progress);

	if (progress->type == TYPE_END)
		entropy_etk_progress_dialog_hide(view->progress);

	
     }
     break;

     case ENTROPY_NOTIFY_THUMBNAIL_REQUEST:{

   	   /*Only bother if we have a thumbnail, and a component */
	      if (el && comp) {
		gui_file *obj;
		entropy_thumbnail *thumb = (entropy_thumbnail *) el;
		entropy_etk_file_list_viewer *view = comp->data;
	
		obj = ecore_hash_get (view->gui_hash, thumb->parent);

		if (obj) {
		  Etk_Tree_Col* col1;
		  obj->thumbnail = thumb;

		  col1 = etk_tree_nth_col_get(ETK_TREE(viewer->tree), 0);
		  etk_tree_freeze(ETK_TREE(viewer->tree));

		  etk_tree_row_fields_set((Etk_Tree_Row*)obj->icon, 
		  col1, obj->thumbnail->thumbnail_filename, 
		  NULL);

		  etk_tree_thaw(ETK_TREE(viewer->tree));

		} else {
		  printf ("ERR: Couldn't find a hash reference for this file!\n");
		}
	      }
	    }				//End case
	    break;					    
	      
  }

}



entropy_gui_component_instance *
entropy_plugin_init (entropy_core * core,
		     entropy_gui_component_instance * layout, void *data)
{	
  entropy_gui_component_instance *instance;	
  entropy_etk_file_list_viewer *viewer;
  char  **dnd_types;
  int dnd_types_num=0;

    
  instance = entropy_gui_component_instance_new ();
  viewer = entropy_malloc (sizeof (entropy_etk_file_list_viewer));

  viewer->files = ecore_list_new();
  viewer->row_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  viewer->gui_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  
  viewer->tree = etk_tree_new(); 
  etk_tree_mode_set(ETK_TREE(viewer->tree), ETK_TREE_MODE_LIST);
 
  viewer->tree_col1 = etk_tree_col_new(ETK_TREE(viewer->tree), _("Icon"), 
		  etk_tree_model_image_new(ETK_TREE(viewer->tree), ETK_TREE_FROM_FILE), 48);
  /* Perhaps this is better than expanding it? -- CodeWarrior
   * etk_tree_col_expand_set(viewer->tree_col1, ETK_TRUE);
  */
  
  viewer->tree_col1 = etk_tree_col_new(ETK_TREE(viewer->tree), _("Filename"), 
		  etk_tree_model_text_new(ETK_TREE(viewer->tree)), 100);
  etk_tree_col_expand_set(viewer->tree_col1, ETK_TRUE);

  viewer->tree_col1 = etk_tree_col_new(ETK_TREE(viewer->tree), _("Size"), 
		  etk_tree_model_text_new(ETK_TREE(viewer->tree)),40);
  etk_tree_col_expand_set(viewer->tree_col1, ETK_TRUE);

  viewer->tree_col1 = etk_tree_col_new(ETK_TREE(viewer->tree), _("Type"), 
		  etk_tree_model_text_new(ETK_TREE(viewer->tree)),40);
  etk_tree_col_expand_set(viewer->tree_col1, ETK_TRUE);

  viewer->tree_col1 = etk_tree_col_new(ETK_TREE(viewer->tree), _("Date Modified"), 
		  etk_tree_model_text_new(ETK_TREE(viewer->tree)),70);
  etk_tree_col_expand_set(viewer->tree_col1, ETK_TRUE);


  /*DND Setup*/
   dnd_types_num = 1;
   dnd_types = calloc(dnd_types_num, sizeof(char*));
   dnd_types[0] = strdup("text/uri-list");  
  etk_widget_dnd_source_set(viewer->tree, ETK_TRUE);
  etk_widget_dnd_drag_widget_set(viewer->tree, etk_button_new_with_label("Drag Widget"));
  //etk_widget_dnd_drag_data_set(viewer->tree, dnd_types, dnd_types_num, "This is the drag data!", strlen("This is the drag data!") + 1);
  etk_signal_connect("drag_begin", viewer->tree , ETK_CALLBACK(_entropy_etk_list_viewer_drag_begin_cb), instance);
  etk_tree_multiple_select_set(ETK_TREE(viewer->tree), ETK_TRUE); 
  etk_tree_build(ETK_TREE(viewer->tree));

  etk_widget_size_request_set(viewer->tree, 600, 600);

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

  /*Register our interest in receiving file mod/create/delete notifications */
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_CHANGE));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_CREATE));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_REMOVE));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_REMOVE_DIRECTORY));

  /*Register interest in getting stat events */
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_STAT));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_STAT_AVAILABLE));

  /*We want to know about file transfer progress events */
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_PROGRESS));

  /*We want to know if the backend needs feedback */
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_USER_INTERACTION_YES_NO_ABORT));

  /*We want to know about thumbnail available events */
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_THUMBNAIL_AVAILABLE));

  
  if (!etk_callback_setup) {
	  etk_callback_setup = 1;
	  row_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  }

  etk_signal_connect("row_clicked", ETK_OBJECT( viewer->tree  ), 
		  ETK_CALLBACK(_etk_list_viewer_row_clicked), NULL);

  printf("Initialising ETK list viewer...%p\n", instance);

  return instance;

}

