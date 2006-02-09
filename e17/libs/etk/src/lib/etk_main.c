/** @file etk_main.c */
#include "etk_main.h"
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Job.h>
#include <Ecore_Evas.h>
#include <Evas.h>
#include <Edje.h>
#include "etk_type.h"
#include "etk_signal.h"
#include "etk_object.h"
#include "etk_toplevel_widget.h"
#include "etk_utils.h"
#include "etk_theme.h"
#include "etk_window.h"
#include "config.h"

/**
 * @addtogroup Etk_Main
 * @{
 */

static void _etk_main_iterate_job_cb(void *data);
static void _etk_main_size_request_recursive(Etk_Widget *widget);
static void _etk_main_size_allocate_recursive(Etk_Widget *widget, Etk_Bool is_top_level);

#if HAVE_ECORE_X
static int _etk_xdnd_enter_handler(void *data, int type, void *event);
static int _etk_xdnd_position_handler(void *data, int type, void *event);
static int _etk_xdnd_drop_handler(void *data, int type, void *event);
static int _etk_xdnd_leave_handler(void *data, int type, void *event);
static int _etk_xdnd_selection_handler(void *data, int type, void *event);

static Etk_Widget *_etk_dnd_widget = NULL;
Evas_List  *_etk_dnd_widgets = NULL;
#endif	  

static Evas_List *_etk_main_toplevel_widgets = NULL;
static Etk_Bool _etk_main_running = ETK_FALSE;
static Etk_Bool _etk_main_initialized = ETK_FALSE;
static Ecore_Job *_etk_main_iterate_job = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Initializes etk
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure
 */
Etk_Bool etk_init()
{
   if (_etk_main_initialized)
      return ETK_TRUE;
   
   if (!evas_init())
   {
      ETK_WARNING("Evas initialization failed!");
      return ETK_FALSE;
   }
   if (!ecore_init())
   {
      ETK_WARNING("Ecore initialization failed!");
      return ETK_FALSE;
   }
   if (!ecore_evas_init())
   {
      ETK_WARNING("Ecore_Evas initialization failed!");
      return ETK_FALSE;
   }
   if (!edje_init())
   {
      ETK_WARNING("Edje initialization failed!");
      return ETK_FALSE;
   }
   
#if HAVE_ECORE_X
   if (!ecore_x_init(NULL))
     {
	ETK_WARNING("Ecore_X initialzation failed!");
	return ETK_FALSE;
     }
#endif   
   
   etk_theme_init();

   /* Gettext */
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

#if HAVE_ECORE_X
   ecore_event_handler_add(ECORE_X_EVENT_XDND_ENTER, _etk_xdnd_enter_handler, NULL);
   ecore_event_handler_add(ECORE_X_EVENT_XDND_POSITION, _etk_xdnd_position_handler, NULL);
   ecore_event_handler_add(ECORE_X_EVENT_XDND_DROP, _etk_xdnd_drop_handler, NULL);
   ecore_event_handler_add(ECORE_X_EVENT_XDND_LEAVE, _etk_xdnd_leave_handler, NULL);
   ecore_event_handler_add(ECORE_X_EVENT_SELECTION_NOTIFY, _etk_xdnd_selection_handler, NULL);
#endif
   
   _etk_main_initialized = ETK_TRUE;
   return ETK_TRUE;
}

/** @brief Shutdowns etk and frees the memory */
void etk_shutdown()
{
   if (!_etk_main_initialized)
      return;

   etk_object_destroy_all_objects();
   etk_signal_shutdown();
   etk_type_shutdown();
   etk_theme_shutdown();
   edje_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();
   evas_shutdown();
   _etk_main_toplevel_widgets = evas_list_free(_etk_main_toplevel_widgets);

   _etk_main_initialized = ETK_FALSE;
}

/** @brief Enters the main loop */
void etk_main()
{
   if (!_etk_main_initialized || _etk_main_running)
      return;
   
   _etk_main_running = ETK_TRUE;
   ecore_main_loop_begin();
}

/** @brief Leaves the main loop */
void etk_main_quit()
{
   if (!_etk_main_running)
      return;

   ecore_main_loop_quit();
   _etk_main_running = ETK_FALSE;
   if (_etk_main_iterate_job)
      ecore_job_del(_etk_main_iterate_job);
   _etk_main_iterate_job = NULL;
}

/** @brief Runs an iteration of the main loop */
void etk_main_iterate()
{
   Evas_List *l;
   Etk_Widget *widget;

   if (!_etk_main_initialized)
      return;

   for (l = _etk_main_toplevel_widgets; l; l = l->next)
   {
      widget = ETK_WIDGET(l->data);
      _etk_main_size_request_recursive(widget);
      _etk_main_size_allocate_recursive(widget, ETK_TRUE);
   }
}

