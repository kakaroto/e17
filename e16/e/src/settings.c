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
#define DECLARE_STRUCT_ICONBOX
#include "E.h"
#include <time.h>

typedef struct _remwinlist
{
   EWin               *ewin;
   char                remember;
}
RememberWinList;

static void
CB_SettingsEscape(int val, void *data)
{
   DialogClose((Dialog *) data);
   val = 0;
}

/* Pager settings */
static char         tmp_show_pagers;
static char         tmp_pager_hiq;
static char         tmp_pager_snap;
static char         tmp_pager_zoom;
static char         tmp_pager_title;
static char         tmp_pager_do_scan;
static int          tmp_pager_scan_speed;
static int          tmp_pager_sel_button;
static int          tmp_pager_win_button;
static int          tmp_pager_menu_button;
static DItem       *pager_scan_speed_label = NULL;
static Dialog      *pager_settings_dialog = NULL;

static void
CB_ConfigurePager(int val, void *data)
{
   if (val < 2)
     {
	if ((!Conf.pagers.enable) && (tmp_show_pagers))
	   EnableAllPagers();
	else if ((Conf.pagers.enable) && (!tmp_show_pagers))
	   DisableAllPagers();
	if (Conf.pagers.hiq != tmp_pager_hiq)
	   PagerSetHiQ(tmp_pager_hiq);
	Conf.pagers.zoom = tmp_pager_zoom;
	Conf.pagers.title = tmp_pager_title;
	Conf.pagers.sel_button = tmp_pager_sel_button;
	Conf.pagers.win_button = tmp_pager_win_button;
	Conf.pagers.menu_button = tmp_pager_menu_button;
	if ((Conf.pagers.scanspeed != tmp_pager_scan_speed)
	    || ((!tmp_pager_do_scan) && (Conf.pagers.scanspeed > 0))
	    || ((tmp_pager_do_scan) && (Conf.pagers.scanspeed == 0)))
	  {
	     if (tmp_pager_do_scan)
		Conf.pagers.scanspeed = tmp_pager_scan_speed;
	     else
		Conf.pagers.scanspeed = 0;
	     PagerSetSnap(tmp_pager_snap);
	  }
	if (Conf.pagers.snap != tmp_pager_snap)
	   PagerSetSnap(tmp_pager_snap);
     }
   autosave();
   data = NULL;
}

static void
CB_PagerScanSlide(int val, void *data)
{
   char                s[256];

   Esnprintf(s, sizeof(s), "%s %03i %s", _("Pager scanning speed:"),
	     tmp_pager_scan_speed, _("lines per second"));
   DialogItemTextSetText(pager_scan_speed_label, s);
   DialogDrawItems(pager_settings_dialog, pager_scan_speed_label, 0, 0, 99999,
		   99999);
   val = 0;
   data = NULL;
}

void
SettingsPager(void)
{
   Dialog             *d;
   DItem              *table, *di, *radio;
   char                s[256];

   if ((d = FindItem("CONFIGURE_PAGER", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_PAGER");

   tmp_show_pagers = Conf.pagers.enable;
   tmp_pager_hiq = Conf.pagers.hiq;
   tmp_pager_snap = Conf.pagers.snap;
   tmp_pager_zoom = Conf.pagers.zoom;
   tmp_pager_title = Conf.pagers.title;
   tmp_pager_sel_button = Conf.pagers.sel_button;
   tmp_pager_win_button = Conf.pagers.win_button;
   tmp_pager_menu_button = Conf.pagers.menu_button;
   if (Conf.pagers.scanspeed == 0)
      tmp_pager_do_scan = 0;
   else
      tmp_pager_do_scan = 1;
   tmp_pager_scan_speed = Conf.pagers.scanspeed;

   d = pager_settings_dialog = DialogCreate("CONFIGURE_PAGER");
   DialogSetTitle(d, _("Pager Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/pager.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Desktop & Area\n"
				"Pager Settings Dialog\n"));

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
   DialogItemCheckButtonSetText(di, _("Enable pager display"));
   DialogItemCheckButtonSetState(di, tmp_show_pagers);
   DialogItemCheckButtonSetPtr(di, &tmp_show_pagers);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_("Make miniature snapshots of the screen"));
   DialogItemCheckButtonSetState(di, tmp_pager_snap);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_snap);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_
				("Smooth high quality snapshots in snapshot mode"));
   DialogItemCheckButtonSetState(di, tmp_pager_hiq);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_hiq);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_
				("Zoom in on pager windows when mouse is over them"));
   DialogItemCheckButtonSetState(di, tmp_pager_zoom);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_zoom);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_
				("Pop up window title when mouse is over the window"));
   DialogItemCheckButtonSetState(di, tmp_pager_title);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_title);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_("Continuously scan screen to update pager"));
   DialogItemCheckButtonSetState(di, tmp_pager_do_scan);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_do_scan);

   di = pager_scan_speed_label = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   Esnprintf(s, sizeof(s), "%s %03i %s", _("Pager scanning speed:"),
	     tmp_pager_scan_speed, _("lines per second"));
   DialogItemTextSetText(di, s);

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 1, 256);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSetColSpan(di, 2);
   DialogItemSliderSetVal(di, tmp_pager_scan_speed);
   DialogItemSliderSetValPtr(di, &tmp_pager_scan_speed);
   DialogItemSetCallback(di, CB_PagerScanSlide, 0, NULL);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 0);
   DialogItemTextSetText(di, _("Mouse button to select and drag windows:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Left"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Middle"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Right"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_pager_win_button);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 0);
   DialogItemTextSetText(di, _("Mouse button to select desktops:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Left"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Middle"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Right"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_pager_sel_button);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 0);
   DialogItemTextSetText(di, _("Mouse button to display pager menu:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Left"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Middle"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Right"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_pager_menu_button);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigurePager, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigurePager, 0);
   DialogAddButton(d, _("Close"), CB_ConfigurePager, 1);
   DialogSetExitFunction(d, CB_ConfigurePager, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigurePager, 0, d);
   ShowDialog(d);
}

static int          tmp_focus;
static char         tmp_new_focus;
static char         tmp_popup_focus;
static char         tmp_owner_popup_focus;
static char         tmp_raise_focus;
static char         tmp_warp_focus;

#ifdef WITH_TARTY_WARP
static char         tmp_warp_after_focus;
static char         tmp_raise_after_focus;
static char         tmp_display_warp;
static char         tmp_warpsticky;
static char         tmp_warpshaded;
static char         tmp_warpiconified;
static char         tmp_warpfocused;
#endif
static char         tmp_clickalways;

