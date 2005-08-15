/*****************************************************************************/
/* Enlightenment - The Window Manager that dares to do what others don't     */
/*****************************************************************************/
/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2005 Kim Woelders
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
#define _GNU_SOURCE
#include "config.h"

#define USE_EXT_INIT_WIN 1

#if HAVE_STRDUP
#define USE_LIBC_STRDUP  1	/* Use libc strdup if present */
#endif
#if HAVE_STRNDUP
#define USE_LIBC_STRNDUP 1	/* Use libc strndup if present */
#endif
#if HAVE_SETENV
#define USE_LIBC_SETENV  1	/* Use libc setenv  if present */
#endif
#define USE_LIBC_MALLOC  1	/* Use unwrapped libc malloc/realloc/free */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/Xlocale.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XShm.h>

#ifdef HAS_XRANDR
#ifdef HAVE_X11_EXTENSIONS_XRANDR_H
#define USE_XRANDR 1
#endif
#endif

#ifdef HAS_COMPOSITE
#define USE_COMPOSITE 1
#endif

#include <Imlib2.h>

#define ENABLE_COLOR_MODIFIERS 0	/* Not functional */

#define ENABLE_TRANSPARENCY 1
#define ENABLE_THEME_TRANSPARENCY 1

#define ICLASS_ATTR_OPAQUE      0x00	/* No transparency */
#define ICLASS_ATTR_BG          0x01	/* Background transparency */
#define ICLASS_ATTR_GLASS       0x02	/* Glass transparency */
#define ICLASS_ATTR_NO_CLIP     0x04	/* Don't apply clip mask */
#define ICLASS_ATTR_USE_CM      0x08	/* Use colormodifier */

#ifdef HAS_XINERAMA
#include <X11/extensions/Xinerama.h>
#endif

/* dmalloc debugging */
/*#include <dmalloc.h> */

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

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#define __PRINTF__ __attribute__((__format__(__printf__, 1, 2)))
#else
#define __UNUSED__
#define __PRINTF__
#endif

/* workaround for 64bit architectures - xlib expects 32bit CARDINALS to be */
/* long's on 64bit machines... thus well the CARD32's Im unsing shoudl be.. */
#define CARD32 long

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

#define FILEPATH_LEN_MAX 4096

#ifdef HAVE_SNPRINTF
#define Evsnprintf vsnprintf
#define Esnprintf snprintf
#else /* HAVE_SNPRINTF */
int                 Evsnprintf(char *str, size_t count, const char *fmt,
			       va_list args);

#ifdef HAVE_STDARG_H
int                 Esnprintf(char *str, size_t count, const char *fmt, ...);

#else
int                 Esnprintf(va_alist);
#endif
#endif /* HAVE_SNPRINTF */

/* This is a start to providing internationalization by means */
/* of gettext */

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <libintl.h>
#define _(String) gettext(String)
#ifdef gettext_noop
#define N_(String) gettext_noop(String)
#else
#define N_(String) (String)
#endif

#include "eobj.h"

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

#define LIST_FINDBY_NAME        0
#define LIST_FINDBY_ID          1
#define LIST_FINDBY_BOTH        2
#define LIST_FINDBY_NONE        3
#define LIST_FINDBY_POINTER     4

#define LIST_TYPE_ANY            0
#define LIST_TYPE_CLIENT         1
#define LIST_TYPE_BORDER         3
#define LIST_TYPE_ICLASS         4
#define LIST_TYPE_ACLASS         5
#define LIST_TYPE_ACLASS_GLOBAL  7
#define LIST_TYPE_TCLASS         9
#define LIST_TYPE_BACKGROUND    10
#define LIST_TYPE_BUTTON        11
#define LIST_TYPE_SCLASS        12
#define LIST_TYPE_WINDOWMATCH   13
#define LIST_TYPE_COLORMODIFIER 14
#define LIST_TYPE_SLIDEOUT      16
#define LIST_TYPE_TOOLTIP       18
#define LIST_TYPE_MENU          21
#define LIST_TYPE_MENU_STYLE    22
#define LIST_TYPE_ECURSOR       23
#define LIST_TYPE_SNAPSHOT      24
#define LIST_TYPE_DIALOG        25
#define LIST_TYPE_FONT          26
#define LIST_TYPE_PAGER         27
#define LIST_TYPE_ICONBOX       28
#define LIST_TYPE_GROUP         32

#define LIST_TYPE_COUNT         33

#define BEVEL_NONE              0
#define BEVEL_AMIGA             1
#define BEVEL_MOTIF             2
#define BEVEL_NEXT              3
#define BEVEL_DOUBLE            4
#define BEVEL_WIDEDOUBLE        5
#define BEVEL_THINPOINT         6
#define BEVEL_THICKPOINT        7

#define EWIN_NORMAL             0
#define EWIN_ACTIVE             1
#define EWIN_STICKY             2
#define EWIN_ICONIFIED          4

#define STATE_NORMAL            0
#define STATE_HILITED           1
#define STATE_CLICKED           2
#define STATE_DISABLED          3

#define FILL_STRETCH            0
#define FILL_TILE_H             1
#define FILL_TILE_V             2
#define FILL_INT_TILE_H         4
#define FILL_INT_TILE_V         8

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

#define GROUP_SELECT_ALL             0
#define GROUP_SELECT_EWIN_ONLY       1
#define GROUP_SELECT_ALL_EXCEPT_EWIN 2

#define GROUP_FEATURE_BORDER  1
#define GROUP_FEATURE_KILL    2
#define GROUP_FEATURE_MOVE    4
#define GROUP_FEATURE_RAISE   8
#define GROUP_FEATURE_ICONIFY 16
#define GROUP_FEATURE_STICK   32
#define GROUP_FEATURE_SHADE   64
#define GROUP_FEATURE_MIRROR  128

/* For window group listing */
#define GROUP_ACTION_ANY                     0
#define GROUP_ACTION_MOVE                    1
#define GROUP_ACTION_RAISE                   2
#define GROUP_ACTION_LOWER                   3
#define GROUP_ACTION_KILL                    4
#define GROUP_ACTION_STICK                   5
#define GROUP_ACTION_ICONIFY                 6
#define GROUP_ACTION_SHADE                   7
#define GROUP_ACTION_SET_WINDOW_BORDER       8
#define GROUP_ACTION_RAISE_LOWER             9

#define SET_OFF    0
#define SET_ON     1
#define SET_TOGGLE 2

#define ST_UNKNWN	0
#define ST_BORDER	1
#define ST_WIDGET	2
#define ST_ICONBOX	3
#define ST_MENU		4
#define ST_MENU_ITEM	5
#define ST_TOOLTIP	6
#define ST_DIALOG	7
#define ST_HILIGHT	8
#define ST_PAGER	9
#define ST_WARPLIST	10
#define ST_BUTTON	11

/*
 * Types
 */

