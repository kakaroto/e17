#include "entropy.h"
#include "entropy_gui.h"
#include <dlfcn.h>
#include <strings.h>
#include <limits.h>
#include <time.h>
#include <Etk.h>
#include "etk_directory_add_dialog.h"
#include "etk_properties_dialog.h"
#include "entropy_etk_context_menu.h"
#include "etk_user_interaction_dialog.h"

#define EN_DND_COL_NUM 5

typedef struct entropy_etk_iconbox_viewer entropy_etk_iconbox_viewer;
struct entropy_etk_iconbox_viewer
{
  Etk_Widget *iconbox;
  Etk_Widget* parent_visual; 
  Etk_Widget* vbox;
  Etk_Widget* slider;

  Ecore_Hash* gui_hash;

  Ecore_List *gui_events;
  Ecore_List *files;		/*The entropy_generic_file references we copy. */

  Etk_Widget* popup;
  Etk_Widget* open_with_menu;
  Etk_Widget* open_with_menuitem;

  /*Current folder - TODO - move to core - per layout API*/
  entropy_generic_file* current_folder;

  /*A file we're waiting on for passback properties*/
  Ecore_Hash* properties_request_hash;  

  /*Timer for tooltips*/
  Ecore_Timer* tooltimer;
  Ecore_Timer* popuptimer;

  /*Hover positions*/
  int hx;
  int hy;

  /*Hover file*/
  entropy_generic_file* hover_file;
};

void _entropy_etk_icon_viewer_click_cb(Etk_Object *object, void *event_info, void *data);
void entropy_etk_icon_viewer_icon_size_set(entropy_etk_iconbox_viewer* viewer, double value ) ;

Entropy_Plugin* entropy_plugin_init (entropy_core * core);

entropy_gui_component_instance * 
entropy_plugin_gui_instance_new (entropy_core * core, entropy_gui_component_instance * 
		layout, void *data);

void
icon_viewer_add_row (entropy_gui_component_instance * instance,
			  entropy_generic_file * file);
void icon_viewer_remove_row(entropy_gui_component_instance * instance,
			  entropy_generic_file * file);
Ecore_List* 
entropy_etk_icon_viewer_selected_get(entropy_etk_iconbox_viewer* viewer);


int
entropy_plugin_type_get ();
int
entropy_plugin_sub_type_get ();
char *
entropy_plugin_identify ();
char*
entropy_plugin_toolkit_get() ;



/*------------- boilerplate -----*/
typedef struct gui_file gui_file;
struct gui_file
{
  entropy_generic_file *file;
  entropy_gui_component_instance *instance;
  Etk_Iconbox_Icon *icon;
};

gui_file* _gui_file_new(entropy_generic_file* file, 
		entropy_gui_component_instance* instance, Etk_Iconbox_Icon* icon)
{
	gui_file* gfile =  entropy_malloc(sizeof(gui_file));
	gfile->file = file;
	gfile->instance = instance;
	gfile->icon = icon;

	return gfile;
}

void
gui_file_destroy (gui_file * file)
{
  entropy_free (file);

}

void
gui_file_remove_destroy_single(entropy_gui_component_instance * comp,
		gui_file* file)
{
	entropy_etk_iconbox_viewer *view = comp->data;
	ecore_hash_remove(view->gui_hash, file->file);
	entropy_free(file);
	
}