static void
CB_ConfigureFocus(int val, void *data)
{
   if (val < 2)
     {
	Conf.focus.mode = tmp_focus;
	Conf.focus.all_new_windows_get_focus = tmp_new_focus;
	Conf.focus.new_transients_get_focus = tmp_popup_focus;
	Conf.focus.new_transients_get_focus_if_group_focused =
	   tmp_owner_popup_focus;
	Conf.focus.raise_on_next_focus = tmp_raise_focus;
	Conf.focus.warp_on_next_focus = tmp_warp_focus;
#ifdef WITH_TARTY_WARP
	Conf.focus.warp_after_next_focus = tmp_warp_after_focus;
	Conf.focus.raise_after_next_focus = tmp_raise_after_focus;
	Conf.warplist.enable = tmp_display_warp;
	Conf.warplist.warpsticky = tmp_warpsticky;
	Conf.warplist.warpshaded = tmp_warpshaded;
	Conf.warplist.warpiconified = tmp_warpiconified;
	Conf.warplist.warpfocused = tmp_warpfocused;
#endif /* WITH_TARTY_WARP */
	Conf.focus.clickraises = tmp_clickalways;
	FocusFix();
     }
   autosave();
   data = NULL;
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
   tmp_raise_focus = Conf.focus.raise_on_next_focus;
   tmp_warp_focus = Conf.focus.warp_on_next_focus;
#ifdef WITH_TARTY_WARP
   tmp_raise_after_focus = Conf.focus.raise_after_next_focus;
   tmp_warp_after_focus = Conf.focus.warp_after_next_focus;
   tmp_display_warp = Conf.warplist.enable;
   tmp_warpsticky = Conf.warplist.warpsticky;
   tmp_warpshaded = Conf.warplist.warpshaded;
   tmp_warpiconified = Conf.warplist.warpiconified;
   tmp_warpfocused = Conf.warplist.warpfocused;
#endif /* WITH_TARTY_WARP */
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

#ifdef WITH_TARTY_WARP
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
   DialogItemCheckButtonSetState(di, tmp_warpsticky);
   DialogItemCheckButtonSetPtr(di, &tmp_warpsticky);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Include shaded windows in focus list"));
   DialogItemCheckButtonSetState(di, tmp_warpshaded);
   DialogItemCheckButtonSetPtr(di, &tmp_warpshaded);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_("Include iconified windows in focus list"));
   DialogItemCheckButtonSetState(di, tmp_warpiconified);
   DialogItemCheckButtonSetPtr(di, &tmp_warpiconified);

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
#endif /* WITH_TARTY_WARP */

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureFocus, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureFocus, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureFocus, 1);
   DialogSetExitFunction(d, CB_ConfigureFocus, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureFocus, 0, d);
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
CB_ConfigureMoveResize(int val, void *data)
{
   Dialog             *d;

   if (val < 2)
     {
	Conf.movemode = tmp_move;
	Conf.resizemode = tmp_resize;
	Conf.geominfomode = tmp_geominfo;
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
   data = NULL;
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

   tmp_move = Conf.movemode;
   tmp_resize = Conf.resizemode;
   tmp_geominfo = Conf.geominfomode;

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
   DialogSetExitFunction(d, CB_ConfigureMoveResize, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureMoveResize, 0, d);
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

static int          tmp_desktops;
static DItem       *tmp_desk_text;
static Dialog      *tmp_desk_dialog;
static char         tmp_desktop_wraparound;

static void
CB_ConfigureDesktops(int val, void *data)
{
   if (val < 2)
     {
	ChangeNumberOfDesktops(tmp_desktops);
	Conf.desks.wraparound = tmp_desktop_wraparound;
     }
   autosave();
   data = NULL;
}

static void
CB_DesktopDisplayRedraw(int val, void *data)
{
   static char         called = 0;
   DItem              *di;
   static Window       win, wins[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
   int                 i;
   int                 w, h;
   static int          prev_desktops = -1;
   char                s[64];

   if (val == 1)
      called = 0;

   if ((val != 1) && (prev_desktops == tmp_desktops))
      return;
   prev_desktops = tmp_desktops;
   di = (DItem *) data;
   win = DialogItemAreaGetWindow(di);
   DialogItemAreaGetSize(di, &w, &h);
   if (!called)
     {
	ImageClass         *ic;

	ic = FindItem("SETTINGS_DESKTOP_AREA", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	   IclassApply(ic, win, w, h, 0, 0, STATE_NORMAL, 0);
	for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
	   wins[i] = 0;
	called = 1;
     }
   for (i = 0; i < tmp_desktops; i++)
     {
	if (!wins[i])
	  {
	     wins[i] = ECreateWindow(win, 0, 0, 64, 48, 0);
	     XSetWindowBorderWidth(disp, wins[i], 1);
	     if (desks.desk[i].bg)
	       {
		  Pixmap              pmap;

		  pmap = ECreatePixmap(disp, wins[i], 64, 48, root.depth);
		  ESetWindowBackgroundPixmap(disp, wins[i], pmap);
		  SetBackgroundTo(pmap, desks.desk[i].bg, 0);
		  EFreePixmap(disp, pmap);
	       }
	  }
     }
   for (i = (tmp_desktops - 1); i >= 0; i--)
     {
	int                 num;

	num = tmp_desktops - 1;
	if (num < 1)
	   num = 1;
	XRaiseWindow(disp, wins[i]);
	EMoveWindow(disp, wins[i], (i * (w - 64 - 2)) / num,
		    (i * (h - 48 - 2)) / num);
	EMapWindow(disp, wins[i]);
     }
   for (i = tmp_desktops; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
      EUnmapWindow(disp, wins[i]);
   if (tmp_desktops > 1)
      Esnprintf(s, sizeof(s), _("%i Desktops"), tmp_desktops);
   else
      Esnprintf(s, sizeof(s), _("%i Desktop"), tmp_desktops);
   DialogItemTextSetText(tmp_desk_text, s);
   DialogDrawItems(tmp_desk_dialog, tmp_desk_text, 0, 0, 99999, 99999);
}

void
SettingsDesktops(void)
{
   Dialog             *d;
   DItem              *table, *di, *area, *slider;
   char                s[64];

   if ((d =
	FindItem("CONFIGURE_DESKTOPS", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_DESKTOPS");

   tmp_desktops = Conf.desks.num;
   tmp_desktop_wraparound = Conf.desks.wraparound;

   d = tmp_desk_dialog = DialogCreate("CONFIGURE_DESKTOPS");
   DialogSetTitle(d, _("Multiple Desktop Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/desktops.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Multiple Desktop\n"
				"Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemTextSetText(di, _("Number of virtual desktops:\n"));

   di = tmp_desk_text = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   if (tmp_desktops > 1)
      Esnprintf(s, sizeof(s), _("%i Desktops"), tmp_desktops);
   else
      Esnprintf(s, sizeof(s), _("%i Desktop"), tmp_desktops);
   DialogItemTextSetText(di, s);

   di = slider = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 1, 32);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSetColSpan(di, 2);
   DialogItemSliderSetVal(di, tmp_desktops);
   DialogItemSliderSetValPtr(di, &tmp_desktops);

   di = area = DialogAddItem(table, DITEM_AREA);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemAreaSetSize(di, 128, 96);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Wrap desktops around"));
   DialogItemCheckButtonSetState(di, tmp_desktop_wraparound);
   DialogItemCheckButtonSetPtr(di, &tmp_desktop_wraparound);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureDesktops, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureDesktops, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureDesktops, 1);
   DialogSetExitFunction(d, CB_ConfigureDesktops, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureDesktops, 0, d);
   ShowDialog(d);
   DialogItemSetCallback(slider, CB_DesktopDisplayRedraw, 0, (void *)area);
   CB_DesktopDisplayRedraw(1, area);
}

static int          tmp_area_x;
static int          tmp_area_y;
static int          tmp_edge_resist;
static char         tmp_edge_flip;
static DItem       *tmp_area_text;
static Dialog      *tmp_area_dialog;
static char         tmp_area_wraparound;

static void
CB_ConfigureAreas(int val, void *data)
{
   if (val < 2)
     {
	SetNewAreaSize(tmp_area_x, 9 - tmp_area_y);
	Conf.areas.wraparound = tmp_area_wraparound;
	if (tmp_edge_flip)
	  {
	     if (tmp_edge_resist < 1)
		tmp_edge_resist = 1;
	     Conf.edge_flip_resistance = tmp_edge_resist;
	  }
	else
	   Conf.edge_flip_resistance = 0;
	ShowEdgeWindows();
     }
   autosave();
   data = NULL;
}

static void
CB_AreaDisplayRedraw(int val, void *data)
{
   char                s[64];
   static char         called = 0;
   DItem              *di;
   static Window       win, awin;
   int                 w, h;
   static int          prev_ax = 0, prev_ay = 0;

   if (val == 1)
      called = 0;

   if ((val != 1) && ((prev_ax == tmp_area_x) && (prev_ay == tmp_area_y)))
      return;
   prev_ax = tmp_area_x;
   prev_ay = tmp_area_y;
   di = (DItem *) data;
   win = DialogItemAreaGetWindow(di);
   DialogItemAreaGetSize(di, &w, &h);
   if (!called)
     {
	ImageClass         *ic;
	PmapMask            pmm;

	ic = FindItem("SETTINGS_AREA_AREA", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	   IclassApply(ic, win, w, h, 0, 0, STATE_NORMAL, 0);
	awin = ECreateWindow(win, 0, 0, 18, 14, 0);
	ic = FindItem("SETTINGS_AREADESK_AREA", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	  {
	     IclassApplyCopy(ic, awin, 18, 14, 0, 0, STATE_NORMAL, &pmm, 0);
	     ESetWindowBackgroundPixmap(disp, awin, pmm.pmap);
	     FreePmapMask(&pmm);
	  }
	XClearWindow(disp, awin);
	called = 1;
     }
   EMoveResizeWindow(disp, awin, ((w / 2) - (9 * tmp_area_x)),
		     ((h / 2) - (7 * (9 - tmp_area_y))), 18 * tmp_area_x,
		     14 * (9 - tmp_area_y));
   EMapWindow(disp, awin);

   if ((tmp_area_x > 1) || ((9 - tmp_area_y) > 1))
      Esnprintf(s, sizeof(s), _("%i x %i\nScreens in size"), tmp_area_x,
		9 - tmp_area_y);
   else
      Esnprintf(s, sizeof(s), _("1\nScreen in size"));
   DialogItemTextSetText(tmp_area_text, s);
   DialogDrawItems(tmp_area_dialog, tmp_area_text, 0, 0, 99999, 99999);
}

void
SettingsArea(void)
{
   Dialog             *d;
   DItem              *table, *di, *area, *slider, *slider2, *table2;
   char                s[64];

   if ((d = FindItem("CONFIGURE_AREA", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_AREA");

   tmp_area_wraparound = Conf.areas.wraparound;
   tmp_edge_resist = Conf.edge_flip_resistance;
   if (tmp_edge_resist == 0)
      tmp_edge_flip = 0;
   else
      tmp_edge_flip = 1;
   GetAreaSize(&tmp_area_x, &tmp_area_y);
   tmp_area_y = 9 - tmp_area_y;

   d = tmp_area_dialog = DialogCreate("CONFIGURE_AREA");
   DialogSetTitle(d, _("Virtual Desktop Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 1, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	table2 = DialogAddItem(table, DITEM_TABLE);
	DialogItemTableSetOptions(table2, 2, 0, 0, 0);

	di = DialogAddItem(table2, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/areas.png");

	di = DialogAddItem(table2, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Virtual Desktop\n"
				"Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemTextSetText(di, _("Virtual Desktop size:\n"));

   di = tmp_area_text = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   if ((tmp_area_x > 1) || (tmp_area_y > 1))
      Esnprintf(s, sizeof(s), _("%i x %i\nScreens in size"), tmp_area_x,
		9 - tmp_area_y);
   else
      Esnprintf(s, sizeof(s), _("1\nScreen in size"));
   DialogItemTextSetText(di, s);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemTableSetOptions(table2, 2, 0, 0, 0);

   di = DialogAddItem(table2, DITEM_NONE);

   di = slider = DialogAddItem(table2, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 1, 8);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSliderSetVal(di, tmp_area_x);
   DialogItemSliderSetValPtr(di, &tmp_area_x);

   di = slider2 = DialogAddItem(table2, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetOrientation(di, 0);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 1);
   DialogItemSliderSetBounds(di, 1, 8);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSliderSetVal(di, tmp_area_y);
   DialogItemSliderSetValPtr(di, &tmp_area_y);

   di = area = DialogAddItem(table2, DITEM_AREA);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemAreaSetSize(di, 160, 120);

   DialogItemSetCallback(slider, CB_AreaDisplayRedraw, 0, (void *)area);
   DialogItemSetCallback(slider2, CB_AreaDisplayRedraw, 0, (void *)area);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Wrap virtual desktops around"));
   DialogItemCheckButtonSetState(di, tmp_area_wraparound);
   DialogItemCheckButtonSetPtr(di, &tmp_area_wraparound);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Enable edge flip"));
   DialogItemCheckButtonSetState(di, tmp_edge_flip);
   DialogItemCheckButtonSetPtr(di, &tmp_edge_flip);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemTextSetText(di, _("Resistance at edge of screen:\n"));

   di = slider = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 1, 100);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 10);
   DialogItemSliderSetVal(di, tmp_edge_resist);
   DialogItemSliderSetValPtr(di, &tmp_edge_resist);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureAreas, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureAreas, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureAreas, 1);
   DialogSetExitFunction(d, CB_ConfigureAreas, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureAreas, 0, d);
   ShowDialog(d);
   CB_AreaDisplayRedraw(1, area);
}

static char         tmp_with_leader;
static char         tmp_switch_popup;
static char         tmp_manual_placement;
static char         tmp_manual_placement_mouse_pointer;

#ifdef HAS_XINERAMA
static char         tmp_extra_head;
#endif

static void
CB_ConfigurePlacement(int val, void *data)
{
   if (val < 2)
     {
	Conf.focus.transientsfollowleader = tmp_with_leader;
	Conf.focus.switchfortransientmap = tmp_switch_popup;
	Conf.manual_placement = tmp_manual_placement;
	Conf.manual_placement_mouse_pointer =
	   tmp_manual_placement_mouse_pointer;
#ifdef HAS_XINERAMA
	if (xinerama_active)
	   Conf.extra_head = tmp_extra_head;
#endif
     }
   autosave();
   data = NULL;
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
   tmp_manual_placement = Conf.manual_placement;
   tmp_manual_placement_mouse_pointer = Conf.manual_placement_mouse_pointer;
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

#ifdef HAS_XINERAMA
   if (xinerama_active)
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
   DialogSetExitFunction(d, CB_ConfigurePlacement, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigurePlacement, 0, d);
   ShowDialog(d);
}

void
SettingsIcons(void)
{
}

static char         tmp_autoraise;
static int          tmp_autoraisetime;

static void
CB_ConfigureAutoraise(int val, void *data)
{
   if (val < 2)
     {
	Conf.autoraise.enable = tmp_autoraise;
	Conf.autoraise.delay = ((double)tmp_autoraisetime) / 100;
     }
   autosave();
   data = NULL;
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
   tmp_autoraisetime = (int)(Conf.autoraise.delay * 100);

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
   DialogSetExitFunction(d, CB_ConfigureAutoraise, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureAutoraise, 0, d);
   ShowDialog(d);
}

static char         tmp_tooltips;
static int          tmp_tooltiptime;
static char         tmp_roottip;

static void
CB_ConfigureTooltips(int val, void *data)
{
   if (val < 2)
     {
	Conf.tooltips.enable = tmp_tooltips;
	Conf.tooltips.delay = ((double)tmp_tooltiptime) / 100;
	Conf.tooltips.showroottooltip = tmp_roottip;
     }
   autosave();
   data = NULL;
}

void
SettingsTooltips(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d =
	FindItem("CONFIGURE_TOOLTIPS", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_TOOLTIPS");

   tmp_tooltips = Conf.tooltips.enable;
   tmp_tooltiptime = (int)(Conf.tooltips.delay * 100);
   tmp_roottip = Conf.tooltips.showroottooltip;

   d = DialogCreate("CONFIGURE_TOOLTIPS");
   DialogSetTitle(d, _("Tooltip Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/tips.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Tooltip\n" "Settings Dialog\n"));

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
   DialogItemCheckButtonSetText(di, _("Display Tooltips"));
   DialogItemCheckButtonSetState(di, tmp_tooltips);
   DialogItemCheckButtonSetPtr(di, &tmp_tooltips);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Display Root Window Tips"));
   DialogItemCheckButtonSetState(di, tmp_roottip);
   DialogItemCheckButtonSetPtr(di, &tmp_roottip);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Tooltip Delay:\n"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 300);
   DialogItemSliderSetUnits(di, 10);
   DialogItemSliderSetJump(di, 25);
   DialogItemSliderSetVal(di, tmp_tooltiptime);
   DialogItemSliderSetValPtr(di, &tmp_tooltiptime);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureTooltips, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureTooltips, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureTooltips, 1);
   DialogSetExitFunction(d, CB_ConfigureTooltips, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureTooltips, 0, d);
   ShowDialog(d);
}

static char         tmp_dialog_headers;
static void
CB_ConfigureMiscellaneous(int val, void *data)
{
   if (val < 2)
     {
	Conf.dialogs.headers = tmp_dialog_headers;
     }
   autosave();
   data = NULL;
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
   DialogSetExitFunction(d, CB_ConfigureMiscellaneous, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureMiscellaneous, 0, d);
   ShowDialog(d);
}

static char         tmp_audio;

static void
CB_ConfigureAudio(int val, void *data)
{
   if (val < 2)
     {
	Conf.sound = tmp_audio;
	if (Conf.sound)
	   SoundInit();
	else
	   SoundExit();
     }
   autosave();
   data = NULL;
}

void
SettingsAudio(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d = FindItem("CONFIGURE_AUDIO", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_AUDIO");

   tmp_audio = Conf.sound;

   d = DialogCreate("CONFIGURE_AUDIO");
   DialogSetTitle(d, _("Audio Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/sound.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Audio\n" "Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }
#ifdef HAVE_LIBESD
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Enable sounds"));
   DialogItemCheckButtonSetState(di, tmp_audio);
   DialogItemCheckButtonSetPtr(di, &tmp_audio);
#else
   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetColSpan(di, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemTextSetText(di,
			 _("Audio not available since EsounD was not\n"
			   "present at the time of compilation."));
#endif

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureAudio, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureAudio, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureAudio, 1);
   DialogSetExitFunction(d, CB_ConfigureAudio, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureAudio, 0, d);
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
static char         tmp_dragbar;
static int          tmp_dragdir;
static int          tmp_slide_mode;
static int          tmp_map_slide_speed;
static int          tmp_cleanup_slide_speed;
static int          tmp_desktop_slide_speed;
static int          tmp_shade_speed;
static char         tmp_effect_raindrops;
static char         tmp_effect_ripples;
static char         tmp_effect_waves;

static void
CB_ConfigureFX(int val, void *data)
{
   if (val < 2)
     {
	Conf.save_under = tmp_saveunders;
	Conf.warpmenus = tmp_warpmenus;
	Conf.menuslide = tmp_animated_menus;
	Conf.menusonscreen = tmp_menusonscreen;
	Conf.mapslide = tmp_map_slide;
	Conf.cleanupslide = tmp_cleanup_slide;
	Conf.desks.slidein = tmp_desktop_slide;
	Conf.animate_shading = tmp_animate_shading;
	Conf.shadespeed = tmp_shade_speed;
	Conf.slidemode = tmp_slide_mode;
	Conf.slidespeedmap = tmp_map_slide_speed;
	Conf.slidespeedcleanup = tmp_cleanup_slide_speed;
	Conf.desks.slidespeed = tmp_desktop_slide_speed;

	FX_Op("raindrops", tmp_effect_raindrops ? FX_OP_START : FX_OP_STOP);
	FX_Op("ripples", tmp_effect_ripples ? FX_OP_START : FX_OP_STOP);
	FX_Op("waves", tmp_effect_waves ? FX_OP_START : FX_OP_STOP);

	if ((Conf.desks.dragdir != tmp_dragdir) ||
	    ((tmp_dragbar) && (Conf.desks.dragbar_width < 1)) ||
	    ((!tmp_dragbar) && (Conf.desks.dragbar_width > 0)))
	  {
	     Button             *b;

	     if (tmp_dragbar)
		Conf.desks.dragbar_width = 16;
	     else
		Conf.desks.dragbar_width = -1;
	     Conf.desks.dragdir = tmp_dragdir;
	     while ((b =
		     RemoveItem("_DESKTOP_DRAG_CONTROL", 0, LIST_FINDBY_NAME,
				LIST_TYPE_BUTTON)))
		ButtonDestroy(b);
	     InitDesktopControls();
	     ShowDesktopControls();
	  }
     }
   autosave();
   data = NULL;
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
   tmp_warpmenus = Conf.warpmenus;
   tmp_animated_menus = Conf.menuslide;
   tmp_menusonscreen = Conf.menusonscreen;
   tmp_map_slide = Conf.mapslide;
   tmp_cleanup_slide = Conf.cleanupslide;
   tmp_desktop_slide = Conf.desks.slidein;
   tmp_animate_shading = Conf.animate_shading;
   if (Conf.desks.dragbar_width < 1)
      tmp_dragbar = 0;
   else
      tmp_dragbar = 1;
   tmp_dragdir = Conf.desks.dragdir;
   tmp_slide_mode = Conf.slidemode;
   tmp_map_slide_speed = Conf.slidespeedmap;
   tmp_shade_speed = Conf.shadespeed;
   tmp_cleanup_slide_speed = Conf.slidespeedcleanup;
   tmp_desktop_slide_speed = Conf.desks.slidespeed;

   tmp_effect_raindrops = FX_IsOn("raindrops");
   tmp_effect_ripples = FX_IsOn("ripples");
   tmp_effect_waves = FX_IsOn("waves");

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

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 4);
   DialogItemCheckButtonSetText(di, _("Display desktop dragbar"));
   DialogItemCheckButtonSetState(di, tmp_dragbar);
   DialogItemCheckButtonSetPtr(di, &tmp_dragbar);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 1);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Drag bar position:"));

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_NONE);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Top"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Bottom"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Left"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Right"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_dragdir);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_NONE);

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

   /* Effects */
   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 4);
   DialogItemTextSetText(di, _("Effects"));
#if 0				/* Disabled */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 4);
   DialogItemCheckButtonSetText(di, _("Enable Effect: Raindrops"));
   DialogItemCheckButtonSetState(di, tmp_effect_raindrops);
   DialogItemCheckButtonSetPtr(di, &tmp_effect_raindrops);
#endif
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Ripples"));
   DialogItemCheckButtonSetState(di, tmp_effect_ripples);
   DialogItemCheckButtonSetPtr(di, &tmp_effect_ripples);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 3);
   DialogItemCheckButtonSetText(di, _("Waves"));
   DialogItemCheckButtonSetState(di, tmp_effect_waves);
   DialogItemCheckButtonSetPtr(di, &tmp_effect_waves);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 4);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureFX, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureFX, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureFX, 1);
   DialogSetExitFunction(d, CB_ConfigureFX, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureFX, 0, d);

   ShowDialog(d);
}

static Dialog      *bg_sel_dialog;
static DItem       *bg_sel;
static DItem       *bg_sel_slider;
static DItem       *bg_mini_disp;
static DItem       *bg_filename;
static DItem       *tmp_w[10];
static int          tmp_bg_sel_sliderval = 0;
static Background  *tbg = NULL;
static Background  *tmp_bg;
static int          tmp_bg_r;
static int          tmp_bg_g;
static int          tmp_bg_b;
static char         tmp_bg_image;
static char         tmp_bg_tile;
static char         tmp_bg_keep_aspect;
static int          tmp_bg_xjust;
static int          tmp_bg_yjust;
static int          tmp_bg_xperc;
static int          tmp_bg_yperc;
static char         tmp_hiq;
static char         tmp_userbg;
static int          tmp_bg_timeout;
static int          tmp_theme_transparency;

static void         BG_RedrawView(char nuke_old);

static void
CB_ConfigureBG(int val, void *data)
{
   int                 i;

   if (val < 2)
     {
	Conf.backgrounds.timeout = tmp_bg_timeout;
	Conf.backgrounds.hiquality = tmp_hiq;
	Conf.backgrounds.user = tmp_userbg;
#ifdef ENABLE_THEME_TRANSPARENCY
	TransparencySet(tmp_theme_transparency);
#endif
	ESetColor(&(tmp_bg->bg_solid), tmp_bg_r, tmp_bg_g, tmp_bg_b);
	tmp_bg->bg_tile = tmp_bg_tile;
	tmp_bg->bg.keep_aspect = tmp_bg_keep_aspect;
	tmp_bg->bg.xjust = tmp_bg_xjust;
	tmp_bg->bg.yjust = 1024 - tmp_bg_yjust;
	tmp_bg->bg.xperc = tmp_bg_xperc;
	tmp_bg->bg.yperc = 1024 - tmp_bg_yperc;
	if (!tmp_bg_image)
	   RemoveImagesFromBG(tmp_bg);
	for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
	  {
	     if (desks.desk[i].bg == tmp_bg)
	       {
		  if (desks.desk[i].viewable)
		     RefreshDesktop(i);
		  if (i == desks.current)
		    {
		       RedrawPagersForDesktop(i, 2);
		       ForceUpdatePagersForDesktop(i);
		    }
		  else
		     RedrawPagersForDesktop(i, 1);
	       }
	  }
	{
	   char                s[4096];
	   Imlib_Image        *im;
	   Pixmap              p2;

	   Esnprintf(s, sizeof(s), "%s/cached/bgsel/%s", EDirUserCache(),
		     tmp_bg->name);
	   p2 = ECreatePixmap(disp, root.win, 64, 48, root.depth);
	   SetBackgroundTo(p2, tmp_bg, 0);
	   imlib_context_set_drawable(p2);
	   im = imlib_create_image_from_drawable(0, 0, 0, 64, 48, 0);
	   imlib_context_set_image(im);
	   imlib_image_set_format("ppm");
	   imlib_save_image(s);
	   imlib_free_image_and_decache();
	   EFreePixmap(disp, p2);
	   BG_RedrawView(1);
	}
     }
   if (val != 1)
     {
	KeepBGimages(tmp_bg, 0);
     }
   if (tbg)
     {
	FreeDesktopBG(tbg);
	tbg = NULL;
     }
   DesktopAccounting();
   autosave();
   /* This is kind of a hack - somehow it loses the correct current desktop
    * information when we actually open up the dialog box, so this
    * should fix the atom which apparently gets clobbered somewhere
    * above here
    * --Mandrake
    */
   RefreshDesktop(desks.current);

   data = NULL;
}

static void
CB_DesktopMiniDisplayRedraw(int val, void *data)
{
   int                 w, h;
   DItem              *di;
   Window              win;
   Pixmap              pmap;
   XColor              xclr;

   di = (DItem *) data;
   win = DialogItemAreaGetWindow(di);
   DialogItemAreaGetSize(di, &w, &h);

   ESetColor(&xclr, tmp_bg_r, tmp_bg_g, tmp_bg_b);
   if (!tbg)
     {
	if (!tmp_bg_image)
	   tbg =
	      CreateDesktopBG("TEMP", &xclr, NULL, tmp_bg_tile,
			      tmp_bg_keep_aspect, tmp_bg_xjust,
			      1024 - tmp_bg_yjust, tmp_bg_xperc,
			      1024 - tmp_bg_yperc, tmp_bg->top.file,
			      tmp_bg->top.keep_aspect, tmp_bg->top.xjust,
			      tmp_bg->top.yjust, tmp_bg->top.xperc,
			      tmp_bg->top.yperc);
	else
	   tbg =
	      CreateDesktopBG("TEMP", &xclr, tmp_bg->bg.file, tmp_bg_tile,
			      tmp_bg_keep_aspect, tmp_bg_xjust,
			      1024 - tmp_bg_yjust, tmp_bg_xperc,
			      1024 - tmp_bg_yperc, tmp_bg->top.file,
			      tmp_bg->top.keep_aspect, tmp_bg->top.xjust,
			      tmp_bg->top.yjust, tmp_bg->top.xperc,
			      tmp_bg->top.yperc);
     }
   else
     {
	if (tbg->pmap)
	   imlib_free_pixmap_and_mask(tbg->pmap);
	ESetColor(&(tbg->bg_solid), tmp_bg_r, tmp_bg_g, tmp_bg_b);
	if (tbg->bg.file)
	   Efree(tbg->bg.file);
	tbg->bg.file = NULL;
	if (tmp_bg_image)
	   tbg->bg.file = Estrdup(tmp_bg->bg.file);
	else
	  {
	     if (tbg->bg.im)
	       {
		  imlib_context_set_image(tbg->bg.im);
		  imlib_free_image();
		  tbg->bg.im = NULL;
	       }
	  }
	if (tbg->bg.real_file)
	   Efree(tbg->bg.real_file);
	tbg->bg.real_file = NULL;
	tbg->bg_tile = tmp_bg_tile;
	tbg->bg.keep_aspect = tmp_bg_keep_aspect;
	tbg->bg.xjust = tmp_bg_xjust;
	tbg->bg.yjust = 1024 - tmp_bg_yjust;
	tbg->bg.xperc = tmp_bg_xperc;
	tbg->bg.yperc = 1024 - tmp_bg_yperc;
     }
   KeepBGimages(tbg, 1);
   pmap = ECreatePixmap(disp, win, w, h, root.depth);
   ESetWindowBackgroundPixmap(disp, win, pmap);
   SetBackgroundTo(pmap, tbg, 0);
   XClearWindow(disp, win);
   EFreePixmap(disp, pmap);
   val = 0;
}

static void
BG_DoDialog(void)
{
   char               *stmp;
   char                s[1024];

   if (tmp_bg->bg.file)
      tmp_bg_image = 1;
   else
      tmp_bg_image = 0;

   KeepBGimages(tmp_bg, 1);

   if (tmp_bg->bg.file)
      stmp = fullfileof(tmp_bg->bg.file);
   else
      stmp = Estrdup(_("-NONE-"));
   Esnprintf(s, sizeof(s),
	     _("Background definition information:\nName: %s\nFile: %s\n"),
	     tmp_bg->name, stmp);
   Efree(stmp);
   DialogItemTextSetText(bg_filename, s);
   DialogDrawItems(bg_sel_dialog, bg_filename, 0, 0, 99999, 99999);

   EGetColor(&(tmp_bg->bg_solid), &tmp_bg_r, &tmp_bg_g, &tmp_bg_b);
   tmp_bg_tile = tmp_bg->bg_tile;
   tmp_bg_keep_aspect = tmp_bg->bg.keep_aspect;
   tmp_bg_xjust = tmp_bg->bg.xjust;
   tmp_bg_yjust = 1024 - tmp_bg->bg.yjust;
   tmp_bg_xperc = tmp_bg->bg.xperc;
   tmp_bg_yperc = 1024 - tmp_bg->bg.yperc;
   DialogItemSliderSetVal(tmp_w[0], tmp_bg_r);
   DialogItemCheckButtonSetState(tmp_w[1], tmp_bg_image);
   DialogItemSliderSetVal(tmp_w[2], tmp_bg_g);
   DialogItemCheckButtonSetState(tmp_w[3], tmp_bg_keep_aspect);
   DialogItemSliderSetVal(tmp_w[4], tmp_bg_b);
   DialogItemCheckButtonSetState(tmp_w[5], tmp_bg_tile);
   DialogItemSliderSetVal(tmp_w[6], tmp_bg_xjust);
   DialogItemSliderSetVal(tmp_w[7], tmp_bg_yjust);
   DialogItemSliderSetVal(tmp_w[8], tmp_bg_yperc);
   DialogItemSliderSetVal(tmp_w[9], tmp_bg_xperc);
   if (tbg)
     {
	FreeDesktopBG(tbg);
	tbg = NULL;
     }
   CB_DesktopMiniDisplayRedraw(0, bg_mini_disp);
}

static void
CB_ConfigureNewBG(int val, void *data)
{
   char                s[1024];
   XColor              xclr;
   Background         *bg;
   int                 lower, upper;

   Esnprintf(s, sizeof(s), "__NEWBG_%i\n", (unsigned)time(NULL));
   ESetColor(&xclr, tmp_bg_r, tmp_bg_g, tmp_bg_b);
   bg = CreateDesktopBG(s, &xclr, tmp_bg->bg.file, tmp_bg_tile,
			tmp_bg_keep_aspect, tmp_bg_xjust, 1024 - tmp_bg_yjust,
			tmp_bg_xperc, 1024 - tmp_bg_yperc, tmp_bg->top.file,
			tmp_bg->top.keep_aspect, tmp_bg->top.xjust,
			tmp_bg->top.yjust, tmp_bg->top.xperc,
			tmp_bg->top.yperc);
   AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);
   tmp_bg = bg;
   desks.desk[desks.current].bg = bg;
   DialogItemSliderGetBounds(bg_sel_slider, &lower, &upper);
   upper += 4;
   DialogItemSliderSetBounds(bg_sel_slider, lower, upper);
   DialogItemSliderSetVal(bg_sel_slider, 0);
   DialogDrawItems(bg_sel_dialog, bg_sel_slider, 0, 0, 99999, 99999);
   RefreshCurrentDesktop();
   RedrawPagersForDesktop(desks.current, 2);
   ForceUpdatePagersForDesktop(desks.current);
   BG_RedrawView(0);
   autosave();
   val = 0;
   data = NULL;
}

static void
CB_ConfigureRemBG(int val, void *data)
{
   Background        **bglist, *bg;
   int                 i, num;

   bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   if ((bglist) && (num > 1))
     {
	int                 slider, lower, upper;

	DialogItemSliderGetBounds(bg_sel_slider, &lower, &upper);
	slider = DialogItemSliderGetVal(bg_sel_slider);
	upper -= 4;
	DialogItemSliderSetBounds(bg_sel_slider, lower, upper);
	if (slider > upper)
	   DialogItemSliderSetVal(bg_sel_slider, upper);

	for (i = 0; i < num; i++)
	  {
	     if (bglist[i] == tmp_bg)
	       {
		  bg = RemoveItem((char *)tmp_bg, 0, LIST_FINDBY_POINTER,
				  LIST_TYPE_BACKGROUND);
		  if (i < (num - 1))
		     tmp_bg = bglist[i + 1];
		  else
		     tmp_bg = bglist[i - 1];
		  i = num;
		  if (bg)
		     FreeDesktopBG(bg);
		  BG_DoDialog();
	       }
	  }
	desks.desk[desks.current].bg = tmp_bg;
	RedrawPagersForDesktop(desks.current, 2);
	ForceUpdatePagersForDesktop(desks.current);
	RefreshCurrentDesktop();
	BG_RedrawView(0);
	for (i = 0; i < 10; i++)
	   DialogDrawItems(bg_sel_dialog, tmp_w[i], 0, 0, 99999, 99999);
	autosave();
     }
   if (bglist)
      Efree(bglist);
   autosave();
   val = 0;
   data = NULL;
}

static void
CB_ConfigureDelBG(int val, void *data)
{
   Background        **bglist, *bg;
   int                 i, num;

   bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   if ((bglist) && (num > 1))
     {
	int                 slider, lower, upper;

	DialogItemSliderGetBounds(bg_sel_slider, &lower, &upper);
	slider = DialogItemSliderGetVal(bg_sel_slider);
	upper -= 4;
	DialogItemSliderSetBounds(bg_sel_slider, lower, upper);
	if (slider > upper)
	   DialogItemSliderSetVal(bg_sel_slider, upper);

	for (i = 0; i < num; i++)
	  {
	     if (bglist[i] == tmp_bg)
	       {
		  bg = RemoveItem((char *)tmp_bg, 0, LIST_FINDBY_POINTER,
				  LIST_TYPE_BACKGROUND);
		  if (i < (num - 1))
		     tmp_bg = bglist[i + 1];
		  else
		     tmp_bg = bglist[i - 1];
		  i = num;
		  if (bg)
		    {
		       if (bg->bg.file)
			 {
			    char               *f;

			    f = FindFile(bg->bg.file);
			    if (f)
			      {
				 E_rm(f);
				 Efree(f);
			      }
			 }
		       if (bg->top.file)
			 {
			    char               *f;

			    f = FindFile(bg->top.file);
			    if (f)
			      {
				 E_rm(f);
				 Efree(f);
			      }
			 }
		       FreeDesktopBG(bg);
		    }
		  BG_DoDialog();
	       }
	  }
	desks.desk[desks.current].bg = tmp_bg;
	RedrawPagersForDesktop(desks.current, 2);
	ForceUpdatePagersForDesktop(desks.current);
	RefreshCurrentDesktop();
	BG_RedrawView(0);
	for (i = 0; i < 10; i++)
	   DialogDrawItems(bg_sel_dialog, tmp_w[i], 0, 0, 99999, 99999);
	autosave();
     }
   if (bglist)
      Efree(bglist);
   autosave();
   val = 0;
   data = NULL;
}

static void
CB_ConfigureFrontBG(int val, void *data)
{
   int                 i;

   MoveItemToListTop(tmp_bg, LIST_TYPE_BACKGROUND);
   CB_DesktopMiniDisplayRedraw(0, bg_mini_disp);
   BG_RedrawView(0);
   for (i = 0; i < 10; i++)
      DialogDrawItems(bg_sel_dialog, tmp_w[i], 0, 0, 99999, 99999);
   autosave();
   val = 0;
   data = NULL;
}

static int          tmp_bg_selected = -1;

static void
BG_RedrawView(char nuke_old)
{
   int                 num, i;
   Background        **bglist;
   int                 w, h;
   Window              win;
   int                 x;
   Pixmap              pmap;
   GC                  gc;
   XGCValues           gcv;

   win = DialogItemAreaGetWindow(bg_sel);
   DialogItemAreaGetSize(bg_sel, &w, &h);
   bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   if (!bglist)
      goto done;

   pmap = ECreatePixmap(disp, win, w, h, root.depth);
   gc = XCreateGC(disp, pmap, 0, &gcv);
   XSetForeground(disp, gc, BlackPixel(disp, root.scr));
   XFillRectangle(disp, pmap, gc, 0, 0, w, h);
   ESetWindowBackgroundPixmap(disp, win, pmap);
   x = -(tmp_bg_sel_sliderval * (64 + 8) / 4);
   if (x < (w - ((64 + 8) * num)))
      x = w - ((64 + 8) * num);
   for (i = 0; i < num; i++)
     {
	if (((x + 64 + 8) >= 0) && (x < w))
	  {
	     Pixmap              p2;
	     ImageClass         *ic;
	     Imlib_Image        *im;
	     char                s[4096];

	     ic = FindItem("DIALOG_BUTTON", 0, LIST_FINDBY_NAME,
			   LIST_TYPE_ICLASS);
	     if (ic)
	       {
		  PmapMask            pmm;

		  if (i == tmp_bg_selected)
		     IclassApplyCopy(ic, pmap, 64 + 8, 48 + 8, 0, 0,
				     STATE_CLICKED, &pmm, 0);
		  else
		     IclassApplyCopy(ic, pmap, 64 + 8, 48 + 8, 0, 0,
				     STATE_NORMAL, &pmm, 0);
		  XCopyArea(disp, pmm.pmap, pmap, gc, 0, 0, 64 + 8, 48 + 8, x,
			    0);
		  FreePmapMask(&pmm);
	       }

	     if (!strcmp(bglist[i]->name, "NONE"))
	       {
		  TextClass          *tc;

		  tc = FindItem("DIALOG", 0, LIST_FINDBY_NAME,
				LIST_TYPE_TCLASS);
		  if (tc)
		    {
		       int                 tw, th;

		       TextSize(tc, 0, 0, STATE_NORMAL,
				_("No\nBackground"), &tw, &th, 17);
		       TextDraw(tc, pmap, 0, 0, STATE_NORMAL,
				_("No\nBackground"), x + 4,
				4 + ((48 - th) / 2), 64, 48, 17, 512);
		    }
	       }
	     else
	       {
		  Esnprintf(s, sizeof(s), "%s/cached/bgsel/%s",
			    EDirUserCache(), bglist[i]->name);
		  im = ELoadImage(s);
		  if (!im)
		    {
		       Esnprintf(s, sizeof(s), "%s/cached/bgsel/%s",
				 EDirUserCache(), bglist[i]->name);
		       p2 = ECreatePixmap(disp, pmap, 64, 48, root.depth);
		       SetBackgroundTo(p2, bglist[i], 0);
		       XCopyArea(disp, p2, pmap, gc, 0, 0, 64, 48, x + 4, 4);
		       imlib_context_set_drawable(p2);
		       im =
			  imlib_create_image_from_drawable(0, 0, 0, 64, 48, 0);
		       imlib_context_set_image(im);
		       imlib_image_set_format("ppm");
		       imlib_save_image(s);
		       imlib_free_image_and_decache();
		       EFreePixmap(disp, p2);
		    }
		  else
		    {
		       if (nuke_old)
			 {
			    imlib_context_set_image(im);
			    imlib_free_image_and_decache();
			    im = ELoadImage(s);
			 }
		       if (im)
			 {
			    imlib_context_set_image(im);
			    imlib_context_set_drawable(pmap);
			    imlib_render_image_on_drawable_at_size(x + 4, 4, 64,
								   48);
			    imlib_free_image();
			 }
		    }
	       }
	  }
	x += (64 + 8);
     }
   XFreeGC(disp, gc);
   EFreePixmap(disp, pmap);
   Efree(bglist);

 done:
   XClearWindow(disp, win);
}

static void
CB_BGAreaSlide(int val, void *data)
{
   BG_RedrawView(0);
   val = 0;
   data = NULL;
}

static void
CB_BGScan(int val, void *data)
{
   int                 slider, lower, upper;

   SoundPlay("SOUND_WAIT");

   DialogItemSliderGetBounds(bg_sel_slider, &lower, &upper);

   for (slider = lower; slider <= upper; slider += 8)
     {
	DialogItemSliderSetVal(bg_sel_slider, slider);
	DialogDrawItems(bg_sel_dialog, bg_sel_slider, 0, 0, 99999, 99999);
	DialogItemCallCallback(bg_sel_slider);
     }
   val = 0;
   data = NULL;
}

static void
CB_BGAreaEvent(int val, void *data)
{
   int                 x, num, w, h;
   Background        **bglist;
   XEvent             *ev;

   ev = (XEvent *) data;
   DialogItemAreaGetSize(bg_sel, &w, &h);
   switch (ev->type)
     {
     case ButtonPress:
	bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
	x = (tmp_bg_sel_sliderval * (64 + 8) / 4);
	if (x > ((num * (64 + 8)) - w))
	   x = ((num * (64 + 8)) - w);
	x += ev->xbutton.x;
	x = x / (64 + 8);
	tmp_bg_selected = x;
	if ((tmp_bg_selected >= 0) && (tmp_bg_selected < num))
	  {
	     KeepBGimages(tmp_bg, 0);
	     tmp_bg = bglist[tmp_bg_selected];
	     BG_DoDialog();
	     desks.desk[desks.current].bg = tmp_bg;
	     RedrawPagersForDesktop(desks.current, 2);
	     ForceUpdatePagersForDesktop(desks.current);
	     RefreshCurrentDesktop();
	     BG_RedrawView(0);
	     for (x = 0; x < 10; x++)
		DialogDrawItems(bg_sel_dialog, tmp_w[x], 0, 0, 99999, 99999);
	     autosave();
	  }
	if (bglist)
	   Efree(bglist);
	break;
     case ButtonRelease:
	tmp_bg_selected = -1;
	BG_RedrawView(0);
	break;
     default:
	break;
     }
   val = 0;
}

static void
CB_DesktopTimeout(int val, void *data)
{
   DItem              *di;
   char                s[256];

   di = (DItem *) data;
   Esnprintf(s, sizeof(s), _("Unused backgrounds freed after %2i:%02i:%02i"),
	     tmp_bg_timeout / 3600,
	     (tmp_bg_timeout / 60) - (60 * (tmp_bg_timeout / 3600)),
	     (tmp_bg_timeout) - (60 * (tmp_bg_timeout / 60)));
   DialogItemTextSetText(di, s);
   DialogDrawItems(bg_sel_dialog, di, 0, 0, 99999, 99999);
   return;
   val = 0;
}

void
BGSettingsGoTo(Background * bg)
{
   Dialog             *bgd;
   Background        **bglist;
   int                 i, num, x;

   if (!(bgd = FindItem("CONFIGURE_BG", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
      return;

   bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   if (bglist)
     {
	for (i = 0; i < num; i++)
	  {
	     if (bglist[i] == bg)
	       {
		  DialogItemSliderSetVal(bg_sel_slider, 4 * i);
		  DialogDrawItems(bg_sel_dialog, bg_sel_slider, 0, 0, 99999,
				  99999);
		  DialogItemCallCallback(bg_sel_slider);
		  tmp_bg_selected = i;
		  KeepBGimages(tmp_bg, 0);
		  tmp_bg = bglist[tmp_bg_selected];
		  BG_DoDialog();
		  BG_RedrawView(0);
		  for (x = 0; x < 10; x++)
		     DialogDrawItems(bg_sel_dialog, tmp_w[x], 0, 0, 99999,
				     99999);
		  Efree(bglist);
		  tmp_bg_selected = -1;
		  return;
	       }
	  }
	Efree(bglist);
     }
}

static void
CB_BGPrev(int val, void *data)
{
   Background        **bglist;
   int                 i, num;

   bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   if (bglist)
     {
	for (i = 0; i < num; i++)
	  {
	     if ((bglist[i] == tmp_bg) && (i > 0))
	       {
		  desks.desk[desks.current].bg = bglist[i - 1];
		  BGSettingsGoTo(bglist[i - 1]);
		  RedrawPagersForDesktop(desks.current, 2);
		  ForceUpdatePagersForDesktop(desks.current);
		  RefreshCurrentDesktop();
		  Efree(bglist);
		  return;
	       }
	  }
	Efree(bglist);
     }
   val = 0;
   data = NULL;
}

static void
CB_BGNext(int val, void *data)
{
   Background        **bglist;
   int                 i, num;

   bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   if (bglist)
     {
	for (i = 0; i < num; i++)
	  {
	     if ((bglist[i] == tmp_bg) && (i < (num - 1)))
	       {
		  desks.desk[desks.current].bg = bglist[i + 1];
		  BGSettingsGoTo(bglist[i + 1]);
		  RedrawPagersForDesktop(desks.current, 2);
		  ForceUpdatePagersForDesktop(desks.current);
		  RefreshCurrentDesktop();
		  Efree(bglist);
		  return;
	       }
	  }
	Efree(bglist);
     }
   val = 0;
   data = NULL;
}

static int
BG_SortFileCompare(Background * bg1, Background * bg2)
{
   /* return < 0 is b1 <  b2 */
   /* return > 0 is b1 >  b2 */
   /* return   0 is b1 == b2 */
   if ((bg1->bg.file) && (bg2->bg.file))
      return strcmp(bg1->bg.file, bg2->bg.file);
   if ((bg1->top.file) && (bg2->top.file))
      return strcmp(bg1->top.file, bg2->top.file);
   return 0;
}

static void
CB_BGSortFile(int val, void *data)
{
   Background        **bglist;
   int                 i, num;

   bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   if (bglist)
     {
	/* remove them all from the list */
	for (i = 0; i < num; i++)
	   RemoveItem((char *)(bglist[i]), 0, LIST_FINDBY_POINTER,
		      LIST_TYPE_BACKGROUND);
	Quicksort((void **)bglist, 0, num - 1,
		  (int (*)(void *d1, void *d2))BG_SortFileCompare);
	for (i = 0; i < num; i++)
	  {
	     Background         *bg;

	     bg = bglist[i];
	     AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);
	  }
	Efree(bglist);
	BGSettingsGoTo(tmp_bg);
     }
   autosave();
   val = 0;
   data = NULL;
}

static void
CB_BGSortAttrib(int val, void *data)
{
   Background        **bglist;
   int                 i, num;

   bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   if (bglist)
     {
	/* remove them all from the list */
	for (i = 0; i < num; i++)
	   RemoveItem((char *)(bglist[i]), 0, LIST_FINDBY_POINTER,
		      LIST_TYPE_BACKGROUND);
	for (i = 0; i < num; i++)
	  {
	     Background         *bg;

	     bg = bglist[i];
	     if ((bg) && (bg->bg_tile) && (bg->bg.xperc == 0)
		 && (bg->bg.yperc == 0))
	       {
		  AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);
		  bglist[i] = NULL;
	       }
	  }
	for (i = 0; i < num; i++)
	  {
	     Background         *bg;

	     bg = bglist[i];
	     if (bg)
	       {
		  AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);
		  bglist[i] = NULL;
	       }
	  }
	Efree(bglist);
	BGSettingsGoTo(tmp_bg);
     }
   autosave();
   val = 0;
   data = NULL;
}

static void
CB_BGSortContent(int val, void *data)
{
   Background        **bglist;
   int                 i, num;

   bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   if (bglist)
     {
	/* remove them all from the list */
	for (i = 0; i < num; i++)
	   RemoveItem((char *)(bglist[i]), 0, LIST_FINDBY_POINTER,
		      LIST_TYPE_BACKGROUND);
	for (i = 0; i < num; i++)
	  {
	     Background         *bg;

	     bg = bglist[i];
	     AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);
	  }
	Efree(bglist);
     }
   autosave();
   val = 0;
   data = NULL;
}

#ifdef ENABLE_THEME_TRANSPARENCY
static void
CB_ThemeTransparency(int val, void *data)
{
   DItem              *di;
   char                s[256];

   di = (DItem *) data;
   Esnprintf(s, sizeof(s), _("Theme transparency: %2d"),
	     tmp_theme_transparency);
   DialogItemTextSetText(di, s);
   DialogDrawItems(bg_sel_dialog, di, 0, 0, 99999, 99999);
   return;
   val = 0;
}
#endif

void
SettingsBackground(Background * bg)
{
   Dialog             *d;
   DItem              *table, *di, *table2, *area, *slider, *slider2, *label;
   DItem              *w1, *w2, *w3, *w4, *w5, *w6;
   int                 num;
   char                s[1024];

   if ((d = FindItem("CONFIGURE_BG", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_BG");

   if ((!bg) || ((bg) && (!strcmp(bg->name, "NONE"))))
     {
	Esnprintf(s, sizeof(s), "__NEWBG_%i\n", (unsigned)time(NULL));
	bg = CreateDesktopBG(s, NULL, NULL, 1, 1, 0, 0, 0, 0, NULL, 1, 512, 512,
			     0, 0);
	AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);
	/*
	 * desks.desk[desks.current].bg = bg;
	 */
     }
   tmp_bg = bg;
   if (bg->bg.file)
      tmp_bg_image = 1;
   else
      tmp_bg_image = 0;
   tmp_bg->keepim = 1;

   EGetColor(&(bg->bg_solid), &tmp_bg_r, &tmp_bg_g, &tmp_bg_b);
   tmp_bg_tile = bg->bg_tile;
   tmp_bg_keep_aspect = bg->bg.keep_aspect;
   tmp_bg_xjust = bg->bg.xjust;
   tmp_bg_yjust = 1024 - bg->bg.yjust;
   tmp_bg_xperc = bg->bg.xperc;
   tmp_bg_yperc = 1024 - bg->bg.yperc;
   tmp_hiq = Conf.backgrounds.hiquality;
   tmp_userbg = Conf.backgrounds.user;
   tmp_bg_timeout = Conf.backgrounds.timeout;
   tmp_theme_transparency = Conf.theme.transparency;

   d = bg_sel_dialog = DialogCreate("CONFIGURE_BG");
   DialogSetTitle(d, _("Desktop Background Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 3, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/bg.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Desktop\n"
				"Background Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 3);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemTextSetText(di, _("BG Colour\n"));

   di = DialogAddItem(table, DITEM_NONE);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemSetPadding(table2, 2, 2, 2, 2);
   DialogItemTableSetOptions(table2, 4, 0, 0, 0);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemButtonSetText(di, _("Move to Front\n"));
   DialogItemSetCallback(di, CB_ConfigureFrontBG, 0, NULL);
   DialogBindKey(d, "F", CB_ConfigureFrontBG, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemButtonSetText(di, _("Duplicate\n"));
   DialogItemSetCallback(di, CB_ConfigureNewBG, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemButtonSetText(di, _("Unlist\n"));
   DialogItemSetCallback(di, CB_ConfigureRemBG, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemButtonSetText(di, _("Delete File\n"));
   DialogItemSetCallback(di, CB_ConfigureDelBG, 0, NULL);
   DialogBindKey(d, "D", CB_ConfigureDelBG, 0, NULL);
   DialogBindKey(d, "Delete", CB_ConfigureDelBG, 0, NULL);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, _("Red:\n"));

   di = w1 = tmp_w[0] = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 255);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 16);
   DialogItemSliderSetVal(di, tmp_bg_r);
   DialogItemSliderSetValPtr(di, &tmp_bg_r);

   di = w2 = tmp_w[1] = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Use background image"));
   DialogItemCheckButtonSetState(di, tmp_bg_image);
   DialogItemCheckButtonSetPtr(di, &tmp_bg_image);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, _("Green:\n"));

   di = w3 = tmp_w[2] = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 255);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 16);
   DialogItemSliderSetVal(di, tmp_bg_g);
   DialogItemSliderSetValPtr(di, &tmp_bg_g);

   di = w4 = tmp_w[3] = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Keep aspect on scale"));
   DialogItemCheckButtonSetState(di, tmp_bg_keep_aspect);
   DialogItemCheckButtonSetPtr(di, &tmp_bg_keep_aspect);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, _("Blue:\n"));

   di = w5 = tmp_w[4] = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 255);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 16);
   DialogItemSliderSetVal(di, tmp_bg_b);
   DialogItemSliderSetValPtr(di, &tmp_bg_b);

   di = w6 = tmp_w[5] = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Tile image across background"));
   DialogItemCheckButtonSetState(di, tmp_bg_tile);
   DialogItemCheckButtonSetPtr(di, &tmp_bg_tile);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 512, 512);
   DialogItemSetColSpan(di, 2);
   DialogItemTextSetText(di,
			 _("Background\n" "Image\n" "Scaling\n" "and\n"
			   "Alignment\n"));

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemSetPadding(table2, 2, 2, 2, 2);
   DialogItemTableSetOptions(table2, 3, 0, 0, 0);

   di = DialogAddItem(table2, DITEM_NONE);

   di = slider = tmp_w[6] = DialogAddItem(table2, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 1024);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 64);
   DialogItemSliderSetVal(di, tmp_bg_xjust);
   DialogItemSliderSetValPtr(di, &tmp_bg_xjust);

   di = DialogAddItem(table2, DITEM_NONE);

   di = slider2 = tmp_w[7] = DialogAddItem(table2, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetOrientation(di, 0);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 1);
   DialogItemSliderSetBounds(di, 0, 1024);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 64);
   DialogItemSliderSetVal(di, tmp_bg_yjust);
   DialogItemSliderSetValPtr(di, &tmp_bg_yjust);

   di = bg_mini_disp = area = DialogAddItem(table2, DITEM_AREA);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemAreaSetSize(di, 64, 48);

   DialogItemSetCallback(w1, CB_DesktopMiniDisplayRedraw, 0, (void *)area);
   DialogItemSetCallback(w2, CB_DesktopMiniDisplayRedraw, 0, (void *)area);
   DialogItemSetCallback(w3, CB_DesktopMiniDisplayRedraw, 0, (void *)area);
   DialogItemSetCallback(w4, CB_DesktopMiniDisplayRedraw, 0, (void *)area);
   DialogItemSetCallback(w5, CB_DesktopMiniDisplayRedraw, 0, (void *)area);
   DialogItemSetCallback(w6, CB_DesktopMiniDisplayRedraw, 0, (void *)area);
   DialogItemSetCallback(slider, CB_DesktopMiniDisplayRedraw, 0, (void *)area);
   DialogItemSetCallback(slider2, CB_DesktopMiniDisplayRedraw, 0, (void *)area);

   di = slider = tmp_w[8] = DialogAddItem(table2, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetOrientation(di, 0);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 1);
   DialogItemSliderSetBounds(di, 0, 1024);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 64);
   DialogItemSliderSetVal(di, tmp_bg_yperc);
   DialogItemSliderSetValPtr(di, &tmp_bg_yperc);
   DialogItemSetCallback(slider, CB_DesktopMiniDisplayRedraw, 0, (void *)area);

   di = DialogAddItem(table2, DITEM_NONE);

   di = slider = tmp_w[9] = DialogAddItem(table2, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 1024);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 64);
   DialogItemSliderSetVal(di, tmp_bg_xperc);
   DialogItemSliderSetValPtr(di, &tmp_bg_xperc);
   DialogItemSetCallback(slider, CB_DesktopMiniDisplayRedraw, 0, (void *)area);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Use dithering in Hi-Colour"));
   DialogItemCheckButtonSetState(di, tmp_hiq);
   DialogItemCheckButtonSetPtr(di, &tmp_hiq);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Background overrides theme"));
   DialogItemCheckButtonSetState(di, tmp_userbg);
   DialogItemCheckButtonSetPtr(di, &tmp_userbg);

   di = label = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 512, 512);
   Esnprintf(s, sizeof(s), _("Unused backgrounds freed after %2i:%02i:%02i"),
	     tmp_bg_timeout / 3600,
	     (tmp_bg_timeout / 60) - (60 * (tmp_bg_timeout / 3600)),
	     (tmp_bg_timeout) - (60 * (tmp_bg_timeout / 60)));
   DialogItemTextSetText(di, s);

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetColSpan(di, 3);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 60 * 60 * 4);
   DialogItemSliderSetUnits(di, 30);
   DialogItemSliderSetJump(di, 60);
   DialogItemSliderSetVal(di, tmp_bg_timeout);
   DialogItemSliderSetValPtr(di, &tmp_bg_timeout);
   DialogItemSetCallback(di, CB_DesktopTimeout, 0, (void *)label);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemSetPadding(table2, 2, 2, 2, 2);
   DialogItemTableSetOptions(table2, 2, 0, 0, 0);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemButtonSetText(di, "<-");
   DialogItemSetCallback(di, CB_BGPrev, 0, NULL);
   DialogBindKey(d, "Left", CB_BGPrev, 0, NULL);
   DialogBindKey(d, "BackSpace", CB_BGPrev, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemButtonSetText(di, "->");
   DialogItemSetCallback(di, CB_BGNext, 0, NULL);
   DialogBindKey(d, "Right", CB_BGNext, 0, NULL);
   DialogBindKey(d, "space", CB_BGNext, 0, NULL);

   di = DialogAddItem(table, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemButtonSetText(di, _("Pre-scan BG's"));
   DialogItemSetCallback(di, CB_BGScan, 0, NULL);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemSetPadding(table2, 2, 2, 2, 2);
   DialogItemTableSetOptions(table2, 3, 0, 0, 0);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemButtonSetText(di, _("Sort by File"));
   DialogItemSetCallback(di, CB_BGSortFile, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemButtonSetText(di, _("Sort by Attr."));
   DialogItemSetCallback(di, CB_BGSortAttrib, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemButtonSetText(di, _("Sort by Image"));
   DialogItemSetCallback(di, CB_BGSortContent, 0, NULL);

   di = bg_sel = DialogAddItem(table, DITEM_AREA);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemAreaSetSize(di, 160, 56);
   DialogItemAreaSetEventFunc(di, CB_BGAreaEvent);

   num = 0;
   {
      Background        **bglist;

      bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
      if (bglist)
	 Efree(bglist);
   }
   di = bg_sel_slider = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, num * 4);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 9);
   DialogItemSliderSetVal(di, tmp_bg_sel_sliderval);
   DialogItemSliderSetValPtr(di, &tmp_bg_sel_sliderval);
   DialogItemSetCallback(bg_sel_slider, CB_BGAreaSlide, 0, NULL);

   di = bg_filename = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 512, 512);
   {
      char               *stmp;

      if (tmp_bg->bg.file)
	 stmp = fullfileof(tmp_bg->bg.file);
      else
	 stmp = Estrdup(_("-NONE-"));
      Esnprintf(s, sizeof(s),
		_("Background definition information:\nName: %s\nFile: %s\n"),
		tmp_bg->name, stmp);
      Efree(stmp);
      DialogItemTextSetText(bg_filename, s);
   }

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

#ifdef ENABLE_THEME_TRANSPARENCY
   di = label = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 512, 512);
   Esnprintf(s, sizeof(s), _("Theme transparency: %2d"),
	     tmp_theme_transparency);
   DialogItemTextSetText(di, s);

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetColSpan(di, 3);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 255);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 16);
   DialogItemSliderSetVal(di, tmp_theme_transparency);
   DialogItemSliderSetValPtr(di, &tmp_theme_transparency);
   DialogItemSetCallback(di, CB_ThemeTransparency, 0, (void *)label);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);
