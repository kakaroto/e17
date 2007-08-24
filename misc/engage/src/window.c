#include "engage.h"
#include "config.h"
#include <Ecore_X.h>
#include <Esmart/Esmart_Trans_X11.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif
#ifdef DMALLOC
#include "dmalloc.h"
#endif

Ecore_Evas     *ee;
Evas           *evas;
Ecore_X_Window  od_window;
int             od_hidden;
static Ecore_Timer *mouse_focus_timer = NULL;
int             fullheight;
int             input_shape_full_area = 1;

static void     handle_pre_render_cb(Ecore_Evas * _ee);
static void     handle_post_render_cb(Ecore_Evas * _ee);
static void     handle_mouse_in(Ecore_Evas * _ee);
static void     handle_mouse_out(Ecore_Evas * _ee);
static void     handle_focus_out(Ecore_Evas * _ee);
#if 0
static void     handle_mouse_down(void *data, Evas * e, Evas_Object * obj,
                                  void *event);
#endif
static void     handle_mouse_move(void *data, Evas * e, Evas_Object * obj,
                                  void *event);
static void     handle_menu_draw(void *data, Evas * e, Evas_Object * obj,
                                 void *event);
static void     od_window_set_hidden(int hidden);
static void     od_window_input_shape_rectangle_set(Ecore_X_Window win, int x, int y, int w, int h);
static void     od_window_input_shape_set_extents();

static void
od_window_input_shape_rectangle_set(Ecore_X_Window win, int x, int y, int w, int h)
{
#ifdef ShapeInput
   XRectangle rect;   
   
   rect.x = x;
   rect.y = y;
   rect.width = w;
   rect.height = h;
   XShapeCombineRectangles(ecore_x_display_get(), win, ShapeInput, 0, 0, &rect, 1, ShapeSet, Unsorted);
#endif
}

static void 
od_window_input_shape_set_extents()
{
#ifdef ShapeInput
   Ecore_X_Window win = ecore_evas_software_x11_window_get(ee);
   od_window_input_shape_rectangle_set(win, 
         (int)dock.left_pos, 
         options.height-options.size, 
         ((int)dock.right_pos - (int)dock.left_pos) + tray_width, 
         options.size); 
         
  input_shape_full_area = 0;       
#endif
}


int
od_window_hide_timer_cb(void *data)
{
  int               ret;

  ret = 1;
  if (mouse_focus_timer) {
    if (dock.state == zooming || dock.state == zoomed) {
      od_dock_zoom_out();
      mouse_focus_timer = NULL;
      ret = 0;
    }
  }
  if (options.auto_hide)
    od_window_hide();
  return (ret);
}
static void
handle_pre_render_cb(Ecore_Evas * _ee)
{
  edje_thaw();
  if (need_redraw)
  od_dock_redraw(_ee);
}
static void
handle_post_render_cb(Ecore_Evas * _ee)
{
  edje_freeze();
}
static void
handle_delete_cb(Ecore_Evas * _ee)
{
  if (_ee != ee)
    return;
  ecore_main_loop_quit();
}
static void
handle_focus_out(Ecore_Evas * _ee)
{
  if (_ee != ee)
    return;
  if (mouse_focus_timer)
    ecore_timer_del(mouse_focus_timer);
  mouse_focus_timer = ecore_timer_add(0.75, od_window_hide_timer_cb, NULL);
}
static void
handle_mouse_in(Ecore_Evas * _ee)
{
  if (_ee != ee)
    return;

#ifdef ShapeInput   
  if(options.use_composite)
  {
    Ecore_X_Window win = ecore_evas_software_x11_window_get(_ee);
    od_window_input_shape_rectangle_set(win,0,0,options.width,options.height);
    input_shape_full_area = 1;
  }
#endif
  
  if (mouse_focus_timer)
    ecore_timer_del(mouse_focus_timer);
  mouse_focus_timer = NULL;

  if (options.auto_hide)
    od_window_unhide();
}
static void
handle_mouse_out(Ecore_Evas * _ee)
{
  if (_ee != ee)
    return;
    
  if (mouse_focus_timer)
    ecore_timer_del(mouse_focus_timer);
  mouse_focus_timer = ecore_timer_add(0.5, od_window_hide_timer_cb, NULL);
}

