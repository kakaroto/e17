#include <e.h>
#include <Ecore.h>
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
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

static int screen_count;
static Ecore_Event_Handler *_screen_exe_exit_handler = NULL;

static Screen *_screen_init(E_Module *m);
static void _screen_config_menu_new(Screen *e);
static void _screen_shutdown(Screen *e);
static int _screen_exe_cb_exit(void *data, int type, void *event);
static int _screen_face_init(Screen_Face *sf);
static void _screen_face_free(Screen_Face *ef);
static void _screen_face_menu_new(Screen_Face *face);
static void _screen_face_enable(Screen_Face *face);
static void _screen_face_disable(Screen_Face *face);
static void _screen_face_cb_menu_edit(void *data, E_Menu *m, E_Menu_Item *mi);
static void _screen_face_cb_mouse_down(void *data, Evas *e,
                                       Evas_Object *obj, void *event_info);
static void _screen_face_cb_gmc_change(void *data,
                                       E_Gadman_Client *gmc,
                                       E_Gadman_Change change);
static void _screen_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);

char *get_options(char **opt);
char *get_filename(Config *conf);

/* public module routines. all modules must have these */
EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "Screenshot"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   Screen *e;

   /* actually init screen */
   e = _screen_init(m);
   m->config_menu = e->config_menu;

   return e;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   Screen *s;

   s = m->data;
   if (s)
     {
        if (m->config_menu)
          {
             e_menu_deactivate(m->config_menu);
             e_object_del(E_OBJECT(m->config_menu));
             m->config_menu = NULL;
          }
        if (s->config_dialog)
          {
             e_object_del(E_OBJECT(s->config_dialog));
             s->config_dialog = NULL;
          }
        _screen_shutdown(s);
     }
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   Screen *e;

   e = m->data;
   if (e)
      e_config_domain_save("module.screenshot", e->conf_edd, e->conf);

   return 1;
}

EAPI int
e_modapi_info(E_Module *m)
{
   m->icon_file = strdup(PACKAGE_DATA_DIR "/module_icon.png");
   return 1;
}

EAPI int
e_modapi_about(E_Module *m)
{
   e_module_dialog_show(_("Enlightenment Screenshot Module"),
                        ("This module is used to take screenshots"));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m)
{
   Screen *s;
   E_Container *con;

   s = m->data;
   if (!s)
      return 0;
   if (!s->face)
      return 0;
   con = e_container_current_get(e_manager_current_get());
   if (s->face->con == con)
      _config_screenshot_module(con, s);
   return 1;
}

