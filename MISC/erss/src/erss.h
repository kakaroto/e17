#ifndef _MAIN_H
#define _MAIN_H

#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_Con.h>
#include <Esmart/Esmart_Trans_X11.h>
#include <Esmart/Esmart_Container.h>
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

#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <errno.h>
#include <time.h>
#include <string.h>

#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xmlerror.h>
#include <libxml2/libxml/parser.h>

#include "config.h"

#define TRUE 1
#define FALSE 0

extern Ecore_Evas  *ee;
extern Evas        *evas;
extern Evas_Object *cont;
extern int          world_x;
extern int          world_y;
extern Ecore_List  *config_files;



typedef struct _erss_config {
	char *header;
	char *hostname;
	char *url;

	char *item_root;
	char *item_start;
	char *item_title;
	char *item_url;
	char *item_description;

	int   update_rate;
	int   clock;
	int   num_stories;

	int   x;
	int   y;

	int   borderless;

	char *prefix;

	char *theme;
	char *config;
} Erss_Config;



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
  Ecore_List       *list;
  xmlDocPtr         doc;
  Erss_Article     *item;
  Erss_Config      *cfg;
} Erss_Feed;


char *erss_time_format ();

#endif
