#include <time.h>
#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"

static TClock *_tclock_new();
static void _tclock_shutdown(TClock *tclock);
static void _tclock_config_menu_new(TClock *tclock);

static TClock_Face *_tclock_face_new(E_Container *con);
static void _tclock_face_free(TClock_Face *face);
static void _tclock_face_menu_new(TClock_Face *face);

static void _tclock_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);

static void _tclock_face_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _tclock_face_cb_menu_edit(void *data, E_Menu *m, E_Menu_Item *mi);
static void _tclock_face_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi);

static int _tclock_cb_check(void *data);

static E_Config_DD *conf_edd;
static E_Config_DD *conf_face_edd;

static int _tclock_count;

/*public code******************************/
EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "TClock"
};

EAPI void *
e_modapi_init(E_Module *module)
{
   TClock *tclock;

   /* Set up module's message catalogue */
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   tclock = _tclock_new();
   module->config_menu = tclock->config_menu;
   return tclock;
}

EAPI int
e_modapi_shutdown(E_Module *module)
{
   TClock *tclock;

   if (module->config_menu)
      module->config_menu = NULL;

   tclock = module->data;
   if (tclock)
     {
        _tclock_shutdown(tclock);
     }
   return 1;
}

EAPI int
e_modapi_info(E_Module *module)
{
   module->icon_file = strdup(PACKAGE_DATA_DIR "/module_icon.png");
   return 1;
}

EAPI int
e_modapi_save(E_Module *module)
{
   TClock *tclock;

   tclock = module->data;
   e_config_domain_save("module.tclock", conf_edd, tclock->conf);
   return 1;
}

EAPI int
e_modapi_about(E_Module *module)
{
   e_module_dialog_show(D_("Simple Digital Clock"), D_("Displays a digital clock on the desktop"));
   return 1;
}

EAPI int
e_modapi_config(E_Module *module)
{
   Evas_List *l;
   TClock *t;

   t = module->data;
   if (!t)
      return 0;
   if (!t->faces)
      return 0;
   for (l = t->faces; l; l = l->next)
     {
        TClock_Face *tf;

        tf = l->data;
        if (!tf)
           return 0;
        if (tf->con == e_container_current_get(e_manager_current_get()))
          {
             /* Configure Clock */
             _config_tclock_module(tf->con, tf);
             break;
          }
     }
   return 1;
}

/******************************************************************
 * private functions
 ****************************************************************/
static TClock *
_tclock_new()
{
   TClock *tclock;
   Evas_List *managers, *l, *l2, *cl;
   E_Menu_Item *mi;

   _tclock_count = 0;

   tclock = E_NEW(TClock, 1);

   if (!tclock)
      return NULL;

   conf_face_edd = E_CONFIG_DD_NEW("TClock_Config_Face", Config_Face);
#undef T
#undef D
#define T Config_Face
#define D conf_face_edd
   E_CONFIG_VAL(D, T, enabled, UCHAR);
   E_CONFIG_VAL(D, T, resolution, UINT);
   E_CONFIG_VAL(D, T, format, STR);
   E_CONFIG_VAL(D, T, userformat, UINT);

   conf_edd = E_CONFIG_DD_NEW("TClock_Config", Config);

#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, faces, conf_face_edd);
   E_CONFIG_VAL(D, T, poll_time, DOUBLE);

   tclock->conf = e_config_domain_load("module.tclock", conf_edd);

   if (!tclock->conf)
     {
        tclock->conf = E_NEW(Config, 1);

        tclock->conf->poll_time = 60.0;
     }

   _tclock_config_menu_new(tclock);

   managers = e_manager_list();
   cl = tclock->conf->faces;
   for (l = managers; l; l = l->next)
     {
        E_Manager *man;

        man = l->data;
        for (l2 = man->containers; l2; l2 = l2->next)
          {
             E_Container *con;
             TClock_Face *face;

             con = l2->data;
             face = _tclock_face_new(con);
             if (face)
               {
                  face->tclock = tclock;
                  tclock->faces = evas_list_append(tclock->faces, face);

                  /* Config */
                  if (!cl)
                    {
                       face->conf = E_NEW(Config_Face, 1);

                       /* set instance config values */
                       face->conf->enabled = 1;
                       face->conf->resolution = RESOLUTION_MINUTE;
                       face->conf->userformat = 0;

                       const char *format;

                       format = edje_object_part_state_get(face->tclock_object, "tclock_format", NULL);
                       face->conf->format = (char *)evas_stringshare_add(format);

                       tclock->conf->faces = evas_list_append(tclock->conf->faces, face->conf);
                    }
                  else
                    {
                       face->conf = cl->data;
                       cl = cl->next;
                    }

                  if (face->conf->resolution == RESOLUTION_SECOND)
                    {
                       E_CONFIG_LIMIT(tclock->conf->poll_time, 0.1, 1.0);
                       tclock->tclock_check_timer = ecore_timer_add(tclock->conf->poll_time, _tclock_cb_check, tclock);
                       TCLOCK_DEBUG("RES_SEC");
                    }
                  else
                    {
                       E_CONFIG_LIMIT(tclock->conf->poll_time, 60.0, 60.0);
                       tclock->tclock_check_timer = ecore_timer_add(tclock->conf->poll_time, _tclock_cb_check, tclock);
                       TCLOCK_DEBUG("RES_MIN");
                       /* to avoid the long display of "Starting the clock..." */
                       _tclock_cb_check(tclock);
                    }

                  /* Menu */
                  /* This menu must be initialized after conf */
                  _tclock_face_menu_new(face);

                  mi = e_menu_item_new(tclock->config_menu);
                  e_menu_item_label_set(mi, _("Configuration"));
                  e_menu_item_callback_set(mi, _tclock_face_cb_menu_configure, face);

                  mi = e_menu_item_new(tclock->config_menu);
                  e_menu_item_label_set(mi, con->name);
                  e_menu_item_submenu_set(mi, face->menu);
               }
          }
     }

   return tclock;
}