#endif /* ENABLE_THEME_TRANSPARENCY */

   DialogAddButton(d, _("OK"), CB_ConfigureBG, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureBG, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureBG, 1);
   DialogSetExitFunction(d, CB_ConfigureBG, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureBG, 0, d);

   ShowDialog(d);

   CB_DesktopMiniDisplayRedraw(1, area);
   BG_RedrawView(0);
   BGSettingsGoTo(tmp_bg);
   /* This is kind of a hack - somehow it loses the correct current desktop
    * information when we actually open up the dialog box, so this
    * should fix the atom which apparently gets clobbered somewhere
    * above here
    * --Mandrake
    */
   RefreshDesktop(desks.current);
}

static char        *tmp_ib_name = NULL;
static char         tmp_ib_nobg;
static char         tmp_ib_shownames;
static int          tmp_ib_vert;
static int          tmp_ib_side;
static int          tmp_ib_arrows;
static int          tmp_ib_iconsize;
static int          tmp_ib_mode;
static char         tmp_ib_auto_resize;
static char         tmp_ib_draw_icon_base;
static char         tmp_ib_scrollbar_hide;
static char         tmp_ib_cover_hide;
static int          tmp_ib_autoresize_anchor;
static char         tmp_ib_animate;

static void
CB_ConfigureIconbox(int val, void *data)
{
   if (val < 2)
     {
	Iconbox            *ib;

	if (!tmp_ib_name)
	   return;
	ib = FindItem(tmp_ib_name, 0, LIST_FINDBY_NAME, LIST_TYPE_ICONBOX);
	if (!ib)
	   return;

	ib->nobg = tmp_ib_nobg;
	ib->shownames = tmp_ib_shownames;
	ib->orientation = tmp_ib_vert;
	ib->scrollbar_side = tmp_ib_side;
	ib->arrow_side = tmp_ib_arrows;
	ib->iconsize = tmp_ib_iconsize;
	ib->icon_mode = tmp_ib_mode;
	ib->auto_resize = tmp_ib_auto_resize;
	ib->draw_icon_base = tmp_ib_draw_icon_base;
	ib->scrollbar_hide = tmp_ib_scrollbar_hide;
	ib->cover_hide = tmp_ib_cover_hide;
	ib->auto_resize_anchor = tmp_ib_autoresize_anchor;
	ib->animate = tmp_ib_animate;
	IB_CompleteRedraw(ib);
     }
   autosave();
   data = NULL;
}

