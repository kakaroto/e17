#include "E.h"

static void         CB_SettingsEscape(int val, void *data);
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
static DItem       *pager_scan_speed_label = NULL;
static Dialog      *pager_settings_dialog = NULL;

static void         CB_ConfigurePager(int val, void *data);
static void
CB_ConfigurePager(int val, void *data)
{
   if (val < 2)
     {
	if ((!mode.show_pagers) && (tmp_show_pagers))
	   EnableAllPagers();
	else if ((mode.show_pagers) && (!tmp_show_pagers))
	   DisableAllPagers();
	if (mode.pager_hiq != tmp_pager_hiq)
	   PagerSetHiQ(tmp_pager_hiq);
	mode.pager_zoom = tmp_pager_zoom;
	mode.pager_title = tmp_pager_title;
	if ((mode.pager_scanspeed != tmp_pager_scan_speed) ||
	    ((!tmp_pager_do_scan) && (mode.pager_scanspeed > 0)) ||
	    ((tmp_pager_do_scan) && (mode.pager_scanspeed == 0)))
	  {
	     if (tmp_pager_do_scan)
		mode.pager_scanspeed = tmp_pager_scan_speed;
	     else
		mode.pager_scanspeed = 0;
	     PagerSetSnap(tmp_pager_snap);
	  }
	if (mode.pager_snap != tmp_pager_snap)
	   PagerSetSnap(tmp_pager_snap);
     }
   autosave();
   data = NULL;
}

static void         CB_PagerScanSlide(int val, void *data);
static void
CB_PagerScanSlide(int val, void *data)
{
   char                s[256];

   Esnprintf(s, sizeof(s), "%s %03i %s", "Pager scanning speed:",
	     tmp_pager_scan_speed, "lines per second");
   DialogItemTextSetText(pager_scan_speed_label, s);
   DialogDrawItems(pager_settings_dialog, pager_scan_speed_label,
		   0, 0, 99999, 99999);
   val = 0;
   data = NULL;
}

void
SettingsPager(void)
{
   Dialog             *d;
   DItem              *table, *di;
   char                s[256];

   if ((d = FindItem("CONFIGURE_PAGER", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_PAGER");

   tmp_show_pagers = mode.show_pagers;
   tmp_pager_hiq = mode.pager_hiq;
   tmp_pager_snap = mode.pager_snap;
   tmp_pager_zoom = mode.pager_zoom;
   tmp_pager_title = mode.pager_title;
   if (mode.pager_scanspeed == 0)
      tmp_pager_do_scan = 0;
   else
      tmp_pager_do_scan = 1;
   tmp_pager_scan_speed = mode.pager_scanspeed;

   d = pager_settings_dialog = CreateDialog("CONFIGURE_PAGER");
   DialogSetTitle(d, "Pager Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/pager.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Desktop & Area\n"
 * "Pager Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 2);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Enable pager display");
   DialogItemCheckButtonSetState(di, tmp_show_pagers);
   DialogItemCheckButtonSetPtr(di, &tmp_show_pagers);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Make miniature snapshots of the screen");
   DialogItemCheckButtonSetState(di, tmp_pager_snap);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_snap);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Smooth high quality snapshots in snapshot mode");
   DialogItemCheckButtonSetState(di, tmp_pager_hiq);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_hiq);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Zoom in on pager windows when mouse is over them");
   DialogItemCheckButtonSetState(di, tmp_pager_zoom);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_zoom);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Pop up window title when mouse is over the window");
   DialogItemCheckButtonSetState(di, tmp_pager_title);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_title);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Continuously scan screen to update pager");
   DialogItemCheckButtonSetState(di, tmp_pager_do_scan);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_do_scan);

   di = pager_scan_speed_label = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   Esnprintf(s, sizeof(s), "%s %03i %s", "Pager scanning speed:",
	     tmp_pager_scan_speed, "lines per second");
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

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, "OK", CB_ConfigurePager, 1);
   DialogAddButton(d, "Apply", CB_ConfigurePager, 0);
   DialogAddButton(d, "Close", CB_ConfigurePager, 1);
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
static char         tmp_warp_after_focus;
static char         tmp_raise_after_focus;
static char         tmp_display_warp;
static char         tmp_clickalways;

