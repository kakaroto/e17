/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <limits.h>
#include <time.h>
#include <e.h>
#include <e_module.h>
#include "e_mod_main.h"
#include "CalcEphem.h"

/******************************************************************************/
/* 
 * Gadcon requirements
 */

static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void             _gc_shutdown(E_Gadcon_Client *gcc);
static void             _gc_orient(E_Gadcon_Client *gcc);
static char            *_gc_label(void);
static Evas_Object     *_gc_icon(Evas *evas);
static const char      *_gc_id_new(void);

static const E_Gadcon_Client_Class _gadcon_class =
{
   GADCON_CLIENT_CLASS_VERSION,
   "moon",
   {
      _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL
   },
   E_GADCON_CLIENT_STYLE_INSET
};

/******************************************************************************/

/* Moon module privates */

typedef struct _Instance Instance;
typedef struct _Moon_Timer Moon_Timer;

typedef Edje_Message_String_Float_Set Moon_Update_Msg;
typedef Edje_Message_Int_Set          Moon_Config_Msg;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object     *o_gadget;
};

struct _Moon_Timer
{
   Ecore_Timer *timer;
   Eina_List   *clients;
};

static void          _button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info); 
static void          _menu_cb_post(void *data, E_Menu *m);
static void          _moon_inst_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi);

static void          _moon_timer_init();
static void          _moon_timer_shutdown();
static void          _moon_timer_client_register(Evas_Object *moon);
static void          _moon_timer_client_unregister(Evas_Object *moon);
static int           _moon_timer_cb_update(void *data);

static Moon_Config_Msg *_moon_config_msg_prepare();
static void             _moon_config_msg_send(Evas_Object *o, Moon_Config_Msg *msg);
static void             _moon_config_msg_free(Moon_Config_Msg *msg);

static Moon_Update_Msg *_moon_update_msg_prepare();
static void             _moon_update_msg_send(Evas_Object *o, Moon_Update_Msg *msg);
static void             _moon_update_msg_free(Moon_Update_Msg *msg);

static double        _moon_phase_calc();
static char         *_moon_display_string_get(Config *conf, double moonphase);

#ifdef DEBUG
static void          _moon_face_signal_cb(void *data, Evas_Object *o, const char *sig, const char *src);
static double _test_val;
#endif

/* 
 * Moon module global data
 */
static Moon_Timer *_moon_timer = NULL;
static E_Module   *_module     = NULL;

/******************************************************************************/

EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
   "Moon"
};

EAPI void *
e_modapi_init(E_Module *module)
{
   char buf[4096];

   /* Set up module message catalog */
   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(module));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   moon_config_init(module);

   _moon_timer_init();
   _module = module;
   e_gadcon_provider_register(&_gadcon_class);
   return module;            // dummy return to keep the API happy
}

EAPI int
e_modapi_shutdown(E_Module *module)
{  
   e_gadcon_provider_unregister(&_gadcon_class);
   _module = NULL;
   _moon_timer_shutdown();

   moon_config_shutdown();

   return 1;
}

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;
   char edje_path[PATH_MAX];
   Moon_Update_Msg *umsg;
   Moon_Config_Msg *cmsg;

#ifdef DEBUG
   printf("----------------------\n");
   printf("name is %s\n", name);
   printf("id is %s\n", id);
   printf("style is %s\n", style);
   printf("----------------------\n");
#endif
   inst = E_NEW(Instance, 1);

   o = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(o, "base/theme/modules/moon",
				"module/moon/main"))
     {
	snprintf(edje_path, sizeof(edje_path), "%s/moon.edj", 
		 e_module_dir_get(_module));
	edje_object_file_set(o, edje_path, "module/moon/main");
     }
   evas_object_show(o);

   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;

   inst->gcc = gcc;
   inst->o_gadget = o;

   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
	                                   _button_cb_mouse_down, inst);

   /* Configure this instance */
   cmsg = _moon_config_msg_prepare();
   _moon_config_msg_send(o, cmsg);
   _moon_config_msg_free(cmsg);

   /* Send this instance the display content */
   umsg = _moon_update_msg_prepare();
   _moon_update_msg_send(o, umsg);
   _moon_update_msg_free(umsg);

   /* Register the new moon timed display content updates */
   _moon_timer_client_register(o);

   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;

   _moon_timer_client_unregister(inst->o_gadget);

   evas_object_del(inst->o_gadget);
   free(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc)
{
   Instance *inst;
   Evas_Coord w, h;

   inst = gcc->data;
   e_gadcon_client_aspect_set(gcc, 1, 1);
   edje_object_size_min_get(inst->o_gadget, &w, &h);
   e_gadcon_client_min_size_set(gcc, w, h);
}