void
SettingsIconbox(char *name)
{
   Dialog             *d;
   DItem              *table, *di, *radio1, *radio2, *radio3, *radio4, *table2;
   Iconbox            *ib;

   if ((d =
	FindItem("CONFIGURE_ICONBOX", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_ICONBOX");

   if (!name)
      return;
   ib = FindItem(name, 0, LIST_FINDBY_NAME, LIST_TYPE_ICONBOX);
   if (!ib)
      return;
   tmp_ib_nobg = ib->nobg;
   tmp_ib_shownames = ib->shownames;
   tmp_ib_vert = ib->orientation;
   tmp_ib_side = ib->scrollbar_side;
   tmp_ib_arrows = ib->arrow_side;
   tmp_ib_iconsize = ib->iconsize;
   tmp_ib_mode = ib->icon_mode;
   tmp_ib_auto_resize = ib->auto_resize;
   tmp_ib_draw_icon_base = ib->draw_icon_base;
   tmp_ib_scrollbar_hide = ib->scrollbar_hide;
   tmp_ib_cover_hide = ib->cover_hide;
   tmp_ib_autoresize_anchor = ib->auto_resize_anchor;
   tmp_ib_animate = ib->animate;
   if (tmp_ib_name)
      Efree(tmp_ib_name);
   tmp_ib_name = Estrdup(name);

   d = DialogCreate("CONFIGURE_ICONBOX");
   DialogSetTitle(d, _("Iconbox Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 1, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	table2 = DialogAddItem(table, DITEM_TABLE);
	DialogItemTableSetOptions(table2, 2, 0, 0, 0);

	di = DialogAddItem(table2, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/iconbox.png");

	di = DialogAddItem(table2, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Iconbox\n" "Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Transparent background"));
   DialogItemCheckButtonSetState(di, tmp_ib_nobg);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_nobg);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Hide border around inner Iconbox"));
   DialogItemCheckButtonSetState(di, tmp_ib_cover_hide);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_cover_hide);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Show icon names"));
   DialogItemCheckButtonSetState(di, tmp_ib_shownames);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_shownames);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Draw base image behind Icons"));
   DialogItemCheckButtonSetState(di, tmp_ib_draw_icon_base);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_draw_icon_base);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Hide scrollbar when not needed"));
   DialogItemCheckButtonSetState(di, tmp_ib_scrollbar_hide);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_scrollbar_hide);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Automatically resize to fit Icons"));
   DialogItemCheckButtonSetState(di, tmp_ib_auto_resize);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_auto_resize);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di,
				_("Animate when iconifying to this Iconbox"));
   DialogItemCheckButtonSetState(di, tmp_ib_animate);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_animate);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 0);
   DialogItemTextSetText(di,
			 _
			 ("Alignment of anchoring when automatically resizing:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 1024);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 8);
   DialogItemSliderSetVal(di, tmp_ib_autoresize_anchor);
   DialogItemSliderSetValPtr(di, &tmp_ib_autoresize_anchor);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 0);
   DialogItemTextSetText(di,
			 _
			 ("Icon image display policy (if one operation fails, try the next):"));

   radio4 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di,
				_
				("Snapshot Windows, Use application icon, Use Enlightenment Icon"));
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di,
				_
				("Use application icon, Use Enlightenment Icon, Snapshot Window"));
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di,
				_("Use Enlightenment Icon, Snapshot Window"));
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 2);
   DialogItemRadioButtonGroupSetValPtr(radio4, &tmp_ib_mode);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Icon size"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 4, 128);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 8);
   DialogItemSliderSetVal(di, tmp_ib_iconsize);
   DialogItemSliderSetValPtr(di, &tmp_ib_iconsize);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemTableSetOptions(table2, 3, 0, 0, 0);

   di = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Orientation:"));

   di = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Scrollbar side:"));

   di = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Scrollbar arrows:"));

   radio1 = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Horizontal"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio2 = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Left / Top"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio3 = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Start"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Vertical"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio1, &tmp_ib_vert);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Right / Bottom"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio2, &tmp_ib_side);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Both ends"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table2, DITEM_NONE);
   di = DialogAddItem(table2, DITEM_NONE);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("End"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table2, DITEM_NONE);
   di = DialogAddItem(table2, DITEM_NONE);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("None"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio3, &tmp_ib_arrows);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureIconbox, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureIconbox, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureIconbox, 1);
   DialogSetExitFunction(d, CB_ConfigureIconbox, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureIconbox, 0, d);
   ShowDialog(d);
}