typedef struct _ewin EWin;
typedef struct _menu Menu;
typedef struct _menuitem MenuItem;
typedef struct _menustyle MenuStyle;
typedef struct _dialog Dialog;
typedef struct _ditem DItem;
typedef struct _snapshot Snapshot;
typedef struct _group Group;
typedef struct _button Button;
typedef struct _background Background;
typedef struct _ecursor ECursor;
typedef struct _efont Efont;
typedef struct _textclass TextClass;
typedef struct _action Action;
typedef struct _actionclass ActionClass;
typedef struct _list List;

typedef struct
{
   char                type;
   Pixmap              pmap;
   Pixmap              mask;
   int                 w, h;
}
PmapMask;

typedef struct _client Client;

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
   Window              win;
   Visual             *vis;
   int                 depth;
   Colormap            cmap;
   int                 scr;
   int                 w, h;
}
RealRoot;

typedef struct
{
   Window              win;
   Visual             *vis;
   int                 depth;
   Colormap            cmap;
   int                 scr;
   int                 w, h;
}
VirtRoot;

#if ENABLE_COLOR_MODIFIERS
typedef struct _modcurve
{
   int                 num;
   unsigned char      *px;
   unsigned char      *py;
   unsigned char       map[256];
}
ModCurve;

typedef struct _colormodifierclass
{
   char               *name;
   ModCurve            red, green, blue;
   unsigned int        ref_count;
}
ColorModifierClass;
#endif

typedef struct _imagestate
{
   char               *im_file;
   char               *real_file;
   char                unloadable;
   char                transparent;
   Imlib_Image        *im;
   Imlib_Border       *border;
   int                 pixmapfillstyle;
   XColor              bg, hi, lo, hihi, lolo;
   int                 bevelstyle;
#if ENABLE_COLOR_MODIFIERS
   ColorModifierClass *colmod;
#endif
}
ImageState;

typedef struct _ImageStateArray
{
   ImageState         *normal;
   ImageState         *hilited;
   ImageState         *clicked;
   ImageState         *disabled;
}
ImageStateArray;

typedef struct _imageclass
{
   char               *name;
   ImageStateArray     norm, active, sticky, sticky_active;
   Imlib_Border        padding;
#if ENABLE_COLOR_MODIFIERS
   ColorModifierClass *colmod;
#endif
   unsigned int        ref_count;
}
ImageClass;

#define MODE_VERBATIM  0
#define MODE_WRAP_CHAR 1
#define MODE_WRAP_WORD 2

#define FONT_TO_RIGHT 0
#define FONT_TO_DOWN  1
#define FONT_TO_UP    2
#define FONT_TO_LEFT  3

typedef struct _textstate
{
   char               *fontname;
   struct
   {
      char                mode;
      char                orientation;
   } style;
   XColor              fg_col;
   XColor              bg_col;
   int                 effect;
   Efont              *efont;
   XFontStruct        *xfont;
   XFontSet            xfontset;
   int                 xfontset_ascent;
   char                need_utf8;
}
TextState;

struct _textclass
{
   char               *name;
   struct
   {
      TextState          *normal;
      TextState          *hilited;
      TextState          *clicked;
      TextState          *disabled;
   }
   norm               , active, sticky, sticky_active;
   int                 justification;
   unsigned int        ref_count;
};

typedef struct
{
   EObj                o;
   int                 num;
   char                viewable;
   char                dirty_stack;
   Background         *bg;
   Button             *tag;
   int                 current_area_x;
   int                 current_area_y;
   long                event_mask;
} Desk;

typedef struct _constraints
{
   int                 min, max;
}
Constraints;

typedef struct _winpoint
{
   int                 originbox;
   struct
   {
      int                 percent;
      int                 absolute;
   }
   x                  , y;
}
WinPoint;

typedef struct _geometry
{
   Constraints         width, height;
   WinPoint            topleft, bottomright;
}
Geometry;

typedef struct _winpart
{
   Geometry            geom;
   ImageClass         *iclass;
   ActionClass        *aclass;
   TextClass          *tclass;
   ECursor            *ec;
   signed char         ontop;
   int                 flags;
   char                keep_for_shade;
}
WinPart;

typedef struct _border
{
   char               *name;
   char               *group_border_name;
   Imlib_Border        border;
   int                 num_winparts;
   WinPart            *part;
   char                changes_shape;
   char                shadedir;
   char                throwaway;
   unsigned int        ref_count;
   ActionClass        *aclass;
}
Border;

typedef struct _ewinbit
{
   EWin               *ewin;	/* Belongs to */
   Window              win;
   int                 x, y, w, h;
   int                 cx, cy, cw, ch;
   int                 state;
   char                expose;
   char                no_expose;
   char                left;
   ImageState         *is;
   TextState          *ts;
}
EWinBit;

typedef struct _groupconfig
{
   char                iconify;
   char                kill;
   char                mirror;
   char                move;
   char                raise;
   char                set_border;
   char                shade;
   char                stick;
}
GroupConfig;

struct _group
{
   int                 index;
   EWin              **members;
   int                 num_members;
   GroupConfig         cfg;
};

/* Configuration parameters */
typedef struct
{
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
      int                 num;
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
      GroupConfig         dflt;
      char                swapmove;
   } groups;
   struct
   {
      char                set_xroot_info_on_root_window;
   } hints;
   struct
   {
      char                animate;
      char                onscreen;
      char                warp;
      int                 opacity;
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
      int                 opacity;
      char                update_while_moving;
   } movres;
   struct
   {
      char                manual;
      char                manual_mouse_pointer;
      char                slidein;
      char                ignore_struts;
      char                raise_fullscreen;
   } place;
   struct
   {
      char                enable_logout_dialog;
      char                enable_reboot_halt;
      char               *cmd_init;
      char               *cmd_start;
      char               *cmd_reboot;
      char               *cmd_halt;
   } session;
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
   int                 deskmode;
   int                 slidemode;
   char                cleanupslide;
   int                 slidespeedmap;
   int                 slidespeedcleanup;
   char                animate_shading;
   int                 shadespeed;
   int                 button_move_resistance;
   char                autosave;
   char                memory_paranoia;
   char                save_under;
   int                 edge_flip_resistance;
   char                argb_client_mode;

   /* Not used */
#ifdef HAS_XINERAMA
   char                extra_head;	/* Not used */
#endif
#if 0				/* Not used */
   char                primaryicondir;
   TextClass          *icon_textclass;
   int                 icon_mode;
#endif
}
EConf;

