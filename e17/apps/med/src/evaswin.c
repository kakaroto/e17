
#include <stdlib.h>
#include <string.h>
#include "evaswin.h"
#include "config.h"
#include "menu.h"
#include "util.h"

#define FONT "nationff"

void idle_cb(void* data);
void
expose_cb(Ecore_Event* ev);
static void
evaswin_mouse_down_cb(Ecore_Event* ev);
static void
evaswin_mouse_up_cb(Ecore_Event* ev);
static void
evaswin_mouse_move_cb(Ecore_Event* ev);
void
winobjects(void);
void
med_tool_add_event_box( E_Menu_Item *mi );
void
med_add_tool( char *text, int x, int y, med_tool_type tt );
void
med_commit_tool_mouse_down(void *_data, Evas * _e, Evas_Object * _o, void *event);
static void
med_add_commit_tool( int x, int y);
static void
med_animate_cb(int val, void* data );
static void
med_commit_dirty_anim(void);


/* in menu.c */
E_Menu_Item *
med_get_selected_mi(void);
void
med_set_menu_event_evas(Evas * parent_evas);


static Evas * e;
static int win_w;
static int win_h;
static char *fntdir;
static char *wintitle;
static Display *disp;
static Evas * e;
static Window win, drag_win;

static Evas_Object *o, *onew, *o_commit, *o_discard;
static const int col = 0;
static int scr_x, scr_y;
static int drag_click = 0, dragging = 0;
static int anim_active = 0;
static double anim_t = 0.02;

static E_Menu* mni = 0;

void
evaswin_new( int w, int h, const char *title )
{
  win_w = w;
  win_h = h;
  /* todo */
  fntdir = strdup(e_config_get("fonts"));
  /*  fntdir = strdup(fd);*/
  /*  const char *fd= "/usr/local/share/enlightenment/data/fonts"*/
  wintitle = title ? strdup(title) : "New Win";

  ecore_display_init(NULL);
  disp = ecore_display_get();

  e = e_evas_new_all(disp,
		   DefaultRootWindow(disp),
		   0, 0, win_w, win_h,
		   0 /*RENDER_METHOD_ALPHA_SOFTWARE*/,
		   216,  /* colors - evas standard 216 */
		   1024 * 1024,    /* font cache size */
		   512 * 1024,  /* image cache size */
		   fntdir);  /* font dir */

  win = e_evas_get_window(e);
  ecore_window_set_title(win, wintitle);

  ecore_window_set_events(win, 
		      /*XEV_BUTTON_RELEASE*/ XEV_BUTTON 
		      | XEV_EXPOSE
		      | XEV_MOUSE_MOVE);

  ecore_window_show(win);

  med_set_parent( win );

  winobjects();

  ecore_event_filter_init();
  /* window event translation init */
  ecore_event_x_init();

  ecore_event_filter_idle_handler_add(idle_cb, NULL);
  ecore_event_filter_handler_add(ECORE_EVENT_WINDOW_EXPOSE, expose_cb);
  ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_DOWN, evaswin_mouse_down_cb);
  ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_UP, evaswin_mouse_up_cb);
  ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_MOVE, evaswin_mouse_move_cb);



  o = evas_object_rectangle_add(e);
  /*evas_move(e, o, 0, 0);*/
  evas_object_resize(o, win_w, win_h);
  evas_object_color_set(o, col, col, col, 255);
  evas_object_layer_set(o, 98);
  evas_object_show(o);

  ecore_window_get_geometry(win, &scr_x, &scr_y, NULL, NULL);
 
}


void
evaswin_free(void)
{
  free( fntdir);
  free( wintitle);
  evas_free(e);

  /* clean up */
  XCloseDisplay(disp);
}


void
winobjects(void)
{
  Evas_Object * ot;

#if 0
  ot = evas_add_text(e, FONT, 20, "New");
  evas_set_color(e, ot, 255, 255, 255, 100);
  /*  iw = evas_get_text_width(e, ot);*/
  /*  ih = evas_get_text_height(e, ot);*/
  /*  evas_text_get_ascent_descent(e, ot, &d_as, &d_de);*/
  /*printf( "as %f de %f\n", d_as, d_de );*/
  /*  evas_move(e, ot, 0, win_h-d_as );*/
  /*evas_move(e, ot, 50, 200 );*/
  evas_move(e, ot, 200, 200 );
  evas_set_layer(e, ot, 110);
  evas_show(e, ot);
  /*  evas_callback_add(e, ot, CALLBACK_MOUSE_IN, mouse_in, NULL);*/
  /*  evas_callback_add(e, ot, CALLBACK_MOUSE_OUT, mouse_out, NULL);*/
  evas_callback_add(e, ot, CALLBACK_MOUSE_UP, mouse_up, NULL);
  evas_callback_add(e, ot, CALLBACK_MOUSE_DOWN, mouse_down, NULL);
  evas_callback_add(e, ot, CALLBACK_MOUSE_MOVE, mouse_move, NULL);
  onew = ot;
#endif

  /*  ot = evas_add_rectangle(e);*/

  med_set_menu_event_evas(e);

  med_add_tool( "New item", 200, 200+40, TOOL_NEW_ITEM );

  med_add_tool( "New submenu", 200, 200+40+30, TOOL_NEW_SUBMENU );

  med_add_tool( "new sep", 200, 200+40+60, TOOL_NEW_SEP);

  med_add_commit_tool( 340, 240);
}


