#include <Ewl.h>
#include "entropy.h"
#include <dlfcn.h>
#include <Ecore.h>
#include <stdlib.h>
#include <stdio.h>
#include "ewl_mime_dialog.h"
#include "entropy_gui.h"
#include "ewl_tip.h"
#include "ewl_about_dialog.h"

#define HEADER_CONFIG_MAX 2048

static Ewl_Widget *win;
static int _ewl_layout_count = 0;

int entropy_plugin_type_get ();
char *entropy_plugin_identify ();
void location_add_execute_cb (Ewl_Widget * item, void *ev_data,
			      void *user_data);
void location_add_cancel_cb (Ewl_Widget * item, void *ev_data,
			     void *user_data);
void mime_cb (Ewl_Widget * main_win, void *ev_data, void *user_data);
void entropy_ewl_layout_simple_tooltip_window ();
void location_menu_popup_delete_cb (Ewl_Widget * label, void *ev_data,
				    void *user_data);
void location_menu_popup_cb (Ewl_Widget * label, void *ev_data,
			     void *user_data);
void location_add_cb (Ewl_Widget * main_win, void *ev_data, void *user_data);
void layout_ewl_simple_config_create (entropy_core * core);
void layout_ewl_simple_add_header (entropy_gui_component_instance * instance,
				   char *name, char *uri);
void layout_ewl_simple_add_config_location (entropy_gui_component_instance *
					    instance, char *name, char *uri);
void __destroy_main_window (Ewl_Widget * main_win, void *ev_data,
			    void *user_data);
void contract_cb (Ewl_Widget * main_win, void *ev_data, void *user_data);
void layout_ewl_simple_local_view_cb (Ewl_Widget * main_win, void *ev_data,
				      void *user_data);
void layout_ewl_simple_structure_view_cb (Ewl_Widget * main_win,
					  void *ev_data, void *user_data);
void entropy_plugin_layout_main ();
void entropy_plugin_destroy (entropy_gui_component_instance * comp);
void entropy_plugin_init (entropy_core * core);
void entropy_delete_current_folder (Ecore_List * el);
entropy_gui_component_instance *entropy_plugin_layout_create (entropy_core *
							      core);
void ewl_layout_simple_tooltip_show_cb (Ewl_Widget * item, void *ev_data,
					void *user_data);
void ewl_layout_simple_about_dialog_cb (Ewl_Widget * item, void *ev_data,
					void *user_data);

typedef struct entropy_ewl_layout_header_uri entropy_ewl_layout_header_uri;
struct entropy_ewl_layout_header_uri
{
  Ewl_Widget *visual;
  entropy_gui_component_instance *layout;

  char *uri;
  char *header;
};

typedef struct entropy_layout_gui entropy_layout_gui;
struct entropy_layout_gui
{
  entropy_gui_component_instance *iconbox_viewer;
  entropy_gui_component_instance *structure_viewer;

  Ewl_Widget *tree;
  Ewl_Widget *paned;
  Ewl_Widget *local_container;
  Ewl_Widget *context_menu;
  Ewl_Widget *context_menu_floater;
  Ecore_List *current_folder;
  Ecore_List *local_components;
  Ecore_Hash *headers;

  entropy_ewl_layout_header_uri *active_header;

  /*Tmp */
  Ewl_Widget *samba_radio;
  Ewl_Widget *posix_radio;
  Ewl_Widget *sftp_radio;
  Ewl_Widget *location_add_window;
  Ewl_Widget *location_add_name;
  Ewl_Widget *location_add_path;
  Ewl_Widget *location_add_server;
  Ewl_Widget *location_add_username;
  Ewl_Widget *location_add_password;
};


int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_GUI_LAYOUT;
}

char *
entropy_plugin_identify ()
{
  return (char *) "ewl";
}

char*
entropy_plugin_toolkit_get() 
{
	return ENTROPY_TOOLKIT_EWL;
}

void
ewl_layout_simple_tooltip_show_cb (Ewl_Widget * item, void *ev_data,
				   void *user_data)
{
  ewl_entropy_tip_window_display ();
}

void
ewl_layout_simple_about_dialog_cb (Ewl_Widget * item, void *ev_data,
				   void *user_data)
{
  entropy_ewl_about_dialog_display ();
}


void
structure_configure_cb (Ewl_Widget * item, void *ev_data, void *user_data)
{
  Ewl_Widget *parent = item;
  entropy_gui_component_instance *instance = user_data;
  entropy_layout_gui *layout = instance->data;

  /*printf("Structure configutre..%p\nHierarchy: ", item);
     while (parent->parent) {
     printf("'%s' : ", parent->inheritance);
     if (VISIBLE(parent)) 
     printf("VISIBLE ");
     else
     printf("INVISIBLE ");

     if (OBSCURED(parent)) 
     printf("OBSCURED \n");
     else
     printf("UNOBSC \n");


     parent = parent->parent;
     }
     printf("\n");
     printf("\n\n"); */
}

