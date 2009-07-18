#include "e.h"
#include "eco_config.h"
#include "eco_actions.h"

/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("ring"); 
}

/* Main creation function */
EAPI void
eco_config_ring(void *data)
{
  ECO_PAGE_BEGIN("ring");
  ECO_PAGE_TABLE("Options");
  
  ECO_CREATE_SLIDER_DOUBLE(0, speed, "Ring speed", 0.1, 5.0, "%1.1f", 0, 0);
  ECO_CREATE_SLIDER_DOUBLE(0, timestep, "Timestep", 0.1, 5.0, "%1.1f", 0, 1);
  ECO_CREATE_SLIDER_INT(0, inactive_opacity, "Inactive window opacity", 1, 100, "%1.0f %%", 0, 2);
  ECO_CREATE_ENTRY(0, window_match, "Showed windows", 0, 3);
  ECO_CREATE_CHECKBOX(0, overlay_icon, "Show emblem icons", 0, 4);
  ECO_CREATE_CHECKBOX(0, darken_back, "Darken background", 0, 5);
  ECO_CREATE_CHECKBOX(0, minimized, "Show minimized", 0, 6);
  ECO_CREATE_CHECKBOX(0, rotate_clockwise, "Rotate clockwise on next", 0, 7);
  ECO_CREATE_CHECKBOX(0, window_title, "Show Window Title", 0, 8);
  ECO_CREATE_CHECKBOX(0, title_font_bold, "Bold Font", 0, 9);
  ECO_CREATE_SLIDER_INT(0, title_font_size, "Font Size", 6, 40, "%1.0f", 0, 10);

  ECO_CREATE_SLIDER_INT(0, ring_width, "Ring Width", 1, 100, "%1.0f %%", 0, 11);
  ECO_CREATE_SLIDER_INT(0, ring_height, "Ring Height", 1, 100, "%1.0f %%", 0, 12);

  ECO_CREATE_SLIDER_INT(0, thumb_width, "Thumb Width", 10, 400, "%1.0f", 0, 13);
  ECO_CREATE_SLIDER_INT(0, thumb_height, "Thumb Height", 10, 400, "%1.0f", 0, 14);

  ECO_CREATE_SLIDER_DOUBLE(0, min_scale, "Minimum Scale Factor", 0.1, 1.0, "%1.1f", 0, 15);
  ECO_CREATE_SLIDER_DOUBLE(0, min_brightness, "Minimum Brightness Factor", 0.1, 1.0, "%1.1f", 0, 16);

  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;
}




