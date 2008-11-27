/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
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
#include "dialog.h"
#include "settings.h"

static int          tmp_move;
static int          tmp_resize;
static int          tmp_geominfo;
static int          tmp_maximize;
static char         tmp_dragbar_nocover;
static char         tmp_avoid_server_grab;
static char         tmp_update_while_moving;
static char         tmp_sync_request;

static void
CB_ConfigureMoveResize(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.movres.mode_move = tmp_move;
	Conf.movres.mode_resize = tmp_resize;
	Conf.movres.mode_info = tmp_geominfo;
	Conf.movres.mode_maximize_default = tmp_maximize;
	Conf.movres.avoid_server_grab = tmp_avoid_server_grab;
	Conf.movres.update_while_moving = tmp_update_while_moving;
	Conf.movres.enable_sync_request = tmp_sync_request;
	Conf.movres.dragbar_nocover = tmp_dragbar_nocover;
     }
   autosave();
}

static void
_DlgFillMoveResize(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *radio1, *radio2, *radio3, *radio4;

   tmp_move = Conf.movres.mode_move;
   tmp_resize = Conf.movres.mode_resize;
   tmp_geominfo = Conf.movres.mode_info;
   tmp_maximize = Conf.movres.mode_maximize_default;
   tmp_avoid_server_grab = Conf.movres.avoid_server_grab;
   tmp_update_while_moving = Conf.movres.update_while_moving;
   tmp_sync_request = Conf.movres.enable_sync_request;
   tmp_dragbar_nocover = Conf.movres.dragbar_nocover;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Move Methods:"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Resize Methods:"));

   radio1 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Opaque"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio2 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Opaque"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Technical"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Technical"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Box"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Box"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Shaded"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Shaded"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Semi-Solid"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 4);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Semi-Solid"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 4);
   DialogItemRadioButtonGroupSetValPtr(radio2, &tmp_resize);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Translucent"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 5);
   DialogItemRadioButtonGroupSetValPtr(radio1, &tmp_move);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Avoid server grab"));
   DialogItemCheckButtonSetPtr(di, &tmp_avoid_server_grab);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Move/Resize Geometry Info Postion:"));

   radio3 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Window Center (O/T/B Methods)"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Always Screen corner"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Don't show"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 0);
   DialogItemRadioButtonGroupSetValPtr(radio3, &tmp_geominfo);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Default Resize Policy:"));

   radio4 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Conservative"));
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Available"));
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Absolute"));
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 0);
   DialogItemRadioButtonGroupSetValPtr(radio4, &tmp_maximize);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Update window while moving"));
   DialogItemCheckButtonSetPtr(di, &tmp_update_while_moving);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Synchronize move/resize with application"));
   DialogItemCheckButtonSetPtr(di, &tmp_sync_request);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Do not cover dragbar"));
   DialogItemCheckButtonSetPtr(di, &tmp_dragbar_nocover);
}

const DialogDef     DlgMoveResize = {
   "CONFIGURE_MOVERESIZE",
   N_("Move/Resize"),
   N_("Move & Resize Settings"),
   "SOUND_SETTINGS_MOVERESIZE",
   "pix/moveres.png",
   N_("Enlightenment Move & Resize\n" "Method Settings Dialog\n"),
   _DlgFillMoveResize,
   DLG_OAC, CB_ConfigureMoveResize,
};

static char         tmp_with_leader;
static char         tmp_switch_popup;
static char         tmp_manual_placement;
static char         tmp_manual_placement_mouse_pointer;
static char         tmp_center_if_desk_full;
static char         tmp_map_slide;
static char         tmp_cleanup_slide;
static int          tmp_slide_mode;
static int          tmp_map_slide_speed;
static int          tmp_cleanup_slide_speed;
static char         tmp_animate_shading;
static int          tmp_shade_speed;
static char         tmp_place_ignore_struts;
static char         tmp_raise_fullscreen;

