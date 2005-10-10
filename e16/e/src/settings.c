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
#include "E.h"
#include <time.h>

static int          tmp_move;
static int          tmp_resize;
static int          tmp_geominfo;
static char         tmp_update_while_moving;

static void
CB_ConfigureMoveResize(Dialog * d, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.movres.mode_move = tmp_move;
	Conf.movres.mode_resize = tmp_resize;
	Conf.movres.mode_info = tmp_geominfo;
	Conf.movres.update_while_moving = tmp_update_while_moving;
     }
   if (val)
      if ((d = FindItem("CONFIGURE_MOVERESIZE_EXAMPLE", 0, LIST_FINDBY_NAME,
			LIST_TYPE_DIALOG)))
	{
	   DialogClose(d);
	}
   autosave();
}

void
SettingsMoveResize(void)
{
   Dialog             *d;
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
   tmp_update_while_moving = Conf.movres.update_while_moving;

   d = DialogCreate("CONFIGURE_MOVERESIZE");
   DialogSetTitle(d, _("Move & Resize Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemImageSetFile(di, "pix/moveres.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetText(di,
			  _("Enlightenment Move & Resize\n"
			    "Method Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
     }

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

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Update window while moving"));
   DialogItemCheckButtonSetPtr(di, &tmp_update_while_moving);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   DialogAddButton(d, _("OK"), CB_ConfigureMoveResize, 1, DIALOG_BUTTON_OK);
   DialogAddButton(d, _("Apply"), CB_ConfigureMoveResize, 0,
		   DIALOG_BUTTON_APPLY);
   DialogAddButton(d, _("Close"), CB_ConfigureMoveResize, 1,
		   DIALOG_BUTTON_CLOSE);
   DialogSetExitFunction(d, CB_ConfigureMoveResize, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureMoveResize, 0);
   ShowDialog(d);
}

static char         tmp_with_leader;
static char         tmp_switch_popup;
static char         tmp_manual_placement;
static char         tmp_manual_placement_mouse_pointer;
static char         tmp_map_slide;
static char         tmp_cleanup_slide;
static int          tmp_slide_mode;
static int          tmp_map_slide_speed;
static int          tmp_cleanup_slide_speed;
static char         tmp_animate_shading;
static int          tmp_shade_speed;
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

	Conf.place.slidein = tmp_map_slide;
	Conf.place.cleanupslide = tmp_cleanup_slide;
	Conf.place.slidemode = tmp_slide_mode;
	Conf.place.slidespeedmap = tmp_map_slide_speed;
	Conf.place.slidespeedcleanup = tmp_cleanup_slide_speed;

	Conf.animate_shading = tmp_animate_shading;
	Conf.shadespeed = tmp_shade_speed;

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
   DItem              *table, *di, *radio;

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

   tmp_map_slide = Conf.place.slidein;
   tmp_cleanup_slide = Conf.place.cleanupslide;
   tmp_slide_mode = Conf.place.slidemode;
   tmp_map_slide_speed = Conf.place.slidespeedmap;
   tmp_cleanup_slide_speed = Conf.place.slidespeedcleanup;

   tmp_animate_shading = Conf.animate_shading;
   tmp_shade_speed = Conf.shadespeed;

   tmp_place_ignore_struts = Conf.place.ignore_struts;
   tmp_raise_fullscreen = Conf.place.raise_fullscreen;

#ifdef HAS_XINERAMA
   tmp_extra_head = Conf.extra_head;
#endif

   d = DialogCreate("CONFIGURE_PLACEMENT");
   DialogSetTitle(d, _("Window Placement Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 1, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemImageSetFile(di, "pix/place.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetText(di,
			  _("Enlightenment Window Placement\n"
			    "Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
     }

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

#ifdef HAS_XINERAMA
   if (Mode.display.xinerama_active)
     {
	di = DialogAddItem(table, DITEM_CHECKBUTTON);
	DialogItemSetColSpan(di, 2);
	DialogItemSetText(di, _("Place windows on another head when full"));
	DialogItemCheckButtonSetPtr(di, &tmp_extra_head);
     }
#endif

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   DialogAddButton(d, _("OK"), CB_ConfigurePlacement, 1, DIALOG_BUTTON_OK);
   DialogAddButton(d, _("Apply"), CB_ConfigurePlacement, 0,
		   DIALOG_BUTTON_APPLY);
   DialogAddButton(d, _("Close"), CB_ConfigurePlacement, 1,
		   DIALOG_BUTTON_CLOSE);
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
	DialogItemImageSetFile(di, "pix/raise.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetText(di,
			  _("Enlightenment Automatic Raising\n"
			    "of Windows Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
     }

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

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   DialogAddButton(d, _("OK"), CB_ConfigureAutoraise, 1, DIALOG_BUTTON_OK);
   DialogAddButton(d, _("Apply"), CB_ConfigureAutoraise, 0,
		   DIALOG_BUTTON_APPLY);
   DialogAddButton(d, _("Close"), CB_ConfigureAutoraise, 1,
		   DIALOG_BUTTON_CLOSE);
   DialogSetExitFunction(d, CB_ConfigureAutoraise, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureAutoraise, 0);
   ShowDialog(d);
}

static char         tmp_dialog_headers;
static char         tmp_logout_dialog;
static char         tmp_reboot_halt;
static char         tmp_button_image;
static char         tmp_animate_startup;
static char         tmp_saveunders;

static void
CB_ConfigureMiscellaneous(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.dialogs.headers = tmp_dialog_headers;
	Conf.session.enable_logout_dialog = tmp_logout_dialog;
	Conf.session.enable_reboot_halt = tmp_reboot_halt;
	Conf.dialogs.button_image = tmp_button_image;
	Conf.startup.animate = tmp_animate_startup;
	Conf.save_under = tmp_saveunders;
     }
   autosave();
}

void
SettingsMiscellaneous(void)
{
   Dialog             *d;
   DItem              *table, *di;

   if ((d = FindItem("CONFIGURE_MISCELLANEOUS", 0, LIST_FINDBY_NAME,
		     LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_MISCELLANEOUS");

   tmp_dialog_headers = Conf.dialogs.headers;
   tmp_logout_dialog = Conf.session.enable_logout_dialog;
   tmp_reboot_halt = Conf.session.enable_reboot_halt;
   tmp_button_image = Conf.dialogs.button_image;
   tmp_animate_startup = Conf.startup.animate;
   tmp_saveunders = Conf.save_under;

   d = DialogCreate("CONFIGURE_MISCELLANEOUS");
   DialogSetTitle(d, _("Miscellaneous Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemImageSetFile(di, "pix/miscellaneous.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetText(di,
			  _("Enlightenment Miscellaneous\n"
			    "Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
     }

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Dialog Headers"));
   DialogItemCheckButtonSetPtr(di, &tmp_dialog_headers);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Button Images"));
   DialogItemCheckButtonSetPtr(di, &tmp_button_image);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Logout Dialog"));
   DialogItemCheckButtonSetPtr(di, &tmp_logout_dialog);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Reboot/Halt on Logout"));
   DialogItemCheckButtonSetPtr(di, &tmp_reboot_halt);

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

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   DialogAddButton(d, _("OK"), CB_ConfigureMiscellaneous, 1, DIALOG_BUTTON_OK);
   DialogAddButton(d, _("Apply"), CB_ConfigureMiscellaneous, 0,
		   DIALOG_BUTTON_APPLY);
   DialogAddButton(d, _("Close"), CB_ConfigureMiscellaneous, 1,
		   DIALOG_BUTTON_CLOSE);
   DialogSetExitFunction(d, CB_ConfigureMiscellaneous, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureMiscellaneous, 0);
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
	DialogItemImageSetFile(di, "pix/tips.png");
#endif
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 2);
	DialogItemSetText(di,
			  _("Enlightenment Composite\n" "Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
     }

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
   DialogItemSetText(di, _("Shadows Blurred"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);
   DialogItemRadioButtonGroupSetValPtr(radio, &Cfg_composite.shadow);

#if 0
   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Fading Speed:\n"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 300);
   DialogItemSliderSetUnits(di, 10);
   DialogItemSliderSetJump(di, 25);
   DialogItemSliderSetValPtr(di, &tmp_tooltiptime);
#endif

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   DialogAddButton(d, _("OK"), CB_ConfigureComposite, 1, DIALOG_BUTTON_OK);
   DialogAddButton(d, _("Apply"), CB_ConfigureComposite, 0,
		   DIALOG_BUTTON_APPLY);
   DialogAddButton(d, _("Close"), CB_ConfigureComposite, 1,
		   DIALOG_BUTTON_CLOSE);
   DialogSetExitFunction(d, CB_ConfigureComposite, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureComposite, 0);
   ShowDialog(d);
}
#endif
