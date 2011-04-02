#include "EWeather_Smart.h"
#include <math.h>
#include <string.h>

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
   EWeather *eweather;

   Evas_Object *obj; //the edje object
   Eina_List *objs; //1 edje object per day
   Evas_Object *main; //main weather object
   int current_day;
   EWeather_Object_Mode mode;
   const char *theme; //eina stringshare
   const char *farenheit_format; //eina stringshare
   const char *celcius_format; // eina stringshare

   struct
     {
	Eina_Bool is : 1;
	Eina_Bool moved : 1;
	Evas_Coord x, y;
     } thumbscroll;
};

#define E_SMART_OBJ_GET_RETURN(smart, o, type, ret) \
{ \
   char *_e_smart_str; \
   \
   if (!o) return ret; \
   smart = evas_object_smart_data_get(o); \
   if (!smart) return ret; \
   _e_smart_str = (char *)evas_object_type_get(o); \
   if (!_e_smart_str) return ret; \
   if (strcmp(_e_smart_str, type)) return ret; \
}

#define E_SMART_OBJ_GET(smart, o, type) \
{ \
   char *_e_smart_str; \
   \
   if (!o) return; \
   smart = evas_object_smart_data_get(o); \
   if (!smart) return; \
   _e_smart_str = (char *)evas_object_type_get(o); \
   if (!_e_smart_str) return; \
   if (strcmp(_e_smart_str, type)) return; \
}

#define E_OBJ_NAME "eweather_object"
static Evas_Smart  *smart = NULL;

static void _smart_init(void);
static void _smart_add(Evas_Object * obj);
static void _smart_del(Evas_Object * obj);
static void _smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object * obj);
static void _smart_hide(Evas_Object * obj);
static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object * obj);
static void _sizing_eval(Evas_Object *obj);
static void update_main(Evas_Object *obj);

static void _eweather_update_cb(void *data, EWeather *eweather);


struct EWeather_Type_Signal
{
   EWeather_Type type;
   const char *signal;
};

static struct EWeather_Type_Signal _tab[] =
{
     {EWEATHER_TYPE_UNKNOWN, "unknown"},
     {EWEATHER_TYPE_WINDY, "right,day_clear,sun,isolated_cloud,windy"},
     {EWEATHER_TYPE_RAIN, "right,day_rain,sun,rain,rain"},
     {EWEATHER_TYPE_SNOW, "right,day_rain,sun,rain,snow"},
     {EWEATHER_TYPE_RAIN_SNOW, "right,day_rain,sun,rain,rain_snow"},
     {EWEATHER_TYPE_FOGGY, "right,day_rain,sun,cloud,foggy"},
     {EWEATHER_TYPE_CLOUDY, "right,day_clear,sun,cloud,"},
     {EWEATHER_TYPE_MOSTLY_CLOUDY_NIGHT, "right,night_clear,moon,cloud,"},
     {EWEATHER_TYPE_MOSTLY_CLOUDY_DAY, "right,day_clear,sun,cloud,"},
     {EWEATHER_TYPE_PARTLY_CLOUDY_NIGHT, "right,night_clear,moon,isolated_cloud,"},
     {EWEATHER_TYPE_PARTLY_CLOUDY_DAY, "right,day_clear,sun,isolated_cloud,"},
     {EWEATHER_TYPE_CLEAR_NIGHT, "right,night_clear,moon,nothing,"},
     {EWEATHER_TYPE_SUNNY, "right,day_clear,sun,nothing,"},
     {EWEATHER_TYPE_ISOLATED_THUNDERSTORMS, "right,day_heavyrain,sun,isolated_tstorm,rain"},
     {EWEATHER_TYPE_THUNDERSTORMS, "right,day_heavyrain,sun,tstorm,rain"},
     {EWEATHER_TYPE_SCATTERED_THUNDERSTORMS, "right,day_heavyrain,sun,tstorm,rain"},
     {EWEATHER_TYPE_HEAVY_SNOW, "right,day_heavyrain,sun,storm,snow"}
};


