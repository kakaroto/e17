#include "e.h"
#include "eco_config.h"

/* TODO
 beam_color
 domino
 explode_tasselation
 fire_color
 fire_direction
 fold_direction
 razr direction
 sidecick zoom for centre
 skewer direction
 skewer tassellation
 zoom from center
 */

/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("animation"); 
}

/* Main creation function */
EAPI void
eco_config_animation(void *data)
{
  ECO_PAGE_BEGIN("animation");
 
  ECO_PAGE_TABLE(_("Glide 1"));
  ECO_CREATE_SLIDER_DOUBLE(0, glide1_away_position, "Closeness of window to camera", -2.0, 1.0, "%1.1f", 0, 0);
  ECO_CREATE_SLIDER_DOUBLE(0, glide1_away_angle, "Angle of window at the end of animation", -540, 540, "%1.0f", 0, 1);
  ECO_CREATE_SLIDER_DOUBLE(0, glide1_thickness, "Window thickness", 0, 100, "%1.0f", 0, 2);
  ECO_CREATE_CHECKBOX(0, glide1_zoom_to_taskbar, "Zoom to taskbar on minimize", 0, 3);
  ECO_PAGE_TABLE_END;

  ECO_PAGE_TABLE(_("Glide 2"));
  ECO_CREATE_SLIDER_DOUBLE(0, glide2_away_position, "Closeness of window to camera", -2.0, 1.0, "%1.1f", 0, 0);
  ECO_CREATE_SLIDER_DOUBLE(0, glide2_away_angle, "Angle of window at the end of animation", -540, 540, "%1.0f", 0, 1);
  ECO_CREATE_SLIDER_DOUBLE(0, glide2_thickness, "Window thickness", 0, 100, "%1.0f", 0, 2);
  ECO_CREATE_CHECKBOX(0, glide2_zoom_to_taskbar, "Zoom to taskbar on minimize", 0, 3);
  ECO_PAGE_TABLE_END;
   
  ECO_PAGE_TABLE(_("Magic lamp"));
  ECO_CREATE_SLIDER_INT(0, magic_lamp_grid_res, "Grid resolution (Y dimension only)", 4, 200, "%1.0f", 0, 0);
  ECO_CREATE_SLIDER_INT(0, magic_lamp_max_waves, "Max waves", 3, 20, "%1.0f", 0, 1);
  ECO_CREATE_SLIDER_DOUBLE(0, magic_lamp_amp_min, "Minimum size of the waves", 200.0, 2000.0, "%1.0f", 0, 2);
  ECO_CREATE_SLIDER_DOUBLE(0, magic_lamp_amp_max, "Maximum size of the waves", 200.0, 2000.0, "%1.0f", 0, 3);
  ECO_CREATE_SLIDER_INT(0, magic_lamp_open_start_width, "Open start width", 0, 500, "%1.0f", 0, 4);
  ECO_CREATE_CHECKBOX(0, magic_lamp_moving_end, "Move magic lamp with the mouse pointer", 0, 5);
  ECO_PAGE_TABLE_END;

  ECO_PAGE_TABLE(_("Zoom"));
  ECO_CREATE_SLIDER_DOUBLE(0, zoom_springiness, "Zoom Springiness", 0.0, 1.0, "%1.2f", 0, 0);
  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;
}




