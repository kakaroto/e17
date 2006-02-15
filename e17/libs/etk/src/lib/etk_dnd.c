/** @file etk_dnd.c */
#include "etk_dnd.h"
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include "etk_main.h"
#include "etk_widget.h"
#include "etk_toplevel_widget.h"
#include "etk_window.h"
#include "config.h"

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
 * selection_received: when our widget gets the selection it has asked for
 *                     (ie, we want to get a selection)
 * selection_get: when a request for a selection has been made to our widget
 *                (ie, someone is getting our selection from us)
 * selection_clear_event: the selection has been cleared
 * 
 * clipboard_received: when our widget gets the clipboard data it has asked for
 *                     (ie, we want to get a clipboard's text / image)
 * 
 */

#define ETK_DND_INSIDE(x, y, xx, yy, ww, hh) (((x) < ((xx) + (ww))) && ((y) < ((yy) + (hh))) && ((x) >= (xx)) && ((y) >= (yy)))

extern Etk_Widget  *_etk_selection_widget;
extern Etk_Widget  *_etk_drag_widget;

static char       **_etk_dnd_types          = NULL;
static int          _etk_dnd_types_num      = 0;
static Etk_Widget  *_etk_dnd_widget         = NULL;
static Evas_List   *_etk_dnd_handlers       = NULL;
static int          _etk_dnd_widget_accepts = 0;

#if HAVE_ECORE_X
static void _etk_xdnd_container_get_widgets_at(Etk_Toplevel_Widget *top, int x, int y, int offx, int offy, Evas_List **list);
static int _etk_xdnd_enter_handler(void *data, int type, void *event);
static int _etk_xdnd_position_handler(void *data, int type, void *event);
static int _etk_xdnd_drop_handler(void *data, int type, void *event);
static int _etk_xdnd_leave_handler(void *data, int type, void *event);
static int _etk_xdnd_selection_handler(void *data, int type, void *event);
static int _etk_xdnd_status_handler(void *data, int type, void *event);
static int _etk_xdnd_finished_handler(void *data, int type, void *event);
#endif

/**************************
 *
 * Implementation
 *
 **************************/
 
/**
 * @brief Inits the drag and drop system
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure
 */
Etk_Bool etk_dnd_init()
{
   if (_etk_dnd_handlers)
      return ETK_TRUE;
   
#if HAVE_ECORE_X
   _etk_dnd_handlers = evas_list_append(_etk_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_ENTER, _etk_xdnd_enter_handler, NULL));
   _etk_dnd_handlers = evas_list_append(_etk_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_POSITION, _etk_xdnd_position_handler, NULL));
   _etk_dnd_handlers = evas_list_append(_etk_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_DROP, _etk_xdnd_drop_handler, NULL));
   _etk_dnd_handlers = evas_list_append(_etk_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_LEAVE, _etk_xdnd_leave_handler, NULL));
   _etk_dnd_handlers = evas_list_append(_etk_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_SELECTION_NOTIFY, _etk_xdnd_selection_handler, NULL));
   _etk_dnd_handlers = evas_list_append(_etk_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_STATUS, _etk_xdnd_status_handler, NULL));
   _etk_dnd_handlers = evas_list_append(_etk_dnd_handlers, ecore_event_handler_add(ECORE_X_EVENT_XDND_FINISHED, _etk_xdnd_finished_handler, NULL));
#endif
   
   return ETK_TRUE;
}

/**
 * @brief Shutdowns the drag and drop system
 */
void etk_dnd_shutdown()
{
#if HAVE_ECORE_X
   while (_etk_dnd_handlers)
   {
      ecore_event_handler_del(_etk_dnd_handlers->data);
      _etk_dnd_handlers = evas_list_remove(_etk_dnd_handlers, _etk_dnd_handlers->data);
   }
#endif   
}

/**************************
 *
 * Private functions
 *
 **************************/

