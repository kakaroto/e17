#ifndef ENNA_H_
#define ENNA_H_

#include <config.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Txt.h>
#include <Ecore_Str.h>
#include <Edje.h>
#include <Ecore_Evas.h>

#include <sqlite3.h>
#include <gettext.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fnmatch.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#if WITH_IPOD_SUPPORT
#include <gpod/itdb.h>
#endif
#define _(string) gettext(string)

//#define ENNA_DEBUG0
#define ENNA_DEBUG 2
//#define ENNA_DEBUG2

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#if ENNA_DEBUG == 2
# define dbg(fmt,arg...) fprintf(stderr, "[Enna] [%s/%d] " fmt,__FILE__,__LINE__,##arg)
#elif ENNA_DEBUG == 1
# define dbg(fmt,arg...) fprintf(stderr, "[Enna] " fmt,##arg)
#else
# define dbg(fmt,arg...) do { } while(0)
#endif

#if ENNA_DEBUG == 2
# define dbg_thm(fmt,arg...) fprintf(stderr, "[Enna Edje] [%s/%d] " fmt,__FILE__,__LINE__,##arg)
#elif ENNA_DEBUG == 1
# define dbg_thm(fmt,arg...) fprintf(stderr, "[Enna Edje] " fmt,##arg)
#else
# define dbg_thm(fmt,arg...) do { } while(0)
#endif

#define NBR_ELEMENTS(arr)		(sizeof (arr) / sizeof ((arr)[0]))
#define ENNA_FREE(p) { if (p) {free(p); p = NULL;} }

#define ENNA_MODULE_TV 0
#define ENNA_MODULE_MUSIC 1
#define ENNA_MODULE_VIDEO 2
#define ENNA_MODULE_PHOTO 3
#define ENNA_MODULE_PLAYLIST 4
#define ENNA_MODULE_MEDIAPLAYER 5

enum
{
   ENNA_EVENT_DEV_AUDIO_CD_ADD = 0,
   ENNA_EVENT_DEV_DATA_CD_ADD,
   ENNA_EVENT_DEV_DATA_DVD_ADD,
   ENNA_EVENT_DEV_USBDISK_ADD,
   ENNA_EVENT_DEV_VIDEO_DVD_ADD,
   ENNA_EVENT_DEV_CAMERA_ADD,
   ENNA_EVENT_DEV_IPOD_ADD,
   ENNA_EVENT_DEV_AUDIO_CD_REMOVE,
   ENNA_EVENT_DEV_DATA_CD_REMOVE,
   ENNA_EVENT_DEV_DATA_DVD_REMOVE,
   ENNA_EVENT_DEV_USBDISK_REMOVE,
   ENNA_EVENT_DEV_VIDEO_DVD_REMOVE,
   ENNA_EVENT_DEV_CAMERA_REMOVE,
   ENNA_EVENT_DEV_IPOD_REMOVE,
   ENNA_EVENT_NBR
};
typedef struct _Enna Enna;
typedef struct _Enna_Db Enna_Db;

struct _Enna_Db
{
   sqlite3            *db;
   unsigned int        version;
   char               *path;
   int                 fd_ev_read;
   int                 fd_ev_write;
   Ecore_Fd_Handler   *fd_ev_handler;
   pthread_t           create_db_thread;
  void              (*func) (Enna_Db * db, char *path);
  
#if WITH_IPOD_SUPPORT
   Itdb_iTunesDB      *itdb;
#endif
};

struct _Enna
{
   char               *home;
   Ecore_Evas         *ee;
   Evas               *evas;
   Evas_Object        *black_background;
   Evas_Object        *edje;
   Evas_Object        *mainmenu;
   Evas_Object        *modules;
   Evas_Object        *mediaplayer;
   Evas_Object        *miniplayer;
   Evas_Object        *watermark;
   int                 events[ENNA_EVENT_NBR];
   Ecore_List         *storage_devices;
   Ecore_List         *volumes;
   Enna_Db            *db;
   unsigned char       fs_obj:1;
#ifdef HAVE_LIRC
   int                 lirc_fd;
   struct lirc_config *lirc_cfg;
   Ecore_Timer        *lirc_timer;
   unsigned int        lirc_actions;
   char               *event_prec;
#endif
#if WITH_IPOD_SUPPORT
   Enna_Db            *ipod_db;
#endif

};

#endif