const char *eweather_object_signal_type_get(EWeather_Type type)
{
   int i;
   for (i = 0; i < sizeof (_tab) / sizeof (struct EWeather_Type_Signal); ++i)
     if (_tab[i].type == type)
       {
	  return _tab[i].signal;
       }

   return "";
}


Evas_Object *eweather_object_add(Evas *evas)
{
   _smart_init();
   return evas_object_smart_add(evas, smart);
}


EWeather *eweather_object_eweather_get(Evas_Object *obj)
{
   Smart_Data *sd ;

   E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, NULL);
   return sd->eweather;
}

void eweather_theme_set(Evas_Object *obj, const char *theme)
{
    Eina_List *l;
    Evas_Object *o, *_obj;
    Smart_Data *sd = evas_object_smart_data_get(obj);
    if(!sd) return ;

    eina_stringshare_del(sd->theme);
    sd->theme = eina_stringshare_add(theme);

    edje_object_file_set(sd->obj, sd->theme, "main");
    edje_object_file_set(sd->main, sd->theme, "weather");
    EINA_LIST_FOREACH(sd->objs, l, _obj)
        edje_object_file_set(_obj, sd->theme, "weather");

    int mode = sd->mode;
    sd->mode = -1; //No mode
    eweather_object_mode_set(obj, mode);

    _eweather_update_cb(obj, sd->eweather);
}

void eweather_object_temp_format_set(Evas_Object *obj, EWeather_Temp type, const char *format)
{
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if(!sd) return;
   if (type == EWEATHER_TEMP_FARENHEIT)
     eina_stringshare_replace(&sd->farenheit_format, format);
   else
     eina_stringshare_replace(&sd->celcius_format, format);
}

void eweather_object_mode_set(Evas_Object *obj, EWeather_Object_Mode mode)
{
   Eina_List *l;
   Evas_Object *o;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if(!sd) return ;

   if(mode == sd->mode) return;

   sd->mode = mode;

   if(sd->mode == EWEATHER_OBJECT_MODE_FULLSCREEN)
     edje_object_signal_emit(sd->obj, "fullscreen", "");
   else
     {
	edje_object_signal_emit(sd->obj, "expose", "");
	EINA_LIST_FOREACH(sd->objs, l, o)
	  {
	     evas_object_show(o);
	  }
     }

   _sizing_eval(obj);
}

   static void
_fullscreen_done_cb(void *data, Evas_Object *_obj, const char *emission, const char *source)
{
   Evas_Object *obj = data;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   Eina_List *l;
   Evas_Object *o;
   if (!sd) return;

   EINA_LIST_FOREACH(sd->objs, l, o)
     {
	evas_object_hide(o);
     }
}

   static void
_mouse_up_cb(void *data, Evas *evas, Evas_Object *o_day, void *event)
{
   Evas_Object *obj = data;
   Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down*) event;
   Evas_Object *o;
   int i;
   Eina_List *l;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if(ev->button != 1) return ;
   sd->thumbscroll.moved = EINA_TRUE;
   if(sd->thumbscroll.is)
     {
	sd->thumbscroll.is = EINA_FALSE;
	return ;
     }


   i = 0;
   EINA_LIST_FOREACH(sd->objs, l, o)
     {
	if(o == o_day)
	  break;
	else
	  i++;
     }
   if(o && sd->mode == EWEATHER_OBJECT_MODE_EXPOSE)
     {
	sd->current_day = i;
	update_main(obj);
     }

   if(o && sd->mode == EWEATHER_OBJECT_MODE_EXPOSE)
     {
	eweather_object_mode_set(obj, EWEATHER_OBJECT_MODE_FULLSCREEN);
     }
   else
     {
	eweather_object_mode_set(obj, EWEATHER_OBJECT_MODE_EXPOSE);
     }
}

   static void
_mouse_down_cb(void *data, Evas *evas, Evas_Object *o_day, void *event)
{
   Evas_Object *obj = data;
   Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down*) event;
   Smart_Data *sd = evas_object_smart_data_get(obj);

   if (!sd) return;
   if(ev->button != 1) return ;

   sd->thumbscroll.moved = EINA_FALSE;

   sd->thumbscroll.x = ev->canvas.x;
   sd->thumbscroll.y = ev->canvas.y;
}

   static void
