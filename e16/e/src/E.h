/*****************************************************************************/
/* Enlightenment - The Window Manager that dares to do what others don't     */
/*****************************************************************************/
/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2007 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "config.h"

#include "alert.h"
#include "edebug.h"
#include "lang.h"
#include "util.h"

#define USE_EXT_INIT_WIN 1

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>

#ifdef HAVE_XSYNC
#define USE_XSYNC 1		/* Experimental */
#endif

#ifdef HAVE_XSCREENSAVER
#define USE_XSCREENSAVER 1	/* Experimental */
#endif

#ifdef HAVE_XRANDR
#ifdef HAVE_X11_EXTENSIONS_XRANDR_H
#define USE_XRANDR 1
#endif
#endif

#ifdef HAVE_COMPOSITE
#define USE_COMPOSITE 1
#endif

#ifdef HAVE_GLX
#define USE_GLX 1
#endif

#include "xwin.h"

#define ENABLE_COLOR_MODIFIERS 0	/* Not functional */

#define ENABLE_TRANSPARENCY 1
#define ENABLE_THEME_TRANSPARENCY 1

/* sgi's stdio.h has:
 * 
 * #if _SGIAPI && _NO_ANSIMODE
 * extern int      vsnprintf(char *, ssize_t, const char *, char *);
 * #endif
 * 
 * so workaround...
 */

#ifdef __sgi
#ifdef _NO_ANSIMODE
#undef _NO_ANSIMODE
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef HAVE_GETCWD
#error "ERROR: Enlightenment needs a system with getcwd() in it's libs."
#error "You may have to upgrade your Operating system, Distribution, base"
#error "system libraries etc. Please see the the instructions for your"
#error "particular Operating System or Distribution"
#endif
#ifndef HAVE_MKDIR
#error "ERROR: Enlightenment needs a system with mkdir() in it's libs."
#error "You may have to upgrade your Operating system, Distribution, base"
#error "system libraries etc. Please see the the instructions for your"
#error "particular Operating System or Distribution"
#endif

#ifndef HAVE_STRCASESTR
# define strcasestr(haystack, needle) strstr(haystack, needle)
#endif
#ifndef HAVE_STRCASECMP
# define strcasecmp(s1, s2) strcmp(s1, s2)
#endif

#define FILEPATH_LEN_MAX 4096

#ifndef MAX
#define MAX(a,b)  ((a)>(b)?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b)  ((a)<(b)?(a):(b))
#endif

#define IN_RANGE(a, b, range) \
   ((((a) >  (b)) && ((a) - (b) <= (range))) || \
   (((a) <= (b)) && ((b) - (a) <= (range))))

#define IN_ABOVE(a, b, range) \
   (((a) >=  (b)) && ((a) - (b) <= (range)))

#define IN_BELOW(a, b, range) \
   (((a) <= (b)) && ((b) - (a) <= (range)))

#define SPANS_COMMON(x1, w1, x2, w2) \
   (!((((x2) + (w2)) <= (x1)) || ((x2) >= ((x1) + (w1)))))

#define STATE_NORMAL            0
#define STATE_HILITED           1
#define STATE_CLICKED           2
#define STATE_DISABLED          3

#define FLAG_BUTTON             0
#define FLAG_TITLE              1
#define FLAG_MINIICON           2
#define FLAG_FIXED              4
#define FLAG_FIXED_HORIZ        8
#define FLAG_FIXED_VERT         16

#define MODE_FOCUS_POINTER      0
#define MODE_FOCUS_SLOPPY       1
#define MODE_FOCUS_CLICK        2

#define DOCK_LEFT               0
#define DOCK_RIGHT              1
#define DOCK_UP                 2
#define DOCK_DOWN               3

#define ICON_LEFT               0
#define ICON_RIGHT              1
#define ICON_UP                 2
#define ICON_DOWN               3

#define MODE_NONE                 0
#define MODE_MOVE_PENDING         1
#define MODE_MOVE                 2
#define MODE_RESIZE               3
#define MODE_RESIZE_H             4
#define MODE_RESIZE_V             5
#define MODE_DESKDRAG             6
#define MODE_BUTTONDRAG           7
#define MODE_DESKRAY              8
#define MODE_PAGER_DRAG_PENDING   9
#define MODE_PAGER_DRAG          10