#if HAVE_ECORE_X
/**
 * @brief Search the container recursively for the widget that accepts xdnd
 * @param top top level widget
 * @param x the x coord we're searching under
 * @param y the y coord we're searching under
 * @param xoff the x offset for the window
 * @param yoff the y offset for the window
 * @param list the evas list we're going to append widgets to
 */
static void _etk_xdnd_container_get_widgets_at(Etk_Toplevel_Widget *top, int x, int y, int offx, int offy, Evas_List **list)
{

   Evas_List *l;
   int wx, wy, ww, wh;
   
   if (!top || !list)
      return;
   
   for (l = etk_widget_dnd_dest_widgets_get(); l; l = l->next)
   {
      Etk_Widget *widget;
      
      if (!(widget = ETK_WIDGET(l->data)) || etk_widget_toplevel_parent_get(widget) != top)
         continue;
      
      etk_widget_geometry_get(widget, &wx, &wy, &ww, &wh);
      if (ETK_DND_INSIDE(x, y, wx + offx, wy + offy, ww, wh))
	 *list = evas_list_append(*list, widget);
   }
}

/**
 * @brief The event handler for when a drag enters our window
 */
static int _etk_xdnd_enter_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Enter *ev;
   int i;
   
   ev = event;
   
   //printf("enter window!\n");   
   //for (i = 0; i < ev->num_types; i++)
   //  printf("type: %s\n", ev->types[i]);   
   
   if(_etk_dnd_types != NULL && _etk_dnd_types_num >= 0)
   {
      for (i = 0; i < _etk_dnd_types_num; i++)
	if(_etk_dnd_types[i]) free(_etk_dnd_types[i]);
   }
   
   if(_etk_dnd_types != NULL) free(_etk_dnd_types);
   _etk_dnd_types_num = 0;
   
   if(ev->num_types > 0)
   {
      _etk_dnd_types = calloc(ev->num_types, sizeof(char*));
      for (i = 0; i < ev->num_types; i++)
	_etk_dnd_types[i] = strdup(ev->types[i]);

      _etk_dnd_types_num = ev->num_types;
   }
   
   return 1;
}

/**
 * @brief The event handler for when a drag is moving in our window
 */