static GroupConfig *tmp_cfgs = NULL;
static int          tmp_current_group;
static int          tmp_index;
static GroupConfig  tmp_cfg;
static EWin        *tmp_ewin;
static Group      **tmp_groups;

static DItem       *di_border;
static DItem       *di_iconify;
static DItem       *di_kill;
static DItem       *di_move;
static DItem       *di_raise;
static DItem       *di_stick;
static DItem       *di_shade;
static DItem       *di_mirror;

static void
CB_ConfigureGroupEscape(int val, void *data)
{
   if (tmp_cfgs)
     {
	ShowHideWinGroups(tmp_ewin, tmp_groups[tmp_current_group], SET_OFF);
	Efree(tmp_cfgs);
	tmp_cfgs = NULL;
     }
   DialogClose((Dialog *) data);
   val = 0;
}

static void
CB_ConfigureGroup(int val, void *data)
{
   int                 i;

   if (val < 2)
     {
	for (i = 0; i < tmp_ewin->num_groups; i++)
	   CopyGroupConfig(&(tmp_cfgs[i]), &(tmp_ewin->groups[i]->cfg));
     }
   if (((val == 0) || (val == 2)) && tmp_cfgs)
     {
	ShowHideWinGroups(tmp_ewin, tmp_groups[tmp_current_group], SET_OFF);
	Efree(tmp_cfgs);
	tmp_cfgs = NULL;
     }
   autosave();
   data = NULL;
}

