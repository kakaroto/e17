/*****************************************************************************/
/* Enlightenment - The Window Manager that dares to do what others don't     */
/*****************************************************************************/
/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
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

#include "config.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/Xlocale.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XShm.h>
#include <Imlib.h>
#if USE_FNLIB
#include <Fnlib.h>
#endif

#define DEBUG_EWMH  0

#define XSync(d, f) \
{XImage *__xim; \
__xim = XGetImage(d, root.win, 0, 0, 1, 1, 0xffffffff, ZPixmap); \
if (__xim) XDestroyImage(__xim);}

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
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <pwd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#ifdef HAVE_FREETYPE1_FREETYPE_FREETYPE_H
#include <freetype1/freetype/freetype.h>
#elif defined(HAVE_FREETYPE_FREETYPE_H)
#include <freetype/freetype.h>
#else
#include <freetype.h>
#endif

/* workaround for 64bit architectures - xlib expects 32bit CARDINALS to be */
/* long's on 64bit machines... thus well the CARD32's Im unsing shoudl be.. */
#define CARD32 long

#if (WITH_TARTY_WARP == 1)
#define WITH_TARTY_WARP 1
#else
#undef WITH_TARTY_WARP
#endif /* !WITH_TARTY_WARP */

#define RESET_ALERT \
   AssignTitleText(_("Enlightenment Message Dialog")); \
   AssignIgnoreText(_("Ignore this")); \
   AssignRestartText(_("Restart Enlightenment")); \
   AssignExitText(_("Quit Enlightenment"));

#define ASSIGN_ALERT(a, b, c, d) \
  AssignTitleText(a); \
  AssignIgnoreText(b); \
  AssignRestartText(c); \
  AssignExitText(d);

/************************************************************************/
/* sound macro convenience funcs                                        */
/************************************************************************/

#define AUDIO_PLAY(sclass) \
ApplySclass(FindItem((sclass), 0, LIST_FINDBY_NAME, LIST_TYPE_SCLASS));
/************************************************************************/
/* dialog macro convenience funcs                                       */
/************************************************************************/

#define DIALOG_OK(title, text) \
{ \
  Dialog *__d; \
  __d = CreateDialog("DIALOG"); \
  DialogSetTitle(__d, title); \
  DialogSetText(__d, text); \
  DialogAddButton(__d, _("OK"), NULL, 1); \
  ShowDialog(__d); \
}

#define DIALOG_PARAM_OK(title) \
{ \
  Dialog *__d; \
  __d = CreateDialog("DIALOG"); \
  DialogSetTitle(__d, title);

#define DIALOG_PARAM \
DialogSetParamText(__d,

#define DIALOG_PARAM_END \
  ); \
  DialogAddButton(__d, _("OK"), NULL, 1); \
  ShowDialog(__d); \
}

/************************************************************************/

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

#ifndef DEFAULT_SH_PATH
#ifdef __sgi
/*
 * It appears that SGI (at least IRIX 6.4) uses ksh as their sh, and it
 * seems to run in restricted mode, so things like restart fail miserably.
 * Let's use csh instead
 * -KDT 07/31/98
 */
#define DEFAULT_SH_PATH "/sbin/csh"
#else
#ifdef __EMX__
#define DEFAULT_SH_PATH "sh.exe"
#else
#define DEFAULT_SH_PATH "/bin/sh"
#endif
#endif
#endif

/* shut warnings up
 * pid_t               wait3(int *status, int options, struct rusage *rusage);
 * int                 setenv(const char *name, const char *value, int overwrite);
 */

#define FILEPATH_LEN_MAX 4096
/* This turns on E's internal stack tracking system for  coarse debugging */
/* and being able to trace E for profiling/optimisation purposes (which */
/* believe it or not I'm actually doing) */

/* #define DEBUG 1 */

#ifdef DEBUG
extern int          call_level;
extern int          debug_level;
extern char        *call_stack[1024];

#endif
#ifdef DEBUG
#define EDBUG(l,x) \
{ \
  call_stack[call_level] = x; \
  call_level++; \
}
#else
#define EDBUG(l,x)  \
;
#endif

#ifdef DEBUG
#define EDBUG_RETURN(x)  \
{ \
  call_level--; \
  return (x); \
}
#define EDBUG_RETURN_  \
{ \
  call_level--; \
  return; \
}
#else
#define EDBUG_RETURN(x)  \
{ \
  return (x); \
}
#define EDBUG_RETURN_  \
{ \
  return; \
}
#endif

int                 Evsnprintf(char *str, size_t count, const char *fmt,
			       va_list args);

#ifdef HAVE_STDARG_H
int                 Esnprintf(char *str, size_t count, const char *fmt, ...);

#else
int                 Esnprintf(va_alist);

#endif

#define Esetenv(var, val, overwrite) \
{ \
  static char envvar[FILEPATH_LEN_MAX]; \
  Esnprintf(envvar, FILEPATH_LEN_MAX, "%s=%s", var, val);\
  putenv(envvar);\
}

#ifdef HAVE_LIBESD
#include <esd.h>
#endif

#include "arrange.h"

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

#define TT_VALID( handle )  ( ( handle ).z != NULL )
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

#define EERR_NONE               0
#define EERR_OUT_OF_MEMORY      1
#define EERR_FILE_NOT_EXIST     2
#define EERR_PERMISSION_DENIED  3
#define EERR_FILING_SYSTEM_FULL 4

#ifndef ENLIGHTENMENT_ROOT
#define ENLIGHTENMENT_ROOT           "/usr/local/enlightenment"
#endif

#define ENLIGHTENMENT_CONF_NUM_DESKTOPS 32
/* the cast is so -1 will == UINT_MAX */
#define DESKTOPS_WRAP_NUM(x) \
 (((unsigned int) (x)) % mode.numdesktops)

#define LIST_FINDBY_NAME        0
#define LIST_FINDBY_ID          1
#define LIST_FINDBY_BOTH        2
#define LIST_FINDBY_NONE        3
#define LIST_FINDBY_POINTER     4

#define LIST_TYPE_ANY            0
#define LIST_TYPE_CLIENT         1
#define LIST_TYPE_EWIN           2
#define LIST_TYPE_BORDER         3
#define LIST_TYPE_ICLASS         4
#define LIST_TYPE_ACLASS         5
#define LIST_TYPE_AWAIT_ICLASS   6
#define LIST_TYPE_ACLASS_GLOBAL  7
#define LIST_TYPE_ACLASS_DESK    8
#define LIST_TYPE_TCLASS         9
#define LIST_TYPE_BACKGROUND    10
#define LIST_TYPE_BUTTON        11
#define LIST_TYPE_SCLASS        12
#define LIST_TYPE_WINDOWMATCH   13
#define LIST_TYPE_COLORMODIFIER 14
#define LIST_TYPE_ICONIFIEDS    15
#define LIST_TYPE_SLIDEOUT      16
#define LIST_TYPE_DRAW          17
#define LIST_TYPE_TOOLTIP       18
#define LIST_TYPE_CONTAINER     19
#define LIST_TYPE_QUEUE_ENTRY   20
#define LIST_TYPE_MENU          21
#define LIST_TYPE_MENU_STYLE    22
#define LIST_TYPE_ECURSOR       23
#define LIST_TYPE_SNAPSHOT      24
#define LIST_TYPE_DIALOG        25
#define LIST_TYPE_CLONE         26
#define LIST_TYPE_PAGER         27
#define LIST_TYPE_ICONBOX       28
#define LIST_TYPE_WARP_RING     29
#define LIST_TYPE_XID           30
#define LIST_TYPE_ICONDEF       31
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

#define FOCUS_POINTER           0
#define FOCUS_SLOPPY            1
#define FOCUS_CLICK             2

#define DOCK_LEFT               0
#define DOCK_RIGHT              1
#define DOCK_UP                 2
#define DOCK_DOWN               3

#define ICON_LEFT               0
#define ICON_RIGHT              1
#define ICON_UP                 2
#define ICON_DOWN               3

#define ACTION_NONE                   0
#define ACTION_EXEC                   1
#define ACTION_ALERT                  2
#define ACTION_MOVE	              3
#define ACTION_RESIZE                 4
#define ACTION_RAISE                  5
#define ACTION_LOWER                  6
#define ACTION_EXIT                   7
#define ACTION_CLEANUP                8
#define ACTION_SHOW_MENU              9
#define ACTION_HIDE_MENU              10
#define ACTION_RESIZE_H               11
#define ACTION_RESIZE_V               12
#define ACTION_KILL                   13
#define ACTION_KILL_NASTY             14
#define ACTION_DESKTOP_NEXT           15
#define ACTION_DESKTOP_PREV           16
#define ACTION_DESKTOP_RAISE          17
#define ACTION_DESKTOP_LOWER          18
#define ACTION_DESKTOP_DRAG           19
#define ACTION_STICK                  20
#define ACTION_DESKTOP_INPLACE        21
#define ACTION_DRAG_BUTTON            22
#define ACTION_FOCUSMODE_SET          23
#define ACTION_MOVEMODE_SET           24
#define ACTION_RESIZEMODE_SET         25
#define ACTION_SLIDEMODE_SET          26
#define ACTION_CLEANUPSILDE_SET       27
#define ACTION_MAPSLIDE_SET           28
#define ACTION_SOUND_SET              29
#define ACTION_BUTTONMOVE_RESIST_SET  30
#define ACTION_DESKTOPBG_TIMEOUT_SET  31
#define ACTION_MAPSLIDE_SPEED_SET     32
#define ACTION_CLEANUPSLIDE_SPEED_SET 33
#define ACTION_DRAGDIR_SET            34
#define ACTION_DRAGBAR_ORDER_SET      35
#define ACTION_DRAGBAR_WIDTH_SET      36
#define ACTION_DRAGBAR_LENGTH_SET     37
#define ACTION_DESKSLIDE_SET          38
#define ACTION_DESKSLIDE_SPEED_SET    39
#define ACTION_HIQUALITYBG_SET        40
#define ACTION_PLAYSOUNDCLASS         41
#define ACTION_GOTO_DESK              42
#define ACTION_DESKRAY                43
#define ACTION_AUTOSAVE_SET           44
#define ACTION_HIDESHOW_BUTTON        45
#define ACTION_ICONIFY                46
#define ACTION_SLIDEOUT               47
#define ACTION_SCROLL_WINDOWS         48
#define ACTION_SHADE                  49
#define ACTION_MAX_HEIGHT             50
#define ACTION_MAX_WIDTH              51
#define ACTION_MAX_SIZE               52
#define ACTION_SEND_TO_NEXT_DESK      53
#define ACTION_SEND_TO_PREV_DESK      54
#define ACTION_SNAPSHOT               55
#define ACTION_SCROLL_CONTAINER       56
#define ACTION_TOOLTIP_SET            57
#define ACTION_FOCUS_NEXT             58
#define ACTION_FOCUS_PREV             59
#define ACTION_FOCUS_SET              60
#define ACTION_BACKGROUND_SET         61
#define ACTION_AREA_SET               62
#define ACTION_MOVE_BY                63
#define ACTION_TOGGLE_FIXED           64
#define ACTION_SET_LAYER              65
#define ACTION_WARP_POINTER           66
#define ACTION_MOVE_WINDOW_TO_AREA    67
#define ACTION_MOVE_WINDOW_BY_AREA    68
#define ACTION_SET_WINDOW_BORDER      69
#define ACTION_LINEAR_AREA_SET        70
#define ACTION_LINEAR_MOVE_BY         71
#define ACTION_ABOUT                  72
#define ACTION_FX                     73
#define ACTION_MOVE_WINDOW_TO_LINEAR_AREA 74
#define ACTION_MOVE_WINDOW_BY_LINEAR_AREA 75
#define ACTION_SET_PAGER_HIQ          76
#define ACTION_SET_PAGER_SNAP         77
#define ACTION_CONFIG                 78
#define ACTION_MOVE_CONSTRAINED	      79
#define ACTION_INSERT_KEYS	      80
#define ACTION_START_GROUP            81
#define ACTION_ADD_TO_GROUP           82
#define ACTION_REMOVE_FROM_GROUP      83
#define ACTION_BREAK_GROUP            84
#define ACTION_SHOW_HIDE_GROUP        85
#define ACTION_CREATE_ICONBOX	      86
#define ACTION_RAISE_LOWER	      87
#define ACTION_ZOOM       	      88
#define ACTION_SET_WINDOW_BORDER_NG   89
#define ACTION_ICONIFY_NG             90
#define ACTION_KILL_NG                91
#define ACTION_MOVE_NG                92
#define ACTION_RAISE_NG               93
#define ACTION_LOWER_NG               94
#define ACTION_STICK_NG               95
#define ACTION_SHADE_NG               96
#define ACTION_RAISE_LOWER_NG         97
#define ACTION_SKIPFOCUS              98
#define ACTION_SKIPTASK               99
#define ACTION_SKIPWINLIST            100
#define ACTION_NEVERFOCUS             101
#define ACTION_SKIPLISTS              102
#define ACTION_SWAPMOVE               103
/* false number excluding the above list */
#define ACTION_NUMBEROF               104

