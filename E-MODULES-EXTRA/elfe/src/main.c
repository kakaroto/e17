#include <e.h>
#include "main.h"
#include "elfe_config.h"
#include "allapps.h"
#include "desktop.h"
#include "winlist.h"
#include "utils.h"
#include "gadget_list.h"

#define ELFE_HOME_WIN_TYPE 0xE0b0102f

/* local structures */
typedef struct _Elfe_Home_Win Elfe_Home_Win;
typedef struct _Elfe_Home_Exec Elfe_Home_Exec;

struct _Elfe_Home_Win
{
   E_Object e_obj_inherit;

   E_Win *win;
   Evas_Object *o_bg;
   Evas_Object *layout;
   Evas_Object *desktop;
   Evas_Object *allapps;
   Evas_Object *rect;
   Evas_Object *floating_icon;
   E_Zone *zone;
   Efreet_Menu *selected_app;
   const char *selected_gadget;
};

typedef enum
  {
    GADMAN_LAYER_BG = 0, /* layer is considered unsigned int */
    GADMAN_LAYER_TOP,
    GADMAN_LAYER_COUNT
  } Gadman_Layer_Type;

#define DEFAULT_POS_X  0.1
#define DEFAULT_POS_Y  0.1
#define DEFAULT_SIZE_W 0.07
#define DEFAULT_SIZE_H 0.07

static E_Gadcon *gc = NULL;

/* local function prototypes */
static void _elfe_home_win_new(E_Zone *zone);
static void _elfe_home_win_cb_free(Elfe_Home_Win *hwin);
static void _elfe_home_win_cb_resize(E_Win *win);

