#ifndef _MAIN_H
#define _MAIN_H

#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Ecore_Con.h>
#include <Ecore_Txt.h>
#include <Evas.h>
#include <Edje.h>
#include <Ewd.h>
#include <Esmart/Esmart_Trans.h>
#include <Esmart/container.h>
#include <math.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <unistd.h>
#include <dirent.h>

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <errno.h>
#include <time.h>

#include <assert.h>

#include "config.h"

extern Evas *evas;
extern Ecore_Evas *ee;
extern Ewd_List *list;
extern Evas_Object *cont;

void cb_mouse_out_item (void *data, Evas_Object *o, const char *sig, const char *src);
void cb_mouse_in (void *data, Evas *e, Evas_Object *obj, void *event_info);
void cb_mouse_out (void *data, Evas *e, Evas_Object *obj, void *event_info);
	

#endif