typedef struct
{
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
#ifdef HAS_XINERAMA
      char                xinerama_active;
#endif
   } display;
   struct
   {
      int                 x, y;
      int                 px, py;
      Time                last_btime;
      Window              last_bpress;
      unsigned int        last_button;
      unsigned int        last_keycode;
      char                double_click;
   } events;
   struct
   {
      Group              *current;
   } groups;
   struct
   {
      int                 server_grabbed;
      char                pointer_grab_active;
      Window              pointer_grab_window;
   } grabs;
   struct
   {
      Pixmap              old_root_pmap;
   } hints;
   struct
   {
      const char         *lang;
      char                utf8_int;	/* Use UTF-8 internally */
      char                utf8_loc;	/* Locale is UTF-8 */
   } locale;
   struct
   {
      unsigned int        numlock;
      unsigned int        scrollock;
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
      char                save_ok;
      char                coredump;
      int                 child_count;
      pid_t              *children;
      char               *machine_name;
   } wm;
   int                 mode;
   char                flipp;
   char                action_inhibit;
   EWin               *focuswin;
   EWin               *mouse_over_ewin;
   EWin               *context_ewin;
   int                 deskdrag;
   Colormap            current_cmap;
   Window              context_win;
   char                constrained;
   char                nogroup;
   char                keybinds_changed;
   Window              button_proxy_win;
}
EMode;

typedef struct _qentry
{
   char               *name;
   double              in_time;
   void                (*func) (int val, void *data);
   struct _qentry     *next;
   int                 runtime_val;
   void               *runtime_data;
   char                just_added;
}
Qentry;

/* Dialog items */
#define DITEM_NONE         0
#define DITEM_BUTTON       1
#define DITEM_CHECKBUTTON  2
#define DITEM_TEXT         3
#define DITEM_IMAGE        4
#define DITEM_SEPARATOR    5
#define DITEM_TABLE        6
#define DITEM_RADIOBUTTON  7
#define DITEM_SLIDER       8
#define DITEM_AREA         9

/* Dialog button icons */
#define DIALOG_BUTTON_NONE   0
#define DIALOG_BUTTON_OK     1
#define DIALOG_BUTTON_CANCEL 2
#define DIALOG_BUTTON_APPLY  3
#define DIALOG_BUTTON_CLOSE  4

typedef struct _rectbox
{
   void               *data;
   int                 x, y, w, h;
   int                 p;
}
RectBox;

/*
 * Function prototypes
 */

/* aclass.c */
int                 AclassConfigLoad(FILE * fs);
ActionClass        *ActionclassCreate(const char *name, int global);
void                ActionclassDestroy(ActionClass * ac);
Action             *ActionCreate(char event, char anymod, int mod, int anybut,
				 int but, char anykey, const char *key,
				 const char *tooltipstring);
void                ActionAddTo(Action * aa, const char *params);
void                ActionclassAddAction(ActionClass * ac, Action * aa);
void                ActionclassSetTooltipString(ActionClass * ac,
						const char *tts);
void                ActionclassIncRefcount(ActionClass * ac);
void                ActionclassDecRefcount(ActionClass * ac);
const char         *ActionclassGetName(ActionClass * ac);
const char         *ActionclassGetTooltipString(ActionClass * ac);
int                 ActionclassGetActionCount(ActionClass * ac);
Action             *ActionclassGetAction(ActionClass * ac, int ix);
int                 ActionclassEvent(ActionClass * ac, XEvent * ev,
				     EWin * ewin);
int                 ActionclassesEvent(XEvent * ev, EWin * ewin);

const char         *ActionGetTooltipString(Action * aa);
int                 ActionGetAnybutton(Action * aa);
int                 ActionGetEvent(Action * aa);
int                 ActionGetButton(Action * aa);
int                 ActionGetModifiers(Action * aa);

void                GrabButtonGrabs(EWin * ewin);
void                UnGrabButtonGrabs(EWin * ewin);

/* actions.c */
int                 ActionsSuspend(void);
int                 ActionsResume(void);
void                ActionsHandleMotion(void);
int                 ActionsEnd(EWin * ewin);
void                About(void);

int                 execApplication(const char *params);
void                Espawn(int argc, char **argv);
void                EspawnCmd(const char *cmd);

/* alert.c */
void                AlertInit(void);
void                Alert(const char *fmt, ...);
void                AlertX(const char *title, const char *ignore,
			   const char *restart, const char *quit,
			   const char *fmt, ...);

/* areas.c */
void                AreaFix(int *ax, int *ay);
void                SetNewAreaSize(int ax, int ay);
void                SetAreaSize(int aw, int ah);
void                GetAreaSize(int *aw, int *ah);
void                SetCurrentArea(int ax, int ay);
void                MoveCurrentAreaBy(int ax, int ay);
void                SetCurrentLinearArea(int a);
int                 GetCurrentLinearArea(void);
void                MoveCurrentLinearAreaBy(int a);
void                EwinMoveToLinearArea(EWin * ewin, int a);
void                EwinMoveLinearAreaBy(EWin * ewin, int a);

/* arrange.c */
#define ARRANGE_VERBATIM    0
#define ARRANGE_BY_SIZE     1
#define ARRANGE_BY_POSITION 2

void                ArrangeRects(RectBox * fixed, int fixed_count,
				 RectBox * floating, int floating_count,
				 RectBox * sorted, int startx, int starty,
				 int width, int height, int policy,
				 char initial_window);
void                SnapEwin(EWin * ewin, int dx, int dy, int *new_dx,
			     int *new_dy);
void                ArrangeEwin(EWin * ewin);
void                ArrangeEwinCentered(EWin * ewin, int focus);
void                ArrangeEwinXY(EWin * ewin, int *px, int *py);
void                ArrangeEwinCenteredXY(EWin * ewin, int *px, int *py);
void                ArrangeEwins(const char *params);

/* backgrounds.c */
int                 BackgroundsConfigLoad(FILE * fs);
char               *BackgroundGetUniqueString(Background * bg);
void                BackgroundPixmapFree(Background * bg);
void                BackgroundImagesFree(Background * bg, int free_pmap);
void                BackgroundDestroyByName(const char *name);
Pixmap              BackgroundApply(Background * bg, Drawable draw,
				    unsigned int rw, unsigned int rh,
				    int is_win);
void                BackgroundSet(Background * bg, Window win, unsigned int rw,
				  unsigned int rh);
void                BackgroundIncRefcount(Background * bg);
void                BackgroundDecRefcount(Background * bg);
void                BackgroundTouch(Background * bg);
const char         *BackgroundGetName(const Background * bg);
int                 BackgroundGetColor(const Background * bg);
Pixmap              BackgroundGetPixmap(const Background * bg);
Background         *BrackgroundCreateFromImage(const char *bgid,
					       const char *file, char *thumb,
					       int thlen);

/* borders.c */
Border             *BorderCreate(const char *name);
void                BorderDestroy(Border * b);
void                BorderIncRefcount(const Border * b);
void                BorderDecRefcount(const Border * b);
const char         *BorderGetName(const Border * b);
int                 BorderConfigLoad(FILE * fs);
void                BorderWinpartAdd(Border * b, ImageClass * ic,
				     ActionClass * aclass, TextClass * tclass,
				     ECursor * ec, char ontop, int flags,
				     char isregion, int wmin, int wmax,
				     int hmin, int hmax, int torigin, int txp,
				     int txa, int typ, int tya, int borigin,
				     int bxp, int bxa, int byp, int bya,
				     char keep_for_shade);
