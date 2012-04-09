#include <stdio.h>
#include <e.h>
#include "e_mod_main.h"

#define MAX_CMD_LINE_LENGTH 4096
#define LF 10

/* Gadcon Function Protos */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static const char *_gc_label(E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *eektool_config = NULL;

/* Define Gadcon Class */
static const E_Gadcon_Client_Class _gadcon_class = {
  GADCON_CLIENT_CLASS_VERSION,
  "eektool", {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL, NULL},
  E_GADCON_CLIENT_STYLE_PLAIN
};

/* Module specifics */
typedef struct _Instance Instance;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object *eektool_obj;
   char cmd_return_buffer[MAX_CMD_LINE_LENGTH];
   Ecore_Timer *timer;
   Config_Item *ci;
};

/* Module Function Protos */
static void _eektool_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _eektool_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _eektool_menu_cb_post(void *data, E_Menu *m);
static Eina_Bool _eektool_update_cb(void *data);
static Config_Item *_eektool_config_item_get(const char *id);
static Eina_Bool _eektool_update_cb(void *data);
static void _eektool_display_update(Instance *inst);
static void _eektool_exec_cmd(Instance *inst);
static Evas_Object *_eektool_add(Evas *evas);
static void _eektool_free(Instance *inst);


static Evas_Object *_eektool_add(Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, sizeof (buf), "%s/e-module-eektool.edj",
            e_module_dir_get(eektool_config->module));
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/eektool", "modules/eektool/text"))
     edje_object_file_set(o, buf, "modules/eektool/text");
   evas_object_show(o);
   edje_object_part_text_set(o, "command_output", "");

   return o;
}

static void _eektool_free(Instance *inst)
{
   evas_object_del(inst->eektool_obj);
   evas_object_event_callback_del(inst->eektool_obj, EVAS_CALLBACK_MOUSE_DOWN, _eektool_cb_mouse_down);
   ecore_timer_del(inst->timer);
   inst->timer = NULL;
}

