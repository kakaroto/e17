#include <e.h>
#include "e_mod_config.h"
#include "e_mod_main.h"
#include "e_mod_gadcon.h"

static void _config_updated(Config_Item *ci);

/* public functions */
EAPI Config_Item *
_weather_config_item_get(Instance *inst, const char *id) 
{
   Config_Item *ci;

   if (!id)
     {
        char buf[128];
	int  num = 0;

	/* Create id */
	if (weather_cfg->items)
	  {
	     const char *p;

	     ci = eina_list_last(weather_cfg->items)->data;
	     p = strrchr(ci->id, '.');
	     if (p) num = atoi(p + 1) + 1;
	  }
	snprintf(buf, sizeof(buf), "%s.%d", _gc_name(), num);
	id = buf;
     }
   else
     {
        Eina_List *l;

        EINA_LIST_FOREACH(weather_cfg->items, l, ci)
	  {
	     if (!ci->id) continue;
	     if (!strcmp(ci->id, id)) 
             {
                ci->inst = inst;
                return ci;
             }
	  }
     }
   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->celcius = 0;
   ci->location = eina_stringshare_add("623164");
   ci->google = eina_stringshare_add("Paris France");
   ci->inst = inst;
   ci->plugin = eina_stringshare_add("yahoo");
   ci->poll_time = 30*60; //30 minutes
   weather_cfg->items = eina_list_append(weather_cfg->items, ci);

   return ci;
}




EAPI void 
_weather_config_new() 
{
   weather_cfg = E_NEW(Config, 1);
   weather_cfg->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) \
   if ((weather_cfg->version & 0xffff) < v) {
#define IFMODCFGEND }

   IFMODCFG(0x008d);
   _weather_config_item_get(NULL, NULL);
   IFMODCFGEND;

   weather_cfg->version = MOD_CONFIG_FILE_VERSION;

   /* set any config limits here with E_CONFIG_LIMIT */

   e_config_save_queue();
}

EAPI void 
_weather_config_free(void) 
{
   E_FREE(weather_cfg);
}



/****** Configuration dialog  */

struct _E_Config_Dialog_Data
{
    int celcius;
    char *location;
    char *google;
    int radio_plugin;
    double poll_time;
};
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata);
static Evas_Object * _basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

    E_Config_Dialog *
weather_config_dialog(Config_Item *ci)
{
    E_Config_Dialog *cfd = NULL;
    E_Config_Dialog_View *v = NULL;
    E_Container *con;
    char buf[4096];

    if (e_config_dialog_find("weather", "extensions/weather")) return NULL;

    v = E_NEW(E_Config_Dialog_View, 1);
    if (!v) return NULL;

    v->create_cfdata = _create_data;
    v->free_cfdata = _free_data;
    v->basic.create_widgets = _basic_create;
    v->basic.apply_cfdata = _basic_apply;
    v->override_auto_apply = 1;

    /* Icon in the theme */
    snprintf(buf, sizeof(buf), "%s/weather.edj", weather_cfg->mod_dir);

    /* create new config dialog */
    con = e_container_current_get(e_manager_current_get());
    cfd = e_config_dialog_new(con, D_("Weather Configuration"), "weather",
			      "extensions/weather", buf, 0, v, ci);

    return cfd;
}

/* Local Functions */
    static void *
