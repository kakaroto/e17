#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define MOD_CONFIG_FILE_EPOCH 0x0004
#define MOD_CONFIG_FILE_GENERATION 0x0001
#define MOD_CONFIG_FILE_VERSION					\
  ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)


#include <libintl.h>
#define D_(str) dgettext(PACKAGE, str)

typedef struct _Config_Item Config_Item;
typedef struct _Config Config;
typedef struct _E_Comp_Win E_Comp_Win;

struct _Config
{
  E_Module	*module;
  Eina_List	*conf_items;
  int		 version;
  E_Config_Dialog *cfd;
  const char    *theme_path;

  unsigned char tight;
  unsigned char grow;
  unsigned char fade_popups;
  unsigned char fade_windows;
  unsigned char fade_desktop;
  double	scale_duration;
  double	spacing;
  double	desks_duration;
  double	desks_spacing;
  int           layout_mode;
  int           desks_layout_mode;
};


struct _Config_Item
{
  const char *id;
  int switch2;
};


struct _E_Comp_Win
{
  EINA_INLIST;

  void			*c;
  Ecore_X_Window	 win;
  E_Border		*bd;
  E_Popup		*pop;
  E_Menu		*menu;
  int			 x, y, w, h;
  struct {
    int			 x, y, w, h;
  } hidden;
  int			 pw, ph;
  int			 border;
  Ecore_X_Pixmap	 pixmap;
  Ecore_X_Damage	 damage;
  Ecore_X_Visual	 vis;
  int			 depth;
  Evas_Object		*obj;
  Evas_Object		*shobj;
  Eina_List		*obj_mirror;
  Ecore_X_Image		*xim;
  void			*up;
  E_Object_Delfn	*dfn;
  Ecore_X_Sync_Counter	 counter;
  Ecore_Timer		*update_timeout;
  Ecore_Timer		*ready_timeout;
  int			 dmg_updates;
  Ecore_X_Rectangle	*rects;
  int			 rects_num;

  Ecore_X_Pixmap        cache_pixmap;
  int                   cache_w, cache_h;
  int                   update_count;
  double                last_visible_time;
  double                last_draw_time;

  int                   pending_count;

  char                 *title, *name, *clas, *role;
  Ecore_X_Window_Type   primary_type;

  Eina_Bool             delete_pending : 1;
  Eina_Bool             hidden_override : 1;

  Eina_Bool             animating : 1;
  Eina_Bool             force : 1;
  Eina_Bool             defer_hide : 1;
  Eina_Bool             delete_me : 1;

  Eina_Bool             visible : 1;
  Eina_Bool             input_only : 1;
  Eina_Bool             override : 1;
  Eina_Bool             argb : 1;
  Eina_Bool             shaped : 1;
  Eina_Bool             update : 1;
  Eina_Bool             redirected : 1;
  Eina_Bool             shape_changed : 1;
  Eina_Bool             native : 1;
  Eina_Bool             drawme : 1;
  Eina_Bool             invalid : 1;
  Eina_Bool             nocomp : 1;
  Eina_Bool             needpix : 1;
  Eina_Bool             needxim : 1;
  Eina_Bool             real_hid : 1;
  Eina_Bool             inhash : 1;
  Eina_Bool             show_ready : 1;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);


E_Config_Dialog *e_int_config_scale_module(E_Container *con, const char *params);
void scale_run(E_Manager *man);

extern Config *scale_conf;

#endif
