/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#ifndef _EWIN_H_
#define _EWIN_H_

#include "eobj.h"
#include "etypes.h"
#include "xwin.h"

/* Window operation sources */
#define OPSRC_NA        0
#define OPSRC_APP       1
#define OPSRC_USER      2
#define OPSRC_WM        3

typedef union
{
   unsigned char       all:8;
   struct
   {
      unsigned char       rsvd:2;
      unsigned char       border:1;	/* W   */
      unsigned char       close:1;	/*  AU */
      unsigned char       focus:1;	/* WA  */
      unsigned char       iconify:1;	/* W U */
      unsigned char       move:1;	/*  AU */
      unsigned char       size:1;	/*  AU */
   } b;
} EWinInhibit;

#define EwinInhGetApp(ewin, item)      (ewin->inh_app.b.item)
#define EwinInhSetApp(ewin, item, on)  ewin->inh_app.b.item = (on)
#define EwinInhGetUser(ewin, item)     (ewin->inh_user.b.item)
#define EwinInhSetUser(ewin, item, on) ewin->inh_user.b.item = (on)
#define EwinInhGetWM(ewin, item)       (ewin->inh_wm.b.item)
#define EwinInhSetWM(ewin, item, on)   ewin->inh_wm.b.item = (on)

struct _ewin
{
   EObj                o;
   char                type;
   Window              win_container;

   const Border       *border;
   const Border       *normal_border;
   const Border       *previous_border;
   EWinBit            *bits;

   struct
   {
      Window              win;
      int                 x, y, w, h, bw;
      int                 grav;
      Colormap            cmap;
      long                event_mask;
      unsigned            argb:1;
   } client;

   struct
   {
      char                state;
      char                visibility;
      char                shaped;
      char                shaded;

      unsigned            identified:1;
      unsigned            placed:1;
      unsigned            iconified:1;
      unsigned            docked:1;

      unsigned            click_grab_isset:1;
      unsigned            maximized_horz:1;
      unsigned            maximized_vert:1;
      unsigned            fullscreen:1;

      unsigned            active:1;
      unsigned            modal:1;
      unsigned            attention:1;

      unsigned            showingdesk:1;	/* Iconified by show desktop */
      unsigned            animated:1;
      unsigned            moving:1;
      unsigned            resizing:1;
      unsigned            show_coords:1;
      unsigned            maximizing:1;

      /* Derived state flags. Change only in EwinStateUpdate() */
      unsigned            no_border:1;

      unsigned            inhibit_move:1;
      unsigned            inhibit_resize:1;
      unsigned            inhibit_iconify:1;
      unsigned            inhibit_shade:1;
      unsigned            inhibit_stick:1;
      unsigned            inhibit_max_hor:1;
      unsigned            inhibit_max_ver:1;
      unsigned            inhibit_fullscreeen:1;
      unsigned            inhibit_change_desk:1;
      unsigned            inhibit_close:1;

      unsigned            inhibit_actions:1;
      unsigned            inhibit_focus:1;
   } state;
   struct
   {
      unsigned            never_use_area:1;
      unsigned            ignorearrange:1;
      unsigned            skip_ext_task:1;
      unsigned            skip_ext_pager:1;
      unsigned            skip_focuslist:1;
      unsigned            skip_winlist:1;
      unsigned            focusclick:1;	/* Click to focus */
      unsigned            no_button_grabs:1;
      unsigned            no_actions:1;
      unsigned            no_resize_h:1;
      unsigned            no_resize_v:1;
      unsigned            donthide:1;	/* Don't hide on show desktop */
      unsigned            vroot:1;	/* Virtual root window */
      unsigned            autosave:1;
      unsigned            no_border:1;	/* Never apply border */
      unsigned            focus_when_mapped:1;
      unsigned            opaque_when_focused:1;
   } props;
   EWinInhibit         inh_app;
   EWinInhibit         inh_user;
   EWinInhibit         inh_wm;
   struct
   {
      char               *wm_name;
      char               *wm_icon_name;
      char               *wm_res_name;
      char               *wm_res_class;
      char               *wm_role;
      char               *wm_command;
      char               *wm_machine;
      /* WM_HINTS */
      char                need_input;
      char                start_iconified;
      Pixmap              icon_pmap, icon_mask;
      Window              icon_win;
      Window              group;
      char                urgency;
      /* WM_PROTOCOLS */
      char                take_focus;
      char                delete_window;
      /* WM_TRANSIENT_FOR */
      signed char         transient;
      Window              transient_for;	/* We are a transient for ... */
      int                 transient_count;	/* We have <N> transients */
      /* WM_CLIENT_LEADER */
      Window              client_leader;