/*TODO/FIXME - This needs a rewrite, to be dynamic, and wizard-based*/
void
location_add_execute_cb (Ewl_Widget * item, void *ev_data, void *user_data)
{
  entropy_gui_component_instance *instance = user_data;
  entropy_layout_gui *viewer = instance->data;

  char new_uri[2048];

  char *display_name =
    ewl_text_text_get (EWL_TEXT (viewer->location_add_name));
  char *path = ewl_text_text_get (EWL_TEXT (viewer->location_add_path));
  char *server = ewl_text_text_get (EWL_TEXT (viewer->location_add_server));
  char *username =
    ewl_text_text_get (EWL_TEXT (viewer->location_add_username));
  char *password =
    ewl_password_text_get (EWL_PASSWORD (viewer->location_add_password));

  printf ("Display name: '%s'\n", display_name);
  printf ("Server: '%s'\n", server);
  printf ("Path: '%s'\n", path);
  printf ("Username: '%s'\n", username);
  printf ("Password: '%s'\n", password);


  if (ewl_checkbutton_is_checked (EWL_CHECKBUTTON (viewer->posix_radio))) {
    snprintf (new_uri, 2048, "file://%s", path);
    printf ("New URI is: '%s'\n", new_uri);
    layout_ewl_simple_add_header (instance, display_name, new_uri);

    layout_ewl_simple_add_config_location (instance, display_name, new_uri);

  } else if (ewl_checkbutton_is_checked (EWL_CHECKBUTTON (viewer->sftp_radio))) {
    snprintf (new_uri, 2048, "sftp:///%s%s", server,path);
    printf ("New URI is: '%s'\n", new_uri);
    layout_ewl_simple_add_header (instance, display_name, new_uri);

    layout_ewl_simple_add_config_location (instance, display_name, new_uri);

  } else if (ewl_checkbutton_is_checked (EWL_CHECKBUTTON (viewer->samba_radio))) {
    if (server) {
      if (username && password) {
	snprintf (new_uri, 2048, "smb://%s:%s@/%s%s", username, password,
		  server, path);
	printf ("New URI is: '%s'\n", new_uri);
	layout_ewl_simple_add_header (instance, display_name, new_uri);
      }
      else {
	snprintf (new_uri, 2048, "smb:///%s%s", server, path);
	printf ("New URI is: '%s'\n", new_uri);
	layout_ewl_simple_add_header (instance, display_name, new_uri);
      }

      layout_ewl_simple_add_config_location (instance, display_name, new_uri);
    }
    else {
      printf ("Server required for remote file systems!\n");
    }
  }
  
  else {
    printf ("No filesystem selected!\n");
  }

  ewl_widget_destroy (viewer->location_add_window);


}


void
location_add_cancel_cb (Ewl_Widget * item, void *ev_data, void *user_data)
{
  ewl_widget_destroy (EWL_WIDGET (user_data));
}

void
mime_cb (Ewl_Widget * main_win, void *ev_data, void *user_data)
{
  entropy_ewl_mime_dialog_display ();
}


void
entropy_ewl_layout_simple_tooltip_window ()
{
  int status = entropy_core_tooltip_status_get ();

}


/*Header context menu*/
void
location_menu_popup_delete_cb (Ewl_Widget * label, void *ev_data,
			       void *user_data)
{
  entropy_gui_component_instance *instance = user_data;
  entropy_layout_gui *layout = instance->data;

  printf ("Destroying '%s' -> %p\n",
	  EWL_WIDGET (layout->active_header->visual)->inheritance,
	  layout->active_header->visual);

  ewl_widget_destroy (layout->active_header->visual);
  ecore_hash_remove (layout->headers, layout->active_header->visual);
}

void
location_menu_popup_cb (Ewl_Widget * label, void *ev_data, void *user_data)
{
  Ewl_Event_Mouse_Down *ev = ev_data;
  entropy_ewl_layout_header_uri *header = user_data;

  if (ev->button == 3) {
    printf ("Click at %d:%d for header %p and visual %p\n", ev->x, ev->y,
	    header, header->visual);

    ((entropy_layout_gui *) header->layout->data)->active_header = header;

    ewl_widget_show (((entropy_layout_gui *) header->layout->data)->
		     context_menu_floater);
    ewl_floater_position_set (EWL_FLOATER
			      (((entropy_layout_gui *) header->layout->data)->
			       context_menu_floater), ev->x, ev->y);
    ewl_callback_call (EWL_WIDGET
		       (((entropy_layout_gui *) header->layout->data)->
			context_menu), EWL_CALLBACK_FOCUS_IN);
  }
}