#define MODE_NONE                 0
#define MODE_MOVE                 1
#define MODE_RESIZE               2
#define MODE_RESIZE_H             3
#define MODE_RESIZE_V             4
#define MODE_DESKDRAG             5
#define MODE_BUTTONDRAG           6
#define MODE_DESKRAY              7
#define MODE_PAGER_DRAG_PENDING   8
#define MODE_PAGER_DRAG           9

#define EVENT_MOUSE_DOWN  0
#define EVENT_MOUSE_UP    1
#define EVENT_MOUSE_ENTER 2
#define EVENT_MOUSE_LEAVE 3
#define EVENT_KEY_DOWN    4
#define EVENT_KEY_UP      5
#define EVENT_DOUBLE_DOWN 6

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

#define SET_OFF    0
#define SET_ON     1
#define SET_TOGGLE 2

/* ISO encodings */
#define ENCOING_ISO_8859_1 0
#define ENCOING_ISO_8859_2 1
#define ENCOING_ISO_8859_3 2
#define ENCOING_ISO_8859_4 3

typedef struct _menu Menu;
typedef struct _dialog Dialog;
typedef struct _pager Pager;
typedef struct _snapshot Snapshot;
typedef struct _iconbox Iconbox;
typedef struct _group Group;

typedef struct _icondef
{
   char               *title_match;
   char               *name_match;
   char               *class_match;
   char               *icon_file;
}
Icondef;

typedef struct _actiontype
{
   void               *params;
   struct _actiontype *Next;
   int                 Type;
}
ActionType;

typedef struct _list
{
   char               *name;
   int                 id;
   void               *item;

   struct _list       *next;
}
List;

typedef struct _client
{
   char               *name;
   Window              win;
   char               *msg;
   char               *clientname;
   char               *version;
   char               *author;
   char               *email;
   char               *web;
   char               *address;
   char               *info;
   Pixmap              pmap;
}
Client;

typedef struct _root
{
   Window              win;
   Visual             *vis;
   int                 depth;
   Colormap            cmap;
   int                 scr;
   int                 w, h;
   Window              focuswin;
}
Root;

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

typedef struct _imagestate
{
   char               *im_file;
   char               *real_file;
   char                unloadable;
   ImlibImage         *im;
   ImlibColor         *transp;
   ImlibBorder        *border;
   int                 pixmapfillstyle;
   ImlibColor          bg, hi, lo, hihi, lolo;
   int                 bevelstyle;
   ColorModifierClass *colmod;
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
   char                external;
   ImageStateArray     norm, active, sticky, sticky_active;
   ImlibBorder         padding;
   ColorModifierClass *colmod;
   unsigned int        ref_count;
}
ImageClass;

typedef struct _efont
{
   TT_Engine           engine;
   TT_Face             face;
   TT_Instance         instance;
   TT_Face_Properties  properties;
   int                 num_glyph;
   TT_Glyph           *glyphs;
   TT_Raster_Map     **glyphs_cached;
   TT_CharMap          char_map;
   int                 max_descent;
   int                 max_ascent;
}
Efont;

#if !USE_FNLIB
#define MODE_VERBATIM  0
#define MODE_WRAP_CHAR 1
#define MODE_WRAP_WORD 2

#define FONT_TO_RIGHT 0
#define FONT_TO_DOWN  1
#define FONT_TO_UP    2
#define FONT_TO_LEFT  3
#endif

typedef struct _textstate
{
   char               *fontname;
#if USE_FNLIB
   FnlibStyle          style;
   FnlibFont          *font;
#else
   struct
   {
      char                mode;
      char                orientation;
   } style;
#endif
   ImlibColor          fg_col;
   ImlibColor          bg_col;
   int                 effect;
   Efont              *efont;
   XFontStruct        *xfont;
   XFontSet            xfontset;
   int                 xfontset_ascent;
}
TextState;

typedef struct _textclass
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
}
TextClass;

typedef struct _action
{
   char                event;
   char                anymodifier;
   int                 modifiers;
   char                anybutton;
   int                 button;
   char                anykey;
   KeyCode             key;
   char               *key_str;
   char               *tooltipstring;
   ActionType         *action;
}
Action;

typedef struct _actionclass
{
   char               *name;
   int                 num;
   Action            **list;
   char               *tooltipstring;
   unsigned int        ref_count;
}
ActionClass;

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

typedef struct _ecursor
{
   char               *name;
   ImlibColor          fg, bg;
   char               *file;
   Cursor              cursor;
   unsigned int        ref_count;
   char                inroot;
}
ECursor;

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
   ImlibBorder         border;
   int                 num_winparts;
   WinPart            *part;
   char                changes_shape;
   char                shadedir;
   unsigned int        ref_count;
}
Border;

typedef struct _ewinbit
{
   Window              win;
   int                 x, y, w, h;
   int                 cx, cy, cw, ch;
   int                 state;
   char                expose;
   char                no_expose;
   char                left;
}
EWinBit;

typedef struct _winclient
{
   Window              win;
   char               *title;
   int                 x, y, w, h, bw;
   Colormap            cmap;
   Window              icon_win;
   Pixmap              icon_pmap;
   Pixmap              icon_mask;
   char                start_iconified;
   Window              group;
   Window              client_leader;
   char                need_input;
   char                transient;
   Window              transient_for;
   char               *class;
   char               *name;
   char               *role;
   char               *command;
   char               *machine;
   char               *icon_name;
   char                is_group_leader;
   char                no_resize_h;
   char                no_resize_v;
   char                shaped;
   Constraints         width, height;
   int                 base_w, base_h;
   int                 w_inc, h_inc;
   int                 grav;
   double              aspect_min, aspect_max;
   char                already_placed;
   char                mwm_decor_border;
   char                mwm_decor_resizeh;
   char                mwm_decor_title;
   char                mwm_decor_menu;
   char                mwm_decor_minimize;
   char                mwm_decor_maximize;
   char                mwm_func_resize;
   char                mwm_func_move;
   char                mwm_func_minimize;
   char                mwm_func_maximize;
   char                mwm_func_close;
   unsigned int        app_state;
}
WinClient;

#define EWIN_TYPE_NORMAL        0x00
#define EWIN_TYPE_DIALOG        0x01
#define EWIN_TYPE_MENU          0x02
#define EWIN_TYPE_ICONBOX       0x04
#define EWIN_TYPE_PAGER         0x08

typedef struct _ewin
{
   Window              win;
   int                 x, y, w, h, reqx, reqy;
   int                 lx, ly, lw, lh;
   char                type;
   char                internal;
   char                toggle;
   Window              win_container;
   WinClient           client;
   Border             *border;
   Border             *previous_border;
   char                border_new;
   EWinBit            *bits;
   int                 flags;
   int                 desktop;
   Group             **groups;
   int                 num_groups;
   int                 docked;
   char                sticky;
   char                visible;
   char                iconified;
   char                shaded;
   char                active;
   int                 layer;
   char                never_use_area;
   Window              parent;
   char                floating;
   char                shapedone;
   char                fixedpos;
#if 0				/* ENABLE_GNOME - Not actually used */
   int                 expanded_x;
   int                 expanded_y;
   int                 expanded_width;
   int                 expanded_height;
#endif
   char                ignorearrange;
   char                skiptask;
   char                skip_ext_pager;
   char                skipfocus;
   char                skipwinlist;
   char                focusclick;
   char                neverfocus;
   char                neverraise;
   int                 ewmh_flags;
   Menu               *menu;
   Window              shownmenu;
   Dialog             *dialog;
   Pager              *pager;
   Iconbox            *ibox;
   int                 area_x;
   int                 area_y;
   char               *session_id;
   int                 has_transients;
   int                 mini_w, mini_h;
   Pixmap              mini_pmap, mini_mask;
   Snapshot           *snap;
   int                 icon_pmap_w, icon_pmap_h;
   Pixmap              icon_pmap, icon_mask;
#if ENABLE_KDE
   char                kde_hint;
#endif
   int                 head;
}
EWin;

typedef struct _groupconfig
{
   char                iconify;
   char                kill;
   char                move;
   char                raise;
   char                set_border;
   char                stick;
   char                shade;
   char                mirror;
}
GroupConfig;

struct _group
{
   int                 index;
   EWin              **members;
   int                 num_members;
   GroupConfig         cfg;
};

typedef struct _awaiticlass
{
   Window              client_win;
   int                 ewin_bit;
   ImageClass         *iclass;
}
AwaitIclass;

typedef struct _bgeometry
{
   Constraints         width, height;
   int                 xorigin, yorigin;
   int                 xabs, xrel;
   int                 yabs, yrel;
   int                 xsizerel, xsizeabs;
   int                 ysizerel, ysizeabs;
   char                size_from_image;
}
BGeometry;

typedef struct _button
{
   char               *name;
   BGeometry           geom;
   ImageClass         *iclass;
   ActionClass        *aclass;
   TextClass          *tclass;
   char               *label;
   signed char         ontop;
   int                 flags;
   char                sticky;
   int                 desktop;
   char                visible;
   char                internal;
   char                default_show;
   char                used;

   Window              win;
   int                 x, y, w, h;
   int                 cx, cy, cw, ch;
   int                 state;
   char                expose;
   Window              inside_win;
   Window              event_win;
   char                destroy_inside_win;
   char                left;
   unsigned int        ref_count;
}
Button;

typedef struct _buttoncontainer
{
   char               *name;
   ImageClass         *iclass;

   Window              win;
   int                 w, h;
   int                 state;
   char                expose;
   char                orientation;
   Button            **ButtonList;
   int                 numofbuttonsinlist;
}
Container;

typedef struct _strip
{
   char               *name;
   int                 xabs, yabs;
   int                 xrel, yrel;
   signed char         ontop;
   char                sticky;
   int                 desktop;
   char                orientation;
   char                out;
   int                 justification;
   int                 spacing;

   char               *anchor_name;
   char               *bg_name;
   Button             *anchor;
   Button             *bg;

   int                 num_buttons;
   Button             *button;

   Window              win;
   int                 x, y, w, h;
   int                 cx, cy, cw, ch;
}
Strip;