void
od_window_move()
{
  int             x, y, w, h;
  Evas_Object    *o = NULL;

  ecore_evas_geometry_get(ee, &x, &y, &w, &h);

  if ((o = evas_object_name_find(ecore_evas_get(ee), "trans")))
    esmart_trans_x11_freshen(o, x, y, w, h);
}

void
od_window_resize()
{
  int             x, y, w, h;
  Evas_Object    *o = NULL;

  ecore_evas_geometry_get(ee, &x, &y, &w, &h);

  if ((o = evas_object_name_find(ecore_evas_get(ee), "trans"))) {
    evas_object_resize(o, w, h);
    esmart_trans_x11_freshen(o, x, y, w, h);
  }

  options.width = w;
  options.height = h;
  ecore_config_int_set("engage.options.width", w);
/* FIXME this cannot be stored, as mode 1 resizes the window, and will mess
 * things up if stored 
  ecore_config_int_set("engage.options.height", h); */
}

void
od_window_init()
{
  Ecore_X_Display *dsp;
  int             x, y, xinerama;
  int             res_x, res_y;
  Evas_Object    *o;
  Evas_Object    *eventer;

  xinerama = 1;
  fullheight = options.height;
  // determine the desktop size
  dsp = ecore_x_display_get();
#ifdef XINERAMA
  XineramaScreenInfo *screen_info;
  int num_screens;

  {
    int event_base, error_base;

    if(XineramaQueryExtension(dsp, &event_base, &error_base) &&
       (screen_info = XineramaQueryScreens(dsp, &num_screens))) {
      // Double check that the selected head is valid
      if(options.head >= num_screens) {
        fprintf(stderr, "Head %d does not exist; defaulting to head 0\n", options.head);
        options.head = 0;
      }

      res_x = screen_info[options.head].width;
      x     = screen_info[options.head].x_org;
      res_y = screen_info[options.head].height;
      y     = screen_info[options.head].y_org;
    } else
      xinerama = 0;
  }
#else
  xinerama = 0;
#endif

  if(!xinerama) {
    Screen         *scr;
    int             def;
    def = DefaultScreen(dsp);
    scr = ScreenOfDisplay(dsp, def);
    res_x = scr->width;
    x     = 0;
    res_y = scr->height;
    y     = 0;
  }

  if (!(strcmp(options.engine, "gl")))
    ee = ecore_evas_gl_x11_new(NULL, 0,
                               (int) ((res_x - options.width) / 2.0 + x),
                               (int) (res_y - options.height + y), 
                               options.width, options.height);
  else {
    if (strcmp(options.engine, "software")) {
      fprintf(stderr,
              "Warning: Invalid engine type specified in config.\n");
      fprintf(stderr, "         Defaulting to software engine.\n");
    }
    ee = ecore_evas_software_x11_new(NULL, 0,
                                     (int) ((res_x - options.width) / 2.0 + x),
                                     (int) (res_y - options.height + y), 
                                     options.width, options.height);
  }

  ecore_evas_title_set(ee, "Engage");
  ecore_evas_name_class_set(ee, "engage", "engage");

#ifdef ShapeInput  
  if(options.use_composite)
    ecore_evas_alpha_set(ee, 1);
#endif 
  ecore_evas_borderless_set(ee, 1);  
  
  if(options.use_composite)
  {
    ecore_evas_avoid_damage_set(ee, 1);
  }
  else if (options.mode == OM_ONTOP) 
  {
    ecore_evas_avoid_damage_set(ee, 1);
    ecore_evas_shaped_set(ee, 1);
  } 
  else
    ecore_evas_shaped_set(ee, 0);
    
  ecore_evas_callback_post_render_set(ee, handle_post_render_cb);
  ecore_evas_callback_pre_render_set(ee, handle_pre_render_cb);
  ecore_evas_callback_delete_request_set(ee, handle_delete_cb);
  ecore_evas_callback_mouse_out_set(ee, handle_mouse_out);
  ecore_evas_callback_mouse_in_set(ee, handle_mouse_in);
  ecore_evas_callback_focus_out_set(ee, handle_focus_out);

  evas = ecore_evas_get(ee);
  eventer = evas_object_rectangle_add(evas);

  evas_object_color_set(eventer, 0, 0, 0, 0);
  evas_object_resize(eventer, options.width, options.height);
  evas_object_move(eventer, 0.0, 0.0);
  evas_object_layer_set(eventer, 9999);
  evas_object_repeat_events_set(eventer, 1);
  evas_object_show(eventer);
#if 1
  evas_object_event_callback_add(eventer, EVAS_CALLBACK_MOUSE_DOWN,
                                 handle_menu_draw, NULL);
#endif
  evas_object_event_callback_add(eventer, EVAS_CALLBACK_MOUSE_MOVE,
                                 handle_mouse_move, NULL);

  od_window = ecore_evas_software_x11_window_get(ee);
 
 
//  printf("move %d %d\n", (int) ((res_x - options.width) / 2.0 + x),
//                              (int) (res_y - options.height + y);

  ecore_x_window_move(od_window,
                      (int) ((res_x - options.width) / 2.0 + x),
                      (int) (res_y - options.height + y));
  
  ecore_x_netwm_window_type_set(od_window, ECORE_X_WINDOW_TYPE_DOCK);

  // Reserve a strut
  if(options.reserve > 0) {
    // Double check that there isn't a head below the selected head
#ifdef XINERAMA
    bool valid = true;

    if(xinerama) {
      int i;
      for(i=0; i < num_screens; i++) {
        if(i != options.head &&
           ((screen_info[i].x_org >= screen_info[options.head].x_org &&
             screen_info[i].x_org < screen_info[options.head].x_org
                                    + screen_info[options.head].width) ||
             (screen_info[i].x_org + screen_info[i].width
              >= screen_info[options.head].x_org &&
              screen_info[i].x_org + screen_info[i].width
              < screen_info[options.head].x_org
              + screen_info[options.head].width)) &&
            screen_info[i].y_org > screen_info[options.head].y_org
            + screen_info[options.head].height - options.reserve) {
          fprintf(stderr, "Another head is below the selected head; no space will be reserved");
          valid = false;
          i = num_screens;
        }
      }
    }

    if(valid) {
#endif
      unsigned long struts[12] = { 0, 0, 0, options.reserve, 0, 0, 0, 0, 0, 0,
                                  (res_x - options.width) / 2.0 + x,
                                  (res_x - options.width ) / 2.0
                                  + options.width + x };
      ecore_x_window_prop_property_set(od_window, XInternAtom(dsp, "_NET_WM_STRUT_PARTIAL", False),
                                       XA_CARDINAL, 32, struts, 12);
      ecore_x_window_prop_property_set(od_window, XInternAtom(dsp, "_NET_WM_STRUT", False),
                                       XA_CARDINAL, 32, struts, 4);
#ifdef XINERAMA
    }
#endif
  }
  
  if (options.use_composite || options.mode == OM_ONTOP)
    ecore_evas_layer_set(ee, 7);
  else
    ecore_evas_layer_set(ee, 2);
  ecore_x_icccm_size_pos_hints_set(od_window, 1, ECORE_X_GRAVITY_NW, 0, 0,
                                   res_x, res_y, 0, 0, 1, 1, 0.0, 0.0);

  ecore_evas_show(ee);
  ecore_evas_callback_move_set(ee, od_window_move);
  ecore_evas_callback_resize_set(ee, od_window_resize);

  if (options.mode == OM_BELOW && !options.use_composite) {
    o = esmart_trans_x11_new(evas);
    evas_object_layer_set(o, 0);
    evas_object_move(o, 0, 0);
    evas_object_resize(o, options.width, options.height);
    evas_object_name_set(o, "trans");

    esmart_trans_x11_freshen(o, (int) ((res_x - options.width) / 2.0),
                             (int) (res_y - options.height),
                             options.width, options.height);
    evas_object_show(o);
  }

  od_dock_redraw(ee);
  
  
  if (options.auto_hide)
    od_window_hide();
}