_mouse_move_cb(void *data, Evas *evas, Evas_Object *o_day, void *event)
{
   Evas_Object *obj = data;
   Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*) event;
   Evas_Coord x, y;

   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if(sd->thumbscroll.moved)
     return ;

   sd->thumbscroll.is = EINA_TRUE;

   x = ev->cur.canvas.x;
   y = ev->cur.canvas.y;

   if(x - sd->thumbscroll.x > 60)
     {
	if(sd->current_day > 0)
	  {
	     sd->current_day--;
	     update_main(obj);
	  }
	sd->thumbscroll.moved = EINA_TRUE;
     }
   else if(x - sd->thumbscroll.x < -60)
     {
	if(sd->current_day < eina_list_count(sd->objs)-1)
	  {
	     sd->current_day++;
	     update_main(obj);
	  }
	sd->thumbscroll.moved = EINA_TRUE;
     }
}

static void _eweather_update_cb(void *data, EWeather *eweather)
{
   Evas_Object *obj = data;
   Smart_Data *sd;
   const char *signal;
   char buf[1024];
   Evas_Object *o_day;
   int i = 0;
   const char *ff;
   const char *cf;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   ff = sd->farenheit_format;
   cf = sd->celcius_format;
   if(sd->current_day >= eweather_data_count(sd->eweather))
     sd->current_day = -1;

   if (sd->eweather)
   for(i=0; i<eweather_data_count(sd->eweather); i++)
     {
	EWeather_Data *e_data = eweather_data_get(sd->eweather, i);

	if(sd->current_day<0)
	  sd->current_day = i;

	o_day = eina_list_nth(sd->objs, i);
	if(!o_day)
	  {
	     o_day = edje_object_add(evas_object_evas_get(obj));
	     edje_object_file_set(o_day, sd->theme, "weather");
	     evas_object_smart_member_add(o_day, obj);
	     evas_object_show(o_day);
	     sd->objs = eina_list_append(sd->objs, o_day);

	     evas_object_event_callback_add(o_day, EVAS_CALLBACK_MOUSE_UP,
		   _mouse_up_cb, obj);

	     if(sd->mode == EWEATHER_OBJECT_MODE_FULLSCREEN)
	       evas_object_hide(o_day);
	  }

        const Evas_Object *content = edje_object_part_object_get(sd->obj, "object.content");
	evas_object_clip_set(o_day, (Evas_Object *)content);


	signal = eweather_object_signal_type_get(eweather_data_type_get(e_data));

	edje_object_signal_emit(o_day, signal, "");

	if(eweather_temp_type_get(eweather) == EWEATHER_TEMP_FARENHEIT)
	  snprintf(buf, sizeof(buf), ff, eweather_data_temp_get(e_data));
	else
	  snprintf(buf, sizeof(buf), cf, eweather_utils_celcius_get(eweather_data_temp_get(e_data)));

	edje_object_part_text_set(o_day, "text.temp", buf);

	if(eweather_temp_type_get(eweather) == EWEATHER_TEMP_FARENHEIT)
	  snprintf(buf, sizeof(buf), ff, eweather_data_temp_min_get(e_data));
	else
	  snprintf(buf, sizeof(buf), cf, eweather_utils_celcius_get(eweather_data_temp_min_get(e_data)));

	edje_object_part_text_set(o_day, "text.temp_min", buf);

	if(eweather_temp_type_get(eweather) == EWEATHER_TEMP_FARENHEIT)
	  snprintf(buf, sizeof(buf), ff, eweather_data_temp_max_get(e_data));
	else
	  snprintf(buf, sizeof(buf), cf, eweather_utils_celcius_get(eweather_data_temp_max_get(e_data)));

	edje_object_part_text_set(o_day, "text.temp_max", buf);

	edje_object_part_text_set(o_day, "text.city", eweather_data_city_get(e_data));
	edje_object_part_text_set(o_day, "text.date", eweather_data_date_get(e_data));
     }

   while(eina_list_count(sd->objs) > eweather_data_count(sd->eweather))
     {
	Evas_Object *o = eina_list_data_get(eina_list_last(sd->objs));
	sd->objs = eina_list_remove(sd->objs, o);
	evas_object_del(o);
     }

   update_main(obj);
   _sizing_eval(obj);
}