void                EwinBorderSelect(EWin * ewin);
void                EwinBorderDetach(EWin * ewin);
void                EwinBorderSetTo(EWin * ewin, const Border * b);
void                EwinBorderDraw(EWin * ewin, int do_shape, int do_paint);
void                EwinBorderCalcSizes(EWin * ewin, int propagate);
void                EwinBorderMinShadeSize(EWin * ewin, int *mw, int *mh);
void                EwinBorderUpdateInfo(EWin * ewin);
void                EwinBorderUpdateState(EWin * ewin);
void                EwinBorderEventsConfigure(EWin * ewin, int mode);
void                EwinSetBorder(EWin * ewin, const Border * b, int apply);
void                EwinSetBorderByName(EWin * ewin, const char *name);
void                BorderWinpartChange(EWin * ewin, int i, int force);
int                 BorderWinpartIndex(EWin * ewin, Window win);
Border             *BorderCreateFiller(int left, int right, int top,
				       int bottom);
void                BordersSetupFallback(void);

/* buttons.c */
int                 ButtonsConfigLoad(FILE * fs);
Button             *ButtonCreate(const char *name, int id, ImageClass * ic,
				 ActionClass * aclass, TextClass * tclass,
				 const char *label, char ontop, int flags,
				 int minw, int maxw, int minh, int maxh, int xo,
				 int yo, int xa, int xr, int ya, int yr,
				 int xsr, int xsa, int ysr, int ysa, char simg,
				 int desk, char sticky);
void                ButtonDestroy(Button * b);
void                ButtonShow(Button * b);
void                ButtonHide(Button * b);
void                ButtonToggle(Button * b);
void                ButtonDraw(Button * b);
void                ButtonDrawWithState(Button * b, int state);
void                ButtonMoveToDesktop(Button * b, int desk);
void                ButtonMoveToCoord(Button * b, int x, int y);
void                ButtonMoveRelative(Button * b, int dx, int dy);
void                ButtonIncRefcount(Button * b);
void                ButtonDecRefcount(Button * b);
void                ButtonSetSwallowed(Button * b);
int                 ButtonGetRefcount(const Button * b);
int                 ButtonGetDesk(const Button * b);
int                 ButtonGetInfo(const Button * b, RectBox * r, int desk);
ActionClass        *ButtonGetAClass(const Button * b);
Window              ButtonGetWin(const Button * b);
int                 ButtonGetWidth(const Button * b);
int                 ButtonGetHeight(const Button * b);
int                 ButtonIsFixed(const Button * b);
int                 ButtonIsInternal(const Button * b);
int                 ButtonDoShowDefault(const Button * b);
void                ButtonDoAction(Button * b, EWin * ewin, XEvent * ev);
int                 ButtonEmbedWindow(Button * ButtonToUse,
				      Window WindowToEmbed);

/* cmclass.c */
#if ENABLE_COLOR_MODIFIERS
void                CreateCurve(ModCurve * c);
void                FreeModCurve(ModCurve * c);
void                FreeCMClass(ColorModifierClass * cm);
ColorModifierClass *CreateCMClass(char *name, int rnum, unsigned char *rpx,
				  unsigned char *rpy, int gnum,
				  unsigned char *gpx, unsigned char *gpy,
				  int bnum, unsigned char *bpx,
				  unsigned char *bpy);
void                ModifyCMClass(char *name, int rnum, unsigned char *rpx,
				  unsigned char *rpy, int gnum,
				  unsigned char *gpx, unsigned char *gpy,
				  int bnum, unsigned char *bpx,
				  unsigned char *bpy);
int                 ColorModifierConfigLoad(FILE * fs);
#endif

/* comms.c */
void                CommsInit(void);
void                CommsSend(Client * c, const char *s);
void                CommsFlush(Client * c);
void                CommsSendToMasterWM(const char *s);
void                CommsBroadcast(const char *s);
void                CommsBroadcastToSlaveWMs(const char *s);

/* config.c */
int                 ConfigSkipIfExists(FILE * fs, const char *name, int type);
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

/* coords.c */
void                CoordsShow(EWin * ewin);
void                CoordsHide(void);

/* cursors.c */
#define ECSR_NONE           0
#define ECSR_ROOT           1
#define ECSR_GRAB           2
#define ECSR_PGRAB          3
#define ECSR_ACT_MOVE       4
#define ECSR_ACT_RESIZE     5
#define ECSR_COUNT          6

void                ECursorApply(ECursor * ec, Window win);
void                ECursorIncRefcount(ECursor * ec);
void                ECursorDecRefcount(ECursor * ec);
Cursor              ECsrGet(int which);
void                ECsrApply(int which, Window win);

/* desktops.c */
Desk               *DeskGet(int desk);
Window              DeskGetWin(int desk);
int                 DeskGetX(int desk);
int                 DeskGetY(int desk);
Background         *DeskGetBackground(int desk);
void                DeskGetArea(int desk, int *ax, int *ay);
void                DeskSetArea(int desk, int ax, int ay);
int                 DeskIsViewable(int desk);
void                DeskSetDirtyStack(int desk);
void                DeskGetCurrentArea(int *ax, int *ay);
Window              DeskGetCurrentRoot(void);
void                DeskSetCurrentArea(int ax, int ay);

void                DeskRefresh(int num);
void                DeskAssignBg(int desk, Background * bg);
void                DeskSetBg(int desk, Background * bg, int refresh);
int                 DesktopAt(int x, int y);
void                DeskGoto(int num);
void                DeskHide(int num);
void                DeskShow(int num);
void                StackDesktop(int num);
void                DeskGotoByEwin(EWin * ewin);
void                DeskDragStart(int desk);
void                DeskDragMotion(void);

int                 DesksGetNumber(void);
int                 DesksGetCurrent(void);
void                DesksSetCurrent(int desk);
void                DesksClear(void);
void                DesksResize(int w, int h);
void                DesksEventsConfigure(int mode);

/* dialog.c */
typedef void        (DialogCallbackFunc) (Dialog * d, int val, void *data);
typedef void        (DialogItemCallbackFunc) (int val, void *data);

Dialog             *DialogCreate(const char *name);
void                DialogBindKey(Dialog * d, const char *key,
				  DialogCallbackFunc * func, int val);
void                DialogSetText(Dialog * d, const char *text);
void                DialogSetTitle(Dialog * d, const char *title);
void                DialogSetExitFunction(Dialog * d, DialogCallbackFunc * func,
					  int val);
void                DialogSetData(Dialog * d, void *data);
void               *DialogGetData(Dialog * d);

void                DialogRedraw(Dialog * d);
void                ShowDialog(Dialog * d);
void                DialogClose(Dialog * d);

void                DialogAddButton(Dialog * d, const char *text,
				    DialogCallbackFunc * func, char doclose,
				    int image);
DItem              *DialogInitItem(Dialog * d);
DItem              *DialogAddItem(DItem * dii, int type);
DItem              *DialogItem(Dialog * d);
void                DialogItemSetCallback(DItem * di, DialogCallbackFunc * func,
					  int val, void *data);
