#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Evas.h>
#include <Esmart/Esmart_Trans.h>

#include "iconbar.h"
#include "util.h"
#include "config.h"
#include "prefs.h"

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
  Evas_List *l = NULL;
  Evas_Object *o = NULL;
  Ecore_Evas *ee = NULL;
  Evas_Coord edjew, edjeh;
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
  
  iconbar_config_init();
  iconbar_config_geometry_get(&x, &y, &w, &h);

  ee = ecore_evas_software_x11_new(NULL, 0, x, y, w, h);
  ecore_evas_callback_mouse_in_set(ee, window_enter);
  ecore_evas_callback_mouse_out_set(ee, window_leave);
  ecore_evas_callback_resize_set(ee, window_resize);
  ecore_evas_callback_move_set(ee, window_resize);
  ecore_evas_callback_delete_request_set(ee, window_delete);
  ecore_evas_callback_destroy_set(ee, window_delete);
  ecore_evas_name_class_set(ee, "Iconbar", "Rephorm");
  ecore_evas_title_set(ee, "Iconbar");
  ecore_evas_borderless_set(ee, iconbar_config_borderless_get());
  ecore_evas_shaped_set(ee, iconbar_config_shaped_get());
  ecore_evas_withdrawn_set(ee, iconbar_config_withdrawn_get());
  ecore_evas_sticky_set(ee, iconbar_config_sticky_get());
  ecore_evas_avoid_damage_set(ee, 1);

  iconbar_config_ecore_evas_set(ee);
#ifdef HAVE_TRANS_BG
  {
    ecore_evas_geometry_get(ee, &x, &y, &w, &h);
    o = esmart_trans_x11_new(ecore_evas_get(ee));
    evas_object_layer_set(o, 0);
    evas_object_move(o, 0, 0);
    evas_object_resize(o, w, h);
    evas_object_name_set(o, "trans");
    
    esmart_trans_x11_freshen(o, x, y, w, h);
    evas_object_show(o);
  }
#endif

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
	    ecore_evas_size_max_set(ee, (int)edjew, (int)edjeh);
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
  ecore_evas_move_resize(ee, x, y, w, h);

  ecore_main_loop_begin();
  
  iconbar_config_free();
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
  if(iconbar_config_raise_lower_get())
    ecore_evas_lower(ee);
  /* FIXME: run through the icons and stop them from pulsing */
}
static void
window_enter(Ecore_Evas *ee)
{
  if(iconbar_config_raise_lower_get())
    ecore_evas_raise(ee);
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
