#include "e.h"
#include "eco_config.h"
#include "eco_actions.h"

/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("scaleaddon");  
}

/* Main creation function */
EAPI void
eco_config_scaleaddon(void *data)
{
  ECO_PAGE_BEGIN("scaleaddon");
  ECO_PAGE_TABLE("Options");
  ECO_CREATE_CHECKBOX(0, window_highlight, "Highlight selected window", 0, 0);
  ECO_CREATE_CHECKBOX(0, window_title, "Show window title", 0, 1);
  ECO_CREATE_CHECKBOX(0, title_bold, "Bold title font", 0, 2);
  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;

}
