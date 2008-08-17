#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Evas.h>
#include <Esmart/Esmart_Trans_X11.h>
#include <limits.h>

#include "iconbar.h"
#include "util.h"
#include "config.h"
#include "prefs.h"
/*#include "icon_editor.h" */

static void window_resize(Ecore_Evas *ee);
static void window_leave(Ecore_Evas *ee);
static void window_enter(Ecore_Evas *ee);
static void window_delete(Ecore_Evas *ee);
static int cb_exit(void *data, int type, void *event);

int
main(int argc, char **argv)
{
  int x, y, w, h;
  char buf[2048];
  Ecore_X_Window win;
  Evas_List *l = NULL;
  Evas_Object *o = NULL;
  Evas_Object *trans = NULL;
  Ecore_Evas *ee = NULL;
  const char *str = NULL;
  Evas_Coord edjew = 0, edjeh = 0;
  Evas_Object *iconbar = NULL;

  if(!ecore_init())
  {
    fprintf(stderr, "Unable to init Ecore, bailing out.\n");
    return(-1);
  }
  ecore_app_args_set(argc, (const char**)argv);

  ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, cb_exit, ee);

  ecore_evas_init();
  edje_init();
  edje_frametime_set(1.0/30.0); 
  iconbar_config_init();
  iconbar_config_geometry_get(&x, &y, &w, &h);
/*  icon_editor_init(&argc, argv); */

  ee = ecore_evas_software_x11_new(NULL, 0, x, y, w, h);
  win = ecore_evas_software_x11_window_get(ee);
  ecore_evas_callback_mouse_in_set(ee, window_enter);
  ecore_evas_callback_mouse_out_set(ee, window_leave);
  ecore_evas_callback_resize_set(ee, window_resize);
  ecore_evas_callback_move_set(ee, window_resize);
  ecore_evas_callback_delete_request_set(ee, window_delete);
  ecore_evas_callback_destroy_set(ee, window_delete);
  ecore_evas_name_class_set(ee, "Iconbar", "Iconbar");
  ecore_evas_title_set(ee, "Iconbar");
  ecore_evas_borderless_set(ee, iconbar_config_borderless_get());
  ecore_evas_withdrawn_set(ee, iconbar_config_withdrawn_get());
  ecore_evas_sticky_set(ee, iconbar_config_sticky_get());
  ecore_evas_avoid_damage_set(ee, 1);
  ecore_evas_move(ee, x, y);
  //ecore_x_window_prop_layer_set(win, 1);

  iconbar_config_ecore_evas_set(ee);

  for(l = iconbar_config_font_path_get(); l; l = l->next)
  {
    evas_font_path_append(ecore_evas_get(ee), (void*)l->data);
    free(l->data);
    l->data = NULL;
  }
  evas_font_path_append(ecore_evas_get(ee), PACKAGE_DATA_DIR);

  iconbar = iconbar_new(ecore_evas_get(ee));
  snprintf(buf, PATH_MAX, "%s/.e/iconbar/", getenv("HOME"));
  if (e_file_exists(buf))
  {
    iconbar_path_set(iconbar, buf);
    if((o = iconbar_gui_get(iconbar)))
    {
	edje_object_size_min_get(o, &edjew, &edjeh);
	if((edjew > 0) && (edjeh > 0))
	    ecore_evas_size_min_set(ee, (int)edjew, (int)edjeh);
	
	edje_object_size_max_get(o, &edjew, &edjeh);

	if((edjew > 0) && (edjeh > 0))
	{
	    if (edjew > INT_MAX) edjew = INT_MAX;
	    if (edjeh > INT_MAX) edjeh = INT_MAX;
	    ecore_evas_size_max_set(ee, (int)edjew, (int)edjeh);
	}
	if(iconbar_config_sticky_get() > 0)
	    edje_object_signal_emit(o, "window,sticky,on", "");
	else
	    edje_object_signal_emit(o, "window,sticky,off", "");
	if(iconbar_config_withdrawn_get() > 0)
	    edje_object_signal_emit(o, "window,withdrawn,on", "");
	else
	    edje_object_signal_emit(o, "window,withdrawn,off", "");
	if(iconbar_config_borderless_get() > 0)
	    edje_object_signal_emit(o, "window,borderless,on", "");
	else
	    edje_object_signal_emit(o, "window,borderless,off", "");
	if((str = edje_object_data_get(o, "iconbar,window")))
	{
#ifdef HAVE_TRANS_BG
	    if(!strcmp(str, "trans"))
	    {
		ecore_evas_geometry_get(ee, &x, &y, &w, &h);
		trans = esmart_trans_x11_new(ecore_evas_get(ee));
		evas_object_layer_set(trans, 0);
		evas_object_move(trans, 0, 0);
		evas_object_resize(trans, w, h);
		evas_object_name_set(trans, "trans");
    
		esmart_trans_x11_freshen(trans, x, y, w, h);
		evas_object_show(trans);
	    }
#else 
	    if(!strcmp(str, "trans"))
	    {
		fprintf(stderr,"iconbar compiled without trans support\n");
	    }
#endif
	    else if(!strcmp(str, "shaped"))
	    {
		ecore_evas_shaped_set(ee, 1);
	    }
	}
	else
	{
		ecore_evas_shaped_set(ee, 1);
	}
    }
  }

  else
  {
    printf("NOTE: To customize, copy everything from %s to %s\n. Edit build_icons.sh and run it.", PACKAGE_DATA_DIR, buf);
    iconbar_path_set(iconbar, PACKAGE_DATA_DIR);
  }

  evas_object_move(iconbar, 0, 0);
  evas_object_resize(iconbar, w, h);
  evas_object_layer_set(iconbar, 10);
  evas_object_name_set(iconbar, "iconbar");
  evas_object_show(iconbar);
  
  ecore_evas_show(ee);

  ecore_main_loop_begin();
  
  iconbar_config_free();
  evas_object_del(iconbar);
  edje_shutdown();
  ecore_evas_shutdown();
  ecore_shutdown();

  return 0;
}
/*==========================================================================
 * Callbacks from here on down 
 *========================================================================*/
