#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define ELFE_DESKTOP_NUM 5
#define ELFE_DESKTOP_PADDING_W 10
#define ELFE_DESKTOP_PADDING_H 10

typedef struct _Elfe_Home_Win Elfe_Home_Win;

struct _Elfe_Home_Win
{
   E_Object e_obj_inherit;

   E_Win *win;
   Evas_Object *o_bg;
   Evas_Object *bg_scroller;
   Evas_Object *layout;
   Evas_Object *desktop;

   E_Zone *zone;

};


EAPI extern E_Module_Api e_modapi;
EAPI extern Elfe_Home_Win *hwin;
EAPI extern Elm_Theme *elfe_theme;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

void elfe_home_win_cfg_update(void);
void elfe_home_winlist_show(Eina_Bool show);

#endif