/** @brief Will run an iteration as soon as possible */
void etk_main_iteration_queue()
{
   if (_etk_main_iterate_job)
      return;
   _etk_main_iterate_job = ecore_job_add(_etk_main_iterate_job_cb, NULL);
}

/**
 * @brief Adds the widget to the list of toplevel widgets. Never call it manually
 * @param widget the toplevel widget to add
 */
void etk_main_toplevel_widget_add(Etk_Toplevel_Widget *widget)
{
   if (!widget)
      return;
   _etk_main_toplevel_widgets = evas_list_append(_etk_main_toplevel_widgets, widget);
}

/**
 * @brief Removes the widget from the list of toplevel widgets. Never call it manually!
 * @param widget the toplevel widget to remove
 */
void etk_main_toplevel_widget_remove(Etk_Toplevel_Widget *widget)
{
   if (!widget)
      return;
   _etk_main_toplevel_widgets = evas_list_remove(_etk_main_toplevel_widgets, widget);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Runs an iteration (used as callback for an ecore_job)*/
static void _etk_main_iterate_job_cb(void *data)
{
   _etk_main_iterate_job = NULL;
   etk_main_iterate();
}

/* Recusively requests the size of all the widgets */
static void _etk_main_size_request_recursive(Etk_Widget *widget)
{
   Evas_List *l;
   Etk_Size unused_size;
   
   etk_widget_size_request(widget, &unused_size);
   
   if (ETK_IS_CONTAINER(widget))
   {
      for (l = ETK_CONTAINER(widget)->children; l; l = l->next)
         _etk_main_size_request_recursive(ETK_WIDGET(l->data));
   }
}

/* Recusively allocates the size of all the widgets */
static void _etk_main_size_allocate_recursive(Etk_Widget *widget, Etk_Bool is_top_level)
{
   Evas_List *l;
   Etk_Geometry geometry;
   
   if (is_top_level)
   {
      geometry.x = 0;
      geometry.y = 0;
      etk_toplevel_widget_geometry_get(ETK_TOPLEVEL_WIDGET(widget), NULL, NULL, &geometry.w, &geometry.h);
   }
   else
      geometry = widget->geometry;
   etk_widget_size_allocate(widget, geometry);
   
   if (ETK_IS_CONTAINER(widget))
   {
      for (l = ETK_CONTAINER(widget)->children; l; l = l->next)
         _etk_main_size_allocate_recursive(ETK_WIDGET(l->data), ETK_FALSE);
   }
}


#if HAVE_ECORE_X
#define E_INSIDE(x, y, xx, yy, ww, hh) (((x) < ((xx) + (ww))) && ((y) < ((yy) + (hh))) && ((x) >= (xx)) && ((y) >= (yy)))

/* Search the container recursively for the widget that accepts xdnd */
static void _etk_xdnd_container_get_widgets_at(Etk_Toplevel_Widget *top, int x, int y, int offx, int offy, Evas_List **list)
{
   Evas_List *l;
     
   for(l = _etk_dnd_widgets; l; l = l->next)
     {
	Etk_Widget *widget;
	
	if(!(widget = ETK_WIDGET(l->data)))
	  continue;
	
	if(E_INSIDE(x, y, 
		    widget->inner_geometry.x + offx,
		    widget->inner_geometry.y + offy,
		    widget->inner_geometry.w, 
		    widget->inner_geometry.h))	  
	  *list = evas_list_append(*list, widget);	  
     }   
}

static int _etk_xdnd_enter_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Enter *ev;
   int i;
   
   ev = event;
   
//   printf("enter window!\n");
//	for (i = 0; i < ev->num_types; i++)
//	  printf("type: %s\n", ev->types[i]);   
   
   return 1;
}