Ecore_List* 
gui_object_destroy_and_free (entropy_gui_component_instance * comp,
			     Ecore_Hash * gui_hash)
{

  Ecore_List *list;
  Ecore_List *file_remove_ref_list;
  entropy_generic_file *obj;
  gui_file *freeobj;
  entropy_etk_iconbox_viewer *view = comp->data;


  file_remove_ref_list = ecore_list_new();
  
  /*Temporarily stop callbacks, we don't want to clobber an in-op process */
  entropy_notify_lock_loop (comp->core->notify);

  list = ecore_hash_keys (gui_hash);

  ecore_list_first_goto (list);
  while ((obj = ecore_list_next (list))) {


    freeobj = ecore_hash_get (gui_hash, obj);
    if (freeobj) {
      /*De-Associate this icon with this file in the core, so DND works */
      entropy_core_object_file_disassociate (freeobj->icon);

      gui_file_destroy (freeobj);
    }

    /*Tell the core we no longer need this file - it might free it now */
    ecore_list_append(file_remove_ref_list, obj->md5);
  }
  ecore_hash_destroy (gui_hash);
  view->gui_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  ecore_list_destroy (list);


  entropy_notify_unlock_loop (comp->core->notify);


  return file_remove_ref_list;

}
/*----- End boilerplate -----*/


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
  return (char *) "iconviewer";
}

char*
entropy_plugin_toolkit_get() 
{
	return ENTROPY_TOOLKIT_ETK;
}


Entropy_Plugin*
entropy_plugin_init (entropy_core * core)
{
  Entropy_Plugin_Gui* plugin;
  Entropy_Plugin* base;
	
  plugin = entropy_malloc(sizeof(Entropy_Plugin_Gui));
  base = ENTROPY_PLUGIN(plugin);
  
  return base;
}

void entropy_etk_icon_viewer_icon_size_set(entropy_etk_iconbox_viewer* viewer, double value ) 
{
  etk_iconbox_model_geometry_set(etk_iconbox_current_model_get(ETK_ICONBOX(viewer->iconbox)),
		  (value*2) + 14,
	  	  value + 22,
		  5, 4);
  etk_iconbox_model_icon_geometry_set(etk_iconbox_current_model_get(ETK_ICONBOX(viewer->iconbox)),
		  26,0, value, value, 1, 1);

  
  etk_iconbox_model_label_geometry_set(etk_iconbox_current_model_get(ETK_ICONBOX(viewer->iconbox)),
		  0, value + 2, (value*2) + 4, 12, 0.5,0);
}

void _entropy_etk_icon_viewer_slider_cb(Etk_Object *object, double value, void *data)
{
  entropy_gui_component_instance *instance;	
  entropy_etk_iconbox_viewer *viewer;

  instance = data;
  viewer = instance->data;

  entropy_etk_icon_viewer_icon_size_set(viewer, value);
}

int _entropy_etk_icon_viewer_hover_popup_cb(void* data)
{
	entropy_gui_component_instance *instance;	
	entropy_etk_iconbox_viewer *viewer;
	entropy_file_request* req;

	req = data;
	instance = (entropy_gui_component_instance*)req->requester;
	viewer = instance->data;

	entropy_event_hover_request(instance,req->file, viewer->hx, viewer->hy);
	entropy_core_file_cache_remove_reference(req->file->md5);

	viewer->hover_file = req->file;
	free(req);

	ecore_timer_del(viewer->popuptimer);
	viewer->popuptimer = NULL;

	return 0;
}

/* Called when the user presses a key */
static void _etk_entropy_iconviewer_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Down *key_event = event;
   Etk_Iconbox* iconbox = ETK_ICONBOX(object);
   Ecore_List* del = NULL;

   if (!strcmp(key_event->key, "Delete")) {
	   Etk_Iconbox_Icon* icon;
	   entropy_generic_file* file;
	   
	   printf("Delete pressed!\n");

	   for (icon = iconbox->first_icon; icon ; icon = icon->next ) {
	   	if (etk_iconbox_is_selected(icon)) {
		   file = etk_iconbox_icon_data_get(icon);

		   if (file) {
			printf("Deleting '%s'...\n", file->filename);
			
			if (key_event->modifiers & ETK_MODIFIER_SHIFT) {
				entropy_plugin_filesystem_file_remove(file, (entropy_gui_component_instance*)data);
			} else {
				if (!del) del = ecore_list_new();
				ecore_list_append(del, file);
			}
		   }
		}
	  }

	  if (del && ecore_list_count(del)) {
		entropy_etk_delete_dialog_new((entropy_gui_component_instance*)data, del);
	  }

   }

}



