#ifdef E_TYPEDEFS
#else
#ifndef E_INT_CONFIG_ECO_H
#define E_INT_CONFIG_ECO_H


struct _E_Config_Dialog_Data
{
   E_Config_Dialog *cfd;
   E_DBus_Connection *conn;
   
   int use_composite;
   int ecomorph;
  
   Evas *evas;
   Evas_Object *o_container;
   Evas_Object *o_content;
   Evas_Object *o_start;
   Evas_Object *o_stop;
};

typedef struct _Eco_Option
{
  int type;

  int intValue;
  double doubleValue;
  char *stringValue;
  Eina_List *listValue;
}Eco_Option;

typedef struct _Eco_Group
{
  Eina_Hash *data;
}Eco_Group;

EAPI E_Config_Dialog *e_int_config_eco(E_Container *con, const char *params __UNUSED__);

EAPI int         eco_config_open();
EAPI void        eco_config_close();
EAPI void        eco_config_group_apply(const char *group);
EAPI void        eco_config_group_open(const char *group);
EAPI void        eco_config_group_close();
EAPI Eco_Option *eco_config_option_get(Eco_Group *group, const char *option);
EAPI Eco_Option *eco_config_option_list_nth(Eco_Group *group, const char *option, int num);
EAPI Eco_Option *eco_config_option_list_del(Eco_Group *group, const char *option, int num);
EAPI Eco_Option *eco_config_option_list_add(Eco_Group *group, const char *option);

E_DBus_Connection *dbus_con;
const char *edje_file;

extern Eco_Group *cfg_screen;
extern Eco_Group *cfg_display;
extern Eet_Data_Descriptor *eco_edd_group, *eco_edd_option;

#define ECO_PAGE_BEGIN(PLUGIN_NAME)		\
  Evas_Object *ob, *ta, *li;			\
  Evas_Object *o;				\
  E_Config_Dialog_Data *cfdata = data;		\
  Eina_List *l;                                 \
  E_Radio_Group *group;				\
  Eco_Option *opt, *item;			\
  int i, r;					\
  eco_config_group_open(PLUGIN_NAME);		\
  li = e_widget_list_add(cfdata->evas, 0, 0);

#define ECO_PAGE_END				\
  eco_attach_widget(li, _apply);

#define ECO_PAGE_TABLE(NAME)			\
  ta = e_widget_frametable_add(cfdata->evas, _(NAME), 0);

#define ECO_PAGE_TABLE_END			\
  e_widget_list_object_append(li, ta, 1, 1, 0.0);