/*----------------*/



void
location_add_cb (Ewl_Widget * main_win, void *ev_data, void *user_data)
{
  entropy_gui_component_instance *instance =
    (entropy_gui_component_instance *) user_data;

  Ewl_Widget *window;
  Ewl_Widget *label;
  Ewl_Widget *vbox, *vbox2;
  Ewl_Widget *hbox;
  Ewl_Widget *button;

  window = ewl_window_new ();

  ((entropy_layout_gui *) instance->data)->location_add_window = window;
  ewl_window_title_set (EWL_WINDOW (window), "Add Location");

  vbox = ewl_vbox_new ();
  ewl_widget_show (vbox);
  ewl_container_child_append (EWL_CONTAINER (window), vbox);

	/*-------*/
  hbox = ewl_hbox_new ();
  ewl_container_child_append (EWL_CONTAINER (vbox), hbox);
  ewl_widget_show (hbox);

  label = ewl_label_new ();
  ewl_label_text_set (EWL_LABEL (label), "Filesystem Type");
  ewl_container_child_append (EWL_CONTAINER (hbox), label);
  ewl_widget_show (label);

  /*Create the filesystem buttons */
  /*TODO query EVFS to get supported file system types */
  vbox2 = ewl_vbox_new ();
  ewl_container_child_append (EWL_CONTAINER (hbox), vbox2);
  ewl_widget_show (vbox2);

  ((entropy_layout_gui *) instance->data)->posix_radio =
    ewl_radiobutton_new ();
  ewl_button_label_set (EWL_BUTTON
			(((entropy_layout_gui *) instance->data)->
			 posix_radio), "Standard Local Posix");
  ewl_container_child_append (EWL_CONTAINER (vbox2),
			      ((entropy_layout_gui *) instance->data)->
			      posix_radio);
  ewl_widget_show (((entropy_layout_gui *) instance->data)->posix_radio);

  ((entropy_layout_gui *) instance->data)->samba_radio =
    ewl_radiobutton_new ();
  ewl_button_label_set (EWL_BUTTON
			(((entropy_layout_gui *) instance->data)->
			 samba_radio), "Samba");
  ewl_radiobutton_chain_set (EWL_RADIOBUTTON
			     (((entropy_layout_gui *) instance->data)->
			      samba_radio),
			     EWL_RADIOBUTTON (((entropy_layout_gui *)
					       instance->data)->posix_radio));
  ewl_container_child_append (EWL_CONTAINER (vbox2),
			      ((entropy_layout_gui *) instance->data)->
			      samba_radio);
  ewl_widget_show (((entropy_layout_gui *) instance->data)->samba_radio);

  ((entropy_layout_gui *) instance->data)->sftp_radio =
    ewl_radiobutton_new ();
  ewl_button_label_set (EWL_BUTTON
			(((entropy_layout_gui *) instance->data)->
			 sftp_radio), "Sftp");
  ewl_radiobutton_chain_set (EWL_RADIOBUTTON
			     (((entropy_layout_gui *) instance->data)->
			      sftp_radio),
			     EWL_RADIOBUTTON (((entropy_layout_gui *)
					       instance->data)->samba_radio));
  ewl_container_child_append (EWL_CONTAINER (vbox2),
			      ((entropy_layout_gui *) instance->data)->
			      sftp_radio);
  ewl_widget_show (((entropy_layout_gui *) instance->data)->sftp_radio);




	/*-------*/
  /*"Name" */
  hbox = ewl_hbox_new ();
  ewl_container_child_append (EWL_CONTAINER (vbox), hbox);
  ewl_widget_show (hbox);

  label = ewl_label_new ();
  ewl_label_text_set (EWL_LABEL (label), "Display Name");
  ewl_container_child_append (EWL_CONTAINER (hbox), label);
  ewl_widget_show (label);

  ((entropy_layout_gui *) instance->data)->location_add_name =
    ewl_entry_new ();
  ewl_container_child_append (EWL_CONTAINER (hbox),
			      ((entropy_layout_gui *) instance->data)->
			      location_add_name);
  ewl_widget_show (((entropy_layout_gui *) instance->data)->
		   location_add_name);
	/*-----------------------*/


	/*-------*/
  /*"Server/Host" */
  hbox = ewl_hbox_new ();
  ewl_container_child_append (EWL_CONTAINER (vbox), hbox);
  ewl_widget_show (hbox);

  label = ewl_label_new ();
  ewl_label_text_set (EWL_LABEL (label), "Server/Host");
  ewl_container_child_append (EWL_CONTAINER (hbox), label);
  ewl_widget_show (label);

  ((entropy_layout_gui *) instance->data)->location_add_server =
    ewl_entry_new ();
  ewl_container_child_append (EWL_CONTAINER (hbox),
			      ((entropy_layout_gui *) instance->data)->
			      location_add_server);
  ewl_widget_show (((entropy_layout_gui *) instance->data)->
		   location_add_server);
	/*-----------------------*/


	/*-------*/
  /*"Path" */
  hbox = ewl_hbox_new ();
  ewl_container_child_append (EWL_CONTAINER (vbox), hbox);
  ewl_widget_show (hbox);

  label = ewl_label_new ();
  ewl_label_text_set (EWL_LABEL (label), "Path");
  ewl_container_child_append (EWL_CONTAINER (hbox), label);
  ewl_widget_show (label);

  ((entropy_layout_gui *) instance->data)->location_add_path =
    ewl_entry_new ();
  ewl_container_child_append (EWL_CONTAINER (hbox),
			      ((entropy_layout_gui *) instance->data)->
			      location_add_path);
  ewl_widget_show (((entropy_layout_gui *) instance->data)->
		   location_add_path);
	/*-----------------------*/

	/*-------*/
  /*"Username" */
  hbox = ewl_hbox_new ();
  ewl_container_child_append (EWL_CONTAINER (vbox), hbox);
  ewl_widget_show (hbox);

  label = ewl_label_new ();
  ewl_label_text_set (EWL_LABEL (label), "Username");
  ewl_container_child_append (EWL_CONTAINER (hbox), label);
  ewl_widget_show (label);

  ((entropy_layout_gui *) instance->data)->location_add_username =
    ewl_entry_new ();
  ewl_container_child_append (EWL_CONTAINER (hbox),
			      ((entropy_layout_gui *) instance->data)->
			      location_add_username);
  ewl_widget_show (((entropy_layout_gui *) instance->data)->
		   location_add_username);
	/*-----------------------*/

	/*-------*/
  /*"Path" */
  hbox = ewl_hbox_new ();
  ewl_container_child_append (EWL_CONTAINER (vbox), hbox);
  ewl_widget_show (hbox);

  label = ewl_label_new ();
  ewl_label_text_set (EWL_LABEL (label), "Password");
  ewl_container_child_append (EWL_CONTAINER (hbox), label);
  ewl_widget_show (label);

  ((entropy_layout_gui *) instance->data)->location_add_password =
    ewl_password_new ();
  ewl_container_child_append (EWL_CONTAINER (hbox),
			      ((entropy_layout_gui *) instance->data)->
			      location_add_password);
  ewl_widget_show (((entropy_layout_gui *) instance->data)->
		   location_add_password);
	/*-----------------------*/


  hbox = ewl_hbox_new ();
  ewl_container_child_append (EWL_CONTAINER (vbox), hbox);
  ewl_widget_show (hbox);

  button = ewl_button_new ();
  ewl_button_label_set (EWL_BUTTON (button), "Add");
  ewl_object_maximum_h_set (EWL_OBJECT (button), 15);
  ewl_container_child_append (EWL_CONTAINER (hbox), button);
  ewl_callback_append (button, EWL_CALLBACK_CLICKED, location_add_execute_cb,
		       instance);
  ewl_widget_show (button);

  button = ewl_button_new ();
  ewl_button_label_set (EWL_BUTTON (button), "Cancel");
  ewl_object_maximum_h_set (EWL_OBJECT (button), 15);
  ewl_container_child_append (EWL_CONTAINER (hbox), button);
  ewl_callback_append (button, EWL_CALLBACK_CLICKED, location_add_cancel_cb,
		       window);
  ewl_widget_show (button);

  ewl_object_custom_size_set (EWL_OBJECT (window), 400, 250);
  ewl_widget_show (window);
}