#define EVENT_MOUSE_DOWN  0
#define EVENT_MOUSE_UP    1
#define EVENT_MOUSE_ENTER 2
#define EVENT_MOUSE_LEAVE 3
#define EVENT_KEY_DOWN    4
#define EVENT_KEY_UP      5
#define EVENT_DOUBLE_DOWN 6
#define EVENT_FOCUS_IN    7
#define EVENT_FOCUS_OUT   8

/* Server extensions */
#define XEXT_SHAPE       0
#define XEXT_SYNC        1
#define XEXT_SCRSAVER    2
#define XEXT_RANDR       3
#define XEXT_COMPOSITE   4
#define XEXT_DAMAGE      5
#define XEXT_FIXES       6
#define XEXT_RENDER      7
#define XEXT_GLX         8

#define XEXT_CM_ALL     16

#define XEXT_AVAILABLE(ext)  (Mode.server.extensions & (1 << ext))

/*
 * Types
 */
#include "etypes.h"

typedef struct _client Client;

typedef struct
{
   int                 min, max;
}
Constraints;

typedef struct
{
   void                (*func) (const char *line, Client * c);
   const char         *name;
   const char         *nick;
   const char         *help_text;
   const char         *extended_help_text;
}
IpcItem;

typedef struct
{
   int                 num;
   const IpcItem      *lst;
} IpcItemList;

typedef struct
{
   Window              xwin;
   Win                 win;
   Visual             *vis;
   int                 depth;
   Colormap            cmap;
   int                 scr;
   int                 w, h;
}
RealRoot;

typedef struct
{
   Window              xwin;
   Win                 win;
   Visual             *vis;
   int                 depth;
   Colormap            cmap;
   int                 scr;
   int                 w, h;
}
VirtRoot;

/* Configuration parameters */
typedef struct
{
   struct
   {
      unsigned int        step;	/* Animation time step, ms */
   } animation;
   struct
   {
      char                enable;
      int                 delay;	/* milliseconds */
   } autoraise;
   struct
   {
      char                hiquality;
      char                user;
      int                 timeout;
   } backgrounds;
   struct
   {
      int                 move_resistance;
   } buttons;
   struct
   {
      unsigned int        num;
      int                 dragdir;
      int                 dragbar_width;
      int                 dragbar_length;
      int                 dragbar_ordering;
      char                desks_wraparound;
      char                slidein;
      int                 slidespeed;
      int                 areas_nx;
      int                 areas_ny;
      char                areas_wraparound;
      int                 edge_flip_resistance;
   } desks;
   struct
   {
      char                headers;
      char                button_image;
   } dialogs;
   struct
   {
      char                enable;	/* wmdockapp only */
      char                sticky;	/* Make dockapps sticky by default */
      int                 dirmode;
      int                 startx;
      int                 starty;
   } dock;
   struct
   {
      int                 mode;
      char                clickraises;
      char                transientsfollowleader;
      char                switchfortransientmap;
      char                all_new_windows_get_focus;
      char                new_transients_get_focus;
      char                new_transients_get_focus_if_group_focused;
      char                raise_on_next;
      char                warp_on_next;
      char                warp_always;
   } focus;
   struct
   {
      char                set_xroot_info_on_root_window;
   } hints;
   struct
   {
      char                animate;
      char                onscreen;
      char                warp;
      char                show_icons;
      int                 icon_size;
      struct
      {
	 KeySym              left, right, up, down, escape, ret;
      } key;
   } menus;
   struct
   {
      int                 mode_move;
      int                 mode_resize;
      int                 mode_info;
      char                update_while_moving;
      char                enable_sync_request;
   } movres;
   struct
   {
      int                 movres;
      int                 menus;
      int                 tooltips;
   } opacity;
   struct
   {
      char                manual;
      char                manual_mouse_pointer;
      char                slidein;
      char                cleanupslide;
      int                 slidemode;
      int                 slidespeedmap;
      int                 slidespeedcleanup;
      char                ignore_struts;
      char                raise_fullscreen;
#ifdef HAVE_XINERAMA
      char                extra_head;	/* Not used */
#endif
   } place;
   struct
   {
      char                enable_script;
      char               *script;
      char                enable_logout_dialog;
      char                enable_reboot_halt;
      char               *cmd_reboot;
      char               *cmd_halt;
   } session;
   struct
   {
      char                animate;
      int                 speed;
   } shading;
   struct
   {
      char                enable;
      int                 edge_snap_dist;
      int                 screen_snap_dist;
   } snap;
   struct
   {
      char                firsttime;
      char                animate;
   } startup;
   struct
   {
      char                localise;
      char               *name;
      char               *extra_path;
      char               *ttfont_path;
   } theme;
#ifdef ENABLE_THEME_TRANSPARENCY
   struct
   {
      int                 alpha;
      int                 border;
      int                 widget;
      int                 iconbox;
      int                 menu;
      int                 menu_item;
      int                 tooltip;
      int                 dialog;
      int                 hilight;
      int                 pager;
      int                 warplist;
   } trans;
#endif
   struct
   {
      char                enable;
      char                showsticky;
      char                showshaded;
      char                showiconified;
      char                warpfocused;
      char                raise_on_select;
      char                warp_on_select;
      int                 icon_mode;
   } warplist;

   struct
   {
      char                argb_internal_objects;
      char                argb_internal_clients;
      char                argb_clients;
      char                argb_clients_inherit_attr;
      int                 mask_alpha_threshold;
   } testing;

   char                autosave;
   char                memory_paranoia;
   char                save_under;
}
EConf;

