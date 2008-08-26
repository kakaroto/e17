#include <Ewl.h>
#include "entropy.h"
#include "entropy_gui.h"
#include "entropy_config.h"
#include "ewl_properties_dialog.h"
#include "ewl_progress_dialog.h"
#include <dlfcn.h>
#include <time.h>

#define DONT_DO_MIME 0
#define DO_MIME 1
#define ICON_ADD_COUNT 20
#define DEFAULT_BG PACKAGE_DATA_DIR "/icons/default_bg.jpg"

typedef struct gui_file gui_file;
struct gui_file
{
  entropy_generic_file *file;
  entropy_thumbnail *thumbnail;
  entropy_gui_component_instance *instance;
  Ewl_Widget *icon;
};
gui_file *gui_file_new ();
void gui_file_destroy (gui_file *);


typedef struct event_idle_processor
{
  entropy_core *core;
  Ecore_List *user_data;
  void *requestor;
  int count;
  int terminate;
} event_idle_processor;



void gui_event_callback (entropy_notify_event * eevent, void *requestor,
			 void *ret, void *user_data);
void ewl_icon_local_viewer_delete_selected (entropy_gui_component_instance *
					    instance);



typedef struct entropy_icon_viewer entropy_icon_viewer;
struct entropy_icon_viewer
{
  Ewl_Widget *list;
  Ecore_Hash *gui_hash;		/*A list of our current directory's files */
  Ecore_Hash *icon_hash;	/*A hash for ewl callbacks */
  Ecore_Hash *file_wait_list;	/*A hash of lists of files that we are waiting for a response to
				   e.g. waiting for a yes/no on delete confirm */

  int default_bg;


  entropy_file_progress_window *progress;
  Ewl_Widget *file_dialog;
  event_idle_processor *last_processor;

  char current_dir[1024];	/* We should handle this at the core.  FUTURE API TODO */
};

char*
entropy_plugin_toolkit_get() 
{
	return ENTROPY_TOOLKIT_EWL;
}


void
entropy_file_wait_list_add (entropy_icon_viewer * viewer, Ecore_List * list)
{
  ecore_hash_set (viewer->file_wait_list, list, list);
}



void
ewl_list_file_paste_cb (Ewl_Widget * w, void *ev_data, void *user_data)
{
  Ecore_List *selected;
  entropy_generic_file *file;
  entropy_gui_component_instance *instance =
    ((entropy_gui_component_instance *) user_data);
  entropy_plugin *plugin =
    entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL);

  void (*copy_func) (entropy_generic_file * source, char *dest_uri,
		     entropy_gui_component_instance * requester);


  /*Get the func ref */
  copy_func = dlsym (plugin->dl_ref, "entropy_filesystem_file_copy");

  //printf("Paste the following files:\n");

  selected = entropy_core_selected_files_get (instance->core);
  ecore_list_first_goto (selected);

  while ((file = ecore_list_next (selected))) {
    //printf("File '%s'\n", file->filename);
    (*copy_func) (file, ((entropy_icon_viewer *) instance->data)->current_dir,
		  instance);


  }


}

void
ewl_list_file_copy_cb (Ewl_Widget * w, void *ev_data, void *user_data)
{
  Ecore_List *icon_list;
  gui_file *file;
  Ewl_Iconbox_Icon *list_item;
  entropy_gui_component_instance *instance =
    (entropy_gui_component_instance *) user_data;

  /*Clear the existing contents */
  entropy_core_selected_files_clear (instance->core);

  //printf("Copy files to clipboard..\n");

  icon_list = ewl_tree_selected_get( EWL_TREE(((entropy_icon_viewer*)instance->data)->list) );

  ecore_list_first_goto (icon_list);
  while ((list_item = ecore_list_next (icon_list))) {
    file =
      ecore_hash_get (((entropy_icon_viewer *) instance->data)->icon_hash,
		      list_item);
    entropy_core_selected_file_add (file->file);



  }


  ecore_list_destroy (icon_list);




}