#define ECO_CREATE_CHECKBOX(SCREEN, NAME,LABEL,COL,ROW)		\
  Eco_Option *opt_##NAME = eco_config_option_get((SCREEN < 0) ?		\
						 cfg_display : cfg_screen, #NAME); \
  o = e_widget_check_add(cfdata->evas, _(LABEL), &(opt_##NAME->intValue)); \
  e_widget_check_checked_set(o,	opt_##NAME->intValue);			\
  e_widget_frametable_object_append(ta, o, COL, ROW, 1, 1, 1, 0, 0, 0);


#define ECO_CREATE_SLIDER_INT(SCREEN, NAME,LABEL,MIN,MAX,FORMAT,COL,ROW) \
  Eco_Option *opt_##NAME = eco_config_option_get((SCREEN < 0) ?		\
						 cfg_display : cfg_screen, #NAME); \
  o = e_widget_label_add(cfdata->evas, _(LABEL));			\
  e_widget_frametable_object_append(ta, o, COL, ROW, 1, 1, 1, 0, 0, 0);	\
  o = e_widget_slider_add(cfdata->evas, 1, 0, FORMAT, MIN, MAX, 1.0, 0, NULL, \
			  &(opt_##NAME->intValue), 0);			\
  e_widget_frametable_object_append(ta, o, COL+1, ROW, 4, 1, 1, 0, 1, 0);

#define ECO_CREATE_SLIDER_LIST_INT(SCREEN, NAME, LABEL,MIN,MAX,FORMAT,COL,ROW) \
  o = e_widget_label_add(cfdata->evas, _(LABEL));		\
  e_widget_frametable_object_append(ta, o, COL, ROW, 1, 1, 1, 0, 0, 0);	\
  o_##NAME = e_widget_slider_add(cfdata->evas, 1, 0, FORMAT, MIN, MAX, 1.0, 0, NULL, \
				 &NAME, 0); 				\
  e_widget_disabled_set(o_##NAME, 1);					\
  e_widget_frametable_object_append(ta, o_##NAME, COL+1, ROW, 1, 1, 1, 0, 1, 0);

#define ECO_CREATE_SLIDER_DOUBLE(SCREEN, NAME,LABEL,MIN,MAX,FORMAT,COL,ROW) \
  Eco_Option *opt_##NAME = eco_config_option_get((SCREEN < 0) ?		\
						 cfg_display : cfg_screen, #NAME); \
  o = e_widget_label_add(cfdata->evas, _(LABEL));			\
  e_widget_frametable_object_append(ta, o, COL, ROW, 1, 1, 1, 0, 0, 0);	\
  o = e_widget_slider_add(cfdata->evas, 1, 0, FORMAT, MIN, MAX, 0.0001, 0, \
			  &(opt_##NAME->doubleValue), NULL, 0);		\
  e_widget_frametable_object_append(ta, o, COL+1, ROW, 4, 1, 1, 0, 1, 0);

#define ECO_CREATE_ENTRY(SCREEN, NAME,LABEL,COL,ROW)			\
  Eco_Option *opt_##NAME = eco_config_option_get((SCREEN < 0) ?		\
						 cfg_display : cfg_screen, #NAME); \
  o = e_widget_label_add(cfdata->evas, LABEL);			\
  e_widget_frametable_object_append(ta, o, COL, ROW, 1, 1, 1, 0, 0, 0); \
  o = e_widget_entry_add(cfdata->evas,  &(opt_##NAME->stringValue), NULL, NULL, NULL); \
  e_widget_frametable_object_append(ta, o, COL+1, ROW, 3, 1, 1, 0, 1, 0);

#define ECO_CREATE_RADIO_GROUP(SCREEN, NAME,LABEL,OPTIONS,OPT_NUM,COL,ROW) \
  opt = eco_config_option_get((SCREEN < 0) ?				\
			      cfg_display : cfg_screen, #NAME);		\
    o = e_widget_label_add(cfdata->evas, _(LABEL));			\
    e_widget_frametable_object_append(ta, o, COL, ROW, 1, 1, 1, 0, 0, 0); \
    group = e_widget_radio_group_new(&(opt->intValue));			\
    for (i = 0, r = COL + 1; i < OPT_NUM; i++)				\
    {									\
      o = e_widget_radio_add(cfdata->evas, OPTIONS[i], i, group);	\
      if (i == opt->intValue) e_widget_radio_toggle_set(o, 1);	\
      e_widget_frametable_object_append(ta, o, r++, ROW, 1, 1, 1, 0, 0, 0); \
    }

#define ECO_CREATE_LIST_STRING(NAME,COL,ROW)				\
  o_##NAME = e_widget_ilist_add(cfdata->evas, 0, 0, NULL);		\
  e_widget_min_size_set(o_##NAME, 100, 110);				\
  e_widget_ilist_go(o_##NAME);						\
  e_widget_frametable_object_append(ta, o_##NAME, COL, ROW, 1, 3, 1, 0, 1, 0); \
  ob = e_widget_button_add(cfdata->evas, "Add", NULL, eco_##NAME##_add, NULL, NULL); \
  e_widget_frametable_object_append(ta, ob, COL + 1, ROW, 2, 1, 1, 1, 0, 0); \
  ob = e_widget_button_add(cfdata->evas, "Del", NULL, eco_##NAME##_del, NULL, NULL); \
  e_widget_frametable_object_append(ta, ob, COL + 1, ROW + 1, 2, 1, 1, 1, 0, 0); \
  ob = e_widget_button_add(cfdata->evas, "Edit", NULL, eco_##NAME##_edit, NULL, NULL); \
  e_widget_frametable_object_append(ta, ob, COL + 1, ROW + 2, 2, 1, 1, 1, 0, 0); 


#define ECO_CREATE_LIST_MATCH(SCREEN, NAME,COL,ROW)			\
  o_matches = e_widget_ilist_add(cfdata->evas, 0, 0, NULL);		\
  e_widget_min_size_set(o_matches, 100, 110);				\
  opt = eco_config_option_get((SCREEN < 0) ?				\
			      cfg_display : cfg_screen, NAME);		\
  for (l = (opt->listValue); l; l = l->next) {				\
      e_widget_ilist_append(o_matches, NULL,				\
			    ((Eco_Option *)l->data)->stringValue,	\
			    _eco_match_click_cb , NULL, NULL); }	\
  if (eina_list_count(opt->listValue) > 0)				\
    e_widget_ilist_selected_set(o_matches, 0);				\
  e_widget_ilist_go(o_matches);						\
  e_widget_frametable_object_append(ta, o_matches, COL, ROW, 1, 3, 1, 0, 1, 0); \
  ob = e_widget_button_add(cfdata->evas, "Add", NULL, _eco_match_add, NULL, NULL); \
  e_widget_frametable_object_append(ta, ob, COL + 1, ROW, 2, 1, 1, 1, 0, 0); \
  ob = e_widget_button_add(cfdata->evas, "Del", NULL, _eco_match_del, NULL, NULL); \
  e_widget_frametable_object_append(ta, ob, COL + 1, ROW + 1, 2, 1, 1, 1, 0, 0); \
  ob = e_widget_button_add(cfdata->evas, "Edit", NULL, _eco_match_edit, NULL, NULL); \
  e_widget_frametable_object_append(ta, ob, COL + 1, ROW + 2, 2, 1, 1, 1, 0, 0); 


typedef void (Check_Plugins_Resp)(int ok, E_Config_Dialog_Data *cfdata);

EAPI void eco_error_messagge(const char *text);
EAPI char *eco_config_path_get(const char *plugin, int screen);
EAPI unsigned char eco_request_option(const char *plugin, int screen, const char *option, E_DBus_Method_Return_Cb cb, void *data);
EAPI unsigned char eco_send_option_double(const char *plugin, int screen, const char *option, double val);
EAPI unsigned char eco_send_option_int(const char *plugin, int screen, const char *option, int val);
EAPI unsigned char eco_send_option_boolean(const char *plugin, int screen, const char *option, int val);
EAPI unsigned char eco_send_option_string(const char *plugin, int screen, const char *option, char *val);
EAPI unsigned char eco_send_option_string_array(const char *plugin, int screen, const char *option, char *val);
EAPI unsigned char eco_send_option_string_list(const char *plugin, int screen, const char *option, Evas_Object *li);
EAPI unsigned char eco_send_option_int_array(const char *plugin, int screen, const char *option, char *val);
EAPI unsigned char eco_send_option_int_array2(const char *plugin, int screen, const char *option, int *val, int size);
EAPI unsigned char eco_send_option_action(const char *plugin, int screen, const char *option, char *key, char *mouse);
EAPI char* eco_decode_string_array(DBusMessageIter array);
EAPI void eco_match_dialog(const char *val, void *_func);
EAPI void eco_attach_widget(Evas_Object *sub, void apply_func(E_Config_Dialog_Data *cfdata));
EAPI void eco_config_cleanup_func_set(void apply_func(void));


EAPI int eco_check_plugins(char *list, Check_Plugins_Resp cb);

EAPI void eco_config_general(void *data);
EAPI void eco_config_opacity(void *data);
EAPI void eco_config_expo(void *data);
EAPI void eco_config_water(void *data);
EAPI void eco_config_blur(void *data);
EAPI void eco_config_switcher(void *data);
EAPI void eco_config_wobbly(void *data);
EAPI void eco_config_wall(void *data);
EAPI void eco_config_scale(void *data);
EAPI void eco_config_scaleaddon(void *data);
EAPI void eco_config_ring(void *data);
EAPI void eco_config_shift(void *data);
EAPI void eco_config_animation(void *data);
EAPI void eco_config_animation3(void *data);
EAPI void eco_config_animation4(void *data);
EAPI void eco_config_animation5(void *data);
EAPI void eco_config_animation_open(void *data);
EAPI void eco_config_animation_close(void *data);
EAPI void eco_config_animation_minimize(void *data);
EAPI void eco_config_move(void *data);
EAPI void eco_config_cube(void *data);
EAPI void eco_config_rotate(void *data);
EAPI void eco_config_thumbnail(void *data);

#endif
#endif
