#include "e.h"
#include "eco_config.h"

static void eco_match_click_cb(void *data);

Evas_Object *o_matches;
Evas_Object *o_opacity;
int opacity;

static void
_eco_match_click_cb(void *data)
{
  int sel = e_widget_ilist_selected_get(o_matches);
  Eco_Option *value = eco_config_option_list_nth(cfg_screen, "opacity_values", sel);
  if (value)
    {
      e_widget_slider_value_int_set(o_opacity, value->intValue);
      e_widget_disabled_set(o_opacity, 0);	
    }
} 

/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  Eco_Option *value;
  int val;
  e_widget_slider_value_int_get(o_opacity, &val);
  int sel = e_widget_ilist_selected_get(o_matches);
  value = eco_config_option_list_nth(cfg_screen, "opacity_values", sel);
  if (value) value->intValue = val;
  
  eco_config_group_apply("core"); 
}

static void
_eco_match_add(void *data, void *data2)
{
  Eco_Option *match, *value;
  e_widget_ilist_append(o_matches, NULL, "class=XTerm",
			_eco_match_click_cb, NULL, NULL);

  match = eco_config_option_list_add(cfg_screen, "opacity_matches");
  match->stringValue = strdup("class=XTerm");  
  value = eco_config_option_list_add(cfg_screen, "opacity_values");
  value->intValue = 100;
  e_widget_slider_value_int_set(o_opacity, value->intValue);
  
  e_widget_disabled_set(o_opacity, 0);
  e_widget_ilist_selected_set(o_matches,
			      e_widget_ilist_count(o_matches));
}


static void
_eco_match_dialog_ok_cb(void *data, E_Dialog *dia)
{
  Evas_Object *entry = data;
  Eco_Option *match;
  
  int sel = e_widget_ilist_selected_get(o_matches);	
  const char *val = e_widget_entry_text_get(entry);	
  e_widget_ilist_nth_label_set(o_matches, sel, val);
  
  match = eco_config_option_list_nth(cfg_screen, "opacity_matches", sel);
  if (match)
    {
      free (match->stringValue);
      match->stringValue = strdup(val);
    }
  e_util_defer_object_del(E_OBJECT(dia));
}

static void
_eco_match_edit(void *data, void *data2)
{
  const char *val;							
  int sel = e_widget_ilist_selected_get(o_matches);			
  val = e_widget_ilist_nth_label_get(o_matches, sel);			
  eco_match_dialog(val, _eco_match_dialog_ok_cb);
}			

static void
_eco_match_del(void *data, void *data2)
{		
  int num = e_widget_ilist_selected_get(o_matches);			
  e_widget_ilist_remove_num(o_matches, num);

  eco_config_option_list_del(cfg_screen, "opacity_matches", num);
  eco_config_option_list_del(cfg_screen, "opacity_values", num);
}				


/* Main creation function */
EAPI void
eco_config_opacity(void *data)
{
  ECO_PAGE_BEGIN("core");

  ECO_PAGE_TABLE(_("Options"));
  ECO_CREATE_SLIDER_INT(0, opacity_step, "Opacity change step", 1, 50, "%1.0f", 0, 0);
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_TABLE(_("Window Match"));
  ECO_CREATE_LIST_MATCH(0, "opacity_matches", 0, 0);
  ECO_PAGE_TABLE_END;

  ECO_PAGE_TABLE( _("Opacity for selected match"));
  ECO_CREATE_SLIDER_LIST_INT(0, opacity, "Opacity", 0, 100, "%1.0f %%", 0, 0);   
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_END;
}


