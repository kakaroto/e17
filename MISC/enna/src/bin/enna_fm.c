/*
 * enna_fm.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_fm.c is free software copyrighted by Nicolas Aguirre.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Nicolas Aguirre'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * enna_fm.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Nicolas Aguirre OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fnmatch.h>
#include <fcntl.h>
#include <unistd.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Str.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <pthread.h>

#include "enna.h"
#include "enna_module.h"
#include "enna_list.h"
#include "enna_config.h"
#include "enna_mediaplayer.h"
#include "enna_util.h"
#include <ctype.h>

#define SMART_NAME "enna_fm"
#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;

typedef struct _E_Smart_Data E_Smart_Data;
typedef struct _Thread_Infos Thread_Infos;
typedef struct _File_List_Element File_List_Element;

struct _E_Smart_Data
{
   Evas_Coord          x, y, w, h;
   Evas_Object        *edje;
   Evas_Object        *obj;
   Evas_Object        *switcher;
   Evas_Object        *o_files;
   char               *root;
   char               *path;
   char               *filter;
   void                (*exe_file_cb) (void *data1, void *data2,
				       char *filename);
   void               *data1;
   void               *data2;
   void                (*hilight_file_cb) (void *data1, void *data2,
					   char *filename);
   void               *hilight_data1;
   void               *hilight_data2;
   void                (*exit_cb) (void *data1, void *data2);
   void               *exit_data1;
   void               *exit_data2;
   void                (*change_path_cb) (void *data, char *path);
   void               *change_path_data;
   int                 fd_ev_read;
   int                 fd_ev_write;
   Ecore_Fd_Handler   *fd_ev_handler;
   pthread_t           thread_scandir;
   unsigned int        is_scanning;
   Evas_List          *list_data;
};

struct _Thread_Infos
{
   char               *path;
   char               *oldpath;
   E_Smart_Data       *sd;
};

struct _File_List_Element
{
   char               *filename;
   char               *item_name;
   unsigned int        is_directory;
   unsigned int        is_selected;
};

/* local subsystem functions */

static void         _scan_dir(E_Smart_Data * sd, char *path, char *oldpath);
static int          _late_clear(void *data);
static void         _parent_go(E_Smart_Data * sd, char *path);
static void         _add_to_pl_cb(E_Smart_Data * sd);
static void         _add_to_pl(E_Smart_Data * sd, char *file);
static void         _e_smart_reconfigure(E_Smart_Data * sd);
static void         _e_smart_add(Evas_Object * obj);
static void         _e_smart_del(Evas_Object * obj);
static void         _e_smart_move(Evas_Object * obj, Evas_Coord x,
				  Evas_Coord y);
static void         _e_smart_resize(Evas_Object * obj, Evas_Coord w,
				    Evas_Coord h);
static void         _e_smart_show(Evas_Object * obj);
static void         _e_smart_hide(Evas_Object * obj);
static void         _e_smart_color_set(Evas_Object * obj, int r, int g, int b,
				       int a);
static void         _e_smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void         _e_smart_clip_unset(Evas_Object * obj);
static void         _e_smart_init(void);

/* local subsystem globals */
static Evas_Smart  *_e_smart = NULL;

/* externally accessible functions */
EAPI Evas_Object   *
enna_fm_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

EAPI void
enna_fm_process_event(Evas_Object * obj, enna_event event)
{
   API_ENTRY           return;

   /* Desactive key event while scanning directories */
   if (sd->is_scanning)
      return;

   if (event == enna_event_escape)
     {
	//dbg("enna_fm: go_parent ev %d %s\n", event, sd->path);
	_parent_go(sd, sd->path);
     }
   else if (event == enna_event_addtopl)
      _add_to_pl_cb(sd);
   else
      enna_list_process_event(sd->o_files, event);
}

EAPI int
enna_fm_root_set(Evas_Object * obj, char *root_path)
{
   API_ENTRY           return 0;

   if (!root_path || !ecore_file_is_dir(root_path))
      return 0;
   sd->root = strdup(root_path);
   if (sd->o_files)
      evas_object_del(sd->o_files);
   _scan_dir(sd, sd->root, NULL);
   return 1;
}

EAPI int
enna_fm_filter_set(Evas_Object * obj, char *filter)
{
   API_ENTRY           return 0;

   if (!filter)
      return 0;
   sd->filter = strdup(filter);
   return 1;
}

EAPI int
enna_fm_exe_file_cb_set(Evas_Object * obj,
			void (exe_file_cb) (void *data1, void *data2,
					    char *filename), void *data1,
			void *data2)
{
   API_ENTRY           return 0;

   sd->exe_file_cb = exe_file_cb;
   sd->data1 = data1;
   sd->data2 = data2;
   return 1;
}

