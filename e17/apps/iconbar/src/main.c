#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Evas.h>
#include <Esmart/Esmart_Trans.h>

#include "iconbar.h"
#include "util.h"
#include "config.h"

static void resize(Ecore_Evas *ee);
static void window_leave(Ecore_Evas *ee);
static int cb_exit(Ecore_Evas *ee);

void write_out_geometry(Ecore_Evas *ee);
void set_default_geom(Ecore_Evas *ee);

Ecore_Evas *ee;
Evas_Object *bg;
Evas_Object *iconbar;

int default_geom_set = 0;
int trans_bg = 0;
int shaped = 1;

int
main()
{
  char buf[2048];

  ecore_init();
  ecore_evas_init();

  ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 50, 100);
  ecore_evas_callback_mouse_out_set(ee, window_leave);
  ecore_evas_callback_resize_set(ee, resize);
  ecore_evas_callback_move_set(ee, resize);


  ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, cb_exit, ee);

  ecore_evas_callback_delete_request_set(ee, cb_exit);
  ecore_evas_callback_destroy_set(ee, cb_exit);
  ecore_evas_name_class_set(ee, "Iconbar", "Rephorm");
  ecore_evas_title_set(ee, "Iconbar");
  ecore_evas_borderless_set(ee, 1);
  ecore_evas_shaped_set(ee, 1);

  evas_font_path_append(ecore_evas_get(ee), "/usr/local/share/elicit/data/font/");

#ifdef HAVE_TRANS_BG
  {
    int x, y, w, h;
    ecore_evas_geometry_get(ee, &x, &y, &w, &h);
    bg = esmart_trans_x11_new(ecore_evas_get(ee));
    evas_object_layer_set(bg, 0);
    evas_object_move(bg, 0, 0);
    evas_object_resize(bg, w, h);
    evas_object_name_set(bg, "trans");
    
    esmart_trans_x11_freshen(bg, x, y, w, h);
    evas_object_show(bg);
  }
#endif

  sprintf(buf, "%s/.e/iconbar", get_user_home());

  iconbar = iconbar_new(ecore_evas_get(ee));
  if (e_file_exists(buf))
  {
    iconbar_path_set(iconbar, buf);
  }

  else
  {
    printf("NOTE: To customize, copy everything from %s to %s\n. Edit build_icons.sh and run it.", PACKAGE_DATA_DIR, buf);
    iconbar_path_set(iconbar, PACKAGE_DATA_DIR);
  }

  evas_object_move(iconbar, 0, 0);
  evas_object_resize(iconbar, 65, 200);
  evas_object_layer_set(iconbar, 10);
  evas_object_show(iconbar);

  ecore_evas_show(ee);
  set_default_geom(ee);


  ecore_main_loop_begin();

  return 0;
}

static void
resize(Ecore_Evas *ee)
{
  int x, y, w, h;
  Evas_Object *o = NULL;
  ecore_evas_geometry_get(ee, &x, &y, &w, &h);

#ifdef HAVE_TRANS_BG
  if((o = evas_object_name_find(ecore_evas_get(ee), "trans")))
    esmart_trans_x11_freshen(o, x, y, w, h);
#endif
  evas_object_resize(iconbar, w, h); 
}

static void
window_leave(Ecore_Evas *ee)
{
  Evas_List *l;
  Iconbar *ib = evas_object_smart_data_get(iconbar);

  /* FIXME: run through the icons and stop them from pulsing */
}

void
write_out_geometry(Ecore_Evas *ee)
{
  FILE *f;
  char buf[PATH_MAX];
  
  snprintf(buf, sizeof(buf), "%s/geometry.txt", iconbar_path_get(iconbar));
  
  f = fopen(buf, "w");
  
  if (f)
  {
    char buf[100];
    int x, y, w, h;

//    printf("writing...\n");

    ecore_evas_geometry_get(ee, &x, &y, &w, &h);

    sprintf(buf, "%d\n", x);
    fputs(buf, f);
    sprintf(buf, "%d\n", y);
    fputs(buf, f);
    sprintf(buf, "%d\n", w);
    fputs(buf, f);
    sprintf(buf, "%d\n", h);
    fputs(buf, f);
    fclose(f);
  }
}


void
set_default_geom(Ecore_Evas *ee)
{
  FILE *f;
  char buf[PATH_MAX];
  int x = 0, y = 0, w = 66, h = 400;
  
//  printf("setting default geom\n");  
  snprintf(buf, sizeof(buf), "%s/geometry.txt", iconbar_path_get(iconbar));
  
  f = fopen(buf, "r");
  
  if (f)
  {
    if(fgets(buf, sizeof(buf) - 1, f))
	x = atoi(buf);
    if(fgets(buf, sizeof(buf) - 1, f))
	y = atoi(buf);
    if(fgets(buf, sizeof(buf) - 1, f))
	w = atoi(buf);
    if(fgets(buf, sizeof(buf) - 1, f))
	h = atoi(buf);

//    printf("(%d, %d) %d x %d\n", x, y, w, h);
    fclose(f);
  }
  else 
  {
    printf("no geometry file. starting at default size. \n");
  }
  ecore_evas_move(ee, x, y);
  ecore_evas_resize(ee, w, h);
  default_geom_set = 1;
}

static int
cb_exit(Ecore_Evas *ee)
{
  write_out_geometry(ee);
  ecore_main_loop_quit();
  return 0;
}