static Screen *
_screen_init(E_Module *m)
{
   Screen *e;
   E_Menu_Item *mi;
   Evas_List *managers, *l, *l2;

   e = E_NEW(Screen, 1);

   if (!e)
      return NULL;

   e->conf_edd = E_CONFIG_DD_NEW("Screen_Config", Config);
#undef T
#undef D
#define T Config
#define D e->conf_edd
   E_CONFIG_VAL(D, T, delay_time, INT);
   E_CONFIG_VAL(D, T, use_import, UCHAR);
   E_CONFIG_VAL(D, T, use_scrot, UCHAR);
   E_CONFIG_VAL(D, T, location, STR);
   E_CONFIG_VAL(D, T, filename, STR);
#ifdef HAVE_IMPORT
   E_CONFIG_VAL(D, T, import.use_img_border, UCHAR);
   E_CONFIG_VAL(D, T, import.use_dither, UCHAR);
   E_CONFIG_VAL(D, T, import.use_frame, UCHAR);
   E_CONFIG_VAL(D, T, import.use_mono, UCHAR);
   E_CONFIG_VAL(D, T, import.use_window, UCHAR);
   E_CONFIG_VAL(D, T, import.use_silent, UCHAR);
   E_CONFIG_VAL(D, T, import.use_trim, UCHAR);
#endif
#ifdef HAVE_SCROT
   E_CONFIG_VAL(D, T, scrot.use_img_border, UCHAR);
   E_CONFIG_VAL(D, T, scrot.use_count, UCHAR);
   E_CONFIG_VAL(D, T, scrot.use_window, UCHAR);
   E_CONFIG_VAL(D, T, scrot.use_thumb, UCHAR);
#endif
   e->conf = e_config_domain_load("module.screenshot", e->conf_edd);
   if (!e->conf)
     {
        e->conf = E_NEW(Config, 1);

        e->conf->delay_time = 60;
#ifdef HAVE_IMPORT
# ifdef HAVE_SCROT
        e->conf->use_import = 0;
        e->conf->use_scrot = 1;
# else
        e->conf->use_import = 1;
        e->conf->use_scrot = 0;
# endif
#else
        e->conf->use_import = 0;
# ifdef HAVE_SCROT
        e->conf->use_scrot = 1;
# else
        e->conf->use_scrot = 0;
# endif
#endif
        e->conf->location = (char *)evas_stringshare_add(e_user_homedir_get());
        e->conf->filename = (char *)evas_stringshare_add("");
#ifdef HAVE_IMPORT
        e->conf->import.use_img_border = 1;
        e->conf->import.use_dither = 1;
        e->conf->import.use_frame = 1;
        e->conf->import.use_mono = 0;
        e->conf->import.use_window = 0;
        e->conf->import.use_silent = 1;
        e->conf->import.use_trim = 1;
#endif
#ifdef HAVE_SCROT
        e->conf->scrot.use_img_border = 1;
        e->conf->scrot.use_count = 0;
        e->conf->scrot.use_window = 0;
        e->conf->scrot.use_thumb = 0;
#endif
     }

   E_CONFIG_LIMIT(e->conf->delay_time, 0, 60);

   _screen_config_menu_new(e);

   /* Managers */
   managers = e_manager_list();
   for (l = managers; l; l = l->next)
     {
        E_Manager *man;

        man = l->data;
        for (l2 = man->containers; l2; l2 = l2->next)
          {
             E_Container *con;
             Screen_Face *ef;

             con = l2->data;
             ef = E_NEW(Screen_Face, 1);

             if (ef)
               {
                  ef->conf_face_edd =
                     E_CONFIG_DD_NEW("Screen_Config_Face", Config_Face);
#undef T
#undef D
#define T Config_Face
#define D ef->conf_face_edd
                  E_CONFIG_VAL(D, T, enabled, UCHAR);

                  e->face = ef;
                  ef->screen = e;
                  ef->con = con;
                  ef->evas = con->bg_evas;

                  ef->conf = E_NEW(Config_Face, 1);

                  ef->conf->enabled = 1;

                  if (!_screen_face_init(ef))
                     return NULL;

                  /* This menu must be initialized after conf */
                  _screen_face_menu_new(ef);

                  /* Add main menu to face menu */
                  mi = e_menu_item_new(e->config_menu);
                  e_menu_item_label_set(mi, _("Configuration"));
                  e_menu_item_callback_set(mi, _screen_menu_cb_configure, ef);

                  mi = e_menu_item_new(e->config_menu);
                  e_menu_item_label_set(mi, con->name);
                  e_menu_item_submenu_set(mi, ef->menu);

                  /* Setup */
                  if (!ef->conf->enabled)
                    {
                       _screen_face_disable(ef);
                    }
                  else
                    {
                       _screen_face_enable(ef);
                    }
               }
          }
     }
   return e;
}

static void
_screen_shutdown(Screen *e)
{
   _screen_face_free(e->face);

   if (e->conf->location)
      evas_stringshare_del(e->conf->location);
   if (e->conf->filename)
      evas_stringshare_del(e->conf->filename);

   free(e->conf);
   E_CONFIG_DD_FREE(e->conf_edd);
   free(e);
}

static void
_screen_config_menu_new(Screen *e)
{
   E_Menu *mn;

   mn = e_menu_new();
   e->config_menu = mn;
}

