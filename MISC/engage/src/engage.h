#ifndef _OD_ENGAGE_H
#define _OD_ENGAGE_H

#define HAVE_ETK 1
#define HAVE_IMLIB 1

#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Ecore_Config.h>
#include <Edje.h>
#include <stdlib.h>
#include <stdio.h>

#include "e.h"
#include "config.h"

#define ARROW_SPACE 6
#define ARROW_SPACE_DOUBLE 12 /* ARROW_SPACE * 2 is less efficient */

typedef enum { false = 0, true = 1 } bool;
typedef struct _OD_Options OD_Options;
typedef struct _OD_Window OD_Window;
typedef struct _OD_Dock OD_Dock;
typedef struct _OD_Icon OD_Icon;

typedef enum {
  OM_ONTOP = 0,
  OM_BELOW = 1
} OD_Mode;

extern OD_Options options;
extern OD_Dock  dock;
extern Eina_List *clients;

extern Ecore_Evas *ee;
extern Evas    *evas;
extern Ecore_X_Window od_window;
extern bool     need_redraw;
extern int      tray_width;

extern E_App   *od_unmatched_app;

int             od_config_init();
#ifdef HAVE_ETK
void            od_config_menu_draw(Evas_Coord x, Evas_Coord y);
#endif

void            od_window_init();
void            od_window_hide();
void            od_window_unhide();

void            od_dock_init();
void            od_dock_add_applnk(OD_Icon * applnk);
void            od_dock_add_minwin(OD_Icon * minwin);
void            od_dock_add_sysicon(OD_Icon * sysicon);
void            od_dock_del_icon(OD_Icon * icon);
void            od_dock_redraw(Ecore_Evas * ee);
void            od_dock_zoom_in();
void            od_dock_zoom_out();
void            od_dock_reposition();

void            od_tray_init();
void            od_tray_move(double x);

void            od_icon_mapping_add(E_App *app);
E_App          *od_icon_mapping_get(const char *winclass);
OD_Icon        *od_icon_new_applnk(E_App *app, char *name_override, char *class_override);
OD_Icon        *od_icon_new_sysicon(const char *name, const char *icon_path);
OD_Icon        *od_icon_new_minwin(Ecore_X_Window win, char *name_override, char *class_override);

void            od_icon_grab(OD_Icon * icon, Ecore_X_Window win);
void            od_icon_reload(OD_Icon * in);

void            od_icon_del(OD_Icon * od_icon);
void            od_icon_arrow_show(OD_Icon * icon);
void            od_icon_arrow_hide(OD_Icon * icon);
void            od_icon_name_change(OD_Icon * icon, const char *name);
void            od_icon_add_path(const char *path);
void            od_icon_add_kde_set(const char *path);

void            od_hex_to_argb(unsigned int h, int *a, int *r, int *g, int *b);

int             userconfig_load();

void            od_dock_icons_update_begin();
char           *od_wm_get_winclass(Ecore_X_Window win); // remember to free
char           *od_wm_get_title(Ecore_X_Window win);    // ditto
bool            od_wm_iconified(Ecore_X_Window win);
void            od_wm_activate_window(Ecore_X_Window win);
void            od_wm_deactivate_window(Ecore_X_Window win);
OD_Window      *od_wm_window_next_by_window_class_get(const char *);
OD_Window      *od_wm_window_prev_by_window_class_get(const char *);
OD_Window      *od_wm_window_current_by_window_class_get(const char *);

struct _OD_Options {
  int             width, height, reserve, zoom, stretch;
#ifdef XINERAMA
  int             head;
#endif
  char           *icon_path, *theme, *engine;
  OD_Mode         mode;
  int             grab_min_icons, grab_app_icons;

  int             auto_hide;
  int             size, spacing;    // icon-*
  double          zoomfactor, dock_zoom_duration;

  unsigned        bg_fore, bg_back; // color - ARGB
  double          icon_appear_duration;
  int             tray, ignore_run, ignore_min;
  int			  use_composite;
};

struct _OD_Window {
  Ecore_X_Window  id;
  struct _OD_Icon *applnk;
  struct _OD_Icon *minwin;
};

#define OD_BG_LEFT   0
#define OD_BG_RIGHT  1
#define OD_BG_FILL   2
#define OD_BG_MIDDLE 3
#define OD_BG_MIDDLE2 4
struct _OD_Dock {
  Eina_List      *icons, *applnks, *minwins, *sysicons;
  enum { unzoomed, zooming, zoomed, unzooming } state;
  double          zoom;
  double          x;            // current pointer x position
  Evas_Object    *background[5];        // left, right, background, middle, see above
  double          left_pos, right_pos, middle_pos, middle2_pos;      // positions of the vertical bars
  double          left_end, right_end;  // full extent, used for testing for "in"-ness of mouse pointer
};

#define OD_ICON_STATE_USEABLE                  0x00000001
#define OD_ICON_STATE_APPEARING                0x00000002
#define OD_ICON_STATE_DISAPPEARING             0x00000004
#define OD_ICON_STATE_BOUNCING                 0x00000008

struct _OD_Icon {
  E_App          *a;
  Evas_Object    *icon, *pic;
  unsigned        state;        // see above
  char           *name, *icon_file, *winclass;
  double          x, y;         // relative to evas
  double          scale;
  double          start_time;
  Ecore_Timer    *appear_timer, *disappear_timer;
  int             launcher;
  enum { application_link, minimised_window, system_icon } type;
  union {
    struct {
      char           *command;
      char           *winclass;
      int             count;
    } applnk;
    struct {
      Ecore_X_Window  window;
    } minwin;
  } data;
};

#endif // _OD_ENGAGE_H
