#include "Elicit.h"
#include "math.h"

/* functions */
int setup(int argc, char **argv, Elicit *el);
void pick_color();
void shoot();

/* variables */

int
main (int argc, char **argv)
{
  Elicit *el;

  /* initialize the libraries */
  if (!ecore_init())
  {
    printf("ERROR: can't initialize Ecore.\n");
    return 1;
  }
  ecore_app_args_set(argc, (const char **)argv);
  
  if (!ecore_evas_init())
  {
    printf("ERROR: can't initialize Ecore_Evas.\n");
    return 1;
  }

  el = calloc(1, sizeof(Elicit));
  if (!el)
  {
    fprintf(stderr, "Elicit: can't create el\n");
    return 1;
  }

  /* create an Ecore_Evas (x11 window with an evas) */
  el->ee = ecore_evas_software_x11_new(0, 0, 0, 0, 255, 255);
  if (!el->ee)
  {
    printf("ERROR: can't create a software X11 Ecore_Evas.\n");
    return 1;
  }

  edje_init();
  edje_frametime_set(1.0 / 60.0);


  /* get the evas from the Ecore_Evas) */
  el->evas = ecore_evas_get(el->ee);

  evas_font_path_append(el->evas, DATADIR"/font");

  /* callbacks to exit on window close, destroy or kill requests */
  ecore_evas_callback_delete_request_set(el->ee, elicit_cb_exit);
  ecore_evas_callback_destroy_set(el->ee, elicit_cb_exit);
  ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, elicit_cb_exit, el->ee);

  ecore_evas_callback_resize_set(el->ee, elicit_cb_resize);
       
  /* do everything app specific in here */
  if (setup(argc, argv, el))
  {
    printf("ERROR: setting up application failed.\n");
    return 1;
  }
  
  ecore_evas_show(el->ee);

  /* start the event loop */
  ecore_main_loop_begin();

  /* shutdown the subsystems (when event loop exits, app is done) */
  elicit_config_zoom_set(el->zoom);
  elicit_config_color_set(el->color.r, el->color.g, el->color.b);
  elicit_config_shutdown(el);
  ecore_evas_shutdown();
  ecore_shutdown();
  edje_shutdown();

  return 0;
}


/* all app specific setup goes here  *
 * return 0 if everything goes ok.   *
 * return 1 if something goes wrong. */
int
setup(int argc, char **argv, Elicit *el)
{
  elicit_config_init(el);

  ecore_evas_borderless_set(el->ee, 1);
  ecore_evas_shaped_set(el->ee, 1);

  el->gui = edje_object_add(el->evas);
  evas_object_name_set(el->gui, "gui");
  evas_object_move(el->gui, 0, 0);
  evas_object_show(el->gui);

  elicit_config_color_get(&el->color.r, &el->color.g, &el->color.b);
  elicit_util_colors_set_from_rgb(el);
  el->zoom = elicit_config_zoom_get();

  /* create the swatch and shot objects */
  el->shot = evas_object_image_add(el->evas);
  evas_object_name_set(el->shot, "shot");
  evas_object_show(el->shot);
  
  el->swatch = evas_object_rectangle_add(el->evas);
  evas_object_color_set(el->swatch, el->color.r, el->color.g, el->color.b, 255);
  evas_object_name_set(el->swatch, "swatch");
  evas_object_show(el->swatch);

  elicit_ui_theme_set(el, elicit_config_theme_get(el), "elicit");
  elicit_ui_update_text(el);
  return 0;
}

void
elicit_ui_theme_set(Elicit *el, char *theme, char *group)
{
  double mw, mh;

  /* set the theme */
  if (!edje_object_file_set(el->gui, elicit_theme_find(theme), group))
  {
    printf("Error: can't set theme to %s\n", theme);
    return;
  }

  printf("theme set\n");

  /* set the default window size */
  edje_object_size_min_get(el->gui, &mw, &mh);
  ecore_evas_size_min_set(el->ee, mw, mh);
  if (mw != 0 && mh != 0)
  {
    printf("resize to: %.2f x %.2f\n", mw, mh);
    evas_object_resize(el->gui, mw, mh);
    ecore_evas_resize(el->ee, mw, mh);
  }
  else
  {
    /* arbitrary default size if theme doesn't set a min size */
    evas_object_resize(el->gui, 255, 255);
    ecore_evas_resize(el->ee, 255, 255);
  }

  /* swallow and update */ 
  edje_object_part_swallow(el->gui, "shot", el->shot);
  edje_object_part_swallow(el->gui, "swatch", el->swatch);
  elicit_ui_update_text(el);

  /* set up edje callbacks */
  edje_object_signal_callback_add(el->gui, "elicit,pick,*", "*", elicit_cb_pick, el);
  edje_object_signal_callback_add(el->gui, "mouse,move", "*", elicit_cb_pick, el);
  edje_object_signal_callback_add(el->gui, "mouse,move", "*", elicit_cb_shoot, el);
  edje_object_signal_callback_add(el->gui, "elicit,shoot,*", "*", elicit_cb_shoot, el);
  edje_object_signal_callback_add(el->gui, "elicit,quit", "*", elicit_cb_exit, el);
  edje_object_signal_callback_add(el->gui, "elicit,color,*", "*", elicit_cb_colors, el);
  edje_object_signal_callback_add(el->gui, "elicit,zoom,*", "*", elicit_cb_colors, el);
  edje_object_signal_callback_add(el->gui, "elicit,switch,*", "*", elicit_cb_switch, el);
  edje_object_signal_callback_add(el->gui, "elicit,copy,*", "*", elicit_cb_copy, el);
  edje_object_signal_callback_add(el->gui, "elicit,resize,*", "*", elicit_cb_resize_sig, el);

  evas_object_hide(el->gui);
  evas_object_show(el->gui);
}

void
elicit_ui_update_text(Elicit *el)
{
  char buf[100];
 
  snprintf(buf, sizeof(buf)-1, "%d", el->color.r);
  edje_object_part_text_set(el->gui, "red-val", buf); 
  
  snprintf(buf, sizeof(buf)-1, "%d", el->color.g);
  edje_object_part_text_set(el->gui, "green-val", buf); 
  
  snprintf(buf, sizeof(buf)-1, "%d", el->color.b);
  edje_object_part_text_set(el->gui, "blue-val", buf); 

  snprintf(buf, sizeof(buf)-1, "%.0f", el->color.h);
  edje_object_part_text_set(el->gui, "hue-val", buf); 

  snprintf(buf, sizeof(buf)-1, "%.2f", el->color.s);
  edje_object_part_text_set(el->gui, "sat-val", buf); 

  snprintf(buf, sizeof(buf)-1, "%.2f", el->color.v);
  edje_object_part_text_set(el->gui, "val-val", buf); 

  snprintf(buf, sizeof(buf)-1, "%s", el->color.hex);
  edje_object_part_text_set(el->gui, "hex-val", buf); 

  snprintf(buf, sizeof(buf)-1, "%.2f", el->zoom);
  edje_object_part_text_set(el->gui, "zoom-val", buf); 

  /* thaw here to force edje to recalc */
  edje_object_thaw(el->gui);
}