static int
_screen_face_init(Screen_Face *sf)
{
   Evas_Object *o;
   char buff[4096];

   evas_event_freeze(sf->evas);
   o = edje_object_add(sf->evas);
   sf->screen_object = o;

   snprintf(buff, sizeof(buff), PACKAGE_DATA_DIR "/screenshot.edj");
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/screenshot", "modules/screenshot/main"))
      edje_object_file_set(o, buff, "modules/screenshot/main");
   edje_object_signal_emit(o, "passive", "");
   evas_object_show(o);

   o = evas_object_rectangle_add(sf->evas);
   sf->event_object = o;
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
                                  _screen_face_cb_mouse_down, sf);
   evas_object_show(o);

   sf->gmc = e_gadman_client_new(sf->con->gadman);
   e_gadman_client_domain_set(sf->gmc, "module.screenshot", screen_count++);
   e_gadman_client_policy_set(sf->gmc,
                              E_GADMAN_POLICY_ANYWHERE | E_GADMAN_POLICY_HMOVE |
                              E_GADMAN_POLICY_VMOVE | E_GADMAN_POLICY_HSIZE |
                              E_GADMAN_POLICY_VSIZE);
   e_gadman_client_min_size_set(sf->gmc, 4, 4);
   e_gadman_client_max_size_set(sf->gmc, 128, 128);
   e_gadman_client_auto_size_set(sf->gmc, 40, 40);
   e_gadman_client_align_set(sf->gmc, 1.0, 1.0);
   e_gadman_client_aspect_set(sf->gmc, 1.0, 1.0);
   e_gadman_client_resize(sf->gmc, 40, 40);
   e_gadman_client_change_func_set(sf->gmc, _screen_face_cb_gmc_change, sf);
   e_gadman_client_load(sf->gmc);
   evas_event_thaw(sf->evas);

   return 1;
}

static void
_screen_face_free(Screen_Face *ef)
{
   if (ef->menu)
      e_object_del(E_OBJECT(ef->menu));
   if (ef->event_object)
      evas_object_del(ef->event_object);
   if (ef->screen_object)
      evas_object_del(ef->screen_object);
   if (ef->gmc)
      e_gadman_client_save(ef->gmc);
   if (ef->gmc)
      e_object_del(E_OBJECT(ef->gmc));

   E_FREE(ef->conf);
   E_FREE(ef);
   screen_count--;
}

static void
_screen_face_menu_new(Screen_Face *face)
{
   E_Menu *mn;
   E_Menu_Item *mi;

   mn = e_menu_new();
   face->menu = mn;

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_menu_item_callback_set(mi, _screen_menu_cb_configure, face);

   /* Edit */
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_menu_item_callback_set(mi, _screen_face_cb_menu_edit, face);
}

static void
_screen_face_enable(Screen_Face *face)
{
   face->conf->enabled = 1;
   e_config_save_queue();
   evas_object_show(face->screen_object);
   evas_object_show(face->event_object);
}

static void
_screen_face_disable(Screen_Face *face)
{
   face->conf->enabled = 0;
   e_config_save_queue();
   evas_object_hide(face->screen_object);
   evas_object_hide(face->event_object);
}

static void
_screen_face_cb_gmc_change(void *data, E_Gadman_Client *gmc,
                           E_Gadman_Change change)
{
   Screen_Face *ef;
   Evas_Coord x, y, w, h;

   ef = data;
   switch (change)
     {
     case E_GADMAN_CHANGE_MOVE_RESIZE:
        e_gadman_client_geometry_get(ef->gmc, &x, &y, &w, &h);
        evas_object_move(ef->screen_object, x, y);
        evas_object_move(ef->event_object, x, y);
        evas_object_resize(ef->screen_object, w, h);
        evas_object_resize(ef->event_object, w, h);
        break;
     case E_GADMAN_CHANGE_RAISE:
        evas_object_raise(ef->screen_object);
        evas_object_raise(ef->event_object);
        break;
     case E_GADMAN_CHANGE_EDGE:
        break;
     case E_GADMAN_CHANGE_ZONE:
        break;
     }
}