typedef struct _background
{
   char               *name;
   Pixmap              pmap;
   time_t              last_viewed;
   struct _bg
   {
      ImlibColor          solid;
      char               *file;
      char               *real_file;
      ImlibImage         *im;
      char                tile;
      char                keep_aspect;
      int                 xjust, yjust;
      int                 xperc, yperc;
   }
   bg;
   struct _top
   {
      char               *file;
      char               *real_file;
      ImlibImage         *im;
      char                keep_aspect;
      int                 xjust, yjust;
      int                 xperc, yperc;
   }
   top;
   ColorModifierClass *cmclass;
   char                keepim;
   unsigned int        ref_count;
}
Background;

typedef struct _desk
{
   char                viewable;
   Window              win;
   int                 x, y;
   Background         *bg;
   int                 num;
   EWin              **list;
   Button             *tag;
   int                 current_area_x;
   int                 current_area_y;
}
Desk;

typedef struct _desktops
{
   int                 current;
   Desk                desk[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
   int                 dragdir;
   int                 dragbar_width;
   int                 dragbar_ordering;
   int                 dragbar_length;
   char                slidein;
   int                 slidespeed;
   char                hiqualitybg;
}
Desktops;

typedef struct _sample
{
   char               *file;
   int                 rate;
   int                 format;
   int                 samples;
   unsigned char      *data;
   int                 id;
}
Sample;

typedef struct _soundclass
{
   char               *name;
   char               *file;
   Sample             *sample;
   unsigned int        ref_count;
}
SoundClass;

typedef struct _windowmatch
{
   char               *name;
   char               *win_title;
   char               *win_name;
   char               *win_class;
   Constraints         width;
   Constraints         height;
   signed char         transient;
   signed char         no_resize_h;
   signed char         no_resize_v;
   signed char         shaped;
   Border             *border;
   ImageClass         *icon;
   int                 desk;
   char                make_sticky;
}
WindowMatch;

typedef struct
{
   char               *name;
   char                direction;
   int                 num_buttons;
   Button            **button;
   int                 w, h;
   Window              win;
   Window              from_win;
   unsigned int        ref_count;
}
Slideout;

typedef struct _emode
{
   int                 mode;
   int                 deskmode;
   char                place;
   char                flipp;
   char                startup;
   char                xselect;
   int                 next_move_x_plus;
   int                 next_move_y_plus;
   EWin               *ewin;
   Button             *button;
   int                 resize_detail;
   int                 win_x, win_y, win_w, win_h;
   int                 start_x, start_y;
   char                noewin;
   char                have_place_grab;
   int                 focusmode;
   char                dockdirmode;
   char                primaryicondir;
   char                click_focus_grabbed;
   EWin               *focuswin;
   EWin               *realfocuswin;
   EWin               *mouse_over_win;
   EWin               *context_ewin;
   int                 px, py, x, y;
   char                firstlast;
   int                 movemode;
   int                 swapmovemode;
   int                 swapcoord_x, swapcoord_y;
   int                 resizemode;
   int                 geominfomode;
   int                 slidemode;
   char                cleanupslide;
   char                mapslide;
   int                 slidespeedmap;
   int                 slidespeedcleanup;
   char                animate_shading;
   int                 shadespeed;
   char                doingslide;
   int                 server_grabbed;
   int                 desktop_bg_timeout;
   int                 deskdrag;
   char                sound;
   int                 button_move_resistance;
   char                button_move_pending;
   Colormap            current_cmap;
   char                autosave;
   char                memory_paranoia;
   char                destroy;
   char                adestroy;
   Slideout           *slideout;
   Window              context_win;
   char                tooltips;
   double              tiptime;
   char                autoraise;
   double              autoraisetime;
   char                dockapp_support;	/* wmdockapp only */
   int                 dockstartx;
   int                 dockstarty;
   char                save_under;
   char                cur_menu_mode;
   int                 cur_menu_depth;
   Menu               *cur_menu[256];
   char                menuslide;
   char                menusonscreen;
   char                warpmenus;
   char                warpsticky;
   char                warpshaded;
   char                warpiconified;
   char                warpfocused;
   char                numdesktops;
   char                transientsfollowleader;
   char                switchfortransientmap;
   char                snap;
   int                 edge_snap_dist;
   int                 screen_snap_dist;
   Window              menu_cover_win;
   Window              menu_win_covered;
   char                all_new_windows_get_focus;
   char                new_transients_get_focus;
   char                new_transients_get_focus_if_group_focused;
   char                manual_placement;
   char                manual_placement_mouse_pointer;
#ifdef HAS_XINERAMA
   char                extra_head;
#endif
   char                raise_on_next_focus;
   char                raise_after_next_focus;
   signed char         display_warp;
   char                warp_on_next_focus;
   char                warp_after_next_focus;
   int                 edge_flip_resistance;
   EWin               *moveresize_pending_ewin;
   char                borderpartpress;
   char                windowdestroy;
   int                 context_w;
   int                 context_h;
   char                autoupgrade;
   char                activenetwork;
   int                 motddate;
   char                motd;
   char                alreadytestingnetwork;
   char                show_pagers;
   Pager              *context_pager;
   char                pager_hiq;
   char                pager_snap;
   char                user_bg;
   char                pager_zoom;
   char                pager_title;
   char                constrained;
   int                 pager_scanspeed;
   TextClass          *icon_textclass;
   int                 icon_mode;
   char                nogroup;
   GroupConfig         group_config;
   char                group_swapmove;
#if ENABLE_KDE
   Window              kde_dock;
   int                 kde_support;
   int                 kde_x1;
   int                 kde_x2;
   int                 kde_y1;
   int                 kde_y2;
#endif
   char                clickalways;
   char                keybinds_changed;
   char                firsttime;
   char                showroottooltip;
   int                 pager_sel_button;
   int                 pager_win_button;
   int                 pager_menu_button;
   char                area_wraparound;
   char                dialog_headers;
}
EMode;

typedef struct _handlestruct
{
   void                (*func) (XEvent * ev);
}
HandleStruct;

typedef struct _tooltip
{
   char               *name;

   ImageClass         *iclass;
   ImageClass         *s_iclass[4];
   TextClass          *tclass;
   int                 dist;
   Window              win;
   Window              iwin;
   Window              s_win[4];
   char                visible;
   ImageClass         *tooltippic;
   unsigned int        ref_count;
}
ToolTip;

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

typedef struct _menustyle
{
   char               *name;
   TextClass          *tclass;
   ImageClass         *bg_iclass;
   ImageClass         *item_iclass;
   ImageClass         *sub_iclass;
   char                use_item_bg;
   char                iconpos;
   int                 maxx;
   int                 maxy;
   char               *border_name;
   unsigned int        ref_count;
}
MenuStyle;

typedef struct _menuitem
{
   ImageClass         *icon_iclass;
   char               *text;
   short               act_id;
   void               *params;
   Menu               *child;
   char                state;
   Pixmap              pmap[3];
   Pixmap              mask[3];
   Window              win;
   Window              icon_win;
   short               icon_w;
   short               icon_h;
   short               text_w;
   short               text_h;
   short               text_x;
   short               text_y;
}
MenuItem;

struct _menu
{
   char               *name;
   char               *title;
   MenuStyle          *style;
   int                 num;
   MenuItem          **items;
   Window              win;
   Pixmap              pmap;
   Pixmap              mask;
   char                stuck;
   Menu               *parent;
   MenuItem           *sel_item;
   time_t              last_change;
   void               *data;
   Menu               *ref_menu;
   unsigned int        ref_count;
};

typedef struct _progressbar
{
   char               *name;
   int                 value;
   int                 x;
   int                 y;
   int                 w;
   int                 h;
   Window              win;
   Window              n_win;
   Window              p_win;
   ImageClass         *ic, *inc, *ipc;
   TextClass          *tc, *tnc;
}
Progressbar;

struct _snapshot
{
   char               *name;
   char               *win_title;
   char               *win_name;
   char               *win_class;
   char               *border_name;
   char                use_desktop;
   int                 desktop;
   int                 area_x, area_y;
   char                use_wh;
   int                 w, h;
   char                use_xy;
   int                 x, y;
   char                use_layer;
   int                 layer;
   char                use_sticky;
   char                sticky;
   char               *iclass_name;
   char                use_shade;
   char                shade;
   char                use_cmd;
   char               *cmd;
   int                *groups;
   int                 num_groups;
   int                 used;
   char                use_skipwinlist;
   char                skipwinlist;
   char                use_skiplists;
   char                skiptask;
   char                skipfocus;
   char                use_neverfocus;
   char                neverfocus;
};

typedef struct _ditembutton DItemButton;
typedef struct _ditemcheckbutton DItemCheckButton;
typedef struct _ditemtext DItemText;
typedef struct _ditemimage DItemImage;
typedef struct _ditemseparator DItemSeparator;
typedef struct _ditemtable DItemTable;
typedef struct _ditemradiobutton DItemRadioButton;
typedef struct _ditemslider DItemSlider;
typedef struct _ditemarea DItemArea;
typedef struct _ditem DItem;

struct _ditemslider
{
   char                horizontal;

   char                numeric;
   char                numeric_side;

   int                 upper;
   int                 lower;
   int                 unit;
   int                 jump;
   int                 val;
   int                *val_ptr;

   int                 min_length;

   int                 base_orig_w, base_orig_h;
   int                 knob_orig_w, knob_orig_h;
   int                 border_orig_w, border_orig_h;

   int                 base_x, base_y, base_w, base_h;
   int                 knob_x, knob_y, knob_w, knob_h;
   int                 border_x, border_y, border_w, border_h;
   int                 numeric_x, numeric_y, numeric_w, numeric_h;

   ImageClass         *ic_base;
   ImageClass         *ic_knob;
   ImageClass         *ic_border;

   char                in_drag;
   int                 wanted_val;

   Window              base_win;
   Window              knob_win;
   Window              border_win;
};

struct _ditemarea
{
   Window              area_win;
   int                 w, h;
   void                (*event_func) (int val, void *data);
};

struct _ditembutton
{
   char               *text;
};

struct _ditemcheckbutton
{
   char               *text;
   Window              check_win;
   int                 check_orig_w, check_orig_h;
   char                onoff;
   char               *onoff_ptr;
};

struct _ditemtext
{
   char               *text;
};

struct _ditemimage
{
   char               *image;
};

struct _ditemseparator
{
   char                horizontal;
};

struct _ditemtable
{
   int                 num_columns;
   char                border;
   char                homogenous_h;
   char                homogenous_v;
   int                 num_items;
   DItem             **items;
};

struct _ditemradiobutton
{
   char               *text;
   Window              radio_win;
   int                 radio_orig_w, radio_orig_h;
   char                onoff;
   int                 val;
   int                *val_ptr;
   DItem              *next;
   DItem              *first;
   void                (*event_func) (int val, void *data);
};

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

struct _ditem
{
   int                 type;
   void                (*func) (int val, void *data);
   int                 val;
   void               *data;
   ImageClass         *iclass;
   TextClass          *tclass;
   ImlibBorder         padding;
   char                fill_h;
   char                fill_v;
   int                 align_h;
   int                 align_v;
   int                 row_span;
   int                 col_span;

   int                 x, y, w, h;
   char                hilited;
   char                clicked;
   Window              win;
   union
   {
      DItemButton         button;
      DItemCheckButton    check_button;
      DItemText           text;
      DItemTable          table;
      DItemImage          image;
      DItemSeparator      separator;
      DItemRadioButton    radio_button;
      DItemSlider         slider;
      DItemArea           area;
   }
   item;
};

typedef struct _dbutton
{
   char               *text;
   void                (*func) (int val, void *data);
   Window              win;
   int                 x, y, w, h;
   char                hilited;
   char                clicked;
   char                close;
   TextClass          *tclass;
   ImageClass         *iclass;
}
DButton;

typedef struct _Dkeybind
{
   KeyCode             key;
   int                 val;
   void               *data;
   void                (*func) (int val, void *data);
}
DKeyBind;

struct _dialog
{
   char               *name;
   char               *title;
   char               *text;
   int                 num_buttons;
   Window              win;
   DButton           **button;
   TextClass          *tclass;
   ImageClass         *iclass;
   int                 w, h;
   DItem              *item;
   void                (*exit_func) (int val, void *data);
   int                 exit_val;
   void               *exit_data;
   int                 num_bindings;
   DKeyBind           *keybindings;
};

typedef struct _PixImg
{
   XImage             *xim;
   XShmSegmentInfo    *shminfo;
   Pixmap              pmap;
   GC                  gc;
}
PixImg;

typedef struct _Clone
{
   char               *name;
   Window              win;
   EWin               *ewin;
}
Clone;

struct _iconbox
{
   /* user settings */
   char               *name;
   char                orientation;
   char                animate;
   char                scrollbar_side;
   char                arrow_side;
   char                shownames;
   char                nobg;
   int                 iconsize;
   int                 icon_mode;

   char                auto_resize;
   char                draw_icon_base;
   char                scrollbar_hide;
   char                cover_hide;
   int                 auto_resize_anchor;

   /* internally set stuff */
   int                 w, h;
   int                 pos;
   int                 max;
   char                arrow1_hilited;
   char                arrow1_clicked;
   char                arrow2_hilited;
   char                arrow2_clicked;
   char                icon_clicked;
   char                scrollbar_hilited;
   char                scrollbar_clicked;
   char                scrollbox_clicked;

   Pixmap              pmap;

   Window              win;
   Window              cover_win;
   Window              icon_win;
   Window              scroll_win;
   Window              arrow1_win;
   Window              arrow2_win;
   Window              scrollbar_win;
   Window              scrollbarknob_win;
   EWin               *ewin;

   int                 num_icons;
   EWin              **icons;

   /* these are theme-settable parameters */
   int                 scroll_thickness;
   int                 arrow_thickness;
   int                 bar_thickness;
   int                 knob_length;

};

struct _pager
{
   char               *name;
   Window              win;
   Pixmap              pmap;
   Pixmap              bgpmap;
   int                 desktop;
   int                 w, h;
   int                 dw, dh;
   char                visible;
   int                 update_phase;
   EWin               *ewin;
   char               *border_name;
   Window              sel_win;
   char                hi_visible;
   Window              hi_win;
   EWin               *hi_ewin;
};

typedef struct _drawqueue
{
   Window              win;
   ImageClass         *iclass;
   int                 w, h, active, sticky, state, expose;
   TextClass          *tclass;
   char               *text;
   char                shape_propagate;
   Pager              *pager;
   Pager              *redraw_pager;
   char                newbg;
   Dialog             *d;
   DItem              *di;
   int                 x, y;
}
DrawQueue;

#if ENABLE_KDE
/* some kde hint enums here */

typedef enum
{
   StickyFlag = (1 << 0),
   MaximizedFlag = (1 << 1),
   IconifiedFlag = (1 << 2),
   AllFlags = 7
}
KStates;

typedef enum
{
   AddWindow,
   RemoveWindow,
   FocusWindow,
   RaiseWindow,
   LowerWindow,
   ChangedClient,
   IconChange
}
KMessage;
#endif

/* only used for remember list dialog callback funcs (SettingsDialog()
 * in in settings.c)... snaps are attached to windows, not a global list */
typedef struct _remwinlist
{
   EWin               *ewin;
   char                remember;
}
RememberWinList;

/* function prototypes */

void                Efont_extents(Efont * f, char *text,
				  int *font_ascent_return,
				  int *font_descent_return, int *width_return,
				  int *max_ascent_return,
				  int *max_descent_return, int *lbearing_return,
				  int *rbearing_return);
Efont              *Efont_load(char *file, int size);
void                Efont_free(Efont * f);
void                EFont_draw_string(Display * disp, Drawable win, GC gc,
				      int x, int y, char *text, Efont * font,
				      Visual * vis, Colormap cm);

void                ErrAlert(int erno);

void                BlumFlimFrub(void);
void                SetEDir(char *d);
char               *UserEDir(void);
void                SetCacheDir(char *d);
char               *UserCacheDir(void);
void                EDisplayMemUse(void);
int                 EExit(void *code);
void                ShowEdgeWindows(void);
void                HideEdgeWindows(void);
int                 IsEdgeWin(Window win);
void                EdgeHandleEnter(XEvent * ev);
void                EdgeHandleLeave(XEvent * ev);
void                EdgeHandleMotion(XEvent * ev);

/* lists.c functions */
void               *FindItem(char *name, int id, int find_by, int type);
void                AddItem(void *item, char *name, int id, int type);
void                AddItemEnd(void *item, char *name, int id, int type);
void               *RemoveItem(char *name, int id, int find_by, int type);
void               *RemoveItemByPtr(void *ptritem, int type);
void              **ListItemType(int *num, int type);
char              **ListItems(int *num, int type);
void              **ListItemTypeID(int *num, int type, int id);
void              **ListItemTypeName(int *num, int type, char *name);
void                MoveItemToListTop(void *item, int type);
void                ListChangeItemID(int type, void *ptr, int id);
void                MoveItemToListBottom(void *item, int type);

void                SetupFallbackClasses(void);
void                SetupInit(void);

void                MapUnmap(int start);
void                SetupX(void);
void                SetupDirs(void);
void                SetupEnv(void);
Window              MakeExtInitWin(void);
void                SetupUserInitialization(void);

/* handlers.c functions */
void                SignalsSetup(void);
void                SignalsRestore(void);
void                EHandleXError(Display * d, XErrorEvent * ev);
void                HandleXIOError(Display * d);

void                CommsSetup(void);
void                CommsFindCommsWindow(void);
void                CommsSend(Client * c, char *s);
void                CommsSendToMasterWM(char *s);
void                CommsBroadcast(char *s);
void                CommsBroadcastToSlaveWMs(char *s);
Client             *MakeClient(Window win);
void                ListFreeClient(void *ptr);
void                DeleteClient(Client * c);
void                HandleComms(XClientMessageEvent * ev);
void                DisplayClientInfo(Client * c, int onoff);
void                HideClientInfo(void);

void                EMoveWindow(Display * d, Window win, int x, int y);
void                EResizeWindow(Display * d, Window win, int w, int h);
void                EMoveResizeWindow(Display * d, Window win, int x, int y,
				      int w, int h);
void                EDestroyWindow(Display * d, Window win);
void                EForgetWindow(Display * d, Window win);
void                EMapWindow(Display * d, Window win);
void                EUnmapWindow(Display * d, Window win);
void                EShapeCombineMask(Display * d, Window win, int dest, int x,
				      int y, Pixmap pmap, int op);
void                EShapeCombineRectangles(Display * d, Window win, int dest,
					    int x, int y, XRectangle * rect,
					    int n_rects, int op, int ordering);
void                EShapeCombineShape(Display * d, Window win, int dest, int x,
				       int y, Window src_win, int src_kind,
				       int op);
XRectangle         *EShapeGetRectangles(Display * d, Window win, int dest,
					int *rn, int *ord);
void                EReparentWindow(Display * d, Window win, Window parent,
				    int x, int y);
void                EMapRaised(Display * d, Window win);
int                 EGetGeometry(Display * d, Window win, Window * root_return,
				 int *x, int *y, unsigned int *w,
				 unsigned int *h, unsigned int *bw,
				 unsigned int *depth);
void                EConfigureWindow(Display * d, Window win, unsigned int mask,
				     XWindowChanges * wc);
void                ESetWindowBackgroundPixmap(Display * d, Window win,
					       Pixmap pmap);
void                ESetWindowBackground(Display * d, Window win, int col);
Pixmap              ECreatePixmap(Display * display, Drawable d,
				  unsigned int width, unsigned int height,
				  unsigned depth);
void                EFreePixmap(Display * display, Pixmap pixmap);
Window              ECreateWindow(Window parent, int x, int y, int w, int h,
				  int saveunder);
Window              ECreateEventWindow(Window parent, int x, int y, int w,
				       int h);
Window              ECreateFocusWindow(Window parent, int x, int y, int w,
				       int h);
void                GrabX(void);
void                UngrabX(void);
void                SetBG(Window win, Pixmap pmap, int color);
void                GetWinXY(Window win, int *x, int *y);
void                GetWinWH(Window win, unsigned int *w, unsigned int *h);
int                 GetWinDepth(Window win);
int                 WinExists(Window win);
Window              WindowAtXY_0(Window base, int bx, int by, int x, int y);
Window              WindowAtXY(int x, int y);
void                PointerAt(int *x, int *y);
void                PastePixmap(Display * d, Drawable w, Pixmap p, Mask m,
				int x, int y);
void                PasteMask(Display * d, Drawable w, Pixmap p, int x, int y,
			      int wd, int ht);

void                CheckEvent(void);
void                WaitEvent(void);

void                DetermineEwinFloat(EWin * ewin, int dx, int dy);
void                SetEInfoOnAll(void);
EWin               *GetEwinPointerInClient(void);
EWin               *GetEwin(void);
EWin               *GetFocusEwin(void);
void                SlideEwinTo(EWin * ewin, int fx, int fy, int tx, int ty,
				int speed);
void                SlideEwinsTo(EWin ** ewin, int *fx, int *fy, int *tx,
				 int *ty, int num_wins, int speed);
void                AddToFamily(Window win);
EWin               *AddInternalToFamily(Window win, char noshow, char *bname,
					int type, void *ptr);
void                SetEwinBorder(EWin * ewin);
void                SetEwinToBorder(EWin * ewin, Border * b);
void                HonorIclass(char *s, int id);
void                SyncBorderToEwin(EWin * ewin);
void                UpdateBorderInfo(EWin * ewin);
void                RealiseEwinWinpart(EWin * ewin, int i);
int                 DrawEwinWinpart(EWin * ewin, int i);
int                 ChangeEwinWinpart(EWin * ewin, int i);
void                DrawEwin(EWin * ewin);
int                 ChangeEwinWinpartContents(EWin * ewin, int i);
void                CalcEwinWinpart(EWin * ewin, int i);
void                CalcEwinSizes(EWin * ewin);
EWin               *Adopt(Window win);
EWin               *AdoptInternal(Window win, Border * border, int type,
				  void *ptr);
EWin               *CreateEwin(void);
void                FreeEwin(EWin * ewin);

/* windowmatch.c functions */
WindowMatch        *CreateWindowMatch(char *name);
char                TestWindowMatch(EWin * ewin, WindowMatch * b);
Border             *MatchEwinBorder(EWin * ewin, WindowMatch * b);
ImageClass         *MatchEwinIcon(EWin * ewin, WindowMatch * b);
int                 MatchEwinDesktop(EWin * ewin, WindowMatch * b);
void               *MatchEwinByFunction(EWin * ewin,
					void *(*FunctionToTest) (EWin *,
								 WindowMatch
								 *));
void                RemoveWindowMatch(WindowMatch * wm);

/* borders.c functions */
void                SetFrameProperty(EWin * ewin);
void                KillEwin(EWin * ewin);
void                ResizeEwin(EWin * ewin, int w, int h);
void                DetermineEwinArea(EWin * ewin);
void                MoveEwin(EWin * ewin, int x, int y);
void                MoveResizeEwin(EWin * ewin, int x, int y, int w, int h);
void                FloatEwin(EWin * ewin);
void                FloatEwinAt(EWin * ewin, int x, int y);
void                RestackEwin(EWin * ewin);
void                RaiseEwin(EWin * ewin);
void                LowerEwin(EWin * ewin);
void                ShowEwin(EWin * ewin);
void                HideEwin(EWin * ewin);
void                FreeBorder(Border * b);
Border             *CreateBorder(char *name);
void                AddBorderPart(Border * b, ImageClass * iclass,
				  ActionClass * aclass, TextClass * tclass,
				  ECursor * ec, char ontop, int flags,
				  char isregion, int wmin, int wmax, int hmin,
				  int hmax, int torigin, int txp, int txa,
				  int typ, int tya, int borigin, int bxp,
				  int bxa, int byp, int bya,
				  char keep_for_shade);
void                MinShadeSize(EWin * ewin, int *mw, int *mh);
void                InstantShadeEwin(EWin * ewin);
void                InstantUnShadeEwin(EWin * ewin);
void                ShadeEwin(EWin * ewin);
void                UnShadeEwin(EWin * ewin);

/* iclass.c functions */
ImageClass         *CreateIclass(void);
void                FreeImageState(ImageState * i);
void                FreeImageStateArray(ImageStateArray * isa);
void                FreeImageClass(ImageClass * i);
ImageState         *CreateImageState(void);
void                ImageStatePopulate(ImageState * is);
void                ImageStateRealize(ImageState * is);
void                IclassPopulate(ImageClass * iclass);
void                IclassApply(ImageClass * iclass, Window win, int w, int h,
				int active, int sticky, int state, char expose);
void                IclassApplyCopy(ImageClass * iclass, Window win, int w,
				    int h, int active, int sticky, int state,
				    Pixmap * pret, Pixmap * mret);

/* draw.c functions */
void                HandleDrawQueue(void);
char                IsPropagateEwinOnQueue(EWin * ewin);
void                EFillPixmap(Window win, Pixmap pmap, int x, int y, int w,
				int h);
void                EPastePixmap(Window win, Pixmap pmap, int x, int y, int w,
				 int h);
void                EBlendRemoveShape(EWin * ewin, Pixmap pmap, int x, int y);
PixImg             *ECreatePixImg(Window win, int w, int h);
void                EDestroyPixImg(PixImg * pi);
void                EBlendPixImg(EWin * ewin, PixImg * s1, PixImg * s2,
				 PixImg * dst, int x, int y, int w, int h);

ImlibImage         *ELoadImage(char *file);
ImlibImage         *ELoadImageImlibData(ImlibData * imd, char *file);
void                DrawEwinShape(EWin * ewin, int md, int x, int y, int w,
				  int h, char firstlast);
void                PropagateShapes(Window win);

void                FlipFocusList(void);
void                RemoveEwinFromFocusList(EWin * ewin);
void                AddEwinToFocusList(EWin * ewin);
void                GetNextFocusEwin(void);
void                GetPrevFocusEwin(void);
void                FixFocus(void);
void                FocusToEWin(EWin * ewin);

#if 0				/* Clean up if OK -- Remove FocusToNone */
void                FocusToNone(void);
#endif
void                BeginNewDeskFocus(void);
void                NewDeskFocus(void);

/* icccm.c functions */
void                ICCCM_Init(void);
void                ICCCM_ProcessClientMessage(XClientMessageEvent * event);
void                ICCCM_GetTitle(EWin * ewin, Atom atom_change);
void                ICCCM_GetColormap(EWin * ewin);
void                ICCCM_Delete(EWin * ewin);
void                ICCCM_Save(EWin * ewin);
void                ICCCM_Iconify(EWin * ewin);
void                ICCCM_DeIconify(EWin * ewin);
void                ICCCM_MatchSize(EWin * ewin);
void                ICCCM_Configure(EWin * ewin);
void                ICCCM_AdoptStart(EWin * ewin);
void                ICCCM_Adopt(EWin * ewin);
void                ICCCM_Withdraw(EWin * ewin);
void                ICCCM_Cmap(EWin * ewin);
void                ICCCM_Focus(EWin * ewin);
void                ICCCM_GetGeoms(EWin * ewin, Atom atom_change);
void                ICCCM_GetInfo(EWin * ewin, Atom atom_change);
void                ICCCM_GetHints(EWin * ewin, Atom atom_change);
void                ICCCM_GetShapeInfo(EWin * ewin);
void                ICCCM_SetIconSizes(void);
void                ICCCM_SetEInfo(EWin * ewin);
int                 ICCCM_GetEInfo(EWin * ewin);
void                ICCCM_SetMainEInfo(void);
void                ICCCM_GetMainEInfo(void);

/* actions.c functions */
void                RefreshScreen(void);
int                 runApp(char *exe, char *params);
void                GrabButtonGrabs(EWin * ewin);
void                UnGrabButtonGrabs(EWin * ewin);
ActionClass        *CreateAclass(char *name);
Action             *CreateAction(char event, char anymod, int mod, int anybut,
				 int but, char anykey, char *key,
				 char *tooltipstring);
void                RemoveActionType(ActionType * ActionTypeToRemove);
void                RemoveAction(Action * ActionToRemove);
void                RemoveActionClass(ActionClass * ActionToRemove);
void                AddToAction(Action * act, int id, void *params);
void                AddAction(ActionClass * a, Action * act);
int                 EventAclass(XEvent * ev, ActionClass * a);
int                 handleAction(ActionType * Action);
int                 spawnMenu(void *params);
int                 hideMenu(void *params);
int                 doNothing(void *params);
int                 execApplication(void *params);
int                 alert(void *params);
int                 doExit(void *params);
int                 doMove(void *params);
int                 doSwapMove(void *params);
int                 doMoveNoGroup(void *params);
int                 doMoveConstrained(void *params);
int                 doMoveConstrainedNoGroup(void *params);
int                 doResize(void *params);
int                 doResizeH(void *params);
int                 doResizeV(void *params);
int                 doResizeEnd(void *params);
int                 doMoveEnd(void *params);
int                 doRaise(void *params);
int                 doRaiseNoGroup(void *params);
int                 doLower(void *params);
int                 doLowerNoGroup(void *params);
int                 doCleanup(void *params);
int                 doKill(void *params);
int                 doKillNoGroup(void *params);
int                 doKillNasty(void *params);
int                 doNextDesktop(void *params);
int                 doPrevDesktop(void *params);
int                 doRaiseDesktop(void *params);
int                 doLowerDesktop(void *params);
int                 doDragDesktop(void *params);
int                 doStick(void *params);
int                 doSkipTask(void *params);
int                 doSkipWinList(void *params);
int                 doSkipFocus(void *params);
int                 doNeverFocus(void *params);
int                 doSkipLists(void *params);
int                 doStickNoGroup(void *params);
int                 doInplaceDesktop(void *params);
int                 doDragButtonStart(void *params);
int                 doDragButtonEnd(void *params);
int                 doFocusModeSet(void *params);
int                 doMoveModeSet(void *params);
int                 doResizeModeSet(void *params);
int                 doSlideModeSet(void *params);
int                 doCleanupSlideSet(void *params);
int                 doMapSlideSet(void *params);
int                 doSoundSet(void *params);
int                 doButtonMoveResistSet(void *params);
int                 doDesktopBgTimeoutSet(void *params);
int                 doMapSlideSpeedSet(void *params);
int                 doCleanupSlideSpeedSet(void *params);
int                 doDragdirSet(void *params);
int                 doDragbarOrderSet(void *params);
int                 doDragbarWidthSet(void *params);
int                 doDragbarLengthSet(void *params);
int                 doDeskSlideSet(void *params);
int                 doDeskSlideSpeedSet(void *params);
int                 doHiQualityBgSet(void *params);
int                 doPlaySoundClass(void *params);
int                 doGotoDesktop(void *params);
int                 doDeskray(void *params);
int                 doAutosaveSet(void *params);
int                 doHideShowButton(void *params);
int                 doIconifyWindow(void *params);
int                 doIconifyWindowNoGroup(void *params);
int                 doSlideout(void *params);
int                 doScrollWindows(void *params);
int                 doShade(void *params);
int                 doShadeNoGroup(void *params);
int                 doMaxH(void *params);
int                 doMaxW(void *params);
int                 doMax(void *params);
int                 doSendToNextDesk(void *params);
int                 doSendToPrevDesk(void *params);
int                 doSnapshot(void *params);
int                 doScrollContainer(void *params);
int                 doToolTipSet(void *params);
int                 doFocusNext(void *params);
int                 doFocusPrev(void *params);
int                 doFocusSet(void *params);
int                 doBackgroundSet(void *params);
int                 doAreaSet(void *params);
int                 doAreaMoveBy(void *params);
int                 doToggleFixedPos(void *params);
int                 doSetLayer(void *params);
int                 doWarpPointer(void *params);
int                 doMoveWinToArea(void *params);
int                 doMoveWinByArea(void *params);
int                 doSetWinBorder(void *params);
int                 doSetWinBorderNoGroup(void *params);
int                 doLinearAreaSet(void *params);
int                 doLinearAreaMoveBy(void *params);
int                 doAbout(void *params);
int                 doFX(void *params);
int                 doMoveWinToLinearArea(void *params);
int                 doMoveWinByLinearArea(void *params);
int                 doSetPagerHiq(void *params);
int                 doSetPagerSnap(void *params);
int                 doConfigure(void *params);
int                 doInsertKeys(void *params);
int                 doCreateIconbox(void *params);
int                 doRaiseLower(void *params);
int                 doRaiseLowerNoGroup(void *params);
int                 doStartGroup(void *params);
int                 doAddToGroup(void *params);
int                 doRemoveFromGroup(void *params);
int                 doBreakGroup(void *params);
int                 doShowHideGroup(void *params);
int                 doZoom(void *params);
int                 initFunctionArray(void);

void                GrabActionKey(Action * a);
void                UnGrabActionKey(Action * a);
void                GrabTheButtons(Window win);
int                 GrabConfineThePointer(Window win);
int                 GrabThePointer(Window win);
void                UnGrabTheButtons(void);

char               *GetUniqueBGString(Background * bg);
void                ChangeNumberOfDesktops(int quantity);
void                ShowDesktopControls(void);
void                ShowDesktopTabs(void);
void                HideDesktopTabs(void);
void                MoveToDeskTop(int num);
void                MoveToDeskBottom(int num);
void                SlideWindowTo(Window win, int fx, int fy, int tx, int ty,
				  int speed);
void                KeepBGimages(Background * bg, char onoff);
void                RemoveImagesFromBG(Background * bg);
void                FreeDesktopBG(Background * bg);
Background         *CreateDesktopBG(char *name, ImlibColor * solid, char *bg,
				    char tile, char keep_aspect, int xjust,
				    int yjust, int xperc, int yperc, char *top,
				    char tkeep_aspect, int txjust, int tyjust,
				    int txperc, int typerc);
void                RefreshCurrentDesktop(void);
void                RefreshDesktop(int num);
void                SetBackgroundTo(ImlibData * imd, Window win,
				    Background * dsk, char setbg);
void                InitDesktopBgs(void);
void                InitDesktopControls(void);
void                SetDesktopBg(int desk, Background * bg);
void                ConformEwinToDesktop(EWin * ewin);
int                 DesktopAt(int x, int y);
void                MoveStickyWindowsToCurrentDesk(void);
void                GotoDesktop(int num);
void                MoveDesktop(int num, int x, int y);
void                RaiseDesktop(int num);
void                LowerDesktop(int num);
void                HideDesktop(int num);
void                ShowDesktop(int num);
void                StackDesktops(void);
void                StackDesktop(int num);
void                UncoverDesktop(int num);
void                MoveEwinToDesktop(EWin * ewin, int num);
void                DesktopAddEwinToBottom(EWin * ewin);
void                DesktopAddEwinToTop(EWin * ewin);
void                DesktopRemoveEwin(EWin * ewin);
void                MoveEwinToDesktopAt(EWin * ewin, int num, int x, int y);
void                FloatEwinAboveDesktops(EWin * ewin);
void                DesktopAccounting(void);

int                 AddEToFile(char *file);
int                 CreateEFile(char *file);
void                AddE(void);
void                CreateStartupDisplay(char start);

/* tclass.c */

TextClass          *CreateTclass(void);
void                FreeTextState(TextState * ts);
void                DeleteTclass(TextClass * t);
TextState          *CreateTextState(void);
void                TclassPopulate(TextClass * tclass);
void                TclassApply(ImageClass * iclass, Window win, int w, int h,
				int active, int sticky, int state, char expose,
				TextClass * tclass, char *text);

void                HandleClientMessage(XEvent * ev);
void                HandleFocusWindowIn(Window win);
void                HandleFocusWindow(Window win);
void                HandleChildShapeChange(XEvent * ev);
void                HandleMotion(XEvent * ev);
void                HandleDestroy(XEvent * ev);
void                HandleProperty(XEvent * ev);
void                HandleCirculate(XEvent * ev);
void                HandleReparent(XEvent * ev);
void                HandleConfigureRequest(XEvent * ev);
void                HandleResizeRequest(XEvent * ev);
void                HandleUnmap(XEvent * ev);
void                HandleMapRequest(XEvent * ev);
void                HandleExpose(XEvent * ev);
void                HandleMouseDown(XEvent * ev);
void                HandleMouseUp(XEvent * ev);
void                HandleMouseIn(XEvent * ev);
void                HandleMouseOut(XEvent * ev);

EWin               *FindEwinByBase(Window win);
EWin               *FindEwinByChildren(Window win);
EWin               *FindEwinByDecoration(Window win);
Button             *FindButton(Window win);
ActionClass        *FindActionClass(Window win);
Menu               *FindMenuItem(Window win, MenuItem ** mi);
Menu               *FindMenu(Window win);
EWin               *FindEwinByMenu(Menu * m);
Group             **ListWinGroups(EWin * ewin, char group_select, int *num);
EWin              **ListWinGroupMembersForEwin(EWin * ewin, int action,
					       char nogroup, int *num);
EWin              **ListTransientsFor(Window win, int *num);
EWin              **ListGroupMembers(Window win, int *num);
EWin               *FindEwinByDialog(Dialog * d);
Dialog             *FindDialogButton(Window win, int *bnum);
Dialog             *FindDialog(Window win);
char                FindADialog(void);
EWin               *FindEwinSpawningMenu(Menu * m);
Pager              *FindPager(Window win);
DItem              *FindDialogItem(Window win, Dialog ** dret);

/* mwm.c functions */
void                MWM_GetHints(EWin * ewin, Atom atom_change);
void                MWM_SetInfo(void);

/* containers.c functions */
Container          *InitializeContainer(char *name, ImageClass * iclass,
					int width, int height,
					char orientation);
void                AddButtonToContainer(Container * bc, Button * b);
void                RemoveButtonFromContainer(Container * bc, Button * b);
void                DestroyContainer(Container * bc);
void                RemoveContainer(Container * bc);

/* buttons.c functions */
Button             *CreateButton(char *name, ImageClass * iclass,
				 ActionClass * aclass, TextClass * tclass,
				 char *label, char ontop, int flags, int minw,
				 int maxw, int minh, int maxh, int xo, int yo,
				 int xa, int xr, int ya, int yr, int xsr,
				 int xsa, int ysr, int ysa, char simg, int desk,
				 char sticky);
void                DestroyButton(Button * b);
void                CalcButton(Button * b);
void                ShowButton(Button * b);
void                MoveButtonToDesktop(Button * b, int num);
void                HideButton(Button * b);
void                ToggleButton(Button * b);
void                DrawButton(Button * b);
void                MovebuttonToCoord(Button * b, int x, int y);
int                 EmbedWindowIntoButton(Button * ButtonToUse,
					  Window WindowToEmbed);
void                FindEmptySpotForButton(Button * bt, char *listname,
					   char dirtomove);

/* atoms.c functions */
void               *AtomGet(Window win, Atom to_get, Atom type, int *size);
void                setSimpleHint(Window win, Atom atom, long value);
long               *getSimpleHint(Window win, Atom atom);
void                deleteHint(Window win, Atom atom);

#if ENABLE_GNOME
/* gnome.c functions */
void                GNOME_SetHints(Window win_wm_check);
void                GNOME_GetHintIcons(EWin * ewin, Atom atom_change);
void                GNOME_SetCurrentDesk(void);
void                GNOME_SetUsedHints(void);
void                GNOME_GetExpandedSize(EWin * ewin, Atom atom_change);
void                GNOME_GetHintDesktop(EWin * ewin, Atom atom_change);
void                GNOME_GetHint(EWin * ewin, Atom atom_change);
void                GNOME_GetHintAppState(EWin * ewin, Atom atom_change);
void                GNOME_GetHintState(EWin * ewin, Atom atom_change);
void                GNOME_GetHintLayer(EWin * ewin, Atom atom_change);
void                GNOME_SetEwinArea(EWin * ewin);
void                GNOME_SetDeskCount(void);
void                GNOME_SetDeskNames(void);
void                GNOME_SetClientList(void);
void                GNOME_GetHints(EWin * ewin, Atom atom_change);
void                GNOME_SetHint(EWin * ewin);
void                GNOME_SetEwinDesk(EWin * ewin);
void                GNOME_SetCurrentArea(void);
void                GNOME_SetAreaCount(void);
void                GNOME_DelHints(EWin * ewin);
void                GNOME_ProcessClientMessage(XClientMessageEvent * event);
#endif

#if ENABLE_KDE
/* kde.c functions */
void                KDE_Init(void);
void                KDE_Shutdown(void);
void                KDE_RemoveModule(Window win);
void                KDE_ClientChange(Window win, Atom a);
void                KDE_ProcessClientMessage(XClientMessageEvent * event);
void                KDE_HintChange(Atom a);
void                KDE_SetRootArea(void);
void                KDE_UpdateFocusedWindow(void);
void                KDE_SetNumDesktops(void);
void                KDE_NewWindow(EWin * ewin);
void                KDE_RemoveWindow(EWin * ewin);
void                KDE_UpdateClient(EWin * ewin);
#endif

#if ENABLE_EWMH
/* ewmh.c functions */
void                EWMH_Init(Window win_wm_check);
void                EWMH_SetDesktopCount(void);
void                EWMH_SetDesktopNames(void);
void                EWMH_SetDesktopSize(void);
void                EWMH_SetCurrentDesktop(void);
void                EWMH_SetDesktopViewport(void);
void                EWMH_SetWorkArea(void);
void                EWMH_SetClientList(void);
void                EWMH_SetActiveWindow(const EWin * ewin);
void                EWMH_SetWindowDesktop(const EWin * ewin);
void                EWMH_SetWindowState(const EWin * ewin);
void                EWMH_GetWindowDesktop(EWin * ewin);
void                EWMH_GetWindowState(EWin * ewin);
void                EWMH_GetWindowHints(EWin * ewin);
void                EWMH_DelWindowHints(const EWin * ewin);
void                EWMH_ProcessClientMessage(XClientMessageEvent * event);
void                EWMH_ProcessPropertyChange(EWin * ewin, Atom atom_change);
#endif

/* hints.c functions */
void                HintsInit(void);
void                HintsSetDesktopConfig(void);
void                HintsSetViewportConfig(void);
void                HintsSetCurrentDesktop(void);
void                HintsSetDesktopViewport(void);
void                HintsSetClientList(void);
void                HintsSetActiveWindow(EWin * ewin);
void                HintsSetWindowDesktop(EWin * ewin);
void                HintsSetWindowArea(EWin * ewin);
void                HintsSetWindowState(EWin * ewin);
void                HintsSetWindowHints(EWin * ewin);
void                HintsGetWindowHints(EWin * ewin);
void                HintsDelWindowHints(EWin * ewin);
void                HintsProcessPropertyChange(EWin * ewin, Atom atom_change);
void                HintsProcessClientMessage(XClientMessageEvent * event);

/* sound.c functions */
Sample             *LoadWav(char *file);
void                SoundPlay(Sample * s);
void                DestroySample(Sample * s);
void                DestroySclass(SoundClass * sclass);
SoundClass         *CreateSoundClass(char *name, char *file);
void                ApplySclass(SoundClass * sclass);
void                SoundInit(void);
void                SoundExit(void);

/* regex.c functions */
int                 isafter(int p, char *s1, char *s2);
int                 matchregexp(char *rx, char *s);

double              GetTime(void);
void                DoIn(char *name, double in_time,
			 void (*func) (int val, void *data), int runtime_val,
			 void *runtime_data);
Qentry             *GetHeadTimerQueue(void);
void                HandleTimerEvent(void);
void                RemoveTimerEvent(char *name);

/* cmclass.c functions */
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

/* config.c functions */
char               *FindFile(const char *file);
int                 LoadConfigFile(const char *file);
int                 LoadEConfig(char *themelocation);
void                SaveUserControlConfig(FILE * autosavefile);
void                RecoverUserConfig(void);

/* theme.c functions */
void                BadThemeDialog(void);
char               *FindTheme(char *theme);
char               *ExtractTheme(char *theme);

/* iconify.c functions */
void                IB_Animate(char iconify, EWin * from, EWin * to);
void                IconifyEwin(EWin * ewin);
void                DeIconifyEwin(EWin * ewin);
void                RemoveMiniIcon(EWin * ewin);
void                MakeIcon(EWin * ewin);
void                DockIt(EWin * ewin);
void                DockDestroy(EWin * ewin);
Iconbox            *CreateIconbox(char *name);
void                FreeIconbox(Iconbox * ib);
void                ShowIconbox(Iconbox * ib);
void                HideIconbox(Iconbox * ib);
void                AddEwinToIconbox(Iconbox * ib, EWin * ewin);
void                DelEwinFromIconbox(Iconbox * ib, EWin * ewin);
void                RedrawIconbox(Iconbox * ib);
void                IconboxHandleEvent(XEvent * ev);
void                UpdateAppIcon(EWin * ewin, int imode);
void                IconboxResize(Iconbox * ib, int w, int h);
void                IB_FixPos(Iconbox * ib);
void                IB_DrawScroll(Iconbox * ib);
EWin               *IB_FindIcon(Iconbox * ib, int px, int py);
void                IB_CalcMax(Iconbox * ib);
void                IB_Scroll(Iconbox * ib, int dir);
void                IB_ShowMenu(Iconbox * ib, int x, int y);
void                IB_CompleteRedraw(Iconbox * ib);
void                IB_SnapEWin(EWin * ewin);
void                IB_GetAppIcon(EWin * ewin);
void                IB_PasteDefaultBase(Drawable d, int x, int y, int w, int h);
void                IB_PasteDefaultBaseMask(Drawable d, int x, int y, int w,
					    int h);
void                IB_GetEIcon(EWin * ewin);
void                IB_AddIcondef(char *title, char *name, char *class,
				  char *file);
void                IB_RemoveIcondef(Icondef * idef);
Icondef            *IB_MatchIcondef(char *title, char *name, char *class);
Icondef           **IB_ListIcondef(int *num);
void                IB_Setup(void);
void                IB_LoadIcondefs(void);
void                IB_ReLoadIcondefs(void);
void                IB_SaveIcondefs(void);
Iconbox           **ListAllIconboxes(int *num);
Iconbox            *SelectIconboxForEwin(EWin * ewin);

/* slideouts.c functions */
void                SlideWindowSizeTo(Window win, int fx, int fy, int tx,
				      int ty, int fw, int fh, int tw, int th,
				      int speed);
Slideout           *CreateSlideout(char *name, char dir);
void                ShowSlideout(Slideout * s, Window win);
void                HideSlideout(Slideout * s, Window w);
void                CalcSlideoutSize(Slideout * s);
void                AddButtonToSlideout(Slideout * s, Button * b);
void                RemoveButtonFromSlideout(Slideout * s, Button * b);

Strip              *CreateStrip(char *name);
void                DestroyStrip(Strip * s);
void                RotateStripTo(Strip * s, char rot);
void                MoveStripTo(Strip * s, int x, int y);
void                ShowStrip(Strip * s);
void                HideStrip(Strip * s);
void                RemoveButtonFromStrip(Strip * s, Button * b);
void                AddButtonToStrip(Strip * s, Button * b, int x, int y);
void                RepackStrip(Strip * s);
Strip              *IsInStrip(int x, int y);

TextState          *TextGetState(TextClass * tclass, int active, int sticky,
				 int state);
char              **TextGetLines(char *text, int *count);
void                TextStateLoadFont(TextState * ts);
void                TextSize(TextClass * tclass, int active, int sticky,
			     int state, char *text, int *width, int *height,
			     int fsize);
void                TextDraw(TextClass * tclass, Window win, int active,
			     int sticky, int state, char *text, int x, int y,
			     int w, int h, int fsize, int justification);

void                SetupInit(void);

/* tooltips.c functions */
ToolTip            *CreateToolTip(char *name, ImageClass * ic0,
				  ImageClass * ic1, ImageClass * ic2,
				  ImageClass * ic3, ImageClass * ic4,
				  TextClass * tclass, int dist,
				  ImageClass * tooltippic);
void                ShowToolTip(ToolTip * tt, char *text, ActionClass * ac,
				int x, int y);
void                HideToolTip(ToolTip * tt);
void                FreeToolTip(ToolTip * tt);

void                ShowTaskMenu(void);
void                ShowAllTaskMenu(void);
void                ShowDeskMenu(void);
Menu               *RefreshTaskMenu(int desk);
Menu               *RefreshAllTaskMenu(Menu * m);
Menu               *RefreshDeskMenu(Menu * m);
void                HideMenu(Menu * m);
void                ShowMenu(Menu * m, char noshow);
MenuStyle          *CreateMenuStyle(void);
MenuItem           *CreateMenuItem(char *text, ImageClass * iclass,
				   int action_id, char *action_params,
				   Menu * child);
Menu               *CreateMenu(void);
void                DestroyMenu(Menu * m);
void                AddItemToMenu(Menu * menu, MenuItem * item);
void                AddTitleToMenu(Menu * menu, char *title);
void                RealizeMenu(Menu * m);
void                DrawMenuItem(Menu * m, MenuItem * mi, char shape);
Menu               *CreateMenuFromDirectory(char *name, MenuStyle * ms,
					    char *dir);
Menu               *CreateMenuFromFlatFile(char *name, MenuStyle * ms,
					   char *file, Menu * parent);
Menu               *CreateMenuFromGnome(char *name, MenuStyle * ms, char *dir);
Menu               *CreateMenuFromAllEWins(char *name, MenuStyle * ms);
Menu               *CreateMenuFromDesktopEWins(char *name, MenuStyle * ms,
					       int desk);
Menu               *CreateMenuFromDesktops(char *name, MenuStyle * ms);
Menu               *CreateMenuFromThemes(char *name, MenuStyle * ms);
Menu               *CreateMenuFromBorders(char *name, MenuStyle * ms);
int                 BorderNameCompare(Border * b1, Border * b2);
void                ShowMenuMasker(Menu * m);
void                HideMenuMasker(void);
void                RepackMenu(Menu * m);
void                EmptyMenu(Menu * m);
Menu               *CreateMenuFromGroups(char *name, MenuStyle * ms);
Menu               *RefreshGroupMenu(Menu * m);
void                ShowGroupMenu(void);

void                SetNewAreaSize(int ax, int ay);
void                GetCurrentArea(int *ax, int *ay);
void                SetAreaSize(int aw, int ah);
void                GetAreaSize(int *aw, int *ah);
void                InitCurrentArea(int ax, int ay);
void                SetCurrentArea(int ax, int ay);
void                MoveEwinToArea(EWin * ewin, int ax, int ay);
void                SetEwinToCurrentArea(EWin * ewin);
void                MoveCurrentAreaBy(int ax, int ay);
void                SetCurrentLinearArea(int a);
int                 GetCurrentLinearArea(void);
void                MoveCurrentLinearAreaBy(int a);
void                SlideWindowsBy(Window * win, int num, int dx, int dy,
				   int speed);
void                MoveEwinToLinearArea(EWin * ewin, int a);
void                MoveEwinLinearAreaBy(EWin * ewin, int a);

int                 Emkstemp(char *template);
void                SnapEwin(EWin * ewin, int dx, int dy, int *new_dx,
			     int *new_dy);
void                ArrangeEwin(EWin * ewin);
void                ArrangeEwinCentered(EWin * ewin, int focus);
void                ArrangeEwinXY(EWin * ewin, int *px, int *py);
void                ArrangeEwinCenteredXY(EWin * ewin, int *px, int *py);

void                SessionInit(void);
void                SessionSave(int shutdown);
void                doSMExit(void *params);
void                ProcessICEMSGS(void);
int                 GetSMfd(void);
void                SessionGetInfo(EWin * ewin, Atom atom_change);
void                SetSMID(char *smid);
void                SetSMFile(char *path);
void                SetSMUserThemePath(char *path);
char               *GetSMFile(void);
char               *GetGenericSMFile(void);
void                MatchEwinToSM(EWin * ewin);
void                MatchToSnapInfoPager(Pager * p);
void                MatchToSnapInfoIconbox(Iconbox * ib);
void                SaveSession(int shutdown);
void                autosave(void);

Progressbar        *CreateProgressbar(char *name, int width, int height);
void                SetProgressbar(Progressbar * p, int progress);
void                ShowProgressbar(Progressbar * p);
void                HideProgressbar(Progressbar * p);
void                FreeProgressbar(Progressbar * p);
Window             *ListProgressWindows(int *num);
void                RaiseProgressbars(void);

#if defined(__FILE__) && defined(__LINE__)
#define Efree(x) \
__Efree(x, __FILE__, __LINE__)
#define Emalloc(x) \
__Emalloc(x, __FILE__, __LINE__)
#define Erealloc(x, y) \
__Erealloc(x, y, __FILE__, __LINE__)
void               *__Emalloc(int size, const char *file, int line);
void               *__Erealloc(void *ptr, int size, const char *file, int line);
void                __Efree(void *ptr, const char *file, int line);

#else
/* We still want our special handling, even if they don't have file/line stuff -- mej */
#define Efree(x) \
__Efree(x, "<unknown>", 0)
#define Emalloc(x) \
__Emalloc(x, "<unknown>", 0)
#define Erealloc(x, y) \
__Erealloc(x, y, "<unknown>", 0)
#endif

char               *duplicate(char *s);

void                Alert(char *fmt, ...);
void                InitStringList(void);
void                AssignIgnoreFunction(int (*FunctionToAssign) (void *),
					 void *params);
void                AssignRestartFunction(int (*FunctionToAssign) (void *),
					  void *params);
void                AssignExitFunction(int (*FunctionToAssign) (void *),
				       void *params);
void                AssignTitleText(char *text);
void                AssignIgnoreText(char *text);
void                AssignRestartText(char *text);
void                AssignExitText(char *text);

void                Etmp(char *s);
void                md(char *s);
int                 exists(char *s);
void                mkdirs(char *s);
int                 isfile(char *s);
int                 isdir(char *s);
char              **ls(char *dir, int *num);
void                freestrlist(char **l, int num);
void                rm(char *s);
void                mv(char *s, char *ss);
void                cp(char *s, char *ss);
time_t              moddate(char *s);
int                 filesize(char *s);
int                 fileinode(char *s);
int                 filedev(char *s);
void                cd(char *s);
char               *cwd(void);
int                 permissions(char *s);
int                 owner(char *s);
int                 group(char *s);
char               *username(int uid);
char               *homedir(int uid);
char               *usershell(int uid);
char               *atword(char *s, int num);
char               *atchar(char *s, char c);
char               *getword(char *s, int num);
void                word(char *s, int num, char *wd);
int                 canread(char *s);
int                 canwrite(char *s);
int                 canexec(char *s);
char               *fileof(char *s);
char               *fullfileof(char *s);
char               *pathtoexec(char *file);
char               *pathtofile(char *file);
char               *FileExtension(char *file);
char               *field(char *s, int field);
int                 fillfield(char *s, int field, char *buf);
void                fword(char *s, int num, char *wd);
int                 findLocalizedFile(char *fname);

/* cursors.c functions */
ECursor            *CreateECursor(char *name, char *image, int native_id,
				  ImlibColor * fg, ImlibColor * bg);
void                ApplyECursor(Window win, ECursor * ec);
void                FreeECursor(ECursor * ec);

/* snaps.c functions */
void                SnapshotEwinDialog(EWin * ewin);
Snapshot           *FindSnapshot(EWin * ewin);
Snapshot           *GetSnapshot(EWin * ewin);
Snapshot           *NewSnapshot(char *name);
void                ClearSnapshot(Snapshot * sn);
void                SnapshotEwinBorder(EWin * ewin);
void                SnapshotEwinDesktop(EWin * ewin);
void                SnapshotEwinSize(EWin * ewin);
void                SnapshotEwinLocation(EWin * ewin);
void                SnapshotEwinLayer(EWin * ewin);
void                SnapshotEwinSticky(EWin * ewin);
void                SnapshotEwinIcon(EWin * ewin);
void                SnapshotEwinShade(EWin * ewin);
void                SnapshotEwinCmd(EWin * ewin);

void                SnapshotEwinSkipLists(EWin * ewin);
void                SnapshotEwinNeverFocus(EWin * ewin);

void                SnapshotEwinGroups(EWin * ewin, char onoff);
void                SnapshotEwinAll(EWin * ewin);
void                UnsnapshotEwin(EWin * ewin);
void                Real_SaveSnapInfo(int dumval, void *dumdat);
void                SaveSnapInfo(void);
void                SpawnSnappedCmds(void);
void                LoadSnapInfo(void);
void                MatchEwinToSnapInfo(EWin * ewin);
void                MatchEwinToSnapInfoAfter(EWin * ewin);
void                RememberImportantInfoForEwin(EWin * ewin);
void                RememberImportantInfoForEwins(EWin * ewin);

void                SetCoords(int x, int y, int w, int h);
void                HideCoords(void);

char               *append_merge_dir(char *dir, char ***list, int *count);
char              **ListThemes(int *number);
char               *GetDefaultTheme(void);
void                SetDefaultTheme(char *theme);

char                SanitiseThemeDir(char *dir);
void                Quicksort(void **a, int l, int r,
			      int (*CompareFunc) (void *d1, void *d2));

Dialog             *CreateDialog(char *name);
void                DialogBindKey(Dialog * d, char *key,
				  void (*func) (int val, void *data), int val,
				  void *data);
void                FreeDButton(DButton * db);
void                FreeDialog(Dialog * d);
void                DialogSetText(Dialog * d, char *text);
void                DialogSetTitle(Dialog * d, char *title);
void                DialogAddButton(Dialog * d, char *text,
				    void (*func) (int val, void *data),
				    char close);
void                DialogDrawButton(Dialog * d, int bnum);
void                DialogActivateButton(Window win, int inclick);
void                DialogDraw(Dialog * d);
void                DialogDrawArea(Dialog * d, int x, int y, int w, int h);
void                DialogRedraw(Dialog * d);
void                ShowDialog(Dialog * d);
void                DialogClose(Dialog * d);
void                DialogSetParamText(Dialog * d, char *fmt, ...);
void                DialogAlert(char *fmt, ...);
void                DialogAlertOK(char *fmt, ...);
void                DialogRestart(int val, void *data);
void                DialogQuit(int val, void *data);
DItem              *DialogInitItem(Dialog * d);
DItem              *DialogAddItem(DItem * dii, int type);
void                DialogItemSetCallback(DItem * di,
					  void (*func) (int val, void *data),
					  int val, char *data);
void                DialogItemSetClass(DItem * di, ImageClass * iclass,
				       TextClass * tclass);
void                DialogItemSetPadding(DItem * di, int left, int right,
					 int top, int bottom);
void                DialogItemSetFill(DItem * di, char fill_h, char fill_v);
void                DialogItemSetAlign(DItem * di, int align_h, int align_v);
void                DialogRealizeItem(Dialog * d, DItem * di);
void                DialogDrawItems(Dialog * d, DItem * di, int x, int y, int w,
				    int h);
void                DialogItemsRealize(Dialog * d);
void                DialogItemButtonSetText(DItem * di, char *text);
void                DialogItemCheckButtonSetText(DItem * di, char *text);
void                DialogItemTextSetText(DItem * di, char *text);
void                DialogItemRadioButtonSetEventFunc(DItem * di,
						      void (*func) (int val,
								    void
								    *data));
void                DialogItemCheckButtonSetState(DItem * di, char onoff);
void                DialogItemCheckButtonSetPtr(DItem * di, char *onoff_ptr);
void                DialogItemTableSetOptions(DItem * di, int num_columns,
					      char border, char homogenous_h,
					      char homogenous_v);
void                DialogItemSeparatorSetOrientation(DItem * di,
						      char horizontal);
void                DialogItemImageSetFile(DItem * di, char *image);
void                DialogFreeItem(DItem * di);
DItem              *DialogItemFindWindow(DItem * di, Window win);
void                DialogItemSetRowSpan(DItem * di, int row_span);
void                DialogItemSetColSpan(DItem * di, int col_span);
void                DialogSetExitFunction(Dialog * d,
					  void (*func) (int val, void *data),
					  int val, void *data);
void                DialogItemRadioButtonSetText(DItem * di, char *text);
void                DialogItemRadioButtonSetFirst(DItem * di, DItem * first);
void                DialogItemRadioButtonGroupSetValPtr(DItem * di,
							int *val_ptr);
void                DialogItemRadioButtonGroupSetVal(DItem * di, int val);
void                MoveTableBy(Dialog * d, DItem * di, int dx, int dy);
void                DialogItemSliderSetVal(DItem * di, int val);
void                DialogItemSliderSetBounds(DItem * di, int lower, int upper);
void                DialogItemSliderSetUnits(DItem * di, int units);
void                DialogItemSliderSetJump(DItem * di, int jump);
void                DialogItemSliderSetMinLength(DItem * di, int min);
void                DialogItemSliderSetValPtr(DItem * di, int *val_ptr);
void                DialogItemSliderSetOrientation(DItem * di, char horizontal);
void                DialogItemAreaSetSize(DItem * di, int w, int h);
void                DialogItemAreaGetSize(DItem * di, int *w, int *h);
Window              DialogItemAreaGetWindow(DItem * di);
void                DialogItemAreaSetEventFunc(DItem * di,
					       void (*func) (int val,
							     void *data));

/* fx.c exportable functions */
#define FX_OP_START  1
#define FX_OP_STOP   2
#define FX_OP_TOGGLE 3
void                FX_Op(const char *name, int fx_op);
void                FX_DeskChange(void);
void                FX_Pause(void);
char              **FX_Active(int *num);
int                 FX_IsOn(char *effect);

/* ipc.c functions */
int                 HandleIPC(char *params, Client * c);
void                ButtonIPC(int val, void *data);

/* sticky.c functions */
void                MakeWindowSticky(EWin * ewin);
void                MakeWindowUnSticky(EWin * ewin);

/* size.c functions */
void                MaxSize(EWin * ewin, char *resize_type);
void                MaxWidth(EWin * ewin, char *resize_type);
void                MaxHeight(EWin * ewin, char *resize_type);

/* modules.c functions */
int                 LoadModule(char *module_name);
int                 UnloadModule(char *module_name);
char               *ModuleErrorCodeToString(int error_code);
char               *ModuleListAsString(void);
int                 IsLoadedModule(char *module_name);
char               *FindModulePath(char *module_name);

Clone              *CloneEwin(EWin * ewin);
void                FreeClone(Clone * c);
void                RemoveClones(void);
void                CloneDesktop(int d);

void                PagerScaleLine(Pixmap dest, Window src, int dx, int dy,
				   int sw, int pw, int sy, int sh);
void                PagerScaleRect(Pixmap dest, Window src, int sx, int sy,
				   int dx, int dy, int sw, int sh, int dw,
				   int dh);
Pager              *CreatePager(void);
EWin               *EwinInPagerAt(Pager * p, int x, int y);
void                PagerResize(Pager * p, int w, int h);
void                PagerShow(Pager * p);
void                PagerHide(Pager * p);
void                PagerTitle(Pager * p, char *title);
void                PagerKill(Pager * p);
Pager             **PagersForDesktop(int d, int *num);
void                RedrawPagersForDesktop(int d, char newbg);
void                ForceUpdatePagersForDesktop(int d);
void                PagerEwinUpdateMini(Pager * p, EWin * ewin);
void                PagerEwinUpdateFromPager(Pager * p, EWin * ewin);
void                PagerRedraw(Pager * p, char newbg);
void                PagerForceUpdate(Pager * p);
void                PagerShowMenu(Pager * p, int x, int y);
void                PagerReArea(void);
void                PagerEwinOutsideAreaUpdate(EWin * ewin);
void                PagerAreaAt(Pager * p, int x, int y, int *ax, int *ay);
void                UpdatePagerSel(void);
void                PagerHandleMotion(Pager * p, Window win, int x, int y);
void                EnableAllPagers(void);
void                DisableAllPagers(void);
void                PagerHideHi(Pager * p);
void                PagerShowHi(Pager * p, EWin * ewin, int x, int y, int w,
				int h);
void                PagerSetHiQ(char onoff);
void                PagerSetSnap(char onoff);
void                PagerHideAllHi(void);
void                NewPagerForDesktop(int desk);
void                EnableSinglePagerForDesktop(int desk);
int                 PagerForDesktop(int desk);
void                DisablePagersForDesktop(int desk);

int                 CompareNetVersion(int major, int minor, int patchlevel,
				      char *date);
int                 RetrieveUpdate(int major, int minor, int patchlevel,
				   char *date);
int                 InstallUpdate(void);
void                SpawnNetworkTester(void);
void                TestForUpdate(void);
void                CheckForNewMOTD(int val, void *data);

char               *GetNetText(char *URL);
time_t              GetNetFileDate(char *URL);
int                 SaveNetFile(char *URL, char *pathtosave);

void                SettingsPager(void);
void                SettingsFocus(void);
void                SettingsMoveResize(void);
void                SettingsDesktops(void);
void                SettingsArea(void);
void                SettingsPlacement(void);
void                SettingsIcons(void);
void                SettingsAutoRaise(void);
void                SettingsTooltips(void);
void                SettingsKDE(void);
void                SettingsAudio(void);
void                SettingsSpecialFX(void);
void                SettingsBackground(Background * bg);
void                SettingsIconbox(char *name);
void                SettingsGroups(EWin * ewin);
void                SettingsDefaultGroupControl(void);
void                RemoveRememberedWindow(EWin * ewin);
void                SettingsRemember(void);
void                SettingsMiscellaneous(void);

void                BGSettingsGoTo(Background * bg);

int                 WarpFocusHandleEvent(XEvent * event);
void                WarpFocus(int delta);
void                WarpFocusFinish(void);

/* groups.c functions */
Group              *CreateGroup(void);
void                FreeGroup(Group * g);
void                CopyGroupConfig(GroupConfig * src, GroupConfig * dest);
void                BreakWindowGroup(EWin * ewin, Group * g);
void                BuildWindowGroup(EWin ** ewins, int num);
int                 EwinInGroup(EWin * ewin, Group * g);
Group              *EwinsInGroup(EWin * ewin1, EWin * ewin2);
char              **GetWinGroupMemberNames(Group ** groups, int num);
void                AddEwinToGroup(EWin * ewin, Group * g);
void                RemoveEwinFromGroup(EWin * ewin, Group * g);
void                ShowHideWinGroups(EWin * ewin, Group * g, char onoff);
void                ChooseGroupDialog(EWin * ewin, char *message,
				      char group_select, int action);
void                SaveGroups(void);
void                LoadGroups(void);
void                GroupSelectCallback(int val, void *data);
void                GroupFeatureChangeCallback(int val, void *data);
void                GroupCallback(int val, void *data);

/* zoom.c functions */
EWin               *GetZoomEWin(void);
void                ReZoom(EWin * ewin);
char                InZoom(void);
char                CanZoom(void);
void                ZoomInit(void);
void                Zoom(EWin * ewin);

/* screen.c functions */
void                ScreenInit(void);
int                 ScreenGetGeometry(int x, int y, int *px, int *py,
				      int *pw, int *ph);
int                 GetPointerScreenGeometry(int *px, int *py,
					     int *pw, int *ph);

/* Global vars */
extern const char   e_wm_name[];
extern const char   e_wm_version[];
extern pid_t        master_pid;
extern int          master_screen;
extern int          display_screens;
extern int          single_screen_mode;
extern Display     *disp;
extern ImlibData   *pImlibData;
extern ImlibData   *prImlibData;

#if USE_FNLIB
extern FnlibData   *pFnlibData;
#endif
extern List        *lists;
extern int          event_base_shape;
extern Window       comms_win;
extern Root         root;
extern int          (*(ActionFunctions[ACTION_NUMBEROF])) (void *);
extern EMode        mode;
extern Desktops     desks;
extern Window       grab_window;
extern Window       init_win1;
extern Window       init_win2;
extern Window       init_win_ext;
extern Window       bpress_win;
extern int          deskorder[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
extern int          sound_fd;

#define FILEPATH_LEN_MAX 4096
extern char         themepath[FILEPATH_LEN_MAX];
extern char         themename[FILEPATH_LEN_MAX];
extern char        *command;
extern char         mustdel;

#define DRAW_QUEUE_ENABLE 1
extern char         queue_up;
extern Menu        *all_task_menu;
extern Menu        *task_menu[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
extern Menu        *desk_menu;
extern Menu        *group_menu;
extern char         no_overwrite;
extern char         clickmenu;
extern Window       last_bpress;
extern int          child_count;
extern pid_t       *e_children;
extern int          numlock_mask;
extern int          scrollock_mask;
extern int          mask_mod_combos[8];
extern Group       *current_group;
extern char        *dstr;
extern char        *badtheme;
extern char        *badreason;
extern char        *e_machine_name;

#ifdef HAS_XINERAMA
extern char         xinerama_active;
#endif

#define EDBUG_CLIENT_MESSAGES	0x00000001
#define EDBUG_PROPERTY_CHANGE	0x00000002
extern int          debug_flags;
