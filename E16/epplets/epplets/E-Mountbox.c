/* E-Mountbox.c
 *
 * Copyright (C) 1999-2000 Christian Kreibich
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */
#include "epplet.h"
#include "config.h"
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

#define FSTAB      "/etc/fstab"
#define PROCMOUNTS "/proc/mounts"
#define ETCMTAB    "/etc/mtab"

#define __BG_IMAGE "E-Mountbox-bg.png"
#define __DEFAULT  "E-Mountbox-blockdev.png"

ConfigItem          defaults[] = {
   {"BG_IMAGE", "E-Mountbox-bg.png"}
   ,
   {"DEFAULT", "E-Mountbox-blockdev.png"}
   ,
   {"EJECT_MODE", "2"}
   ,
   {"DO_POLL", "1"}
   ,
   {"POLLINTVAL", "5"}
};

char               *default_types[] = {
   "cd   E-Mountbox-cd.png",
   "fd   E-Mountbox-floppy.png",
   "zip  E-Mountbox-zip.png",
   "jazz E-Mountbox-jazz.png"
};

typedef enum eject_mode
{
   NO_EJECT,
   MANUAL_EJECT,
   AUTO_EJECT
}
EjectMode;

typedef struct _tile Tile;
typedef struct _mountpointtype MountPointType;
typedef struct _mode Mode;

struct _mode
{
   EjectMode           eject_mode;
   int                 do_polling;
   int                 polling_interval;
   int                 anim_mount;
   int                 show_buttons;
};

struct _mountpointtype
{
   char               *key;
   char               *imagefile;
   Imlib_Image        *image;
   MountPointType     *next;
   MountPointType     *prev;
};

typedef struct _mountpoint
{
   char               *device;
   char               *path;
   char                mounted;
}
MountPoint;

struct _tile
{
   Imlib_Image        *image;
   MountPoint         *mountpoint;
   Tile               *prev;
   Tile               *next;
};

Mode                mode;
Tile               *tiles = NULL;
Tile               *current_tile = NULL;
int                 current_tile_index = 0;
int                 num_tiles = 0;
MountPointType     *types = NULL;
int                 num_types = 0;
Imlib_Image        *bg_image = NULL;
Imlib_Image        *default_image = NULL;
Epplet_gadget       action_area, button_close, button_config, button_help;

/* stuff for the config win */
Epplet_gadget       tbox_key, tbox_file, tbox_default, tbox_bg;
Epplet_gadget       arrow_left, arrow_right, button_add, button_del,
   button_add_long;
Epplet_gadget       label_key, label_file, label_interval, label_slider,
   hslider_interval;
Epplet_gadget       togglebutton_no_eject, togglebutton_manual_eject,
   togglebutton_auto_eject;
int                 ejectbuttons[3];
Window              config_win = 0;
MountPointType     *current_type = NULL;
RGB_buf             window_buf = NULL;	/* the currently displayed mountpoint */
RGB_buf             widescreen_buf = NULL;	/* the wide image of all mountpoints */
RGB_buf             widescreen_canvas_buf = NULL;	/* only the background */

/* graphx handling */
int                 IsTransparent(Imlib_Image * im, int x, int y);
void                UpdateView(int dir, int fast);
void                FreeImages(void);
void                UpdateGraphics(void);
static void         ConfigUpdateDialog(void *data);

/* mount handling */
void                SetupMounts(void);
void                FreeMounts(void);
void                AddMountPoint(char *device, char *path);
void                FreeMountPointTypes(void);
void                AddMountPointType(char *key, char *image);
void                ModifyMountPointType(MountPointType * mpt, char *key,
					 char *imagefile);
void                DeleteMountPointType(MountPointType * mpt);
void                Mount(MountPoint * mp);
void                Umount(MountPoint * mp);
void                UpdatePolling(void);

/* miscellaneous nitty gritty */
int                 ParseFstab(void);
int                 ParseProcMounts(void);
int                 ParseEtcMtab(void);
void                VisitMountPoints(void);
MountPoint         *FindMountPointByClick(int x, int y);
void                SyncConfigs(void);

/* callbacks/ handlers */
/*
static void     CallbackEnter(void *data, Window w);
static void     CallbackLeave(void *data, Window w);
*/
static void         CallbackExpose(void *data, Window win, int x, int y, int w,
				   int h);
static void         CallbackButtonUp(void *data, Window win, int x, int y,
				     int b);
static void         CallbackExit(void *data);
static void         CallbackSlideLeft(void *data);
static void         CallbackSlideRight(void *data);
static void         CallbackAnimate(void *data);
static void         CallbackHelp(void *data);
static void         CallbackShowMore(void *data);
static void         Callback_ConfigOK(void *data);
static void         Callback_ConfigApply(void *data);
static void         Callback_ConfigCancel(void *data);
static void         Callback_DefaultChange(void *data);
static void         Callback_BGChange(void *data);
static void         Callback_TypeChange(void *data);
static void         Callback_ConfigLeft(void *data);
static void         Callback_ConfigRight(void *data);
static void         Callback_ConfigAdd(void *data);
static void         Callback_ConfigDel(void *data);
static void         Callback_ConfigInterval(void *data);

#ifdef HAVE_EJECT
static void         Callback_EjectType(void *data);
#endif

/* config stuff */
void                SetupDefaults(void);
void                SetupGraphx(void);

static void
error_exit(void)
{
   Esync();
   exit(1);
}

void
UpdateGraphics(void)
{
   /* ok, this is cheap. */
   FreeMounts();
   FreeMountPointTypes();
   FreeImages();

   SetupDefaults();
   SetupMounts();
   SetupGraphx();
}

static void
ConfigUpdateDialog(void *data)
{
   if (current_type)
     {
	Epplet_gadget_hide(button_add_long);
	Epplet_gadget_show(tbox_key);
	Epplet_gadget_show(tbox_file);
	Epplet_gadget_show(arrow_left);
	Epplet_gadget_show(button_add);
	Epplet_gadget_show(button_del);
	Epplet_gadget_show(arrow_right);
	Epplet_gadget_show(label_key);
	Epplet_gadget_show(label_file);
     }
   else
     {
	Epplet_gadget_show(button_add_long);
	Epplet_gadget_hide(tbox_key);
	Epplet_gadget_hide(tbox_file);
	Epplet_gadget_hide(arrow_left);
	Epplet_gadget_hide(button_add);
	Epplet_gadget_hide(button_del);
	Epplet_gadget_hide(arrow_right);
	Epplet_gadget_hide(label_key);
	Epplet_gadget_hide(label_file);
     }

   if (mode.do_polling)
     {
	Epplet_gadget_show(hslider_interval);
	Epplet_gadget_show(label_slider);
	Epplet_gadget_show(label_interval);
     }
   else
     {
	Epplet_gadget_hide(hslider_interval);
	Epplet_gadget_hide(label_slider);
	Epplet_gadget_hide(label_interval);
     }

   return;
   data = NULL;
}

