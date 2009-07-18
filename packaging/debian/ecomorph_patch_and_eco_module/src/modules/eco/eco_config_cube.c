#include "e.h"
#include "eco_config.h"
#include "eco_actions.h"

/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
   eco_config_group_apply("cube");  
}

/* Main creation function */
EAPI void
eco_config_cube(void *data)
{
   ECO_PAGE_BEGIN("cube");
   ECO_PAGE_TABLE("Options");

   ECO_CREATE_CHECKBOX(0, in, "Inside Cube", 0, 0);
   ECO_CREATE_SLIDER_DOUBLE(0, active_opacity, "Opacity during rotation", 0.0, 100.0, "%1.1f", 0, 1);
   ECO_CREATE_SLIDER_DOUBLE(0, inactive_opacity, "Opacity when not rotating", 0.0, 100.0, "%1.1f", 0, 2);
   ECO_CREATE_SLIDER_DOUBLE(0, fade_time, "Desktop window opacity fade time", 0.0, 10.0, "%1.1f", 0, 3);
   ECO_CREATE_CHECKBOX(0, transparent_manual_only, "Transparency only on mouse rotate", 0, 4);

   ECO_PAGE_TABLE_END;
   ECO_PAGE_END;

}