static void         CB_ConfigureFocus(int val, void *data);
static void
CB_ConfigureFocus(int val, void *data)
{
   if (val < 2)
     {
	mode.focusmode = tmp_focus;
	mode.all_new_windows_get_focus = tmp_new_focus;
	mode.new_transients_get_focus = tmp_popup_focus;
	mode.new_transients_get_focus_if_group_focused = tmp_owner_popup_focus;
	mode.raise_on_next_focus = tmp_raise_focus;
	mode.warp_on_next_focus = tmp_warp_focus;
#ifdef WITH_TARTY_WARP
	mode.warp_after_next_focus = tmp_warp_after_focus;
	mode.raise_after_next_focus = tmp_raise_after_focus;
	mode.display_warp = tmp_display_warp;
#endif /* WITH_TARTY_WARP */
	mode.clickalways = tmp_clickalways;
	FixFocus();
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
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_FOCUS");

   tmp_focus = mode.focusmode;
   tmp_new_focus = mode.all_new_windows_get_focus;
   tmp_popup_focus = mode.new_transients_get_focus;
   tmp_owner_popup_focus = mode.new_transients_get_focus_if_group_focused;
   tmp_raise_focus = mode.raise_on_next_focus;
   tmp_warp_focus = mode.warp_on_next_focus;
#ifdef WITH_TARTY_WARP
   tmp_raise_after_focus = mode.raise_after_next_focus;
   tmp_warp_after_focus = mode.warp_after_next_focus;
   tmp_display_warp = mode.display_warp;
#endif /* WITH_TARTY_WARP */
   tmp_clickalways = mode.clickalways;

   d = CreateDialog("CONFIGURE_FOCUS");
   DialogSetTitle(d, "Focus Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/focus.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Focus\n"
 * "Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 2);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemRadioButtonSetText(di, "Focus follows pointer");
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemRadioButtonSetText(di, "Focus follows pointer sloppily");
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemRadioButtonSetText(di, "Focus follows mouse clicks");
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
   DialogItemCheckButtonSetText(di, "Clicking in a window always raises it");
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
   DialogItemCheckButtonSetText(di, "All new windows first get the focus");
   DialogItemCheckButtonSetState(di, tmp_new_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_new_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Only new dialog windows get the focus");
   DialogItemCheckButtonSetState(di, tmp_popup_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_popup_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Only new dialogs whose owner is focused get the focus");
   DialogItemCheckButtonSetState(di, tmp_owner_popup_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_owner_popup_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Raise windows on focus switch");
   DialogItemCheckButtonSetState(di, tmp_raise_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_raise_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Send mouse pointer to window on focus switch");
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
   DialogItemCheckButtonSetText(di, "Display and use focuslist");
   DialogItemCheckButtonSetState(di, tmp_display_warp);
   DialogItemCheckButtonSetPtr(di, &tmp_display_warp);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Raise windows after focus switch");
   DialogItemCheckButtonSetState(di, tmp_raise_after_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_raise_after_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Send mouse pointer to window after focus switch");
   DialogItemCheckButtonSetState(di, tmp_warp_after_focus);
   DialogItemCheckButtonSetPtr(di, &tmp_warp_after_focus);
#endif /* WITH_TARTY_WARP */

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, "OK", CB_ConfigureFocus, 1);
   DialogAddButton(d, "Apply", CB_ConfigureFocus, 0);
   DialogAddButton(d, "Close", CB_ConfigureFocus, 1);
   DialogSetExitFunction(d, CB_ConfigureFocus, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureFocus, 0, d);
   ShowDialog(d);
}

static int          tmp_move;
static int          tmp_resize;

static void         CB_ConfigureMoveResize(int val, void *data);
static void
CB_ConfigureMoveResize(int val, void *data)
{
   if (val < 2)
     {
	mode.movemode = tmp_move;
	mode.resizemode = tmp_resize;
     }
   autosave();
   data = NULL;
}

void
SettingsMoveResize(void)
{
   Dialog             *d;
   DItem              *table, *di, *radio1, *radio2;

   if ((d = FindItem("CONFIGURE_MOVERESIZE", 0, LIST_FINDBY_NAME,
		     LIST_TYPE_DIALOG)))
     {
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_MOVERESIZE");

   tmp_move = mode.movemode;
   tmp_resize = mode.resizemode;

   d = CreateDialog("CONFIGURE_MOVERESIZE");
   DialogSetTitle(d, "Move & Resize Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/moveres.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Move & Resize\n"
 * "Method Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 2);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, "Move Methods:");

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, "Resize Methods:");

   radio1 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Opaque");
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio2 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Opaque");
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Technical");
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Technical");
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Box");
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Box");
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Shaded");
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Shaded");
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Semi-Solid");
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 4);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Semi-Solid");
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 4);
   DialogItemRadioButtonGroupSetValPtr(radio2, &tmp_resize);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Translucent");
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 5);
   DialogItemRadioButtonGroupSetValPtr(radio1, &tmp_move);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, "OK", CB_ConfigureMoveResize, 1);
   DialogAddButton(d, "Apply", CB_ConfigureMoveResize, 0);
   DialogAddButton(d, "Close", CB_ConfigureMoveResize, 1);
   DialogSetExitFunction(d, CB_ConfigureMoveResize, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureMoveResize, 0, d);
   ShowDialog(d);
}

static int          tmp_desktops;
static DItem       *tmp_desk_text;
static Dialog      *tmp_desk_dialog;

static void         CB_ConfigureDesktops(int val, void *data);
static void
CB_ConfigureDesktops(int val, void *data)
{
   if (val < 2)
     {
	ChangeNumberOfDesktops(tmp_desktops);
     }
   autosave();
   data = NULL;
}

static void         CB_DesktopDisplayRedraw(int val, void *data);
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
		  SetBackgroundTo(id, pmap, desks.desk[i].bg, 0);
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
	EMoveWindow(disp, wins[i],
		    (i * (w - 64 - 2)) / num,
		    (i * (h - 48 - 2)) / num);
	EMapWindow(disp, wins[i]);
     }
   for (i = tmp_desktops; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
      EUnmapWindow(disp, wins[i]);
   if (tmp_desktops > 1)
      Esnprintf(s, sizeof(s), "%i Desktops", tmp_desktops);
   else
      Esnprintf(s, sizeof(s), "%i Desktop", tmp_desktops);
   DialogItemTextSetText(tmp_desk_text, s);
   DialogDrawItems(tmp_desk_dialog, tmp_desk_text,
		   0, 0, 99999, 99999);
}

void
SettingsDesktops(void)
{
   Dialog             *d;
   DItem              *table, *di, *area, *slider;
   char                s[64];

   if ((d = FindItem("CONFIGURE_DESKTOPS", 0, LIST_FINDBY_NAME,
		     LIST_TYPE_DIALOG)))
     {
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_DESKTOPS");

   tmp_desktops = mode.numdesktops;

   d = tmp_desk_dialog = CreateDialog("CONFIGURE_DESKTOPS");
   DialogSetTitle(d, "Multiple Desktop Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/desktops.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Multiple Desktop\n"
 * "Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 2);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemTextSetText(di, "Number of virtual desktops:\n");

   di = tmp_desk_text = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   if (tmp_desktops > 1)
      Esnprintf(s, sizeof(s), "%i Desktops", tmp_desktops);
   else
      Esnprintf(s, sizeof(s), "%i Desktop", tmp_desktops);
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

   DialogAddButton(d, "OK", CB_ConfigureDesktops, 1);
   DialogAddButton(d, "Apply", CB_ConfigureDesktops, 0);
   DialogAddButton(d, "Close", CB_ConfigureDesktops, 1);
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

static void         CB_ConfigureAreas(int val, void *data);
static void
CB_ConfigureAreas(int val, void *data)
{
   if (val < 2)
     {
	SetNewAreaSize(tmp_area_x, 9 - tmp_area_y);
	if (tmp_edge_flip)
	  {
	     if (tmp_edge_resist < 1)
		tmp_edge_resist = 1;
	     mode.edge_flip_resistance = tmp_edge_resist;
	  }
	else
	   mode.edge_flip_resistance = 0;
	ShowEdgeWindows();
     }
   autosave();
   data = NULL;
}

static void         CB_AreaDisplayRedraw(int val, void *data);
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
	Pixmap              pmap;

	ic = FindItem("SETTINGS_AREA_AREA", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	   IclassApply(ic, win, w, h, 0, 0, STATE_NORMAL, 0);
	awin = ECreateWindow(win, 0, 0, 18, 14, 0);
	ic = FindItem("SETTINGS_AREADESK_AREA", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	  {
	     IclassApplyCopy(ic, awin, 18, 14, 0, 0, STATE_NORMAL, &pmap, NULL);
	     ESetWindowBackgroundPixmap(disp, awin, pmap);
	     Imlib_free_pixmap(id, pmap);
	  }
	XClearWindow(disp, awin);
	called = 1;
     }
   EMoveResizeWindow(disp, awin,
		     ((w / 2) - (9 * tmp_area_x)),
		     ((h / 2) - (7 * (9 - tmp_area_y))),
		     18 * tmp_area_x,
		     14 * (9 - tmp_area_y));
   EMapWindow(disp, awin);

   if ((tmp_area_x > 1) || ((9 - tmp_area_y) > 1))
      Esnprintf(s, sizeof(s), "%i x %i\nScreens in size", tmp_area_x,
		9 - tmp_area_y);
   else
      Esnprintf(s, sizeof(s), "1\nScreen in size");
   DialogItemTextSetText(tmp_area_text, s);
   DialogDrawItems(tmp_area_dialog, tmp_area_text,
		   0, 0, 99999, 99999);
}

void
SettingsArea(void)
{
   Dialog             *d;
   DItem              *table, *di, *area, *slider, *slider2, *table2;
   char                s[64];

   if ((d = FindItem("CONFIGURE_AREA", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_AREA");

   tmp_edge_resist = mode.edge_flip_resistance;
   if (tmp_edge_resist == 0)
      tmp_edge_flip = 0;
   else
      tmp_edge_flip = 1;
   GetAreaSize(&tmp_area_x, &tmp_area_y);
   tmp_area_y = 9 - tmp_area_y;

   d = tmp_area_dialog = CreateDialog("CONFIGURE_AREA");
   DialogSetTitle(d, "Virtual Deskop Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 1, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_NONE);
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/areas.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Virtual Desktop\n"
 * "Size Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 3);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemTextSetText(di, "Virtual Desktop size:\n");

   di = tmp_area_text = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   if ((tmp_area_x > 1) || (tmp_area_y > 1))
      Esnprintf(s, sizeof(s), "%i x %i\nScreens in size", tmp_area_x,
		9 - tmp_area_y);
   else
      Esnprintf(s, sizeof(s), "1\nScreen in size");
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
   DialogItemCheckButtonSetText(di, "Enable edge flip");
   DialogItemCheckButtonSetState(di, tmp_edge_flip);
   DialogItemCheckButtonSetPtr(di, &tmp_edge_flip);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemTextSetText(di, "Resistance at edge of screen:\n");

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

   DialogAddButton(d, "OK", CB_ConfigureAreas, 1);
   DialogAddButton(d, "Apply", CB_ConfigureAreas, 0);
   DialogAddButton(d, "Close", CB_ConfigureAreas, 1);
   DialogSetExitFunction(d, CB_ConfigureAreas, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureAreas, 0, d);
   ShowDialog(d);
   CB_AreaDisplayRedraw(1, area);
}

static char         tmp_with_leader;
static char         tmp_switch_popup;
static char         tmp_manual_placement;

static void         CB_ConfigurePlacement(int val, void *data);
static void
CB_ConfigurePlacement(int val, void *data)
{
   if (val < 2)
     {
	mode.transientsfollowleader = tmp_with_leader;
	mode.switchfortransientmap = tmp_switch_popup;
	mode.manual_placement = tmp_manual_placement;
     }
   autosave();
   data = NULL;
}

void
SettingsPlacement(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d = FindItem("CONFIGURE_PLACEMENT", 0, LIST_FINDBY_NAME,
		     LIST_TYPE_DIALOG)))
     {
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_PLACEMENT");

   tmp_with_leader = mode.transientsfollowleader;
   tmp_switch_popup = mode.switchfortransientmap;
   tmp_manual_placement = mode.manual_placement;

   d = CreateDialog("CONFIGURE_PLACEMENT");
   DialogSetTitle(d, "Window Placement Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/place.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Window Placement\n"
 * "Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 2);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Dialog windows appear together with their owner");
   DialogItemCheckButtonSetState(di, tmp_with_leader);
   DialogItemCheckButtonSetPtr(di, &tmp_with_leader);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Switch to desktop where dialog appears");
   DialogItemCheckButtonSetState(di, tmp_switch_popup);
   DialogItemCheckButtonSetPtr(di, &tmp_switch_popup);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Place windows manually");
   DialogItemCheckButtonSetState(di, tmp_manual_placement);
   DialogItemCheckButtonSetPtr(di, &tmp_manual_placement);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, "OK", CB_ConfigurePlacement, 1);
   DialogAddButton(d, "Apply", CB_ConfigurePlacement, 0);
   DialogAddButton(d, "Close", CB_ConfigurePlacement, 1);
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

static void         CB_ConfigureAutoraise(int val, void *data);
static void
CB_ConfigureAutoraise(int val, void *data)
{
   if (val < 2)
     {
	mode.autoraise = tmp_autoraise;
	mode.autoraisetime = ((double)tmp_autoraisetime) / 100;
     }
   autosave();
   data = NULL;
}

void
SettingsAutoRaise(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d = FindItem("CONFIGURE_AUTORAISE", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_AUTORAISE");

   tmp_autoraise = mode.autoraise;
   tmp_autoraisetime = (int)(mode.autoraisetime * 100);

   d = CreateDialog("CONFIGURE_AUTORAISE");
   DialogSetTitle(d, "Autoraise Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/raise.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Automatic Raising\n"
 * "of Windows Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 2);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Raise Windows Automatically");
   DialogItemCheckButtonSetState(di, tmp_autoraise);
   DialogItemCheckButtonSetPtr(di, &tmp_autoraise);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, "Autoraise delay:\n");

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

   DialogAddButton(d, "OK", CB_ConfigureAutoraise, 1);
   DialogAddButton(d, "Apply", CB_ConfigureAutoraise, 0);
   DialogAddButton(d, "Close", CB_ConfigureAutoraise, 1);
   DialogSetExitFunction(d, CB_ConfigureAutoraise, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureAutoraise, 0, d);
   ShowDialog(d);
}

static char         tmp_tooltips;
static int          tmp_tooltiptime;

static void         CB_ConfigureTooltips(int val, void *data);
static void
CB_ConfigureTooltips(int val, void *data)
{
   if (val < 2)
     {
	mode.tooltips = tmp_tooltips;
	mode.tiptime = ((double)tmp_tooltiptime) / 100;
     }
   autosave();
   data = NULL;
}

void
SettingsTooltips(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d = FindItem("CONFIGURE_TOOLTIPS", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_TOOLTIPS");

   tmp_tooltips = mode.tooltips;
   tmp_tooltiptime = (int)(mode.tiptime * 100);

   d = CreateDialog("CONFIGURE_TOOLTIPS");
   DialogSetTitle(d, "Tooltip Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/tips.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Tooltip\n"
 * "Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 2);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Display tooltips");
   DialogItemCheckButtonSetState(di, tmp_tooltips);
   DialogItemCheckButtonSetPtr(di, &tmp_tooltips);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, "Tooltip delay:\n");

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

   DialogAddButton(d, "OK", CB_ConfigureTooltips, 1);
   DialogAddButton(d, "Apply", CB_ConfigureTooltips, 0);
   DialogAddButton(d, "Close", CB_ConfigureTooltips, 1);
   DialogSetExitFunction(d, CB_ConfigureTooltips, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureTooltips, 0, d);
   ShowDialog(d);
}

static char         tmp_kde;
static void         CB_ConfigureKDE(int val, void *data);
static void
CB_ConfigureKDE(int val, void *data)
{
   if (val < 2)
     {
	if (tmp_kde != mode.kde_support)
	  {
	     mode.kde_support = tmp_kde;
	     if (mode.kde_support)
		KDE_Init();
	     else if (!mode.kde_support)
		KDE_Shutdown();
	  }
     }
   autosave();
   data = NULL;
}

void
SettingsKDE(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d = FindItem("CONFIGURE_KDE", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	ShowDialog(d);
	return;
     }

   tmp_kde = mode.kde_support;

   d = CreateDialog("CONFIGURE_KDE");
   DialogSetTitle(d, "KDE Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Enable KDE Support");
   DialogItemCheckButtonSetState(di, tmp_kde);
   DialogItemCheckButtonSetPtr(di, &tmp_kde);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, "OK", CB_ConfigureKDE, 1);
   DialogAddButton(d, "Apply", CB_ConfigureKDE, 0);
   DialogAddButton(d, "Close", CB_ConfigureKDE, 1);
   DialogSetExitFunction(d, CB_ConfigureKDE, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureKDE, 0, d);
   ShowDialog(d);
}

static char         tmp_audio;

static void         CB_ConfigureAudio(int val, void *data);
static void
CB_ConfigureAudio(int val, void *data)
{
   if (val < 2)
     {
	mode.sound = tmp_audio;
	if ((mode.sound) && (sound_fd < 0))
	   SoundInit();
	else if ((!mode.sound) && (sound_fd >= 0))
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
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_AUDIO");

   tmp_audio = mode.sound;

   d = CreateDialog("CONFIGURE_AUDIO");
   DialogSetTitle(d, "Audio Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/sound.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Audio\n"
 * "Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 2);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Enable sounds");
   DialogItemCheckButtonSetState(di, tmp_audio);
   DialogItemCheckButtonSetPtr(di, &tmp_audio);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, "OK", CB_ConfigureAudio, 1);
   DialogAddButton(d, "Apply", CB_ConfigureAudio, 0);
   DialogAddButton(d, "Close", CB_ConfigureAudio, 1);
   DialogSetExitFunction(d, CB_ConfigureAudio, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureAudio, 0, d);
   ShowDialog(d);
}

static char         tmp_saveunders;
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

static void         CB_ConfigureFX(int val, void *data);
static void
CB_ConfigureFX(int val, void *data)
{
   if (val < 2)
     {
	mode.save_under = tmp_saveunders;
	mode.menuslide = tmp_animated_menus;
	mode.menusonscreen = tmp_menusonscreen;
	mode.mapslide = tmp_map_slide;
	mode.cleanupslide = tmp_cleanup_slide;
	desks.slidein = tmp_desktop_slide;
	mode.animate_shading = tmp_animate_shading;
	mode.shadespeed = tmp_shade_speed;
	mode.slidemode = tmp_slide_mode;
	mode.slidespeedmap = tmp_map_slide_speed;
	mode.slidespeedcleanup = tmp_cleanup_slide_speed;
	desks.slidespeed = tmp_desktop_slide_speed;
	if ((desks.dragdir != tmp_dragdir) ||
	    ((tmp_dragbar) && (desks.dragbar_width < 1)) ||
	    ((!tmp_dragbar) && (desks.dragbar_width > 0)))
	  {
	     Button             *b;

	     if (tmp_dragbar)
		desks.dragbar_width = 16;
	     else
		desks.dragbar_width = -1;
	     desks.dragdir = tmp_dragdir;
	     while ((b = RemoveItem("_DESKTOP_DRAG_CONTROL", 0,
				    LIST_FINDBY_NAME,
				    LIST_TYPE_BUTTON)))
		DestroyButton(b);
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
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_FX");

   tmp_saveunders = mode.save_under;
   tmp_animated_menus = mode.menuslide;
   tmp_menusonscreen = mode.menusonscreen;
   tmp_map_slide = mode.mapslide;
   tmp_cleanup_slide = mode.cleanupslide;
   tmp_desktop_slide = desks.slidein;
   tmp_animate_shading = mode.animate_shading;
   if (desks.dragbar_width < 1)
      tmp_dragbar = 0;
   else
      tmp_dragbar = 1;
   tmp_dragdir = desks.dragdir;
   tmp_slide_mode = mode.slidemode;
   tmp_map_slide_speed = mode.slidespeedmap;
   tmp_shade_speed = mode.shadespeed;
   tmp_cleanup_slide_speed = mode.slidespeedcleanup;
   tmp_desktop_slide_speed = desks.slidespeed;

   d = CreateDialog("CONFIGURE_FX");
   DialogSetTitle(d, "Special FX Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 4, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/fx.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Special Effects\n"
 * "Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_NONE);
 * 
 * di = DialogAddItem(table, DITEM_NONE);
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 4);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemTextSetText(di, "Slide Method:");

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 3);
   DialogItemCheckButtonSetText(di, "Slide desktops around when changing");
   DialogItemCheckButtonSetState(di, tmp_desktop_slide);
   DialogItemCheckButtonSetPtr(di, &tmp_desktop_slide);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Opaque");
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 3);
   DialogItemCheckButtonSetText(di, "Slide windows around when cleaning up");
   DialogItemCheckButtonSetState(di, tmp_cleanup_slide);
   DialogItemCheckButtonSetPtr(di, &tmp_cleanup_slide);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Technical");
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 3);
   DialogItemCheckButtonSetText(di, "Slide windows in when they appear");
   DialogItemCheckButtonSetState(di, tmp_map_slide);
   DialogItemCheckButtonSetPtr(di, &tmp_map_slide);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Box");
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 3);
   DialogItemCheckButtonSetText(di, "Animated display of menus");
   DialogItemCheckButtonSetState(di, tmp_animated_menus);
   DialogItemCheckButtonSetPtr(di, &tmp_animated_menus);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Shaded");
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 3);
   DialogItemCheckButtonSetText(di, "Always pop up menus on screen");
   DialogItemCheckButtonSetState(di, tmp_menusonscreen);
   DialogItemCheckButtonSetPtr(di, &tmp_menusonscreen);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Semi-Solid");
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 4);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_slide_mode);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 4);
   DialogItemCheckButtonSetText(di, "Use saveunders to reduce window exposures");
   DialogItemCheckButtonSetState(di, tmp_saveunders);
   DialogItemCheckButtonSetPtr(di, &tmp_saveunders);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 4);
   DialogItemCheckButtonSetText(di, "Animate shading and unshading of windows");
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
   DialogItemCheckButtonSetText(di, "Display desktop dragbar");
   DialogItemCheckButtonSetState(di, tmp_dragbar);
   DialogItemCheckButtonSetPtr(di, &tmp_dragbar);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 1);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, "Drag bar position:");

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_NONE);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Top");
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Bottom");
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Left");
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Right");
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
   DialogItemTextSetText(di, "Desktop Slide speed: (slow)\n");

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
   DialogItemTextSetText(di, "(fast)\n");

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, "Cleanup Slide speed: (slow)\n");

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
   DialogItemTextSetText(di, "(fast)\n");

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, "Appear Slide speed: (slow)\n");

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
   DialogItemTextSetText(di, "(fast)\n");

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, "Window Shading speed: (slow)\n");

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
   DialogItemTextSetText(di, "(fast)\n");

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 4);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, "OK", CB_ConfigureFX, 1);
   DialogAddButton(d, "Apply", CB_ConfigureFX, 0);
   DialogAddButton(d, "Close", CB_ConfigureFX, 1);
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