static void
CallbackShowMore(void *data)
{
   if (!mode.show_buttons)
     {
	Epplet_gadget_show(button_help);
	Epplet_gadget_show(button_config);
	Epplet_gadget_show(button_close);
     }
   else
     {
	Epplet_gadget_hide(button_help);
	Epplet_gadget_hide(button_config);
	Epplet_gadget_hide(button_close);
     }
   mode.show_buttons = !(mode.show_buttons);
   return;
   data = NULL;
}

/* don't need that right now.

static void
CallbackEnter(void *data, Window w)
{
  Epplet_gadget_show(button_more);
  return;
  data = NULL;
  w = (Window) 0;
}

static void
CallbackLeave(void *data, Window w)
{
  Epplet_gadget_hide(button_more);
  return;
  data = NULL;
  w = (Window) 0;
}

*/

static void
CallbackKeyPress(void *data, Window win, char *key)
{
   if (key)
     {
	if (!strcmp(key, "Left"))
	  {
	     CallbackSlideLeft(NULL);
	  }
	else if (!strcmp(key, "Right"))
	  {
	     CallbackSlideRight(NULL);
	  }
     }
   return;
   data = NULL;
   win = 0;
}

static void
CallbackHelp(void *data)
{
   Epplet_show_about("E-Mountbox");
   CallbackShowMore(NULL);
   return;
   data = NULL;
}

static void
Callback_ConfigOK(void *data)
{
   Callback_DefaultChange(NULL);
   Callback_BGChange(NULL);
   Callback_TypeChange(NULL);
   SyncConfigs();
   Epplet_save_config();
   UpdatePolling();
   UpdateGraphics();

   Epplet_window_hide(config_win);
   config_win = 0;
   return;
   data = NULL;
}

static void
Callback_ConfigApply(void *data)
{
   Callback_DefaultChange(NULL);
   Callback_BGChange(NULL);
   Callback_TypeChange(NULL);
   SyncConfigs();
   UpdatePolling();
   UpdateGraphics();

   /* Oh man. Of course current_type is now invalid. Banging head ... */
   current_type = types;
   if (current_type)
     {
	Epplet_change_textbox(tbox_key, current_type->key);
	Epplet_change_textbox(tbox_file, current_type->imagefile);
     }
   else
     {
	Epplet_change_textbox(tbox_key, "");
	Epplet_change_textbox(tbox_file, "");
     }
   return;
   data = NULL;
}

static void
Callback_ConfigCancel(void *data)
{
   Epplet_load_config();
   UpdateGraphics();
   Epplet_window_hide(config_win);
   config_win = 0;
   return;
   data = NULL;
}

static void
Callback_DefaultChange(void *data)
{
   Epplet_modify_config("DEFAULT", Epplet_textbox_contents(tbox_default));
   return;
   data = NULL;
}

static void
Callback_BGChange(void *data)
{
   Epplet_modify_config("BG_IMAGE", Epplet_textbox_contents(tbox_bg));
   return;
   data = NULL;
}

static void
Callback_TypeChange(void *data)
{
   ModifyMountPointType(current_type, Epplet_textbox_contents(tbox_key),
			Epplet_textbox_contents(tbox_file));
   return;
   data = NULL;
}

static void
Callback_ConfigLeft(void *data)
{
   if (current_type)
     {
	if (current_type->prev)
	  {
	     ModifyMountPointType(current_type,
				  Epplet_textbox_contents(tbox_key),
				  Epplet_textbox_contents(tbox_file));
	     current_type = current_type->prev;
	     Epplet_change_textbox(tbox_key, current_type->key);
	     Epplet_change_textbox(tbox_file, current_type->imagefile);
	  }
     }
   return;
   data = NULL;
}

static void
Callback_ConfigRight(void *data)
{
   if (current_type)
     {
	if (current_type->next)
	  {
	     ModifyMountPointType(current_type,
				  Epplet_textbox_contents(tbox_key),
				  Epplet_textbox_contents(tbox_file));
	     current_type = current_type->next;
	     Epplet_change_textbox(tbox_key, current_type->key);
	     Epplet_change_textbox(tbox_file, current_type->imagefile);
	  }
     }
   return;
   data = NULL;
}

static void
Callback_ConfigAdd(void *data)
{
   if (num_types == 0)
     {
	current_type = NULL;
	ConfigUpdateDialog(NULL);
     }
   ModifyMountPointType(current_type, Epplet_textbox_contents(tbox_key),
			Epplet_textbox_contents(tbox_file));
   AddMountPointType(NULL, NULL);
   current_type = types;
   Epplet_reset_textbox(tbox_key);
   Epplet_reset_textbox(tbox_file);

   return;
   data = NULL;
}

static void
Callback_ConfigDel(void *data)
{
   if (current_type)
     {
	if (current_type->next)
	  {
	     current_type = current_type->next;
	     DeleteMountPointType(current_type->prev);
	  }
	else if (current_type->prev)
	  {
	     current_type = current_type->prev;
	     DeleteMountPointType(current_type->next);
	  }
	else
	  {
	     DeleteMountPointType(current_type);
	     current_type = NULL;
	  }
     }

   if (current_type)
     {
	Epplet_change_textbox(tbox_key, current_type->key);
	Epplet_change_textbox(tbox_file, current_type->imagefile);
     }
   else
     {
	ConfigUpdateDialog(NULL);
     }

   return;
   data = NULL;
}

static void
Callback_ConfigInterval(void *data)
{
   char                s[64];

   Esnprintf(s, sizeof(s), "%i s", mode.polling_interval);
   Epplet_change_label(label_interval, s);

   return;
   data = NULL;
}

#ifdef HAVE_EJECT
static void
Callback_EjectType(void *data)
{
   EjectMode           ej_mode = (EjectMode) data;

   mode.eject_mode = ej_mode;

   switch (ej_mode)
     {
     case NO_EJECT:
	ejectbuttons[0] = 1;
	ejectbuttons[1] = 0;
	ejectbuttons[2] = 0;
	break;
     case MANUAL_EJECT:
	ejectbuttons[0] = 0;
	ejectbuttons[1] = 1;
	ejectbuttons[2] = 0;
	break;
     case AUTO_EJECT:
     default:
	ejectbuttons[0] = 0;
	ejectbuttons[1] = 0;
	ejectbuttons[2] = 1;
	break;
     }

   Epplet_gadget_data_changed(togglebutton_no_eject);
   Epplet_gadget_data_changed(togglebutton_manual_eject);
   Epplet_gadget_data_changed(togglebutton_auto_eject);

   return;
   data = NULL;
}
#endif

