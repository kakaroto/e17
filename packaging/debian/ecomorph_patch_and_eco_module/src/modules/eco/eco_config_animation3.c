#include "e.h"
#include "eco_config.h"


static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("animation"); 
}

/* Main creation function */
EAPI void
eco_config_animation3(void *data)
{
  ECO_PAGE_BEGIN("animation");


  ECO_PAGE_TABLE(_("Wave"));
  ECO_CREATE_SLIDER_DOUBLE(0, wave_width, "Wave width", 0.0, 3.0, "%1.2f", 0, 0);
  ECO_CREATE_SLIDER_DOUBLE(0, wave_amp, "Wave amplitude", 0.0, 1.0, "%1.2f", 0, 1);
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_TABLE(_("Side kick"));
  ECO_CREATE_SLIDER_DOUBLE(0, sidekick_num_rotations, "Number of rotations", 0.0, 5.0, "%1.2f", 0, 0);
  ECO_CREATE_SLIDER_DOUBLE(0, sidekick_springiness, "Sidekick springiness", 0.0, 1.0, "%1.2f", 0, 1);
  ECO_PAGE_TABLE_END;

  ECO_PAGE_TABLE(_("Beam up"));
  ECO_CREATE_SLIDER_DOUBLE(0, beam_size, "Beam width", 0.1, 20.0, "%1.1f", 0, 0);
  ECO_CREATE_SLIDER_INT(0, beam_spacing, "Beam spacing", 1, 20, "%1.0f", 0, 1);
  ECO_CREATE_SLIDER_DOUBLE(0, beam_slowdown, "Beam slowdown", 0.1, 10.0, "%1.1f", 0, 2);
  ECO_CREATE_SLIDER_DOUBLE(0, beam_life, "Beam life", 0.1, 1.0, "%1.1f", 0, 3);
  ECO_PAGE_TABLE_END;

  ECO_PAGE_TABLE(_("Dream"));
  ECO_CREATE_CHECKBOX(0, dream_zoom_to_taskbar, "Zoom to taskbar on minimize", 0, 1);
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_TABLE(_("Explode"));
  ECO_CREATE_SLIDER_DOUBLE(0, explode_thickness, "Thickness of Exploding Polygons", 1.0, 100.0, "%1.0f pixel", 0, 0);
  ECO_CREATE_SLIDER_INT(0, explode_gridx, "Window grid width", 1, 200, "%1.0f", 0, 1);
  ECO_CREATE_SLIDER_INT(0, explode_gridy, "Window grid height", 1, 200, "%1.0f", 0, 2);
  ECO_PAGE_TABLE_END;

  ECO_PAGE_END;
}