void
icon_properties_cb (Ewl_Widget * w, void *ev_data, void *user_data)
{
  entropy_gui_component_instance *instance =
    (entropy_gui_component_instance *) user_data;
  entropy_icon_viewer *viewer = instance->data;
  entropy_gui_event *gui_event;
  gui_file *local_file =
    ecore_hash_get (viewer->icon_hash,
		    EWL_ICONBOX (viewer->list)->select_icon);


  //Stat test..
  /*Send an event to the core */
  if (local_file) {

    gui_event = entropy_malloc (sizeof (entropy_gui_event));
    gui_event->event_type =
      entropy_core_gui_event_get (ENTROPY_GUI_EVENT_FILE_STAT);
    gui_event->data = local_file->file;
    entropy_core_layout_notify_event (instance, gui_event,
				      ENTROPY_EVENT_LOCAL);
  }
  else {
    printf ("Could not find selected icon!\n");
  }
}




void
icon_click_cb (Ewl_Widget * w, void *ev_data, void *user_data)
{
  Ewl_Event_Mouse_Down *ev = ev_data;
  entropy_gui_event *gui_event;
  gui_file *local_file =
    ecore_hash_get (((entropy_icon_viewer *) user_data)->icon_hash, w);

  if (!local_file) {
    printf ("*Alert* Couldn't find a local file reference for icon\n");
  }



  if (ev->clicks > 1) {

    if (ev->button == 1) {
      //printf("Icon clicked %d, widget %p!\n", ev->clicks, w);

      /*Send an event to the core */
      gui_event = entropy_malloc (sizeof (entropy_gui_event));
      gui_event->event_type =
	entropy_core_gui_event_get (ENTROPY_GUI_EVENT_ACTION_FILE);
      gui_event->data = local_file->file;
      entropy_core_layout_notify_event (local_file->instance, gui_event,
					ENTROPY_EVENT_GLOBAL);

    }
    else if (ev->button == 2) {

    }

  }




}


gui_file *
gui_file_new ()
{
  allocated_gui_file++;
  return entropy_malloc (sizeof (gui_file));
}

void
gui_file_destroy (gui_file * file)
{
  allocated_gui_file--;
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
  return (char *) "listviewer";
}

void
gui_object_destroy_and_free (entropy_gui_component_instance * comp,
			     Ecore_Hash * gui_hash)
{

  Ecore_List *list;
  entropy_generic_file *obj;
  gui_file *freeobj;
  entropy_icon_viewer *view = comp->data;

  /*Temporarily stop callbacks, we don't want to clobber an in-op process */
  entropy_notify_lock_loop (comp->core->notify);

  list = ecore_hash_keys (gui_hash);

  ecore_list_first_goto (list);
  while ((obj = ecore_list_next (list))) {


    freeobj = ecore_hash_get (gui_hash, obj);
    if (freeobj) {
      ewl_tree_row_destroy (EWL_TREE (view->list), EWL_ROW (freeobj->icon));

      /*De-Associate this icon with this file in the core, so DND works */
      entropy_core_object_file_disassociate (freeobj->icon);

      gui_file_destroy (freeobj);
    }

    /*Tell the core we no longer need this file - it might free it now */
    entropy_core_file_cache_remove_reference (obj->md5);
  }
  ecore_hash_destroy (gui_hash);
  ecore_list_destroy (list);

  entropy_notify_unlock_loop (comp->core->notify);


}


void
entropy_plugin_destroy (entropy_gui_component_instance * comp)
{
//      printf ("Destroying icon viewer...\n");
}




void
ewl_icon_local_viewer_delete_cb (Ewl_Widget * w, void *ev_data,
				 void *user_data)
{
  Ecore_List *file_list = user_data;
  const char *text = ewl_button_label_get (EWL_BUTTON (w));
  entropy_generic_file *file;
  entropy_gui_component_instance *instance;

  printf ("Delete callback!\n");
  if (!strcmp (text, "Yes")) {
    entropy_plugin *plugin;
    void (*del_func) (entropy_generic_file * source);


    printf ("Selected delete\n");

    ecore_list_first_goto (file_list);

    /*As mentioned below, this is awkward,
     * but we avoid a sep. data structure here,
     * as long as we make sure this is documented
     * i.e. - this first item on this list is the plugin
     * reference */
    instance = ecore_list_next (file_list);

    plugin =
      entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				      ENTROPY_PLUGIN_SUB_TYPE_ALL);

    /*Get the func ref */
    del_func = dlsym (plugin->dl_ref, "entropy_filesystem_file_remove");


    while ((file = ecore_list_next (file_list))) {
      printf ("Deleting '%s'\n", file->filename);

      (*del_func) (file);

      entropy_core_file_cache_remove_reference (file->md5);


    }

  }
  else {
    printf ("Selected cancel..\n");
  }



  /*Um...FIXME bad - we need to save a reference to the dialog somewhere */
  ewl_widget_destroy (w->parent->parent->parent);

  ecore_list_destroy (file_list);
}

