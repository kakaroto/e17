#include "epplet.h"
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

#define TYPE_CD 0
#define TYPE_HD 1
#define TYPE_FD 2
#define TYPE_ZIP 3
#define TYPE_BG 4
#define MAXTYPE 5

#define FSTAB      "/etc/fstab"
#define PROCMOUNTS "/proc/mounts"
#define ETCMTAB    "/etc/mtab"
#define MOUNT_CMD  "/bin/mount"
#define UMOUNT_CMD "/bin/umount"

ConfigItem defaults[] = {
  {"CD_IMAGE", EROOT"/epplet_data/E-Mountbox-cd.png"},
  {"HD_IMAGE", EROOT"/epplet_data/E-Mountbox-blockdev.png"},
  {"FD_IMAGE", EROOT"/epplet_data/E-Mountbox-floppy.png"},
  {"ZIP_IMAGE", EROOT"/epplet_data/E-Mountbox-zip.png"},
  {"BG_IMAGE", EROOT"/epplet_data/E-Mountbox-bg.png"},
  {"BG_BORDER", "5 5 5 5"}
};

typedef struct _tile Tile;

typedef struct _mountpoint
{
  char         *device;
  char         *path;
  int           type;
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
ImlibData      *id = NULL;
ImlibImage     *images[5];
Epplet_gadget   action_area, button_left, button_right, button_more, button_close, button_config, button_help;
RGB_buf         window_buf;
RGB_buf         widescreen_buf;
RGB_buf         widescreen_canvas_buf;
char            anim_mount = 0;
int             is_shown = 0;

/* graphx handling */
int             IsTransparent(ImlibImage * im, int x, int y);
void            UpdateView(int dir, int fast);
void            FreeImages(void);

/* mount handling */
void            SetupMounts(void);
void            FreeMounts(void);
void            AddMountPoint(char *device, char *path);
void            Mount(MountPoint * mp);
void            Umount(MountPoint * mp);

/* miscellaneous nitty gritty */
int             ParseFstab(void);
int             ParseProcMounts(void);
int             ParseEtcMtab(void);
void            VisitMountPoints(void);
MountPoint     *FindMountPointByClick(int x, int y);

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

/* config stuff */
void            SetupDefaults(void);
void            SetupGraphx(void);
