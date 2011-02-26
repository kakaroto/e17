#include <e.h>
#include "e_mod_main.h"
#include "e_mod_parse.h"

/* The typedef for this structure is declared inside the E code in order to
 * allow everybody to use this type, you dont need to declare the typedef, 
 * just use the E_Config_Dialog_Data for your data structures declarations */
struct _E_Config_Dialog_Data 
{
   int switch1;
};

/* Local Function Prototypes */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

/* External Functions */

/* Function for calling our personal dialog menu */
E_Config_Dialog *
e_int_config_xkbswitch_module(E_Container *con, const char *params) 
{
   E_Config_Dialog *cfd = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   /* is this config dialog already visible ? */
   if (e_config_dialog_find("XKB Switcher", "input/xkbswitch")) return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return NULL;

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.create_widgets = _basic_create;
   v->basic.apply_cfdata = _basic_apply;

   /* Icon in the theme */
   snprintf(buf, sizeof(buf), "%s/e-module-xkbswitch.edj", xkbswitch_conf->module->dir);

   /* create our config dialog */
   cfd = e_config_dialog_new(con, D_("XKB Switcher Module"), "XKB Switcher", 
                             "advanced/xkbswitch", buf, 0, v, NULL);

   e_dialog_resizable_set(cfd->dia, 1);
   xkbswitch_conf->cfd = cfd;
   return cfd;
}

/* Local Functions */
static void *
_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata = NULL;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(cfdata);
   return cfdata;
}

static void 
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   xkbswitch_conf->cfd = NULL;
   E_FREE(cfdata);
}

static void 
_fill_data(E_Config_Dialog_Data *cfdata) 
{
   /* load a temp copy of the config variables */
   cfdata->switch1 = xkbswitch_conf->switch1;
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o = NULL, *of = NULL, *ob = NULL, *oh = NULL, *off = NULL, *obb = NULL;
   Eina_List *l = NULL, *ll = NULL;
   e_xkb_model *m = NULL;
   e_xkb_layout *la = NULL;
   e_xkb_variant *v = NULL;

   parse_rules("/usr/local/share/X11/xkb/rules/xorg.xml");

   o = e_widget_list_add(evas, 0, 0);

   oh = e_widget_list_add(evas, 0, 1);
   of = e_widget_framelist_add(evas, D_("Available Layouts"), 1);
   ob = e_widget_ilist_add(evas, 32, 32, NULL);
   e_widget_size_min_set(ob, 220, 160);

   char buf[4096];
   EINA_LIST_FOREACH(layouts, l, la)
   {
       snprintf(buf, sizeof(buf), "%s (%s, %s)", la->name, la->short_descr, la->description);
       e_widget_ilist_append(ob, NULL, buf, NULL, NULL, la->name);
   }

   e_widget_ilist_go(ob);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(oh, of, 1, 1, 0.5);

   off = e_widget_framelist_add(evas, D_("Available Models"), 1);
   obb = e_widget_ilist_add(evas, 32, 32, NULL);
   e_widget_size_min_set(obb, 220, 160);

   EINA_LIST_FOREACH(models, l, m)
   {
       snprintf(buf, sizeof(buf), "%s (%s, %s)", m->name, m->description, m->vendor);
       e_widget_ilist_append(obb, NULL, buf, NULL, NULL, m->name);
   }

   e_widget_ilist_go(obb);
   e_widget_framelist_object_append(off, obb);
   e_widget_list_object_append(oh, off, 1, 1, 0.5);

   e_widget_list_object_append(o, oh, 1, 1, 0.5);


   /*of = e_widget_framelist_add(evas, D_("General"), 0);
   e_widget_framelist_content_align_set(of, 0.0, 0.0);
   ow = e_widget_check_add(evas, D_("Use Switch 1"), 
                           &(cfdata->switch1));
   e_widget_framelist_object_append(of, ow);
   e_widget_list_object_append(o, of, 1, 1, 0.5);*/

   EINA_LIST_FOREACH(layouts, l, la)
   {
       EINA_LIST_FOREACH(la->variants, ll, v)
          free(v);
       eina_list_free(la->variants);
       free(la);
   }
   eina_list_free(layouts);

   EINA_LIST_FOREACH(models, l, m)
      free(m);
   eina_list_free(models);

   return o;
}

static int 
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   xkbswitch_conf->switch1 = cfdata->switch1;
   e_config_save_queue();
   return 1;
}