static void
CallbackConfigure(void *data)
{
   current_type = types;

   if (!config_win)
     {
	/* the position of some dialog items depends */
	/* on whether eject is available or not ... */
#ifdef HAVE_EJECT
	config_win =
	   Epplet_create_window_config(420, 280, "E-Mountbox Configuration",
				       Callback_ConfigOK, &config_win,
				       Callback_ConfigApply, &config_win,
				       Callback_ConfigCancel, &config_win);
#else
	config_win =
	   Epplet_create_window_config(420, 220, "E-Mountbox Configuration",
				       Callback_ConfigOK, &config_win,
				       Callback_ConfigApply, &config_win,
				       Callback_ConfigCancel, &config_win);
#endif
	Epplet_gadget_show(Epplet_create_label(12, 10, "Default icon", 2));
	Epplet_gadget_show((tbox_default =
			    Epplet_create_textbox(NULL,
						  Epplet_query_config
						  ("DEFAULT"), 10, 23, 400, 20,
						  2, Callback_DefaultChange,
						  NULL)));

	Epplet_gadget_show(Epplet_create_label(12, 55, "Background", 2));
	Epplet_gadget_show((tbox_bg =
			    Epplet_create_textbox(NULL,
						  Epplet_query_config
						  ("BG_IMAGE"), 10, 68, 400, 20,
						  2, Callback_BGChange, NULL)));

	label_key = Epplet_create_label(12, 100, "Pattern", 2);
	label_file = Epplet_create_label(72, 100, "Image file", 2);
	if (current_type)
	  {
	     tbox_key =
		Epplet_create_textbox(NULL, current_type->key, 10, 113, 60, 20,
				      2, Callback_TypeChange, NULL);
	     tbox_file =
		Epplet_create_textbox(NULL, current_type->imagefile, 70, 113,
				      340, 20, 2, Callback_TypeChange, NULL);
	  }
	else
	  {
	     tbox_key =
		Epplet_create_textbox(NULL, "", 10, 113, 60, 20, 2,
				      Callback_TypeChange, NULL);
	     tbox_file =
		Epplet_create_textbox(NULL, "", 70, 113, 340, 20, 2,
				      Callback_TypeChange, NULL);
	  }
	arrow_left =
	   Epplet_create_button(NULL, NULL, 170, 140, 0, 0, "ARROW_LEFT", 0,
				NULL, Callback_ConfigLeft, NULL);
	button_add =
	   Epplet_create_button("Add", NULL, 187, 140, 24, 12, NULL, 0, NULL,
				Callback_ConfigAdd, NULL);
	button_add_long =
	   Epplet_create_button("Add mountpoint type", NULL, 165, 120, 110, 16,
				NULL, 0, NULL, Callback_ConfigAdd, NULL);
	button_del =
	   Epplet_create_button("Delete", NULL, 216, 140, 36, 12, NULL, 0, NULL,
				Callback_ConfigDel, NULL);
	arrow_right =
	   Epplet_create_button(NULL, NULL, 257, 140, 0, 0, "ARROW_RIGHT", 0,
				NULL, Callback_ConfigRight, NULL);

#ifdef HAVE_EJECT
	switch (mode.eject_mode)
	  {
	  case NO_EJECT:
	     ejectbuttons[0] = 1;
	     ejectbuttons[1] = 0;
	     ejectbuttons[2] = 0;
	     break;
	  case MANUAL_EJECT:
	     ejectbuttons[0] = 0;
	     ejectbuttons[1] = 1;
	     ejectbuttons[2] = 0;
	     break;
	  case AUTO_EJECT:
	  default:
	     ejectbuttons[0] = 0;
	     ejectbuttons[1] = 0;
	     ejectbuttons[2] = 1;
	     break;
	  }

	Epplet_gadget_show(Epplet_create_label
			   (170, 160, "Don't eject media when unmounting", 2));
	Epplet_gadget_show((togglebutton_no_eject =
			    Epplet_create_togglebutton("", NULL, 150, 160, 12,
						       12, &ejectbuttons[0],
						       Callback_EjectType,
						       (void *)NO_EJECT)));
	Epplet_gadget_show(Epplet_create_label
			   (170, 180, "Manually eject unmounted media", 2));
	Epplet_gadget_show((togglebutton_manual_eject =
			    Epplet_create_togglebutton("", NULL, 150, 180, 12,
						       12, &ejectbuttons[1],
						       Callback_EjectType,
						       (void *)MANUAL_EJECT)));
	Epplet_gadget_show(Epplet_create_label
			   (170, 200, "Eject media when unmounting", 2));
	Epplet_gadget_show((togglebutton_auto_eject =
			    Epplet_create_togglebutton("", NULL, 150, 200, 12,
						       12, &ejectbuttons[2],
						       Callback_EjectType,
						       (void *)AUTO_EJECT)));

	Epplet_gadget_show(Epplet_create_label
			   (170, 220, "Watch status of mountpoints", 2));
	Epplet_gadget_show(Epplet_create_togglebutton
			   ("", NULL, 150, 220, 12, 12, &mode.do_polling,
			    ConfigUpdateDialog, NULL));
	Epplet_gadget_show(label_slider =
			   Epplet_create_label(170, 240, "Interval:", 2));
	Epplet_gadget_show(hslider_interval =
			   Epplet_create_hslider(230, 242, 60, 1, 60, 1, 10,
						 &mode.polling_interval,
						 Callback_ConfigInterval,
						 NULL));
	Epplet_gadget_show(label_interval =
			   Epplet_create_label(300, 240, "", 2));
#else
	Epplet_gadget_show(Epplet_create_label
			   (170, 160, "Watch status of mountpoints", 2));
	Epplet_gadget_show(Epplet_create_togglebutton
			   ("", NULL, 150, 160, 12, 12, &mode.do_polling,
			    ConfigUpdateDialog, NULL));
	Epplet_gadget_show(label_slider =
			   Epplet_create_label(170, 180, "Interval:", 2));
	Epplet_gadget_show(hslider_interval =
			   Epplet_create_hslider(230, 182, 60, 1, 60, 1, 10,
						 &mode.polling_interval,
						 Callback_ConfigInterval,
						 NULL));
	Epplet_gadget_show(label_interval =
			   Epplet_create_label(300, 180, "", 2));
#endif
	Callback_ConfigInterval(NULL);

	Epplet_window_pop_context();
     }

   ConfigUpdateDialog(NULL);
   Epplet_window_show(config_win);

   CallbackShowMore(NULL);
   return;
   data = NULL;
}