static void         BG_RedrawView(char nuke_old);

static void         CB_ConfigureBG(int val, void *data);
static void
CB_ConfigureBG(int val, void *data)
{
   int                 i;

   if (val < 2)
     {
	mode.desktop_bg_timeout = tmp_bg_timeout;
	desks.hiqualitybg = tmp_hiq;
	mode.user_bg = tmp_userbg;
	tmp_bg->bg.solid.r = tmp_bg_r;
	tmp_bg->bg.solid.g = tmp_bg_g;
	tmp_bg->bg.solid.b = tmp_bg_b;
	tmp_bg->bg.tile = tmp_bg_tile;
	tmp_bg->bg.keep_aspect = tmp_bg_keep_aspect;
	tmp_bg->bg.xjust = tmp_bg_xjust;
	tmp_bg->bg.yjust = 1024 - tmp_bg_yjust;
	tmp_bg->bg.xperc = tmp_bg_xperc;
	tmp_bg->bg.yperc = 1024 - tmp_bg_yperc;
	if (!tmp_bg_image)
	   RemoveImagesFromBG(tmp_bg);
	if (tmp_bg->pmap)
	   Imlib_free_pixmap(id, tmp_bg->pmap);
	tmp_bg->pmap = 0;
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
	   ImlibImage         *im;
	   Pixmap              p2;

	   Esnprintf(s, sizeof(s), "%s/cached/bgsel/%s", UserEDir(), tmp_bg->name);
	   p2 = ECreatePixmap(disp, root.win, 64, 48, root.depth);
	   SetBackgroundTo(id, p2, tmp_bg, 0);
	   im = Imlib_create_image_from_drawable(id, p2, 0, 0, 0, 64, 48);
	   Imlib_save_image_to_ppm(id, im, s);
	   Imlib_kill_image(id, im);
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
   data = NULL;
}

static void         CB_DesktopMiniDisplayRedraw(int val, void *data);
static void
CB_DesktopMiniDisplayRedraw(int val, void *data)
{
   int                 w, h;
   DItem              *di;
   Window              win;
   Pixmap              pmap;
   ImlibColor          icl;

   di = (DItem *) data;
   win = DialogItemAreaGetWindow(di);
   DialogItemAreaGetSize(di, &w, &h);

   icl.r = tmp_bg_r;
   icl.g = tmp_bg_g;
   icl.b = tmp_bg_b;
   if (!tbg)
     {
	if (!tmp_bg_image)
	   tbg = CreateDesktopBG("TEMP", &icl, NULL,
				 tmp_bg_tile, tmp_bg_keep_aspect,
				 tmp_bg_xjust, 1024 - tmp_bg_yjust,
				 tmp_bg_xperc, 1024 - tmp_bg_yperc,
				 tmp_bg->top.file, tmp_bg->top.keep_aspect,
				 tmp_bg->top.xjust, tmp_bg->top.yjust,
				 tmp_bg->top.xperc, tmp_bg->top.yperc);
	else
	   tbg = CreateDesktopBG("TEMP", &icl, tmp_bg->bg.file,
				 tmp_bg_tile, tmp_bg_keep_aspect,
				 tmp_bg_xjust, 1024 - tmp_bg_yjust,
				 tmp_bg_xperc, 1024 - tmp_bg_yperc,
				 tmp_bg->top.file, tmp_bg->top.keep_aspect,
				 tmp_bg->top.xjust, tmp_bg->top.yjust,
				 tmp_bg->top.xperc, tmp_bg->top.yperc);
     }
   else
     {
	if (tbg->pmap)
	   Imlib_free_pixmap(id, tbg->pmap);
	tbg->bg.solid.r = tmp_bg_r;
	tbg->bg.solid.g = tmp_bg_g;
	tbg->bg.solid.b = tmp_bg_b;
	if (tbg->bg.file)
	   Efree(tbg->bg.file);
	tbg->bg.file = NULL;
	if (tmp_bg_image)
	   tbg->bg.file = duplicate(tmp_bg->bg.file);
	else
	  {
	     if (tbg->bg.im)
		Imlib_destroy_image(id, tbg->bg.im);
	     tbg->bg.im = NULL;
	  }
	if (tbg->bg.real_file)
	   Efree(tbg->bg.real_file);
	tbg->bg.real_file = NULL;
	tbg->bg.tile = tmp_bg_tile;
	tbg->bg.keep_aspect = tmp_bg_keep_aspect;
	tbg->bg.xjust = tmp_bg_xjust;
	tbg->bg.yjust = 1024 - tmp_bg_yjust;
	tbg->bg.xperc = tmp_bg_xperc;
	tbg->bg.yperc = 1024 - tmp_bg_yperc;
     }
   KeepBGimages(tbg, 1);
   pmap = ECreatePixmap(disp, win, w, h, root.depth);
   ESetWindowBackgroundPixmap(disp, win, pmap);
   SetBackgroundTo(id, pmap, tbg, 0);
   XClearWindow(disp, win);
   EFreePixmap(disp, pmap);
   val = 0;
}

static void         CB_ConfigureNewBG(int val, void *data);
static void
CB_ConfigureNewBG(int val, void *data)
{
   char                s[1024];
   ImlibColor          icl;
   Background         *bg;

   Esnprintf(s, sizeof(s), "__NEWBG_%i\n", time(NULL));
   icl.r = tmp_bg_r;
   icl.g = tmp_bg_g;
   icl.b = tmp_bg_b;
   bg = CreateDesktopBG(s, &icl, tmp_bg->bg.file,
			tmp_bg_tile, tmp_bg_keep_aspect,
			tmp_bg_xjust, 1024 - tmp_bg_yjust,
			tmp_bg_xperc, 1024 - tmp_bg_yperc,
			tmp_bg->top.file, tmp_bg->top.keep_aspect,
			tmp_bg->top.xjust, tmp_bg->top.yjust,
			tmp_bg->top.xperc, tmp_bg->top.yperc);
   AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);
   tmp_bg = bg;
   desks.desk[desks.current].bg = bg;
   bg_sel_slider->item.slider.upper += 4;
   bg_sel_slider->item.slider.val = 0;
   if (bg_sel_slider->item.slider.val_ptr)
      *(bg_sel_slider->item.slider.val_ptr) = 0;
   DialogDrawItems(bg_sel_dialog, bg_sel_slider,
		   0, 0, 99999, 99999);
   RefreshCurrentDesktop();
   RedrawPagersForDesktop(desks.current, 2);
   ForceUpdatePagersForDesktop(desks.current);
   BG_RedrawView(0);
   autosave();
   val = 0;
   data = NULL;
}