void
ewl_icon_local_viewer_menu_delete_cb (Ewl_Widget * w, void *ev_data,
				      void *user_data)
{

  /*User_data is entropy_gui_component_instance */
  ewl_icon_local_viewer_delete_selected (user_data);
}

void
ewl_icon_local_viewer_delete_selected (entropy_gui_component_instance *
				       instance)
{
  entropy_icon_viewer *viewer = instance->data;

  Ecore_List *new_file_list = ecore_list_new ();
  Ecore_List *icon_list;
  gui_file *local_file;
  Ewl_Iconbox_Icon *list_item;



  Ewl_Widget *dialog_win;
  Ewl_Widget *dialog_label;
  Ewl_Widget *button;

  /*This is kind of awkward - the first item on the list is
   * the plugin instance reference*/
  ecore_list_append (new_file_list, instance);

  dialog_win = ewl_dialog_new ();
  ewl_window_title_set (EWL_WINDOW (dialog_win), "Delete?");

  ewl_dialog_active_area_set (EWL_DIALOG (dialog_win), EWL_POSITION_TOP);
  dialog_label = ewl_label_new ();
  ewl_label_text_set (EWL_LABEL (dialog_label),
		      "Are you sure you want to delete these files?");
  ewl_container_child_append (EWL_CONTAINER (dialog_win), dialog_label);
  ewl_widget_show (dialog_label);

  ewl_dialog_active_area_set (EWL_DIALOG (dialog_win), EWL_POSITION_BOTTOM);



  //////////////////////
  icon_list = ewl_tree_selected_get(EWL_TREE(viewer->list) );

  ecore_list_first_goto (icon_list);
  while ((list_item = ecore_list_next (icon_list))) {
    local_file = ecore_hash_get (viewer->icon_hash, list_item);
    if (local_file) {
      entropy_core_file_cache_add_reference (local_file->file->md5);
      ecore_list_append (new_file_list, local_file->file);
    }
  }
  entropy_file_wait_list_add (viewer, new_file_list);
  ecore_list_destroy (icon_list);


  /////////////////////
  //

  button = ewl_button_new ();
  ewl_button_label_set (EWL_BUTTON (button), "Yes");
  ewl_widget_show (button);
  ewl_container_child_append (EWL_CONTAINER (dialog_win), button);
  ewl_callback_append (button, EWL_CALLBACK_CLICKED,
		       ewl_icon_local_viewer_delete_cb, new_file_list);

  button = ewl_button_new ();
  ewl_button_label_set (EWL_BUTTON (button), "No");
  ewl_widget_show (button);
  ewl_container_child_append (EWL_CONTAINER (dialog_win), button);
  ewl_callback_append (button, EWL_CALLBACK_CLICKED,
		       ewl_icon_local_viewer_delete_cb, new_file_list);



  ewl_widget_show (dialog_win);
}


void
ewl_icon_local_viewer_key_event_cb (Ewl_Iconbox * ib, void *data, char *key)
{
  printf ("Received controlled key: '%s'\n", key);

  if (!strcmp (key, "Delete")) {
    ewl_icon_local_viewer_delete_selected (data);
  }
}


Entropy_Plugin*
entropy_plugin_init (entropy_core * core)
{
  Entropy_Plugin_Gui* plugin;
  Entropy_Plugin* base;
	
  plugin = entropy_malloc(sizeof(Entropy_Plugin_Gui));
  base = ENTROPY_PLUGIN(plugin);
  
  return plugin;
}


