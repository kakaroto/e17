#include "e.h"
#include "eco_config.h"


static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("animation"); 
}

/* Main creation function */
EAPI void
eco_config_animation4(void *data)
{
  ECO_PAGE_BEGIN("animation");

  
  ECO_PAGE_TABLE(_("Skewer"));
  ECO_CREATE_SLIDER_INT(0, skewer_gridx, "Window grid width", 1, 100, "%1.0f", 0, 0);
  ECO_CREATE_SLIDER_INT(0, skewer_gridy, "Window grid height", 1, 100, "%1.0f", 0, 1);
  ECO_CREATE_SLIDER_DOUBLE(0, skewer_thickness, "Thickness of animated polygons", 1.0, 100.0, "%1.0f", 0, 2);
  ECO_CREATE_SLIDER_INT(0, skewer_rotation, "Rotation angle", -720, 720, "%1.0f", 0, 3);
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_TABLE(_("Vacuum"));
  ECO_CREATE_SLIDER_INT(0, vacuum_grid_res, "Grid resolution (Y dimension only)", 4, 200, "%1.0f", 0, 0);
  ECO_CREATE_SLIDER_INT(0, vacuum_open_start_width, "Vacuum open start width", 0, 500, "%1.0f", 0, 1);
  ECO_CREATE_CHECKBOX(0, vacuum_moving_end, "Move vacuum ending point with the mouse pointer", 0, 2);
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_TABLE(_("Airplane"));
  ECO_CREATE_SLIDER_DOUBLE(0, airplane_path_length, "Flying path length", 0.2, 3.0, "%1.1f", 0, 0);
  ECO_CREATE_CHECKBOX(0, airplane_fly_to_taskbar, "Fly to taskbar on minimize", 0, 1);
  ECO_PAGE_TABLE_END;  

  ECO_PAGE_TABLE(_("Fire"));
  ECO_CREATE_SLIDER_INT(0, fire_particles, "Number of fire particles", 1, 10000, "%1.0f", 0, 0);
  ECO_CREATE_SLIDER_DOUBLE(0, fire_size, "Particle size", 0.1, 20.0, "%1.1f", 0, 1);
  ECO_CREATE_SLIDER_DOUBLE(0, fire_slowdown, "Particle slowdown", 0.1, 10.0, "%1.1f", 0, 2);
  ECO_CREATE_SLIDER_DOUBLE(0, fire_life, "Particle life", 0.1, 1.0, "%1.1f", 0, 3);
  ECO_CREATE_CHECKBOX(0, fire_constant_speed, "Constant speed", 0, 4);
  ECO_CREATE_CHECKBOX(0, fire_smoke, "Enable smoke", 0, 5);
  ECO_CREATE_CHECKBOX(0, fire_mystical, "Mystical Fire (random colors)", 0, 6);
  ECO_PAGE_TABLE_END;
  
  
  ECO_PAGE_END;
}