      /* WM_NORMAL_HINTS */
      Constraints         width, height;
      int                 base_w, base_h;
      int                 w_inc, h_inc;
      int                 grav;
      double              aspect_min, aspect_max;

      char                is_group_leader;
   } icccm;
   struct
   {
      unsigned            valid:1;
      unsigned            decor_border:1;
      unsigned            decor_resizeh:1;
      unsigned            decor_title:1;
      unsigned            decor_menu:1;
      unsigned            decor_minimize:1;
      unsigned            decor_maximize:1;
      unsigned            func_resize:1;
      unsigned            func_move:1;
      unsigned            func_minimize:1;
      unsigned            func_maximize:1;
      unsigned            func_close:1;
   } mwm;
   struct
   {
      char               *wm_name;
      char               *wm_icon_name;
      unsigned int       *wm_icon, wm_icon_len;
      unsigned int        opacity;
#if USE_XSYNC
      char                sync_request_enable;
      XID                 sync_request_counter;
      long long           sync_request_count;
#endif
   } ewmh;
   struct
   {
      signed char         gravity;
      int                 ax, ay;	/* Current placed area */
      int                 gx, gy;	/* Distance to edge given by gravity */
   } place;
   struct
   {
      int                 left, right, top, bottom;
   } strut;
   struct
   {
      char                shape;
      char                border;
   } update;

   int                 num_groups;
   Group             **groups;
   int                 area_x, area_y;
   char               *session_id;
   PmapMask            mini_pmm;
   int                 mini_w, mini_h;

   int                 shape_x, shape_y, shape_w, shape_h;
   int                 req_x, req_y;

   Snapshot           *snap;
   int                 head;	/* Unused? */

   int                 vx, vy;	/* Position in virtual root */
   int                 lx, ly;	/* Last pos */
   int                 lw, lh;	/* Last size */
   int                 ll;	/* Last layer */

   void               *data;	/* Data hook for internal windows */
   void                (*Layout) (EWin * ewin, int *px, int *py, int *pw,
				  int *ph);
   void                (*MoveResize) (EWin * ewin, int resize);
   void                (*Close) (EWin * ewin);
};

#define EWIN_STATE_NEW          0	/* New */
#define EWIN_STATE_STARTUP      1	/* New - during startup */
#define EWIN_STATE_WITHDRAWN    2
#define EWIN_STATE_ICONIC       3
#define EWIN_STATE_MAPPED       4

#define EWIN_TYPE_NORMAL        0x00
#define EWIN_TYPE_DIALOG        0x01
#define EWIN_TYPE_MENU          0x02
#define EWIN_TYPE_ICONBOX       0x04
#define EWIN_TYPE_PAGER         0x08

#define EWIN_GRAVITY_NW         0
#define EWIN_GRAVITY_NE         1
#define EWIN_GRAVITY_SW         2
#define EWIN_GRAVITY_SE         3

#define EwinGetDesk(ewin)		EoGetDesk(ewin)