void
idle_cb(void* data)
{
  evas_render(e);
}


void
expose_cb(Ecore_Event* ev)
{
  Ecore_Event_Window_Expose *event = (Ecore_Event_Window_Expose *) ev->event;

  /* area exposes */
  evas_damage_rectangle_add(e,
		   event->x, event->y,
		   event->w, event->h
		   );
}


static void
evaswin_mouse_down_cb(Ecore_Event* ev)
{
  Ecore_Event_Mouse_Down *event = (Ecore_Event_Mouse_Down *) ev->event;

  evas_event_feed_mouse_down(e, event->button);
}


static void
evaswin_mouse_up_cb(Ecore_Event* ev)
{
  Ecore_Event_Mouse_Up *event = (Ecore_Event_Mouse_Up *) ev->event;

  evas_event_feed_mouse_up(e, event->button);
}


static void
evaswin_mouse_move_cb(Ecore_Event* ev)
{
  Ecore_Event_Mouse_Move *event = (Ecore_Event_Mouse_Move *) ev->event;

  /* Evas2, abs coords... */
  evas_event_feed_mouse_move(e, event->x, event->y);
}


Evas *
evaswin_get_e(void)
{
  return e;
}


Display *
evaswin_get_disp(void)
{
  return disp;
}


Window
evaswin_get_win(void)
{
  return win;
}


void
med_tool_mouse_up(void *_data, Evas * _e, Evas_Object * _o, void *_event)
{
  E_Menu *m;
  E_Menu_Item *mi_in = _data;
  Evas_Event_Mouse_Up *ev = _event;

  if(mi_in) m = mi_in->menu;

  if( m && dragging )
    {
      E_Menu_Item *mi;
      
      ecore_window_destroy(drag_win);
      dragging = 0;

      /* get selection, edit menu... */
      mi = med_get_selected_mi();

      /* Did the drag start from a insertion tool? */
      if( m->edit_tool )
	{
	  if(mi) med_insert_mi_at(mi, m->edit_tool);
	}
      /* No, drag from within the menu we are editing. */
      else if( mi )
	{
	  /* Dropped on menu */
	  med_move_mi(mi_in, mi);
	}
      else
	{
	  /* Dropped outside menu */
	  if(mi_in) med_delete_mi_at(mi_in);
	}
    }
  return;
  UN(ev);
}


void
med_tool_mouse_down(void *_data, Evas * _e, Evas_Object * _o, void *_event)
{
  E_Menu *m;
  E_Menu_Item *mi;
  int df = 0;
  Evas_Event_Mouse_Down *ev = _event;

  mi = _data;
  if(mi) m = mi->menu;

  if( m /*&& m->edit_tool*/ )
    {
      Evas_List * l;
      
      drag_click = 1;
#if 0
	for (l = m->entries; l; l = l->next)
	  {
	     E_Menu_Item *mi;
	     
	     mi = l->data;
	     if( mi->obj_entry == _o )
	       df = 1;
	     printf( "Click..%s\n", df ? "drag" : "" );
	     df = 0;
	  }
#endif
    }
  return;
  UN(ev);
}



void
med_tool_mouse_move(void *_data, Evas * _e, Evas_Object * _o, void *_event)
{
  E_Menu *m = 0;
  E_Menu_Item *mi = _data;
  Evas_Event_Mouse_Move *ev = _event;

  if(mi) m = mi->menu;

  if( m /*&& m->edit_tool*/ )
    {
      if( drag_click == 1)
	{
	  Pixmap pmap /*, mask*/;
	  /*int rx,ry;*/

	  dragging = 1;
	  drag_click = 0;

	  /* create drag object */

	  /*printf( "scr %d,%d ev: %d,%d r: %d,%d\n", scr_x, scr_y, _x, _y);*/

	     drag_win = ecore_window_override_new(0, 
					      /*_x - 20, _y - 10, 
					      100, 100
					      */
					      ev->cur.canvas.x - 15 /*(m->current.w)/2*/,
					      ev->cur.canvas.y - (m->current.h)/2,
					      /*
					      m->current.w,
					      m->current.h
					      */
					      mi->size.w + m->sel_border.l + m->sel_border.r 
					      + m->border.l + m->border.r,
					      mi->size.h  + m->sel_border.t + m->sel_border.b
					      + m->border.t + m->border.b

					      );

	     med_drag_image( drag_win, m, mi);

	} 
      if( dragging )
	{
	  int win_ax, win_ay;
	  
	  ecore_window_get_root_relative_location(e_evas_get_window(_e), 
						  &win_ax, &win_ay
						  );
	  
	  /* move this to idle handler - update based on drag flag */
	  ecore_window_move(drag_win, 
			    ev->cur.canvas.x - 15 + win_ax,
			    ev->cur.canvas.y + win_ay - 
			    (mi->size.h  + m->sel_border.t + m->sel_border.b
			     + m->border.t + m->border.b) /2
			    );
	}
    }
  return;
  UN(ev);
}