void                DialogItemSetClass(DItem * di, ImageClass * ic,
				       TextClass * tclass);
void                DialogItemSetPadding(DItem * di, int left, int right,
					 int top, int bottom);
void                DialogItemSetFill(DItem * di, char fill_h, char fill_v);
void                DialogItemSetAlign(DItem * di, int align_h, int align_v);
void                DialogItemSetText(DItem * di, const char *text);
void                DialogItemCallCallback(Dialog * d, DItem * di);
void                DialogDrawItems(Dialog * d, DItem * di, int x, int y, int w,
				    int h);
void                DialogItemRadioButtonSetEventFunc(DItem * di,
						      DialogItemCallbackFunc *
						      func);
void                DialogItemCheckButtonSetState(DItem * di, char onoff);
void                DialogItemCheckButtonSetPtr(DItem * di, char *onoff_ptr);
void                DialogItemTableSetOptions(DItem * di, int num_columns,
					      char border, char homogenous_h,
					      char homogenous_v);
void                DialogItemSeparatorSetOrientation(DItem * di,
						      char horizontal);
void                DialogItemImageSetFile(DItem * di, const char *image);
void                DialogFreeItem(DItem * di);
void                DialogItemSetRowSpan(DItem * di, int row_span);
void                DialogItemSetColSpan(DItem * di, int col_span);
void                DialogItemRadioButtonSetFirst(DItem * di, DItem * first);
void                DialogItemRadioButtonGroupSetValPtr(DItem * di,
							int *val_ptr);
void                DialogItemRadioButtonGroupSetVal(DItem * di, int val);

void                DialogItemSliderSetVal(DItem * di, int val);
void                DialogItemSliderSetBounds(DItem * di, int lower, int upper);
void                DialogItemSliderSetUnits(DItem * di, int units);
void                DialogItemSliderSetJump(DItem * di, int jump);
void                DialogItemSliderSetMinLength(DItem * di, int min);
void                DialogItemSliderSetValPtr(DItem * di, int *val_ptr);
void                DialogItemSliderSetOrientation(DItem * di, char horizontal);
int                 DialogItemSliderGetVal(DItem * di);
void                DialogItemSliderGetBounds(DItem * di, int *lower,
					      int *upper);

void                DialogItemAreaSetSize(DItem * di, int w, int h);
void                DialogItemAreaGetSize(DItem * di, int *w, int *h);
Window              DialogItemAreaGetWindow(DItem * di);
void                DialogItemAreaSetEventFunc(DItem * di,
					       DialogItemCallbackFunc * func);

void                DialogCallbackClose(Dialog * d, int val, void *data);

void                DialogsCheckUpdate(void);

void                DialogOK(const char *title, const char *fmt, ...);
void                DialogOKstr(const char *title, const char *txt);
void                DialogAlert(const char *fmt, ...);
void                DialogAlertOK(const char *fmt, ...);

EWin               *FindEwinByDialog(Dialog * d);
int                 FindADialog(void);

/* dock.c */
void                DockIt(EWin * ewin);
void                DockDestroy(EWin * ewin);

/* draw.c */
Imlib_Image        *ELoadImage(const char *file);
void                DrawEwinShape(EWin * ewin, int md, int x, int y, int w,
				  int h, char firstlast);

/* econfig.c */
void                ConfigurationLoad(void);
void                ConfigurationSave(void);
void                ConfigurationSet(const char *params);
void                ConfigurationShow(const char *params);

/* edge.c */
void                EdgeCheckMotion(int x, int y);
void                EdgeWindowsShow(void);
void                EdgeWindowsHide(void);

/* events.c */
/* Re-mapped X-events */
#define EX_EVENT_SHAPE_NOTIFY            64
#define EX_EVENT_SCREEN_CHANGE_NOTIFY    65
#define EX_EVENT_DAMAGE_NOTIFY           66

#define ENABLE_DEBUG_EVENTS 1
#if ENABLE_DEBUG_EVENTS
#define EDBUG_TYPE_EWINS        128
#define EDBUG_TYPE_FOCUS        129
#define EDBUG_TYPE_COMPRESSION  130
#define EDBUG_TYPE_STACKING     131
#define EDBUG_TYPE_RAISELOWER   132
#define EDBUG_TYPE_MOVERESIZE   133
#define EDBUG_TYPE_SESSION      134
#define EDBUG_TYPE_SNAPS        135
#define EDBUG_TYPE_DESKS        136
#define EDBUG_TYPE_GRABS        137
#define EDBUG_TYPE_DISPATCH     138
#define EDBUG_TYPE_MODULES      139
#define EDBUG_TYPE_CONFIG       140
#define EDBUG_TYPE_IPC          141
#define EDBUG_TYPE_EVENTS       142
#define EDBUG_TYPE_ICONBOX      143
#define EDBUG_TYPE_VERBOSE      144

int                 EventDebug(unsigned int type);
void                EventDebugSet(unsigned int type, int value);
#else
#define             EventDebug(type) 0
#define             EventDebugSet(type, value)
#endif
void                EventsInit(void);
void                WaitEvent(void);
void                EventDebugInit(const char *s);
void                EventShow(const XEvent * ev);

#if ENABLE_EWMH
/* ewmh.c */
void                EWMH_Init(Window win_wm_check);
void                EWMH_SetDesktopCount(void);
void                EWMH_SetDesktopRoots(void);
void                EWMH_SetDesktopNames(void);
void                EWMH_SetDesktopSize(void);
void                EWMH_SetCurrentDesktop(void);
void                EWMH_SetDesktopViewport(void);
void                EWMH_SetWorkArea(void);
void                EWMH_SetClientList(void);
void                EWMH_SetClientStacking(void);
void                EWMH_SetActiveWindow(Window win);
void                EWMH_SetShowingDesktop(int on);
void                EWMH_SetWindowName(Window win, const char *name);
void                EWMH_SetWindowDesktop(const EWin * ewin);
void                EWMH_SetWindowState(const EWin * ewin);
void                EWMH_SetWindowBorder(const EWin * ewin);
void                EWMH_SetWindowOpacity(const EWin * ewin);
void                EWMH_SetWindowActions(const EWin * ewin);
void                EWMH_GetWindowHints(EWin * ewin);
void                EWMH_DelWindowHints(const EWin * ewin);
void                EWMH_ProcessClientMessage(XClientMessageEvent * event);
void                EWMH_ProcessPropertyChange(EWin * ewin, Atom atom_change);
#endif

/* extinitwin.c */
Window              ExtInitWinCreate(void);
void                ExtInitWinSet(Window win);
Window              ExtInitWinGet(void);
void                ExtInitWinKill(void);