#define EwinIsMapped(ewin)		((ewin)->state.state >= EWIN_STATE_MAPPED)
#define EwinIsInternal(ewin)		((ewin)->type != EWIN_TYPE_NORMAL)
#define EwinIsTransientChild(ewin)	((ewin)->icccm.transient > 0)
#define EwinIsTransient(ewin)		((ewin)->icccm.transient != 0)
#define EwinGetTransientFor(ewin)	((ewin)->icccm.transient_for)
#define EwinGetTransientCount(ewin)	((ewin)->icccm.transient_count)
#define EwinIsWindowGroupLeader(ewin)	((ewin)->icccm.is_group_leader)
#define EwinGetWindowGroup(ewin)	((ewin)->icccm.group)

#define _EwinGetClientWin(ewin)		((ewin)->client.win)
#define _EwinGetClientXwin(ewin)	((ewin)->client.win)
#define _EwinGetContainerWin(ewin)	((ewin)->win_container)
#define _EwinGetContainerXwin(ewin)	((ewin)->win_container)

/* ewins.c */
#define EWIN_CHANGE_NAME        (1<<0)
#define EWIN_CHANGE_ICON_NAME   (1<<1)
#define EWIN_CHANGE_ICON_PMAP   (1<<2)
#define EWIN_CHANGE_DESKTOP     (1<<3)
#define EWIN_CHANGE_LAYER       (1<<4)
#define EWIN_CHANGE_OPACITY     (1<<5)
#define EWIN_CHANGE_ATTENTION   (1<<6)

void                EwinShapeSet(EWin * ewin);
void                RaiseEwin(EWin * ewin);
void                LowerEwin(EWin * ewin);
void                ShowEwin(EWin * ewin);
void                HideEwin(EWin * ewin);
void                DetermineEwinFloat(EWin * ewin, int dx, int dy);
EWin               *GetEwinPointerInClient(void);
EWin               *GetEwinByCurrentPointer(void);
EWin               *GetFocusEwin(void);
EWin               *GetContextEwin(void);
void                SetContextEwin(EWin * ewin);
void                EwinGetPosition(const EWin * ewin, int x, int y, int bw,
				    int grav, int *px, int *py);
void                EwinUpdateShapeInfo(EWin * ewin);
void                EwinPropagateShapes(EWin * ewin);
void                EwinStateUpdate(EWin * ewin);
void                AddToFamily(EWin * ewin, Window win);
EWin               *AddInternalToFamily(Window win, const char *bname, int type,
					void *ptr,
					void (*init) (EWin * ewin, void *ptr));
void                EwinReparent(EWin * ewin, Window parent);
Window              EwinGetClientWin(const EWin * ewin);
const char         *EwinGetName(const EWin * ewin);
const char         *EwinGetIconName(const EWin * ewin);
const char         *EwinBorderGetName(const EWin * ewin);
void                EwinBorderGetSize(const EWin * ewin, int *bl, int *br,
				      int *bt, int *bb);
void                EwinBorderUpdateState(EWin * ewin);
int                 EwinIsOnScreen(const EWin * ewin);
void                EwinRememberPositionSet(EWin * ewin);
void                EwinRememberPositionGet(EWin * ewin, Desk * dsk,
					    int *px, int *py);
void                EwinSetPlacementGravity(EWin * ewin, int x, int y);
void                EwinReposition(EWin * ewin);
unsigned int        EwinFlagsEncode(const EWin * ewin);
void                EwinFlagsDecode(EWin * ewin, unsigned int flags);
void                EwinUpdateOpacity(EWin * ewin);

void                EwinChange(EWin * ewin, unsigned int flag);

void                EwinsEventsConfigure(int mode);
void                EwinsSetFree(void);
void                EwinsShowDesktop(int on);
void                EwinsMoveStickyToDesk(Desk * d);

/* ewin-ops.c */
void                SlideEwinTo(EWin * ewin, int fx, int fy, int tx, int ty,
				int speed);
void                SlideEwinsTo(EWin ** ewin, int *fx, int *fy, int *tx,
				 int *ty, int num_wins, int speed);