EAPI int
enna_fm_hilight_file_cb_set(Evas_Object * obj,
			    void (hilight_file_cb) (void *data1, void *data2,
						    char *filename),
			    void *data1, void *data2)
{
   API_ENTRY           return 0;

   sd->hilight_file_cb = hilight_file_cb;
   sd->hilight_data1 = data1;
   sd->hilight_data2 = data2;
   return 1;
}

EAPI int
enna_fm_exit_cb_set(Evas_Object * obj,
		    void (exit_cb) (void *data1, void *data2), void *data1,
		    void *data2)
{
   API_ENTRY           return 0;

   sd->exit_cb = exit_cb;
   sd->exit_data1 = data1;
   sd->exit_data2 = data2;
   return 1;
}

EAPI int
enna_fm_change_path_cb_set(Evas_Object * obj,
			   void (change_path_cb) (void *data, char *path),
			   void *data)
{
   API_ENTRY           return 0;

   sd->change_path_cb = change_path_cb;
   sd->change_path_data = data;
   return 1;
}

EAPI void
enna_fm_parent_go(Evas_Object * obj)
{
   API_ENTRY           return;

   _parent_go(sd, sd->path);
}

EAPI char          *
enna_fm_selected_file_get(Evas_Object * obj)
{
   API_ENTRY           return NULL;

   return (char *)enna_list_selected_data2_get(sd->o_files);
}

static int
_has_suffix(char *str, Evas_List * patterns)
{
   Evas_List          *l;
   int                 result = 0;
   char               *buf, *p;

   if (!patterns || !str)
      return 0;

   buf = strdup(str);
   p = buf;
   while (*p)
     {
	*p = tolower(*p);
	p++;
     }

   for (l = patterns; l; l = evas_list_next(l))
      result |= ecore_str_has_suffix(buf, (char *)l->data);

   free(buf);

   return result;
}

static void
_directory_selected_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   char               *path;

   sd = (E_Smart_Data *) data;
   path = (char *)data2;
   if (!sd || !path || !ecore_file_is_dir(path))
      return;
   _scan_dir(sd, path, NULL);
}

static void
_hilight_file_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   char               *path;

   sd = (E_Smart_Data *) data;
   path = (char *)data2;

   if (sd->hilight_file_cb)
      sd->hilight_file_cb(sd->hilight_data1, sd->hilight_data2, path);

}

static void
_file_selected_cb(void *data, void *data2)
{
   E_Smart_Data       *sd;
   char               *path;

   sd = (E_Smart_Data *) data;
   path = (char *)data2;
   if (sd->exe_file_cb)
      sd->exe_file_cb(sd->data1, sd->data2, path);
}

static int
_late_clear(void *data)
{
   Evas_Object        *obj;

   obj = (Evas_Object *) data;
   evas_object_hide(obj);
   evas_object_del(obj);

   return 0;
}

static void        *
_scan_dir_thread(void *ptr)
{
   char               *path, *oldpath;
   void               *buf[1];
   char               *filename = NULL;
   char                dir[1024];
   E_Smart_Data       *sd;
   Ecore_List         *files = NULL;
   Evas_List          *file_list, *l;
   int                 i;
   Thread_Infos       *thread_infos;

   thread_infos = (Thread_Infos *) ptr;
   sd = thread_infos->sd;

   path = sd->path;
   oldpath = thread_infos->oldpath;

   files = ecore_file_ls(path);
   ecore_list_sort(files, ECORE_COMPARE_CB(strcasecmp), ECORE_SORT_MIN);
   filename = ecore_list_first_goto(files);

   while (sd->list_data)
     {
	File_List_Element  *e;

	e = sd->list_data->data;
	sd->list_data = evas_list_remove(sd->list_data, e);
	if (e->filename)
	   free(e->filename);
	if (e->item_name)
	   free(e->item_name);
	free(e);
     }

   sd->list_data = NULL;
   file_list = NULL;

   while ((filename = (char *)ecore_list_next(files)) != NULL)
     {
	sprintf(dir, "%s/%s", path, filename);
	if (filename[0] == '.')
	   continue;
	else if (ecore_file_is_dir(dir))
	  {
	     File_List_Element  *e;

	     e = calloc(1, sizeof(File_List_Element));
	     e->filename = strdup(dir);
	     e->item_name = strdup(filename);
	     e->is_directory = 1;
	     sd->list_data = evas_list_append(sd->list_data, e);
	     if (oldpath)
	       {
		  if (!strcmp(dir, oldpath))
		     e->is_selected = 1;
		  else
		     e->is_selected = 0;
	       }
	  }
	else if (_has_suffix(filename, enna_config_extensions_get(sd->filter)))
	  {
	     File_List_Element  *e;

	     e = calloc(1, sizeof(File_List_Element));
	     e->filename = strdup(dir);
	     e->item_name = strdup(filename);
	     file_list = evas_list_append(file_list, e);
	  }
     }

   for (i = 0, l = file_list; l; l = l->next, i++)
     {
	sd->list_data = evas_list_append(sd->list_data, l->data);
     }

   buf[0] = sd->list_data;
   write(sd->fd_ev_write, buf, sizeof(buf));
   return NULL;
}

