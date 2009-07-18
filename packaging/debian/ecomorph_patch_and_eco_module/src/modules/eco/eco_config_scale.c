#include "e.h"
#include "eco_config.h"
#include "eco_actions.h"

/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
   eco_config_group_apply("scale");  
}

/* Main creation function */
EAPI void
eco_config_scale(void *data)
{
  ECO_PAGE_BEGIN("scale");
  ECO_PAGE_TABLE("Options");

  ECO_CREATE_SLIDER_INT(0, spacing, "Space between windows", 0, 250, "%1.0f px", 0, 0);
  ECO_CREATE_SLIDER_DOUBLE(0, speed, "Scale speed", 0.1, 50.0, "%1.1f", 0, 1);
  ECO_CREATE_SLIDER_DOUBLE(0, timestep, "Scale timestep", 0.1, 50.0, "%1.1f", 0, 2);
  ECO_CREATE_ENTRY(0, window_match, "Window scaled", 0, 3);
  ECO_CREATE_CHECKBOX(0, darken_back, "Darken background", 0, 4);
  ECO_CREATE_CHECKBOX(0, overlay_icon, "Show overlay icon", 0, 5);
  ECO_CREATE_SLIDER_INT(0, opacity, "Window opacity", 0, 100, "%1.0f %%", 0, 6);
  ECO_CREATE_SLIDER_INT(0, hover_time, "Mouse hover time", 50, 10000, "%1.0f ms", 0, 7);

  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;

}




