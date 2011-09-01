#include <e.h>
#include <Elementary.h>

#include "main.h"
#include "elfe_config.h"
#include "allapps.h"
#include "desktop.h"
#include "winlist.h"
#include "utils.h"
#include "gadget_list.h"

#define ELFE_HOME_WIN_TYPE 0xE0b0102f

/* local structures */
typedef struct _Elfe_Home_Exec Elfe_Home_Exec;

Elfe_Home_Win *hwin;
Elm_Theme *elfe_theme;

/* local function prototypes */
static void _elfe_home_win_new(E_Zone *zone);
static void _elfe_home_win_cb_free(Elfe_Home_Win *hwin);
static void _elfe_home_win_cb_resize(E_Win *win);
static Eina_Bool _elfe_home_cb_bg_change(void *data __UNUSED__, int type, void *event __UNUSED__);

/* local variables */
static Eina_List *hwins = NULL;
static Eina_List *hdls = NULL;

/* public functions */
EAPI E_Module_Api e_modapi = { E_MODULE_API_VERSION, "Elfe" };

EAPI void *
e_modapi_init(E_Module *m)
{
   E_Manager *man;
   Eina_List *ml;

   /* Set this module to be loaded after all other modules, or we don't see
    modules loaded after this */
   e_module_priority_set(m, 100);

   if (!elfe_home_config_init(m)) return NULL;

   elfe_winlist_init();

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

   elfe_winlist_shutdown();

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
_elfe_home_win_new(E_Zone *zone)
{

   Evas *evas;
   E_Desk *desk;
   char buf[PATH_MAX];
   const char *bgfile;

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

   elfe_theme = elm_theme_new();
   /* Use specific module theme as elm theme overlay */
   elm_theme_overlay_add(elfe_theme, buf);
   elm_object_theme_set(hwin->layout, elfe_theme);

   hwin->desktop = elfe_desktop_add(hwin->layout, hwin->zone);
   elm_layout_content_set(hwin->layout, "elfe.swallow.desktop", hwin->desktop);

   evas_object_move(hwin->layout, 0, 0);
   evas_object_show(hwin->layout);

   e_win_move_resize(hwin->win, zone->x, zone->y, zone->w, (zone->h / 2));
   e_win_show(hwin->win);
   e_border_zone_set(hwin->win->border, zone);
   if (hwin->win->evas_win)
     e_drop_xdnd_register_set(hwin->win->evas_win, EINA_TRUE);

   elfe_home_winlist_show(EINA_TRUE);

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

void
elfe_home_winlist_show(Eina_Bool show)
{
    Evas_Object *o_edje;

    o_edje = elm_layout_edje_get(hwin->layout);
    if (show)
        edje_object_signal_emit(o_edje, "elfe,desktop,show", "elfe");
    else
        edje_object_signal_emit(o_edje, "elfe,desktop,hide", "elfe");
}