static void
CallbackAnimate(void *data)
{
   static double       k = 0.0;
   static double       step = M_PI / 10.0;

   int                 i, j, linear, linear_w;
   double              ratio;
   unsigned char      *current_tile_data;

   imlib_context_set_image(current_tile->image);
   current_tile_data = (unsigned char *)imlib_image_get_data();
   if (k < M_PI)
     {
	if (mode.anim_mount)
	   ratio = ((cos(k) + 1.0) / 4.0) * 1.3;
	else
	   ratio = ((cos(k + M_PI) + 1.0) / 4.0) * 1.3;

	for (i = 0; i < 32; i++)
	  {
	     for (j = 0; j < 44; j++)
	       {
		  if (!IsTransparent(current_tile->image, j, i))
		    {
		       linear = 4 * (i * 44 + j);
		       linear_w =
			  (i * 44 * 4 * num_tiles) +
			  (current_tile_index * 44 * 4) + 4 * j;
		       Epplet_get_rgb_pointer(window_buf)[linear] =
			  Epplet_get_rgb_pointer(widescreen_buf)[linear_w] =
			  ratio *
			  (Epplet_get_rgb_pointer(widescreen_canvas_buf)
			   [linear_w]) + (1.0 -
					  ratio) * (current_tile_data[linear]);
		       Epplet_get_rgb_pointer(window_buf)[linear + 1] =
			  Epplet_get_rgb_pointer(widescreen_buf)[linear_w + 1] =
			  ratio *
			  (Epplet_get_rgb_pointer(widescreen_canvas_buf)
			   [linear_w + 1]) + (1.0 -
					      ratio) *
			  (current_tile_data[linear + 1]);
		       Epplet_get_rgb_pointer(window_buf)[linear + 2] =
			  Epplet_get_rgb_pointer(widescreen_buf)[linear_w + 2] =
			  ratio *
			  (Epplet_get_rgb_pointer(widescreen_canvas_buf)
			   [linear_w + 2]) + (1.0 -
					      ratio) *
			  (current_tile_data[linear + 2]);
		    }
	       }
	  }
	Epplet_paste_buf(window_buf, Epplet_get_drawingarea_window(action_area),
			 -2, -2);
	k += step;
	Epplet_timer(CallbackAnimate, NULL, 0.05, "Anim");
     }
   else
      k = 0.0;

   return;
   data = NULL;
}

void
UpdateView(int dir, int fast)
{
   int                 i, j;
   double              start_t, delta_t, wait;
   double              step = M_PI / 44;

   if (dir == 0)
     {
	for (i = 0; i < 32; i++)
	  {
	     memcpy(Epplet_get_rgb_pointer(window_buf) + i * 44 * 4,
		    Epplet_get_rgb_pointer(widescreen_buf) +
		    (i * 44 * 4 * num_tiles) + (current_tile_index * 44 * 4),
		    44 * 4 * sizeof(unsigned char));
	  }
	Epplet_paste_buf(window_buf, Epplet_get_drawingarea_window(action_area),
			 -2, -2);
     }
   else
     {
	for (j = 0; j <= 44; j++)
	  {
	     if (!fast)
	       {
		  start_t = Epplet_get_time();
		  wait = fabs(cos(j * step)) / 100.0;
		  while ((delta_t = Epplet_get_time() - start_t) < wait);
	       }
	     for (i = 0; i < 32; i++)
	       {
		  memcpy(Epplet_get_rgb_pointer(window_buf) + i * 44 * 4,
			 Epplet_get_rgb_pointer(widescreen_buf) +
			 (i * 44 * 4 * num_tiles) +
			 (current_tile_index * 44 * 4) + (dir) * j * 4,
			 44 * 4 * sizeof(unsigned char));
	       }
	     Epplet_paste_buf(window_buf,
			      Epplet_get_drawingarea_window(action_area), -2,
			      -2);
	  }
     }
}

int
IsTransparent(Imlib_Image * im, int x, int y)
{
   int                 linear;
   unsigned char      *data;

   if (!im || x < 0 || y < 0)
      return 0;

   imlib_context_set_image(im);
   if (x >= imlib_image_get_width() || y >= imlib_image_get_height())
      return 0;

   linear = 4 * (y * imlib_image_get_width() + x);

   data = (unsigned char *)imlib_image_get_data();
   if (data[linear + 3] != 0xff)
      return 0;			/* FIXME - Should return 1 but blending needs to be fixed */

   return 0;
}

/* mount handling */
void
SetupMounts(void)
{

   /* first, parse /etc/fstab to see what user-mountable mountpoints we have */
   if (!(ParseFstab()))
     {
	/* Couldn't read /etc/fstab */
	Epplet_dialog_ok("Could not read mountpoint information.");
	error_exit();
     }

   /* do we have user-mountable fs's at all? */
   if (num_tiles == 0)
     {
	Epplet_dialog_ok("Could not find any usable mountpoints.");
	error_exit();
     }

   /* now, check if these are actually mounted already */

   /* check for /proc/mounts */
   if (!(ParseProcMounts()))
      /* no? ok, check for /etc/mtab */
      if (!(ParseEtcMtab()))
	 /* damnit, look if files exist in the mountpoints. */
	 VisitMountPoints();

   /* Man, this code has far too many comments :) */
}

