/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "E.h"
#include <time.h>

typedef struct _remwinlist
{
   EWin               *ewin;
   char                remember;
}
RememberWinList;

static int          tmp_focus;
static char         tmp_new_focus;
static char         tmp_popup_focus;
static char         tmp_owner_popup_focus;
static char         tmp_raise_focus;
static char         tmp_warp_focus;

static char         tmp_warp_after_focus;
static char         tmp_raise_after_focus;
static char         tmp_display_warp;
static char         tmp_showsticky;
static char         tmp_showshaded;
static char         tmp_showiconified;
static char         tmp_warpfocused;

static char         tmp_clickalways;

static void
CB_ConfigureFocus(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.focus.mode = tmp_focus;
	Conf.focus.all_new_windows_get_focus = tmp_new_focus;
	Conf.focus.new_transients_get_focus = tmp_popup_focus;
	Conf.focus.new_transients_get_focus_if_group_focused =
	   tmp_owner_popup_focus;
	Conf.focus.raise_on_next = tmp_raise_focus;
	Conf.focus.warp_on_next = tmp_warp_focus;

	Conf.warplist.warp_on_select = tmp_warp_after_focus;
	Conf.warplist.raise_on_select = tmp_raise_after_focus;
	Conf.warplist.enable = tmp_display_warp;
	Conf.warplist.showsticky = tmp_showsticky;
	Conf.warplist.showshaded = tmp_showshaded;
	Conf.warplist.showiconified = tmp_showiconified;
	Conf.warplist.warpfocused = tmp_warpfocused;

	Conf.focus.clickraises = tmp_clickalways;
	FocusFix();
     }
   autosave();
}