#ifdef USE_XINERAMA
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
	Conf.place.center_if_desk_full = tmp_center_if_desk_full;

	Conf.place.slidein = tmp_map_slide;
	Conf.place.cleanupslide = tmp_cleanup_slide;
	Conf.place.slidemode = tmp_slide_mode;
	Conf.place.slidespeedmap = tmp_map_slide_speed;
	Conf.place.slidespeedcleanup = tmp_cleanup_slide_speed;

	Conf.shading.animate = tmp_animate_shading;
	Conf.shading.speed = tmp_shade_speed;

	Conf.place.ignore_struts = tmp_place_ignore_struts;
	Conf.place.raise_fullscreen = tmp_raise_fullscreen;
#ifdef USE_XINERAMA
	if (Mode.display.xinerama_active)
	   Conf.place.extra_head = tmp_extra_head;
#endif
     }
   autosave();
}

static void
_DlgFillPlacement(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *radio;

   tmp_with_leader = Conf.focus.transientsfollowleader;
   tmp_switch_popup = Conf.focus.switchfortransientmap;

   tmp_manual_placement = Conf.place.manual;
   tmp_manual_placement_mouse_pointer = Conf.place.manual_mouse_pointer;
   tmp_center_if_desk_full = Conf.place.center_if_desk_full;

   tmp_map_slide = Conf.place.slidein;
   tmp_cleanup_slide = Conf.place.cleanupslide;
   tmp_slide_mode = Conf.place.slidemode;
   tmp_map_slide_speed = Conf.place.slidespeedmap;
   tmp_cleanup_slide_speed = Conf.place.slidespeedcleanup;

   tmp_animate_shading = Conf.shading.animate;
   tmp_shade_speed = Conf.shading.speed;

   tmp_place_ignore_struts = Conf.place.ignore_struts;
   tmp_raise_fullscreen = Conf.place.raise_fullscreen;

#ifdef USE_XINERAMA
   tmp_extra_head = Conf.place.extra_head;
#endif

   DialogItemTableSetOptions(table, 2, 0, 1, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Dialog windows appear together with their owner"));
   DialogItemCheckButtonSetPtr(di, &tmp_with_leader);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Switch to desktop where dialog appears"));
   DialogItemCheckButtonSetPtr(di, &tmp_switch_popup);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Place windows manually"));
   DialogItemCheckButtonSetPtr(di, &tmp_manual_placement);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Place windows under mouse"));
   DialogItemCheckButtonSetPtr(di, &tmp_manual_placement_mouse_pointer);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Center windows when desk is full"));
   DialogItemCheckButtonSetPtr(di, &tmp_center_if_desk_full);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Slide windows in when they appear"));
   DialogItemCheckButtonSetPtr(di, &tmp_map_slide);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Slide windows around when cleaning up"));
   DialogItemCheckButtonSetPtr(di, &tmp_cleanup_slide);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Slide Method:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Opaque"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Technical"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Box"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Shaded"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Semi-Solid"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 4);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_slide_mode);

   di = DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Appear Slide speed:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetValPtr(di, &tmp_map_slide_speed);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Cleanup Slide speed:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetValPtr(di, &tmp_cleanup_slide_speed);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Animate shading and unshading of windows"));
   DialogItemCheckButtonSetPtr(di, &tmp_animate_shading);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Window Shading speed:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetValPtr(di, &tmp_shade_speed);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Ignore struts"));
   DialogItemCheckButtonSetPtr(di, &tmp_place_ignore_struts);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Raise fullscreen windows"));
   DialogItemCheckButtonSetPtr(di, &tmp_raise_fullscreen);

#ifdef USE_XINERAMA_no		/* Not implemented */
   if (Mode.display.xinerama_active)
     {
	di = DialogAddItem(table, DITEM_CHECKBUTTON);
	DialogItemSetColSpan(di, 2);
	DialogItemSetText(di, _("Place windows on another head when full"));
	DialogItemCheckButtonSetPtr(di, &tmp_extra_head);
     }
#endif
}

const DialogDef     DlgPlacement = {
   "CONFIGURE_PLACEMENT",
   N_("Placement"),
   N_("Window Placement Settings"),
   "SOUND_SETTINGS_PLACEMENT",
   "pix/place.png",
   N_("Enlightenment Window Placement\n" "Settings Dialog\n"),
   _DlgFillPlacement,
   DLG_OAC, CB_ConfigurePlacement,
};

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

static void
_DlgFillAutoraise(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di;

   tmp_autoraise = Conf.autoraise.enable;
   tmp_autoraisetime = Conf.autoraise.delay / 10;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Raise Windows Automatically"));
   DialogItemCheckButtonSetPtr(di, &tmp_autoraise);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Autoraise delay:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 300);
   DialogItemSliderSetUnits(di, 10);
   DialogItemSliderSetJump(di, 25);
   DialogItemSliderSetValPtr(di, &tmp_autoraisetime);
}

