#include "e.h"
#include "eco_config.h"

#define MAX_NUM 22

#define MINIMIZE_EFF_NUM 20
static const char *minimize_effect_name[MINIMIZE_EFF_NUM] = {
   "None", "Random", "Airplane",
   "Beam Up", "Burn", "Curved Fold",
   "Domino", "Dream", "Explode",
   "Fade", "Fold", "Glide 1",
   "Glide 2", "Horiz Folds", "Leaf Spread",
   "Magic Lamp", "Razr", "Sidekick",
   "Skewer", "Zoom"
};

#define OPEN_CLOSE_EFF_NUM 22
static const char *open_close_effect_name[OPEN_CLOSE_EFF_NUM] = {
  "None", "Random", "Airplane",
  "Beam Up", "Burn", "Curved Fold",
  "Domino", "Dream", "Explode",
  "Fade", "Fold", "Glide 1",
  "Glide 2", "Horiz Folds", "Leaf Spread",
  "Magik Lamp", "Razr", "Sidekick",
  "Skewer", "Vacuum", "Wave", "Zoom"
};

#define FOCUS_EFF_NUM 4
static const char *focus_effect_name[FOCUS_EFF_NUM] = {
  "None", "Dodge", "Focus Fade", "Wave"
};

static const char *default_match =
  "(type=Normal | Dialog | ModalDialog | Utility | Unknown)";


/* Local config objects */
static int effect;
static int duration;
static int effect_cnt;
static int selected;
static char *str_effects, *str_matches, *str_randoms,
            *str_durations, *str_options;
static const char **effect_names;

/* Local widget objects */
static Evas_Object *o_matches;
static Evas_Object *o_duration;
static Evas_Object *o_effect[MAX_NUM];
static int random_effect[MAX_NUM];

/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  Eco_Option *opt, *item;
  int val, sel, i;
  Eina_List *l = NULL;
  const char *match;
  
  sel = e_widget_ilist_selected_get(o_matches);

  /* set effect */
  opt = eco_config_option_list_nth(cfg_screen, str_effects, sel);
  if (opt) opt->intValue = effect;

  /* update match */
  match = e_widget_ilist_nth_label_get(o_matches, sel);
  opt = eco_config_option_list_nth(cfg_screen, str_matches, sel);
  if (opt && match)
    {
      free (opt->stringValue);
      opt->stringValue = strdup(match);
    }
  /* update duration */
  opt = eco_config_option_list_nth(cfg_screen, str_durations, sel);
  if (opt) opt->intValue = duration;
  
  /* update random effect list */
  opt = eco_config_option_get(cfg_screen, str_randoms);
  EINA_LIST_FREE(opt->listValue, item) free(item);
  for (i = 0; i < effect_cnt; i++)
    {
     if (random_effect[i])
	{
	  item = calloc (1, sizeof(Eco_Option));
	  item->intValue = i;
	  l = eina_list_append(l, item);
	}
    }
  opt->listValue = l;
  
  eco_config_group_apply("animation"); 
}

static void
_eco_match_click_cb(void *data)
{
  Eco_Option *opt;
  int sel;
  const char *match;
  
  /* remember changes to the last selected item */
  opt = eco_config_option_list_nth(cfg_screen, str_effects, selected);
  if (opt) opt->intValue = effect;
  
  opt = eco_config_option_list_nth(cfg_screen, str_durations, selected);
  if (opt) opt->intValue = duration;

  opt = eco_config_option_list_nth(cfg_screen, str_matches, selected);
  match = e_widget_ilist_nth_label_get(o_matches, selected);
  if (opt && match)
    {
      free (opt->stringValue);
      opt->stringValue = strdup(match);
    }
  
  /* set stored values for selected match */
  sel = e_widget_ilist_selected_get(o_matches);
  opt = eco_config_option_list_nth(cfg_screen, str_durations, sel);
  if (opt)
    {
      e_widget_slider_value_int_set(o_duration, opt->intValue);
      e_widget_disabled_set(o_duration, 0);
    }
  opt = eco_config_option_list_nth(cfg_screen, str_effects, sel);
  if (opt)
    {
      e_widget_radio_toggle_set(o_effect[opt->intValue], 1);
    }
  selected = sel;
} 

static void
_eco_match_add(void *data, void *data2)
{
  Eco_Option *opt;

  e_widget_ilist_append(o_matches, NULL, default_match,
			_eco_match_click_cb, NULL, NULL);

  opt = eco_config_option_list_add(cfg_screen, str_matches);
  opt->stringValue = strdup(default_match);  
  
  opt = eco_config_option_list_add(cfg_screen, str_effects);
  opt->intValue = 11;
  
  opt = eco_config_option_list_add(cfg_screen, str_durations);
  opt->intValue = 300;
  e_widget_slider_value_int_set(o_duration, opt->intValue);
  
  opt = eco_config_option_list_add(cfg_screen, str_randoms);

  /* opt = eco_config_option_list_add(cfg_screen, str_options); */
  
  e_widget_disabled_set(o_duration, 0);
  e_widget_ilist_selected_set(o_matches, e_widget_ilist_count(o_matches));
}