void
SettingsFocus(void)
{
   Dialog             *d;
   DItem              *table, *di, *radio;

   if ((d = FindItem("CONFIGURE_FOCUS", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_FOCUS");

   tmp_focus = Conf.focus.mode;
   tmp_new_focus = Conf.focus.all_new_windows_get_focus;
   tmp_popup_focus = Conf.focus.new_transients_get_focus;
   tmp_owner_popup_focus = Conf.focus.new_transients_get_focus_if_group_focused;
   tmp_raise_focus = Conf.focus.raise_on_next;
   tmp_warp_focus = Conf.focus.warp_on_next;

   tmp_raise_after_focus = Conf.warplist.raise_on_select;
   tmp_warp_after_focus = Conf.warplist.warp_on_select;
   tmp_display_warp = Conf.warplist.enable;
   tmp_showsticky = Conf.warplist.showsticky;
   tmp_showshaded = Conf.warplist.showshaded;
   tmp_showiconified = Conf.warplist.showiconified;
   tmp_warpfocused = Conf.warplist.warpfocused;

   tmp_clickalways = Conf.focus.clickraises;

   d = DialogCreate("CONFIGURE_FOCUS");
   DialogSetTitle(d, _("Focus Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/focus.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Focus\n" "Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemRadioButtonSetText(di, _("Focus follows pointer"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemRadioButtonSetText(di, _("Focus follows pointer sloppily"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemRadioButtonSetText(di, _("Focus follows mouse clicks"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_focus);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Clicking in a window always raises it"));
   DialogItemCheckButtonSetState(di, tmp_clickalways);
   DialogItemCheckButtonSetPtr(di, &tmp_clickalways);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("All new windows first get the focus"));
   DialogItemCheckButtonSetState(di, tmp_new_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_new_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Only new dialog windows get the focus"));
   DialogItemCheckButtonSetState(di, tmp_popup_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_popup_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_
				("Only new dialogs whose owner is focused get the focus"));
   DialogItemCheckButtonSetState(di, tmp_owner_popup_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_owner_popup_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Raise windows on focus switch"));
   DialogItemCheckButtonSetState(di, tmp_raise_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_raise_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_
				("Send mouse pointer to window on focus switch"));
   DialogItemCheckButtonSetState(di, tmp_warp_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_warp_focus);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Display and use focus list"));
   DialogItemCheckButtonSetState(di, tmp_display_warp);
   DialogItemCheckButtonSetPtr(di, &tmp_display_warp);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Include sticky windows in focus list"));
   DialogItemCheckButtonSetState(di, tmp_showsticky);
   DialogItemCheckButtonSetPtr(di, &tmp_showsticky);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Include shaded windows in focus list"));
   DialogItemCheckButtonSetState(di, tmp_showshaded);
   DialogItemCheckButtonSetPtr(di, &tmp_showshaded);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_("Include iconified windows in focus list"));
   DialogItemCheckButtonSetState(di, tmp_showiconified);
   DialogItemCheckButtonSetPtr(di, &tmp_showiconified);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Focus windows while switching"));
   DialogItemCheckButtonSetState(di, tmp_warpfocused);
   DialogItemCheckButtonSetPtr(di, &tmp_warpfocused);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Raise windows after focus switch"));
   DialogItemCheckButtonSetState(di, tmp_raise_after_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_raise_after_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_
				("Send mouse pointer to window after focus switch"));
   DialogItemCheckButtonSetState(di, tmp_warp_after_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_warp_after_focus);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureFocus, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureFocus, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureFocus, 1);
   DialogSetExitFunction(d, CB_ConfigureFocus, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureFocus, 0);
   ShowDialog(d);
}

static int          tmp_move;
static int          tmp_resize;
static int          tmp_geominfo = 1;

/*
static void         je_move(int, void *);
static void         je_moveend(int, void *);
static void         je_movectrl(int, void *);
static void         je_resizectrl(int, void *);

float               je_angle = 0.0, je_radius = 0.0;
int                 je_prevx, je_prevy;
int                 je_prevmode = -1;
*/

static void
CB_ConfigureMoveResize(Dialog * d, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.movres.mode_move = tmp_move;
	Conf.movres.mode_resize = tmp_resize;
	Conf.movres.mode_info = tmp_geominfo;
     }
   if (val)
      if ((d =
	   FindItem("CONFIGURE_MOVERESIZE_EXAMPLE", 0, LIST_FINDBY_NAME,
		    LIST_TYPE_DIALOG)))
	{
	   DialogClose(d);
	   /* je_moveend(je_prevmode, (void *)1); */
	}
   autosave();
}

/* I'm commenting this out for now. --cK.

#define JE_BASE_X	100
#define JE_BASE_Y	100
#define JE_DELAY	0.02

static void
je_move(int movemode, void *foo)
{
   Dialog             *d;
   EWin               *ew;
   int                 je_x, je_y;

   d =
      FindItem("CONFIGURE_MOVERESIZE_EXAMPLE", 0, LIST_FINDBY_NAME,
	       LIST_TYPE_DIALOG);
   if (!d)
      return;

   ew = FindEwinByDialog(d);
   if (!ew)
      return;

   Mode.ewin = ew;
   if (je_prevmode != movemode)
      DrawEwinShape(ew, je_prevmode, je_prevx, je_prevy, ew->client.w,
		    ew->client.h, 2);

   je_x = JE_BASE_X - (cos(je_angle * (M_PI / 180.0)) * je_radius);
   je_y = JE_BASE_Y - (sin(je_angle * (M_PI / 180.0)) * je_radius);
   DrawEwinShape(ew, movemode, je_x, je_y, ew->client.w, ew->client.h,
		 je_prevmode == movemode);

   je_prevx = je_x;
   je_prevy = je_y;
   je_prevmode = movemode;

   je_angle += (180.0 * 0.02);
   if (je_angle > 360.0)
      je_angle -= 360.0;

   if (je_radius < 50.0)
      je_radius += 0.5;

   DoIn("JE_MOVE", JE_DELAY, je_move, movemode, NULL);
   return;
   foo = NULL;
}

static void
je_moveend(int movemode, void *foo)
{
   Dialog             *d;
   EWin               *ew;
   int                 je_x, je_y;

   d =
      FindItem("CONFIGURE_MOVERESIZE_EXAMPLE", 0, LIST_FINDBY_NAME,
	       LIST_TYPE_DIALOG);
   if (!d)
      return;

   ew = FindEwinByDialog(d);
   if (!ew)
      return;

   Mode.ewin = ew;
   if (je_prevmode != movemode)
      DrawEwinShape(ew, je_prevmode, je_prevx, je_prevy, ew->client.w,
		    ew->client.h, 2);

   if (je_radius <= 0.0 || foo)
     {
	je_radius = 0.0;
	DrawEwinShape(ew, movemode, JE_BASE_X, JE_BASE_Y, ew->client.w,
		      ew->client.h, 2);
	je_prevmode = -1;
	return;
     }

   je_x = JE_BASE_X - (cos(je_angle * (M_PI / 180.0)) * je_radius);
   je_y = JE_BASE_Y - (sin(je_angle * (M_PI / 180.0)) * je_radius);
   DrawEwinShape(ew, movemode, je_x, je_y, ew->client.w, ew->client.h,
		 je_prevmode == movemode);

   je_prevx = je_x;
   je_prevy = je_y;
   je_prevmode = movemode;

   je_angle += (180.0 * 0.02);
   if (je_angle > 360.0)
      je_angle -= 360.0;

   je_radius -= 0.5;
   DoIn("JE_MOVE", JE_DELAY, je_moveend, movemode, NULL);
   return;
   foo = NULL;
}

static void
je_movectrl(int val, void *ptr)
{
   if (!ptr)
     {
	RemoveTimerEvent("JE_MOVE");
	DoIn("JE_MOVE", JE_DELAY, je_moveend, val, NULL);
     }
   else
     {
	RemoveTimerEvent("JE_MOVE");
	je_move(val, (void *)1);
	DoIn("JE_MOVE", JE_DELAY, je_move, val, NULL);
     }
}

static void
je_resizectrl(int val, void *ptr)
{
   if (!ptr)
      printf("leave resize\n");
   else
      printf("enter resize\n");
   return;
   val = 0;
}

*/

void
SettingsMoveResize(void)
{
   Dialog             *d;

   /* Dialog             *dexp; */
   DItem              *table, *di, *radio1, *radio2, *radio3;

   if ((d =
	FindItem("CONFIGURE_MOVERESIZE", 0, LIST_FINDBY_NAME,
		 LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_MOVERESIZE");

   tmp_move = Conf.movres.mode_move;
   tmp_resize = Conf.movres.mode_resize;
   tmp_geominfo = Conf.movres.mode_info;

   d = DialogCreate("CONFIGURE_MOVERESIZE");
   DialogSetTitle(d, _("Move & Resize Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/moveres.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Move & Resize\n"
				"Method Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Move Methods:"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Resize Methods:"));

   radio1 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Opaque"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 0);
   /* DialogItemRadioButtonSetEventFunc(di, je_movectrl); */

   radio2 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Opaque"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 0);
   /* DialogItemRadioButtonSetEventFunc(di, je_resizectrl); */

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Technical"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 1);
   /* DialogItemRadioButtonSetEventFunc(di, je_movectrl); */

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Technical"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 1);
   /* DialogItemRadioButtonSetEventFunc(di, je_resizectrl); */

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Box"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 2);
   /* DialogItemRadioButtonSetEventFunc(di, je_movectrl); */

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Box"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 2);
   /* DialogItemRadioButtonSetEventFunc(di, je_resizectrl); */

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Shaded"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 3);
   /* DialogItemRadioButtonSetEventFunc(di, je_movectrl); */

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Shaded"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 3);
   /* DialogItemRadioButtonSetEventFunc(di, je_resizectrl); */

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Semi-Solid"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 4);
   /* DialogItemRadioButtonSetEventFunc(di, je_movectrl); */

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Semi-Solid"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 4);
   DialogItemRadioButtonGroupSetValPtr(radio2, &tmp_resize);
   /* DialogItemRadioButtonSetEventFunc(di, je_resizectrl); */

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Translucent"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 5);
   DialogItemRadioButtonGroupSetValPtr(radio1, &tmp_move);
   /* DialogItemRadioButtonSetEventFunc(di, je_movectrl); */

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Move/Resize Geometry Info Postion:"));

   radio3 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Window Center (O/T/B Methods)"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Always Screen corner"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Don't show"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 0);
   DialogItemRadioButtonGroupSetValPtr(radio3, &tmp_geominfo);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureMoveResize, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureMoveResize, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureMoveResize, 1);
   DialogSetExitFunction(d, CB_ConfigureMoveResize, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureMoveResize, 0);
   ShowDialog(d);

   /*
    * dexp = DialogCreate("CONFIGURE_MOVERESIZE_EXAMPLE");
    * DialogSetTitle(dexp, _("Move & Resize Settings Example"));
    */

   /* Example dialog
    * table = DialogInitItem(dexp);
    * DialogItemTableSetOptions(table, 1, 0, 0, 0);
    * 
    * di = DialogAddItem(table, DITEM_TEXT);
    * DialogItemSetPadding(di, 2, 2, 2, 2);
    * DialogItemSetFill(di, 0, 0);
    * DialogItemSetAlign(di, 0, 512);
    * DialogItemTextSetText(di, _("Example"));
    * 
    * di = DialogAddItem(table, DITEM_SEPARATOR);
    * DialogItemSetColSpan(di, 1);
    * DialogItemSetPadding(di, 2, 2, 2, 2);
    * DialogItemSetFill(di, 1, 0);
    * DialogItemSeparatorSetOrientation(di, 0);
    */

/*
 * DialogAddButton(dexp, _("OK"), CB_ConfigureMoveResize, 1);
 */

   /* ShowDialog(dexp); */
}

static char         tmp_with_leader;
static char         tmp_switch_popup;
static char         tmp_manual_placement;
static char         tmp_manual_placement_mouse_pointer;
static char         tmp_place_ignore_struts;
static char         tmp_raise_fullscreen;

#ifdef HAS_XINERAMA
static char         tmp_extra_head;
#endif

static void
CB_ConfigurePlacement(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.focus.transientsfollowleader = tmp_with_leader;
	Conf.focus.switchfortransientmap = tmp_switch_popup;
	Conf.place.manual = tmp_manual_placement;
	Conf.place.manual_mouse_pointer = tmp_manual_placement_mouse_pointer;
	Conf.place.ignore_struts = tmp_place_ignore_struts;
	Conf.place.raise_fullscreen = tmp_raise_fullscreen;
#ifdef HAS_XINERAMA
	if (Mode.display.xinerama_active)
	   Conf.extra_head = tmp_extra_head;
#endif
     }
   autosave();
}

void
SettingsPlacement(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d =
	FindItem("CONFIGURE_PLACEMENT", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_PLACEMENT");

   tmp_with_leader = Conf.focus.transientsfollowleader;
   tmp_switch_popup = Conf.focus.switchfortransientmap;
   tmp_manual_placement = Conf.place.manual;
   tmp_manual_placement_mouse_pointer = Conf.place.manual_mouse_pointer;
   tmp_place_ignore_struts = Conf.place.ignore_struts;
   tmp_raise_fullscreen = Conf.place.raise_fullscreen;
#ifdef HAS_XINERAMA
   tmp_extra_head = Conf.extra_head;
#endif

   d = DialogCreate("CONFIGURE_PLACEMENT");
   DialogSetTitle(d, _("Window Placement Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/place.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Window Placement\n"
				"Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_
				("Dialog windows appear together with their owner"));
   DialogItemCheckButtonSetState(di, tmp_with_leader);
   DialogItemCheckButtonSetPtr(di, &tmp_with_leader);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_("Switch to desktop where dialog appears"));
   DialogItemCheckButtonSetState(di, tmp_switch_popup);
   DialogItemCheckButtonSetPtr(di, &tmp_switch_popup);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Place windows manually"));
   DialogItemCheckButtonSetState(di, tmp_manual_placement);
   DialogItemCheckButtonSetPtr(di, &tmp_manual_placement);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Place windows under mouse"));
   DialogItemCheckButtonSetState(di, tmp_manual_placement_mouse_pointer);
   DialogItemCheckButtonSetPtr(di, &tmp_manual_placement_mouse_pointer);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Ignore struts"));
   DialogItemCheckButtonSetState(di, tmp_place_ignore_struts);
   DialogItemCheckButtonSetPtr(di, &tmp_place_ignore_struts);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Raise fullscreen windows"));
   DialogItemCheckButtonSetState(di, tmp_raise_fullscreen);
   DialogItemCheckButtonSetPtr(di, &tmp_raise_fullscreen);

#ifdef HAS_XINERAMA
   if (Mode.display.xinerama_active)
     {
	di = DialogAddItem(table, DITEM_CHECKBUTTON);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetColSpan(di, 2);
	DialogItemCheckButtonSetText(di,
				     _
				     ("Place windows on another head when full"));
	DialogItemCheckButtonSetState(di, tmp_extra_head);
	DialogItemCheckButtonSetPtr(di, &tmp_extra_head);
     }
#endif

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigurePlacement, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigurePlacement, 0);
   DialogAddButton(d, _("Close"), CB_ConfigurePlacement, 1);
   DialogSetExitFunction(d, CB_ConfigurePlacement, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigurePlacement, 0);
   ShowDialog(d);
}

static char         tmp_autoraise;
static int          tmp_autoraisetime;

static void
CB_ConfigureAutoraise(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.autoraise.enable = tmp_autoraise;
	Conf.autoraise.delay = 10 * tmp_autoraisetime;
     }
   autosave();
}

void
SettingsAutoRaise(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d =
	FindItem("CONFIGURE_AUTORAISE", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_AUTORAISE");

   tmp_autoraise = Conf.autoraise.enable;
   tmp_autoraisetime = Conf.autoraise.delay / 10;

   d = DialogCreate("CONFIGURE_AUTORAISE");
   DialogSetTitle(d, _("Autoraise Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/raise.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Automatic Raising\n"
				"of Windows Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Raise Windows Automatically"));
   DialogItemCheckButtonSetState(di, tmp_autoraise);
   DialogItemCheckButtonSetPtr(di, &tmp_autoraise);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Autoraise delay:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 300);
   DialogItemSliderSetUnits(di, 10);
   DialogItemSliderSetJump(di, 25);
   DialogItemSliderSetVal(di, tmp_autoraisetime);
   DialogItemSliderSetValPtr(di, &tmp_autoraisetime);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureAutoraise, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureAutoraise, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureAutoraise, 1);
   DialogSetExitFunction(d, CB_ConfigureAutoraise, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureAutoraise, 0);
   ShowDialog(d);
}

static char         tmp_dialog_headers;
static void
CB_ConfigureMiscellaneous(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.dialogs.headers = tmp_dialog_headers;
     }
   autosave();
}

void
SettingsMiscellaneous(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d =
	FindItem("CONFIGURE_MISCELLANEOUS", 0, LIST_FINDBY_NAME,
		 LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_MISCELLANEOUS");

   tmp_dialog_headers = Conf.dialogs.headers;

   d = DialogCreate("CONFIGURE_MISCELLANEOUS");
   DialogSetTitle(d, _("Miscellaneous Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/miscellaneous.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Miscellaneous\n"
				"Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Enable Dialog Headers"));
   DialogItemCheckButtonSetState(di, tmp_dialog_headers);
   DialogItemCheckButtonSetPtr(di, &tmp_dialog_headers);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureMiscellaneous, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureMiscellaneous, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureMiscellaneous, 1);
   DialogSetExitFunction(d, CB_ConfigureMiscellaneous, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureMiscellaneous, 0);
   ShowDialog(d);
}

static char         tmp_saveunders;
static char         tmp_warpmenus;
static char         tmp_animated_menus;
static char         tmp_menusonscreen;
static char         tmp_map_slide;
static char         tmp_cleanup_slide;
static char         tmp_desktop_slide;
static char         tmp_animate_shading;
static int          tmp_slide_mode;
static int          tmp_map_slide_speed;
static int          tmp_cleanup_slide_speed;
static int          tmp_desktop_slide_speed;
static int          tmp_shade_speed;

static void
CB_ConfigureFX(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.save_under = tmp_saveunders;
	Conf.menus.warp = tmp_warpmenus;
	Conf.menus.slide = tmp_animated_menus;
	Conf.menus.onscreen = tmp_menusonscreen;
	Conf.mapslide = tmp_map_slide;
	Conf.cleanupslide = tmp_cleanup_slide;
	Conf.desks.slidein = tmp_desktop_slide;
	Conf.animate_shading = tmp_animate_shading;
	Conf.shadespeed = tmp_shade_speed;
	Conf.slidemode = tmp_slide_mode;
	Conf.slidespeedmap = tmp_map_slide_speed;
	Conf.slidespeedcleanup = tmp_cleanup_slide_speed;
	Conf.desks.slidespeed = tmp_desktop_slide_speed;
     }
   autosave();
}

