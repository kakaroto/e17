#include "e.h"
#include "eco_config.h"
#include "eco_actions.h"

/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("move"); 
}

/* Main creation function */
EAPI void
eco_config_move(void *data)
{
  ECO_PAGE_BEGIN("move");
  ECO_PAGE_TABLE("Options");
  
  ECO_CREATE_SLIDER_INT(-1, opacity, "Window Opacity", 1.0, 100.0, "%1.0f %%", 0, 0);

  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;
}