static void update_main(Evas_Object *obj)
{
   Smart_Data *sd;
   const char *signal;
   char buf[1024];
   EWeather *eweather;
   const char *ff;
   const char *cf;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   ff = sd->farenheit_format;
   cf = sd->celcius_format;
   if(sd->current_day < 0) return ;

   eweather = sd->eweather;

   evas_object_show(sd->main);
   edje_object_part_swallow(sd->obj, "object.swallow", sd->main);

   EWeather_Data *e_data = eweather_data_get(sd->eweather, sd->current_day);

   signal = eweather_object_signal_type_get(eweather_data_type_get(e_data));

   edje_object_signal_emit(sd->main, signal, "");

   if(eweather_temp_type_get(eweather) == EWEATHER_TEMP_FARENHEIT)
     snprintf(buf, sizeof(buf), ff, eweather_data_temp_get(e_data));
   else
     snprintf(buf, sizeof(buf), cf, eweather_utils_celcius_get(eweather_data_temp_get(e_data)));

   edje_object_part_text_set(sd->main, "text.temp", buf);

   if(eweather_temp_type_get(eweather) == EWEATHER_TEMP_FARENHEIT)
     snprintf(buf, sizeof(buf), ff, eweather_data_temp_min_get(e_data));
   else
     snprintf(buf, sizeof(buf), cf, eweather_utils_celcius_get(eweather_data_temp_min_get(e_data)));

   edje_object_part_text_set(sd->main, "text.temp_min", buf);

   if(eweather_temp_type_get(eweather) == EWEATHER_TEMP_FARENHEIT)
     snprintf(buf, sizeof(buf), ff, eweather_data_temp_max_get(e_data));
   else
     snprintf(buf, sizeof(buf), cf, eweather_utils_celcius_get(eweather_data_temp_max_get(e_data)));

   edje_object_part_text_set(sd->main, "text.temp_max", buf);

   edje_object_part_text_set(sd->main, "text.city", eweather_data_city_get(e_data));
   edje_object_part_text_set(sd->main, "text.date", eweather_data_date_get(e_data));
}

static void _sizing_eval(Evas_Object *obj)
{
   Smart_Data *sd;
   int i, j;
   int x, y, w, h;
   int col,line;
   int w_inter, h_inter;
   int w_size, h_size;
   Eina_List *l;
   const Evas_Object *content;
   Evas_Coord wmin, hmin;
   double ratiow, ratioh, ratio;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if(eina_list_count(sd->objs) <= 0) return ;

   content = edje_object_part_object_get(sd->obj, "object.content");
   evas_object_geometry_get(content, &x, &y, &w, &h);

   edje_object_size_min_get(eina_list_data_get(sd->objs), &wmin, &hmin);

   double rac = sqrt(eina_list_count(sd->objs));
   col = rac;
   if(rac > col) col++;

   line = eina_list_count(sd->objs) / col;
   if(eina_list_count(sd->objs) % col) line++;


   l = sd->objs;

   if(col > 0)
     w_size = (w-(col-1)*5)/col;
   if(line > 0)
     h_size = (h-(line-1)*5)/line;

   ratiow = w_size / (double)wmin;
   ratioh = h_size / (double)hmin;
   ratio = ratiow;
   if(ratiow>ratioh) ratio = ratioh;

   w_size = wmin * ratio;
   h_size = hmin * ratio;


   if(col > 0)
     w_inter = (w - w_size*col) / (col+1);
   if(line > 0)
     h_inter = (h - h_size*line) / (line+1);

   x+=w_inter;
   y+=h_inter;

   for(j=0; j<line; j++)
     {
	for(i=0; i<col && i*j<eina_list_count(sd->objs); i++)
	  {
	     Evas_Object *o = eina_list_data_get(l);
	     l = eina_list_next(l);

	     evas_object_move(o, x + i*(w_size + w_inter), y + j*(h_size + h_inter));
	     evas_object_resize(o, w_size, h_size);

	     edje_object_scale_set(o, ratio);
	  }
     }

   //main object
   evas_object_geometry_get(sd->main, NULL, NULL, &w_size, &h_size);
   ratiow = w_size / (double)wmin;
   ratioh = h_size / (double)hmin;

   ratio = ratiow;
   if(ratiow>ratioh) ratio = ratioh;
   edje_object_scale_set(sd->main, ratio);
}