void
AddMountPoint(char *device, char *path)
{
   Tile               *newtile = NULL;
   char               *tmp_dev = NULL;
   char               *tmp_path = NULL;
   int                 i;
   MountPointType     *type = NULL;
   char               *s = NULL;
   Imlib_Image        *tmp_image = NULL;

   static Tile        *tail_tile = NULL;

   if (!tiles)
     {
	tiles = (Tile *) malloc(sizeof(Tile));
	if (tiles)
	  {
	     memset(tiles, 0, sizeof(Tile));
	     num_tiles = 1;
	     current_tile = tail_tile = tiles;
	  }
     }
   else
     {
	newtile = (Tile *) malloc(sizeof(Tile));
	if (newtile)
	  {
	     memset(newtile, 0, sizeof(Tile));
	     newtile->next = NULL;
	     newtile->prev = tail_tile;
	     tail_tile->next = newtile;
	     tail_tile = newtile;
	     num_tiles++;
	  }
     }
   current_tile_index = 0;

   if (tail_tile)
     {
	tail_tile->mountpoint = (MountPoint *) malloc(sizeof(MountPoint));
	if (tail_tile->mountpoint)
	  {
	     memset(tail_tile->mountpoint, 0, sizeof(MountPoint));
	     if (device)
		tail_tile->mountpoint->device = strdup(device);
	     if (path)
		tail_tile->mountpoint->path = strdup(path);
	  }
	if (device && path)
	  {
	     tmp_path = strdup(path);
	     tmp_dev = strdup(device);
	     for (i = 0; i < (int)strlen(tmp_path); i++)
		tmp_path[i] = (char)(tolower(tmp_path[i]));
	     for (i = 0; i < (int)strlen(tmp_dev); i++)
		tmp_dev[i] = (char)(tolower(tmp_dev[i]));
	     if (tmp_path && tmp_dev)
	       {
		  type = types;
		  while (type)
		    {
		       if (strstr(tmp_dev, type->key))
			 {
			    tail_tile->image = type->image;
			    break;
			 }
		       else if (strstr(tmp_path, type->key))
			 {
			    tail_tile->image = type->image;
			    break;
			 }
		       type = type->next;
		    }

		  if (tail_tile->image == NULL)
		    {
		       s = Epplet_query_config("DEFAULT");

		       if (!default_image)
			 {
			    tmp_image = imlib_load_image(s);
			    if (!tmp_image)
			      {
				 char                buf[1024];

				 Esnprintf(buf, sizeof(buf), "%s/%s",
					   Epplet_data_dir(), __DEFAULT);
				 tmp_image = imlib_load_image(buf);
			      }
			    if (!tmp_image)
			      {
				 Epplet_dialog_ok
				    ("  E-Mountbox could not load a default icon\n  "
				     "  for the mountpoints. Check your installation.  ");
				 error_exit();
			      }
			    imlib_context_set_image(tmp_image);
			    default_image =
			       imlib_create_cropped_scaled_image(0, 0,
								 imlib_image_get_width
								 (),
								 imlib_image_get_height
								 (), 44, 32);
			    imlib_free_image();
			 }
		       tail_tile->image = default_image;
		    }

		  free(tmp_path);
		  free(tmp_dev);
	       }
	  }
     }
}

void
AddMountPointType(char *key, char *image)
{
   MountPointType     *newtype = NULL;
   Imlib_Image        *tmp_image = NULL;

   if (!types)
     {
	types = (MountPointType *) malloc(sizeof(MountPointType));
	if (types)
	  {
	     memset(types, 0, sizeof(MountPointType));
	     num_types = 1;
	  }
     }
   else
     {
	newtype = (MountPointType *) malloc(sizeof(MountPointType));
	if (newtype)
	  {
	     memset(newtype, 0, sizeof(MountPointType));
	     newtype->next = types;
	     types->prev = newtype;
	     types = newtype;
	     num_types++;
	  }
     }

   if (types)
     {
	if ((types->key == NULL) && (types->image == NULL))
	  {
	     if (key)
		types->key = strdup(key);
	     if (image)
		types->imagefile = strdup(image);
	     tmp_image = imlib_load_image(image);
	     if (tmp_image)
	       {
		  imlib_context_set_image(tmp_image);
		  types->image =
		     imlib_create_cropped_scaled_image(0, 0,
						       imlib_image_get_width(),
						       imlib_image_get_height(),
						       44, 32);
		  imlib_free_image();
	       }
	  }
     }
}

void
ModifyMountPointType(MountPointType * mpt, char *key, char *imagefile)
{

   if (mpt)
     {
	if (key)
	  {
	     if (mpt->key)
		free(mpt->key);
	     mpt->key = strdup(key);
	  }
	if (imagefile)
	  {
	     if (mpt->imagefile)
		free(mpt->imagefile);
	     mpt->imagefile = strdup(imagefile);
	  }
     }
}

void
DeleteMountPointType(MountPointType * mpt)
{
   if (mpt)
     {
	/* is it in the middle */
	if (mpt->next && mpt->prev)
	  {
	     mpt->prev->next = mpt->next;
	     mpt->next->prev = mpt->prev;
	  }
	/* or at the beginning */
	else if (mpt->next)
	  {
	     mpt->next->prev = NULL;
	     types = mpt->next;
	  }
	/* or at the end ... */
	else if (mpt->prev)
	  {
	     mpt->prev->next = NULL;
	  }

	num_types--;
	if (num_types == 0)
	  {
	     types = NULL;
	  }

	/* free it */
	if (mpt->key)
	  {
	     free(mpt->key);
	  }
	if (mpt->imagefile)
	  {
	     free(mpt->imagefile);
	  }
	if (mpt->image)
	  {
	     imlib_context_set_image(mpt->image);
	     imlib_free_image();
	     mpt->image = NULL;
	  }
	free(mpt);
     }
}

void
FreeImages(void)
{
   if (bg_image)
     {
	imlib_context_set_image(bg_image);
	imlib_free_image();
	bg_image = NULL;
     }
   if (default_image)
     {
	imlib_context_set_image(default_image);
	imlib_free_image();
	default_image = NULL;
     }
}

void
FreeMounts(void)
{
   Tile               *current, *tmp;

   current = tiles;
   while (current)
     {
	if (current->mountpoint)
	  {
	     if (current->mountpoint->device)
		free(current->mountpoint->device);
	     if (current->mountpoint->path)
		free(current->mountpoint->path);
	     free(current->mountpoint);
	  }
	/* images need _not_ be freed here */
	tmp = current;
	current = current->next;
	free(tmp);
     }
   tiles = NULL;
   num_tiles = 0;
}

void
FreeMountPointTypes(void)
{
   MountPointType     *current, *tmp;

   current = types;
   while (current)
     {
	if (current->key)
	  {
	     free(current->key);
	  }
	if (current->imagefile)
	  {
	     free(current->imagefile);
	  }
	if (current->image)
	  {
	     imlib_context_set_image(current->image);
	     imlib_free_image();
	     current->image = NULL;
	  }
	tmp = current;
	current = current->next;
	free(tmp);
     }
   types = NULL;
   num_types = 0;
}

void
Mount(MountPoint * mp)
{
   char                s[1024];

   if (mp)
     {
	if (mp->mounted)
	   return;
	if (mp->path)
	  {
	     Esnprintf(s, sizeof(s), "%s %s", MOUNT, mp->path);
	     if (!Epplet_run_command(s))
	       {
		  mp->mounted = 1;
		  mode.anim_mount = 1;
		  Epplet_timer(CallbackAnimate, NULL, 0, "Anim");
	       }
	     else
	       {
		  s[0] = 0;
		  Esnprintf(s, sizeof(s), "Could not mount %s.", mp->path);
		  Epplet_dialog_ok(s);
	       }
	  }
     }
}