void
layout_ewl_simple_add_config_location (entropy_gui_component_instance *
				       instance, char *name, char *uri)
{
  char *current_uri =
    entropy_config_str_get ("layout_ewl_simple", "structure_bar");
  char new_uri[HEADER_CONFIG_MAX];

  snprintf (new_uri, HEADER_CONFIG_MAX, "%s|%s;%s", current_uri, name, uri);
  entropy_config_str_set ("layout_ewl_simple", "structure_bar", new_uri);

  entropy_free (current_uri);
}


void
layout_ewl_simple_config_create (entropy_core * core)
{
  char *eg = calloc (HEADER_CONFIG_MAX, sizeof (char));

  snprintf (eg, HEADER_CONFIG_MAX,
	    "Computer;file:///|Home;file://%s|Samba Example (Don't use!);smb://username:password@/test/machine/folder",
	    entropy_core_home_dir_get (core));

  //printf("Setting default config string..\n");
  entropy_config_str_set ("layout_ewl_simple", "structure_bar", eg);

  free (eg);
}


void
layout_ewl_simple_add_header (entropy_gui_component_instance * instance,
			      char *name, char *uri)
{

  Ewl_Widget *hbox;
  entropy_plugin *structure;
  entropy_ewl_layout_header_uri *header =
    calloc (1, sizeof (entropy_ewl_layout_header_uri));
  void *(*structure_plugin_init) (entropy_core * core,
				  entropy_gui_component_instance *, 
				  void* parent_visual,
				  void *data);
  entropy_layout_gui *gui = ((entropy_layout_gui *) instance->data);
  Ewl_Widget *tree = gui->tree;



  hbox = ewl_border_new ();
  ewl_border_text_set (EWL_BORDER (hbox), name);
  ewl_container_child_append (EWL_CONTAINER (tree), hbox);
  ewl_widget_show (hbox);

  header->header = strdup (name);
  header->uri = strdup (uri);
  
  header->visual = hbox;
  header->layout = instance;
  ecore_hash_set (gui->headers, hbox, header);
  printf ("Adding %p to header hash with visual %p\n", header,
	  header->visual);



  /*Now attach an object to it */
  structure =
    entropy_plugins_type_get_first (ENTROPY_PLUGIN_GUI_COMPONENT,
				    ENTROPY_PLUGIN_GUI_COMPONENT_STRUCTURE_VIEW);

  if (structure) {
    Ewl_Widget *visual;
    entropy_generic_file *file = entropy_core_parse_uri (uri);

    /*Add the callback for the popup */
    ewl_callback_append (EWL_BORDER (hbox)->label, EWL_CALLBACK_MOUSE_DOWN,
			 location_menu_popup_cb, header);
    strcpy (file->mime_type, "file/folder");

    /*Main drive viewer */
    {
      structure_plugin_init =
	dlsym (structure->dl_ref, "entropy_plugin_init");
      gui->structure_viewer =
	(*structure_plugin_init) (instance->core, instance, tree, file);
      gui->structure_viewer->plugin = structure;
      visual = EWL_WIDGET (gui->structure_viewer->gui_object);
      if (!visual)
	printf ("Alert! - Visual component not found\n");
      else;			// printf("Visual component found\n");
      ewl_container_child_append (EWL_CONTAINER (hbox), visual);
      ewl_object_fill_policy_set (EWL_OBJECT (visual), EWL_FLAG_FILL_HFILL);

      ewl_callback_append (visual, EWL_CALLBACK_CONFIGURE,
			   structure_configure_cb, instance);

      ewl_widget_show (visual);
    }
  }
}