static int _etk_xdnd_position_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Position *ev;
   Etk_Window *window;
   Evas_List *l;
   Evas_List *children = NULL;
   Etk_Widget *widget;
   int x = 0, y = 0;
   
   ev = event;
   
   /* loop top level widgets (windows) */
   for(l = _etk_main_toplevel_widgets; l; l = l->next)
     {	  
	if (!(window = ETK_WINDOW(l->data)))
	  continue;
	
	/* if this isnt the active window, dont waste time */
	if(ev->win != window->x_window)
	  continue;	  
	
	ecore_evas_geometry_get(window->ecore_evas, &x, &y, NULL, NULL);

	/* find the widget we want to drop on */
	_etk_xdnd_container_get_widgets_at(ETK_TOPLEVEL_WIDGET(window), ev->position.x, ev->position.y, x, y, &children);
	
	/* check if we're leaving a widget */
	if(_etk_dnd_widget)
	  {
	     if(!E_INSIDE(ev->position.x, ev->position.y, 
			  _etk_dnd_widget->geometry.x + x, _etk_dnd_widget->geometry.y + y,
			  _etk_dnd_widget->geometry.w, _etk_dnd_widget->geometry.h))
	       {
		  etk_widget_drag_leave(_etk_dnd_widget);
		  _etk_dnd_widget = NULL;
	       }
	  }	
	
	break;
     }   
   
   /* if we found a widget, emit signals */
   if(children != NULL)
     {	
	Ecore_X_Rectangle rect;
	
	widget = (evas_list_last(children))->data;
	_etk_dnd_widget = widget;
	/* TODO: filter types according to what widget wants */
	rect.x = widget->inner_geometry.x;
	rect.y = widget->inner_geometry.y;
	rect.width = widget->inner_geometry.w;
	rect.height = widget->inner_geometry.h;	
	ecore_x_dnd_send_status(1, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
	
	etk_widget_drag_motion(widget);
     }
   
   return 1;
}

static int _etk_xdnd_drop_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Drop *ev;
   
   /* printf("drop\n"); */
   ev = event;
      
   ecore_x_selection_xdnd_request(ev->win, "text/uri-list");
   return 1;
}

static int _etk_xdnd_leave_handler(void *data, int type, void *event)
{
   //printf("leave window\n");
      
   return 1;
}

static int _etk_xdnd_selection_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Selection_Notify *ev;
   Ecore_X_Selection_Data *sel;
   Ecore_X_Selection_Data_Files *files;
   Ecore_X_Selection_Data_Text *text;
   Ecore_X_Selection_Data_Targets *targets;
   int i;

   //printf("selection\n"); 
   ev = event;
   switch (ev->selection) 
     {
      case ECORE_X_SELECTION_PRIMARY:
	if (!strcmp(ev->target, ECORE_X_SELECTION_TARGET_TARGETS)) 
	  {
	     /* printf("primary: %s\n", ev->target); */
	     targets = ev->data;
	     /* 
	     for (i = 0; i < targets->num_targets; i++)
	       printf("target: %s\n", targets->targets[i]);
	     */
	  } 
	else 
	  {
	     text = ev->data;
	     /* printf("primary: %s %s\n", ev->target, text->text); */
	  }
	break;
	
      case ECORE_X_SELECTION_SECONDARY:
	sel = ev->data;
	 /* printf("secondary: %s %s\n", ev->target, sel->data); */
	break;
	
      case ECORE_X_SELECTION_XDND:
	 /* printf("xdnd: %s\n", ev->target); */

	files = ev->data;
	
	if(!_etk_dnd_widget || files->num_files < 1)
	  break;	
	
	/* free old data, should this be done here? */
	for (i = 0; i < _etk_dnd_widget->xdnd_files_num; i++)
	  {
	     if(_etk_dnd_widget->xdnd_files[i])
	       free(_etk_dnd_widget->xdnd_files[i]);	     
	  }
	
	if(_etk_dnd_widget->xdnd_files)
	  free(_etk_dnd_widget->xdnd_files);	    
	
	_etk_dnd_widget->xdnd_files = calloc(files->num_files, sizeof(char*));
		
	/* printf("num_files: %d\n", files->num_files); */
	
	/* Fill in the drop data into the widget */
	_etk_dnd_widget->xdnd_files_num = files->num_files;
	for (i = 0; i < files->num_files; i++)
	  {
	     /* printf("file: %s\n", files->files[i]); */
	     _etk_dnd_widget->xdnd_files[i] = strdup(files->files[i]);
	  }
	
	/* emit the drop signal so the widget can react */
	etk_widget_drag_drop(_etk_dnd_widget);
	
	ecore_x_dnd_send_finished();
	break;
	
      case ECORE_X_SELECTION_CLIPBOARD:
	if (!strcmp(ev->target, ECORE_X_SELECTION_TARGET_TARGETS)) 
	  {
	     //printf("clipboard: %s\n", ev->target);
	     targets = ev->data;
	     /*
	     for (i = 0; i < targets->num_targets; i++)
	       printf("target: %s\n", targets->targets[i]);
	     */
	  } 
	else 
	  {
	     text = ev->data;
	     /* printf("clipboard: %s %s\n", ev->target, text->text); */
	  }
	break;
     }
   
   return 1;
}

#endif

/** @} */
