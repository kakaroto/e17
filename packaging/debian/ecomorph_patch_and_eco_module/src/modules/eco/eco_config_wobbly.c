#include "e.h"
#include "eco_config.h"


/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("wobbly"); 
}

/* Main creation function */
EAPI void
eco_config_wobbly(void *data)
{
  ECO_PAGE_BEGIN("wobbly");
  ECO_PAGE_TABLE("Options");
  ECO_CREATE_SLIDER_DOUBLE(0, friction, "Spring friction", 0.1, 10.0, "%1.1f", 0, 0);
  ECO_CREATE_SLIDER_DOUBLE(0, spring_k, "Spring Konstant", 0.1, 10.0, "%1.1f", 0, 1);
  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;
}