entropy_gui_component_instance *
entropy_plugin_gui_instance_new (entropy_core * core,
		     entropy_gui_component_instance * layout)
{
  char *headers[5];

  entropy_gui_component_instance *instance =
    entropy_gui_component_instance_new ();
  entropy_icon_viewer *viewer = entropy_malloc (sizeof (entropy_icon_viewer));



  /*Save a reference to our local data */
  instance->data = viewer;
  instance->layout_parent = layout;

  viewer->list = ewl_tree_new (4);
  ewl_tree_mode_set (EWL_TREE (viewer->list), EWL_SELECTION_MODE_MULTI);

  headers[0] = "Icon";
  headers[1] = "Filename";
  headers[2] = "Size";
  headers[3] = "Date Modified";
  headers[4] = NULL;
  ewl_tree_headers_set (EWL_TREE (viewer->list), headers);



  viewer->default_bg = 0;
  instance->gui_object = viewer->list;
  ewl_widget_show (EWL_WIDGET (viewer->list));


  /*Initialise the progress window */
  viewer->progress = entropy_malloc (sizeof (entropy_file_progress_window));

  /*Init the file wait list */
  viewer->file_wait_list =
    ecore_hash_new (ecore_direct_hash, ecore_direct_compare);


  /*Init the hash */
  viewer->gui_hash = ecore_hash_new (ecore_direct_hash, ecore_direct_compare);
  viewer->icon_hash =
    ecore_hash_new (ecore_direct_hash, ecore_direct_compare);

  /*Set the core back reference */
  instance->core = core;

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


  return instance;
}


void
ewl_icon_local_viewer_remove_icon (entropy_gui_component_instance * comp,
				   entropy_generic_file * list_item)
{
  entropy_icon_viewer *view = comp->data;
  gui_file *gui_object;

  if ((gui_object = ecore_hash_get (view->gui_hash, list_item))) {
    //FIXME
    ewl_tree_row_destroy (EWL_TREE (view->list), EWL_ROW (gui_object->icon));
  }
}


Ecore_List *
entropy_list_data_get (void)
{
  return NULL;
}


gui_file *
ewl_icon_local_viewer_add_icon (entropy_gui_component_instance * comp,
				entropy_generic_file * list_item, int do_mime)
{
  entropy_icon_viewer *view = comp->data;
  Ewl_Widget *text[5];
  char buf[50];


  Ewl_Widget *icon;
  gui_file *gui_object;

  if (!ecore_hash_get (view->gui_hash, list_item)) {
    entropy_core_file_cache_add_reference (list_item->md5);


    text[0] = ewl_image_new ();
    ewl_image_file_set (EWL_IMAGE (text[0]),
			PACKAGE_DATA_DIR "/icons/folder.png", NULL);
    ewl_image_constrain_set (EWL_IMAGE (text[0]), 20);
    ewl_image_proportional_set(EWL_IMAGE(text[0]), TRUE);
    ewl_widget_show (text[0]);


    text[1] = ewl_text_new ();
    ewl_text_text_set (EWL_TEXT (text[1]), list_item->filename);	/*Name */
    ewl_object_minimum_w_set(EWL_OBJECT(text[1]), 60);
    ewl_widget_show (text[1]);

    if (list_item->retrieved_stat) {
      snprintf (buf, 50, "%d kb",
		((int) list_item->properties.st_size / 1024));
      text[2] = ewl_text_new ();
      ewl_text_text_set (EWL_TEXT (text[2]), buf);
      ewl_widget_show (text[2]);
    }
    else {
      text[2] = ewl_text_new ();
      ewl_widget_show (text[2]);	/* Size */
    }
   ewl_object_minimum_w_set(EWL_OBJECT(text[2]), 30);


    if (list_item->retrieved_stat) {
      time_t stime;
      stime = list_item->properties.st_mtime;
      text[3] = ewl_text_new ();
      ewl_text_text_set (EWL_TEXT (text[3]), ctime (&stime));
      ewl_widget_show (text[3]);
    }
    else {
      text[3] = ewl_text_new ();
      ewl_widget_show (text[3]);	/*Mod time */
    }
    ewl_object_minimum_w_set(EWL_OBJECT(text[3]), 40);
    text[4] = NULL;

    icon = ewl_tree_row_add (EWL_TREE (view->list), NULL, text);
    ewl_widget_draggable_set (icon, 1, entropy_list_data_get);

    ewl_object_fill_policy_set (EWL_OBJECT (icon),
				EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_VFILL |
				EWL_FLAG_FILL_HFILL);

    ewl_callback_append (EWL_WIDGET (icon), EWL_CALLBACK_MOUSE_DOWN,
			 icon_click_cb, view);


    gui_object = gui_file_new ();
    gui_object->file = list_item;
    gui_object->thumbnail = NULL;
    gui_object->instance = comp;
    gui_object->icon = EWL_WIDGET (icon);



    ecore_hash_set (view->gui_hash, list_item, gui_object);
    ecore_hash_set (view->icon_hash, icon, gui_object);

    /*Associate this icon with this file in the core, so DND works */
    entropy_core_object_file_associate (icon, list_item);

    if (do_mime == DO_MIME) {
      char *mime;
      entropy_plugin *thumb;

      mime = entropy_mime_file_identify (list_item);


      if (mime && strcmp (mime, ENTROPY_NULL_MIME)) {
		entropy_plugin_thumbnail_request(comp, list_item, (void*)gui_event_callback);
      }
      else {
	thumb = NULL;
      }

    }

    return gui_object;

  }

  return NULL;

}


