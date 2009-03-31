#include <e.h>
#include "e_mod_main.h"
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#if HAVE_LOCALE_H
#  include <locale.h>
#endif

/* Func Proto Requirements for Gadcon */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class);

/* Module Protos */
static void _tclock_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _tclock_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _tclock_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _tclock_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _tclock_menu_cb_post(void *data, E_Menu *m);
static int _tclock_cb_check(void *data);
static Config_Item *_tclock_config_item_get(const char *id);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;
static Ecore_Timer *check_timer;

Config *tclock_config = NULL;

/* Define the class and gadcon functions this module provides */
static const E_Gadcon_Client_Class _gc_class = {
   GADCON_CLIENT_CLASS_VERSION,
     "tclock", {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL},
   E_GADCON_CLIENT_STYLE_PLAIN
};

typedef struct _Instance Instance;
struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object *tclock, *o_tip;
   Config_Item *ci;
   E_Popup *tip;
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;
   char buf[4096];

   inst = E_NEW(Instance, 1);

   inst->ci = _tclock_config_item_get(id);
   if (!inst->ci->id) inst->ci->id = eina_stringshare_add(id);

   o = edje_object_add(gc->evas);
   snprintf(buf, sizeof (buf), "%s/tclock.edj",
            e_module_dir_get(tclock_config->module));
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/tclock", "modules/tclock/main"))
     edje_object_file_set(o, buf, "modules/tclock/main");
   evas_object_show(o);

   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;
   inst->gcc = gcc;
   inst->tclock = o;

   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
                                  _tclock_cb_mouse_down, inst);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_IN,
                                  _tclock_cb_mouse_in, inst);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_OUT,
                                  _tclock_cb_mouse_out, inst);

   tclock_config->instances =
     eina_list_append(tclock_config->instances, inst);

   _tclock_cb_check(inst);
   if (!check_timer)
     check_timer = ecore_timer_add(1.0, _tclock_cb_check, NULL);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
     
   evas_object_event_callback_del(inst->tclock, EVAS_CALLBACK_MOUSE_DOWN,
                                  _tclock_cb_mouse_down);
   evas_object_event_callback_del(inst->tclock, EVAS_CALLBACK_MOUSE_IN,
                                  _tclock_cb_mouse_in);
   evas_object_event_callback_del(inst->tclock, EVAS_CALLBACK_MOUSE_OUT,
                                  _tclock_cb_mouse_out);

   evas_object_del(inst->tclock);

   tclock_config->instances =
     eina_list_remove(tclock_config->instances, inst);

   if (eina_list_count(tclock_config->instances) <= 0) 
     {
	ecore_timer_del(check_timer);
	check_timer = NULL;
     }

   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{
   Instance *inst;
   Evas_Coord mw, mh;

   inst = gcc->data;
   edje_object_size_min_calc(inst->tclock, &mw, &mh);
   e_gadcon_client_min_size_set(gcc, mw, mh);
}

static char *
_gc_label(E_Gadcon_Client_Class *client_class)
{
   return D_("TClock");
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add (evas);
   snprintf(buf, sizeof (buf), "%s/e-module-tclock.edj",
            e_module_dir_get(tclock_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class)
{
   Config_Item *ci;

   ci = _tclock_config_item_get(NULL);
   return ci->id;
}

static void
_tclock_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;

   inst = data;
   ev = event_info;
   if ((ev->button == 3) && (!tclock_config->menu))
     {
        E_Menu *mn;
        E_Menu_Item *mi;
        int x, y;

        mn = e_menu_new();
        e_menu_post_deactivate_callback_set(mn, _tclock_menu_cb_post, inst);
        tclock_config->menu = mn;

        mi = e_menu_item_new(mn);
        e_menu_item_label_set(mi, D_("Configuration"));
        e_util_menu_item_theme_icon_set(mi, "preferences-system");
        e_menu_item_callback_set(mi, _tclock_menu_cb_configure, inst);

        mi = e_menu_item_new(mn);
        e_menu_item_separator_set(mi, 1);

        e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, NULL, NULL);
        e_menu_activate_mouse (mn,
                               e_util_zone_current_get(e_manager_current_get
                                                        ()), x + ev->output.x,
                               y + ev->output.y, 1, 1,
                               E_MENU_POP_DIRECTION_AUTO, ev->timestamp);
     }
}