/*******************************************/
/* Internal smart object required routines */
/*******************************************/
   static void
_smart_init(void)
{
   if (smart) return;
     {
	static const Evas_Smart_Class sc =
	  {
	     E_OBJ_NAME,
	     EVAS_SMART_CLASS_VERSION,
	     _smart_add,
	     _smart_del,
	     _smart_move,
	     _smart_resize,
	     _smart_show,
	     _smart_hide,
	     NULL,
	     _smart_clip_set,
	     _smart_clip_unset,
	     NULL,
	     NULL,
	     NULL,
	     NULL
	  };
	smart = evas_smart_class_new(&sc);
     }
}

   static void
_smart_add(Evas_Object * obj)
{
   Smart_Data *sd;
   Eina_Array_Iterator it;
   Eina_Array *array;
   Eina_Module *m;
   int i;

   sd = calloc(1, sizeof(Smart_Data));
   if (!sd) return;
   evas_object_smart_data_set(obj, sd);

   sd->theme = eina_stringshare_add(PACKAGE_DATA_DIR"/default/theme.edj");
   sd->farenheit_format = eina_stringshare_add("%.1f°F");
   sd->celcius_format = eina_stringshare_add("%.1f°C");
   sd->mode = EWEATHER_OBJECT_MODE_EXPOSE;
   sd->thumbscroll.moved = EINA_TRUE;

   sd->obj = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->obj, sd->theme, "main");
   evas_object_smart_member_add(sd->obj, obj);

   sd->main = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->main, sd->theme, "weather");
   evas_object_smart_member_add(sd->main, obj);

   sd->eweather = eweather_new();
   eweather_callbacks_set(sd->eweather, _eweather_update_cb, obj);

   evas_object_event_callback_add(sd->main, EVAS_CALLBACK_MOUSE_UP,
	 _mouse_up_cb, obj);
   evas_object_event_callback_add(sd->main, EVAS_CALLBACK_MOUSE_DOWN,
	 _mouse_down_cb, obj);
   evas_object_event_callback_add(sd->main, EVAS_CALLBACK_MOUSE_MOVE,
	 _mouse_move_cb, obj);

   edje_object_signal_callback_add(sd->obj, "fullscreen,done", "", _fullscreen_done_cb, obj);

   eweather_object_mode_set(obj, EWEATHER_OBJECT_MODE_FULLSCREEN);
   array = eweather_plugins_list_get(sd->eweather);

   EINA_ARRAY_ITER_NEXT(array, i, m, it)
     {
	eweather_plugin_set(sd->eweather, m);
	break;
     }
}

   static void
_smart_del(Evas_Object * obj)
{
   Smart_Data *sd;
   Evas_Object *o;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   eweather_free(sd->eweather);

   EINA_LIST_FREE(sd->objs, o)
      evas_object_del(o);
   evas_object_del(sd->obj);
   evas_object_del(sd->main);
   eina_stringshare_del(sd->theme);

   free(sd);
}

   static void
_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_move(sd->obj, x, y);
   _sizing_eval(obj);
}

   static void
_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_resize(sd->obj,w,h);
   _sizing_eval(obj);
}

   static void
_smart_show(Evas_Object * obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_show(sd->obj);
}

   static void
_smart_hide(Evas_Object * obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_hide(sd->obj);
}

   static void
_smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_clip_set(sd->obj, clip);
}

   static void
_smart_clip_unset(Evas_Object * obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_clip_unset(sd->obj);
}

