#include "e.h"
#include "eco_config.h"
#include "eco_actions.h"


/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("switcher");  
}

EAPI void
eco_config_switcher(void *data)
{
  ECO_PAGE_BEGIN("switcher");
  ECO_PAGE_TABLE("Options");
  ECO_CREATE_CHECKBOX(0, bring_to_front, "Bring selected window to front", 0, 1);
  ECO_CREATE_CHECKBOX(0, minimized, "Show minimized windows", 0, 2);
  // option is not implemented atm
  /* ECO_CREATE_CHECKBOX(0, auto_rotate, "Rotate to the selected window while switching", 0, 3); */
  ECO_CREATE_CHECKBOX(0, icon, "Show icon next to thumbnail", 0, 4);
  ECO_CREATE_ENTRY(0, window_match, "Window showed", 0, 5);
  ECO_CREATE_SLIDER_DOUBLE(0, speed, "Speed", 0.1, 50.0, "%1.1f", 0, 6);
  ECO_CREATE_SLIDER_INT(0, opacity, "Opacity", 0, 100, "%1.0f", 0, 7);
  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;
}


