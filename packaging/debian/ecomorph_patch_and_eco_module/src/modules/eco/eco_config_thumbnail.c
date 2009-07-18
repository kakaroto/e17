#include "e.h"
#include "eco_config.h"


/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("thumbnail"); 
}

/* Main creation function */
EAPI void
eco_config_thumbnail(void *data)
{
  ECO_PAGE_BEGIN("thumbnail");
  ECO_PAGE_TABLE("Options");
  ECO_CREATE_SLIDER_DOUBLE(0, thumb_size, "Thumbnail Window Size", 50, 1500, "%1.1f", 0, 0);
  ECO_CREATE_SLIDER_DOUBLE(0, show_delay, "Time (in ms) before Thumbnail is shown", 100, 10000, "%1.1f", 0, 1);
  ECO_CREATE_SLIDER_DOUBLE(0, border, "Size of Thumbnail Border", 1, 32, "%1.1f", 0, 2);
  ECO_CREATE_SLIDER_DOUBLE(0, fade_speed, "Fade In/Out Duration in seconds", 0, 5, "%1.1f", 0, 3);
  ECO_CREATE_CHECKBOX(0, current_viewport, "Set if the Taskbar shows only Windows of Current Viewport", 0, 4);
  ECO_CREATE_CHECKBOX(0, window_like, "Paint Window Like Background instead of Glow", 0, 5);
  ECO_CREATE_CHECKBOX(0, mipmap, "Generate mipmaps when possible for higher quality scaling", 0, 6);
  ECO_CREATE_CHECKBOX(0, title_enabled, "Show Window Title in Thumbnail", 0, 7);
  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;
}