#if 0
static void
handle_mouse_down(void *data, Evas * e, Evas_Object * obj, void *event)
{
  Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *) event;
  OD_Icon        *icon = NULL;

  {
    Evas_List      *objects =
      evas_objects_at_xy_get(evas, ev->canvas.x, ev->canvas.y, 0, 0);
    Evas_List      *item = objects;
    bool            done = false;

    while (item && !done) {
      const char     *name = evas_object_name_get((Evas_Object *) item->data);

      if (name && strncmp("icon", name, 4) == 0) {
        Evas_List      *i2 = dock.icons;

        while (i2 && !done) {
          if (((OD_Icon *) (i2->data))->pic == (Evas_Object *) item->data) {
            icon = (OD_Icon *) i2->data;
            done = true;
          }
          i2 = i2->next;
        }
      }
      item = item->next;
    }
  }
  if (!icon || !(icon->state & OD_ICON_STATE_USEABLE))
    return;

  if (icon->type == application_link) {
    if ((icon->data.applnk.count == 0 && ev->button == 1) || ev->button == 2) {
      // then make one...
      system(icon->data.applnk.command);
    } else if (icon->data.applnk.count > 0 && ev->button == 1) {
      Evas_List      *item = clients;

      while (item) {
        OD_Window      *window = (OD_Window *) item->data;

        if (window->applnk == icon)
          od_wm_activate_window(window->id);
        item = evas_list_next(item);
      }
    }
  } else if (icon->type == minimised_window) {
    if (ev->button == 1) {
      od_wm_activate_window(icon->data.minwin.window);
    }
  }
}
#endif

