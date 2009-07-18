#include "e.h"
#include "eco_config.h"
#include "eco_actions.h"


/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("expo"); 
}

/* Main creation function */
EAPI void
eco_config_expo(void *data)
{
  ECO_PAGE_BEGIN("expo");
  ECO_PAGE_TABLE("Options");
  ECO_CREATE_CHECKBOX(-1, reflection, "Show reflection", 0, 0);
  ECO_CREATE_CHECKBOX(-1, rotate, "Tilt view", 0, 1);
  ECO_CREATE_CHECKBOX(-1, hide_docks, "Hide panels/docks in expo", 0, 2);
  ECO_CREATE_CHECKBOX(-1, immediate_move, "Immediate move (disables wobbliness)", 0, 3);
  ECO_CREATE_CHECKBOX(-1, mipmaps, "Use mipmaps (better quality)", 0, 4);
  ECO_CREATE_SLIDER_DOUBLE(-1, distance, "Wall distance", 0.0, 1.0, "%1.2f", 0, 5);
  ECO_CREATE_SLIDER_DOUBLE(-1, zoom_time, "Zoom duration", 0.1, 5.0, "%1.1f", 0, 6);
  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;
}