static void
_tclock_shutdown(TClock *tclock)
{
   Evas_List *list;

   E_CONFIG_DD_FREE(conf_edd);
   E_CONFIG_DD_FREE(conf_face_edd);

   ecore_timer_del(tclock->tclock_check_timer);

   for (list = tclock->faces; list; list = list->next)
      _tclock_face_free(list->data);
   evas_list_free(tclock->faces);

   e_object_del(E_OBJECT(tclock->config_menu));

   evas_list_free(tclock->conf->faces);
   free(tclock->conf);
   free(tclock);
}

static void
_tclock_config_menu_new(TClock *tclock)
{
   tclock->config_menu = e_menu_new();
}

static TClock_Face *
_tclock_face_new(E_Container *con)
{
   TClock_Face *face;
   Evas_Object *o;
   Evas_Coord x, y, w, h;
   char buff[4096];

   face = E_NEW(TClock_Face, 1);

   if (!face)
      return NULL;

   face->con = con;
   e_object_ref(E_OBJECT(con));

   evas_event_freeze(con->bg_evas);
   o = edje_object_add(con->bg_evas);
   face->tclock_object = o;

   snprintf(buff, sizeof(buff), PACKAGE_DATA_DIR "/tclock.edj");
   if (!e_theme_edje_object_set(o, "base/theme/modules/tclock", "modules/tclock/main"))
      edje_object_file_set(o, buff, "modules/tclock/main");
   evas_object_show(o);

   o = evas_object_rectangle_add(con->bg_evas);
   face->event_object = o;
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _tclock_face_cb_mouse_down, face);
   evas_object_show(o);

   evas_object_resize(face->tclock_object, 200, 200);
   edje_object_calc_force(face->tclock_object);
   edje_object_part_geometry_get(face->tclock_object, "main", &x, &y, &w, &h);
   face->inset.l = x;
   face->inset.r = 200 - (x + w);
   face->inset.t = y;
   face->inset.b = 200 - (y + h);

   face->gmc = e_gadman_client_new(con->gadman);
   e_gadman_client_domain_set(face->gmc, "module.tclock", _tclock_count++);
   e_gadman_client_policy_set(face->gmc,
                              E_GADMAN_POLICY_ANYWHERE |
                              E_GADMAN_POLICY_HMOVE | E_GADMAN_POLICY_VMOVE | E_GADMAN_POLICY_HSIZE | E_GADMAN_POLICY_VSIZE);
   e_gadman_client_min_size_set(face->gmc, 4, 4);
   e_gadman_client_max_size_set(face->gmc, 512, 512);
   e_gadman_client_auto_size_set(face->gmc, 40 + (face->inset.l + face->inset.r), 40 + (face->inset.t + face->inset.b));

   e_gadman_client_align_set(face->gmc, 1.0, 1.0);
   //e_gadman_client_aspect_set(face->gmc, 1.0, 1.0);
   e_gadman_client_padding_set(face->gmc, face->inset.l, face->inset.r, face->inset.t, face->inset.b);

   e_gadman_client_resize(face->gmc, 40 + (face->inset.l + face->inset.r), 40 + (face->inset.t + face->inset.b));

   e_gadman_client_change_func_set(face->gmc, _tclock_face_cb_gmc_change, face);
   e_gadman_client_load(face->gmc);

   evas_event_thaw(con->bg_evas);

   return face;
}