void _entropy_etk_icon_viewer_move_cb(Etk_Object *object, void *event_info, void *data)
{
	Etk_Event_Mouse_Move *event =event_info;
	Etk_Iconbox_Icon* icon;
	entropy_generic_file* file;
	entropy_gui_component_instance *instance;	
	entropy_etk_iconbox_viewer *viewer;
	Etk_Toplevel* toplevel;
	int win_x,win_y;

	instance = data;
	viewer = instance->data;

	if (viewer->popuptimer) {
		ecore_timer_del(viewer->popuptimer);
		viewer->popuptimer = NULL;
	}

	if (viewer->hover_file) {
		entropy_event_dehover_request(instance,viewer->hover_file);
		viewer->hover_file = NULL;
	}

	if (!(icon = etk_iconbox_icon_get_at_xy(ETK_ICONBOX(viewer->iconbox), 
   		event->cur.canvas.x, event->cur.canvas.y, ETK_FALSE, ETK_TRUE, ETK_TRUE))) return;

	toplevel = etk_widget_toplevel_parent_get(viewer->iconbox);
	etk_window_geometry_get(ETK_WINDOW(toplevel), &win_x, &win_y, NULL, NULL);
	
	file = etk_iconbox_icon_data_get(icon);

	if (file) {
		entropy_file_request* req = calloc(1,sizeof(entropy_file_request));
		req->file = file;
		req->requester = instance;
		entropy_core_file_cache_add_reference(file->md5);

		viewer->hx = event->cur.canvas.x+win_x;
		viewer->hy = event->cur.canvas.y+win_y;

		viewer->popuptimer = ecore_timer_add(2.0,_entropy_etk_icon_viewer_hover_popup_cb , req);
	}
}