static void         CB_ConfigureRemBG(int val, void *data);
static void
CB_ConfigureRemBG(int val, void *data)
{
   Background        **bglist, *bg;
   int                 i, num;

   bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   if ((bglist) && (num > 1))
     {
	bg_sel_slider->item.slider.upper -= 4;
	if (bg_sel_slider->item.slider.val > bg_sel_slider->item.slider.upper)
	  {
	     bg_sel_slider->item.slider.val = bg_sel_slider->item.slider.upper;
	     if (bg_sel_slider->item.slider.val_ptr)
		*(bg_sel_slider->item.slider.val_ptr) = bg_sel_slider->item.slider.val;
	  }
	for (i = 0; i < num; i++)
	  {
	     if (bglist[i] == tmp_bg)
	       {
		  bg = RemoveItem((char *)tmp_bg, 0, LIST_FINDBY_POINTER, LIST_TYPE_BACKGROUND);
		  if (i < (num - 1))
		     tmp_bg = bglist[i + 1];
		  else
		     tmp_bg = bglist[i - 1];
		  i = num;
		  if (bg)
		     FreeDesktopBG(bg);
		  if (tmp_bg->bg.file)
		     tmp_bg_image = 1;
		  else
		     tmp_bg_image = 0;
		  KeepBGimages(tmp_bg, 1);

		  {
		     char               *stmp;
		     char                s[1024];

		     if (tmp_bg->bg.file)
			stmp = fullfileof(tmp_bg->bg.file);
		     else
			stmp = duplicate("-NONE-");
		     Esnprintf(s, sizeof(s), "Background definition information:\nName: %s\nFile: %s\n",
			       tmp_bg->name,
			       stmp);
		     Efree(stmp);
		     DialogItemTextSetText(bg_filename, s);
		     DialogDrawItems(bg_sel_dialog, bg_filename,
				     0, 0, 99999, 99999);
		  }
		  tmp_bg_r = tmp_bg->bg.solid.r;
		  tmp_bg_g = tmp_bg->bg.solid.g;
		  tmp_bg_b = tmp_bg->bg.solid.b;
		  tmp_bg_tile = tmp_bg->bg.tile;
		  tmp_bg_keep_aspect = tmp_bg->bg.keep_aspect;
		  tmp_bg_xjust = tmp_bg->bg.xjust;
		  tmp_bg_yjust = 1024 - tmp_bg->bg.yjust;
		  tmp_bg_xperc = tmp_bg->bg.xperc;
		  tmp_bg_yperc = 1024 - tmp_bg->bg.yperc;
		  tmp_w[0]->item.slider.val = tmp_bg_r;
		  tmp_w[1]->item.check_button.onoff = tmp_bg_image;
		  tmp_w[2]->item.slider.val = tmp_bg_g;
		  tmp_w[3]->item.check_button.onoff = tmp_bg_keep_aspect;
		  tmp_w[4]->item.slider.val = tmp_bg_b;
		  tmp_w[5]->item.check_button.onoff = tmp_bg_tile;
		  tmp_w[6]->item.slider.val = tmp_bg_xjust;
		  tmp_w[7]->item.slider.val = tmp_bg_yjust;
		  tmp_w[8]->item.slider.val = tmp_bg_yperc;
		  tmp_w[9]->item.slider.val = tmp_bg_xperc;
		  if (tbg)
		    {
		       FreeDesktopBG(tbg);
		       tbg = NULL;
		    }
		  CB_DesktopMiniDisplayRedraw(0, bg_mini_disp);
	       }
	  }
	desks.desk[desks.current].bg = tmp_bg;
	RedrawPagersForDesktop(desks.current, 2);
	ForceUpdatePagersForDesktop(desks.current);
	RefreshCurrentDesktop();
	BG_RedrawView(0);
	for (i = 0; i < 10; i++)
	   DialogDrawItems(bg_sel_dialog, tmp_w[i],
			   0, 0, 99999, 99999);
	autosave();
     }
   if (bglist)
      Efree(bglist);
   autosave();
   val = 0;
   data = NULL;
}

