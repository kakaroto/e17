#include "e.h"
#include "eco_config.h"


static void
_apply(E_Config_Dialog_Data *cfdata)
{
  eco_config_group_apply("animation"); 
}

/* Main creation function */
EAPI void
eco_config_animation5(void *data)
{
  ECO_PAGE_BEGIN("animation");

  ECO_PAGE_TABLE(_("General"));
  ECO_CREATE_CHECKBOX(0, all_random, "Random animations for all effects", 0, 0);
  ECO_PAGE_TABLE_END;

  ECO_PAGE_TABLE(_("Dodge"));
  ECO_CREATE_SLIDER_DOUBLE(0, dodge_gap_ratio, "Dodge gap ratio", 0.0, 1.0, "%1.2f", 0, 0);
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_TABLE(_("Horizontal folds"));
  ECO_CREATE_SLIDER_DOUBLE(0, horizontal_folds_amp, "Size of the waves in the fold", -0.5, 0.5, "%1.2f", 0, 0);
  ECO_CREATE_SLIDER_INT(0, horizontal_folds_num_folds, "Number of Horizontal Folds", 1, 50, "%1.0f", 0, 1);
  ECO_CREATE_CHECKBOX(0, horizontal_folds_zoom_to_taskbar, "Zoom to taskbar on minimize", 0, 2);
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_TABLE(_("Fold"));
  ECO_CREATE_SLIDER_INT(0, fold_gridx, "Window grid width", 1, 100, "%1.0f", 0, 0);
  ECO_CREATE_SLIDER_INT(0, fold_gridy, "Window grid height", 1, 100, "%1.0f", 0, 1);
  ECO_PAGE_TABLE_END;
 
  ECO_PAGE_TABLE(_("Rollup"));
  ECO_CREATE_CHECKBOX(0, rollup_fixed_interior, "Rollup fixed interior", 0, 0);
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_TABLE(_("Curved fold"));
  ECO_CREATE_SLIDER_DOUBLE(0, curved_fold_amp, "Curved fold amplitude", -0.5, 0.5, "%1.2f", 0, 0);
  ECO_CREATE_CHECKBOX(0, curved_fold_zoom_to_taskbar, "Zoom to taskbar on minimize", 0, 1);
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_END;
}