void _entropy_etk_icon_viewer_click_cb(Etk_Object *object, void *event_info, void *data)
{
  entropy_gui_component_instance *instance;	
  entropy_etk_iconbox_viewer *viewer;
  entropy_generic_file* file;
  Ecore_List* selected;
  int selected_count  =0;
  Etk_Iconbox_Icon* icon;
  Etk_Event_Mouse_Down *event;
  Etk_Bool ctrl_pressed;

  instance = data;
  viewer = instance->data;
  event = event_info;
  selected_count = 0;

 if (!(icon = etk_iconbox_icon_get_at_xy(ETK_ICONBOX(viewer->iconbox), 
   	event->canvas.x, event->canvas.y, ETK_FALSE, ETK_TRUE, ETK_TRUE)))
  return;

  
  file = etk_iconbox_icon_data_get(icon);
  ctrl_pressed = (event->modifiers & ETK_MODIFIER_CTRL);

  if (event->button == 1) {
	  if (event->flags & ETK_MOUSE_DOUBLE_CLICK) {
		  if (file) {
			entropy_event_action_file(file,instance);
		  }
	  }
  } else if (event->button == 3) {
	  if (ctrl_pressed != ETK_TRUE) etk_iconbox_unselect_all(ETK_ICONBOX(viewer->iconbox));
	  etk_iconbox_icon_select(icon);

	  selected = entropy_etk_icon_viewer_selected_get(viewer);
	  if (ecore_list_count(selected)) {
		  if (ecore_list_count(selected) > 1) {
			  entropy_etk_context_menu_popup_multi(instance,selected);
		  } else {
			  entropy_etk_context_menu_popup(instance, file);
		  }
	  }
	  ecore_list_destroy(selected);
  }
}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor,
		    void *el, entropy_gui_component_instance * comp)
{
  entropy_etk_iconbox_viewer *viewer =
    (entropy_etk_iconbox_viewer *) comp->data;

  switch (eevent->event_type) {
  	  case ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL:
	  case ENTROPY_NOTIFY_FILELIST_REQUEST:{
	      entropy_generic_file *file;
	      char* ref;
	      Ecore_List* remove_ref;
	      entropy_generic_file *event_file =
		((entropy_file_request *) eevent->data)->file;

	      viewer->current_folder = event_file;

	      remove_ref = gui_object_destroy_and_free(comp, viewer->gui_hash);

	      etk_iconbox_freeze(ETK_ICONBOX(viewer->iconbox));
	      etk_iconbox_clear(ETK_ICONBOX(viewer->iconbox));
	      ecore_list_first_goto (el);
		while ((file = ecore_list_next (el))) {
		      icon_viewer_add_row (comp, file);
		}

		while ( (ref = ecore_list_first_remove(remove_ref)))  {
			entropy_core_file_cache_remove_reference (ref);
		}
		ecore_list_destroy(remove_ref);
	      etk_iconbox_thaw(ETK_ICONBOX(viewer->iconbox));


	      }
	      break;

           case ENTROPY_NOTIFY_THUMBNAIL_REQUEST:{

   	   /*Only bother if we have a thumbnail, and a component */
	      if (el && comp) {
		gui_file *obj;
		entropy_thumbnail *thumb = (entropy_thumbnail *) el;
	
		obj = ecore_hash_get (viewer->gui_hash, thumb->parent);

		if (obj) {
		  etk_iconbox_icon_file_set(obj->icon, thumb->thumbnail_filename, NULL);
		} else {
		  /*printf ("ERR: Couldn't find a hash reference for this file!\n");*/
		}

	      }
	    }				//End case
	    break;					  

     case ENTROPY_NOTIFY_FILE_REMOVE_DIRECTORY:
     case ENTROPY_NOTIFY_FILE_REMOVE:{
	 icon_viewer_remove_row(comp, (entropy_generic_file*)el);
     }
     break;

     case ENTROPY_NOTIFY_FILE_CREATE:{
      entropy_generic_file* file = el;
      
      /*Check that this file is the current dir we are displaying*/
      entropy_generic_file* parent_folder = entropy_core_parent_folder_file_get(file);

      if (parent_folder && parent_folder == viewer->current_folder) {
	      icon_viewer_add_row (comp, file);				      
      }
     }
     break;	

     case ENTROPY_NOTIFY_COPY_REQUEST: {
	Ecore_List* selected;
	entropy_generic_file* file;
					       
	printf("ICONBOX: Copy request\n");

	entropy_core_selected_files_clear();
	entropy_core_selection_type_set(ENTROPY_SELECTION_COPY);
	selected = entropy_etk_icon_viewer_selected_get(viewer);

	ecore_list_first_goto(selected);
	while ( (file = ecore_list_next(selected))) {
		entropy_core_selected_file_add(file);
	}
	ecore_list_destroy(selected);
     }
     break;

     case ENTROPY_NOTIFY_CUT_REQUEST: {
	Ecore_List* selected;
	entropy_generic_file* file;
					       
	printf("ICONBOX: Cut request\n");

	entropy_core_selected_files_clear();
	entropy_core_selection_type_set(ENTROPY_SELECTION_CUT);
	selected = entropy_etk_icon_viewer_selected_get(viewer);

	ecore_list_first_goto(selected);
	while ( (file = ecore_list_next(selected))) {
		entropy_core_selected_file_add(file);
	}
	ecore_list_destroy(selected);
     }
     break;

  }
}

Ecore_List* 
entropy_etk_icon_viewer_selected_get(entropy_etk_iconbox_viewer* viewer)
{
	Ecore_List* selected;
	Etk_Iconbox_Icon* icon;
	entropy_generic_file* file;
	
	  selected = ecore_list_new();
	  for (icon = ETK_ICONBOX(viewer->iconbox)->first_icon; icon ; icon = icon->next ) {
	   	if (etk_iconbox_is_selected(icon)) {
		     file = etk_iconbox_icon_data_get(icon);
		     ecore_list_append(selected, file);
		}
	  }

	  return selected;
}