static void
handle_mouse_move(void *data, Evas * e, Evas_Object * obj, void *event)
{
  Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move *) event;

  if (ev->cur.canvas.y >=
      (options.height - ARROW_SPACE - options.size * dock.zoom) &&
      ev->cur.canvas.y <= options.height && ev->cur.canvas.x > dock.left_end &&
      ev->cur.canvas.x < dock.right_end) {
    dock.x = ev->cur.canvas.x;
    if (dock.state == unzoomed || dock.state == unzooming)
      od_dock_zoom_in();
    need_redraw = true;
  } else if (dock.state == zoomed || dock.state == zooming)
      od_dock_zoom_out();
#ifdef ShapeInput  
    if(options.use_composite && input_shape_full_area && 
      (dock.state == unzoomed || dock.state == unzooming))
         od_window_input_shape_set_extents();
#endif
}

static void
handle_menu_draw(void *data, Evas * e, Evas_Object * obj, void *event)
{
  Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *) event;

#ifdef HAVE_ETK
  if (ev->button == 3)
      od_config_menu_draw(ev->canvas.x, ev->canvas.y);
#endif
}

void
od_window_hide(void) {
  od_window_set_hidden(1);
}

void
od_window_unhide(void) {
  od_window_set_hidden(0);
}

static void
od_window_set_hidden(int hidden) {

  Ecore_X_Display *dsp;
  Screen         *scr;
  int             def;
  int             res_x, res_y;
  int             height;

  if (od_hidden != hidden) {

    od_hidden = hidden;

    /* We really should use edje and just resize / hide the main object which
       would not include the esmart background, but until then... */

    if (options.mode == 1) {
    /* cannot use the nice way below, as it will screw up the esmart bg */
    /* hack to get round broken ecore_x calls */
    // determine the desktop size
    dsp = ecore_x_display_get();
    def = DefaultScreen(dsp);
    scr = ScreenOfDisplay(dsp, def);
    res_x = scr->width;
    res_y = scr->height;

    if (hidden)
      height = 2;
    else
      height = fullheight;
	    
    ecore_x_window_resize(od_window, options.width, height);
    ecore_x_window_move(od_window, (res_x - options.width) / 2, res_y - height);
    } else {
      if (hidden)
        height = 3000;
      else
        height = fullheight;
		
      evas_output_viewport_set(evas, 0, 0 + (fullheight - height),
                               options.width, height);
    }
  }
}
