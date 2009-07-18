#include "e.h"
#include "eco_config.h"
#include "eco_actions.h"

/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("water"); 
}

/* Main creation function */
EAPI void
eco_config_water(void *data)
{
  ECO_PAGE_BEGIN("water");
  ECO_PAGE_TABLE("Options");
  ECO_CREATE_SLIDER_DOUBLE(-1, offset_scale, "Offset scale", 0.0, 10.0, "%1.1f", 0, 1);
  ECO_CREATE_SLIDER_INT(-1, rain_delay, "Rain delay", 1, 3000, "%1.0f milliseconds", 0, 2);
  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;
}


