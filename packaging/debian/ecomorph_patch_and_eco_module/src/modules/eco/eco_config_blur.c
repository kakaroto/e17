#include "e.h"
#include "eco_config.h"
#include "eco_actions.h"

#define NUM_FILTER 3
static char *filters[NUM_FILTER] = {"4xBilinear", "Gaussian", "Mipmap"};

/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("blur"); 
}


/* Main creation function */
EAPI void
eco_config_blur(void *data)
{
  ECO_PAGE_BEGIN("blur");
  ECO_PAGE_TABLE("Options");
  ECO_CREATE_SLIDER_DOUBLE(0, blur_speed, "Blur speed", 0.1, 10.0, "%1.1f", 0, 0);
  ECO_CREATE_CHECKBOX(0, focus_blur, "Focus blurring", 0, 1);
  ECO_CREATE_ENTRY(0, focus_blur_match, "Focus blur matching", 0, 2);
  ECO_CREATE_CHECKBOX(0, alpha_blur, "Transparency blurring", 0, 3);
  ECO_CREATE_ENTRY(0, alpha_blur_match, "Transparency blur matching", 0, 4);
  ECO_CREATE_RADIO_GROUP(0, filter, "Filter", filters, NUM_FILTER, 0, 5);
  ECO_CREATE_SLIDER_INT(0, gaussian_radius, "Gausian radius", 1, 15, "%1.1f", 0, 9);
  ECO_CREATE_SLIDER_DOUBLE(0, gaussian_strength, "Gausian strength", 0.0, 1, "%1.1f", 0, 10);
  ECO_CREATE_SLIDER_DOUBLE(0, mipmap_lod, "Mipmap LOD", 0.1, 5, "%1.1f", 0, 11);
  ECO_CREATE_SLIDER_INT(0, saturation, "Saturation", 0, 100, "%1.1f", 0, 12);
  ECO_CREATE_CHECKBOX(0, occlusion, "occlusion", 0, 13);
  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;
}