void
icon_viewer_add_row (entropy_gui_component_instance * instance,
			  entropy_generic_file * file)
{
	entropy_etk_iconbox_viewer* viewer;
	Etk_Iconbox_Icon* icon;
	
	viewer = instance->data;
	
	if (!file->thumbnail) {
		icon = etk_iconbox_append(ETK_ICONBOX(viewer->iconbox), PACKAGE_DATA_DIR "/icons/default.png", NULL, file->filename);
		etk_iconbox_icon_data_set(icon, file);
		ecore_hash_set(viewer->gui_hash, file, _gui_file_new(file,instance,icon));
		entropy_plugin_thumbnail_request(instance, file, (void*)gui_event_callback);
	} else {
		icon = etk_iconbox_append(ETK_ICONBOX(viewer->iconbox), file->thumbnail->thumbnail_filename, NULL, file->filename);		
		ecore_hash_set(viewer->gui_hash, file, _gui_file_new(file,instance,icon));
		etk_iconbox_icon_data_set(icon, file);		
	}

	entropy_core_file_cache_add_reference (file->md5);
}

void icon_viewer_remove_row(entropy_gui_component_instance * instance,
			  entropy_generic_file * file)
{
	entropy_etk_iconbox_viewer* viewer;
	gui_file* event_file = NULL;
	
	viewer = instance->data;

	if (file) {
		event_file = ecore_hash_get(viewer->gui_hash,file);
		if (event_file) {
			etk_iconbox_icon_del(event_file->icon);
			gui_file_remove_destroy_single(instance,event_file);
		}
	}
}

#if 0
static void _entropy_etk_icon_viewer_drag_begin_cb(Etk_Object *object, void *data)
{
   Etk_Iconbox *iconbox;
   const char **types;
   unsigned int num_types;
   Etk_Widget *drag;
   Etk_Widget *image;
   entropy_gui_component_instance* instance;
   entropy_etk_iconbox_viewer* viewer;
   char buffer[8192]; /* Um - help - what do we size this to? */
   Etk_Widget* table;
   int l=0,r=0,t=0,b=0;
   int added_object = 0;
   entropy_generic_file* file;
   Etk_Widget* vbox;
   Etk_Widget* label;
   char label_buffer[50];
   Etk_Iconbox_Icon* icon;

   instance = data;
   viewer = instance->data;

   iconbox = ETK_ICONBOX(object);
   
   drag = (ETK_WIDGET(iconbox))->drag;

   table = etk_table_new(5,5,ETK_FALSE);
   bzero(buffer,8192);
   for (icon = iconbox->first_icon; icon ; icon = icon->next ) {
	   if (etk_iconbox_is_selected(icon)) {
		   file = etk_iconbox_icon_data_get(icon);
	   
		   strcat(buffer, file->uri);
		   strcat(buffer, "\r\n");

		   if (added_object < (EN_DND_COL_NUM*5)-1) {
			   /*Build the drag widget*/
			   vbox = etk_vbox_new(ETK_TRUE,0);

			   /*Print the label*/
			   bzero(label_buffer, sizeof(label_buffer));

			   if (strlen(file->filename) > 5) {
				   snprintf(label_buffer,5,"%s", file->filename);
				   strcat(label_buffer, "...");
			   } else {
				   sprintf(label_buffer,"%s", file->filename);
			   }
	   		   label = etk_label_new(label_buffer);
		   
			  if (file->thumbnail && file->thumbnail->thumbnail_filename) {
				image = etk_image_new_from_file(file->thumbnail->thumbnail_filename);
			  } else {
				image = etk_image_new_from_file(PACKAGE_DATA_DIR "/icons/default.png");
			  }
			 etk_image_keep_aspect_set(ETK_IMAGE(image), ETK_TRUE);
			 etk_widget_size_request_set(image, 48, 48);
			 etk_box_append(ETK_BOX(vbox), image, ETK_BOX_START, ETK_BOX_NONE, 0);
			  
			  etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
			  etk_table_attach(ETK_TABLE(table), vbox, l, r, t, b, 3, 3,
				   ETK_TABLE_NONE);
		  
			  ++l; ++r;
			  added_object++;
			  if(l == EN_DND_COL_NUM) {
			       l = r = 0;
			       ++t; ++b;
			    }	 
		  }
	  }
	  
   }

   etk_container_add(ETK_CONTAINER(drag), table);
  

   types = entropy_malloc(sizeof(char*));
   num_types = 1;
   types[0] = strdup("text/uri-list");
    

   printf("Drag buffer: %s\n", buffer);
   
   etk_drag_types_set(ETK_DRAG(drag), types, num_types);
   etk_drag_data_set(ETK_DRAG(drag), buffer, strlen(buffer)+1);



   
   /*image = etk_image_new_from_file(icol1_string);
   etk_image_keep_aspect_set(ETK_IMAGE(image), ETK_TRUE);
   etk_widget_size_request_set(image, 96, 96);
   etk_container_add(ETK_CONTAINER(drag), image);*/

}
#endif