void
Umount(MountPoint * mp)
{
   char                s[1024];

   s[0] = 0;

   if (mp)
     {
	if (!(mp->mounted))
	   return;
	if (mp->path)
	  {
	     Esnprintf(s, sizeof(s), "%s %s", UMOUNT, mp->path);
	     if (!Epplet_run_command(s))
	       {
		  mp->mounted = 0;
		  mode.anim_mount = 0;
		  Epplet_timer(CallbackAnimate, NULL, 0, "Anim");
#ifdef HAVE_EJECT
		  if (mode.eject_mode == AUTO_EJECT)
		    {
		       Esnprintf(s, sizeof(s), "%s %s", EJECT, mp->device);
		       Epplet_run_command(s);
		    }
#endif
	       }
	     else
	       {
		  s[0] = 0;
		  Esnprintf(s, sizeof(s), "Could not unmount %s.", mp->path);
		  Epplet_dialog_ok(s);
	       }
	  }
     }
}

int
ParseFstab(void)
{
   FILE               *f;
   char                s[1024];
   char               *token = NULL;
   char               *info[4];
   int                 i;

   if ((f = fopen(FSTAB, "r")) == NULL)
      return 0;
   *s = 0;
   for (; fgets(s, sizeof(s), f);)
     {
	/* skip comments and blank lines */
	if (!(*s) || (*s == '\n') || (*s == '#'))
	  {
	     continue;
	  }

	for (i = 0; i < 4; i++)
	   info[i] = NULL;

	/* parse out tokens we need */
	i = 0;
	token = strtok(s, " \t");
	if (token)
	  {
	     info[i++] = strdup(token);
	  }
	while ((token = strtok(NULL, " \t")))
	  {
	     info[i++] = strdup(token);
	  }

	/* see if device is user-mountable */
	if (strstr(info[3], "user"))
	  {
	     AddMountPoint(info[0], info[1]);
	  }

	for (i = 0; i < 4; i++)
	   if (info[i])
	      free(info[i]);
     }

   fclose(f);
   return 1;
}

int
ParseProcMounts(void)
{
   FILE               *f;
   char                s[1024];
   char               *token = NULL, *device = NULL, *path = NULL;
   Tile               *tile;

   if ((f = fopen(PROCMOUNTS, "r")) == NULL)
      return 0;
   *s = 0;
   for (; fgets(s, sizeof(s), f);)
     {
	/* skip comments and blank lines (shouldn't be there, actually ...) */
	if (!(*s) || (*s == '\n') || (*s == '#'))
	  {
	     continue;
	  }

	/* parse out tokens we need */
	token = strtok(s, " \t");
	if (token)
	  {
	     device = strdup(token);
	  }
	token = NULL;
	token = strtok(NULL, " \t");
	if (token)
	  {
	     path = strdup(token);
	  }

	/* set that device mounted in our list ... */
	tile = tiles;
	while (tile)
	  {
	     if (tile->mountpoint)
	       {
		  if (!strcmp(tile->mountpoint->path, path))
		    {
		       tile->mountpoint->mounted = 1;
		    }
	       }
	     tile = tile->next;
	  }

	if (device)
	   free(device);
	if (path)
	   free(path);
     }

   fclose(f);
   return 1;
}

int
ParseEtcMtab(void)
{
   FILE               *f;
   char                s[1024];
   char               *token = NULL, *device = NULL, *path = NULL;
   Tile               *tile;

   if ((f = fopen(ETCMTAB, "r")) == NULL)
      return 0;
   *s = 0;
   for (; fgets(s, sizeof(s), f);)
     {
	/* skip comments and blank lines (shouldn't be there, actually ...) */
	if (!(*s) || (*s == '\n') || (*s == '#'))
	  {
	     continue;
	  }

	/* parse out tokens we need */
	token = strtok(s, " \t");
	if (token)
	  {
	     device = strdup(token);
	  }
	token = NULL;
	token = strtok(NULL, " \t");
	if (token)
	  {
	     path = strdup(token);
	  }

	/* set that device mounted in our list ... */
	tile = tiles;
	while (tile)
	  {
	     if (tile->mountpoint)
	       {
		  if (!strcmp(tile->mountpoint->path, path))
		    {
		       tile->mountpoint->mounted = 1;
		    }
	       }
	     tile = tile->next;
	  }

	if (device)
	   free(device);
	if (path)
	   free(path);
     }

   fclose(f);
   return 1;
}

void
VisitMountPoints(void)
{
   DIR                *dir;
   int                 num_entries;
   struct dirent      *dp;
   Tile               *tile = NULL;

   tile = tiles;
   while (tile)
     {
	if (tile->mountpoint)
	  {
	     if (tile->mountpoint->path)
	       {
		  dir = NULL;
		  dir = opendir(tile->mountpoint->path);
		  if (dir)
		    {
		       num_entries = 0;
		       for (num_entries = 0; (dp = readdir(dir)) != NULL;
			    num_entries++);
		       if (num_entries > 2)
			 {
			    tile->mountpoint->mounted = 1;
			 }
		    }
	       }
	  }
	tile = tile->next;
     }
}

MountPoint         *
FindMountPointByClick(int x, int y)
{
   if (!IsTransparent(current_tile->image, x, y))
      return current_tile->mountpoint;

   return NULL;
}