static void
_screen_face_cb_mouse_down(void *data, Evas *e, Evas_Object *obj,
                           void *event_info)
{
   Ecore_Exe *x;
   Evas_Event_Mouse_Down *ev;
   Edje_Message_Int_Set *msg;
   Screen_Face *ef;
   char buff[1024];
   char *opts[8] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
   char *opt;
   char *f;

   ev = event_info;
   ef = data;
   if (ev->button == 3)
     {
        e_menu_activate_mouse(ef->menu, e_zone_current_get(ef->con),
                              ev->output.x, ev->output.y, 1, 1,
                              E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
        e_util_container_fake_mouse_up_all_later(ef->con);
     }
   else if (ev->button == 1)
     {
        /* Take Shot */
        if (ef->screen->conf->use_import == 1)
          {
             if (ef->screen->conf->import.use_img_border == 1)
                opts[0] = strdup("-border");
             if (ef->screen->conf->import.use_dither == 1)
                opts[1] = strdup("-dither");
             if (ef->screen->conf->import.use_frame == 1)
                opts[2] = strdup("-frame");
             if (ef->screen->conf->import.use_mono == 1)
                opts[3] = strdup("-mono");
             if (ef->screen->conf->import.use_silent == 1)
                opts[4] = strdup("-silent");
             if (ef->screen->conf->import.use_trim == 1)
                opts[5] = strdup("-trim");
             if (ef->screen->conf->import.use_window != 1)
                opts[6] = strdup("-window root");
             if (ef->screen->conf->delay_time > 0)
               {
                  snprintf(buff, sizeof(buff), "-pause %d",
                           ef->screen->conf->delay_time);
                  opts[7] = strdup(buff);
               }
             opt = get_options(opts);
             f = get_filename(ef->screen->conf);

             snprintf(buff, sizeof(buff), "import %s %s", opt, f);
             if (ef->screen->conf->delay_time > 0)
               {
                  msg = malloc(sizeof(Edje_Message_Int_Set) + 1 * sizeof(int));
                  msg->count = 1;
                  msg->val[0] = ef->screen->conf->delay_time - 1;
                  edje_object_message_send(ef->screen_object,
                                           EDJE_MESSAGE_INT_SET, 1, msg);
                  free(msg);
               }

             _screen_exe_exit_handler =
                ecore_event_handler_add(ECORE_EXE_EVENT_DEL,
                                        _screen_exe_cb_exit, NULL);
             x = ecore_exe_run(buff, ef);
          }
        else if (ef->screen->conf->use_scrot == 1)
          {
             if (ef->screen->conf->scrot.use_img_border == 1)
                opts[0] = strdup("--border");
             if (ef->screen->conf->scrot.use_thumb == 1)
                opts[1] = strdup("--thumb 25");
             if (ef->screen->conf->delay_time > 0)
               {
                  snprintf(buff, sizeof(buff), "--delay %d",
                           ef->screen->conf->delay_time);
                  opts[2] = strdup(buff);
               }
             opt = get_options(opts);
             f = get_filename(ef->screen->conf);
             snprintf(buff, sizeof(buff), "scrot %s %s", opt, f);
             if (ef->screen->conf->delay_time > 0)
               {
                  msg = malloc(sizeof(Edje_Message_Int_Set) + 1 * sizeof(int));
                  msg->count = 1;
                  msg->val[0] = ef->screen->conf->delay_time - 1;
                  edje_object_message_send(ef->screen_object,
                                           EDJE_MESSAGE_INT_SET, 1, msg);
                  free(msg);
               }

             _screen_exe_exit_handler =
                ecore_event_handler_add(ECORE_EXE_EVENT_DEL,
                                        _screen_exe_cb_exit, NULL);
             x = ecore_exe_run(buff, ef);
          }
        else
          {
             e_module_dialog_show(_("Enlightenment Screenshot Module"),
                                  _
                                  ("Please Choose A Program To Use For Taking Screenshots."));
             return;
          }
     }
}

static void
_screen_face_cb_menu_edit(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Screen_Face *face;

   face = data;
   e_gadman_mode_set(face->gmc->gadman, E_GADMAN_MODE_EDIT);
}

char *
get_options(char **opt)
{
   int i, j;
   char buff[1024];

   j = 0;
   for (i = 0; i <= 7; i++)
     {
        if (opt[i] != '\0')
          {
             if (j == 0)
               {
                  snprintf(buff, sizeof(buff), "%s", opt[i]);
                  j++;
               }
             else
               {
                  snprintf(buff, sizeof(buff), "%s %s", strdup(buff), opt[i]);
               }
          }
     }
   return strdup(buff);
}

char *
get_filename(Config *conf)
{
   char buff[256];
   time_t t;
   struct tm *loctime;
   Ecore_List *fl = NULL;
   int c = 0;
   char *file, *x;

   /* Get Location */
   if (!conf->location)
     {
        conf->location = (char *)evas_stringshare_add(e_user_homedir_get());
        e_config_save_queue();
     }

   if (!conf->filename)
     {
        t = time(NULL);
        loctime = localtime(&t);
        strftime(buff, sizeof(buff), "%Y-%m-%d-%H%M%S", loctime);
        snprintf(buff, sizeof(buff), "%s/%s.png", strdup(conf->location),
                 strdup(buff));
     }
   else
     {
        /* Parse File Name For %d */
        if (strstr(conf->filename, "%d"))
          {
             /* Get File Count In Location */
             if (ecore_file_is_dir(conf->location))
               {
                  fl = ecore_file_ls(conf->location);
                  ecore_list_goto_first(fl);
                  x = strtok(strdup(conf->filename), "%d");
                  while ((file = ecore_list_next(fl)) != NULL)
                    {
                       if (strstr(file, x))
                          c++;
                    }
                  if (fl)
                     ecore_list_destroy(fl);
                  if (c == 0)
                    {
                       c = 1;
                    }
                  else
                    {
                       c++;
                    }
                  /* Add To Filename */
                  snprintf(buff, sizeof(buff), strdup(conf->filename), c);
                  snprintf(buff, sizeof(buff), "%s/%s.png",
                           strdup(conf->location), strdup(buff));
               }
          }
        else
          {
             snprintf(buff, sizeof(buff), strdup(conf->filename));
             snprintf(buff, sizeof(buff), "%s/%s.png", strdup(conf->location),
                      strdup(buff));
          }
     }
   if (buff)
     {
        return strdup(buff);
     }
   else
     {
        return NULL;
     }
}

static int
_screen_exe_cb_exit(void *data, int type, void *event)
{
   Ecore_Exe_Event_Del *ev;
   Ecore_Exe *x;
   Screen_Face *ef;

   ev = event;
   if (!ev->exe)
      return 1;
   x = ev->exe;
   if (!x)
      return 1;

   ef = ecore_exe_data_get(x);
   x = NULL;
   edje_object_signal_emit(ef->screen_object, "passive", "");

   ecore_event_handler_del(_screen_exe_exit_handler);

#ifdef WANT_OSIRIS
   char buff[256], tmp[1024];
   time_t t;
   struct tm *loctime;
   Ecore_List *fl = NULL;
   int c = 0;
   char *file, *z;
   Config *conf;

   conf = ef->screen->conf;

   if (!conf->filename)
     {
        t = time(NULL);
        loctime = localtime(&t);
        strftime(buff, sizeof(buff), "%Y-%m-%d-%I%M%S", loctime);
        snprintf(buff, sizeof(buff), "%s/%s.png", strdup(conf->location),
                 strdup(buff));
     }
   else
     {
        /* Parse File Name For %d */
        if (strstr(conf->filename, "%d"))
          {
             /* Get File Count In Location */
             if (ecore_file_is_dir(conf->location))
               {
                  fl = ecore_file_ls(conf->location);
                  ecore_list_goto_first(fl);
                  z = strtok(strdup(conf->filename), "%d");
                  while ((file = ecore_list_next(fl)) != NULL)
                    {
                       if (strstr(file, z))
                          c++;
                    }
                  if (fl)
                     ecore_list_destroy(fl);
                  if (c == 0)
                    {
                       c = 1;
                    }
                  /* Add To Filename */
                  snprintf(buff, sizeof(buff), strdup(conf->filename), c);
                  snprintf(buff, sizeof(buff), "%s/%s.png",
                           strdup(conf->location), strdup(buff));
               }
          }
        else
          {
             snprintf(buff, sizeof(buff), strdup(conf->filename));
             snprintf(buff, sizeof(buff), "%s/%s.png", strdup(conf->location),
                      strdup(buff));
          }
     }

   snprintf(tmp, sizeof(tmp), "sudo mv %s /var/www/html/screenshots", buff);
   system(tmp);
#endif

   return 0;
}

static void
_screen_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Screen_Face *sf;

   sf = data;
   if (!sf)
      return;
   _config_screenshot_module(sf->con, sf->screen);
}