void
GroupSelectCallback(int val, void *data)
{
   DialogItemCheckButtonSetState(di_border, tmp_cfgs[val].set_border);
   DialogItemCheckButtonSetState(di_kill, tmp_cfgs[val].kill);
   DialogItemCheckButtonSetState(di_move, tmp_cfgs[val].move);
   DialogItemCheckButtonSetState(di_raise, tmp_cfgs[val].raise);
   DialogItemCheckButtonSetState(di_iconify, tmp_cfgs[val].iconify);
   DialogItemCheckButtonSetState(di_stick, tmp_cfgs[val].stick);
   DialogItemCheckButtonSetState(di_shade, tmp_cfgs[val].shade);
   DialogItemCheckButtonSetState(di_mirror, tmp_cfgs[val].mirror);
   DialogRedraw((Dialog *) data);
   ShowHideWinGroups(tmp_ewin, tmp_groups[tmp_current_group], SET_OFF);
   ShowHideWinGroups(tmp_ewin, tmp_groups[val], SET_ON);
   tmp_current_group = val;
}

void
GroupFeatureChangeCallback(int val, void *data)
{
   switch (val)
     {
     case GROUP_FEATURE_BORDER:
	tmp_cfgs[tmp_current_group].set_border = *((char *)data);
	break;
     case GROUP_FEATURE_KILL:
	tmp_cfgs[tmp_current_group].kill = *((char *)data);
	break;
     case GROUP_FEATURE_MOVE:
	tmp_cfgs[tmp_current_group].move = *((char *)data);
	break;
     case GROUP_FEATURE_RAISE:
	tmp_cfgs[tmp_current_group].raise = *((char *)data);
	break;
     case GROUP_FEATURE_ICONIFY:
	tmp_cfgs[tmp_current_group].iconify = *((char *)data);
	break;
     case GROUP_FEATURE_STICK:
	tmp_cfgs[tmp_current_group].stick = *((char *)data);
	break;
     case GROUP_FEATURE_SHADE:
	tmp_cfgs[tmp_current_group].shade = *((char *)data);
	break;
     case GROUP_FEATURE_MIRROR:
	tmp_cfgs[tmp_current_group].mirror = *((char *)data);
	break;
     default:
	break;
     }
}

