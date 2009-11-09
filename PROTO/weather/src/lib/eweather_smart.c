// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "EWeather_Smart.h"

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
   EWeather *eweather; 

   Evas_Object *obj; //the edje object
   Eina_List *objs; //1 edje object per day
   EWeather_Object_Mode mode;
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

static void
_mouse_down_cb(void *data, Evas *evas, Evas_Object *o_day, void *event)
{
   Evas_Object *obj = data;
   Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down*) event;
   Evas_Object *o;
   int i;
   char buf[1024];
   Eina_List *l;
   Smart_Data *sd = evas_object_smart_data_get(obj);
   if (!sd) return;
	
   if(ev->button != 1) return ;

   i = 1; 
   EINA_LIST_FOREACH(sd->objs, l, o)
     {
	if(o == o_day)
	  break;
	else
	  i++;
     }

   if(o && sd->mode == EWEATHER_OBJECT_MODE_EXPOSE)
     {
	snprintf(buf, sizeof(buf), "show,%d", i);
	edje_object_signal_emit(sd->obj, buf, "");

	sd->mode = EWEATHER_OBJECT_MODE_NORMAL;
     }
   else if(o)
     {
	if( ev->flags & EVAS_BUTTON_DOUBLE_CLICK)
	  {
	     edje_object_signal_emit(sd->obj, "expose", "");
	     sd->mode = EWEATHER_OBJECT_MODE_EXPOSE;
	  }
	else
	  {
	     Evas_Coord x = ev->output.x;
	     Evas_Coord x2,w2;
	     evas_object_geometry_get(sd->obj, &x2, NULL, &w2, NULL);

	     if(x < x2+w2/2 && i>1)
	       {	
		  snprintf(buf, sizeof(buf), "show,%d", i-1);
		  edje_object_signal_emit(sd->obj, buf, "");
	       }
	     else if(x > x2+w2/2 && i<3)
	       {	
		  snprintf(buf, sizeof(buf), "show,%d", i+1);
		  edje_object_signal_emit(sd->obj, buf, "");
	       }
	  }
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

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   for(i=0; i<eweather_data_count(sd->eweather); i++)
     {
	EWeather_Data *e_data = eweather_data_get(sd->eweather, i);
	
	o_day = eina_list_nth(sd->objs, i);
	if(!o_day)
	  {
	     o_day = edje_object_add(evas_object_evas_get(obj));
	     edje_object_file_set(o_day, PACKAGE_DATA_DIR"/theme.edj", "weather");
	     evas_object_smart_member_add(o_day, obj);	
	     evas_object_show(o_day);
	     snprintf(buf, sizeof(buf), "object.swallow.%d", i+1);
	     edje_object_part_swallow(sd->obj, buf, o_day);
	     sd->objs = eina_list_append(sd->objs, o_day);

	     evas_object_event_callback_add(o_day, EVAS_CALLBACK_MOUSE_DOWN,
		   _mouse_down_cb, obj);
	  }
	signal = eweather_object_signal_type_get(eweather_data_type_get(e_data));

	edje_object_signal_emit(o_day, signal, "");

	snprintf(buf, sizeof(buf), "%d°F", eweather_data_temp_get(e_data));
	edje_object_part_text_set(o_day, "text.temp", buf);

	snprintf(buf, sizeof(buf), "%d°F", eweather_data_temp_min_get(e_data));
	edje_object_part_text_set(o_day, "text.temp_min", buf);

	snprintf(buf, sizeof(buf), "%d°F", eweather_data_temp_max_get(e_data));
	edje_object_part_text_set(o_day, "text.temp_max", buf);

	edje_object_part_text_set(o_day, "text.city", eweather_data_city_get(e_data));
	edje_object_part_text_set(o_day, "text.date", eweather_data_date_get(e_data));
     }
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

   sd = calloc(1, sizeof(Smart_Data));
   if (!sd) return;
   evas_object_smart_data_set(obj, sd);

   sd->mode = EWEATHER_OBJECT_MODE_EXPOSE;

   sd->obj = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->obj, PACKAGE_DATA_DIR"/theme.edj", "main");
   evas_object_smart_member_add(sd->obj, obj);
	
   sd->eweather = eweather_new();
   eweather_callbacks_set(sd->eweather, _eweather_update_cb, obj);

   Eina_List *l = eweather_plugins_list_get();
   char *s, *plugin = NULL;
   //select the first plugin
   EINA_LIST_FREE(l, s)
     {
	if(!plugin)
	  plugin = s;
	else
	  free(s);
     }

   if(plugin)
     {
	//eweather_plugin_set(sd->eweather, plugin);
	eweather_plugin_set(sd->eweather, "test.so");
	free(plugin);
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

   free(sd);
}

   static void
_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_move(sd->obj, x, y);
}

   static void
_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_resize(sd->obj,w,h);
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