int
idle_add_icons (void *data)
{
  event_idle_processor *proc = (event_idle_processor *) data;
  entropy_gui_component_instance *comp =
    (entropy_gui_component_instance *) proc->requestor;
  entropy_icon_viewer *view = ((entropy_icon_viewer *) comp->data);

  Ecore_List *el = proc->user_data;
  entropy_generic_file *file;
  int i = 0;
  char *mime;
  entropy_plugin *thumb;
  Ecore_List *added_list = ecore_list_new ();
  Ecore_List *events;
  int term = 0;


  if (proc->terminate) {
    goto FREE_AND_LEAVE;
  }

  /*data = file list */


  while (i < ICON_ADD_COUNT && (file = ecore_list_first_remove (el))) {
    //printf("Adding '%s'\n", file->filename);
    ewl_icon_local_viewer_add_icon (proc->requestor, file, DONT_DO_MIME);

    entropy_core_file_cache_remove_reference (file->md5);
    
    ecore_list_append (added_list, file);

    i++;

  }
  if (!file)
    term = 1;

  events = ecore_list_new ();
  while ((file = ecore_list_first_remove (added_list))) {
    mime =
      (char *) entropy_mime_file_identify (file);


    if (mime && strcmp (mime, ENTROPY_NULL_MIME)) {
	entropy_plugin_thumbnail_request(comp,file,(void*)gui_event_callback);
    }
    else {
      thumb = NULL;
    }
  }

  ecore_list_destroy (added_list);

  if (!term) {
    proc->count += ICON_ADD_COUNT;
    //printf("Continuing process thread..(%d)\n", proc->count);
    return 1;
  }
  else {
    view->last_processor = NULL;
    //printf("Terminated process thread..\n");
    goto FREE_AND_LEAVE;

    return 0;
  }


FREE_AND_LEAVE:
  ecore_list_destroy (proc->user_data);
  entropy_free (proc);
  return 0;



}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor, void *ret,
		    void *user_data)
{
  entropy_gui_component_instance *comp =
    (entropy_gui_component_instance *) user_data;

  switch (eevent->event_type) {
  case ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL:
  case ENTROPY_NOTIFY_FILELIST_REQUEST:{

      event_idle_processor *proc =
	entropy_malloc (sizeof (event_idle_processor));

      entropy_generic_file *event_file;
      entropy_file_request *request = eevent->data;	/*A file request's data is the dest dir */

      entropy_icon_viewer *view = comp->data;
      Ecore_Hash *tmp_gui_hash;
      Ecore_Hash *tmp_icon_hash;

      /*Keep a reference to our existing hash */
      tmp_gui_hash = view->gui_hash;
      tmp_icon_hash = view->icon_hash;
      view->gui_hash =
	ecore_hash_new (ecore_direct_hash, ecore_direct_compare);
      view->icon_hash =
	ecore_hash_new (ecore_direct_hash, ecore_direct_compare);

      /*Terminate our last load if we are still going...Not the most elegant solution, but there can only be 1 at once */
      if (view->last_processor)
	view->last_processor->terminate = 1;


      /*Setup the background processor object */
      //proc->eevent = eevent;
      proc->requestor = comp;
      proc->count = 0;
      proc->terminate = 0;
      proc->user_data = ecore_list_new ();
      view->last_processor = proc;




      ecore_list_first_goto (ret);
      while ((event_file = ecore_list_next (ret))) {
	//printf("Populating with '%s'\n", event_file->filename);
	
	entropy_core_file_cache_add_reference (event_file->md5);
	ecore_list_append (proc->user_data, event_file);
      }

      ecore_idle_enterer_add (idle_add_icons, proc);



      //printf("Starting..\n");


      /*Set the current path from the event source... */
      snprintf (view->current_dir, 1024, "%s://%s/%s",
		request->file->uri_base, request->file->path,
		request->file->filename);


      /*Before we begin, see if our file hash is initialized, if so - we must destroy it first */
       /*TODO*/ gui_object_destroy_and_free (comp, tmp_gui_hash);
      ecore_hash_destroy (tmp_icon_hash);



    }
    break;


  case ENTROPY_NOTIFY_THUMBNAIL_REQUEST:{
      /*Only bother if we have a thumbnail, and a component */
      if (ret && user_data) {
	gui_file *obj;
	entropy_thumbnail *thumb = (entropy_thumbnail *) ret;
	entropy_icon_viewer *view = comp->data;

	obj = ecore_hash_get (view->gui_hash, thumb->parent);

	if (obj) {
	  Ewl_Widget *cell;
	  Ewl_Widget *image;
	  obj->thumbnail = thumb;

	  cell = ewl_container_child_get (EWL_CONTAINER (obj->icon), 0);
	  image = ewl_container_child_get (EWL_CONTAINER (cell), 0);


	  ewl_image_file_set (EWL_IMAGE (image),
			      obj->thumbnail->thumbnail_filename, 0);

	}
	else {
	  printf ("ERR: Couldn't find a hash reference for this file!\n");
	}
      }
    }				//End case
    break;


  case ENTROPY_NOTIFY_FILE_CHANGE:{
      //printf ("Received file change event at icon viewer for file %s \n", ((entropy_generic_file*)ret)->filename);
    }
    break;

  case ENTROPY_NOTIFY_FILE_CREATE:{
      //printf ("Received file create event at icon viewer for file %s \n", ((entropy_generic_file*)ret)->filename);
      ewl_icon_local_viewer_add_icon (comp, (entropy_generic_file *) ret,
				      DO_MIME);
    }
    break;

  case ENTROPY_NOTIFY_FILE_REMOVE_DIRECTORY:
  case ENTROPY_NOTIFY_FILE_REMOVE:{
      printf ("Received a remove file notify\n");
      ewl_icon_local_viewer_remove_icon (comp, (entropy_generic_file *) ret);
    }
    break;

  case ENTROPY_NOTIFY_FILE_STAT_EXECUTED:{
      //printf("STAT EXECUTED Response back at ewl_icon_local_viewer\n");
    }
    break;

  case ENTROPY_NOTIFY_FILE_STAT_AVAILABLE:{

      entropy_file_stat *file_stat = (entropy_file_stat *) eevent->return_struct;
      if (file_stat->file == NULL) {
	printf ("***** File stat file is null\n");
      }
      ewl_icon_local_viewer_show_stat (file_stat);


    }
    break;

  case ENTROPY_NOTIFY_FILE_PROGRESS:{
      entropy_icon_viewer *view = comp->data;
      entropy_file_progress *progress = ret;


      if (!view->progress->progress_window) {
	printf ("Showing progressbar dialog..\n");

	ewl_progress_window_create (view->progress);
	ewl_widget_show (view->progress->progress_window);
      }

      if (view->progress->progress_window) {
	ewl_text_text_set (EWL_TEXT (view->progress->file_from),
			   progress->file_from->filename);
	ewl_text_text_set (EWL_TEXT (view->progress->file_to),
			   progress->file_to->filename);
	ewl_progressbar_value_set (EWL_PROGRESSBAR
				   (view->progress->progressbar),
				   progress->progress);
      }

      /*Is it time to hide (i.e. end) */
      if (progress->type == TYPE_END) {
	printf ("Hiding progressbar dialog..\n");
	ewl_widget_destroy (view->progress->progress_window);
	view->progress->progress_window = NULL;
      }

    }
    break;

  }				//End switch

}
