#ifndef _OD_ENGAGE_H
#define _OD_ENGAGE_H

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Config.h>
#include <stdlib.h>
#include <stdio.h>

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
extern Evas_List *clients;

extern Ecore_Evas *ee;
extern Evas    *evas;
extern Ecore_X_Window od_window;
extern bool     need_redraw;

void            od_config_init();

void            od_window_init();

void            od_dock_init();
void            od_dock_add_applnk(OD_Icon * applnk);
void            od_dock_add_dicon(OD_Icon * dicon);
void            od_dock_add_minwin(OD_Icon * minwin);
void            od_dock_del_icon(OD_Icon * icon);
void            od_dock_redraw(Ecore_Evas * ee);
void            od_dock_zoom_in();
void            od_dock_zoom_out();

void            od_icon_mapping_add(const char *winclass, const char *name,
                                    const char *icon_name);
OD_Icon        *od_icon_new_applnk(const char *command, const char *winclass);
OD_Icon        *od_icon_new_dicon(const char *command, const char *name,
                                  const char *icon_name);
OD_Icon        *od_icon_new_minwin(Ecore_X_Window win);
void            od_icon_del(OD_Icon * od_icon);
void            od_icon_arrow_show(OD_Icon * icon);
void            od_icon_arrow_hide(OD_Icon * icon);
void            od_icon_tt_show(OD_Icon * icon);
void            od_icon_tt_hide(OD_Icon * icon);
void            od_icon_name_change(OD_Icon * icon, const char *name);
void            od_icon_add_path(const char *path);
void            od_icon_add_kde_set(const char *path);

void            od_hex_to_argb(unsigned int h, int *a, int *r, int *g, int *b);

void            od_dock_icons_update_begin();
char           *od_wm_get_winclass(Ecore_X_Window win); // remember to free
char           *od_wm_get_title(Ecore_X_Window win);    // ditto
bool            od_wm_iconified(Ecore_X_Window win);
void            od_wm_activate_window(Ecore_X_Window win);

struct _OD_Options {
  int             width, height;
  char           *icon_path;
  OD_Mode         mode;

  int             size, spacing, arrow_size;    // icon-*
  double          zoomfactor, dock_zoom_duration;

  unsigned        tt_txt_color, tt_shd_color, bg_fore, bg_back; // color - ARGB
  char           *tt_fa;        // font name
  int             tt_fs;        // font size
  double          icon_appear_duration;
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
struct _OD_Dock {
  Evas_List      *icons, *applnks, *dicons, *minwins;
  enum { unzoomed, zooming, zoomed, unzooming } state;
  double          zoom;
  double          x;            // current pointer x position
  Evas_Object    *background[4];        // left, right, background, middle, see above
  double          left_pos, right_pos, middle_pos;      // positions of the vertical bars
  double          left_end, right_end;  // full extent, used for testing for "in"-ness of mouse pointer
};

#define OD_ICON_STATE_USEABLE                  0x00000001
#define OD_ICON_STATE_APPEARING                0x00000002
#define OD_ICON_STATE_DISAPPEARING             0x00000004
#define OD_ICON_STATE_BOUNCING                 0x00000008

struct _OD_Icon {
  Evas_Object    *icon, *tt_txt, *tt_shd, *arrow;
  unsigned        state;        // see above
  char           *name;
  double          x, y;         // relative to evas
  double          scale;
  double          start_time;
  Ecore_Timer    *appear_timer;
  enum { application_link, docked_icon, minimised_window } type;
  union {
    struct {
      char           *command;
      char           *winclass;
      int             count;
    } applnk;
    struct {
      char           *command;
    } dicon;
    struct {
      Ecore_X_Window  window;
    } minwin;
  } data;
};

#endif // _OD_ENGAGE_H