void                EwinFixPosition(EWin * ewin);
void                EwinMove(EWin * ewin, int x, int y);
void                EwinResize(EWin * ewin, int w, int h);
void                EwinMoveResize(EWin * ewin, int x, int y, int w, int h);
void                EwinMoveResizeWithGravity(EWin * ewin, int x, int y, int w,
					      int h, int grav);
void                EwinMoveToDesktop(EWin * ewin, Desk * d);
void                EwinMoveToDesktopAt(EWin * ewin, Desk * d, int x, int y);
void                EwinIconify(EWin * ewin);
void                EwinDeIconify(EWin * ewin);
void                EwinInstantShade(EWin * ewin, int force);
void                EwinInstantUnShade(EWin * ewin);
void                EwinShade(EWin * ewin);
void                EwinUnShade(EWin * ewin);
void                EwinMoveToArea(EWin * ewin, int ax, int ay);
void                EwinMoveToLinearArea(EWin * ewin, int a);
void                EwinMoveLinearAreaBy(EWin * ewin, int a);

void                EwinOpMove(EWin * ewin, int source, int x, int y);
void                EwinOpResize(EWin * ewin, int source, int w, int h);
void                EwinOpMoveResize(EWin * ewin, int source, int x, int y,
				     int w, int h);
void                EwinOpMoveToDesktopAt(EWin * ewin, int source, Desk * dsk,
					  int x, int y);
void                EwinOpFloatAt(EWin * ewin, int source, int x, int y);
void                EwinOpUnfloatAt(EWin * ewin, int source, Desk * d,
				    int x, int y);
void                EwinOpClose(EWin * ewin, int source);
void                EwinOpActivate(EWin * ewin, int source);
void                EwinOpKill(EWin * ewin, int source);
void                EwinOpRaise(EWin * ewin, int source);
void                EwinOpLower(EWin * ewin, int source);
void                EwinOpStick(EWin * ewin, int source, int on);
void                EwinOpSkipLists(EWin * ewin, int source, int skip);
void                EwinOpIconify(EWin * ewin, int source, int on);
void                EwinOpShade(EWin * ewin, int source, int on);
void                EwinOpSetLayer(EWin * ewin, int source, int layer);
void                EwinOpSetBorder(EWin * ewin, int source, const char *name);
void                EwinOpSetOpacity(EWin * ewin, int source, int opacity);
void                EwinOpMoveToDesk(EWin * ewin, int source, Desk * dsk,
				     int inc);
void                EwinOpFullscreen(EWin * ewin, int source, int on);

/* finders.c */
EWin               *EwinFindByPtr(const EWin * ewin);
EWin               *EwinFindByFrame(Window win);
EWin               *EwinFindByClient(Window win);
EWin               *EwinFindByChildren(Window win);
EWin               *EwinFindByString(const char *win, int type);
EWin              **EwinListTransients(const EWin * ewin, int *num, int group);
EWin              **EwinListTransientFor(const EWin * ewin, int *num);

/* session.c */
void                SessionGetInfo(EWin * ewin, Atom atom_change);

/* size.c */
void                MaxSize(EWin * ewin, const char *resize_type);
void                MaxWidth(EWin * ewin, const char *resize_type);
void                MaxHeight(EWin * ewin, const char *resize_type);

/* stacking.c */
EWin               *const *EwinListStackGet(int *num);
EWin               *const *EwinListFocusGet(int *num);
EWin               *const *EwinListGetForDesk(int *num, Desk * d);
EWin               *const *EwinListOrderGet(int *num);
EWin               *EwinListStackGetTop(void);
int                 EwinListStackIsRaised(const EWin * ewin);

#define EwinListGetAll EwinListStackGet

/* zoom.c */
EWin               *GetZoomEWin(void);
void                ReZoom(EWin * ewin);
char                InZoom(void);
char                CanZoom(void);
void                ZoomInit(void);
void                Zoom(EWin * ewin);

#endif /* _EWIN_H_ */