void
SettingsSpecialFX(void)
{
   Dialog             *d;
   DItem              *table, *di, *radio;

   if ((d = FindItem("CONFIGURE_FX", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_FX");

   tmp_saveunders = Conf.save_under;
   tmp_warpmenus = Conf.menus.warp;
   tmp_animated_menus = Conf.menus.slide;
   tmp_menusonscreen = Conf.menus.onscreen;
   tmp_map_slide = Conf.mapslide;
   tmp_cleanup_slide = Conf.cleanupslide;
   tmp_desktop_slide = Conf.desks.slidein;
   tmp_animate_shading = Conf.animate_shading;
   tmp_slide_mode = Conf.slidemode;
   tmp_map_slide_speed = Conf.slidespeedmap;
   tmp_shade_speed = Conf.shadespeed;
   tmp_cleanup_slide_speed = Conf.slidespeedcleanup;
   tmp_desktop_slide_speed = Conf.desks.slidespeed;

   d = DialogCreate("CONFIGURE_FX");
   DialogSetTitle(d, _("Special FX Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 4, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/fx.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Special Effects\n"
				"Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_NONE);

	di = DialogAddItem(table, DITEM_NONE);

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 4);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemTextSetText(di, _("Slide Method:"));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 3);
   DialogItemCheckButtonSetText(di, _("Slide desktops around when changing"));
   DialogItemCheckButtonSetState(di, tmp_desktop_slide);
   DialogItemCheckButtonSetPtr(di, &tmp_desktop_slide);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Opaque"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 3);
   DialogItemCheckButtonSetText(di, _("Slide windows around when cleaning up"));
   DialogItemCheckButtonSetState(di, tmp_cleanup_slide);
   DialogItemCheckButtonSetPtr(di, &tmp_cleanup_slide);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Technical"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 3);
   DialogItemCheckButtonSetText(di, _("Slide windows in when they appear"));
   DialogItemCheckButtonSetState(di, tmp_map_slide);
   DialogItemCheckButtonSetPtr(di, &tmp_map_slide);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Box"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 3);
   DialogItemCheckButtonSetText(di, _("Animated display of menus"));
   DialogItemCheckButtonSetState(di, tmp_animated_menus);
   DialogItemCheckButtonSetPtr(di, &tmp_animated_menus);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Shaded"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 3);
   DialogItemCheckButtonSetText(di, _("Always pop up menus on screen"));
   DialogItemCheckButtonSetState(di, tmp_menusonscreen);
   DialogItemCheckButtonSetPtr(di, &tmp_menusonscreen);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Semi-Solid"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 4);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_slide_mode);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 4);
   DialogItemCheckButtonSetText(di, _("Warp pointer after moving menus"));
   DialogItemCheckButtonSetState(di, tmp_warpmenus);
   DialogItemCheckButtonSetPtr(di, &tmp_warpmenus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 4);
   DialogItemCheckButtonSetText(di,
				_("Use saveunders to reduce window exposures"));
   DialogItemCheckButtonSetState(di, tmp_saveunders);
   DialogItemCheckButtonSetPtr(di, &tmp_saveunders);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 4);
   DialogItemCheckButtonSetText(di,
				_("Animate shading and unshading of windows"));
   DialogItemCheckButtonSetState(di, tmp_animate_shading);
   DialogItemCheckButtonSetPtr(di, &tmp_animate_shading);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 4);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, _("Desktop Slide speed: (slow)\n"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetVal(di, tmp_desktop_slide_speed);
   DialogItemSliderSetValPtr(di, &tmp_desktop_slide_speed);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("(fast)\n"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, _("Cleanup Slide speed: (slow)\n"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetVal(di, tmp_cleanup_slide_speed);
   DialogItemSliderSetValPtr(di, &tmp_cleanup_slide_speed);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("(fast)\n"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, _("Appear Slide speed: (slow)\n"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetVal(di, tmp_map_slide_speed);
   DialogItemSliderSetValPtr(di, &tmp_map_slide_speed);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("(fast)\n"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, _("Window Shading speed: (slow)\n"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetVal(di, tmp_shade_speed);
   DialogItemSliderSetValPtr(di, &tmp_shade_speed);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("(fast)\n"));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 4);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureFX, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureFX, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureFX, 1);
   DialogSetExitFunction(d, CB_ConfigureFX, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureFX, 0);

   ShowDialog(d);
}

/* list of remembered items for the remember dialog -- it's either
 * _another_ global var, or a wrapper struct to pass data to the 
 * callback funcs besides the dialog itself -- this is much easier */
static RememberWinList **rd_ewin_list;

void
RemoveRememberedWindow(EWin * ewin)
{
   RememberWinList    *rd;

   for (rd = rd_ewin_list[0]; rd; rd++)
      if (rd->ewin == ewin)
	{
	   rd->ewin = 0;
	   rd->remember = 0;
	   break;
	}

   return;
}

static void
CB_ApplyRemember(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   int                 i;

   if (val < 2 && rd_ewin_list)
     {
	for (i = 0; rd_ewin_list[i]; i++)
	  {
	     if (rd_ewin_list[i])
	       {
		  if (rd_ewin_list[i]->ewin && !rd_ewin_list[i]->remember)
		    {
		       UnsnapshotEwin(rd_ewin_list[i]->ewin);
		       /* would this be a better way to do things? */
		       /* sn = FindSnapshot(rd_ewin_list[i]->ewin); */
		       /* ClearSnapshot(sn); */
		       /* rd_ewin_list[i]->ewin->snap = 0; */
		    }
	       }
	  }
	/* save snapshot info to disk */
	SaveSnapInfo();
     }
   if (((val == 0) || (val == 2)) && rd_ewin_list)
     {
	for (i = 0; rd_ewin_list[i]; i++)
	   Efree(rd_ewin_list[i]);
	Efree(rd_ewin_list);
	rd_ewin_list = 0;
     }
}

static void
CB_ApplyRememberEscape(Dialog * d, int val, void *data __UNUSED__)
{
   int                 i;

   DialogClose(d);
   val = 0;
   if (rd_ewin_list)
     {
	for (i = 0; rd_ewin_list[i]; i++)
	   Efree(rd_ewin_list[i]);
	Efree(rd_ewin_list);
	rd_ewin_list = 0;
     }
}

static void
CB_RememberWindowSettings(Dialog * d __UNUSED__, int val, void *data)
{
   RememberWinList    *rd;
   EWin               *ewin;

   val = 0;
   if (!data)
      return;
   rd = (RememberWinList *) data;
   ewin = (EWin *) rd->ewin;
   if (!ewin)
      return;
   SnapshotEwinDialog(ewin);
}

void
SettingsRemember()
{
   Dialog             *d;
   DItem              *table, *di;
   EWin               *const *lst, *ewin;
   int                 i, ri, num;

   /* init remember window */
   if ((d = FindItem("REMEMBER_WINDOW", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_REMEMBER");

   d = DialogCreate("REMEMBER_WINDOW");
   DialogSetTitle(d, _("Remembered Windows Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 3, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/snapshots.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Remembered\n"
				"Windows Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   /* there's a much more efficient way of doing this, but this will work
    * for now */
   lst = EwinListGetAll(&num);
   rd_ewin_list = Emalloc(sizeof(RememberWinList *) * (num + 1));
   ri = 0;
   if ((lst) && (num > 0))
     {
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     if (!ewin || !FindSnapshot(ewin)
		 || !(ewin->icccm.wm_name || ewin->icccm.wm_res_name
		      || ewin->icccm.wm_res_class))
	       {
		  /* fprintf(stderr,"Skipping window #%d \"%s\".\n",
		   * i, ewin->icccm.wm_name?ewin->icccm.wm_name:"null"); */
	       }
	     else
	       {
		  rd_ewin_list[ri] = Emalloc(sizeof(RememberWinList));
		  rd_ewin_list[ri]->ewin = ewin;
		  rd_ewin_list[ri]->remember = 1;
		  /* fprintf(stderr," Window #%d \"%s\" is remembered (ri==%d)\n",
		   * i, ewin->icccm.wm_name?ewin->icccm.wm_name:"null", ri);
		   * fprintf(stderr,"  title:\t%s\n  name:\t%s\n  class:\t%s\n  command:\t%s\n",
		   * ewin->icccm.wm_name?ewin->icccm.wm_name:"null",
		   * ewin->icccm.wm_res_name?ewin->icccm.wm_res_name:"null",
		   * ewin->icccm.wm_res_class?ewin->icccm.wm_res_class:"null",
		   * ewin->icccm.wm_command?ewin->icccm.wm_command:"null"
		   * ); */

		  di = DialogAddItem(table, DITEM_CHECKBUTTON);
		  DialogItemSetColSpan(di, 2);
		  DialogItemSetPadding(di, 2, 2, 2, 2);
		  DialogItemSetFill(di, 1, 0);
		  DialogItemSetAlign(di, 0, 512);
		  DialogItemCheckButtonSetText(di, ewin->icccm.wm_name);
		  DialogItemCheckButtonSetState(di, rd_ewin_list[ri]->remember);
		  DialogItemCheckButtonSetPtr(di,
					      &(rd_ewin_list[ri]->remember));

		  di = DialogAddItem(table, DITEM_BUTTON);
		  DialogItemSetPadding(di, 2, 2, 2, 2);
		  DialogItemSetFill(di, 1, 0);
		  DialogItemSetAlign(di, 1024, 512);
		  DialogItemButtonSetText(di, _("Remembered Settings..."));
		  DialogItemSetCallback(di, CB_RememberWindowSettings, 0,
					(char *)rd_ewin_list[ri]);

		  ri++;
	       }
	  }
     }
   rd_ewin_list[ri] = 0;

   /* finish remember window */
   if (!ri)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _
			      ("There are no active windows with remembered attributes."));
     }

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ApplyRemember, 1);
   DialogAddButton(d, _("Apply"), CB_ApplyRemember, 0);
   DialogAddButton(d, _("Close"), CB_ApplyRemember, 1);
   DialogSetExitFunction(d, CB_ApplyRemember, 2);
   DialogBindKey(d, "Escape", CB_ApplyRememberEscape, 0);
   DialogBindKey(d, "Return", CB_ApplyRemember, 0);

   ShowDialog(d);
}

#if USE_COMPOSITE
#include "ecompmgr.h"

/*
 * Composite dialog
 */

static void
CB_ConfigureComposite(Dialog * d, int val, void *data __UNUSED__)
{
   cfg_composite      *cfg;

   if (val >= 2)
      return;

   /* Configure and read back */
   cfg = DialogGetData(d);
   ECompMgrConfigSet(cfg);
   ECompMgrConfigGet(cfg);

   autosave();
}

void
SettingsComposite(void)
{
   static cfg_composite Cfg_composite;
   Dialog             *d;
   DItem              *table, *di, *radio;

   if ((d =
	FindItem("CONFIGURE_COMPOSITE", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_COMPOSITE");

   d = DialogCreate("CONFIGURE_COMPOSITE");
   DialogSetTitle(d, _("Composite Settings"));

   /* Get current settings */
   ECompMgrConfigGet(&Cfg_composite);
   DialogSetData(d, &Cfg_composite);

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
#if 0
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/tips.png");
#endif
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetColSpan(di, 2);
	DialogItemTextSetText(di,
			      _("Enlightenment Composite\n"
				"Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Enable Composite"));
   DialogItemCheckButtonSetState(di, Cfg_composite.enable);
   DialogItemCheckButtonSetPtr(di, &Cfg_composite.enable);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemRadioButtonSetText(di, _("Shadows Off"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemRadioButtonSetText(di, _("Shadows Sharp"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemRadioButtonSetText(di, _("Shadows Blurred"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);
   DialogItemRadioButtonGroupSetValPtr(radio, &Cfg_composite.shadow);

#if 0
   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Fading Speed:\n"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 300);
   DialogItemSliderSetUnits(di, 10);
   DialogItemSliderSetJump(di, 25);
   DialogItemSliderSetVal(di, tmp_tooltiptime);
   DialogItemSliderSetValPtr(di, &tmp_tooltiptime);
#endif

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureComposite, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureComposite, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureComposite, 1);
   DialogSetExitFunction(d, CB_ConfigureComposite, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureComposite, 0);
   ShowDialog(d);
}
#endif