static int
_pipe_read_active(void *data, Ecore_Fd_Handler * fdh)
{
   int                 fd;
   int                 len;
   void               *buf[1];
   E_Smart_Data       *sd;
   Evas_List          *l;
   int                 i = 0;
   char               *temp;
   Evas_Object        *o;
   unsigned int        to_select = 0;

   sd = data;
   fd = ecore_main_fd_handler_fd_get(fdh);

   while ((len = read(fd, buf, sizeof(buf))) > 0)
      if (len == sizeof(buf))
	 temp = buf[0];

   pthread_join(sd->thread_scandir, NULL);
   o = sd->o_files;
   sd->o_files = enna_list_add(evas_object_evas_get(sd->edje));
   if (!sd->list_data || !evas_list_count(sd->list_data))
     {
	File_List_Element  *e;

	e = calloc(1, sizeof(File_List_Element));
	enna_list_append(sd->o_files, NULL, "No Media", 0,
			 NULL, NULL, NULL, NULL);
     }
   else
     {
	for (l = sd->list_data; l; l = l->next)
	  {
	     File_List_Element  *e;

	     e = l->data;
	     if (e->is_directory)
	       {
		  Evas_Object        *icon;

		  icon = edje_object_add(evas_object_evas_get(sd->edje));
		  edje_object_file_set(icon, enna_config_theme_get(),
				       "icon_directory");
		  enna_list_append(sd->o_files, icon, e->item_name, 0,
				   _directory_selected_cb, NULL, sd,
				   e->filename);
		  if (e->is_selected)
		    {
		       enna_list_selected_set(sd->o_files, to_select);
		       to_select = i;
		    }
		  i++;
	       }
	     else
	       {
		  Evas_Object        *icon;
		  char                tmp[256];

		  icon = edje_object_add(evas_object_evas_get(sd->edje));
		  sprintf(tmp, "icon_%s", sd->filter);
		  edje_object_file_set(icon, enna_config_theme_get(), tmp);
		  enna_list_append(sd->o_files, icon, e->item_name, 0,
				   _file_selected_cb, _hilight_file_cb, sd,
				   e->filename);
		  i++;
	       }

	  }
     }
   enna_util_switch_objects(sd->switcher, o, sd->o_files);
   ecore_timer_add(1.0, _late_clear, o);
   enna_list_selected_set(sd->o_files, to_select);
   sd->is_scanning = 0;
   edje_object_signal_emit(sd->edje, "enna,scanning,stop", "enna");
   return 1;
}

static void
_scan_dir(E_Smart_Data * sd, char *path, char *oldpath)
{
   Thread_Infos       *thread_infos;

   if (!sd || !path || !ecore_file_is_dir(path))
      return;

   edje_object_signal_emit(sd->edje, "enna,scanning,start", "enna");
   sd->path = strdup(path);

   if (sd->change_path_cb)
      sd->change_path_cb(sd->change_path_data, sd->path);

   thread_infos = calloc(1, sizeof(Thread_Infos));
   thread_infos->path = path;
   thread_infos->oldpath = oldpath;
   thread_infos->sd = sd;

   pthread_create(&sd->thread_scandir, NULL, _scan_dir_thread,
		  (void *)thread_infos);
   sd->is_scanning = 1;

}

static void
_parent_go(E_Smart_Data * sd, char *path)
{
   char               *p;
   char               *path_tmp;

   if (!sd || !path || !sd->root)
      return;

   if (!strcmp(path, sd->root))
     {
	if (sd->exit_cb)
	   sd->exit_cb(sd->exit_data1, sd->exit_data2);
	return;
     }
   path_tmp = strdup(path);

   if (path_tmp[strlen(path_tmp) - 1] == '/')
      path_tmp[strlen(path_tmp) - 1] = 0;
   p = strrchr(path_tmp, '/');
   if (p && *(p - 1) == '/')
      *(p) = 0;
   else if (p)
      *(p) = 0;
   _scan_dir(sd, path_tmp, path);
   // Fixme: memory leak
}