static void
PollMountpoints(void *data)
{
   Tile               *tile;
   int                 i, j, k, linear, linear_w, status = 0;
   unsigned char      *widescreen_data, *widescreen_canvas_data, *tile_data;

   if (current_tile->mountpoint)
     {
	status = current_tile->mountpoint->mounted;
     }

   /* reset mount status */
   tile = tiles;
   while (tile)
     {
	if (tile->mountpoint)
	  {
	     tile->mountpoint->mounted = 0;
	  }
	tile = tile->next;
     }

   /* get new mount status */
   if (!(ParseProcMounts()))
      if (!(ParseEtcMtab()))
	 VisitMountPoints();

   /* build new image */
   tile = tiles;
   imlib_context_set_image(widescreen_buf->im);
   widescreen_data = (unsigned char *)imlib_image_get_data();
   imlib_context_set_image(widescreen_canvas_buf->im);
   widescreen_canvas_data = (unsigned char *)imlib_image_get_data();

   for (k = 0; k < num_tiles; k++, tile = tile->next)
     {
	imlib_context_set_image(tile->image);
	tile_data = (unsigned char *)imlib_image_get_data();
	for (i = 0; i < 32; i++)
	  {
	     for (j = 0; j < 44; j++)
	       {
		  if (!IsTransparent(tile->image, j, i))
		    {
		       linear = 4 * (i * 44 + j);
		       linear_w =
			  (i * 44 * 4 * num_tiles) + (k * 44 * 4) + 4 * j;
		       if (tile->mountpoint->mounted)
			 {
			    widescreen_data[linear_w] = tile_data[linear];
			    widescreen_data[linear_w + 1] =
			       tile_data[linear + 1];
			    widescreen_data[linear_w + 2] =
			       tile_data[linear + 2];
			 }
		       else
			 {
			    widescreen_data[linear_w] =
			       0.65 * widescreen_canvas_data[linear_w] +
			       0.35 * tile_data[linear];
			    widescreen_data[linear_w + 1] =
			       0.65 * widescreen_canvas_data[linear_w + 1] +
			       0.35 * tile_data[linear + 1];
			    widescreen_data[linear_w + 2] =
			       0.65 * widescreen_canvas_data[linear_w + 2] +
			       0.35 * tile_data[linear + 2];
			 }
		    }
	       }
	  }
     }

   /* see if current mountpoint is affected */
   if (current_tile->mountpoint)
     {
	if (current_tile->mountpoint->mounted != status)
	  {
	     mode.anim_mount = current_tile->mountpoint->mounted;
	     CallbackAnimate(NULL);
	  }
     }

   Epplet_timer(PollMountpoints, NULL, (double)mode.polling_interval,
		"POLLING");
   return;
   data = NULL;
}

static void
CallbackExit(void *data)
{
   data = NULL;
   FreeMounts();
   FreeMountPointTypes();
   FreeImages();
   Epplet_unremember();
   Esync();
   exit(0);
}

static void
CallbackButtonUp(void *data, Window win, int x, int y, int b)
{
   MountPoint         *mountpoint = NULL;
   char                s[1024];

   if (win == Epplet_get_drawingarea_window(action_area))
     {
	mountpoint = FindMountPointByClick(x, y);
	if (mountpoint)
	  {
	     if (b == 1)
	       {
		  if (mountpoint->mounted)
		     Umount(mountpoint);
		  else
		     Mount(mountpoint);
	       }
#ifdef HAVE_EJECT
	     else if (b == 3 && mode.eject_mode == MANUAL_EJECT)
	       {
		  Esnprintf(s, sizeof(s), "%s %s", EJECT, mountpoint->device);
		  Epplet_run_command(s);
	       }
#endif
	     else
	       {
		  Epplet_gadget       popup = Epplet_create_popup();

		  s[0] = 0;
		  Esnprintf(s, sizeof(s), "%s at %s.", mountpoint->device,
			    mountpoint->path);
		  Epplet_add_popup_entry(popup, s, NULL, NULL, NULL);
		  Epplet_pop_popup(popup, 0);
	       }
	  }
     }
   return;
   data = NULL;
}

static void
CallbackSlideLeft(void *data)
{
   if (current_tile->prev)
     {
	UpdateView(-1, 0);
	current_tile = current_tile->prev;
	current_tile_index--;
     }
   else
     {
	while (current_tile->next)
	  {
	     UpdateView(+1, 1);
	     current_tile = current_tile->next;
	     current_tile_index++;
	  }
     }
   return;
   data = NULL;
}

static void
CallbackSlideRight(void *data)
{
   if (current_tile->next)
     {
	UpdateView(1, 0);
	current_tile = current_tile->next;
	current_tile_index++;
     }
   else
     {
	while (current_tile->prev)
	  {
	     UpdateView(-1, 1);
	     current_tile = current_tile->prev;
	     current_tile_index--;
	  }
     }
   return;
   data = NULL;
}

static void
CallbackExpose(void *data, Window win, int x, int y, int w, int h)
{
   UpdateView(0, 0);
   return;
   data = NULL;
   win = x = y = w = h = 0;
}

void
SetupDefaults(void)
{
   int                 i, instance, num_results;
   char               *s, s2[256], *key = NULL, *image = NULL, *token;
   char              **results = NULL;

   for (i = 0; i < (int)(sizeof(defaults) / sizeof(ConfigItem)); i++)
     {
	if (!Epplet_query_config(defaults[i].key))
	   Epplet_add_config(defaults[i].key, defaults[i].value);
     }

   instance = atoi(Epplet_query_config_def("INSTANCE", "0"));
   Esnprintf(s2, sizeof(s), "%i", ++instance);
   Epplet_modify_config("INSTANCE", s2);

   mode.eject_mode = (EjectMode) atoi(Epplet_query_config("EJECT_MODE"));
   mode.do_polling = atoi(Epplet_query_config("DO_POLL"));
   mode.polling_interval = atoi(Epplet_query_config("POLLINTVAL"));
   mode.anim_mount = 0;
   mode.show_buttons = 0;

   results = Epplet_query_multi_config("TYPEDEF", &num_results);
   if ((!results) && (instance == 1))
     {
	Epplet_modify_multi_config("TYPEDEF", default_types,
				   (int)(sizeof(default_types) /
					 sizeof(char *)));
	results = Epplet_query_multi_config("TYPEDEF", &num_results);
	if (!results)
	  {
	     Epplet_dialog_ok("  Could not set up mountpoint types.  \n"
			      "  Check your installation.  \n");
	     error_exit();
	  }
     }

   for (i = 0; i < num_results; i++)
     {
	if (results[i])
	  {
	     s = strdup(results[i]);
	     token = strtok(s, " \t");
	     if (token)
		key = strdup(token);
	     token = strtok(NULL, " \t");
	     if (token)
		image = strdup(token);

	     if (key && image)
	       {
		  AddMountPointType(key, image);
	       }
	     free(key);
	     free(image);
	     free(s);
	  }
     }
   free(results);
}