void
med_tool_add_event_box( E_Menu_Item *mi )
{
  Evas_Object * o;
#if 0
  o = evas_add_rectangle(e);
  evas_move(e, o, mi->menu->current.x, mi->menu->current.y );
  /*evas_resize(e, o, win_w, win_h);*/
  evas_set_color(e, o, 255, 255, 255, 0);
  evas_set_layer(e, o, 100);
  evas_show(e, o);

  evas_callback_add(e, o, CALLBACK_MOUSE_UP, med_tool_mouse_up, mi);
  evas_callback_add(e, o, CALLBACK_MOUSE_DOWN, med_tool_mouse_down, mi);
  evas_callback_add(e, o, CALLBACK_MOUSE_MOVE, med_tool_mouse_move, mi);

  mi->menu->event_evas = e;
  mi->event_rect = o;
#endif
}


void
med_add_tool( char *text, int x, int y, med_tool_type tt )
{
   E_Menu *menu, *sub;
   E_Menu_Item *menuitem, *subi;

   /*
     TOOL_NEW_ITEM,
     TOOL_NEW_SUBMENU
   */

   menu = e_menu_new();

   menu->always_visible = 1;

   e_menu_set_padding_icon(menu, 2);
   e_menu_set_padding_state(menu, 2);

   menuitem = e_menu_item_new(text);

   menu->edit_tool = tt;

   switch( tt )
     {
     case TOOL_NEW_ITEM:
       break;
     case TOOL_NEW_SUBMENU:
       sub = e_menu_new();
       e_menu_set_padding_icon(sub, 2);
       e_menu_set_padding_state(sub, 2);
       subi = e_menu_item_new("Empty");
       e_menu_add_item(sub, subi);

       e_menu_item_set_submenu(menuitem, sub /*menu2*/);
       break;
     case TOOL_NEW_SEP:
       e_menu_item_set_separator(menuitem, 1);
       menuitem->size.min.w = 70;
       /*menuitem->size.min.h = 15;*/
       break;
     }

   e_menu_add_item(menu, menuitem);

   med_tool_add_event_box( menuitem );

   e_menu_show_at_mouse(menu, x, y, 0);

   /*printf( "%d,%d,%d\n", menu->current.x, menu->current.y, menu->current.w);*/
}


void
med_commit_tool_mouse_down(void *_data, Evas * _e, Evas_Object * _o, void *event)
{
  if(_o == o_commit)
    {
      med_commit_dirty_entries();
    }
  else if(_o == o_discard)
    {
      med_discard_dirty_entries();
    }
  return;
  UN(event);
}


static void
med_add_commit_tool( int x, int y)
{
  Evas_Object * o;

  o = evas_object_rectangle_add(e);
  evas_object_move(o, x, y );
  evas_object_resize(o, 20, 12);
  evas_object_color_set(o, 50, 255, 0, 255);
  evas_object_layer_set(o, 100);
  /*  evas_show(e, o);*/
  evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, med_commit_tool_mouse_down, NULL);
#if 0
  evas_callback_add(e, o, CALLBACK_MOUSE_UP, med_tool_mouse_up, mi);
  evas_callback_add(e, o, CALLBACK_MOUSE_MOVE, med_tool_mouse_move, mi);
#endif

  o_commit = o;


  o = evas_object_rectangle_add(e);
  evas_object_move(o, x+30, y );
  evas_object_resize(o, 20, 12);
  evas_object_color_set(o, 255, 50, 0, 255);
  evas_object_layer_set(o, 100);
  /*  evas_show(e, o);*/
  evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, med_commit_tool_mouse_down, NULL);
#if 0
  evas_callback_add(e, o, CALLBACK_MOUSE_UP, med_tool_mouse_up, mi);
  evas_callback_add(e, o, CALLBACK_MOUSE_MOVE, med_tool_mouse_move, mi);
#endif

  o_discard = o;

  ecore_add_event_timer("anim", anim_t, med_animate_cb, 1, NULL);
}


void
med_show_commit_tool(void)
{
  evas_object_show(o_commit);
  evas_object_show(o_discard);
}


void
med_hide_commit_tool(void)
{
  evas_object_hide(o_commit);
  evas_object_hide(o_discard);
}


void
med_indicate_entries_dirty(void)
{
  if(!anim_active)
    {
      anim_active++;
      /*med_commit_dirty_anim();*/
    }
}


static void
med_animate_cb(int val, void* data )
{
  if(anim_active)
    {
      med_commit_dirty_anim();
    }
  ecore_add_event_timer("anim", anim_t, med_animate_cb, 1, NULL);
  /*printf("anim %d\n", anim_active);*/
}


static void
med_commit_dirty_anim(void)
{
  if(anim_active == 10 || anim_active == 30)
    med_hide_commit_tool();
  else if(anim_active == 20 || anim_active == 40)
    med_show_commit_tool();

  anim_active++;

  if(anim_active == 70)
    anim_active = 0;
}


/*eof*/
