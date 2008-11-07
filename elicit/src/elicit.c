#include "Elicit.h"
#include "math.h"

#include <string.h>

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
  
  if (!ecore_evas_init())
  {
    printf("ERROR: can't initialize Ecore_Evas.\n");
    return 1;
  }

  ecore_app_args_set(argc, (const char **)argv);

  el = calloc(1, sizeof(Elicit));
  if (!el)
  {
    fprintf(stderr, "Elicit: can't create el\n");
    return 1;
  }

  el->app_name = strdup(argv[0]);
  /* create an Ecore_Evas (x11 window with an evas) */
  el->ee = ecore_evas_software_x11_new(0, 0, 0, 0, 255, 255);
  if (!el->ee)
  {
    printf("ERROR: can't create a software X11 Ecore_Evas.\n");
    return 1;
  }
  ecore_evas_title_set(el->ee, "Elicit");
  ecore_evas_name_class_set(el->ee, "Elicit", "Elicit");

  edje_init();
  edje_frametime_set(1.0 / 60.0);

  /* get the evas from the Ecore_Evas) */
  el->evas = ecore_evas_get(el->ee);

  evas_font_path_append(el->evas, DATADIR"/font");

  /* callbacks to exit on window close, destroy or kill requests */
  ecore_evas_callback_delete_request_set(el->ee, elicit_cb_exit);
  ecore_evas_callback_destroy_set(el->ee, elicit_cb_exit);
  ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, elicit_cb_exit, el->ee);

  ecore_evas_callback_mouse_in_set(el->ee, elicit_cb_enter);
  ecore_evas_callback_mouse_out_set(el->ee, elicit_cb_leave);
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
  
  if (el->tmpdir)
  {
    char buf[PATH_MAX];
    snprintf(buf, PATH_MAX, "rm -rf %s", el->tmpdir);
    ecore_exe_run(buf, NULL);
  }
  elicit_config_zoom_set(el->zoom);
  elicit_config_zoom_max_set(el->zoom_max);
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
  char *theme; 

  elicit_config_init(el);

  ecore_evas_borderless_set(el->ee, 1);
  if (ecore_x_screen_is_composited(0))
    ecore_evas_alpha_set(el->ee, 1);
  else
    ecore_evas_shaped_set(el->ee, 1);

  el->gui = edje_object_add(el->evas);
  evas_object_name_set(el->gui, "gui");
  evas_object_move(el->gui, 0, 0);
  evas_object_show(el->gui);

  el->draggie = esmart_draggies_new(el->ee);
  esmart_draggies_button_set(el->draggie, 1);
  evas_object_layer_set(el->draggie, -1);
  evas_object_move(el->draggie, 0, 0);
  evas_object_name_set(el->draggie, "draggie");
  evas_object_show(el->draggie);

  elicit_config_color_get(&el->color.r, &el->color.g, &el->color.b);
  elicit_util_colors_set_from_rgb(el);
  el->zoom = elicit_config_zoom_get();
  el->zoom_max = elicit_config_zoom_max_get();

  /* create the swatch and shot objects */
  el->shot = elicit_zoom_add(el->evas);
  evas_object_name_set(el->shot, "shot");
  evas_object_show(el->shot);
  
  el->swatch = evas_object_rectangle_add(el->evas);
  evas_object_color_set(el->swatch, el->color.r, el->color.g, el->color.b, 255);
  evas_object_name_set(el->swatch, "swatch");
  evas_object_show(el->swatch);

  theme = elicit_config_theme_get(el);
  elicit_ui_theme_set(el, theme, "elicit");
  free(theme);

  elicit_swatches_init(el);
  elicit_shots_init(el);
  elicit_themes_init(el);
  elicit_spectra_init(el);

  elicit_ui_update(el);
  return 0;
}