static void         CB_ConfigureDelBG(int val, void *data);
static void
CB_ConfigureDelBG(int val, void *data)
{
   Background        **bglist, *bg;
   int                 i, num;

   bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   if ((bglist) && (num > 1))
     {
	bg_sel_slider->item.slider.upper -= 4;
	if (bg_sel_slider->item.slider.val > bg_sel_slider->item.slider.upper)
	  {
	     bg_sel_slider->item.slider.val = bg_sel_slider->item.slider.upper;
	     if (bg_sel_slider->item.slider.val_ptr)
		*(bg_sel_slider->item.slider.val_ptr) = bg_sel_slider->item.slider.val;
	  }
	for (i = 0; i < num; i++)
	  {
	     if (bglist[i] == tmp_bg)
	       {
		  bg = RemoveItem((char *)tmp_bg, 0, LIST_FINDBY_POINTER, LIST_TYPE_BACKGROUND);
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
				 rm(f);
				 Efree(f);
			      }
			 }
		       if (bg->top.file)
			 {
			    char               *f;

			    f = FindFile(bg->top.file);
			    if (f)
			      {
				 rm(f);
				 Efree(f);
			      }
			 }
		       FreeDesktopBG(bg);
		    }
		  if (tmp_bg->bg.file)
		     tmp_bg_image = 1;
		  else
		     tmp_bg_image = 0;
		  KeepBGimages(tmp_bg, 1);

		  {
		     char               *stmp;
		     char                s[1024];

		     if (tmp_bg->bg.file)
			stmp = fullfileof(tmp_bg->bg.file);
		     else
			stmp = duplicate("-NONE-");
		     Esnprintf(s, sizeof(s), "Background definition information:\nName: %s\nFile: %s\n",
			       tmp_bg->name,
			       stmp);
		     Efree(stmp);
		     DialogItemTextSetText(bg_filename, s);
		     DialogDrawItems(bg_sel_dialog, bg_filename,
				     0, 0, 99999, 99999);
		  }
		  tmp_bg_r = tmp_bg->bg.solid.r;
		  tmp_bg_g = tmp_bg->bg.solid.g;
		  tmp_bg_b = tmp_bg->bg.solid.b;
		  tmp_bg_tile = tmp_bg->bg.tile;
		  tmp_bg_keep_aspect = tmp_bg->bg.keep_aspect;
		  tmp_bg_xjust = tmp_bg->bg.xjust;
		  tmp_bg_yjust = 1024 - tmp_bg->bg.yjust;
		  tmp_bg_xperc = tmp_bg->bg.xperc;
		  tmp_bg_yperc = 1024 - tmp_bg->bg.yperc;
		  tmp_w[0]->item.slider.val = tmp_bg_r;
		  tmp_w[1]->item.check_button.onoff = tmp_bg_image;
		  tmp_w[2]->item.slider.val = tmp_bg_g;
		  tmp_w[3]->item.check_button.onoff = tmp_bg_keep_aspect;
		  tmp_w[4]->item.slider.val = tmp_bg_b;
		  tmp_w[5]->item.check_button.onoff = tmp_bg_tile;
		  tmp_w[6]->item.slider.val = tmp_bg_xjust;
		  tmp_w[7]->item.slider.val = tmp_bg_yjust;
		  tmp_w[8]->item.slider.val = tmp_bg_yperc;
		  tmp_w[9]->item.slider.val = tmp_bg_xperc;
		  if (tbg)
		    {
		       FreeDesktopBG(tbg);
		       tbg = NULL;
		    }
		  CB_DesktopMiniDisplayRedraw(0, bg_mini_disp);
	       }
	  }
	desks.desk[desks.current].bg = tmp_bg;
	RedrawPagersForDesktop(desks.current, 2);
	ForceUpdatePagersForDesktop(desks.current);
	RefreshCurrentDesktop();
	BG_RedrawView(0);
	for (i = 0; i < 10; i++)
	   DialogDrawItems(bg_sel_dialog, tmp_w[i],
			   0, 0, 99999, 99999);
	autosave();
     }
   if (bglist)
      Efree(bglist);
   autosave();
   val = 0;
   data = NULL;
}