typedef struct
{
   struct
   {
      unsigned int        startup_id;
   } apps;
   struct
   {
      char               *name;
      char               *dir;
      char               *cache_dir;
   } conf;
   struct
   {
      char               *name;
      int                 screens;
#ifdef HAVE_XINERAMA
      char                xinerama_active;
#endif
#if USE_XSYNC
      XID                 server_time;
#endif
   } display;
   struct
   {
      Time                time;
      int                 x, y;
      int                 px, py;
      Time                last_btime;
      Window              last_bpress;
      Window              last_bpress2;
      unsigned int        last_button;
      unsigned int        last_keycode;
      char                double_click;
      char                on_screen;
   } events;
   struct
   {
      int                 server_grabbed;
      char                pointer_grab_active;
      Window              pointer_grab_window;
   } grabs;
   struct
   {
      const char         *lang;
      char                utf8_int;	/* Use UTF-8 internally */
      char                utf8_loc;	/* Locale is UTF-8 */
   } locale;
   struct
   {
      unsigned int        mod_key_mask;
      unsigned int        mod_combos[8];
   } masks;
   struct
   {
      char                check;	/* Avoid losing windows offscreen */
      char                swap;
      int                 swapcoord_x, swapcoord_y;
   } move;
   struct
   {
      char                enable_features;
      char                doing_manual;
      char                doing_slide;
   } place;
   struct
   {
      int                 w_old;
      int                 h_old;
   } screen;
   struct
   {
      unsigned int        extensions;
   } server;
   struct
   {
      Pixmap              ext_pmap;
      char                ext_pmap_valid;
   } root;
   struct
   {
      char               *path;
   } theme;
   struct
   {
      char               *exec_name;	/* argv[0] */
      char                master;	/* We are the master E */
      char                single;	/* No slaves */
      char                window;	/* Running in virtual root window */
      pid_t               pid;
      int                 master_screen;
      char                session_start;
      char                startup;
      char                restart;
      char                xselect;
      char                exiting;
      char                in_signal_handler;
      char                save_ok;
      char                coredump;
      int                 exit_mode;
      char               *exit_param;
      int                 child_count;
      pid_t              *children;
      char               *machine_name;
   } wm;
   int                 mode;
   int                 op_source;
   char                action_inhibit;
   EWin               *focuswin;
   EWin               *mouse_over_ewin;
   EWin               *context_ewin;
   Colormap            current_cmap;
   Win                 context_win;
   char                constrained;
   char                nogroup;
   char                keybinds_changed;
   char                showing_desktop;
   Window              button_proxy_win;
   char                firsttime;
}
EMode;