/* file.c */
void                Etmp(char *s);
void                E_md(const char *s);
int                 exists(const char *s);
void                mkdirs(const char *s);
int                 isfile(const char *s);
int                 isdir(const char *s);
int                 isabspath(const char *s);
char              **E_ls(const char *dir, int *num);
void                E_rm(const char *s);
void                E_mv(const char *s, const char *ss);
void                E_cp(const char *s, const char *ss);
time_t              moddate(const char *s);
int                 filesize(const char *s);
int                 fileinode(const char *s);
int                 filedev_map(int dev);
int                 filedev(const char *s);
void                E_cd(const char *s);
char               *cwd(void);
int                 permissions(const char *s);
char               *username(int uid);
char               *homedir(int uid);
char               *usershell(int uid);
const char         *atword(const char *s, int num);
const char         *atchar(const char *s, char c);
void                word(const char *s, int num, char *wd);
int                 canread(const char *s);
int                 canwrite(const char *s);
int                 canexec(const char *s);
char               *fileof(const char *s);
char               *fullfileof(const char *s);
char               *pathtoexec(const char *file);
char               *pathtofile(const char *file);
const char         *FileExtension(const char *file);
char               *field(char *s, int fieldno);
void                fword(char *s, int num, char *wd);

/* finders.c */
EWin               *EwinFindByPtr(const EWin * ewin);
EWin               *EwinFindByFrame(Window win);
EWin               *EwinFindByClient(Window win);
EWin               *EwinFindByChildren(Window win);
EWin               *EwinFindByString(const char *win, int type);

Button             *FindButton(Window win);
ActionClass        *FindActionClass(Window win);
Group             **ListWinGroups(EWin * ewin, char group_select, int *num);
EWin              **ListWinGroupMembersForEwin(EWin * ewin, int action,
					       char nogroup, int *num);
EWin              **EwinListTransients(EWin * ewin, int *num, int group);
EWin              **EwinListTransientFor(EWin * ewin, int *num);

/* focus.c */
#define FOCUS_NOP         0
#define FOCUS_INIT        1
#define FOCUS_SET         2
#define FOCUS_NONE        3
#define FOCUS_ENTER       4
#define FOCUS_LEAVE       5
#define FOCUS_EWIN_NEW    6
#define FOCUS_EWIN_GONE   7
#define FOCUS_DESK_ENTER  8
#define FOCUS_DESK_LEAVE  9
#define FOCUS_NEXT       10
#define FOCUS_PREV       11
#define FOCUS_CLICK      12

void                FocusEnable(int on);
void                FocusGetNextEwin(void);
void                FocusGetPrevEwin(void);
void                FocusEwinSetGrabs(EWin * ewin);
void                FocusFix(void);
void                FocusToEWin(EWin * ewin, int why);
void                FocusHandleEnter(EWin * ewin, XEvent * ev);
void                FocusHandleLeave(EWin * ewin, XEvent * ev);
void                FocusHandleClick(EWin * ewin, Window win);
void                FocusNewDeskBegin(void);
void                FocusNewDesk(void);

/* fonts.c */
int                 FontConfigLoad(FILE * fs);
void                FontConfigUnload(void);
const char         *FontLookup(const char *name);

#if ENABLE_GNOME
/* gnome.c */
void                GNOME_SetCurrentDesk(void);
void                GNOME_SetDeskCount(void);
void                GNOME_SetDeskNames(void);
void                GNOME_SetClientList(void);
void                GNOME_SetEwinDesk(const EWin * ewin);
void                GNOME_SetEwinArea(const EWin * ewin);
void                GNOME_SetHint(const EWin * ewin);
void                GNOME_SetCurrentArea(void);
void                GNOME_SetAreaCount(void);
void                GNOME_GetHints(EWin * ewin, Atom atom_change);
void                GNOME_DelHints(const EWin * ewin);
void                GNOME_SetHints(Window win_wm_check);
void                GNOME_ProcessClientMessage(XClientMessageEvent * event);
#endif

/* grabs.c */
int                 GrabKeyboardSet(Window win);
int                 GrabKeyboardRelease(void);
int                 GrabPointerSet(Window win, unsigned int csr, int confine);
void                GrabPointerRelease(void);
void                GrabButtonSet(unsigned int button, unsigned int modifiers,
				  Window window, unsigned int event_mask,
				  unsigned int csr, int confine);
void                GrabButtonRelease(unsigned int button,
				      unsigned int modifiers, Window win);

/* groups.c */
void                BreakWindowGroup(EWin * ewin, Group * g);
void                BuildWindowGroup(EWin ** ewins, int num);
int                 EwinInGroup(EWin * ewin, Group * g);
Group              *EwinsInGroup(EWin * ewin1, EWin * ewin2);
void                AddEwinToGroup(EWin * ewin, Group * g);
void                RemoveEwinFromGroup(EWin * ewin, Group * g);
void                GroupsEwinRemove(EWin * ewin);
void                ShowHideWinGroups(EWin * ewin, Group * g, char onoff);
void                ChooseGroupDialog(EWin * ewin, char *message,
				      char group_select, int action);
void                SaveGroups(void);

/* handlers.c */
void                SignalsSetup(void);
void                SignalsRestore(void);
void                HandleXError(Display * d, XErrorEvent * ev);
void                HandleXIOError(Display * d);

/* hints.c */
void                HintsInit(void);
void                HintsSetDesktopConfig(void);
void                HintsSetViewportConfig(void);
void                HintsSetCurrentDesktop(void);
void                HintsSetDesktopViewport(void);
void                HintsSetClientList(void);
void                HintsSetClientStacking(void);
void                HintsSetActiveWindow(Window win);
void                HintsSetWindowName(Window win, const char *name);
void                HintsSetWindowClass(Window win, const char *name,
					const char *clss);
void                HintsSetWindowDesktop(const EWin * ewin);
void                HintsSetWindowArea(const EWin * ewin);
void                HintsSetWindowState(const EWin * ewin);
void                HintsSetWindowOpacity(const EWin * ewin);
void                HintsSetWindowBorder(const EWin * ewin);
void                HintsGetWindowHints(EWin * ewin);
void                HintsDelWindowHints(const EWin * ewin);
void                HintsProcessPropertyChange(EWin * ewin, Atom atom_change);
void                HintsProcessClientMessage(XClientMessageEvent * event);

Pixmap              HintsGetRootPixmap(Window win);
void                HintsSetRootHints(Window win);
void                HintsSetRootInfo(Window win, Pixmap pmap,
				     unsigned int color);

void                EHintsSetInfo(const EWin * ewin);
int                 EHintsGetInfo(EWin * ewin);
void                EHintsSetDeskInfo(void);
void                EHintsGetDeskInfo(void);
void                EHintsSetInfoOnAll(void);

/* icccm.c */
void                ICCCM_Init(void);
void                ICCCM_ProcessClientMessage(XClientMessageEvent * event);
void                ICCCM_GetTitle(EWin * ewin, Atom atom_change);
void                ICCCM_GetColormap(EWin * ewin);
void                ICCCM_Delete(const EWin * ewin);
void                ICCCM_Save(const EWin * ewin);
void                ICCCM_Iconify(const EWin * ewin);
void                ICCCM_DeIconify(const EWin * ewin);
void                ICCCM_SizeMatch(const EWin * ewin, int wi, int hi, int *pwo,
				    int *pho);