void
SettingsGroups(EWin * ewin)
{
   Dialog             *d;
   DItem              *table, *radio, *di;
   int                 i;
   char              **group_member_strings;

   if (!ewin)
      return;
   if (ewin->num_groups == 0)
     {
	DialogOK(_("Window Group Error"),
		 _
		 ("\n  This window currently does not belong to any groups.  \n\n"));
	return;
     }
   if ((d = FindItem("CONFIGURE_GROUP", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("GROUP_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_GROUP");

   tmp_ewin = ewin;
   tmp_groups = ewin->groups;
   tmp_cfgs = (GroupConfig *) Emalloc(ewin->num_groups * sizeof(GroupConfig));
   tmp_current_group = 0;
   group_member_strings =
      GetWinGroupMemberNames(ewin->groups, ewin->num_groups);
   ShowHideWinGroups(ewin, ewin->groups[0], SET_ON);

   for (i = 0; i < ewin->num_groups; i++)
      CopyGroupConfig(&(ewin->groups[i]->cfg), &(tmp_cfgs[i]));

   d = DialogCreate("CONFIGURE_GROUP");
   DialogSetTitle(d, _("Window Group Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/group.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Window Group\n"
				"Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("   Pick the group to configure:   "));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupSelectCallback, 0, (void *)d);
   DialogItemRadioButtonSetText(di, group_member_strings[0]);
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   for (i = 1; i < ewin->num_groups; i++)
     {
	di = DialogAddItem(table, DITEM_RADIOBUTTON);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSetCallback(di, &GroupSelectCallback, i, (void *)d);
	DialogItemRadioButtonSetText(di, group_member_strings[i]);
	DialogItemRadioButtonSetFirst(di, radio);
	DialogItemRadioButtonGroupSetVal(di, i);
     }
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_index);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di,
			 _("  The following actions are  \n"
			   "  applied to all group members:  "));

   di_border = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_BORDER,
			 &(tmp_cfg.set_border));
   DialogItemCheckButtonSetText(di, _("Changing Border Style"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].set_border);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.set_border));

   di_iconify = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback,
			 GROUP_FEATURE_ICONIFY, &(tmp_cfg.iconify));
   DialogItemCheckButtonSetText(di, _("Iconifying"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].iconify);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.iconify));

   di_kill = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_KILL,
			 &(tmp_cfg.kill));
   DialogItemCheckButtonSetText(di, _("Killing"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].kill);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.kill));

   di_move = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_MOVE,
			 &(tmp_cfg.move));
   DialogItemCheckButtonSetText(di, _("Moving"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].move);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.move));

   di_raise = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_RAISE,
			 &(tmp_cfg.raise));
   DialogItemCheckButtonSetText(di, _("Raising/Lowering"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].raise);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.raise));

   di_stick = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_STICK,
			 &(tmp_cfg.stick));
   DialogItemCheckButtonSetText(di, _("Sticking"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].stick);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.stick));

   di_shade = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_SHADE,
			 &(tmp_cfg.shade));
   DialogItemCheckButtonSetText(di, _("Shading"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].shade);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.shade));

   di_mirror = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_MIRROR,
			 &(tmp_cfg.mirror));
   DialogItemCheckButtonSetText(di, _("Mirror Shade/Iconify/Stick"));
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].mirror);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.mirror));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureGroup, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureGroup, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureGroup, 1);
   DialogSetExitFunction(d, CB_ConfigureGroup, 2, d);
   DialogBindKey(d, "Escape", CB_ConfigureGroupEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureGroup, 0, d);

   for (i = 0; i < ewin->num_groups; i++)
      Efree(group_member_strings[i]);
   Efree(group_member_strings);

   ShowDialog(d);
}