const DialogDef     DlgAutoraise = {
   "CONFIGURE_AUTORAISE",
   N_("Autoraise"),
   N_("Autoraise Settings"),
   "SOUND_SETTINGS_AUTORAISE",
   "pix/raise.png",
   N_("Enlightenment Automatic Raising\n" "of Windows Settings Dialog\n"),
   _DlgFillAutoraise,
   DLG_OAC, CB_ConfigureAutoraise,
};

static char         tmp_dialog_headers;
static char         tmp_button_image;
static char         tmp_animate_startup;
static char         tmp_saveunders;

static void
CB_ConfigureMiscellaneous(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.dialogs.headers = tmp_dialog_headers;
	Conf.dialogs.button_image = tmp_button_image;
	Conf.startup.animate = tmp_animate_startup;
	Conf.save_under = tmp_saveunders;
     }
   autosave();
}

static void
_DlgFillMisc(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di;

   tmp_dialog_headers = Conf.dialogs.headers;
   tmp_button_image = Conf.dialogs.button_image;
   tmp_animate_startup = Conf.startup.animate;
   tmp_saveunders = Conf.save_under;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Dialog Headers"));
   DialogItemCheckButtonSetPtr(di, &tmp_dialog_headers);

#if 0				/* Not functional */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Button Images"));
   DialogItemCheckButtonSetPtr(di, &tmp_button_image);
#endif

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable sliding startup windows"));
   DialogItemCheckButtonSetPtr(di, &tmp_animate_startup);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Use saveunders to reduce window exposures"));
   DialogItemCheckButtonSetPtr(di, &tmp_saveunders);
}

const DialogDef     DlgMisc = {
   "CONFIGURE_MISCELLANEOUS",
   N_("Miscellaneous"),
   N_("Miscellaneous Settings"),
   "SOUND_SETTINGS_MISCELLANEOUS",
   "pix/miscellaneous.png",
   N_("Enlightenment Miscellaneous\n" "Settings Dialog\n"),
   _DlgFillMisc,
   DLG_OAC, CB_ConfigureMiscellaneous,
};

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
   cfg = (cfg_composite *) DialogGetData(d);
   ECompMgrConfigSet(cfg);
   ECompMgrConfigGet(cfg);
}