_create_data(E_Config_Dialog *cfd)
{
    E_Config_Dialog_Data *cfdata = NULL;
    Config_Item *ci;

    ci = cfd->data;

    cfdata = E_NEW(E_Config_Dialog_Data, 1);
    _fill_data(ci, cfdata);
    return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
    if(cfdata->location) free(cfdata->location);
    if(cfdata->google) free(cfdata->google);
    E_FREE(cfdata);
}

    static void
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata)
{
    /* load a temp copy of the config variables */
    cfdata->celcius = ci->celcius;
    if(ci->location)
        cfdata->location = strdup(ci->location);
    if(ci->google)
        cfdata->google = strdup(ci->google);
    cfdata->radio_plugin = eweather_plugin_id_search(ci->inst->eweather, ci->plugin);
    if(cfdata->radio_plugin <0)
        cfdata->radio_plugin = 0;
    cfdata->poll_time = ci->poll_time / 60;
}

    static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
    Evas_Object *o = NULL, *of = NULL, *or = NULL, *ob = NULL, *oc = NULL;
    char buf[4096];
    E_Radio_Group *rg;
    Eina_Array_Iterator it;
    Eina_Array *array;
    Eina_Module *m;
    int i, id;
    Config_Item *ci;

    ci = cfd->data;

    o = e_widget_list_add(evas, 0, 0);

    of = e_widget_framelist_add(evas, D_("Poll Time"), 0);
    e_widget_framelist_content_align_set(of, 0.0, 0.0);

    ob = e_widget_slider_add(evas, 1, 0, D_("%2.0f minutes"), 15.0, 60.0, 1.0, 0,
            &(cfdata->poll_time), NULL, 40);
    e_widget_framelist_object_append(of, ob);

    e_widget_list_object_append(o, of, 1, 1, 0.5);



    of = e_widget_framelist_add(evas, D_("Select a data source"), 0);
    e_widget_framelist_content_align_set(of, 0.0, 0.0);

    array = eweather_plugins_list_get(ci->inst->eweather);

    rg = e_widget_radio_group_new(&(cfdata->radio_plugin));
    EINA_ARRAY_ITER_NEXT(array, i, m, it)
    {
        or = e_widget_radio_add(evas, eweather_plugin_name_get(ci->inst->eweather, i), i, rg);
        e_widget_framelist_object_append(of, or);
    }

    e_widget_list_object_append(o, of, 1, 1, 0.5);


    of = e_widget_framelist_add(evas, D_("Units Settings"), 0);
    e_widget_framelist_content_align_set(of, 0.0, 0.0);

    rg = e_widget_radio_group_new(&(cfdata->celcius));
    or = e_widget_radio_add(evas, D_("(°F) Fahrenheit"), 0, rg);
    e_widget_framelist_object_append(of, or);
    or = e_widget_radio_add(evas, D_("(°C) Celcius"), 1, rg);
    e_widget_framelist_object_append(of, or);

    e_widget_list_object_append(o, of, 1, 1, 0.5);

    
    id = eweather_plugin_id_search(ci->inst->eweather, "Yahoo");
    if(id >= 0)
    {
        of = e_widget_frametable_add(evas, D_("Yahoo Forecasts Code"), 0);

        oc = e_widget_image_add_from_file(evas, eweather_plugin_logo_get(ci->inst->eweather, id), 50, 25);
        e_widget_frametable_object_append(of, oc, 0, 0, 1, 1, 1, 0, 1, 0);

        ob = e_widget_label_add(evas, D_("Yahoo Forecasts Code/US Zip Code"));
        e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 0, 1, 0);
        ob = e_widget_entry_add(evas, &cfdata->location, NULL, NULL, NULL);
        e_widget_size_min_set(ob, 100, 1);
        e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 0, 1, 0);
        ob = e_widget_label_add(evas, D_("To find the code for your area, go to:"));
        e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 0, 1, 0);
        snprintf(buf, sizeof(buf), D_("%s, find your area, and look at the URL"), "http://weather.yahoo.com/");
        ob = e_widget_label_add(evas, buf);
        e_widget_frametable_object_append(of, ob, 0, 3, 2, 1, 1, 0, 1, 0);
        e_widget_list_object_append(o, of, 1, 1, 0.5);
    }

    id = eweather_plugin_id_search(ci->inst->eweather, "Google");
    if(id >= 0)
    {
        of = e_widget_frametable_add(evas, D_("Google Forecasts Code"), 0);

        oc = e_widget_image_add_from_file(evas, eweather_plugin_logo_get(ci->inst->eweather, id), 50, 25);
        e_widget_frametable_object_append(of, oc, 0, 0, 1, 1, 1, 0, 1, 0);

        ob = e_widget_label_add(evas, D_("Google Forecasts City Code"));
        e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 0, 1, 0);
        ob = e_widget_entry_add(evas, &cfdata->google, NULL, NULL, NULL);
        e_widget_size_min_set(ob, 100, 1);
        e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 0, 1, 0);
        ob = e_widget_label_add(evas, D_("Specify the name of your city"));
        e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 0, 1, 0);
        snprintf(buf, sizeof(buf), D_(" with extra information like the state or the country"));
        ob = e_widget_label_add(evas, buf);
        e_widget_frametable_object_append(of, ob, 0, 3, 2, 1, 1, 0, 1, 0);
        e_widget_list_object_append(o, of, 1, 1, 0.5);
    }


    int mw,mh;
    e_widget_size_min_get(o, &mw, &mh);
    e_dialog_content_set((E_Dialog*)cfd, o, mw, mh);

    return o;
}

    static int
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
    Config_Item *ci;

    ci = cfd->data;

    ci->celcius = cfdata->celcius;
    if(ci->location) eina_stringshare_del(ci->location);
    ci->location = eina_stringshare_add(cfdata->location);
    if(ci->google) eina_stringshare_del(ci->google);
    ci->google = eina_stringshare_add(cfdata->google);
    ci->poll_time = cfdata->poll_time*60;

    
    if(ci->plugin) eina_stringshare_del(ci->plugin);
    ci->plugin = eina_stringshare_add(eweather_plugin_name_get(ci->inst->eweather, cfdata->radio_plugin));

    e_config_save_queue();
   
    _config_updated(ci);

    return 1;
}

static void
_config_updated(Config_Item *ci)
{
    EWeather *eweather;
    if(!ci->inst->eweather) return ;

    eweather = ci->inst->eweather;

    eweather_plugin_byname_set(eweather, ci->plugin);
    eweather_poll_time_set(eweather, ci->poll_time);
    if(!strcmp(ci->plugin, "Yahoo"))
        eweather_code_set(eweather, ci->location);
    else
        eweather_code_set(eweather, ci->google);
    if(ci->celcius)
       eweather_temp_type_set(eweather, EWEATHER_TEMP_CELCIUS);
    else
        eweather_temp_type_set(eweather, EWEATHER_TEMP_FARENHEIT);
}