/* Gadcon Functions */
static E_Gadcon_Client *
_gc_init (E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   E_Gadcon_Client *gcc;
   Instance *inst;

   inst = E_NEW(Instance, 1);
   inst->ci = _eektool_config_item_get(id);
   inst->eektool_obj = _eektool_add(gc->evas);

   gcc = e_gadcon_client_new(gc, name, id, style, inst->eektool_obj);
   inst->gcc = gcc;
   gcc->data = inst;

   evas_object_event_callback_add(inst->eektool_obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  _eektool_cb_mouse_down, inst);
   eektool_config->instances =
     eina_list_append(eektool_config->instances, inst);

   _eektool_update_cb(inst);

   inst->timer =
     ecore_timer_add(inst->ci->refresh_interval, _eektool_update_cb, inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;

   eektool_config->instances =
     eina_list_remove(eektool_config->instances, inst);

   _eektool_free(inst);
   inst->ci = NULL;

   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient __UNUSED__)
{
   Instance *inst;

   inst = gcc->data;
   e_gadcon_client_aspect_set(gcc, 12, 12);
   e_gadcon_client_min_size_set(gcc, 12, 12);
}

static const char *
_gc_label(E_Gadcon_Client_Class *client_class __UNUSED__)
{
   return "Eektool";
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class __UNUSED__, Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, (sizeof(buf) - 1), "%s/e-module-eektool.edj",
            e_module_dir_get(eektool_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class __UNUSED__)
{
   Config_Item *ci;

   ci = _eektool_config_item_get(NULL);
   return ci->id;
}

static void
_eektool_cb_mouse_down(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;

   inst = data;
   ev = event_info;
   if ((ev->button == 3) && (!eektool_config->menu))
     {
        E_Menu *m;
        E_Menu_Item *mi;
        int x, y, w, h;

        m = e_menu_new();
        mi = e_menu_item_new(m);
        e_menu_item_label_set(mi, D_("Settings"));
        e_util_menu_item_theme_icon_set(mi, "preferences-system");
        e_menu_item_callback_set(mi, _eektool_menu_cb_configure, inst);

        m = e_gadcon_client_util_menu_items_append(inst->gcc, m, 0);
        e_menu_post_deactivate_callback_set(m, _eektool_menu_cb_post, inst);
        eektool_config->menu = m;
        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, &w, &h);
        e_menu_activate_mouse(m,
                              e_util_zone_current_get(e_manager_current_get()),
			      x + ev->output.x, y + ev->output.y, 1, 1,
                              E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
        evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
                                 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

static void
_eektool_menu_cb_post(void *data __UNUSED__, E_Menu *m __UNUSED__)
{
   if (!eektool_config->menu) return;
   e_object_del(E_OBJECT(eektool_config->menu));
   eektool_config->menu = NULL;
}

static void
_eektool_menu_cb_configure(void *data, E_Menu *m __UNUSED__, E_Menu_Item *mi __UNUSED__)
{
   Instance *inst;

   inst = data;
   _config_eektool_module(inst->ci);
}

static Config_Item *
_eektool_config_item_get(const char *id)
{
   Eina_List *l;
   Config_Item *ci;
   char buf[128];

   if (!id)
     {
	int  num = 0;

	/* Create id */
	if (eektool_config->items)
	  {
	     const char *p;

	     ci = eina_list_last(eektool_config->items)->data;
	     p = strrchr(ci->id, '.');
	     if (p) num = atoi(p + 1) + 1;
	  }
	snprintf(buf, sizeof (buf), "%s.%d", _gadcon_class.name, num);
	id = buf;
     }
   else
     {
	for (l = eektool_config->items; l; l = l->next)
	  {
	     ci = l->data;
	     if (!ci->id) continue;
	     if (!strcmp(ci->id, id)) return ci;
	  }
     }

   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->refresh_interval = 60.0;
   ci->cmd = eina_stringshare_add("echo \"Hello World\";");

   eektool_config->items = eina_list_append(eektool_config->items, ci);
   return ci;
}

/* Gadman Module Setup */
EAPI E_Module_Api e_modapi = { E_MODULE_API_VERSION, "Eektool" };

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

   snprintf(buf, sizeof (buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   conf_item_edd = E_CONFIG_DD_NEW("Eektool_Text_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, cmd, STR);
   E_CONFIG_VAL(D, T, refresh_interval, DOUBLE);

   conf_edd = E_CONFIG_DD_NEW("Eektool_Text_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   eektool_config = e_config_domain_load("module.eektool", conf_edd);
   if (!eektool_config)
     {
        Config_Item *ci;

        eektool_config = E_NEW(Config, 1);
        ci = E_NEW(Config_Item, 1);
        ci->id = eina_stringshare_add("0");
        ci->refresh_interval = 60.0;
        ci->cmd = "echo \"Hello World\";";
        eektool_config->items = eina_list_append(eektool_config->items, ci);
     }

   eektool_config->module = m;
   e_gadcon_provider_register(&_gadcon_class);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m __UNUSED__)
{
   Config_Item *ci;

   eektool_config->module = NULL;
   e_gadcon_provider_unregister(&_gadcon_class);

   if (eektool_config->config_dialog)
     e_object_del(E_OBJECT(eektool_config->config_dialog));
   if (eektool_config->menu)
     {
        e_menu_post_deactivate_callback_set(eektool_config->menu, NULL, NULL);
        e_object_del(E_OBJECT(eektool_config->menu));
        eektool_config->menu = NULL;
     }

   EINA_LIST_FREE(eektool_config->items, ci)
     {
        if (ci->id) eina_stringshare_del(ci->id);
        eina_stringshare_del(ci->cmd);
        E_FREE(ci);
     }

   E_FREE(eektool_config);
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m __UNUSED__)
{
   e_config_domain_save("module.eektool", conf_edd, eektool_config);
   return 1;
}

static Eina_Bool
_eektool_update_cb(void *data)
{
   Instance *inst = data;

   if (!inst)
     return ECORE_CALLBACK_CANCEL;

   _eektool_exec_cmd(inst);
   _eektool_display_update(inst);

   return ECORE_CALLBACK_RENEW;
}

static void
_eektool_display_update(Instance *inst)
{
   if (!inst) return;

   edje_object_part_text_set(inst->eektool_obj, "command_output", inst->cmd_return_buffer);
}

static void
_eektool_exec_cmd(Instance *inst)
{
   FILE* pipe;
   char buf[256], *c;
   unsigned int len = 0;

   if (!inst) return;

   inst->cmd_return_buffer[0] = '\0';

   if (!inst->ci->cmd || !strcmp(inst->ci->cmd, "") || !(pipe = popen((const char*)inst->ci->cmd, "r")))
     return;

   while (fgets(buf, sizeof(buf), pipe))
     {
        if ((strlen(inst->cmd_return_buffer) + strlen(buf) - 1) > MAX_CMD_LINE_LENGTH)
          break;
        strcat(inst->cmd_return_buffer, buf);
        len = strlen(inst->cmd_return_buffer);
        c = &inst->cmd_return_buffer[len - 1];
        if (*c == LF)
          {
             len += sizeof("<br/>");
             if (len > MAX_CMD_LINE_LENGTH)
               break;
             snprintf(c, (sizeof("<br/>") + 1), "<br/>");
          }
     }

   //make sure the string is NULL terminated.
   inst->cmd_return_buffer[len] = '\0';

   pclose(pipe);
   return;

   pclose(pipe);
}

void
_eektool_config_updated(Config_Item *ci)
{
   Instance *inst;
   Eina_List *l;

   if (!eektool_config) return;
   EINA_LIST_FOREACH(eektool_config->instances, l, inst)
     {
        if (inst->ci != ci)
          continue;
        _eektool_update_cb(inst);
        if (!inst->timer)
          inst->timer = ecore_timer_add(ci->refresh_interval,
                                              _eektool_update_cb, inst);
        else
          ecore_timer_interval_set(inst->timer, ci->refresh_interval * 1000);
     }
}