void                ICCCM_MatchSize(EWin * ewin);
void                ICCCM_Configure(const EWin * ewin);
void                ICCCM_AdoptStart(const EWin * ewin);
void                ICCCM_Adopt(const EWin * ewin);
void                ICCCM_Withdraw(const EWin * ewin);
void                ICCCM_Cmap(EWin * ewin);
void                ICCCM_Focus(const EWin * ewin);
void                ICCCM_GetGeoms(EWin * ewin, Atom atom_change);
void                ICCCM_GetInfo(EWin * ewin, Atom atom_change);
void                ICCCM_GetHints(EWin * ewin, Atom atom_change);
void                ICCCM_SetIconSizes(void);
void                ICCCM_ProcessPropertyChange(EWin * ewin, Atom atom_change);

/* iclass.c */
int                 ImageclassConfigLoad(FILE * fs);

#ifdef ENABLE_THEME_TRANSPARENCY
void                TransparencySet(int transparency);
int                 TransparencyEnabled(void);
int                 TransparencyUpdateNeeded(void);
int                 ImageclassIsTransparent(ImageClass * ic);
#else
#define TransparencyEnabled() 0
#define TransparencyUpdateNeeded() 0
#define ImageclassIsTransparent(ic) 0
#endif
ImageState         *ImageclassGetImageState(ImageClass * ic, int state,
					    int active, int sticky);
ImageClass         *ImageclassCreateSimple(const char *name, const char *image);
ImageClass         *ImageclassFind(const char *name, int fallback);
Imlib_Image        *ImageclassGetImage(ImageClass * ic, int active, int sticky,
				       int state);
Pixmap              ImageclassApplySimple(ImageClass * ic, Window win,
					  Drawable draw, int state, int x,
					  int y, int w, int h);
void                ImageclassApply(ImageClass * ic, Window win, int w, int h,
				    int active, int sticky, int state,
				    char expose, int image_type);
void                ImageclassApplyCopy(ImageClass * ic, Window win, int w,
					int h, int active, int sticky,
					int state, PmapMask * pmm,
					int make_mask, int image_type);
void                FreePmapMask(PmapMask * pmm);
void                ITApply(Window win, ImageClass * ic, ImageState * is, int w,
			    int h, int state, int active, int sticky,
			    char expose, int image_type, TextClass * tc,
			    TextState * ts, const char *text);

/* ipc.c */
void __PRINTF__     IpcPrintf(const char *fmt, ...);
int                 HandleIPC(const char *params, Client * c);
void                ButtonIPC(int val, void *data);
int                 EFunc(EWin * ewin, const char *params);

/* lang.c */
void                LangInit(void);
char               *EstrLoc2Int(const char *str, int len);
char               *EstrUtf82Int(const char *str, int len);
const char         *EstrInt2Enc(const char *str, int want_utf8);
void                EstrInt2EncFree(const char *str, int want_utf8);

/* lists.c */
void                ListsInit(int num);
void               *FindItem(const void *name, int id, int find_by, int type);
void                AddItem(const void *item, const char *name, int id,
			    int type);
void                AddItemEnd(const void *item, const char *name, int id,
			       int type);
void               *RemoveItem(const void *name, int id, int find_by, int type);
void               *RemoveItemByPtr(const void *item, int type);
void              **ListItemType(int *num, int type);
char              **ListItems(int *num, int type);
void              **ListItemTypeID(int *num, int type, int id);
void              **ListItemTypeName(int *num, int type, const char *name);
void                MoveItemToListTop(const void *item, int type);
void                MoveItemToListBottom(const void *item, int type);
void                ListChangeItemID(int type, void *ptr, int id);

/* main.c */
void                EExit(int exitcode);
const char         *EDirRoot(void);
const char         *EDirBin(void);
const char         *EDirUser(void);
const char         *EDirUserCache(void);
void                EDirMake(const char *base, const char *name);
const char         *EGetSavePrefix(void);
const char         *EGetSavePrefixCommon(void);

/* memory.c */
#define Ecalloc     calloc
#define Emalloc     malloc
#define Efree       free
#define Erealloc    realloc

#define _EFREE(p)    do { if (p) { Efree(p); p = NULL; } } while (0)
#define _EFDUP(p, s) do { if (p) Efree(p); p = Estrdup(s); } while (0)

#if USE_LIBC_STRDUP
#define Estrdup(s) ((s) ? strdup(s) : NULL)
#else
char               *Estrdup(const char *s);
#endif
#if USE_LIBC_STRNDUP
#define Estrndup(s,n) ((s) ? strndup(s,n) : NULL)
#else
char               *Estrndup(const char *s, int n);
#endif
#if USE_LIBC_SETENV
#define Esetenv setenv
#else
int                 Esetenv(const char *name, const char *value, int overwrite);
#endif
char               *Estrdupcat2(char *ss, const char *s1, const char *s2);

char              **StrlistDup(char **lst, int num);
void                StrlistFree(char **lst, int num);
char               *StrlistJoin(char **lst, int num);
char               *StrlistEncodeEscaped(char *buf, int len, char **lst,
					 int num);
char              **StrlistDecodeEscaped(const char *str, int *pnum);
char              **StrlistFromString(const char *str, int delim, int *num);

/* menus.c */
int                 MenuStyleConfigLoad(FILE * fs);

Menu               *MenuCreate(const char *name, const char *title,
			       Menu * parent, MenuStyle * ms);
void                MenuDestroy(Menu * m);
void                MenuHide(Menu * m);
void                MenuEmpty(Menu * m);
void                MenuRepack(Menu * m);
MenuItem           *MenuItemCreate(const char *text, ImageClass * ic,
				   const char *action_params, Menu * child);
void                MenuSetName(Menu * m, const char *name);
void                MenuSetStyle(Menu * m, MenuStyle * ms);
void                MenuSetTitle(Menu * m, const char *title);
void                MenuSetData(Menu * m, char *data);
void                MenuSetTimestamp(Menu * m, time_t t);
const char         *MenuGetName(const Menu * m);
const char         *MenuGetData(const Menu * m);
time_t              MenuGetTimestamp(const Menu * m);
int                 MenuIsNotEmpty(const Menu * m);
void                MenuAddItem(Menu * m, MenuItem * mi);
void                ShowInternalMenu(Menu ** pm, MenuStyle ** pms,
				     const char *style,
				     Menu * (mcf) (const char *name,
						   MenuStyle * ms));

int                 MenusActive(void);

/* menus-misc.c */
Menu               *MenuCreateFromDirectory(const char *name, Menu * parent,
					    MenuStyle * ms, const char *dir);
Menu               *MenuCreateFromFlatFile(const char *name, Menu * parent,
					   MenuStyle * ms, const char *file);
Menu               *MenuCreateFromGnome(const char *name, Menu * parent,
					MenuStyle * ms, const char *dir);