static void         CB_ConfigureFrontBG(int val, void *data);
static void
CB_ConfigureFrontBG(int val, void *data)
{
   int                 i;

   MoveItemToListTop(tmp_bg, LIST_TYPE_BACKGROUND);
   CB_DesktopMiniDisplayRedraw(0, bg_mini_disp);
   BG_RedrawView(0);
   for (i = 0; i < 10; i++)
      DialogDrawItems(bg_sel_dialog, tmp_w[i],
		      0, 0, 99999, 99999);
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

   win = DialogItemAreaGetWindow(bg_sel);
   DialogItemAreaGetSize(bg_sel, &w, &h);
   bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
   if (bglist)
     {
	int                 x;
	Pixmap              pmap;
	GC                  gc;
	XGCValues           gcv;

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
		  ImlibImage         *im;
		  char                s[4096];

		  ic = FindItem("DIALOG_BUTTON", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		  if (ic)
		    {
		       Pixmap              pbg;

		       if (i == tmp_bg_selected)
			  IclassApplyCopy(ic, pmap, 64 + 8, 48 + 8, 0, 0, STATE_CLICKED, &pbg, NULL);
		       else
			  IclassApplyCopy(ic, pmap, 64 + 8, 48 + 8, 0, 0, STATE_NORMAL, &pbg, NULL);
		       XCopyArea(disp, pbg, pmap, gc, 0, 0, 64 + 8, 48 + 8, x, 0);
		       Imlib_free_pixmap(id, pbg);
		    }
		  if (!strcmp(bglist[i]->name, "NONE"))
		    {
		       TextClass          *tc;

		       tc = FindItem("DIALOG", 0, LIST_FINDBY_NAME,
				     LIST_TYPE_TCLASS);
		       if (tc)
			 {
			    int                 tw, th;

			    TextSize(tc, 0, 0, STATE_NORMAL, "No\nBackground",
				     &tw, &th, 17);
			    TextDraw(tc, pmap, 0, 0, STATE_NORMAL,
				     "No\nBackground", x + 4,
				     4 + ((48 - th) / 2), 64, 48, 17, 512);
			 }
		    }
		  else
		    {
		       Esnprintf(s, sizeof(s), "cached/bgsel/%s", bglist[i]->name);
		       im = ELoadImage(s);
		       if (!im)
			 {
			    Esnprintf(s, sizeof(s), "%s/cached/bgsel/%s", UserEDir(), bglist[i]->name);
			    p2 = ECreatePixmap(disp, pmap, 64, 48, id->x.depth);
			    SetBackgroundTo(id, p2, bglist[i], 0);
			    XCopyArea(disp, p2, pmap, gc, 0, 0, 64, 48, x + 4, 4);
			    im = Imlib_create_image_from_drawable(id, p2, 0, 0, 0, 64, 48);
			    Imlib_save_image_to_ppm(id, im, s);
			    Imlib_kill_image(id, im);
			    EFreePixmap(disp, p2);
			 }
		       else
			 {
			    if (nuke_old)
			      {
				 Imlib_changed_image(id, im);
				 Imlib_kill_image(id, im);
				 im = ELoadImage(s);
			      }
			    if (im)
			      {
				 Imlib_paste_image(id, im, pmap, x + 4, 4, 64, 48);
				 Imlib_destroy_image(id, im);
			      }
			 }
		    }
	       }
	     x += (64 + 8);
	  }
	XFreeGC(disp, gc);
	EFreePixmap(disp, pmap);
	Efree(bglist);
     }
   XClearWindow(disp, win);
}

static void         CB_BGAreaSlide(int val, void *data);
static void
CB_BGAreaSlide(int val, void *data)
{
   BG_RedrawView(0);
   val = 0;
   data = NULL;
}

static void         CB_BGScan(int val, void *data);
static void
CB_BGScan(int val, void *data)
{
   AUDIO_PLAY("SOUND_WAIT");

   bg_sel_slider->item.slider.val = bg_sel_slider->item.slider.lower;
   if (bg_sel_slider->item.slider.val_ptr)
      *(bg_sel_slider->item.slider.val_ptr) = bg_sel_slider->item.slider.val;

   while (bg_sel_slider->item.slider.val <= bg_sel_slider->item.slider.upper)
     {
	DialogDrawItems(bg_sel_dialog, bg_sel_slider,
			0, 0, 99999, 99999);
	if (bg_sel_slider->func)
	   (bg_sel_slider->func) (bg_sel_slider->val, bg_sel_slider->data);
	bg_sel_slider->item.slider.val += 8;
	if (bg_sel_slider->item.slider.val_ptr)
	   *(bg_sel_slider->item.slider.val_ptr) = bg_sel_slider->item.slider.val;
     }
   val = 0;
   data = NULL;
}

static void         CB_BGAreaEvent(int val, void *data);
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
	     if (tmp_bg->bg.file)
		tmp_bg_image = 1;
	     else
		tmp_bg_image = 0;
	     KeepBGimages(tmp_bg, 1);

	     {
		char               *stmp;
		char                s[1024];

		if (tmp_bg->bg.file)
		   stmp = fullfileof(tmp_bg->bg.file);
		else
		   stmp = duplicate("-NONE-");
		Esnprintf(s, sizeof(s), "Background definition information:\nName: %s\nFile: %s\n",
			  tmp_bg->name,
			  stmp);
		Efree(stmp);
		DialogItemTextSetText(bg_filename, s);
		DialogDrawItems(bg_sel_dialog, bg_filename,
				0, 0, 99999, 99999);
	     }
	     tmp_bg_r = tmp_bg->bg.solid.r;
	     tmp_bg_g = tmp_bg->bg.solid.g;
	     tmp_bg_b = tmp_bg->bg.solid.b;
	     tmp_bg_tile = tmp_bg->bg.tile;
	     tmp_bg_keep_aspect = tmp_bg->bg.keep_aspect;
	     tmp_bg_xjust = tmp_bg->bg.xjust;
	     tmp_bg_yjust = 1024 - tmp_bg->bg.yjust;
	     tmp_bg_xperc = tmp_bg->bg.xperc;
	     tmp_bg_yperc = 1024 - tmp_bg->bg.yperc;
	     tmp_w[0]->item.slider.val = tmp_bg_r;
	     tmp_w[1]->item.check_button.onoff = tmp_bg_image;
	     tmp_w[2]->item.slider.val = tmp_bg_g;
	     tmp_w[3]->item.check_button.onoff = tmp_bg_keep_aspect;
	     tmp_w[4]->item.slider.val = tmp_bg_b;
	     tmp_w[5]->item.check_button.onoff = tmp_bg_tile;
	     tmp_w[6]->item.slider.val = tmp_bg_xjust;
	     tmp_w[7]->item.slider.val = tmp_bg_yjust;
	     tmp_w[8]->item.slider.val = tmp_bg_yperc;
	     tmp_w[9]->item.slider.val = tmp_bg_xperc;
	     if (tbg)
	       {
		  FreeDesktopBG(tbg);
		  tbg = NULL;
	       }
	     CB_DesktopMiniDisplayRedraw(0, bg_mini_disp);
	     desks.desk[desks.current].bg = tmp_bg;
	     RedrawPagersForDesktop(desks.current, 2);
	     ForceUpdatePagersForDesktop(desks.current);
	     RefreshCurrentDesktop();
	     BG_RedrawView(0);
	     for (x = 0; x < 10; x++)
		DialogDrawItems(bg_sel_dialog, tmp_w[x],
				0, 0, 99999, 99999);
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

static void         CB_DesktopTimeout(int val, void *data);
static void
CB_DesktopTimeout(int val, void *data)
{
   DItem              *di;
   char                s[256];

   di = (DItem *) data;
   Esnprintf(s, sizeof(s), "Unused backgrounds freed after %2i:%02i:%02i",
	     tmp_bg_timeout / 3600,
	     (tmp_bg_timeout / 60) - (60 * (tmp_bg_timeout / 3600)),
	     (tmp_bg_timeout) - (60 * (tmp_bg_timeout / 60)));
   DialogItemTextSetText(di, s);
   DialogDrawItems(bg_sel_dialog, di,
		   0, 0, 99999, 99999);
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
		  bg_sel_slider->item.slider.val = (4 * i);
		  if (bg_sel_slider->item.slider.lower >
		      bg_sel_slider->item.slider.val)
		     bg_sel_slider->item.slider.val =
			bg_sel_slider->item.slider.lower;
		  if (bg_sel_slider->item.slider.upper <
		      bg_sel_slider->item.slider.val)
		     bg_sel_slider->item.slider.val =
			bg_sel_slider->item.slider.upper;
		  if (bg_sel_slider->item.slider.val_ptr)
		     *(bg_sel_slider->item.slider.val_ptr) = bg_sel_slider->item.slider.val;
		  DialogDrawItems(bg_sel_dialog, bg_sel_slider,
				  0, 0, 99999, 99999);
		  if (bg_sel_slider->func)
		     (bg_sel_slider->func) (bg_sel_slider->val, bg_sel_slider->data);
		  tmp_bg_selected = i;
		  KeepBGimages(tmp_bg, 0);
		  tmp_bg = bglist[tmp_bg_selected];
		  if (tmp_bg->bg.file)
		     tmp_bg_image = 1;
		  else
		     tmp_bg_image = 0;
		  KeepBGimages(tmp_bg, 1);

		  {
		     char               *stmp;
		     char                s[1024];

		     if (tmp_bg->bg.file)
			stmp = fullfileof(tmp_bg->bg.file);
		     else
			stmp = duplicate("-NONE-");
		     Esnprintf(s, sizeof(s), "Background definition information:\nName: %s\nFile: %s\n",
			       tmp_bg->name,
			       stmp);
		     Efree(stmp);
		     DialogItemTextSetText(bg_filename, s);
		     DialogDrawItems(bg_sel_dialog, bg_filename,
				     0, 0, 99999, 99999);
		  }
		  tmp_bg_r = tmp_bg->bg.solid.r;
		  tmp_bg_g = tmp_bg->bg.solid.g;
		  tmp_bg_b = tmp_bg->bg.solid.b;
		  tmp_bg_tile = tmp_bg->bg.tile;
		  tmp_bg_keep_aspect = tmp_bg->bg.keep_aspect;
		  tmp_bg_xjust = tmp_bg->bg.xjust;
		  tmp_bg_yjust = 1024 - tmp_bg->bg.yjust;
		  tmp_bg_xperc = tmp_bg->bg.xperc;
		  tmp_bg_yperc = 1024 - tmp_bg->bg.yperc;
		  tmp_w[0]->item.slider.val = tmp_bg_r;
		  tmp_w[1]->item.check_button.onoff = tmp_bg_image;
		  tmp_w[2]->item.slider.val = tmp_bg_g;
		  tmp_w[3]->item.check_button.onoff = tmp_bg_keep_aspect;
		  tmp_w[4]->item.slider.val = tmp_bg_b;
		  tmp_w[5]->item.check_button.onoff = tmp_bg_tile;
		  tmp_w[6]->item.slider.val = tmp_bg_xjust;
		  tmp_w[7]->item.slider.val = tmp_bg_yjust;
		  tmp_w[8]->item.slider.val = tmp_bg_yperc;
		  tmp_w[9]->item.slider.val = tmp_bg_xperc;
		  if (tbg)
		    {
		       FreeDesktopBG(tbg);
		       tbg = NULL;
		    }
		  CB_DesktopMiniDisplayRedraw(0, bg_mini_disp);
		  BG_RedrawView(0);
		  for (x = 0; x < 10; x++)
		     DialogDrawItems(bg_sel_dialog, tmp_w[x],
				     0, 0, 99999, 99999);
		  Efree(bglist);
		  tmp_bg_selected = -1;
		  return;
	       }
	  }
	Efree(bglist);
     }
}