static void
_DlgFillComposite(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   static cfg_composite Cfg_composite;
   DItem              *di, *radio;

   /* Get current settings */
   ECompMgrConfigGet(&Cfg_composite);
   DialogSetData(d, &Cfg_composite);

   /* Layout */
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Composite"));
   DialogItemCheckButtonSetPtr(di, &Cfg_composite.enable);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Fading"));
   DialogItemCheckButtonSetPtr(di, &Cfg_composite.fading);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Fading Speed:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 100);
   DialogItemSliderSetUnits(di, 5);
   DialogItemSliderSetJump(di, 5);
   DialogItemSliderSetValPtr(di, &Cfg_composite.fade_speed);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shadows Off"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shadows Sharp"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shadows Sharp2"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shadows Blurred"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);
   DialogItemRadioButtonGroupSetValPtr(radio, &Cfg_composite.shadow);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Default focused window opacity:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 100);
   DialogItemSliderSetUnits(di, 5);
   DialogItemSliderSetJump(di, 5);
   DialogItemSliderSetValPtr(di, &Cfg_composite.opacity_focused);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Default unfocused window opacity:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 100);
   DialogItemSliderSetUnits(di, 5);
   DialogItemSliderSetJump(di, 5);
   DialogItemSliderSetValPtr(di, &Cfg_composite.opacity_unfocused);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Default pop-up window opacity:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 100);
   DialogItemSliderSetUnits(di, 5);
   DialogItemSliderSetJump(di, 5);
   DialogItemSliderSetValPtr(di, &Cfg_composite.opacity_override);
}

const DialogDef     DlgComposite = {
   "CONFIGURE_COMPOSITE",
   N_("Composite"),
   N_("Composite Settings"),
   "SOUND_SETTINGS_COMPOSITE",
   "pix/pager.png",
   N_("Enlightenment Composite\n" "Settings Dialog\n"),
   _DlgFillComposite,
   DLG_OAC, CB_ConfigureComposite,
};
#endif

/*
 * Combined configuration dialog
 */

static const DialogDef *dialogs[] = {
   &DlgFocus,
   &DlgMoveResize,
   &DlgPlacement,
   &DlgDesks,
   &DlgAreas,
   &DlgPagers,
   &DlgMenus,
   &DlgAutoraise,
   &DlgTooltips,
#if HAVE_SOUND
   &DlgSound,
#endif
   &DlgGroupDefaults,
   &DlgRemember,
   &DlgFx,
   &DlgBackground,
#ifdef ENABLE_THEME_TRANSPARENCY
   &DlgThemeTrans,
#endif
#if USE_COMPOSITE
   &DlgComposite,
#endif
   &DlgSession,
   &DlgMisc,
};
#define N_CFG_DLGS (sizeof(dialogs)/sizeof(DialogDef*))

static void
CB_DlgSelect(Dialog * d, int val, void *data)
{
   const DialogDef    *dd = dialogs[val];
   DItem              *table = (DItem *) data;

   if (!table)
      return;
   if (!dd->fill)
      return;

   DialogCallExitFunction(d);

   DialogItemTableEmpty(table);
   DialogKeybindingsDestroy(d);

   DialogSetTitle(d, _(dd->title));
   DialogFill(d, table, dd, NULL);

   DialogArrange(d, 1);
}

static void
_DlgFillConfiguration(Dialog * d, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *buttons, *content;
   unsigned int        i;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   buttons = DialogAddItem(table, DITEM_TABLE);
   content = DialogAddItem(table, DITEM_TABLE);

   for (i = 0; i < N_CFG_DLGS; i++)
     {
	di = DialogAddItem(buttons, DITEM_BUTTON);
	DialogItemSetPadding(di, 2, 2, 0, 0);
	DialogItemSetText(di, _(dialogs[i]->label));
	DialogItemSetCallback(di, CB_DlgSelect, i, content);
     }

   DialogFill(d, content, dialogs[0], NULL);
}

static const DialogDef DlgConfiguration = {
   "CONFIGURE_ALL",
   NULL,
   N_("Enlightenment Settings"),
   "SOUND_SETTINGS_ALL",
   NULL,
   NULL,
   _DlgFillConfiguration,
   0, NULL,
};

void
IPC_Cfg(const char *params)
{
   unsigned int        i;
   const char         *name;

   if (!params || !params[0])
     {
	DialogShowSimple(&DlgConfiguration, NULL);
	return;
     }

   for (i = 0; i < N_CFG_DLGS; i++)
     {
	name = dialogs[i]->label;
	if (strcasecmp(params, name))
	   continue;
	DialogShowSimple(dialogs[i], NULL);
     }
}
