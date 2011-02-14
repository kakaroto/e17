#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eet.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Efreet_Mime.h>
#include <Elementary.h>
#include <Eina.h>
#include <Edje.h>
#include <Evas.h>
#include <Eio.h>
#include <Ethumb_Client.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/*Ephoto Typedefs*/
typedef struct	_Ephoto Ephoto;
typedef enum	_Ephoto_State Ephoto_State;

/*Main Functions*/
Evas_Object	*ephoto_window_add(void);
void		 ephoto_populate(void);
void		 ephoto_title_set(const char *title);
void		 ephoto_thumb_size_set(int size);
void		 ephoto_directory_set(const char *path);

/*Main Children*/
Evas_Object	*ephoto_flow_browser_add(void);
void		 ephoto_flow_browser_image_set(void);
void		 ephoto_flow_browser_del(void);
void		 ephoto_flow_browser_show(void);

Evas_Object	*ephoto_slideshow_add(void);
void		 ephoto_slideshow_del(void);
void		 ephoto_slideshow_show(void);

Evas_Object	*ephoto_thumb_browser_add(void);
void		 ephoto_thumb_browser_thumb_append(Eina_List *node);
void		 ephoto_thumb_browser_del(void);
void		 ephoto_thumb_browser_show(void);

/*Ephoto States*/
enum _Ephoto_State
{
   EPHOTO_STATE_THUMB,
   EPHOTO_STATE_FLOW,
   EPHOTO_STATE_SLIDESHOW
};

/*Ephoto Main Structure*/
struct _Ephoto
{
   /*Main Objects*/
   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *pager;

   /*Main Children*/
   Evas_Object *thumb_browser;
   Evas_Object *flow_browser;
   Evas_Object *slideshow;

   /*Main Variables*/
   const char *directory;
   const char *file;
   Eio_File *ls;
   Eina_List *images;
   Eina_List *current_index;
   Ephoto_State state, prev_state;
   Ethumb_Client *client;
};

extern Ephoto *ephoto;

#endif
