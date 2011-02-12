#include <e.h>
#include <X11/Xlib.h>

#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define AUTOHIDE_NONE 0
#define AUTOHIDE_NORMAL 1
#define AUTOHIDE_FULLSCREEN 2

#define ITEM_MOUSE_DOWN(_item, _ev)     \
  if(_item && _item->cb_mouse_down)     \
    _item->cb_mouse_down(_item, _ev);   \

#define ITEM_MOUSE_UP(_item, _ev)       \
  if(_item && _item->cb_mouse_up)       \
    _item->cb_mouse_up(_item, _ev);     \

#define ITEM_MOUSE_OUT(_item)           \
  if(_item && _item->cb_mouse_out)      \
    _item->cb_mouse_out(_item);         \

#define ITEM_MOUSE_IN(_item)            \
  if(_item && _item->cb_mouse_in)       \
    _item->cb_mouse_in(_item);          \

#define ITEM_MOUSE_WHEEL(_item, ev)	\
  if(_item && _item->cb_mouse_wheel)    \
    _item->cb_mouse_wheel(_item, ev);	\

#define ITEM_DRAG_START(_item)          \
  if(_item && _item->cb_drag_start)     \
    _item->cb_drag_start(_item);        \

#define ITEM_FREE(_item)                \
  if(_item && _item->cb_free)           \
    _item->cb_free(_item);              \
  _item = NULL;                         \


typedef struct _Config        Config;
typedef struct _Config_Item   Config_Item;
typedef struct _Config_Box    Config_Box;
typedef struct _Config_Gadcon Config_Gadcon;

typedef struct _Ng            Ng;
typedef struct _Ngi_Item      Ngi_Item;
typedef struct _Ngi_Win       Ngi_Win;
typedef struct _Ngi_Box       Ngi_Box;

struct _Config
{
  E_Module      *module;
  Eina_List     *instances;
  E_Menu        *menu;

  Eina_List     *items;
  char          *theme_path;

  int            use_composite;
  E_Config_Dialog *cfd;
  Eina_List       *handlers;
};

struct _Config_Item
{
  Ng            *ng;

  int            show_label;
  int            show_background;
  int            container;
  int            zone;
  int            orient;
  int            size;
  int            window_size;
  int	         autohide;
  int	         autohide_show_urgent;
  int            hide_below_windows;
  float          zoomfactor;
  float          zoom_range;
  float	         hide_timeout;
  float	         zoom_duration;
  int            alpha;
  int            sia_remove; /* remove single instance apps from launcher while running */
  int            zoom_one;   /* zoom only the icon under the pointer */
  int            mouse_over_anim;

  enum { above_all, below_fullscreen } stacking;

  int            lock_deskswitch;
  int            ecomorph_features;

  Eina_List     *boxes;

  E_Config_Dialog *config_dialog;
};

struct _Config_Box
{
  enum { launcher, taskbar, gadcon } type;

  int            taskbar_skip_dialogs;
  int            taskbar_adv_bordermenu;
  int            taskbar_show_iconified;
  int            taskbar_show_desktop;
  int            taskbar_append_right;
  int            taskbar_group_apps;

  const char    *launcher_app_dir;
  int            launcher_lock_dnd;

  Eina_List     *gadcon_items;

  Ngi_Box       *box;
};

struct _Config_Gadcon
{
  const char    *name;
};



struct _Ngi_Win
{
  Ng             *ng;
  E_Popup        *popup;
  Ecore_X_Window  input;
  XRectangle      rect;
  E_Win          *fake_iwin;
  E_Object       *drop_win;
};



struct _Ng
{
  Ngi_Win         *win;
  Evas            *evas;
  Eina_List       *boxes;

  Config_Item     *cfg;

  E_Zone          *zone;

  Evas_Object     *o_bg;
  Evas_Object     *o_frame;
  Evas_Object     *o_label;
  Evas_Object     *o_event;
  Evas_Object     *clip;
  Evas_Object     *bg_clip;

  unsigned char    changed : 1;

  int              pos;  /* mouse position    */
  int              size; /* current icon size */

  enum { unzoomed, zooming, zoomed, unzooming } state;
  enum { hidden, hiding, showing, show } hide_state;
  int             hide_fullscreen;

  double           zoom;
  double           start_time;

  Eina_List       *items_scaling;
  Ngi_Item        *item_active;

  int              hide_step;

  int              w, h, start;
  int              horizontal;

  int              mouse_in;
  int              show_bar;

  int              dnd;
  Ngi_Item        *item_drag;

  Eina_List       *handlers;

  Ecore_Animator  *animator;
  Ecore_Timer     *mouse_in_timer;
  Ecore_Timer     *menu_wait_timer;
  Ecore_Timer     *effect_timer;

  struct
  {
    int clip_separator;
    int separator_width;
    int item_spacing;
    int edge_spacing;
  } opt;
};

struct _Ngi_Item
{
  enum { taskbar_item, launcher_item, gadcon_item, transient_item } type;

  enum { normal, appearing, disappearing, bouncing } state;

  Ngi_Box        *box;