static void 
_tclock_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info) 
{
   Instance *inst = NULL;
   E_Zone *zone = NULL;
   char buf[4096];
   int x, y, w, h;
   int gx, gy, gw, gh;
   time_t current_time;
   struct tm *local_time;

   if (!(inst = data)) return;
   if (!inst->ci->show_tip) return;
   if (inst->tip) return;
   snprintf(buf, sizeof(buf), "%s/tclock.edj", 
	    e_module_dir_get(tclock_config->module));

   zone = e_util_zone_current_get(e_manager_current_get());
   inst->tip = e_popup_new(zone, 0, 0, 0, 0);
   e_popup_layer_set(inst->tip, 255);

   inst->o_tip = edje_object_add(inst->tip->evas);
   if (!e_theme_edje_object_set
       (inst->o_tip, "base/theme/modules/tclock", "modules/tclock/tip"))
     edje_object_file_set(inst->o_tip, buf, "modules/tclock/tip");
   evas_object_show(inst->o_tip);

   current_time = time(NULL);
   local_time = localtime(&current_time);
   memset(buf, 0, sizeof(buf));
   strftime(buf, 1024, inst->ci->tip_format, local_time);
   edje_object_part_text_set(inst->o_tip, "e.text.tip", buf);

   evas_object_move(inst->o_tip, 0, 0);
   edje_object_size_min_calc(inst->o_tip, &w, &h);
   evas_object_resize(inst->o_tip, w, h);

   e_popup_edje_bg_object_set(inst->tip, inst->o_tip);

   ecore_x_pointer_xy_get(zone->container->win, &x, &y);
   e_gadcon_client_geometry_get(inst->gcc, &gx, &gy, &gw, &gh);
   switch (inst->gcc->gadcon->orient) 
     {
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_RB:
      case E_GADCON_ORIENT_RIGHT:
	x = gx - w;
	y = gy;
	if ((y + h) >= zone->h)
	  y = gy + gh - h;
	break;
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_LB:
	x = gx + gw;
	y = gy;
	if ((y + h) >= zone->h)
	  y = gy + gh - h;
	break;
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
	y = gy + gh;
	x = gx;
	if ((x + w) >= zone->w)
	  x = gx + gw - w;
	break;
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	y = gy - h;
	x = gx;
	if ((x + w) >= zone->w)
	  x = gx + gw - w;
	break;
      default:
	break;
     }
   e_popup_move_resize(inst->tip, x, y, w, h);
   e_popup_show(inst->tip);
}

static void 
_tclock_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info) 
{
   Instance *inst = NULL;

   if (!(inst = data)) return;
   if (!inst->tip) return;
   evas_object_del(inst->o_tip);
   e_object_del(E_OBJECT(inst->tip));
   inst->tip = NULL;
   inst->o_tip = NULL;
}

static void
_tclock_menu_cb_post(void *data, E_Menu *m)
{
   if (!tclock_config->menu) return;
   e_object_del(E_OBJECT(tclock_config->menu));
   tclock_config->menu = NULL;
}

static void
_tclock_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Instance *inst;

   inst = data;
   _config_tclock_module(inst->ci);
}

void
_tclock_config_updated(Config_Item *ci)
{
   Eina_List *l;

   if (!tclock_config) return;
   for (l = tclock_config->instances; l; l = l->next)
     {
        Instance *inst;

        inst = l->data;
        if (inst->ci != ci) continue;
        if (!inst->ci->show_time)
          edje_object_signal_emit(inst->tclock, "time_hidden", "");
        else
          edje_object_signal_emit(inst->tclock, "time_visible", "");
        edje_object_message_signal_process(inst->tclock);

        if (!inst->ci->show_date)
          edje_object_signal_emit(inst->tclock, "date_hidden", "");
        else
          edje_object_signal_emit(inst->tclock, "date_visible", "");
        edje_object_message_signal_process(inst->tclock);

        _tclock_cb_check(inst);
     }
}

