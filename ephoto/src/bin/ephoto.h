#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <Efreet_Mime.h>
#include <Elementary.h>
#include <Eina.h>
#include <Edje.h>
#include <Evas.h>
#include <Ethumb.h>
#include <Ethumb_Client.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "config.h"

/*Main Functions*/
void ephoto_create_main_window(void);

/*Ephoto Thumb Browser*/
void ephoto_create_thumb_browser(void);
void ephoto_show_thumb_browser(void);
void ephoto_hide_thumb_browser(void);
void ephoto_delete_thumb_browser(void);
void ephoto_populate_thumbnails(void);

/*Ephoto Main Structure*/
struct _Ephoto
{
	Evas *e;
	Evas_Object *win;
	Evas_Object *bg;
	Evas_Object *box;
	Evas_Object *dir_label;
	Evas_Object *thbox;
	Evas_Object *thumb_browser;
	Evas_Object *thumb_slider;
	Evas_Object *toolbar;
	Eina_Hash   *thumbs_images;
	Eina_List   *images;
};
typedef struct _Ephoto Ephoto;
extern Ephoto *em;

#endif