static char *
_gc_label(void)
{
   return D_("Moon");
}

static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-moon.edj", e_module_dir_get(_module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(void)
{
   return _gadcon_class.name;
}


static void
_button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;

   inst = data;
   ev = event_info;
   if ((ev->button == 3) && (!moon_config->menu))
     {
	E_Menu *mn;
	E_Menu_Item *mi;
	int cx, cy, cw, ch;

	mn = e_menu_new();
	e_menu_post_deactivate_callback_set(mn, _menu_cb_post, inst);
	moon_config->menu = mn;
	
	mi = e_menu_item_new(mn); e_menu_item_label_set(mi, D_("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _moon_inst_cb_menu_configure, obj); 
	
	e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
	e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, 
					  &cx, &cy, &cw, &ch);
	e_menu_activate_mouse(mn, 
			      e_util_zone_current_get(e_manager_current_get()),
			      cx + ev->output.x, cy + ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button, 
	      EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

static void
_menu_cb_post(void *data, E_Menu *m)
{
   if (!moon_config->menu) return;
   e_object_del(E_OBJECT(moon_config->menu));
   moon_config->menu = NULL;
}

static void
_moon_inst_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Evas_Object *o;

   o = data;
   if (!moon_config->menu) return;
   if (moon_config->config_dialog) return;
   moon_config_dialog_show(o);
}


static void
_moon_timer_init()
{
   if (!_moon_timer)
     {
	_moon_timer = E_NEW(Moon_Timer, 1);
	_moon_timer->timer = ecore_timer_add(60.0, _moon_timer_cb_update, NULL);
	_moon_timer->clients = NULL;
     }
}

static void
_moon_timer_shutdown()
{
   ecore_timer_del(_moon_timer->timer);
   if (_moon_timer->clients)
     printf("Moon module leak on shutdown!\n");
   free(_moon_timer); 
   _moon_timer = NULL;
}

static void
_moon_timer_client_register(Evas_Object *moon)
{
   _moon_timer->clients = eina_list_append(_moon_timer->clients, moon);
}

static void
_moon_timer_client_unregister(Evas_Object *moon)
{
   _moon_timer->clients = eina_list_remove(_moon_timer->clients, moon);
}

/* Timer event - update all moon objects */
static int
_moon_timer_cb_update(void *data)
{
   Moon_Update_Msg *msg;
   Eina_List *l;

   if (!_moon_timer->clients)
     return 1;
 
   msg = _moon_update_msg_prepare();
   for (l = _moon_timer->clients; l; l = l->next)
     {
	Evas_Object *gadget;
	gadget = l->data;
	_moon_update_msg_send(gadget, msg);
     }
   _moon_update_msg_free(msg);
   return 1;
}

/* Update the specified instance with the latest configuration */
void
moon_reset(Evas_Object *o)
{ 
   Moon_Config_Msg *cmsg;
   Moon_Update_Msg *umsg;

   cmsg = _moon_config_msg_prepare();
   umsg = _moon_update_msg_prepare();
   if (o)
     {
	_moon_config_msg_send(o, cmsg);
        _moon_update_msg_send(o, umsg);
     }
   else
     {
	// FIXME
	printf("Moon module bug: moon_config_reset wants a valid object\n");
     }
   _moon_config_msg_free(cmsg);
   _moon_update_msg_free(umsg);
}


static Moon_Update_Msg *
_moon_update_msg_prepare()
{
   Edje_Message_String_Float_Set *msg;
   char *str;

   msg = E_NEW(Edje_Message_String_Float_Set, 1);
   msg->count = 1;
   msg->val[0] = _moon_phase_calc();
#ifdef DEBUG
   msg->val[0] = _test_val;
   _test_val = _test_val + (1.0 / 180.0);
   if (_test_val > 1.0) 
      _test_val = 0.0;
#endif
   str = _moon_display_string_get(moon_config, msg->val[0]);
   msg->str = str;
   return (Moon_Update_Msg *) msg;
}

static void
_moon_update_msg_send(Evas_Object *o, Moon_Update_Msg *msg)
{
   edje_object_message_send(o, 
	 EDJE_MESSAGE_STRING_FLOAT_SET, 
	 1, 
	 (Edje_Message_String_Float_Set *) msg);
}

static void
_moon_update_msg_free(Moon_Update_Msg *msg)
{
   free(msg->str);
   free(msg); 
}

static Moon_Config_Msg *
_moon_config_msg_prepare()
{
   Edje_Message_Int_Set *msg;

   msg = calloc(1, 
	        sizeof(Edje_Message_Int_Set) - 
		sizeof(int) +
		(3 * sizeof(int)));
   msg->count = 3;
   msg->val[0] = moon_config->show_phase_value;
   msg->val[1] = moon_config->show_border; // deprecated
   msg->val[2] = moon_config->show_cloud_anim;

   return (Moon_Config_Msg *)msg;
}

static void
_moon_config_msg_send(Evas_Object *o, Moon_Config_Msg *msg)
{
   edje_object_message_send(o, 
	 EDJE_MESSAGE_INT_SET, 
	 1, 
	 (Edje_Message_Int_Set *) msg);
}

static void
_moon_config_msg_free(Moon_Config_Msg *msg)
{
   free(msg);
}

static double
_moon_phase_calc()
{
   time_t current_time, date;
   struct tm *universal_time;
   double UT;
   CTrans c;
   
   current_time = time(NULL);
   if (current_time == -1) return 0; 

   /* Get Universal Time */
   universal_time = gmtime(&current_time);

   /* Date in YYYYMMDD format */
   date = (universal_time->tm_year + 1900) * 10000 + 
          (universal_time->tm_mon + 1) * 100 +
	  universal_time->tm_mday;

   /* UTC representation in decimal hours */ 
   UT = universal_time->tm_hour +
        universal_time->tm_min / 60.0 +
        universal_time->tm_sec / 3600.0;

   CalcEphem(date, UT, &c);
#ifdef DEBUG
   printf("date is %f\n", (double) date);
   printf("UT is %f\n", (double) UT);
   printf("MoonPhase is %f\n", c.MoonPhase);
   printf("MoonAge is %f\n", c.MoonAge);
   printf("Visible is %d\n", c.Visible);
#endif

   if (c.MoonPhase < 0.0) 
      return 0.0;
   if (c.MoonPhase > 1.0) 
      return 1.0;
   return c.MoonPhase; 
}

static char *
_moon_display_string_get(Config *conf, double phase)
{
   char strbuf[1024];
   int value_format;

   if (!conf)
     value_format = VALUE_FMT_STR;
   else
     value_format = conf->value_format;

   switch (value_format)
     {
      case VALUE_FMT_NUM:
	 if (phase <= 0.50)
	   snprintf(strbuf, sizeof(strbuf), "%.2f%%", phase * 200);
	 else if (phase > 0.50)
	   snprintf(strbuf, sizeof(strbuf), "%.2f%%", (0.50 - (phase - 0.50)) * 200);
	 break;
      case VALUE_FMT_STR:
	 // hmmmmm, something like this...
	 if (phase < 0.050)
	   snprintf(strbuf, sizeof(strbuf), "%s", D_("New Moon"));
	 else if ((phase >= 0.050) && (phase < 0.225))
	   snprintf(strbuf, sizeof(strbuf), "%s", D_("Waxing Crescent"));
	 else if ((phase >= 0.225) && (phase < 0.275))
	   snprintf(strbuf, sizeof(strbuf), "%s", D_("First Quarter"));
	 else if ((phase >= 0.275) && (phase < 0.475))
	   snprintf(strbuf, sizeof(strbuf), "%s", D_("Waxing Gibbous"));
	 else if ((phase >= 0.475) && (phase < 0.525))
	   snprintf(strbuf, sizeof(strbuf), "%s", D_("Full Moon"));
	 else if ((phase >= 0.525) && (phase < 0.725))
	   snprintf(strbuf, sizeof(strbuf), "%s", D_("Waning Gibbous"));
	 else if ((phase >= 0.725) && (phase < 0.775))
	   snprintf(strbuf, sizeof(strbuf), "%s", D_("Last Quarter"));
	 else if  (phase >= 0.775)
	   snprintf(strbuf, sizeof(strbuf), "%s", D_("Waning Crescent"));
	 break;
     }

   return strdup(strbuf);
}

