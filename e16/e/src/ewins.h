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
#ifndef _EWIN_H_
#define _EWIN_H_

#if 0
typedef struct _ewin EWin;
#endif

struct _ewin
{
   EObj                o;
   char                type;
   Window              win_container;

   const Border       *border;
   const Border       *normal_border;
   const Border       *previous_border;
   EWinBit            *bits;

   char                toggle;	/* FIXME - Eliminate */

   struct
   {
      Window              win;
      int                 x, y, w, h, bw;
      Colormap            cmap;
      char                no_resize_h;
      char                no_resize_v;
      Constraints         width, height;
      int                 base_w, base_h;
      int                 w_inc, h_inc;
      int                 grav;
      double              aspect_min, aspect_max;
      long                event_mask;
   } client;

   struct
   {
      char                state;
      char                shaped;
      char                docked;
      char                visibility;
      char                iconified;
      char                shaded;
      char                active;

      unsigned            placed:1;
      unsigned            maximized_horz:1;
      unsigned            maximized_vert:1;
      unsigned            fullscreen:1;
      unsigned            showingdesk:1;	/* Iconified by show desktop */
      unsigned            attention:1;
      unsigned            animated:1;

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
      char                fixedpos;
      char                never_use_area;
      char                ignorearrange;
      char                skip_ext_task;
      char                skip_ext_pager;
      char                skip_focuslist;
      char                skip_winlist;
      char                focusclick;	/* Click to focus */
      char                never_focus;	/* Never focus */
      char                no_button_grabs;
      char                no_actions;
      unsigned            donthide:1;	/* Don't hide on show desktop */
      unsigned            vroot:1;	/* Virtual root window */
      unsigned            autosave:1;
      unsigned            no_border:1;	/* Never apply border */
      unsigned            never_iconify:1;	/* Never iconify */
   } props;
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
   } ewmh;
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

#define EwinIsMapped(ewin)		(ewin->state.state >= EWIN_STATE_MAPPED)
#define EwinIsInternal(ewin)		(ewin->type != EWIN_TYPE_NORMAL)
#define EwinIsTransientChild(ewin)	(ewin->icccm.transient > 0)
#define EwinIsTransient(ewin)		(ewin->icccm.transient != 0)
#define EwinGetTransientFor(ewin)	(ewin->icccm.transient_for)
#define EwinGetTransientCount(ewin)	(ewin->icccm.transient_count)
#define EwinIsWindowGroupLeader(ewin)	(ewin->icccm.is_group_leader)
#define EwinGetWindowGroup(ewin)	(ewin->icccm.group)

#define _EwinGetClientWin(ewin)		(ewin->client.win)
#define _EwinGetClientXwin(ewin)	(ewin->client.win)
#define _EwinGetContainerWin(ewin)	(ewin->win_container)
#define _EwinGetContainerXwin(ewin)	(ewin->win_container)

/* ewins.c */
#define EWIN_CHANGE_NAME        (1<<0)
#define EWIN_CHANGE_ICON_NAME   (1<<1)
#define EWIN_CHANGE_ICON_PMAP   (1<<2)
#define EWIN_CHANGE_DESKTOP     (1<<3)
#define EWIN_CHANGE_LAYER       (1<<4)
#define EWIN_CHANGE_OPACITY     (1<<5)
#define EWIN_CHANGE_ATTENTION   (1<<6)

void                EwinShapeSet(EWin * ewin);
void                EwinFloatAt(EWin * ewin, int x, int y);
void                EwinUnfloatAt(EWin * ewin, int desk, int x, int y);
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
int                 EwinIsOnScreen(const EWin * ewin);
void                EwinRememberPositionSet(EWin * ewin);
void                EwinRememberPositionGet(EWin * ewin, int *px, int *py);

void                EwinChange(EWin * ewin, unsigned int flag);

void                EwinsEventsConfigure(int mode);
void                EwinsSetFree(void);
void                EwinsShowDesktop(int on);

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
void                EwinIconify(EWin * ewin);
void                EwinDeIconify(EWin * ewin);
void                EwinStick(EWin * ewin);
void                EwinUnStick(EWin * ewin);
void                EwinInstantShade(EWin * ewin, int force);
void                EwinInstantUnShade(EWin * ewin);
void                EwinShade(EWin * ewin);
void                EwinUnShade(EWin * ewin);
void                EwinSetFullscreen(EWin * ewin, int on);
void                EwinMoveToArea(EWin * ewin, int ax, int ay);
void                EwinMoveToDesktop(EWin * ewin, int num);
void                EwinMoveToDesktopAt(EWin * ewin, int num, int x, int y);

unsigned int        OpacityExt(int op);
void                EwinOpClose(EWin * ewin);
void                EwinOpKill(EWin * ewin);
void                EwinOpRaise(EWin * ewin);
void                EwinOpLower(EWin * ewin);
void                EwinOpRaiseLower(EWin * ewin);
void                EwinOpStick(EWin * ewin, int on);
void                EwinOpSkipLists(EWin * ewin, int skip);
void                EwinOpSkipTask(EWin * ewin, int skip);
void                EwinOpSkipFocus(EWin * ewin, int skip);
void                EwinOpSkipWinlist(EWin * ewin, int skip);
void                EwinOpNeverFocus(EWin * ewin, int on);
void                EwinOpIconify(EWin * ewin, int on);
void                EwinOpShade(EWin * ewin, int on);
void                EwinOpSetLayer(EWin * ewin, int layer);
void                EwinOpSetBorder(EWin * ewin, const char *name);
void                EwinOpSetOpacity(EWin * ewin, int opacity);
void                EwinOpMoveToDesk(EWin * ewin, int desk);
void                EwinOpMoveToArea(EWin * ewin, int x, int y);

/* zoom.c */
EWin               *GetZoomEWin(void);
void                ReZoom(EWin * ewin);
char                InZoom(void);
char                CanZoom(void);
void                ZoomInit(void);
void                Zoom(EWin * ewin);

#endif /* _EWIN_H_ */