void
SetupGraphx(void)
{
   static int          first_time = 1;
   int                 i, j, k, linear, linear_w;
   Imlib_Image        *tmp = NULL;
   Tile               *tile;
   char               *s = NULL, buf[1024];
   unsigned char      *widescreen_data, *widescreen_canvas_data, *tile_data,
      *bg_data;

   s = Epplet_query_config("BG_IMAGE");

   tmp = imlib_load_image(s);
   if (!tmp)
     {
	Esnprintf(buf, sizeof(buf), "%s/%s", Epplet_data_dir(), __BG_IMAGE);
	tmp = imlib_load_image(buf);
     }
   if (!tmp)
     {
	/* Even the fallbacks didn't work.  If we don't exit
	 * here, we'll seg fault.  -- mej */
	Epplet_dialog_ok("Could not load all images.");
	Esync();
	exit(-1);
     }
   /*
    * sscanf(Epplet_query_config("BG_BORDER"), "%i %i %i %i",
    * &(border.left), &(border.right), &(border.top), &(border.bottom));    
    * Imlib_set_image_border(id, tmp, &border);
    */
   imlib_context_set_image(tmp);
   bg_image =
      imlib_create_cropped_scaled_image(0, 0, imlib_image_get_width(),
					imlib_image_get_height(),
					44 * num_tiles, 32);
   imlib_free_image();

   /* setup widescreen according to current mounts */
   if (!window_buf)
      window_buf = Epplet_make_rgb_buf(44, 32);
   if (widescreen_buf)
      Epplet_free_rgb_buf(widescreen_buf);
   widescreen_buf = Epplet_make_rgb_buf((44 * num_tiles), 32);
   if (widescreen_canvas_buf)
      Epplet_free_rgb_buf(widescreen_canvas_buf);
   widescreen_canvas_buf = Epplet_make_rgb_buf((44 * num_tiles), 32);

   imlib_context_set_image(widescreen_buf->im);
   widescreen_data = (unsigned char *)imlib_image_get_data();
   imlib_context_set_image(widescreen_canvas_buf->im);
   widescreen_canvas_data = (unsigned char *)imlib_image_get_data();
   imlib_context_set_image(bg_image);
   bg_data = (unsigned char *)imlib_image_get_data();

   memcpy(widescreen_data, bg_data,
	  sizeof(unsigned char) * 44 * 4 * num_tiles * 32);
   memcpy(widescreen_canvas_data, bg_data,
	  sizeof(unsigned char) * 44 * 4 * num_tiles * 32);

   tile = tiles;
   for (k = 0; k < num_tiles; k++, tile = tile->next)
     {
	imlib_context_set_image(tile->image);
	tile_data = (unsigned char *)imlib_image_get_data();
	for (i = 0; i < 32; i++)
	  {
	     for (j = 0; j < 44; j++)
	       {
		  if (!IsTransparent(tile->image, j, i))
		    {
		       linear = 4 * (i * 44 + j);
		       linear_w =
			  (i * 44 * 4 * num_tiles) + (k * 44 * 4) + 4 * j;
		       if (tile->mountpoint->mounted)
			 {
			    widescreen_data[linear_w] = tile_data[linear];
			    widescreen_data[linear_w + 1] =
			       tile_data[linear + 1];
			    widescreen_data[linear_w + 2] =
			       tile_data[linear + 2];
			 }
		       else
			 {
			    widescreen_data[linear_w] =
			       0.65 * widescreen_data[linear_w] +
			       0.35 * tile_data[linear];
			    widescreen_data[linear_w + 1] =
			       0.65 * widescreen_data[linear_w + 1] +
			       0.35 * tile_data[linear + 1];
			    widescreen_data[linear_w + 2] =
			       0.65 * widescreen_data[linear_w + 2] +
			       0.35 * tile_data[linear + 2];
			 }
		    }
	       }
	  }
     }

   if (first_time)
     {
	first_time = 0;
	Epplet_gadget_show((Epplet_create_button(NULL, NULL,
						 2, 34, 0, 0, "ARROW_LEFT", 0,
						 NULL, CallbackSlideLeft,
						 NULL)));
	Epplet_gadget_show((Epplet_create_button
			    (NULL, NULL, 33, 34, 0, 0, "ARROW_RIGHT", 0, NULL,
			     CallbackSlideRight, NULL)));
	Epplet_gadget_show((action_area =
			    Epplet_create_drawingarea(2, 2, 44, 32)));

	Epplet_gadget_show((Epplet_create_button
			    ("...", NULL, 14, 34, 20, 12, NULL, 0, NULL,
			     CallbackShowMore, NULL)));
	button_help =
	   Epplet_create_button(NULL, NULL, 3, 3, 0, 0, "HELP", 0, NULL,
				CallbackHelp, NULL);
	button_close =
	   Epplet_create_button(NULL, NULL, 33, 3, 0, 0, "CLOSE", 0, NULL,
				CallbackExit, NULL);
	button_config =
	   Epplet_create_button(NULL, NULL, 18, 3, 0, 0, "CONFIGURE", 0, NULL,
				CallbackConfigure, NULL);

	/*
	 * Epplet_register_focus_in_handler(CallbackEnter, NULL);
	 * Epplet_register_focus_out_handler(CallbackLeave, NULL);
	 */
	Epplet_register_expose_handler(CallbackExpose, NULL);
	Epplet_register_button_release_handler(CallbackButtonUp, NULL);
	Epplet_register_key_press_handler(CallbackKeyPress, NULL);

	/* Setup the current view */
	Epplet_show();
     }

   UpdateView(0, 0);
}

void
UpdatePolling(void)
{
   if (mode.do_polling)
     {
	Epplet_timer(PollMountpoints, NULL, (double)mode.polling_interval,
		     "POLLING");
     }
   else
     {
	Epplet_remove_timer("POLLING");
     }
}

void
SyncConfigs(void)
{
   char              **strings = NULL;
   char                s[1024];
   int                 i;
   MountPointType     *mpt = NULL;

   Esnprintf(s, sizeof(s), "%i", (int)mode.eject_mode);
   Epplet_modify_config("EJECT_MODE", s);

   Esnprintf(s, sizeof(s), "%i", mode.do_polling);
   Epplet_modify_config("DO_POLL", s);

   Esnprintf(s, sizeof(s), "%i", mode.polling_interval);
   Epplet_modify_config("POLLINTVAL", s);

   strings = (char **)malloc(sizeof(char *) * num_types);
   if (strings)
     {
	for (mpt = types, i = 0; mpt; mpt = mpt->next, i++)
	  {
	     Esnprintf(s, sizeof(s), "%s  %s", mpt->key, mpt->imagefile);
	     strings[i] = strdup(s);
	  }

	Epplet_modify_multi_config("TYPEDEF", strings, num_types);

	for (i = 0; i < num_types; i++)
	   if (strings[i])
	      free(strings[i]);
	free(strings);
     }
}

int
main(int argc, char **argv)
{
   atexit(Epplet_cleanup);
   Epplet_Init("E-Mountbox", "0.1", "Enlightenment Mount Epplet",
	       3, 3, argc, argv, 0);
   Epplet_load_config();

   SetupDefaults();
   SetupMounts();
   SetupGraphx();
   UpdatePolling();

   Epplet_Loop();
   error_exit();
   return 0;
}