void
__destroy_main_window (Ewl_Widget * main_win, void *ev_data, void *user_data)
{
  entropy_core *core = (entropy_core *) user_data;
  ewl_widget_destroy (main_win);
  _ewl_layout_count--;

  if (_ewl_layout_count == 0)  {
	  entropy_core_destroy (core);
  	  /*TODO cleanup this layout's structure to avoid leaks*/
	  exit (0); 
  }
}

void
contract_cb (Ewl_Widget * main_win, void *ev_data, void *user_data)
{
  Ewl_Box *box = EWL_BOX (user_data);

  ewl_object_maximum_w_set (EWL_OBJECT (box), 15);
}


void
layout_ewl_simple_structure_view_cb (Ewl_Widget * main_win, void *ev_data,
				     void *user_data)
{

  entropy_gui_component_instance *instance = user_data;
  entropy_layout_gui *layout = instance->data;

  /*entropy_gui_component_instance_disable(layout->structure_viewer);
     ewl_widget_hide(EWL_WIDGET(layout->structure_viewer->gui_object)); */

  printf ("Hiding tree...\n");

  ewl_widget_hide (layout->tree);
}

void
layout_ewl_simple_structure_view_show_cb (Ewl_Widget * main_win,
					  void *ev_data, void *user_data)
{

  entropy_gui_component_instance *instance = user_data;
  entropy_layout_gui *layout = instance->data;

  /*entropy_gui_component_instance_disable(layout->structure_viewer);
     ewl_widget_hide(EWL_WIDGET(layout->structure_viewer->gui_object)); */
  ewl_widget_show (layout->tree);
  ewl_container_child_prepend (EWL_CONTAINER (layout->paned), layout->tree);

}


