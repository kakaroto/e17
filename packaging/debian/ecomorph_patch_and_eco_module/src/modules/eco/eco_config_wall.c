#include "e.h"
#include "eco_config.h"

/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("wall"); 
}

/* Main creation function */
EAPI void
eco_config_wall(void *data)
{
  ECO_PAGE_BEGIN("wall");
  ECO_PAGE_TABLE("Options");
  ECO_CREATE_CHECKBOX(-1, show_switcher, "Show viewport switcher", 0, 0);
  ECO_CREATE_CHECKBOX(-1, miniscreen, "Show live previews", 0, 1);
  ECO_CREATE_CHECKBOX(-1, move_background, "Move background and popup", 0, 2);
  ECO_CREATE_SLIDER_DOUBLE(-1, preview_timeout, "Viewport switcher duration", 0.0, 2.0, "%1.2f sec", 0, 3);
  ECO_CREATE_SLIDER_DOUBLE(-1, slide_duration, "Wall sliding duration", 0.1, 5.0, "%1.2f sec", 0, 4);
  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;
}




