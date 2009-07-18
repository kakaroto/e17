#include "e.h"
#include "eco_config.h"
#include "eco_actions.h"

/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
   eco_config_group_apply("rotate");  
}

/* Main creation function */
EAPI void
eco_config_rotate(void *data)
{
   ECO_PAGE_BEGIN("rotate");
   ECO_PAGE_TABLE("Options");

   ECO_CREATE_SLIDER_DOUBLE(0, speed, "Rotation speed", 0.1, 50.0, "%1.1f", 0, 0);
   ECO_CREATE_SLIDER_DOUBLE(0, acceleration, "Rotation acceleration", 1.0, 20.0, "%1.1f", 0, 1);
   ECO_CREATE_SLIDER_DOUBLE(0, timestep, "Rotation timestep", 0.1, 50.0, "%1.1f", 0, 2);
   ECO_CREATE_SLIDER_DOUBLE(0, zoom, "Rotation zoom", 0.0, 2.0, "%1.1f", 0, 3);

   ECO_PAGE_TABLE_END;
   ECO_PAGE_END;

}