void
layout_ewl_simple_local_view_cb (Ewl_Widget * main_win, void *ev_data,
				 void *user_data)
{

  entropy_gui_component_instance *instance = user_data;
  entropy_layout_gui *layout = instance->layout_parent->data;
  entropy_gui_component_instance *iter;


  /*Hide all the other local viewers, and disable them */
  ecore_list_goto_first (layout->local_components);
  while ((iter = ecore_list_next (layout->local_components))) {
    if (iter != instance) {
      entropy_gui_component_instance_disable (iter);

      if (EWL_WIDGET (iter->gui_object)->parent
	  && EWL_WIDGET (iter->gui_object)->parent ==
	  layout->local_container) {
	printf ("Removed a local view..\n");
	ewl_widget_hide (EWL_WIDGET (iter->gui_object));
      }
    }


  }

  entropy_gui_component_instance_enable (instance);

  if (!EWL_WIDGET (instance->gui_object)->parent)
    ewl_container_child_append (EWL_CONTAINER (layout->local_container),
				EWL_WIDGET (instance->gui_object));

  ewl_widget_show (EWL_WIDGET (instance->gui_object));
}




void
entropy_plugin_layout_main ()
{
  ewl_main ();
}


void
entropy_plugin_destroy (entropy_gui_component_instance * comp)
{
  if (!comp) {
	printf("*** No plugin to destroy at layout_ewl_simple.c\n");
  	return;
  }
	
  entropy_layout_gui *gui = comp->data;
  Ecore_List *keys;
  void *key;
  char write_str[HEADER_CONFIG_MAX];

  keys = ecore_hash_keys (gui->headers);
  ecore_list_goto_first (keys);

  bzero (write_str, HEADER_CONFIG_MAX);
  while ((key = ecore_list_remove_first (keys))) {
    entropy_ewl_layout_header_uri *header =
      ecore_hash_get (gui->headers, key);
    printf ("Saving header '%s' with '%s'\n", header->header, header->uri);
    strcat (write_str, header->header);
    strcat (write_str, ";");
    strcat (write_str, header->uri);
    strcat (write_str, "|");
  }

  printf ("Write string: '%s'\n", write_str);

  entropy_config_str_set ("layout_ewl_simple", "structure_bar", write_str);


  printf ("Destroying layout...\n");
}


void
entropy_plugin_init (entropy_core * core)
{
  int i = 0;
  char **c = NULL;
  /*Init ewl */
  ewl_init (&i, c);
}

void
entropy_delete_current_folder (Ecore_List * el)
{
  ecore_list_destroy (el);
}


