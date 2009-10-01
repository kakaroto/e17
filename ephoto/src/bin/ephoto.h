#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <Eet.h>
#include <Efreet_Mime.h>
#include <Eina.h>
#include <Edje.h>
#include <Evas.h>
#include <Ethumb.h>
#include <Ethumb_Client.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "config.h"

/*Main Functions*/
void create_main_window(void);

/*Ephoto Image Browser*/
void add_image_browser(void);
void show_image_browser(void);
void hide_image_browser(void);
void populate_thumbnails(void);

/*Ephoto Flow Browser*/
void add_flow_view(void);
void show_flow_view(Eina_List *node, Eina_List *list);
void hide_flow_view(void);

/*Ephoto Flow*/
Evas_Object *ephoto_flow_add(Evas *e);
void ephoto_flow_current_node_set(Evas_Object *obj, Eina_List *node);
void ephoto_flow_item_list_set(Evas_Object *obj, Eina_List *list);

/*Ephoto Image*/
Evas_Object *ephoto_image_add();
void ephoto_image_file_set(Evas_Object *obj, const char *file, int w, int h);
void ephoto_image_fill_inside_set(Evas_Object *obj, int fill);
Evas_Object *ephoto_image_edje_object_get(Evas_Object *obj);
void ephoto_image_size_get(Evas_Object *obj, int *w, int *h);

/*Ephoto Table*/
Evas_Object *ephoto_table_add(Evas *e);
void ephoto_table_homogenous_set(Evas_Object *obj, int homogenous);
void ephoto_table_padding_set(Evas_Object *obj, int paddingw, int paddingh);
void ephoto_table_pack(Evas_Object *obj, char *image);
void ephoto_table_viewport_set(Evas_Object *obj, int w, int h);
void ephoto_table_next_page(Evas_Object *obj);
void ephoto_table_prev_page(Evas_Object *obj);

typedef struct _Ephoto Ephoto;
struct _Ephoto
{
	Evas *e;
	Ecore_Evas *ee;
	Ethumb_Client *thumb_cli;
	Evas_Object *bg;
	Evas_Object *image_browser;
	Evas_Object *flow;
	Evas_Object *sel;
	Eina_List *images;
};
extern Ephoto *em;

#endif
