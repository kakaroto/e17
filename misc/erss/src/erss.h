#ifndef _MAIN_H
#define _MAIN_H

#include <Ecore.h>
#include <Ecore_Con.h>
#include <Ewd.h>
#include <Esmart/Esmart_Trans.h>
#include <Esmart/container.h>
#include <Etox.h>
#include <math.h>
#include <unistd.h>
#include <dirent.h>

#include <Ecore_X.h>
#include <Ecore_X_Cursor.h>
#include <Ecore_Evas.h>
#include <Evas.h>
#include <Edje.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <Imlib2.h>

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <errno.h>
#include <time.h>

#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xmlerror.h>
#include <libxml2/libxml/parser.h>

#include "config.h"

extern Ecore_Evas  *ee;
extern Evas        *evas;
extern Evas_Object *cont;
extern int          world_x;
extern int          world_y;
extern Ewd_List    *config_files;



typedef struct _erss_article {
  Evas_Object *obj;
  char        *title;
  char        *url;
  char        *description;
  time_t       ts;
} Erss_Article;



typedef struct _erss_feed {
  Ecore_Con_Server *server;
  char             *main_buffer;
  char             *last_time;
  size_t            main_bufsize;
  int               waiting_for_reply;
  int               total_connects;
  Ewd_List         *list;
  xmlDocPtr         doc;
  Erss_Article     *item;
} erss_feed;


char *erss_time_format ();

#endif