void
elicit_ui_theme_set(Elicit *el, char *theme, char *group)
{
  Evas_Coord mw, mh;

  /* set the theme */
  if (!edje_object_file_set(el->gui, elicit_theme_find(theme), group))
  {
    printf("Error: can't set theme to %s\n", theme);
    return;
  }
  /* update the config */
  elicit_config_theme_set(theme);

  /* set the default window size */
  edje_object_size_min_get(el->gui, &mw, &mh);
  ecore_evas_size_min_set(el->ee, mw, mh);
  if (mw != 0 && mh != 0)
  {
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
  elicit_zoom_grid_visible_set(el->shot, elicit_config_grid_visible_get());
  elicit_ui_update(el);

  /* set up edje callbacks */
  edje_object_signal_callback_add(el->gui, "elicit,pick,*", "*", elicit_cb_pick, el);
  edje_object_signal_callback_add(el->gui, "mouse,move", "*", elicit_cb_move, el);
  edje_object_signal_callback_add(el->gui, "elicit,ruler,toggle", "ruler", elicit_cb_ruler, el);
  edje_object_signal_callback_add(el->gui, "elicit,shoot,*", "*", elicit_cb_shoot, el);
  edje_object_signal_callback_add(el->gui, "elicit,quit", "*", elicit_cb_exit, el);
  edje_object_signal_callback_add(el->gui, "elicit,color,*", "*", elicit_cb_colors, el);
  edje_object_signal_callback_add(el->gui, "elicit,zoom,*", "*", elicit_cb_colors, el);
  edje_object_signal_callback_add(el->gui, "elicit,switch,*", "*", elicit_cb_switch, el);
  edje_object_signal_callback_add(el->gui, "elicit,copy,*", "*", elicit_cb_copy, el);
  edje_object_signal_callback_add(el->gui, "elicit,resize,*", "*", elicit_cb_resize_sig, el);
  edje_object_signal_callback_add(el->gui, "drag", "*-slider", elicit_cb_slider, el);
  edje_object_signal_callback_add(el->gui, "elicit,freeze", "*", elicit_cb_freeze, el);
  edje_object_signal_callback_add(el->gui, "elicit,thaw", "*", elicit_cb_thaw, el);
  edje_object_signal_callback_add(el->gui, "elicit,size,min,*", "*", elicit_cb_size_min, el);
  edje_object_signal_callback_add(el->gui, "elicit,colorclass", "*", elicit_cb_colorclass, el);
  edje_object_signal_callback_add(el->gui, "elicit,shot,edit", "*", elicit_cb_edit, el);
  edje_object_signal_callback_add(el->gui, "elicit,grid,*", "*", elicit_cb_grid, el);

  edje_object_signal_callback_add(el->gui, "elicit,swatch,save", "*", elicit_swatch_save_cb, el);
  edje_object_signal_callback_add(el->gui, "elicit,shot,save", "*", elicit_shot_save_cb, el);

  edje_object_signal_callback_add(el->gui, "drag", "swatch.scroll.bar", elicit_swatch_scroll_cb, el);
  edje_object_signal_callback_add(el->gui, "elicit,swatch,scroll,*", "*", elicit_swatch_scroll_cb, el);

  edje_object_signal_callback_add(el->gui, "drag", "shot.scroll.bar", elicit_shot_scroll_cb, el);
  edje_object_signal_callback_add(el->gui, "elicit,shot,scroll,*", "*", elicit_shot_scroll_cb, el);

  edje_object_signal_callback_add(el->gui, "drag", "theme.scroll.bar", elicit_theme_scroll_cb, el);
  edje_object_signal_callback_add(el->gui, "elicit,theme,scroll,*", "*", elicit_theme_scroll_cb, el);

  evas_object_hide(el->gui);
  evas_object_show(el->gui);

  /* load up the colorclass */
  {
    int r, g, b;
    elicit_config_colorclass_get(&r, &g, &b);
    edje_object_color_class_set(el->gui, "border", r, g, b, 255, 0, 0, 0, 0, 0, 0, 0, 0);
  }
}

void
elicit_ui_update_text(Elicit *el)
{
  char buf[100];

  snprintf(buf, sizeof(buf)-1, "%d", el->color.r);
  edje_object_part_text_set(el->gui, "red-val", buf); 
  edje_object_part_text_set(el->gui, "red-val2", buf); 
  
  snprintf(buf, sizeof(buf)-1, "%d", el->color.g);
  edje_object_part_text_set(el->gui, "green-val", buf); 
  edje_object_part_text_set(el->gui, "green-val2", buf); 
  
  snprintf(buf, sizeof(buf)-1, "%d", el->color.b);
  edje_object_part_text_set(el->gui, "blue-val", buf); 
  edje_object_part_text_set(el->gui, "blue-val2", buf); 

  snprintf(buf, sizeof(buf)-1, "%.0f", el->color.h);
  edje_object_part_text_set(el->gui, "hue-val", buf); 
  edje_object_part_text_set(el->gui, "hue-val2", buf); 

  snprintf(buf, sizeof(buf)-1, "%.2f", el->color.s);
  edje_object_part_text_set(el->gui, "sat-val", buf); 
  edje_object_part_text_set(el->gui, "sat-val2", buf); 

  snprintf(buf, sizeof(buf)-1, "%.2f", el->color.v);
  edje_object_part_text_set(el->gui, "val-val", buf); 
  edje_object_part_text_set(el->gui, "val2-val", buf); 

  snprintf(buf, sizeof(buf)-1, "%s", el->color.hex);
  edje_object_part_text_set(el->gui, "hex-val", buf); 
  edje_object_part_text_set(el->gui, "hex-val2", buf); 

  snprintf(buf, sizeof(buf)-1, "%d", (int)el->zoom);
  edje_object_part_text_set(el->gui, "zoom-val", buf); 
  edje_object_part_text_set(el->gui, "zoom-val2", buf); 

  /* thaw here to force edje to recalc */
  edje_object_thaw(el->gui);
}

void
elicit_ui_update_sliders(Elicit *el)
{
  double v = 0;

  v = (double)el->color.r / 255;
  edje_object_part_drag_value_set(el->gui, "red-slider", v, v);
  v = (double)el->color.g / 255;
  edje_object_part_drag_value_set(el->gui, "green-slider", v, v);
  v = (double)el->color.b / 255;
  edje_object_part_drag_value_set(el->gui, "blue-slider", v, v);
  v = (double)el->color.h / 360;
  edje_object_part_drag_value_set(el->gui, "hue-slider", v, v);
  v = (double)el->color.s;
  edje_object_part_drag_value_set(el->gui, "sat-slider", v, v);
  v = (double)el->color.v;
  edje_object_part_drag_value_set(el->gui, "val-slider", v, v);
  v = (double)el->zoom / el->zoom_max;
  edje_object_part_drag_value_set(el->gui, "zoom-slider", v, v);

}

void
elicit_ui_update(Elicit *el)
{
  elicit_ui_update_text(el);
  elicit_ui_update_sliders(el);
  elicit_zoom_zoom_set(el->shot, el->zoom);
  elicit_spectra_update(el);
  
  evas_object_color_set(el->swatch, el->color.r, el->color.g, el->color.b, 255);
}
