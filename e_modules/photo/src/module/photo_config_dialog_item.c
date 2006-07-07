#include "Photo.h"

struct _E_Config_Dialog_Data
{
   Photo_Item *pi;

   int timer_s;
   int timer_active;

   int alpha;
   int show_label;

   int action_mouse_over;
   int action_mouse_left;
   int action_mouse_middle;
};

static void        *_create_data(E_Config_Dialog *cfd);
static void         _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void         _fill_data(E_Config_Dialog_Data *cfdata, Photo_Item *pi);
static void         _common_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata, Evas_Object *o);
static int          _common_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int          _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int          _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);


/*
 * Public functions
 */

int  photo_config_dialog_item_show(Photo_Item *pi)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   char buf[4096];

   v = E_NEW(E_Config_Dialog_View, 1);
   
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;
   
   snprintf(buf, sizeof(buf), "%s/module.eap", e_module_dir_get(photo->module));
   cfd = e_config_dialog_new(e_container_current_get(e_manager_current_get()),
			     _("Photo Item Configuration"), buf, 0, v, pi);
   pi->config_dialog = cfd;

   return 1;
}

void photo_config_dialog_item_hide(Photo_Item *pi)
{
   e_object_del(E_OBJECT(pi->config_dialog));
   pi->config_dialog = NULL;
}


/*
 * Private functions
 *
 */

static void *
_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata;
   Photo_Item *pi;

   pi = cfd->data;
   
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(cfdata, pi);
   cfd->cfdata = cfdata;
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   cfdata->pi->config_dialog = NULL;
   free(cfdata);
}

static void
_fill_data(E_Config_Dialog_Data *cfdata, Photo_Item *pi)
{
   cfdata->pi = pi;

   cfdata->timer_s = pi->config->timer_s;
   cfdata->timer_active = pi->config->timer_active;

   cfdata->alpha = pi->config->alpha;
   cfdata->show_label = pi->config->show_label;

   cfdata->action_mouse_over = pi->config->action_mouse_over;
   cfdata->action_mouse_left = pi->config->action_mouse_left;
   cfdata->action_mouse_middle = pi->config->action_mouse_middle;
}

static void
_common_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata, Evas_Object *o)
{
   Evas_Object *of, *ob;
   E_Radio_Group *rg;


   of = e_widget_frametable_add(evas, _("Timer between pictures change"), 0);

   ob = e_widget_check_add(evas, _("Activate"), &(cfdata->timer_active));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);

   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f seconds"),
                            (float)ITEM_TIMER_S_MIN, (float)ITEM_TIMER_S_MAX,
                            1.0, 0,
                            NULL, &(cfdata->timer_s), 130);
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 0, 1, 0);

   e_widget_table_object_append(o, of, 0, 0, 1, 1, 1, 1, 1, 1);

   

   of = e_widget_frametable_add(evas, _("Appearance"), 0);

   ob = e_widget_label_add(evas, _("Alpha"));
   e_widget_frametable_object_append(of, ob, 0, 0, 2, 1, 1, 1, 1, 1);

   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0.0, 255, 1.0, 0,
                            NULL, &(cfdata->alpha), 130);
   e_widget_frametable_object_append(of, ob, 0, 1, 3, 1, 1, 0, 1, 0);

   ob = e_widget_label_add(evas, _("Always show label"));
   e_widget_frametable_object_append(of, ob, 0, 2, 3, 1, 1, 1, 1, 1);

   rg = e_widget_radio_group_new(&(cfdata->show_label));
   ob = e_widget_radio_add(evas, _("Yes"), ITEM_SHOW_LABEL_YES, rg);
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("No"), ITEM_SHOW_LABEL_NO, rg);
   e_widget_frametable_object_append(of, ob, 1, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Default setting"), ITEM_SHOW_LABEL_PARENT, rg);
   e_widget_frametable_object_append(of, ob, 2, 3, 1, 1, 1, 1, 1, 1);   

   e_widget_table_object_append(o, of, 0, 1, 1, 1, 1, 1, 1, 1);
}

static int
_common_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Photo_Item *pi;

   pi = cfdata->pi;

   if ((pi->config->timer_s != cfdata->timer_s) ||
       (pi->config->timer_active != cfdata->timer_active))
     photo_item_timer_set(pi, cfdata->timer_active, cfdata->timer_s);

   if (pi->config->alpha != cfdata->alpha)
     {
        pi->config->alpha = cfdata->alpha;
        evas_object_color_set(pi->obj, 255, 255, 255, pi->config->alpha);
     }

   if (pi->config->show_label != cfdata->show_label)
     {
        pi->config->show_label = cfdata->show_label;
        photo_item_label_mode_set(pi);
     }

   return 1;
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o;
   
   o = e_widget_table_add(evas, 0);

   _common_create_widgets(cfd, evas, cfdata, o);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   Photo_Item *pi;
   int ret;

   pi = cfdata->pi;
   ret = _common_apply_data(cfd, cfdata);

   photo_item_label_mode_set(pi);

   photo_config_save();
   return ret;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o, *of, *ob;
   E_Radio_Group *rg;

   o = e_widget_table_add(evas, 0);

   _common_create_widgets(cfd, evas, cfdata, o);


   of = e_widget_frametable_add(evas, _("Mouse actions"), 0);

   ob = e_widget_label_add(evas, _("Over"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   CONFIG_DIALOG_ACTION_CHOICE(of, 0, 2, cfdata->action_mouse_over, ITEM_ACTION_PARENT);

   ob = e_widget_label_add(evas, _("Left"));
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("click"));
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 1, 1, 1);
   CONFIG_DIALOG_ACTION_CHOICE(of, 1, 2, cfdata->action_mouse_left, ITEM_ACTION_PARENT);

   ob = e_widget_label_add(evas, _("Middle"));
   e_widget_frametable_object_append(of, ob, 2, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("click"));
   e_widget_frametable_object_append(of, ob, 2, 1, 1, 1, 1, 1, 1, 1);
   CONFIG_DIALOG_ACTION_CHOICE(of, 2, 2, cfdata->action_mouse_middle, ITEM_ACTION_PARENT);
   CONFIG_DIALOG_ACTION_CHOICE_LABELS(of, 3, 2, _("Default setting"));

   e_widget_table_object_append(o, of, 1, 0, 1, 2, 1, 1, 1, 1);

   return o;
}

static int
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   Photo_Item *pi;
   int ret;

   pi = cfdata->pi;
   ret = _common_apply_data(cfd, cfdata);

   if ( (pi->config->action_mouse_over = cfdata->action_mouse_over) ||
        (pi->config->action_mouse_left = cfdata->action_mouse_left) ||
        (pi->config->action_mouse_middle = cfdata->action_mouse_middle) )
     {
        pi->config->action_mouse_over = cfdata->action_mouse_over;
        pi->config->action_mouse_left = cfdata->action_mouse_left;
        pi->config->action_mouse_middle = cfdata->action_mouse_middle;

        photo_item_label_mode_set(pi);
     }

   photo_config_save();
   return ret;
}