static int
_tclock_cb_check(void *data)
{
   Instance *inst;
   Eina_List *l;
   time_t current_time;
   struct tm *local_time;
   char buf[1024];

   current_time = time(NULL);
   local_time = localtime(&current_time);
   for (l = tclock_config->instances; l; l = l->next) 
     {
	inst = l->data;

	if (!inst->ci->show_time)
	  edje_object_signal_emit(inst->tclock, "time_hidden", "");
	else
	  edje_object_signal_emit(inst->tclock, "time_visible", "");
	edje_object_message_signal_process(inst->tclock);
	
	if (!inst->ci->show_date)
	  edje_object_signal_emit(inst->tclock, "date_hidden", "");
	else
	  edje_object_signal_emit(inst->tclock, "date_visible", "");
	edje_object_message_signal_process(inst->tclock);

	memset (buf, 0, sizeof(buf));
	
	if (inst->ci->time_format)
	  {
             strftime(buf, 1024, inst->ci->time_format, local_time);
             edje_object_part_text_set(inst->tclock, "tclock_time", buf);
	     if (inst->tip) 
	       edje_object_part_text_set(inst->o_tip, "e.text.tip", buf);
	  }
	if (inst->ci->date_format)
	  {
             strftime(buf, 1024, inst->ci->date_format, local_time);
             edje_object_part_text_set(inst->tclock, "tclock_date", buf);
	  }
	if ((inst->ci->tip_format) && (inst->o_tip))
	  {
             strftime(buf, 1024, inst->ci->tip_format, local_time);
             edje_object_part_text_set(inst->o_tip, "e.text.tip", buf);
	  }
     }
   
   return 1;
}

static Config_Item *
_tclock_config_item_get(const char *id)
{
   Eina_List *l;
   Config_Item *ci;
   char buf[128];

   if (!id)
     {
	int  num = 0;

	/* Create id */
	if (tclock_config->items)
	  {
	     const char *p;

	     ci = eina_list_last(tclock_config->items)->data;
	     p = strrchr(ci->id, '.');
	     if (p) num = atoi(p + 1) + 1;
	  }
	snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, num);
	id = buf;
     }
   else
     {
	for (l = tclock_config->items; l; l = l->next)
	  {
	     ci = l->data;
	     if (!ci->id) continue;
	     if (!strcmp(ci->id, id)) return ci;
	  }
     }

   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->show_date = 1;
   ci->show_time = 1;
   ci->show_tip = 1;
   ci->time_format = eina_stringshare_add("%T");
   ci->date_format = eina_stringshare_add("%d/%m/%y");
   ci->tip_format = eina_stringshare_add("%A, %B %d, %Y");

   tclock_config->items = eina_list_append(tclock_config->items, ci);
   return ci;
}

EAPI E_Module_Api e_modapi = {
  E_MODULE_API_VERSION, "TClock"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

#if HAVE_LOCALE_H
   setlocale(LC_ALL, "");
#endif
   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   conf_item_edd = E_CONFIG_DD_NEW("TClock_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, show_date, INT);
   E_CONFIG_VAL(D, T, show_time, INT);
   E_CONFIG_VAL(D, T, show_tip, INT);
   E_CONFIG_VAL(D, T, date_format, STR);
   E_CONFIG_VAL(D, T, time_format, STR);
   E_CONFIG_VAL(D, T, tip_format, STR);

   conf_edd = E_CONFIG_DD_NEW("TClock_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   tclock_config = e_config_domain_load("module.tclock", conf_edd);
   if (!tclock_config)
     {
        Config_Item *ci;

        tclock_config = E_NEW(Config, 1);

        ci = E_NEW(Config_Item, 1);
        ci->id = eina_stringshare_add("0");
        ci->show_date = 1;
        ci->show_time = 1;
        ci->show_tip = 1;
        ci->time_format = eina_stringshare_add("%T");
        ci->date_format = eina_stringshare_add("%d/%m/%y");
        ci->tip_format = eina_stringshare_add("%d");

        tclock_config->items = eina_list_append(tclock_config->items, ci);
     }
   tclock_config->module = m;

   e_gadcon_provider_register(&_gc_class);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   tclock_config->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);

   if (tclock_config->config_dialog)
     e_object_del(E_OBJECT(tclock_config->config_dialog));
   if (tclock_config->menu)
     {
        e_menu_post_deactivate_callback_set(tclock_config->menu, NULL, NULL);
        e_object_del(E_OBJECT(tclock_config->menu));
        tclock_config->menu = NULL;
     }

   while (tclock_config->items)
     {
        Config_Item *ci;

        ci = tclock_config->items->data;
        tclock_config->items =
          eina_list_remove_list(tclock_config->items, tclock_config->items);
        if (ci->id)
          eina_stringshare_del(ci->id);
        if (ci->time_format)
          eina_stringshare_del(ci->time_format);
        if (ci->date_format)
          eina_stringshare_del(ci->date_format);
        if (ci->tip_format)
          eina_stringshare_del(ci->tip_format);
        E_FREE (ci);
     }

   E_FREE(tclock_config);
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save("module.tclock", conf_edd, tclock_config);
   return 1;
}
