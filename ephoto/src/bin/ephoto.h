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
typedef struct	_Ephoto_Entry Ephoto_Entry;
typedef struct	_Ephoto_Event_Entry_Create Ephoto_Event_Entry_Create;
typedef enum	_Ephoto_State Ephoto_State;

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
   Ecore_Job *change_dir;
   Ecore_Timer *regen;
   Eio_File *ls;
   Eina_List *entries;
   Ephoto_State state, prev_state;
   Ethumb_Client *client;
};

/*Ephoto Entry*/
struct _Ephoto_Entry
{
   /*These variables identify each thumbnail*/
   const char *path;
   const char *basename;
   const char *label;
   Elm_Gengrid_Item *item;
   Eina_List *free_listeners;
};

/*Ephoto Entry Event*/
struct _Ephoto_Event_Entry_Create
{
   Ephoto_Entry *entry;
};

/*Main Functions*/
Evas_Object	*ephoto_window_add(const char *path);
void		 ephoto_populate(const char *path);
void		 ephoto_title_set(const char *title);
void		 ephoto_state_set(Ephoto_State state);
void             ephoto_flow_browser_show(Ephoto_Entry *entry);
void             ephoto_slideshow_show(Ephoto_Entry *entry);
void             ephoto_thumb_browser_show(Ephoto_Entry *entry);
Ephoto_Entry	*ephoto_entry_new(const char *path, const char *label);
void		 ephoto_entry_free(Ephoto_Entry *entry);
void		 ephoto_entry_free_listener_add(Ephoto_Entry *entry, void (*cb)(void *data, const Ephoto_Entry *entry), const void *data);
void		 ephoto_entry_free_listener_del(Ephoto_Entry *entry, void (*cb)(void *data, const Ephoto_Entry *entry), const void *data);
void		 ephoto_entries_free(void);

/*Main Children*/
Evas_Object	*ephoto_flow_browser_add(void);
void		 ephoto_flow_browser_entry_set(Ephoto_Entry *entry);
void		 ephoto_flow_browser_del(void);

Evas_Object	*ephoto_slideshow_add(void);
void		 ephoto_slideshow_entry_set(Ephoto_Entry *entry);
void		 ephoto_slideshow_del(void);

Evas_Object	*ephoto_thumb_browser_add(void);
void		 ephoto_thumb_browser_entry_set(Ephoto_Entry *entry);
void		 ephoto_thumb_browser_thumb_append(Eina_List *node);
void		 ephoto_thumb_browser_del(void);

/*Ephoto Events*/
extern int EPHOTO_EVENT_ENTRY_CREATE;
extern int EPHOTO_EVENT_POPULATE_START;
extern int EPHOTO_EVENT_POPULATE_END;
extern int EPHOTO_EVENT_POPULATE_ERROR;

extern Ephoto *ephoto;

#endif