static void         CB_BGPrev(int val, void *data);
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

static void         CB_BGNext(int val, void *data);
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

static int          BG_SortFileCompare(Background * bg1, Background * bg2);
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

static void         CB_BGSortFile(int val, void *data);
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

static void         CB_BGSortAttrib(int val, void *data);
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
	     if ((bg) && (bg->bg.tile) && (bg->bg.xperc == 0)
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

static void         CB_BGSortContent(int val, void *data);
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
	   RemoveItem((char *)(bglist[i]), 0, LIST_FINDBY_POINTER, LIST_TYPE_BACKGROUND);
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

void
SettingsBackground(Background * bg)
{
   Dialog             *d;
   DItem              *table, *di, *table2, *area, *slider, *slider2, *label;
   DItem              *w1, *w2, *w3, *w4, *w5, *w6;
   int                 num;
   char                s[256];

   if ((d = FindItem("CONFIGURE_BG", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_BG");

   if ((!bg) || ((bg) && (!strcmp(bg->name, "NONE"))))
     {
	char                s[1024];

	Esnprintf(s, sizeof(s), "__NEWBG_%i\n", time(NULL));
	bg = CreateDesktopBG(s, NULL, NULL, 1, 1, 0, 0, 0, 0, NULL, 1, 512, 512, 0, 0);
	AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);
	desks.desk[desks.current].bg = bg;
     }
   tmp_bg = bg;
   if (bg->bg.file)
      tmp_bg_image = 1;
   else
      tmp_bg_image = 0;
   tmp_bg->keepim = 1;

   tmp_bg_r = bg->bg.solid.r;
   tmp_bg_g = bg->bg.solid.g;
   tmp_bg_b = bg->bg.solid.b;
   tmp_bg_tile = bg->bg.tile;
   tmp_bg_keep_aspect = bg->bg.keep_aspect;
   tmp_bg_xjust = bg->bg.xjust;
   tmp_bg_yjust = 1024 - bg->bg.yjust;
   tmp_bg_xperc = bg->bg.xperc;
   tmp_bg_yperc = 1024 - bg->bg.yperc;
   tmp_hiq = desks.hiqualitybg;
   tmp_userbg = mode.user_bg;
   tmp_bg_timeout = mode.desktop_bg_timeout;

   d = bg_sel_dialog = CreateDialog("CONFIGURE_BG");
   DialogSetTitle(d, "Desktop Background Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 3, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetColSpan(di, 2);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/bg.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Desktop\n"
 * "Background Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 3);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemTextSetText(di, "BG Color\n");

   di = DialogAddItem(table, DITEM_NONE);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemSetPadding(table2, 2, 2, 2, 2);
   DialogItemTableSetOptions(table2, 4, 0, 0, 0);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemButtonSetText(di, "Move to Front\n");
   DialogItemSetCallback(di, CB_ConfigureFrontBG, 0, NULL);
   DialogBindKey(d, "F", CB_ConfigureFrontBG, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemButtonSetText(di, "Duplicate\n");
   DialogItemSetCallback(di, CB_ConfigureNewBG, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemButtonSetText(di, "Unlist\n");
   DialogItemSetCallback(di, CB_ConfigureRemBG, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemButtonSetText(di, "Delete File\n");
   DialogItemSetCallback(di, CB_ConfigureDelBG, 0, NULL);
   DialogBindKey(d, "D", CB_ConfigureDelBG, 0, NULL);
   DialogBindKey(d, "Delete", CB_ConfigureDelBG, 0, NULL);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, "Red:\n");

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
   DialogItemCheckButtonSetText(di, "Use background image");
   DialogItemCheckButtonSetState(di, tmp_bg_image);
   DialogItemCheckButtonSetPtr(di, &tmp_bg_image);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, "Green:\n");

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
   DialogItemCheckButtonSetText(di, "Keep aspect on scale");
   DialogItemCheckButtonSetState(di, tmp_bg_keep_aspect);
   DialogItemCheckButtonSetPtr(di, &tmp_bg_keep_aspect);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemTextSetText(di, "Blue:\n");

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
   DialogItemCheckButtonSetText(di, "Tile image across background");
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
			 "Background\n"
			 "Image\n"
			 "Scaling\n"
			 "and\n"
			 "Alignment\n");

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
   DialogItemCheckButtonSetText(di, "Use dithering in Hi-Color");
   DialogItemCheckButtonSetState(di, tmp_hiq);
   DialogItemCheckButtonSetPtr(di, &tmp_hiq);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, "Background overrides theme");
   DialogItemCheckButtonSetState(di, tmp_userbg);
   DialogItemCheckButtonSetPtr(di, &tmp_userbg);

   di = label = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 512, 512);
   Esnprintf(s, sizeof(s), "Unused backgrounds freed after %2i:%02i:%02i",
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

   di = table2 = DialogAddItem(table, DITEM_TABLE);
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
   DialogItemButtonSetText(di, "Prescan BG's");
   DialogItemSetCallback(di, CB_BGScan, 0, NULL);

   di = table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemSetPadding(table2, 2, 2, 2, 2);
   DialogItemTableSetOptions(table2, 3, 0, 0, 0);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemButtonSetText(di, "Sort by File");
   DialogItemSetCallback(di, CB_BGSortFile, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemButtonSetText(di, "Sort by Attr.");
   DialogItemSetCallback(di, CB_BGSortAttrib, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemButtonSetText(di, "Sort by Image");
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
	 stmp = duplicate("-NONE-");
      Esnprintf(s, sizeof(s), "Background definition information:\nName: %s\nFile: %s\n",
		tmp_bg->name,
		stmp);
      Efree(stmp);
      DialogItemTextSetText(bg_filename, s);
   }

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 3);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, "OK", CB_ConfigureBG, 1);
   DialogAddButton(d, "Apply", CB_ConfigureBG, 0);
   DialogAddButton(d, "Close", CB_ConfigureBG, 1);
   DialogSetExitFunction(d, CB_ConfigureBG, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureBG, 0, d);
   ShowDialog(d);

   CB_DesktopMiniDisplayRedraw(1, area);
   BG_RedrawView(0);
   BGSettingsGoTo(tmp_bg);
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

static void         CB_ConfigureIconbox(int val, void *data);
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

   if ((d = FindItem("CONFIGURE_ICONBOX", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_ICONBOX");

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
   if (tmp_ib_name)
      Efree(tmp_ib_name);
   tmp_ib_name = duplicate(name);

   d = CreateDialog("CONFIGURE_ICONBOX");
   DialogSetTitle(d, "Iconbox Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 1, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/iconbox.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetColSpan(di, 2);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Iconbox\n"
 * "Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 3);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, "Transparent background");
   DialogItemCheckButtonSetState(di, tmp_ib_nobg);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_nobg);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, "Hide border around inner Iconbox");
   DialogItemCheckButtonSetState(di, tmp_ib_cover_hide);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_cover_hide);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, "Show icon names");
   DialogItemCheckButtonSetState(di, tmp_ib_shownames);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_shownames);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, "Draw base image behind Icons");
   DialogItemCheckButtonSetState(di, tmp_ib_draw_icon_base);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_draw_icon_base);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, "Hide scrollbar when not needed");
   DialogItemCheckButtonSetState(di, tmp_ib_scrollbar_hide);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_scrollbar_hide);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, "Automatically resize to fit Icons");
   DialogItemCheckButtonSetState(di, tmp_ib_auto_resize);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_auto_resize);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 0);
   DialogItemTextSetText(di, "Alignment of anchoring when automatically resizing:");

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
   DialogItemTextSetText(di, "Icon image display policy (if one operation fails, try the next):");

   radio4 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Snapshot Windows, Use application icon, Use Enlightenment Icon");
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Use application icon, Use Enlightenment Icon, Snapshot Window");
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Use Enlightenment Icon, Snapshot Window");
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
   DialogItemTextSetText(di, "Icon size");

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
   DialogItemTextSetText(di, "Orientation:");

   di = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, "Scrollbar side:");

   di = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, "Scrollbar arrows:");

   radio1 = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Horizontal");
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio2 = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Left / Top");
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio3 = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Start");
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Vertical");
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio1, &tmp_ib_vert);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Right / Bottom");
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio2, &tmp_ib_side);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "Both ends");
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table2, DITEM_NONE);
   di = DialogAddItem(table2, DITEM_NONE);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "End");
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table2, DITEM_NONE);
   di = DialogAddItem(table2, DITEM_NONE);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, "None");
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio3, &tmp_ib_arrows);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, "OK", CB_ConfigureIconbox, 1);
   DialogAddButton(d, "Apply", CB_ConfigureIconbox, 0);
   DialogAddButton(d, "Close", CB_ConfigureIconbox, 1);
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

