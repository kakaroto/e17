/* E-Mountbox.h
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
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

#define FSTAB      "/etc/fstab"
#define PROCMOUNTS "/proc/mounts"
#define ETCMTAB    "/etc/mtab"
#define MOUNT_CMD  "/bin/mount"
#define UMOUNT_CMD "/bin/umount"

#define __BG_IMAGE EROOT"/epplet_data/E-Mountbox/E-Mountbox-bg.png"
#define __DEFAULT  EROOT"/epplet_data/E-Mountbox/E-Mountbox-blockdev.png"

ConfigItem defaults[] = {
  {"BG_IMAGE", EROOT"/epplet_data/E-Mountbox/E-Mountbox-bg.png"},
  {"DEFAULT", EROOT"/epplet_data/E-Mountbox/E-Mountbox-blockdev.png"}
};

char *default_types[] = {
  "cd   "EROOT"/epplet_data/E-Mountbox/E-Mountbox-cd.png",
  "fd   "EROOT"/epplet_data/E-Mountbox/E-Mountbox-floppy.png",
  "zip  "EROOT"/epplet_data/E-Mountbox/E-Mountbox-zip.png",
  "jazz "EROOT"/epplet_data/E-Mountbox/E-Mountbox-jazz.png"
};

typedef struct _tile Tile;
typedef struct _mountpointtype MountPointType;

struct _mountpointtype
{
  char           *key;
  char           *imagefile;
  ImlibImage     *image;
  MountPointType *next;
  MountPointType *prev;
};

typedef struct _mountpoint
{
  char         *device;
  char         *path;
  char          mounted;
}
MountPoint;

struct _tile
{
  ImlibImage   *image;
  MountPoint   *mountpoint;  
  Tile         *prev;
  Tile         *next;
};

Tile           *tiles = NULL;
Tile           *current_tile = NULL;
int             current_tile_index = 0;
int             num_tiles = 0;
MountPointType *types = NULL;
int             num_types = 0;
ImlibData      *id = NULL;
ImlibImage     *bg_image = NULL;
ImlibImage     *default_image = NULL;
Epplet_gadget   action_area, button_close, button_config, button_help;

/* stuff for the config win */
Epplet_gadget   tbox_key, tbox_file, tbox_default, tbox_bg;
Epplet_gadget   arrow_left, arrow_right, button_add, button_del, button_add_long;
Epplet_gadget   label_key, label_file;
Window          config_win = 0;
MountPointType *current_type = NULL;

RGB_buf         window_buf = NULL;
RGB_buf         widescreen_buf = NULL;
RGB_buf         widescreen_canvas_buf = NULL;
char            anim_mount = 0;
int             is_shown = 0;

/* graphx handling */
int             IsTransparent(ImlibImage * im, int x, int y);
void            UpdateView(int dir, int fast);
void            FreeImages(void);
void            UpdateGraphics(void);
void            ConfigShowMore(void);
void            ConfigShowLess(void);

/* mount handling */
void            SetupMounts(void);
void            FreeMounts(void);
void            AddMountPoint(char *device, char *path);
void            FreeMountPointTypes(void);
void            AddMountPointType(char *key, char *image);
void            ModifyMountPointType(MountPointType *mpt, char *key, char *imagefile);
void            DeleteMountPointType(MountPointType *mpt);
void            Mount(MountPoint * mp);
void            Umount(MountPoint * mp);

/* miscellaneous nitty gritty */
int             ParseFstab(void);
int             ParseProcMounts(void);
int             ParseEtcMtab(void);
void            VisitMountPoints(void);
MountPoint     *FindMountPointByClick(int x, int y);
void            SyncConfigs(void);

/* callbacks/ handlers */
/*
static void     CallbackEnter(void *data, Window w);
static void     CallbackLeave(void *data, Window w);
*/
static void     CallbackExpose(void *data, Window win, int x, int y, int w, int h);
static void     CallbackButtonUp(void *data, Window win, int x, int y, int b);
static void     CallbackExit(void *data);
static void     CallbackSlideLeft(void *data);
static void     CallbackSlideRight(void *data);
static void     CallbackAnimate(void *data);
static void     CallbackHelp(void *data);
static void     CallbackShowMore(void *data);
static void     Callback_ConfigOK(void *data);
static void     Callback_ConfigApply(void *data);
static void     Callback_ConfigCancel(void *data);
static void     Callback_DefaultChange(void *data);
static void     Callback_BGChange(void *data);
static void     Callback_TypeChange(void *data);
static void     Callback_ConfigLeft(void *data);
static void     Callback_ConfigRight(void *data);
static void     Callback_ConfigAdd(void *data);
static void     Callback_ConfigDel(void *data);

/* config stuff */
void            SetupDefaults(void);
void            SetupGraphx(void);