static void
_add_to_pl(E_Smart_Data * sd, char *file)
{
   Enna               *enna;
   Ecore_List         *files = NULL;
   char               *filename;

   enna = evas_data_attach_get(evas_object_evas_get(sd->edje));
   if (!sd || !file || !enna)
      return;

   if (ecore_file_is_dir(file))
     {
	//enna_mediaplayer_playlist_add(enna->mediaplayer, file);
	files = ecore_file_ls(file);
	filename = ecore_list_first_goto(files);
	while ((filename = (char *)ecore_list_next(files)) != NULL)
	  {
	     char                tmp[4096];

	     sprintf(tmp, "%s/%s", file, filename);
	     //_add_to_pl(sd, tmp);
	  }
     }
}

static void
_add_to_pl_cb(E_Smart_Data * sd)
{
   char               *file;

   file = (char *)enna_list_selected_data2_get(sd->o_files);

   enna_list_in_playlist_set(sd->o_files, 1,
			     enna_list_selected_get(sd->o_files));

   _add_to_pl(sd, file);
}

static void
_e_smart_reconfigure(E_Smart_Data * sd)
{
   evas_object_move(sd->edje, sd->x, sd->y);
   evas_object_resize(sd->edje, sd->w, sd->h);
}

static void
_e_smart_add(Evas_Object * obj)
{
   E_Smart_Data       *sd;
   int                 fds[2];

   sd = calloc(1, sizeof(E_Smart_Data));
   if (!sd)
      return;
   evas_object_smart_data_set(obj, sd);

   sd->obj = obj;
   sd->x = 0;
   sd->y = 0;
   sd->w = 0;
   sd->h = 0;

   sd->edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->edje, enna_config_theme_get(), "enna/filemanager");
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_show(sd->edje);

   sd->switcher = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->switcher, enna_config_theme_get(), "enna/switcher");

   evas_object_smart_member_add(sd->switcher, obj);
   evas_object_smart_member_add(sd->edje, obj);
   evas_object_smart_member_add(sd->o_files, obj);

   evas_object_show(sd->switcher);
   edje_object_part_swallow(sd->edje, "enna.switcher.swallow", sd->switcher);
   sd->o_files = NULL;
   sd->root = NULL;
   sd->path = NULL;
   sd->filter = NULL;
   sd->exe_file_cb = NULL;
   sd->data1 = NULL;
   sd->data2 = NULL;

   sd->is_scanning = 0;

   if (pipe(fds) == 0)
     {
	sd->fd_ev_read = fds[0];
	sd->fd_ev_write = fds[1];
	fcntl(sd->fd_ev_read, F_SETFL, O_NONBLOCK);
	sd->fd_ev_handler = ecore_main_fd_handler_add(sd->fd_ev_read,
						      ECORE_FD_READ,
						      _pipe_read_active,
						      sd, NULL, NULL);
	ecore_main_fd_handler_active_set(sd->fd_ev_handler, ECORE_FD_READ);
     }
   evas_object_propagate_events_set(obj, 0);
}

static void
_e_smart_del(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_del(sd->o_files);
   evas_object_del(sd->switcher);
   evas_object_del(sd->edje);
   ENNA_FREE(sd->root);
   ENNA_FREE(sd->path);
   ENNA_FREE(sd->filter);
   ENNA_FREE(sd);
}

static void
_e_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
   INTERNAL_ENTRY;
   if ((sd->x == x) && (sd->y == y))
      return;
   sd->x = x;
   sd->y = y;
   _e_smart_reconfigure(sd);
}

static void
_e_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
   INTERNAL_ENTRY;

   if ((sd->w == w) && (sd->h == h))
      return;
   sd->w = w;
   sd->h = h;
   _e_smart_reconfigure(sd);

}

static void
_e_smart_show(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_show(sd->edje);
}

static void
_e_smart_hide(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_hide(sd->edje);
}

static void
_e_smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
   INTERNAL_ENTRY;
   evas_object_color_set(sd->edje, r, g, b, a);
}

static void
_e_smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
   INTERNAL_ENTRY;
   evas_object_clip_set(sd->edje, clip);
}

static void
_e_smart_clip_unset(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_clip_unset(sd->edje);
}

/* never need to touch this */

static void
_e_smart_init(void)
{
   if (_e_smart)
      return;
   static const Evas_Smart_Class sc = {
      SMART_NAME,
      EVAS_SMART_CLASS_VERSION,
      _e_smart_add,
      _e_smart_del,
      _e_smart_move,
      _e_smart_resize,
      _e_smart_show,
      _e_smart_hide,
      _e_smart_color_set,
      _e_smart_clip_set,
      _e_smart_clip_unset,
      NULL
   };
   _e_smart = evas_smart_class_new(&sc);
}
