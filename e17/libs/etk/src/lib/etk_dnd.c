/** @file etk_dnd.c */
#include "etk_dnd.h"
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

#if HAVE_ECORE_X

/* Some notes for when we're developing this:
 * Signals:
 * - source:
 * drag_begin: drag has started
 * drag_end: drag has ended
 * 
 * - destination:
 * drag_leave: mouse leaves widget
 * drag_motion: mouse is moving on widget
 * drag_drop: object dropped on widget
 * 
 */

extern Evas_List *_etk_main_toplevel_widgets;

static Etk_Widget *_etk_dnd_widget = NULL;
static Etk_Widget *_etk_selection_widget = NULL;
Evas_List  *_etk_dnd_widgets = NULL;
Evas_List  *_etk_dnd_handlers = NULL;

#define E_INSIDE(x, y, xx, yy, ww, hh) (((x) < ((xx) + (ww))) && ((y) < ((yy) + (hh))) && ((x) >= (xx)) && ((y) >= (yy)))

static void _etk_xdnd_container_get_widgets_at(Etk_Toplevel_Widget *top, int x, int y, int offx, int offy, Evas_List **list);
static int  _etk_xdnd_enter_handler(void *data, int type, void *event);
static int  _etk_xdnd_position_handler(void *data, int type, void *event);
static int  _etk_xdnd_drop_handler(void *data, int type, void *event);
static int  _etk_xdnd_leave_handler(void *data, int type, void *event);
static int  _etk_xdnd_selection_handler(void *data, int type, void *event);
	    
int etk_dnd_init()
{
   if(!ecore_x_init(NULL))
     return 0;
   
   _etk_dnd_handlers = evas_list_append(_etk_dnd_handlers,
    ecore_event_handler_add(ECORE_X_EVENT_XDND_ENTER, 
			    _etk_xdnd_enter_handler, NULL));
					
   _etk_dnd_handlers = evas_list_append(_etk_dnd_handlers,
    ecore_event_handler_add(ECORE_X_EVENT_XDND_POSITION, 
			    _etk_xdnd_position_handler, NULL));
   
   _etk_dnd_handlers = evas_list_append(_etk_dnd_handlers,
    ecore_event_handler_add(ECORE_X_EVENT_XDND_DROP, 
			    _etk_xdnd_drop_handler, NULL));
   
   _etk_dnd_handlers = evas_list_append(_etk_dnd_handlers,
    ecore_event_handler_add(ECORE_X_EVENT_XDND_LEAVE, 
			    _etk_xdnd_leave_handler, NULL));
   
   _etk_dnd_handlers = evas_list_append(_etk_dnd_handlers,
    ecore_event_handler_add(ECORE_X_EVENT_SELECTION_NOTIFY, 
			    _etk_xdnd_selection_handler, NULL));
   
   return 1;
}

int etk_dnd_shutdown()
{
   while(_etk_dnd_handlers)
     {
	ecore_event_handler_del(_etk_dnd_handlers->data);
	_etk_dnd_handlers = evas_list_remove(_etk_dnd_handlers, 
					     _etk_dnd_handlers->data);
     }
   
   return ecore_x_shutdown();
}

void etk_selection_text_request(Etk_Widget *widget)
{
   Ecore_X_Window win;
   
   if(!ETK_IS_WINDOW(widget->toplevel_parent))
     return;
   
   win = ETK_WINDOW(widget->toplevel_parent)->x_window;
   _etk_selection_widget = widget;
   ecore_x_selection_clipboard_request(win, ECORE_X_SELECTION_TARGET_UTF8_STRING);
}

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
   //int i;
   
   ev = event;
   
   printf("enter window!\n");
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

   //printf("position!\n");
   
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
   else
     {
	/* tell the source we wont accept it here */
	Ecore_X_Rectangle rect;
	
	rect.x = 0;
	rect.y = 0;
	rect.width = 0;
	rect.height = 0;
	ecore_x_dnd_send_status(0, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
     }
   return 1;
}

static int _etk_xdnd_drop_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Drop *ev;
   
   printf("drop\n");
   ev = event;
      
   ecore_x_selection_xdnd_request(ev->win, "text/uri-list");
   return 1;
}

static int _etk_xdnd_leave_handler(void *data, int type, void *event)
{
   printf("leave window\n");
      
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

   printf("selection\n"); 
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
	_etk_dnd_widget = NULL;
	
	ecore_x_dnd_send_finished();
	break;
	
      case ECORE_X_SELECTION_CLIPBOARD:
	if (!strcmp(ev->target, ECORE_X_SELECTION_TARGET_TARGETS)) 
	  {
	     Etk_Event_Selection_Get event;
	     Etk_Selection_Data_Targets _targets;
	     
	     event.content = ETK_SELECTION_CONTENT_TARGETS;

	     targets = ev->data;
	     
	     _targets.num_targets = targets->num_targets;
	     _targets.targets = targets->targets;
	     
	     //printf("clipboard: %s\n", ev->target);
	     //for (i = 0; i < targets->num_targets; i++)
	     //  printf("target: %s\n", targets->targets[i]);
	  } 
	else 
	  {
	     Etk_Event_Selection_Get event;
	     
	     text = ev->data;
	     //printf("clipboard: %s %s\n", ev->target, text->text);
	     event.data = text->text;
	     event.content = ETK_SELECTION_CONTENT_TEXT;
	     etk_widget_selection_get(_etk_selection_widget, &event);
	  }
	break;
     }
   
   return 1;
}

#endif