static void
_tclock_face_free(TClock_Face *face)
{
   e_object_unref(E_OBJECT(face->con));
   e_object_del(E_OBJECT(face->gmc));
   evas_object_del(face->tclock_object);
   evas_object_del(face->event_object);
   e_object_del(E_OBJECT(face->menu));

   if (face->conf->format)
      evas_stringshare_del(face->conf->format);
   free(face->conf);
   free(face);
   _tclock_count--;
}

static int
_tclock_cb_check(void *data)
{
   TClock *tclock;
   Evas_List *l;
   time_t current_time;
   struct tm *local_time;
   char buf[TIME_BUF];

   memset(buf, 0, sizeof(buf));
   current_time = time(NULL);
   local_time = localtime(&current_time);

   tclock = data;
   for (l = tclock->faces; l; l = l->next)
     {
        TClock_Face *face;

        face = l->data;

        const char *format;

        /* Load the default format string from the module.edj-file
         * when the user defineable format string shouldn't be used 
         * otherwise use the user defined format string */
        if (!face->conf->userformat)
          {
             format = edje_object_part_state_get(face->tclock_object, "tclock_format", NULL);
          }
        else
           format = face->conf->format;

        strftime(buf, TIME_BUF, format, local_time);

        TCLOCK_DEBUG(face->conf->format);
        edje_object_part_text_set(face->tclock_object, "tclock_text", buf);
        e_config_save_queue();
     }
   return 1;
}

static void
_tclock_face_menu_new(TClock_Face *face)
{
   E_Menu *mn;
   E_Menu_Item *mi;

   mn = e_menu_new();
   face->menu = mn;

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
   e_menu_item_callback_set(mi, _tclock_face_cb_menu_configure, face);

   /* Edit */
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/gadgets");
   e_menu_item_callback_set(mi, _tclock_face_cb_menu_edit, face);
}

static void
_tclock_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change)
{
   TClock_Face *face;
   Evas_Coord x, y, w, h;

   face = data;
   switch (change)
     {
     case E_GADMAN_CHANGE_MOVE_RESIZE:
        e_gadman_client_geometry_get(face->gmc, &x, &y, &w, &h);
        evas_object_move(face->tclock_object, x, y);
        evas_object_move(face->event_object, x, y);
        evas_object_resize(face->tclock_object, w, h);
        evas_object_resize(face->event_object, w, h);
        break;
     case E_GADMAN_CHANGE_RAISE:
        evas_object_raise(face->tclock_object);
        evas_object_raise(face->event_object);
        break;
     case E_GADMAN_CHANGE_EDGE:
     case E_GADMAN_CHANGE_ZONE:
        /* FIXME
         * Must we do something here?
         */
        break;
     }
}

static void
_tclock_face_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   TClock_Face *face;
   Evas_Event_Mouse_Down *ev;

   face = data;
   ev = event_info;

   if (ev->button == 3)
     {
        e_menu_activate_mouse(face->menu, e_zone_current_get(face->con),
                              ev->output.x, ev->output.y, 1, 1, E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
        e_util_container_fake_mouse_up_all_later(face->con);
     }
}

static void
_tclock_face_cb_menu_edit(void *data, E_Menu *m, E_Menu_Item *mi)
{
   TClock_Face *face;

   face = data;
   e_gadman_mode_set(face->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void
_tclock_face_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   TClock_Face *f;

   f = data;
   if (!f)
      return;
   _config_tclock_module(f->con, f);
}
