#include "e.h"
#include "eco_config.h"
#include "eco_actions.h"

#define NUM_MODES 2
#define NUM_PLACEMENT 3
static char *modes[NUM_MODES] = {"Cover", "Flip"};
static char *placement[NUM_PLACEMENT] = {"Centered", "Above", "Below"};
/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("shift"); 
}

/* Main creation function */
EAPI void
eco_config_shift(void *data)
{
  ECO_PAGE_BEGIN("shift");
  ECO_PAGE_TABLE(_("Options"));
  ECO_CREATE_SLIDER_DOUBLE(0, speed, _("Fade animation speed"),
			   0.1, 10.0, "%1.1f", 0, 0);
  ECO_CREATE_SLIDER_DOUBLE(0, shift_speed, _("Shift animation speed"),
			   0.1, 5.0, "%1.1f", 0, 1);
  /* this seems to have no effect here */
  /* ECO_CREATE_SLIDER_DOUBLE(0, timestep, _("Timestep"),
   * 			   0.1, 5.0, "%1.1f", 0, 2); */
  ECO_CREATE_ENTRY(0, window_match, _("Showed windows"), 0, 2);

  ECO_CREATE_CHECKBOX(0, minimized, _("Show minimized"), 0, 3);
  ECO_CREATE_CHECKBOX(0, mipmaps, _("Use Mipmaps (better quality)"), 0, 4);
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_TABLE("Appearance");
  ECO_CREATE_CHECKBOX(0, reflection, _("Enable reflection"), 0, 0);
  ECO_CREATE_SLIDER_INT(0, size, _("Maximum window size"),
			   1, 100, "%1.0f %%", 0, 1);
  ECO_CREATE_SLIDER_DOUBLE(0, background_intensity, _("Background intensity"),
			   0.0, 1.0, "%1.2f", 0, 2);
  ECO_CREATE_CHECKBOX(0, overlay_icon, _("Show emblem icons"), 0, 3);
  ECO_CREATE_CHECKBOX(0, hide_all, _("Hide dock windows"), 0, 4);

  ECO_CREATE_RADIO_GROUP(0, mode, _("Switcher Mode"), modes, NUM_MODES, 0, 5);
  ECO_CREATE_SLIDER_INT(0, flip_rotation, _("Flip Rotation Angle"),
			1, 100, "%1.0f %%", 0, 6);

  ECO_CREATE_SLIDER_DOUBLE(0, cover_offset, _("Cover movement offset"),
			   -0.4, 0.2, "%1.1f", 0, 7);
  ECO_PAGE_TABLE_END;

  ECO_PAGE_TABLE(_("Window title"));
  ECO_CREATE_CHECKBOX(0, window_title, _("Show window title"), 0, 0);
  ECO_CREATE_CHECKBOX(0, title_font_bold, _("Use Bold font"), 0, 1);
  ECO_CREATE_SLIDER_INT(0, title_font_size, _("Font size"),
			6, 96, "%1.0f", 0, 2);
  
  ECO_CREATE_RADIO_GROUP(0, title_text_placement, _("Title placement"),
			 placement, NUM_PLACEMENT, 0, 3);
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_END;
}