static void
window_resize(Ecore_Evas *ee)
{
  int x, y, w, h;
  Evas_Object *o = NULL;
  
  ecore_evas_geometry_get(ee, &x, &y, &w, &h);
  iconbar_config_geometry_set(x, y, w, h);
#ifdef HAVE_TRANS_BG
  if((o = evas_object_name_find(ecore_evas_get(ee), "trans")))
  {
    evas_object_resize(o, w, h);
    esmart_trans_x11_freshen(o, x, y, w, h);
  }
#endif
  if((o = evas_object_name_find(ecore_evas_get(ee), "iconbar")))
  {
    evas_object_resize(o, w, h);
  }
}
static void
window_leave(Ecore_Evas *ee)
{
  static double last = 0.0;
  Evas_Object *o = NULL, *edje = NULL;
  Evas_List *l = NULL;
  
  if((o = evas_object_name_find(ecore_evas_get(ee), "iconbar")))
  {
    Iconbar *ib = evas_object_smart_data_get(o);
    if((edje = iconbar_gui_get(o)))
    {
	if(ecore_time_get() - last > 0.05)
	{
	    edje_object_signal_emit(edje, "window,leave", "");	
	    last = ecore_time_get();

            /* tell the icons too */
	    for (l = esmart_container_elements_get(ib->cont); l; l = l->next)
	    {
	        Evas_Object *obj = l->data;
	        edje_object_signal_emit(obj, "window,leave", "");	
	    }
        }
    }
  }
}
static void
window_enter(Ecore_Evas *ee)
{
  static double last = 0.0;
  Evas_Object *o = NULL, *edje = NULL;
  Evas_List *l = NULL;

  if((o = evas_object_name_find(ecore_evas_get(ee), "iconbar")))
  {
    Iconbar *ib = evas_object_smart_data_get(o);
    if((edje = iconbar_gui_get(o)))
    {
	if(ecore_time_get() - last > 0.05)
	{
	    edje_object_signal_emit(edje, "window,enter", "");	
	    last = ecore_time_get();

            /* tell the icons too */
	    for (l = esmart_container_elements_get(ib->cont); l; l = l->next)
	    {
	        Evas_Object *obj = l->data;
	        edje_object_signal_emit(obj, "window,enter", "");	
	    }
	}
    }
  }
}
static void
window_delete(Ecore_Evas *ee)
{
    ecore_main_loop_quit();
}
static int
cb_exit(void *data, int type, void *event)
{
  ecore_main_loop_quit();
  return 0;
}