/*
 * Function prototypes
 */

/* about.c */
void                About(void);

/* actions.c */
#define EXEC_SET_LANG       0x01
int                 execApplication(const char *params, int flags);
void                Espawn(int argc, char **argv);
void                EspawnCmd(const char *cmd);

/* comms.c */
void                CommsInit(void);
void                CommsSend(Client * c, const char *s);
void                CommsFlush(Client * c);
void                CommsSendToMasterWM(const char *s);
void                CommsBroadcast(const char *s);
void                CommsBroadcastToSlaveWMs(const char *s);

/* config.c */
void                SkipTillEnd(FILE * ConfigFile);
char               *GetLine(char *s, int size, FILE * f);
void                ConfigAlertLoad(const char *txt);
char               *FindFile(const char *file, const char *themepath,
			     int localized);
char               *ThemeFileFind(const char *file, int localized);
char               *ConfigFileFind(const char *name, const char *themepath,
				   int pp);
int                 ConfigFileLoad(const char *name, const char *themepath,
				   int (*parse) (FILE * fs), int preparse);
int                 ConfigFileRead(FILE * fs);
int                 ThemeConfigLoad(void);
void                RecoverUserConfig(void);

/* dialog.c */
void                DialogOK(const char *title, const char *fmt, ...);
void                DialogOKstr(const char *title, const char *txt);
void                DialogAlert(const char *fmt, ...);
void                DialogAlertOK(const char *fmt, ...);

/* econfig.c */
void                ConfigurationLoad(void);
void                ConfigurationSave(void);
void                ConfigurationSet(const char *params);
void                ConfigurationShow(const char *params);

/* edge.c */
void                EdgeCheckMotion(int x, int y);
void                EdgeWindowsShow(void);
void                EdgeWindowsHide(void);

/* extinitwin.c */
Window              ExtInitWinCreate(void);
void                ExtInitWinSet(Window win);
Window              ExtInitWinGet(void);
void                ExtInitWinKill(void);

/* fonts.c */
int                 FontConfigLoad(FILE * fs);
void                FontConfigUnload(void);
const char         *FontLookup(const char *name);

/* handlers.c */
void                SignalsSetup(void);
void                SignalsRestore(void);
void                HandleXError(Display * d, XErrorEvent * ev);
void                HandleXIOError(Display * d);

/* ipc.c */
void __PRINTF__     IpcPrintf(const char *fmt, ...);
int                 HandleIPC(const char *params, Client * c);
int                 EFunc(EWin * ewin, const char *params);
void                EFuncDefer(EWin * ewin, const char *params);

/* main.c */
void                EExit(int exitcode);
const char         *EDirRoot(void);
const char         *EDirBin(void);
const char         *EDirUser(void);
const char         *EDirUserCache(void);
void                EDirMake(const char *base, const char *name);
const char         *EGetSavePrefix(void);
const char         *EGetSavePrefixCommon(void);
void                Etmp(char *s);

/* misc.c */
void                Quicksort(void **a, int l, int r,
			      int (*CompareFunc) (void *d1, void *d2));
void                ETimedLoopInit(int k1, int k2, int speed);
int                 ETimedLoopNext(void);

/* mod-misc.c */
void                autosave(void);

/* regex.c */
int                 matchregexp(const char *rx, const char *s);

/* setup.c */
void                MapUnmap(int start);
void                SetupX(const char *dstr);

/* slideouts.c */
int                 SlideoutsConfigLoad(FILE * fs);

/* sound.c */
void                SoundPlay(const char *name);

/* startup.c */
void                StartupWindowsCreate(void);
void                StartupWindowsOpen(void);

/* theme.c */
void                ThemePathFind(void);
char              **ThemesList(int *num);

/*
 * Global vars
 */
extern const char   e_wm_name[];
extern const char   e_wm_version[];
extern const char   e_wm_date[];
extern Display     *disp;
extern RealRoot     RRoot;
extern VirtRoot     VRoot;
extern EConf        Conf;
extern EMode        Mode;