  Evas_Object    *obj;
  Evas_Object    *over;
  Evas_Object    *o_icon;
  Evas_Object    *o_icon2;

  /* FIXME use union for different types */
  E_Border       *border;
  Efreet_Desktop *app;
  E_Gadcon       *gadcon;
  Config_Gadcon  *cfg_gadcon;

  const char     *label;

  const char     *class; /* store icccm.class...*/

  unsigned int    mouse_down;
  unsigned int    usable;

  int             pos;
  double          start_time;

  double          scale;

  struct
  {
    unsigned char  start : 1;
    unsigned char  dnd : 1;
    int            x, y;
    int            dx, dy;
  } drag;

  unsigned int border_was_fullscreen;
  unsigned int urgent;

  Ecore_Timer *overlay_signal_timer;

  /* void (*cb_free)       (Ngi_Item *it); */
  void (*cb_mouse_down) (Ngi_Item *it, Ecore_Event_Mouse_Button *ev);
  void (*cb_mouse_up)   (Ngi_Item *it, Ecore_Event_Mouse_Button *ev);
  void (*cb_mouse_in)   (Ngi_Item *it);
  void (*cb_mouse_out)  (Ngi_Item *it);
  void (*cb_drag_start) (Ngi_Item *it);

  int size;

  int visible;
};

struct _Ngi_Box
{
  Ng              *ng;
  Config_Box      *cfg;
  Eina_List       *items;
  Evas_Coord       start, w, h;
  Ecore_Timer     *dnd_timer;
  E_Drop_Handler  *drop_handler;
  Ngi_Item        *item_drop;
  Evas_Object     *separator;
  E_Order         *apps;

  int              pos;
};

EAPI extern E_Module_Api e_modapi;
EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);
EAPI int   e_modapi_about    (E_Module *m);
EAPI int   e_modapi_config   (E_Module *m);

EAPI E_Config_Dialog *ngi_instances_config(E_Container *con, const char *params);



Ng          *ngi_new                          (Config_Item *ci);
void         ngi_free                         (Ng *ng);
void         ngi_reposition                   (Ng *ng);
void         ngi_input_extents_calc           (Ng *ng);
void         ngi_freeze                       (Ng *ng);
void         ngi_thaw                         (Ng *ng);
void         ngi_animate                      (Ng *ng);
void         ngi_mouse_in                     (Ng *ng);
void         ngi_mouse_out                    (Ng *ng);
void         ngi_bar_show                     (Ng *ng);
void         ngi_bar_hide                     (Ng *ng);


void         ngi_win_show                     (Ngi_Win *win);
void         ngi_win_hide                     (Ngi_Win *win);
void         ngi_win_position_calc            (Ngi_Win *win);

Ngi_Box     *ngi_box_new                      (Ng *ng);
void         ngi_box_free                     (Ngi_Box *box);
void         ngi_box_item_remove              (Ng *ng, Ngi_Item *it, int instant);
void         ngi_box_item_show                (Ng *ng, Ngi_Item *it, int instant);
Ngi_Item    *ngi_box_item_at_position_get     (Ngi_Box *box);

void         ngi_configure_module             (Config_Item *ci);
void         ngi_configure_box                (Ngi_Box *box);

Ngi_Item    *ngi_item_new                     (Ngi_Box *box);
void         ngi_item_free                    (Ngi_Item *it);
void         ngi_item_remove                  (Ngi_Item *it);
void         ngi_item_del_icon                (Ngi_Item *it);
void         ngi_item_mouse_in                (Ngi_Item *it);
void         ngi_item_mouse_out               (Ngi_Item *it);
void         ngi_item_signal_emit             (Ngi_Item *it, char *sig);
void         ngi_item_activate                (Ng *ng);

void         ngi_launcher_new                 (Ng *ng, Config_Box *cfg_box);
void         ngi_launcher_remove              (Ngi_Box *box);
int          ngi_launcher_handle_border_event (Ng *ng, E_Border *bd, int remove);

void         ngi_taskbar_init                 (void);
void         ngi_taskbar_new                  (Ng *ng, Config_Box *cfg_box);
void         ngi_taskbar_remove               (Ngi_Box *box);
void         ngi_taskbar_fill                 (Ngi_Box *box);
void         ngi_taskbar_item_border_show     (Ngi_Item *it, int to_desk);

void         ngi_gadcon_init                  (void);
void         ngi_gadcon_shutdown              (void);
void         ngi_gadcon_new                   (Ng *ng, Config_Box *cfg_box);
void         ngi_gadcon_remove                (Ngi_Box *box);
void         ngi_gadcon_config                (Ngi_Box *box);

void         ngi_border_menu_show             (Ngi_Box *box, E_Border *bd,
                                               Evas_Coord x, Evas_Coord y,
                                               int dir, Ecore_X_Time timestamp);

Eina_Bool    ngi_object_theme_set(Evas_Object *obj, const char *part);

extern Config *ngi_config;
extern E_Config_DD *ngi_conf_edd;
extern E_Config_DD *ngi_conf_item_edd;
extern E_Config_DD *ngi_conf_box_edd;
extern E_Config_DD *ngi_conf_gadcon_edd;
#endif