static int _etk_xdnd_position_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Position *ev;
   Etk_Window *window;
   Evas_List *l;
   Evas_List *children = NULL;
   Etk_Widget *widget;
   int x = 0, y = 0;
   int wx, wy, ww, wh;
   
   ev = event;

   //printf("position!\n");
   
   /* loop top level widgets (windows) */
   for (l = etk_main_toplevel_widgets_get(); l; l = l->next)
   {
      if (!ETK_IS_WINDOW(l->data))
         continue;
      window = ETK_WINDOW(l->data);
	
      /* if this isnt the active window, dont waste time */
      if (ev->win != window->x_window)
         continue;	  
	
      ecore_evas_geometry_get(window->ecore_evas, &x, &y, NULL, NULL);

      /* find the widget we want to drop on */
      _etk_xdnd_container_get_widgets_at(ETK_TOPLEVEL_WIDGET(window), ev->position.x, ev->position.y, x, y, &children);
      
      /* check if we're leaving a widget */
      if (_etk_dnd_widget)
      {
         etk_widget_geometry_get(_etk_dnd_widget, &wx, &wy, &ww, &wh);
         if (!ETK_DND_INSIDE(ev->position.x, ev->position.y, wx + x, wy + y, ww, wh))
         {
            etk_widget_drag_leave(_etk_dnd_widget);
            _etk_dnd_widget = NULL;
	    _etk_dnd_widget_accepts = 0;
         }
      }
      
      break;
   }
   
   /* if we found a widget, emit signals */
   if (children != NULL)
   {	
      Ecore_X_Rectangle rect;
      int i;
      
      widget = (evas_list_last(children))->data;
      etk_widget_geometry_get(widget, &wx, &wy, &ww, &wh);

      rect.x = wx;
      rect.y = wy;
      rect.width = ww;
      rect.height = wh;
      
      if(_etk_dnd_widget == widget && _etk_dnd_widget_accepts)
      {
	 etk_widget_drag_motion(widget);
	 ecore_x_dnd_send_status(1, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
      }
      else
      {      
	 _etk_dnd_widget = widget;
      
	 /* first case - no specific types, so just accept */
	 if(_etk_dnd_widget->dnd_types == NULL || _etk_dnd_widget->dnd_types_num <= 0)
	 {
	    ecore_x_dnd_send_status(1, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
	    _etk_dnd_widget_accepts = 1;
	    etk_widget_drag_enter(widget);
	    return 1;
	 }
	 
	 /* second case - we found matching types, accept */
	 for(i = 0; i < _etk_dnd_types_num; i++)
	   {
	      int j;
	      
	      for(j = 0; j < _etk_dnd_widget->dnd_types_num; j++)
		{
		   if(!strcmp(_etk_dnd_widget->dnd_types[j], _etk_dnd_types[i]))
		   {
		      ecore_x_dnd_send_status(1, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
		      _etk_dnd_widget_accepts = 1;
		      etk_widget_drag_enter(widget);
		      return 1;
		   }
		}
	   }
	 
	 /* third case - no matches at all, dont accept */
	 ecore_x_dnd_send_status(0, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
	 _etk_dnd_widget_accepts = 0;
      }
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

/* TODO: doc */
static int _etk_xdnd_drop_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Drop *ev;
   int i;
   
   //printf("drop\n");
   ev = event;
   
   /* first case - if we dont have a type preferece, send everyting */
   if(_etk_dnd_widget->dnd_types == NULL || _etk_dnd_widget->dnd_types_num <= 0)
   {
      for(i = 0; i < _etk_dnd_types_num; i++)
	ecore_x_selection_xdnd_request(ev->win, _etk_dnd_types[i]);
   }
   /* second case - send only our preferred types */
   else
   {
      for(i = 0; i < _etk_dnd_widget->dnd_types_num; i++)
	ecore_x_selection_xdnd_request(ev->win, _etk_dnd_widget->dnd_types[i]);
   }
   
   return 1;
}

/* TODO: doc */
static int _etk_xdnd_leave_handler(void *data, int type, void *event)
{
   //printf("leave window\n");
      
   return 1;
}

/* TODO: doc */
static int _etk_xdnd_selection_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Selection_Notify *ev;
   Ecore_X_Selection_Data *sel;
   Ecore_X_Selection_Data_Files *files;
   Ecore_X_Selection_Data_Text *text;
   Ecore_X_Selection_Data_Targets *targets;
   //int i;

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
	    /* emit signal to widget that the clipboard text is sent to it */
	    Etk_Event_Selection_Request event;
	    Etk_Selection_Data_Text     event_text;
	    
	    text = ev->data;
	   
	    if (!_etk_selection_widget)
	      break;	    
	    
	    event_text.text = text->text;
	    event_text.data.data = text->data.data;
	    event_text.data.length = text->data.length;
	    event_text.data.free = text->data.free;
	    
	    event.data = &event_text;
	    event.content = ETK_SELECTION_CONTENT_TEXT;	    	    
	    
	    etk_widget_selection_received(_etk_selection_widget, &event);
	 }
	 break;
      
      case ECORE_X_SELECTION_SECONDARY:
         sel = ev->data;
	 printf("secondary: %s %s\n", ev->target, sel->data);
         break;
      
      case ECORE_X_SELECTION_XDND:
        if(!strcmp(ev->target, "text/uri-list"))
	 {
	    Etk_Event_Selection_Request event;	    
	    Etk_Selection_Data_Files    event_files;
	    
	    files = ev->data;
	
	    if (!_etk_dnd_widget || files->num_files < 1)
	      break;		    
	    
	    event_files.files = files->files;
	    event_files.num_files = files->num_files;
	    event_files.data.data = files->data.data;
	    event_files.data.length = files->data.length;
	    event_files.data.free = files->data.free;
	    
	    event.data = &event_files;
	    event.content = ETK_SELECTION_CONTENT_FILES;	   
	    
	    /* emit the drop signal so the widget can react */
	    etk_widget_drag_drop(_etk_dnd_widget, &event);
	 }
         else if(!strcmp(ev->target, "text/plain") || 
		 !strcmp(ev->target, ECORE_X_SELECTION_TARGET_UTF8_STRING))
	 {
	    Etk_Event_Selection_Request event;
	    Etk_Selection_Data_Text     event_text;
	    
	    text = ev->data;
	   
	    if (!_etk_dnd_widget)
	      break;	    
	    
	    event_text.text = text->text;
	    event_text.data.data = text->data.data;
	    event_text.data.length = text->data.length;
	    event_text.data.free = text->data.free;
	    
	    event.data = &event_text;
	    event.content = ETK_SELECTION_CONTENT_TEXT;	    
	    
	    /* emit the drop signal so the widget can react */
	    etk_widget_drag_drop(_etk_dnd_widget, &event);
	 }
         else
	 {
	    /* couldnt find any data type that etk supports, send raw data */
	    Etk_Event_Selection_Request event;
	    
	    event.data = ev->data;
	    event.content = ETK_SELECTION_CONTENT_CUSTOM;
	    
	    /* emit the drop signal so the widget can react */
	    etk_widget_drag_drop(_etk_dnd_widget, &event);
	 }
      
	 _etk_dnd_widget = NULL;	 
	 
         ecore_x_dnd_send_finished();
         break;
	
      case ECORE_X_SELECTION_CLIPBOARD:
         if (!strcmp(ev->target, ECORE_X_SELECTION_TARGET_TARGETS)) 
	 {
	    /* REDO THIS CODE!!!
	    
	    Etk_Event_Selection_Get event;
	    Etk_Selection_Data_Targets _targets;
	    
	    event.content = ETK_SELECTION_CONTENT_TARGETS;

	    targets = ev->data;
	    
            _targets.num_targets = targets->num_targets;
	    _targets.targets = targets->targets;
	    
	    //printf("clipboard: %s\n", ev->target);
	    //for (i = 0; i < targets->num_targets; i++)
	    //  printf("target: %s\n", targets->targets[i]);
	    
	    */
         } 
         else 
	 {
	    /* emit signal to widget that the clipboard text is sent to it */
	    Etk_Event_Selection_Request event;
	    Etk_Selection_Data_Text     event_text;
	    
	    text = ev->data;
	   
	    if (!_etk_selection_widget)
	      break;	    
	    
	    event_text.text = text->text;
	    event_text.data.data = text->data.data;
	    event_text.data.length = text->data.length;
	    event_text.data.free = text->data.free;
	    
	    event.data = &event_text;
	    event.content = ETK_SELECTION_CONTENT_TEXT;	    
	    
	    etk_widget_clipboard_received(_etk_selection_widget, &event);
	 }
	 break;
         
      default:
         break;
   }
   
   return 1;
}

static int _etk_xdnd_status_handler(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Status *ev;
   
   ev = event;
   
   if (ev->win != (ETK_WINDOW(_etk_drag_widget))->x_window) return 1;
   
   if(!ev->will_accept)
   {
  //    etk_toplevel_widget_pointer_pop(etk_widget_toplevel_parent_get(ETK_WIDGET(_etk_drag_widget)), ETK_POINTER_RESIZE_BR);
      return 1;
   }
   
//   etk_toplevel_widget_pointer_push(etk_widget_toplevel_parent_get(ETK_WIDGET(_etk_drag_widget)), ETK_POINTER_RESIZE_BR);
   etk_toplevel_widget_pointer_push(ETK_TOPLEVEL_WIDGET(ETK_WINDOW(_etk_drag_widget)), ETK_POINTER_RESIZE_BR);
   
   return 1;
}

static int _etk_xdnd_finished_handler(void *data, int type, void *event)
{
   return 1;
}

#endif

/** @} */