entropy_gui_component_instance *
entropy_plugin_layout_create (entropy_core * core)
{
  entropy_gui_component_instance *layout;
  entropy_layout_gui *gui;
  char *tmp = NULL;

  _ewl_layout_count++;


  /*EWL Stuff ----------------- */

  void *(*entropy_plugin_init) (entropy_core * core,
				entropy_gui_component_instance *);

  Ewl_Widget *box;
  Ewl_Widget *tree;
  Ewl_Widget *vbox;
  Ewl_Widget *hbox;
  Ewl_Widget *contract_button;
  Ewl_Widget *expand_button;
  Ewl_Widget *scrollpane;
  Ewl_Widget *menubar;
  Ewl_Widget *menu;
  Ewl_Widget *item;
  
  Ecore_Hash *config_hash;
  Ecore_List *config_hash_keys;
  char* key;
  
  Ecore_List *local_plugins;
  entropy_gui_component_instance *instance;

  entropy_plugin *plugin;
  Ewl_Widget *iconbox = NULL;

  layout = entropy_malloc (sizeof (entropy_gui_component_instance));
  gui = entropy_malloc (sizeof (entropy_layout_gui));
  gui->current_folder = NULL;
  layout->data = gui;
  layout->core = core;
  gui->local_components = ecore_list_new ();
  gui->headers = ecore_hash_new (ecore_direct_hash, ecore_direct_compare);

  /*Register this layout container with the core, so our children can get events */
  entropy_core_layout_register (core, layout);

	/*---------------------------*/
  /*HACK - get the iconbox - this should be configurable */
  local_plugins =
    entropy_plugins_type_get (ENTROPY_PLUGIN_GUI_COMPONENT,
			      ENTROPY_PLUGIN_GUI_COMPONENT_LOCAL_VIEW);

  if (local_plugins) {
    while ((plugin = ecore_list_remove_first (local_plugins))) {
      entropy_gui_component_instance *instance;
      char *name = NULL;

      entropy_plugin_init = dlsym (plugin->dl_ref, "entropy_plugin_init");
      instance = (*entropy_plugin_init) (core, layout);
      gui->iconbox_viewer = instance;

      gui->iconbox_viewer->plugin = plugin;
      instance->plugin = plugin;

      name = entropy_plugin_plugin_identify (plugin);
      printf ("Loaded '%s'...\n", name);

      //FIXME default to icon view for now
      if (!strcmp (name, "Icon View")) {
	iconbox = EWL_WIDGET (gui->iconbox_viewer->gui_object);
	entropy_gui_component_instance_enable (instance);
      }
      else {
	entropy_gui_component_instance_disable (instance);
      }

      /*Add this plugin to the local viewers list */
      ecore_list_append (gui->local_components, instance);
    }
  }
  else {
    fprintf (stderr, "No visual component found! *****\n");
    return NULL;
  }

  /*EWL Setup */
  win = ewl_window_new ();
  ewl_window_dnd_aware_set (EWL_WINDOW (win));

  box = ewl_vbox_new ();
  vbox = ewl_vbox_new ();
  hbox = ewl_hbox_new ();

  scrollpane = ewl_scrollpane_new ();
  contract_button = ewl_button_new ();
  ewl_button_label_set (EWL_BUTTON (contract_button), "<");
  expand_button = ewl_button_new ();
  ewl_button_label_set (EWL_BUTTON (expand_button), ">");

  tree = ewl_scrollpane_new ();
  ewl_box_spacing_set (EWL_BOX (EWL_SCROLLPANE (tree)->box), 5);
  gui->tree = tree;

  gui->paned = ewl_hpaned_new ();
  gui->local_container = ewl_cell_new ();

  ewl_window_title_set (EWL_WINDOW (win), "Entropy");
  ewl_window_name_set (EWL_WINDOW (win), "Entropy");
  ewl_window_class_set (EWL_WINDOW (win), "Entropy");

  ewl_object_size_request (EWL_OBJECT (win), 800, 600);

  ewl_object_maximum_size_set (EWL_OBJECT (contract_button), 20, 10);
  ewl_object_maximum_size_set (EWL_OBJECT (expand_button), 20, 10);

  /*Main menu setup */
  menubar = ewl_menubar_new ();
  ewl_object_fill_policy_set (EWL_OBJECT (menubar), EWL_FLAG_FILL_HFILL);
  ewl_widget_show (menubar);


  menu = ewl_menu_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (menu), "File");
  ewl_container_child_append (EWL_CONTAINER (menubar), menu);
  ewl_object_fill_policy_set (EWL_OBJECT (menu), EWL_FLAG_FILL_NONE);
  ewl_widget_show (menu);

  item = ewl_menu_item_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (item), "Exit");
  ewl_container_child_append (EWL_CONTAINER (menu), item);
  ewl_callback_append (EWL_WIDGET (item), EWL_CALLBACK_CLICKED,
		       __destroy_main_window, core);
  ewl_widget_show (item);


  menu = ewl_menu_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (menu), "Tools");
  ewl_container_child_append (EWL_CONTAINER (menubar), menu);
  ewl_object_fill_policy_set (EWL_OBJECT (menu), EWL_FLAG_FILL_NONE);
  ewl_widget_show (menu);

  item = ewl_menu_item_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (item), "Add Location...");
  ewl_container_child_append (EWL_CONTAINER (menu), item);
  ewl_callback_append (EWL_WIDGET (item), EWL_CALLBACK_CLICKED,
		       location_add_cb, layout);
  ewl_widget_show (item);

  item = ewl_menu_item_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (item), "Setup MIME Actions...");
  ewl_container_child_append (EWL_CONTAINER (menu), item);
  ewl_callback_append (EWL_WIDGET (item), EWL_CALLBACK_CLICKED, mime_cb,
		       layout);
  ewl_widget_show (item);

  menu = ewl_menu_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (menu), "View");
  ewl_container_child_append (EWL_CONTAINER (menubar), menu);
  ewl_object_fill_policy_set (EWL_OBJECT (menu), EWL_FLAG_FILL_NONE);
  ewl_widget_show (menu);

  ecore_list_goto_first (gui->local_components);
  while ((instance = ecore_list_next (gui->local_components))) {
    char *name = entropy_plugin_plugin_identify (instance->plugin);

    item = ewl_menu_item_new ();
    ewl_menu_item_text_set (EWL_MENU_ITEM (item), name);
    ewl_container_child_append (EWL_CONTAINER (menu), item);
    ewl_callback_append (EWL_WIDGET (item), EWL_CALLBACK_CLICKED,
			 layout_ewl_simple_local_view_cb, instance);
    ewl_widget_show (item);
  }

  item = ewl_menu_item_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (item), "Hide Tree View");
  ewl_container_child_append (EWL_CONTAINER (menu), item);
  ewl_callback_append (EWL_WIDGET (item), EWL_CALLBACK_CLICKED,
		       layout_ewl_simple_structure_view_cb, layout);
  ewl_widget_show (item);

  item = ewl_menu_item_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (item), "Show Tree View");
  ewl_container_child_append (EWL_CONTAINER (menu), item);
  ewl_callback_append (EWL_WIDGET (item), EWL_CALLBACK_CLICKED,
		       layout_ewl_simple_structure_view_show_cb, layout);
  ewl_widget_show (item);


  menu = ewl_menu_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (menu), "Debug");
  ewl_container_child_append (EWL_CONTAINER (menubar), menu);
  ewl_object_fill_policy_set (EWL_OBJECT (menu), EWL_FLAG_FILL_NONE);
  ewl_widget_show (menu);

  item = ewl_menu_item_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (item), "File Cache");
  ewl_container_child_append (EWL_CONTAINER (menu), item);
  ewl_widget_show (item);






  menu = ewl_spacer_new ();
  ewl_container_child_append (EWL_CONTAINER (menubar), menu);
  ewl_widget_show (menu);


  menu = ewl_menu_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (menu), "Help");
  ewl_container_child_append (EWL_CONTAINER (menubar), menu);
  ewl_object_fill_policy_set (EWL_OBJECT (menu), EWL_FLAG_FILL_NONE);
  ewl_widget_show (menu);

  item = ewl_menu_item_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (item), "Tip Of The Day..");
  ewl_container_child_append (EWL_CONTAINER (menu), item);
  ewl_callback_append (EWL_WIDGET (item), EWL_CALLBACK_CLICKED,
		       ewl_layout_simple_tooltip_show_cb, layout);
  ewl_widget_show (item);

  item = ewl_menu_item_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (item), "About..");
  ewl_container_child_append (EWL_CONTAINER (menu), item);
  ewl_callback_append (EWL_WIDGET (item), EWL_CALLBACK_CLICKED,
		       ewl_layout_simple_about_dialog_cb, layout);
  ewl_widget_show (item);
	/*-------------------------------*/



  /*Context menu */
  gui->context_menu_floater = ewl_floater_new ();
  ewl_container_child_append (EWL_CONTAINER (win), gui->context_menu_floater);

  gui->context_menu = ewl_menu_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (gui->context_menu), " ");
  ewl_container_child_append (EWL_CONTAINER (gui->context_menu_floater),
			      gui->context_menu);


  item = ewl_menu_item_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (item), "Edit");
  ewl_container_child_append (EWL_CONTAINER (gui->context_menu), item);
  ewl_widget_show (item);

  item = ewl_menu_item_new ();
  ewl_menu_item_text_set (EWL_MENU_ITEM (item), "Delete");
  ewl_container_child_append (EWL_CONTAINER (gui->context_menu), item);
  ewl_callback_append (item, EWL_CALLBACK_CLICKED,
		       location_menu_popup_delete_cb, layout);
  ewl_widget_show (item);

  ewl_widget_show (gui->context_menu);

	/*--------------------------*/
  ewl_container_child_append (EWL_CONTAINER (hbox), expand_button);
  ewl_container_child_append (EWL_CONTAINER (hbox), contract_button);
  ewl_container_child_append (EWL_CONTAINER (box), menubar);
  ewl_container_child_append (EWL_CONTAINER (box), gui->paned);

  ewl_container_child_append (EWL_CONTAINER (gui->paned), gui->tree);

  ewl_container_child_append (EWL_CONTAINER (gui->paned),
			      gui->local_container);
  ewl_container_child_append (EWL_CONTAINER (gui->local_container), iconbox);

  if (!(tmp = entropy_config_str_get ("layout_ewl_simple", "structure_bar"))) {
    layout_ewl_simple_config_create (core);
    tmp = entropy_config_str_get ("layout_ewl_simple", "structure_bar");
  }

  printf ("Config for layout is: '%s' (%d)\n", tmp, strlen (tmp));
  
  config_hash = entropy_config_standard_structures_parse (layout, tmp);
  config_hash_keys = ecore_hash_keys(config_hash);
  while ( (key = ecore_list_remove_first(config_hash_keys))) {
	  char* uri = ecore_hash_get(config_hash, key);
	  layout_ewl_simple_add_header (layout, key, uri);
	  
	  ecore_hash_remove(config_hash, key);
	  free(key);
	  free(uri);
  }
  ecore_list_destroy(config_hash_keys);
  ecore_hash_destroy(config_hash);
  
  entropy_free (tmp);

  ewl_widget_show (box);
  ewl_widget_show (vbox);
  ewl_widget_show (hbox);
  ewl_widget_show (contract_button);
  ewl_widget_show (expand_button);
  ewl_widget_show (scrollpane);
  ewl_widget_show (gui->paned);
  ewl_widget_show (gui->local_container);
  ewl_widget_show (tree);

  ewl_container_child_append (EWL_CONTAINER (win), box);

  ewl_callback_append (win, EWL_CALLBACK_DELETE_WINDOW,
		       __destroy_main_window, core);

  //ewl_container_child_resize(EWL_WIDGET(EWL_PANED(paned)->first), 80, EWL_ORIENTATION_HORIZONTAL);


  /*Tooltip display function */
  entropy_ewl_layout_simple_tooltip_window ();

  layout->gui_object = win;
  ewl_widget_show (win);
  return layout;
}