static void
_eco_match_dialog_ok_cb(void *data, E_Dialog *dia)
{
  Evas_Object *entry = data;
  Eco_Option *match;
  
  int sel = e_widget_ilist_selected_get(o_matches);	
  const char *val = e_widget_entry_text_get(entry);	
  e_widget_ilist_nth_label_set(o_matches, sel, val);
 
  e_util_defer_object_del(E_OBJECT(dia));
}

static void
_eco_match_edit(void *data, void *data2)
{
  int sel = e_widget_ilist_selected_get(o_matches);
  const char *val = e_widget_ilist_nth_label_get(o_matches, sel);			
  eco_match_dialog(val, _eco_match_dialog_ok_cb);
}

static void
_eco_match_del(void *data, void *data2)
{
  int sel = e_widget_ilist_selected_get(o_matches);			

  e_widget_ilist_remove_num(o_matches, sel);
  eco_config_option_list_del(cfg_screen, str_effects, sel);
  eco_config_option_list_del(cfg_screen, str_matches, sel);
  eco_config_option_list_del(cfg_screen, str_durations, sel);
  eco_config_option_list_del(cfg_screen, str_randoms, sel);
  /* eco_config_option_list_del(cfg_screen, str_options, sel); */
}				

/* Page creation functions */
static void
eco_config_animation_page(void *data)
{
  int val = 0;
  int max = 6;
  selected = -1;
    
  ECO_PAGE_BEGIN("animation");
  
  ECO_PAGE_TABLE(_("Window Match"));
  ECO_CREATE_LIST_MATCH(0, str_matches, 0, 0);
  /* to remember previously selected item */
  if (e_widget_ilist_count(o_matches) > 0) selected = 0;
  ECO_PAGE_TABLE_END;

  ECO_PAGE_TABLE(_("Animation duration"));
  o = e_widget_label_add(cfdata->evas, _("Duration"));
  e_widget_frametable_object_append(ta, o, 0, 0, 1, 1, 1, 0, 0, 0);
  o_duration = e_widget_slider_add(cfdata->evas, 1, 0, "%1.0f ms", 50, 2000,
				   1.0, 0, NULL, &duration, 0);
  e_widget_disabled_set(o_duration, 1);					\
  e_widget_frametable_object_append(ta, o_duration, 1, 0, 1, 1, 1, 0, 1, 0);
  
  opt = eco_config_option_list_nth(cfg_screen, str_durations, 0);
  if (opt)
    {      
      e_widget_slider_value_int_set(o_duration, opt->intValue);
      e_widget_disabled_set(o_duration, 0);
    }
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_TABLE( _("Animation for selected match"));
  opt = eco_config_option_list_nth(cfg_screen, str_effects, 0);
  if (opt) effect = opt->intValue;
  group = e_widget_radio_group_new(&effect);
  for (i = 0; i < effect_cnt; i++)					
    {								
      o_effect[i] = e_widget_radio_add(cfdata->evas, effect_names[i], i, group);
      e_widget_frametable_object_append(ta, o_effect[i], (i/max), (i%max),
					1, 1, 1, 0, 1, 0);
      if (effect == i) e_widget_radio_toggle_set(o_effect[i], 1);	
    }
  ECO_PAGE_TABLE_END;
  
  ECO_PAGE_TABLE( _("Animation random pool"));
  opt = eco_config_option_get(cfg_screen, str_randoms); 

  for (i = 0; i < effect_cnt; i++)  random_effect[i] = 0;
  
  for (l = opt->listValue; l; l = l->next)
    {
      item = l->data;
      random_effect[item->intValue] = 1;
    }
  
  for (i = 0; i < effect_cnt; i++)					
    {								
      o = e_widget_check_add(cfdata->evas, effect_names[i], &random_effect[i]);
      e_widget_frametable_object_append(ta, o, (i/max), (i%max),
					1, 1, 1, 0, 1, 0);
    }
  ECO_PAGE_TABLE_END;
  ECO_PAGE_END;
}


void
eco_config_animation_open(void *data)
{
  str_effects   = "open_effects";
  str_matches   = "open_matches" ;
  str_durations = "open_durations";
  str_randoms   = "open_random_effects";
  str_options   = "open_options";
  effect_names  =  open_close_effect_name;
  effect_cnt    =  OPEN_CLOSE_EFF_NUM;
  
  eco_config_animation_page(data);
}

void
eco_config_animation_close(void *data)
{
  str_effects   = "close_effects";
  str_matches   = "close_matches" ;
  str_durations = "close_durations";
  str_randoms   = "close_random_effects";
  str_options   = "close_options";
  effect_names  =  open_close_effect_name;
  effect_cnt    =  OPEN_CLOSE_EFF_NUM;
  
  eco_config_animation_page(data);
}

void
eco_config_animation_minimize(void *data)
{
  str_effects   = "minimize_effects";
  str_matches   = "minimize_matches" ;
  str_durations = "minimize_durations";
  str_randoms   = "minimize_random_effects";
  str_options   = "minimize_options";
  effect_names  =  minimize_effect_name;
  effect_cnt    =  MINIMIZE_EFF_NUM;
  
  eco_config_animation_page(data);
  
}