static void         CB_ConfigureGroupEscape(int val, void *data);
static void
CB_ConfigureGroupEscape(int val, void *data)
{
   if (tmp_cfgs)
     {
	ShowHideWinGroups(tmp_ewin, tmp_groups[tmp_current_group], SET_OFF);
	Efree(tmp_cfgs);
     }
   DialogClose((Dialog *) data);
   val = 0;
}

static void         CB_ConfigureGroup(int val, void *data);
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
	DIALOG_OK("Window Group Error",
	    "\n  This window currently does not belong to any groups.  \n\n");
	return;
     }
   if ((d = FindItem("CONFIGURE_GROUP", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	AUDIO_PLAY("GROUP_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_GROUP");

   tmp_ewin = ewin;
   tmp_groups = ewin->groups;
   tmp_cfgs = (GroupConfig *) Emalloc(ewin->num_groups * sizeof(GroupConfig));
   tmp_current_group = 0;
   group_member_strings = GetWinGroupMemberNames(ewin->groups, ewin->num_groups);
   ShowHideWinGroups(ewin, ewin->groups[0], SET_ON);

   for (i = 0; i < ewin->num_groups; i++)
      CopyGroupConfig(&(ewin->groups[i]->cfg), &(tmp_cfgs[i]));

   d = CreateDialog("CONFIGURE_GROUP");
   DialogSetTitle(d, "Window Group Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/place.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Window Group\n"
 * "Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 2);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, "   Pick the group to configure:   ");

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
			 "  The following actions are  \n"
			 "  applied to all group members:  ");

   di_border = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_BORDER, &(tmp_cfg.set_border));
   DialogItemCheckButtonSetText(di, "Changing Border Style");
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].set_border);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.set_border));

   di_iconify = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_ICONIFY, &(tmp_cfg.iconify));
   DialogItemCheckButtonSetText(di, "Iconifying");
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].iconify);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.iconify));

   di_kill = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_KILL, &(tmp_cfg.kill));
   DialogItemCheckButtonSetText(di, "Killing");
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].kill);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.kill));

   di_move = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_MOVE, &(tmp_cfg.move));
   DialogItemCheckButtonSetText(di, "Moving");
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].move);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.move));

   di_raise = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_RAISE, &(tmp_cfg.raise));
   DialogItemCheckButtonSetText(di, "Raising/Lowering");
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].raise);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.raise));

   di_stick = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_STICK, &(tmp_cfg.stick));
   DialogItemCheckButtonSetText(di, "Sticking");
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].stick);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.stick));

   di_shade = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_SHADE, &(tmp_cfg.shade));
   DialogItemCheckButtonSetText(di, "Shading");
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].shade);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.shade));

   di_mirror = di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, &GroupFeatureChangeCallback, GROUP_FEATURE_MIRROR, &(tmp_cfg.mirror));
   DialogItemCheckButtonSetText(di, "Mirror Shade/Iconify/Stick");
   DialogItemCheckButtonSetState(di, tmp_cfgs[0].mirror);
   DialogItemCheckButtonSetPtr(di, &(tmp_cfg.mirror));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, "OK", CB_ConfigureGroup, 1);
   DialogAddButton(d, "Apply", CB_ConfigureGroup, 0);
   DialogAddButton(d, "Close", CB_ConfigureGroup, 1);
   DialogSetExitFunction(d, CB_ConfigureGroup, 2, d);
   DialogBindKey(d, "Escape", CB_ConfigureGroupEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureGroup, 0, d);

   for (i = 0; i < ewin->num_groups; i++)
      Efree(group_member_strings[i]);
   Efree(group_member_strings);

   ShowDialog(d);
}

static GroupConfig  tmp_group_cfg;

static void         CB_ConfigureDefaultGroupSettings(int val, void *data);
static void
CB_ConfigureDefaultGroupSettings(int val, void *data)
{
   if (val < 2)
     {
	CopyGroupConfig(&tmp_group_cfg, &(mode.group_config));
     }
   autosave();
   data = NULL;
}

void
SettingsDefaultGroupControl(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d = FindItem("CONFIGURE_DEFAULT_GROUP_CONTROL", 0, LIST_FINDBY_NAME,
		     LIST_TYPE_DIALOG)))
     {
	AUDIO_PLAY("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   AUDIO_PLAY("SOUND_SETTINGS_GROUP");

   CopyGroupConfig(&(mode.group_config), &tmp_group_cfg);

   d = CreateDialog("CONFIGURE_DEFAULT_GROUP_CONTROL");
   DialogSetTitle(d, "Default Group Control Settings");

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);
/*
 * di = DialogAddItem(table, DITEM_IMAGE);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemImageSetFile(di, "pix/place.png");
 * 
 * di = DialogAddItem(table, DITEM_TEXT);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemTextSetText(di,
 * "Enlightenment Default\n"
 * "Group Control Settings Dialog\n");
 * 
 * di = DialogAddItem(table, DITEM_SEPARATOR);
 * DialogItemSetColSpan(di, 2);
 * DialogItemSetPadding(di, 2, 2, 2, 2);
 * DialogItemSetFill(di, 1, 0);
 * DialogItemSeparatorSetOrientation(di, 0);
 */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Changing Border Style");
   DialogItemCheckButtonSetState(di, tmp_group_cfg.set_border);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.set_border));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Iconifying");
   DialogItemCheckButtonSetState(di, tmp_group_cfg.iconify);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.iconify));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Killing");
   DialogItemCheckButtonSetState(di, tmp_group_cfg.kill);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.kill));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Moving");
   DialogItemCheckButtonSetState(di, tmp_group_cfg.move);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.move));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Raising/Lowering");
   DialogItemCheckButtonSetState(di, tmp_group_cfg.raise);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.raise));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Sticking");
   DialogItemCheckButtonSetState(di, tmp_group_cfg.stick);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.stick));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Shading");
   DialogItemCheckButtonSetState(di, tmp_group_cfg.shade);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.shade));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, "Mirror Shade/Iconify/Stick");
   DialogItemCheckButtonSetState(di, tmp_group_cfg.mirror);
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.mirror));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, "OK", CB_ConfigureDefaultGroupSettings, 1);
   DialogAddButton(d, "Apply", CB_ConfigureDefaultGroupSettings, 0);
   DialogAddButton(d, "Close", CB_ConfigureDefaultGroupSettings, 1);
   DialogSetExitFunction(d, CB_ConfigureDefaultGroupSettings, 2, d);
   DialogBindKey(d, "Escape", CB_SettingsEscape, 0, d);
   DialogBindKey(d, "Return", CB_ConfigureDefaultGroupSettings, 0, d);
   ShowDialog(d);
}
