#ifndef _MAIN_H
#define _MAIN_H

#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_X_Cursor.h>
#include <Ecore_Evas.h>
#include <Ecore_Con.h>
#include <Evas.h>
#include <Edje.h>
#include <Ewd.h>
#include <Esmart/Esmart_Trans.h>
#include <Esmart/container.h>
#include <Etox.h>
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

#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xmlerror.h>
#include <libxml2/libxml/parser.h>

#include "config.h"

extern Evas *evas;
extern Ecore_Evas *ee;
extern Ewd_List *list;
extern Evas_Object *cont;
extern Ewd_List *config_files;
extern int world_x;
extern int world_y;

void erss_xml_error_handler (void *ctx, const char *msg, ...);
int erss_connect (void *data);
int erss_alphasort (const void *a, const void *b);

char *erss_time_format ();
int erss_set_time (void *data);

int erss_handler_signal_exit (void *data, int ev_type, void *ev);
int erss_handler_server_add (void *data, int type, void *event);
int erss_handler_server_data (void *data, int type, void *event);
int erss_handler_server_del (void *data, int type, void *event);

void erss_window_move (Ecore_Evas * ee);
void erss_window_resize (Ecore_Evas *ee);

void erss_mouse_click_item (void *data, Evas_Object *o, 
		const char *sig, const char *src);
void erss_mouse_in_cursor_change (void *data, Evas *e, Evas_Object *obj,
		void *event_info);
void erss_mouse_out_cursor_change (void *data, Evas *e, Evas_Object *obj,
		void *event_info);

void erss_list_config_files (int output);
void erss_display_default_usage ();

#endif