static Eina_Bool _elfe_home_update_deferred(void *data __UNUSED__);
static Eina_Bool _elfe_home_desktop_cache_update(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__);
static Eina_Bool _elfe_home_cb_border_add(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _elfe_home_cb_border_del(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _elfe_home_cb_exe_del(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _elfe_home_cb_client_message(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _elfe_home_cb_prop_change(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _elfe_home_cb_bg_change(void *data __UNUSED__, int type __UNUSED__, void *event);

/* local variables */
static Eina_List *hwins = NULL;
static Eina_List *hdls = NULL;
static Eina_List *exes = NULL;
static Ecore_Timer *defer = NULL;

/* public functions */
EAPI E_Module_Api e_modapi = { E_MODULE_API_VERSION, "Illume Home" };

EAPI void *
e_modapi_init(E_Module *m)
{
   E_Manager *man;
   Eina_List *ml;

   /* Set this module to be loaded after all other modules, or we don't see
    modules loaded after this */
   e_module_priority_set(m, 100);

   if (!elfe_home_config_init(m)) return NULL;

   e_winilist_init();

   hdls =
     eina_list_append(hdls,
                      ecore_event_handler_add(E_EVENT_BG_UPDATE,
                                              _elfe_home_cb_bg_change, NULL));

   EINA_LIST_FOREACH(e_manager_list(), ml, man)
     {
        E_Container *con;
        Eina_List *cl;

        EINA_LIST_FOREACH(man->containers, cl, con)
          {
             E_Zone *zone;
             Eina_List *zl;

             EINA_LIST_FOREACH(con->zones, zl, zone)
               {
                  Ecore_X_Illume_Mode mode;

                  mode = ecore_x_e_illume_mode_get(zone->black_win);
                  _elfe_home_win_new(zone);
                  if (mode > ECORE_X_ILLUME_MODE_SINGLE)
                    _elfe_home_win_new(zone);
               }
          }
     }

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m __UNUSED__)
{
   Ecore_Event_Handler *hdl;
   Elfe_Home_Win *hwin;

   e_winilist_shutdown();

   EINA_LIST_FREE(hwins, hwin)
     e_object_del(E_OBJECT(hwin));

   EINA_LIST_FREE(hdls, hdl)
     ecore_event_handler_del(hdl);

   elfe_home_config_shutdown();
   return 1;
}

EAPI int
e_modapi_save(E_Module *m __UNUSED__)
{
   return elfe_home_config_save();
}

void
elfe_home_win_cfg_update(void)
{

}

static void
_gadget_added_cb(void *data , Evas_Object *obj, void *event_info)
{
   Elfe_Home_Win *hwin = data;
   Evas_Object *o_edje;

   o_edje = elm_layout_edje_get(hwin->layout);
   edje_object_signal_emit(o_edje, "inwin,hide", "elfe");
}

static void
_icon_mouse_move_cb(void *data,Evas *evas, Evas_Object *obj, void *event_info)
{
   Elfe_Home_Win *hwin = data;
   Evas_Event_Mouse_Move *ev = event_info;

   evas_object_move(hwin->floating_icon, ev->cur.output.x - 92 / 2, ev->cur.output.y - 92 / 2);

}


static void
_icon_mouse_up_cb(void *data,Evas *evas, Evas_Object *obj, void *event_info)
{

   Elfe_Home_Win *hwin = data;
   Evas_Event_Mouse_Up *ev = event_info;

   evas_object_del(hwin->floating_icon);
   evas_object_event_callback_del(hwin->desktop, EVAS_CALLBACK_MOUSE_MOVE, _icon_mouse_move_cb);
   evas_object_event_callback_del(hwin->desktop, EVAS_CALLBACK_MOUSE_UP, _icon_mouse_up_cb);
   elfe_desktop_edit_mode_set(hwin->desktop, EINA_FALSE);

   if (hwin->selected_app)
       elfe_desktop_app_add(hwin->desktop, hwin->selected_app, ev->output.x, ev->output.y);
   else if (hwin->selected_gadget)
       elfe_desktop_gadget_add(hwin->desktop, hwin->selected_gadget, ev->output.x, ev->output.y);

}


static void
_app_longpressed_cb(void *data , Evas_Object *obj, void *event_info)
{
   Elfe_Home_Win *hwin = data;
   Efreet_Menu *entry = event_info;
   Evas_Object *ic;
   Evas_Coord x, y;
   Evas_Object *o_edje;
   Evas_Coord ow, oh;
   Evas_Coord size = 0;

   evas_object_geometry_get(hwin->desktop, NULL, NULL, &ow, &oh);

   size = MIN(ow, oh) / 5;

   elfe_desktop_edit_mode_set(hwin->desktop, EINA_TRUE);

   o_edje = elm_layout_edje_get(hwin->layout);
   edje_object_signal_emit(o_edje, "appslist,toggle", "elfe");

   ic = elfe_utils_fdo_icon_add(o_edje, entry->icon, size);
   evas_object_show(ic);
   evas_pointer_canvas_xy_get(evas_object_evas_get(obj), &x, &y);
   evas_object_resize(ic, size, size);
   evas_object_move(ic, x - size / 2, y - size /2);
   hwin->floating_icon = ic;

   evas_object_del(hwin->allapps);
   hwin->allapps = NULL;
   evas_object_pass_events_set(ic, EINA_TRUE);

   hwin->selected_app = entry;

   evas_object_event_callback_add(hwin->desktop, EVAS_CALLBACK_MOUSE_MOVE, _icon_mouse_move_cb, hwin);
   evas_object_event_callback_add(hwin->desktop, EVAS_CALLBACK_MOUSE_UP, _icon_mouse_up_cb, hwin);
}

static void
_gadget_longpressed_cb(void *data , Evas_Object *obj, void *event_info)
{
   Elfe_Home_Win *hwin = data;
   const char *name = event_info;
   Evas_Coord x, y;
   Evas_Object *o_edje;
   Evas_Coord ow, oh;
   Evas_Coord size = 0;
   Evas_Object *ic;
   E_Gadcon_Client_Class *gcc = NULL;

   gcc = elfe_utils_gadcon_client_class_from_name(name);
   if (!gcc)
     {
         printf("error : unable to find gadcon client class from name : %s\n", name);
         return;
     }

   evas_object_geometry_get(hwin->desktop, NULL, NULL, &ow, &oh);

   size = MIN(ow, oh) / 5;

   elfe_desktop_edit_mode_set(hwin->desktop, EINA_TRUE);

   o_edje = elm_layout_edje_get(hwin->layout);
   edje_object_signal_emit(o_edje, "appslist,toggle", "elfe");

   ic = gcc->func.icon(gcc, evas_object_evas_get(obj));
   if (!ic)
     ic = elfe_utils_fdo_icon_add(obj, NULL, 64);

   evas_object_show(ic);
   evas_pointer_canvas_xy_get(evas_object_evas_get(obj), &x, &y);
   evas_object_resize(ic, size, size);
   evas_object_move(ic, x - size / 2, y - size /2);
   hwin->floating_icon = ic;

   evas_object_del(hwin->allapps);
   hwin->allapps = NULL;
   evas_object_pass_events_set(ic, EINA_TRUE);

   hwin->selected_app = NULL;
   hwin->selected_gadget = name;

   evas_object_event_callback_add(hwin->desktop, EVAS_CALLBACK_MOUSE_MOVE, _icon_mouse_move_cb, hwin);
   evas_object_event_callback_add(hwin->desktop, EVAS_CALLBACK_MOUSE_UP, _icon_mouse_up_cb, hwin);
}

static void*
_app_exec_cb(void *data, Efreet_Desktop *desktop, char *command, int remaining)
{
    ecore_exe_run(command, NULL);
}

static void
_allapps_item_selected_cb(void *data , Evas_Object *obj, void *event_info)
{
    Efreet_Menu *menu = event_info;
    Evas_Object *o_edje;
    Elfe_Home_Win *hwin = data;



    o_edje = elm_layout_edje_get(hwin->layout);
    edje_object_signal_emit(o_edje, "appslist,toggle", "elfe");

    efreet_desktop_command_get(menu->desktop, NULL,
                               _app_exec_cb, NULL);
}

static void  _edje_signal_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Elfe_Home_Win *hwin;
   Evas_Object *o_edje;
   Evas_Object *winlist;
   hwin = data;



   if (!strcmp(emission, "action,settings"))
     {
	//elfe_desktop_edit_mode_set(hwin->desktop, EINA_TRUE);
     }
   else if (!strcmp(emission, "action,windows"))
     {
	/* printf("Receive edje signal %s:%s\n", emission, source); */

	/* winlist = e_winilist_add(hwin->layout); */
	/* elm_layout_content_set(hwin->layout, "windowlist.swallow", winlist); */
	/* o_edje = elm_layout_edje_get(hwin->layout); */
	/* edje_object_signal_emit(o_edje, "windowlist,show", "elfe"); */
     }
   else if (!strcmp(emission, "action,apps"))
     {
	if (!hwin->allapps)
	  {
	     hwin->allapps = elfe_allapps_add(hwin->layout);
	     evas_object_smart_callback_add(hwin->allapps, "entry,longpressed", _app_longpressed_cb, hwin);
             evas_object_smart_callback_add(hwin->allapps, "gadget,longpressed", _gadget_longpressed_cb, hwin);
             evas_object_smart_callback_add(hwin->allapps, "item,selected", _allapps_item_selected_cb, hwin);
	     evas_object_show(hwin->allapps);
	     elm_layout_content_set(hwin->layout, "apps-list-swallow", hwin->allapps);
	  }
     }
}

static void
_elfe_home_win_new(E_Zone *zone)
{
   Elfe_Home_Win *hwin;
   Evas *evas;
   E_Desk *desk;
   char buf[PATH_MAX];
   const char *bgfile;
   Elm_Theme *theme;
   Evas_Object *winlist;
   Evas_Object *o_edje;
   const char *file;

   hwin = E_OBJECT_ALLOC(Elfe_Home_Win, ELFE_HOME_WIN_TYPE, _elfe_home_win_cb_free);
   if (!hwin) return;

   hwin->zone = zone;
   hwin->win = e_win_new(zone->container);
   if (!hwin->win)
     {
        e_object_del(E_OBJECT(hwin));
        return;
     }
   hwin->win->data = hwin;
   e_win_title_set(hwin->win, "Elfe");
   e_win_name_class_set(hwin->win, "Illume-Home", "Illume-Home");
   e_win_resize_callback_set(hwin->win, _elfe_home_win_cb_resize);
   e_win_no_remember_set(hwin->win, EINA_TRUE);
   //ecore_x_e_illume_conformant_set(hwin->win->evas_win, EINA_TRUE);

   snprintf(buf, sizeof(buf), "%s/default.edj",
            elfe_home_cfg->mod_dir);

   evas = e_win_evas_get(hwin->win);

   desk = e_desk_current_get(zone);
   if (desk)
     bgfile = e_bg_file_get(zone->container->num, zone->num, desk->x, desk->y);
   else
     bgfile = e_bg_file_get(zone->container->num, zone->num, -1, -1);

   hwin->o_bg = edje_object_add(evas);
   edje_object_file_set(hwin->o_bg, bgfile, "e/desktop/background");
   evas_object_move(hwin->o_bg, 0, 0);
   evas_object_show(hwin->o_bg);

   printf("Create layout\n");


   /* Specific ELM initialisation */
   elm_init(0, NULL);
   elm_need_efreet();


   hwin->layout = elm_layout_add(hwin->o_bg);
   /* Try default theme first */
   file = e_theme_edje_file_get("base/theme/modules/elfe", "module/elfe/main");
   printf("file : %s\n", file);
   if (file && file[0])
     elm_layout_file_set(hwin->layout, file, "module/elfe/main");
   else /* specific module theme otherwise */
     {
	printf("specific file : %s\n", buf);
	if (!elm_layout_file_set(hwin->layout, buf, "module/elfe/main"))
	  {
	     printf("Error loading group %s in %s theme file\n", "module/elfe/main", buf);
	  }
     }
   o_edje = elm_layout_edje_get(hwin->layout);
   edje_object_signal_callback_add(o_edje, "*", "*", _edje_signal_cb, hwin);

   theme = elm_theme_new();
   /* Use specific module theme as elm theme overlay */
   elm_theme_overlay_add(theme, buf);
   elm_object_theme_set(hwin->layout, theme);

   hwin->desktop = elfe_desktop_add(hwin->layout, hwin->zone);
   elm_layout_content_set(hwin->layout, "launcher.swallow", hwin->desktop);
   evas_object_smart_callback_add(hwin->desktop, "gadget,added", _gadget_added_cb, hwin);


   evas_object_move(hwin->layout, 0, 0);
   evas_object_show(hwin->layout);

   e_win_move_resize(hwin->win, zone->x, zone->y, zone->w, (zone->h / 2));
   e_win_show(hwin->win);
   e_border_zone_set(hwin->win->border, zone);
   if (hwin->win->evas_win)
     e_drop_xdnd_register_set(hwin->win->evas_win, EINA_TRUE);

   hwin->rect = evas_object_rectangle_add(evas);
   evas_object_resize(hwin->rect, zone->w, zone->h);
   evas_object_move(hwin->rect, 0, 0);
   //evas_object_show(hwin->rect);
   evas_object_color_set(hwin->rect, 0, 0, 0, 0);
   evas_object_raise(hwin->rect);

   hwins = eina_list_append(hwins, hwin);
}

static void
_elfe_home_win_cb_free(Elfe_Home_Win *hwin)
{
   if (hwin->win->evas_win) e_drop_xdnd_register_set(hwin->win->evas_win, 0);
   if (hwin->o_bg) evas_object_del(hwin->o_bg);
   if (hwin->layout) evas_object_del(hwin->layout);
   if (hwin->win) e_object_del(E_OBJECT(hwin->win));
   E_FREE(hwin);
}

static void
_elfe_home_win_cb_resize(E_Win *win)
{
   Elfe_Home_Win *hwin;

   if (!(hwin = win->data)) return;
   if (hwin->o_bg) evas_object_resize(hwin->o_bg, win->w, win->h);
   if (hwin->layout) evas_object_resize(hwin->layout, win->w, win->h);
}


static Eina_Bool
_elfe_home_cb_client_message(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_X_Event_Client_Message *ev;

   ev = event;
   if (ev->message_type == ECORE_X_ATOM_E_ILLUME_HOME_NEW)
     {
        E_Zone *zone;

        zone = e_util_zone_window_find(ev->win);
        if (zone->black_win != ev->win) return ECORE_CALLBACK_PASS_ON;
        _elfe_home_win_new(zone);
     }
   else if (ev->message_type == ECORE_X_ATOM_E_ILLUME_HOME_DEL)
     {
        E_Border *bd;
        Eina_List *l;
        Elfe_Home_Win *hwin;

        if (!(bd = e_border_find_by_client_window(ev->win))) return ECORE_CALLBACK_PASS_ON;
        EINA_LIST_FOREACH(hwins, l, hwin)
          {
             if (hwin->win->border == bd)
               {
                  hwins = eina_list_remove_list(hwins, hwins);
                  e_object_del(E_OBJECT(hwin));
                  break;
               }
          }
     }
   return ECORE_CALLBACK_PASS_ON;
}


static Eina_Bool
_elfe_home_cb_bg_change(void *data __UNUSED__, int type, void *event __UNUSED__)
{
   Elfe_Home_Win *hwin;
   Eina_List *l;

   if (type != E_EVENT_BG_UPDATE) return ECORE_CALLBACK_PASS_ON;

   EINA_LIST_FOREACH(hwins, l, hwin)
     {
	E_Zone *zone;
	E_Desk *desk;
	const char *bgfile;

	zone = hwin->zone;
	desk = e_desk_current_get(zone);
	if (desk)
	  bgfile = e_bg_file_get(zone->container->num, zone->num, desk->x, desk->y);
	else
	  bgfile = e_bg_file_get(zone->container->num, zone->num, -1, -1);
	edje_object_file_set(hwin->o_bg, bgfile, "e/desktop/background");
     }

   return ECORE_CALLBACK_PASS_ON;
}