entropy_gui_component_instance *
entropy_plugin_gui_instance_new (entropy_core * core,
		     entropy_gui_component_instance * layout, void *data)
{	
  entropy_gui_component_instance *instance;	
  entropy_etk_iconbox_viewer *viewer;
  char* size;

    
  instance = entropy_gui_component_instance_new ();
  viewer = entropy_malloc (sizeof (entropy_etk_iconbox_viewer));

  viewer->gui_hash = ecore_hash_new(ecore_direct_hash,ecore_direct_compare);


  /*Make the various widgets*/
  viewer->vbox = etk_vbox_new(ETK_FALSE,0);

  instance->gui_object = viewer->vbox;
  instance->core = core;
  instance->data = viewer;
  instance->layout_parent = layout;
  
  /*Make the slider*/
  viewer->slider = etk_hslider_new(10,128, 48, 1, 1);
  etk_box_append(ETK_BOX(viewer->vbox), viewer->slider, ETK_BOX_START, ETK_BOX_NONE, 0);
  etk_signal_connect("value-changed", ETK_OBJECT(viewer->slider), 
		  ETK_CALLBACK(_entropy_etk_icon_viewer_slider_cb), instance);
  
  viewer->iconbox = etk_iconbox_new();
  etk_box_append(ETK_BOX(viewer->vbox), viewer->iconbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

  if ((size = entropy_config_misc_item_str_get("general.iconsize"))) {
	etk_range_value_set(ETK_RANGE(viewer->slider), atoi(size));
	entropy_etk_icon_viewer_icon_size_set(viewer, atoi(size));
  }
  
  /*etk_iconbox_model_icon_geometry_set(etk_iconbox_current_model_get(ETK_ICONBOX(viewer->iconbox)),
		  0,0,64,64,1,1);*/
		  
	  
  etk_signal_connect("mouse-down", ETK_OBJECT(viewer->iconbox), ETK_CALLBACK(_entropy_etk_icon_viewer_click_cb), instance);
  etk_signal_connect("mouse-move", ETK_OBJECT(viewer->iconbox), ETK_CALLBACK(_entropy_etk_icon_viewer_move_cb), instance);
  etk_signal_connect("key-down", ETK_OBJECT(viewer->iconbox), 
		  ETK_CALLBACK(_etk_entropy_iconviewer_key_down_cb), instance);
  
  /*DND Setup*/
  /* dnd_types_num = 1;
   dnd_types = entropy_malloc(dnd_types_num* sizeof(char*));
   dnd_types[0] = strdup("text/uri-list");  
  etk_widget_dnd_source_set(viewer->iconbox, ETK_TRUE);
  etk_signal_connect("drag_begin", ETK_OBJECT(viewer->iconbox) , ETK_CALLBACK(_entropy_etk_icon_viewer_drag_begin_cb), instance);*/


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

  /*We want to know about thumbnail available events */
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_THUMBNAIL_AVAILABLE));

  /*We want to know if the core sends copy/cut requests*/
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_COPY_REQUEST));

  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_CUT_REQUEST));


  etk_widget_show_all(viewer->vbox);

  return instance;
}