static GroupConfig  tmp_group_cfg;
static char         tmp_group_swap;
static void
CB_ConfigureDefaultGroupSettings(int val, void *data)
{
   if (val < 2)
     {
	CopyGroupConfig(&tmp_group_cfg, &(Conf.group_config));
	Conf.group_swapmove = tmp_group_swap;
     }
   autosave();
   data = NULL;
}

void
SettingsDefaultGroupControl(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d =
	FindItem("CONFIGURE_DEFAULT_GROUP_CONTROL", 0, LIST_FINDBY_NAME,
		 LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_GROUP");

   CopyGroupConfig(&(Conf.group_config), &tmp_group_cfg);
   tmp_group_swap = Conf.group_swapmove;

   d = DialogCreate("CONFIGURE_DEFAULT_GROUP_CONTROL");
   DialogSetTitle(d, _("Default Group Control Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/group.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Default\n"
				"Group Control Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _(" Per-group settings: "));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Changing Border Style"));
   DialogItemCheckButtonSetState(di, tmp_group_cfg.set_border);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.set_border));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Iconifying"));
   DialogItemCheckButtonSetState(di, tmp_group_cfg.iconify);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.iconify));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Killing"));
   DialogItemCheckButtonSetState(di, tmp_group_cfg.kill);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.kill));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Moving"));
   DialogItemCheckButtonSetState(di, tmp_group_cfg.move);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.move));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Raising/Lowering"));
   DialogItemCheckButtonSetState(di, tmp_group_cfg.raise);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.raise));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Sticking"));
   DialogItemCheckButtonSetState(di, tmp_group_cfg.stick);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.stick));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Shading"));
   DialogItemCheckButtonSetState(di, tmp_group_cfg.shade);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.shade));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Mirror Shade/Iconify/Stick"));
   DialogItemCheckButtonSetState(di, tmp_group_cfg.mirror);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.mirror));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _(" Global settings: "));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Swap Window Locations"));
   DialogItemCheckButtonSetState(di, tmp_group_swap);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_swap));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureDefaultGroupSettings, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureDefaultGroupSettings, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureDefaultGroupSettings, 1);
   DialogSetExitFunction(d, CB_ConfigureDefaultGroupSettings, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureDefaultGroupSettings, 0, d);
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

void                CB_ApplyRemember(int val, void *data);
void
CB_ApplyRemember(int val, void *data)
{
/*   Snapshot           *sn; */
   int                 i;

   data = NULL;

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

void                CB_ApplyRememberEscape(int val, void *data);
void
CB_ApplyRememberEscape(int val, void *data)
{
   int                 i;

   DialogClose((Dialog *) data);
   val = 0;
   if (rd_ewin_list)
     {
	for (i = 0; rd_ewin_list[i]; i++)
	   Efree(rd_ewin_list[i]);
	Efree(rd_ewin_list);
	rd_ewin_list = 0;
     }
}

void                CB_RememberWindowSettings(int val, void *data);
void
CB_RememberWindowSettings(int val, void *data)
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
   DialogSetExitFunction(d, CB_ApplyRemember, 2, d);
   DialogBindKey(d, "Escape", CB_ApplyRememberEscape, 0, d);
   DialogBindKey(d, "Return", CB_ApplyRemember, 0, d);

   ShowDialog(d);
}