Menu               *MenuCreateFromAllEWins(const char *name, MenuStyle * ms);
Menu               *MenuCreateFromDesktopEWins(const char *name, MenuStyle * ms,
					       int desk);
Menu               *MenuCreateFromDesktops(const char *name, MenuStyle * ms);
Menu               *MenuCreateFromThemes(const char *name, MenuStyle * ms);
Menu               *MenuCreateFromBorders(const char *name, MenuStyle * ms);

/* misc.c */
void                Quicksort(void **a, int l, int r,
			      int (*CompareFunc) (void *d1, void *d2));
void                ETimedLoopInit(int k1, int k2, int speed);
int                 ETimedLoopNext(void);
void __PRINTF__     Eprintf(const char *fmt, ...);

/* moveresize.c */
int                 ActionMoveStart(EWin * ewin, int grab, char constrained,
				    int nogroup);
int                 ActionMoveEnd(EWin * ewin);
int                 ActionMoveSuspend(void);
int                 ActionMoveResume(void);
void                ActionMoveHandleMotion(void);
int                 ActionResizeStart(EWin * ewin, int grab, int hv);
int                 ActionResizeEnd(EWin * ewin);
void                ActionResizeHandleMotion(void);

/* mwm.c */
void                MWM_GetHints(EWin * ewin, Atom atom_change);
void                MWM_SetInfo(void);

/* progress.c */
typedef struct _progressbar Progressbar;

Progressbar        *ProgressbarCreate(char *name, int width, int height);
void                ProgressbarDestroy(Progressbar * p);
void                ProgressbarSet(Progressbar * p, int progress);
void                ProgressbarShow(Progressbar * p);
void                ProgressbarHide(Progressbar * p);

/* regex.c */
int                 matchregexp(const char *rx, const char *s);

/* screen.c */
void                ScreenInit(void);
int                 ScreenGetGeometry(int x, int y, int *px, int *py,
				      int *pw, int *ph);
int                 ScreenGetAvailableArea(int x, int y, int *px, int *py,
					   int *pw, int *ph);
int                 GetPointerScreenGeometry(int *px, int *py,
					     int *pw, int *ph);
int                 GetPointerScreenAvailableArea(int *px, int *py,
						  int *pw, int *ph);

/* session.c */
#define EEXIT_EXIT      0
#define EEXIT_ERROR     1
#define EEXIT_LOGOUT    2
#define EEXIT_RESTART   3
#define EEXIT_THEME     4
#define EEXIT_EXEC      5

void                SessionInit(void);
void                SessionSave(int shutdown);
void                SessionExit(int mode, const char *params);
void                ProcessICEMSGS(void);
int                 GetSMfd(void);
void                SessionGetInfo(EWin * ewin, Atom atom_change);
void                SetSMID(const char *smid);
void                MatchEwinToSM(EWin * ewin);
void                autosave(void);

/* settings.c */
void                SettingsMoveResize(void);
void                SettingsDesktops(void);
void                SettingsArea(void);
void                SettingsPlacement(void);
void                SettingsAutoRaise(void);
void                SettingsSpecialFX(void);
void                SettingsMiscellaneous(void);
void                SettingsComposite(void);

/* setup.c */
void                MapUnmap(int start);
void                SetupX(const char *dstr);
void                RootResize(int root, int w, int h);

/* size.c */
void                MaxSize(EWin * ewin, const char *resize_type);
void                MaxWidth(EWin * ewin, const char *resize_type);
void                MaxHeight(EWin * ewin, const char *resize_type);

/* slideouts.c */
int                 SlideoutsConfigLoad(FILE * fs);

/* snaps.c */
void                Real_SaveSnapInfo(int dumval, void *dumdat);
void                LoadSnapInfo(void);
void                SaveSnapInfo(void);
void                SpawnSnappedCmds(void);
void                SnapshotEwinMatch(EWin * ewin);
void                SnapshotEwinUnmatch(EWin * ewin);
void                SnapshotEwinUpdate(EWin * ewin, unsigned int flags);
void                SnapshotEwinParse(EWin * ewin, const char *params);
void                SettingsRemember(void);
extern const char   SnapIpcText[];
void                SnapIpcFunc(const char *params, Client * c);

/* sound.c */
void                SoundPlay(const char *name);

/* startup.c */
void                StartupWindowsCreate(void);
void                StartupWindowsOpen(void);

/* tclass.c */
int                 TextclassConfigLoad(FILE * fs);
TextClass          *TextclassFind(const char *name, int fallback);
void                TextclassApply(ImageClass * ic, Window win, int w,
				   int h, int active, int sticky, int state,
				   char expose, TextClass * tclass,
				   const char *text);

/* text.c */
TextState          *TextclassGetTextState(TextClass * tclass, int state,
					  int active, int sticky);
void                TextstateDrawText(TextState * ts, Window win,
				      const char *text, int x, int y, int w,
				      int h, int fsize, int justification);
void                TextSize(TextClass * tclass, int active, int sticky,
			     int state, const char *text, int *width,
			     int *height, int fsize);
void                TextDraw(TextClass * tclass, Window win, int active,
			     int sticky, int state, const char *text, int x,
			     int y, int w, int h, int fsize, int justification);

/* theme.c */
void                ThemePathFind(void);
char              **ThemesList(int *num);

/* timers.c */
double              GetTime(void);
void                DoIn(const char *name, double in_time,
			 void (*func) (int val, void *data), int runtime_val,
			 void *runtime_data);
Qentry             *GetHeadTimerQueue(void);
void                HandleTimerEvent(void);
int                 RemoveTimerEvent(const char *name);

/* tooltips.c */
typedef struct _tooltip ToolTip;

int                 TooltipConfigLoad(FILE * fs);
void                TooltipShow(ToolTip * tt, const char *text,
				ActionClass * ac, int x, int y);
void                TooltipHide(ToolTip * tt);

void                TooltipsHandleEvent(void);
void                TooltipsHide(void);
void                TooltipsEnable(int enable);

/* ttfont.c */
void                Efont_extents(Efont * f, const char *text,
				  int *font_ascent_return,
				  int *font_descent_return, int *width_return,
				  int *max_ascent_return,
				  int *max_descent_return, int *lbearing_return,
				  int *rbearing_return);
Efont              *Efont_load(const char *file, int size);
void                Efont_free(Efont * f);
void                EFont_draw_string(Display * disp, Drawable win, GC gc,
				      int x, int y, const char *text, Efont * f,
				      Visual * vis, Colormap cm);

/* warp.c */
void                WarpFocus(int delta);

/* windowmatch.c */
typedef struct _windowmatch WindowMatch;

int                 WindowMatchConfigLoad(FILE * fs);
void               *WindowMatchEwin(EWin * ewin);
Border             *WindowMatchEwinBorder(const EWin * ewin);
const char         *WindowMatchEwinIcon(const EWin * ewin);
void                WindowMatchEwinOps(EWin * ewin);

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
