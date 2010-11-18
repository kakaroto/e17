/*
 * Eyesight - EFL-based document renderer
 * Copyright (C) 2010 Vincent Torri <vtorri at univ-evry dot fr>
 * Copyright (C) 2010 Fabiano Fidencio
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Eyesight.h"
#include "eyesight_private.h"

#define DBG(...) EINA_LOG_DOM_DBG(_eyesight_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_eyesight_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_eyesight_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_eyesight_log_domain, __VA_ARGS__)
#define CRIT(...) EINA_LOG_DOM_CRIT(_eyesight_log_domain, __VA_ARGS__)


#define E_SMART_OBJ_GET(smart, o, type)               \
   {                                                  \
      char *_e_smart_str;                             \
                                                      \
      if (!o) return;                                 \
      smart = evas_object_smart_data_get(o);          \
      if (!smart) return;                             \
      _e_smart_str = (char *)evas_object_type_get(o); \
      if (!_e_smart_str) return;                      \
      if (strcmp(_e_smart_str, type)) return;         \
   }

#define E_SMART_OBJ_GET_RETURN(smart, o, type, ret)   \
   {                                                  \
      char *_e_smart_str;                             \
                                                      \
      if (!o) return ret;                             \
      smart = evas_object_smart_data_get(o);          \
      if (!smart) return ret;                         \
      _e_smart_str = (char *)evas_object_type_get(o); \
      if (!_e_smart_str) return ret;                  \
      if (strcmp(_e_smart_str, type)) return ret;     \
   }

#define E_OBJ_NAME "eyesight_object"

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
   Eyesight_Module *module;
   void            *backend;

   char            *module_filename;

   void            *doc;
   char            *filename;
   Evas_Object     *obj;
};

static Evas_Smart  *smart = NULL; /* The smart object */
static Eina_Hash  *_eyesight_backends = NULL; /* the list of backends */
static Eina_Array *_eyesight_modules = NULL; /* the list of shared lib corresponding to a backend */
static int _eyesight_log_domain = -1;

static const char *_eyesight_backend_priority[] = {
  "img",
  "pdf",
  "ps",
  "dvi",
  "rtf",
  "txt"
};

static const Evas_Smart_Cb_Description _smart_callbacks[] = {
  {NULL, NULL}
};

static void _smart_init(void);
static void _smart_add(Evas_Object * obj);
static void _smart_del(Evas_Object * obj);
static void _smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object * obj);
static void _smart_hide(Evas_Object * obj);
static void _smart_color_set(Evas_Object * obj, int r, int g, int b, int a);
static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object * obj);

static void
_eyesight_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  int w;
  int h;

  evas_object_geometry_get(obj, NULL, NULL, &w, &h);
  evas_object_resize((Evas_Object *)data, w, h);
}

static const char *
_eyesight_module_open(const char *name, Evas *evas, Evas_Object **obj, Eyesight_Module **module, void **backend)
{
   Eyesight_Plugin *plugin = NULL;
   unsigned int     i = 0;

   if (!_eyesight_backends)
     {
        ERR("No backend loaded");
        return NULL;
     }

   /* FIXME: Always look for a working backend. */
 retry:
   if (!name || i > 0)
     name = _eyesight_backend_priority[i++];

   plugin = eina_hash_find(_eyesight_backends, name);
   if (!plugin)
     {
        if (i != 0 && i < (sizeof (_eyesight_backend_priority) / sizeof (char*)))
          goto retry;

        ERR("No backend loaded");
        return NULL;
     }

   if (plugin->m_open(evas, obj, (const Eyesight_Module **)module, backend))
     {
        if (*module)
          {
             (*module)->plugin = plugin;
             return name;
          }
     }

   if (i != 0 && i < (sizeof (_eyesight_backend_priority) / sizeof (char*)))
     goto retry;

   ERR("Unable to load module: %s", name);

   return NULL;
}

static void
_eyesight_module_close(Eyesight_Module *module, void *backend)
{
   if (!module) return;
   if (module->plugin->m_close && backend)
     module->plugin->m_close(module, backend);
   /* FIXME: we can't go dlclosing here as a thread still may be running from
    * the module - this in theory will leak- but it shouldnt be too bad and
    * mean that once a module is dlopened() it cant be closed - its refcount
    * will just keep going up
    */
}

EAPI Eina_Bool
 _eyesight_module_register(const char *name, Eyesight_Module_Open m_open, Eyesight_Module_Close m_close)
{
   Eyesight_Plugin *plugin;

   plugin = malloc(sizeof (Eyesight_Plugin));
   if (!plugin) return EINA_FALSE;

   plugin->m_open = m_open;
   plugin->m_close = m_close;

   return eina_hash_add(_eyesight_backends, name, plugin);
}

EAPI Eina_Bool
_eyesight_module_unregister(const char *name)
{
   return eina_hash_del(_eyesight_backends, name, NULL);
}


/* API */

EAPI Evas_Object *
eyesight_object_add(Evas *evas)
{
   _smart_init();
   return evas_object_smart_add(evas, smart);
}

EAPI Eyesight_Backend
eyesight_object_init(Evas_Object *obj, const char *module_filename)
{
   Smart_Data *sd;
   char *filename;
   const char *modulename;

   E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, EINA_FALSE);

   DBG("module filename=%s", module_filename);

   if ((sd->module_filename) && module_filename && (!strcmp(sd->module_filename, module_filename)))
     goto backend_success;

   if (sd->module_filename)
     {
        free(sd->module_filename);
        sd->module_filename = NULL;
     }

   filename = sd->filename;
   sd->filename = NULL;

   _eyesight_module_close(sd->module, sd->backend);
   sd->module = NULL;
   sd->backend = NULL;

   modulename = _eyesight_module_open(module_filename, evas_object_evas_get(obj), &sd->obj, &sd->module, &sd->backend);
   if (!modulename)
     return EYESIGHT_BACKEND_NONE;

   evas_object_smart_member_add(sd->obj, obj);
   evas_object_smart_data_set(obj, sd);
   evas_object_event_callback_add(sd->obj, EVAS_CALLBACK_RESIZE, _eyesight_resize_cb, obj);

   sd->module_filename = strdup(modulename);
   if (!sd->module_filename)
     return EYESIGHT_BACKEND_NONE;

   if (filename)
     {
        eyesight_object_file_set(obj, filename);
        free(filename);
     }

 backend_success:
   if (strcmp(sd->module_filename, "img") == 0)
     return EYESIGHT_BACKEND_IMG;
   if (strcmp(sd->module_filename, "pdf") == 0)
     return EYESIGHT_BACKEND_PDF;
   if (strcmp(sd->module_filename, "ps") == 0)
     return EYESIGHT_BACKEND_PS;
   if (strcmp(sd->module_filename, "txt") == 0)
     return EYESIGHT_BACKEND_TXT;
}

EAPI void *
eyesight_object_file_set(Evas_Object *obj, const char *filename)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, EINA_FALSE);

   DBG("filename=%s", filename);

   if (!sd->module)
     return NULL;

   if ((filename) && (sd->filename) && (!strcmp(filename, sd->filename)))
     return sd->doc;

   if (filename && *filename)
     {
        free(sd->filename);
        sd->filename = strdup(filename);
        if (!sd->filename)
          return NULL;

        sd->module->file_close(sd->backend);
        if (!(sd->doc = sd->module->file_open(sd->backend, sd->filename)))
          return NULL;
     }
   else
     {
        if (sd->backend && sd->module)
          {
             sd->module->file_close(sd->backend);
          }
        free(sd->filename);
        sd->filename = NULL;
     }

   return sd->doc;
}

EAPI const char *
eyesight_object_file_get(Evas_Object *obj)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, NULL);

   return sd->filename;
}

EAPI const Eina_List *
eyesight_object_toc_get(Evas_Object *obj)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, NULL);

   if (!sd->module)
     return NULL;

   return sd->module->toc_get(sd->backend);
}

EAPI int
eyesight_object_page_count(Evas_Object *obj)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, 0);

   if (!sd->module)
     return 0;

   return sd->module->page_count(sd->backend);
}

EAPI void
eyesight_object_page_set(Evas_Object *obj, int page)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);

   DBG("page number=%d", page);

   if (!sd->module)
     return;

   sd->module->page_set(sd->backend, page);
}

EAPI int
eyesight_object_page_get(Evas_Object *obj)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, 0);

   if (!sd->module)
     return 0;

   return sd->module->page_get(sd->backend);
}

EAPI void
eyesight_object_page_scale_set(Evas_Object *obj, double hscale, double vscale)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);

   DBG("horizontal scale=%f vertical scale=%f", hscale, vscale);

   if (!sd->module)
     return;

   sd->module->page_scale_set(sd->backend, hscale, vscale);
}

EAPI void
eyesight_object_page_scale_get(Evas_Object *obj, double *hscale, double *vscale)
{
   Smart_Data *sd;
   char *_e_smart_str;

   E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);

   if (!obj)
     {
       if (hscale) *hscale = 1.0;
       if (vscale) *vscale = 1.0;
       return;
     }
   smart = evas_object_smart_data_get(obj);
   if (!smart)
     {
       if (hscale) *hscale = 1.0;
       if (vscale) *vscale = 1.0;
       return;
     }
   _e_smart_str = (char *)evas_object_type_get(obj);
   if (!_e_smart_str)
     {
       if (hscale) *hscale = 1.0;
       if (vscale) *vscale = 1.0;
       return;
     }
   if (strcmp(_e_smart_str, E_OBJ_NAME))
     {
       if (hscale) *hscale = 1.0;
       if (vscale) *vscale = 1.0;
       return;
     }

   if (!sd->module)
     {
       if (hscale) *hscale = 1.0;
       if (vscale) *vscale = 1.0;
       return;
     }

   sd->module->page_scale_get(sd->backend, hscale, vscale);
}

EAPI void
eyesight_object_page_orientation_set(Evas_Object *obj, Eyesight_Orientation orientation)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);

   DBG("orientation=%d", orientation);

   if (!sd->module)
     return;

   sd->module->page_orientation_set(sd->backend, orientation);
}

EAPI Eyesight_Orientation
eyesight_object_page_orientation_get(Evas_Object *obj)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, EYESIGHT_ORIENTATION_PORTRAIT);

   if (!sd->module)
     return EYESIGHT_ORIENTATION_PORTRAIT;

   return sd->module->page_orientation_get(sd->backend);
}

EAPI void
eyesight_object_page_size_get(Evas_Object *obj,
                              int *width,
                              int *height)
{
   Smart_Data *sd;
   char *_e_smart_str;

   E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);

   if (!obj)
     {
       if (width) *width = 0;
       if (height) *height = 0;
       return;
     }
   smart = evas_object_smart_data_get(obj);
   if (!smart)
     {
       if (width) *width = 0;
       if (height) *height = 0;
       return;
     }
   _e_smart_str = (char *)evas_object_type_get(obj);
   if (!_e_smart_str)
     {
       if (width) *width = 0;
       if (height) *height = 0;
       return;
     }
   if (strcmp(_e_smart_str, E_OBJ_NAME))
     {
       if (width) *width = 0;
       if (height) *height = 0;
       return;
     }

   if (!sd->module)
     {
       if (width) *width = 0;
       if (height) *height = 0;
       return;
     }

   sd->module->page_size_get(sd->backend, width, height);
}

EAPI void
eyesight_object_page_render(Evas_Object *obj)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET(sd, obj, E_OBJ_NAME);

   if (!sd->module)
     return;

   sd->module->page_render(sd->backend);
}

EAPI char *
eyesight_object_page_text_get(Evas_Object *obj,
                              Eina_Rectangle rect)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, NULL);

   if (!sd->module)
     return NULL;

   return sd->module->page_text_get(sd->backend, rect);
}

EAPI Eina_List *
eyesight_object_page_text_find(Evas_Object *obj,
                               const char *text,
                               Eina_Bool is_case_sensitive,
                               Eina_Bool backward)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, NULL);

   if (!sd->module)
     return NULL;

   return sd->module->page_text_find(sd->backend, text, is_case_sensitive, backward);
}

EAPI Eina_List *
eyesight_object_page_links_get(Evas_Object *obj)
{
   Smart_Data *sd;

   E_SMART_OBJ_GET_RETURN(sd, obj, E_OBJ_NAME, NULL);

   if (!sd->module)
     return NULL;

   return sd->module->page_links_get(sd->backend);
}


/*******************************************/
/* Internal smart object required routines */
/*******************************************/

#ifdef EINA_STATIC_BUILD_IMG
Eina_Bool img_module_init(void);
#endif

#ifdef EINA_STATIC_BUILD_PDF
Eina_Bool pdf_module_init(void);
#endif

#ifdef EINA_STATIC_BUILD_POSTSCRIPT
Eina_Bool ps_module_init(void);
#endif

#ifdef EINA_STATIC_BUILD_TXT
Eina_Bool txt_module_init(void);
#endif

static void
_smart_init(void)
{
   static Evas_Smart_Class sc = EVAS_SMART_CLASS_INIT_NAME_VERSION(E_OBJ_NAME);
   char *path;

   if (smart) return;

   if (!eina_init())
     return;

   _eyesight_log_domain = eina_log_domain_register("eyesight", EINA_COLOR_LIGHTCYAN);
   if (_eyesight_log_domain < 0)
     {
        EINA_LOG_CRIT("Could not register log domain 'eyesight'");
        goto shutdown_eina;
     }


   _eyesight_backends = eina_hash_string_small_new(free);
   if (!_eyesight_backends)
     {
        goto unregister_log_domain;
     }

   _eyesight_modules = eina_module_list_get(NULL, PACKAGE_LIB_DIR "/eyesight/", 0, NULL, NULL);

   path = eina_module_environment_path_get("HOME", "/.eyesight/");
   _eyesight_modules = eina_module_list_get(_eyesight_modules, path, 0, NULL, NULL);
   if (path) free(path);

   path = eina_module_environment_path_get("EYESIGHT_MODULES_DIR", "/eyesight/");
   _eyesight_modules = eina_module_list_get(_eyesight_modules, path, 0, NULL, NULL);
   if (path) free(path);

   path = eina_module_symbol_path_get(eyesight_object_add, "/eyesight/");
   _eyesight_modules = eina_module_list_get(_eyesight_modules, path, 0, NULL, NULL);
   if (path) free(path);

   if (!_eyesight_modules)
     {
        ERR("No module found!");
        goto free_backends;
     }

   eina_module_list_load(_eyesight_modules);

#ifdef EYESIGHT_STATIC_BUILD_IMG
   img_module_init();
#endif

#ifdef EYESIGHT_STATIC_BUILD_POPPLER
   pdf_module_init();
#endif

#ifdef EYESIGHT_STATIC_BUILD_POSTSCRIPT
   ps_module_init();
#endif

#ifdef EYESIGHT_STATIC_BUILD_TXT
   txt_module_init();
#endif

   if (!sc.add)
     {
        sc.add = _smart_add;
        sc.del = _smart_del;
        sc.move = _smart_move;
        sc.resize = _smart_resize;
        sc.show = _smart_show;
        sc.hide = _smart_hide;
        sc.color_set = _smart_color_set;
        sc.clip_set = _smart_clip_set;
        sc.clip_unset = _smart_clip_unset;
        sc.callbacks = _smart_callbacks;
     }
   smart = evas_smart_class_new(&sc);

   return;

 free_backends:
   eina_hash_free(_eyesight_backends);
 unregister_log_domain:
   eina_log_domain_unregister(_eyesight_log_domain);
   _eyesight_log_domain = -1;
 shutdown_eina:
   eina_shutdown();
}

static void
_smart_add(Evas_Object * obj)
{
   Smart_Data *sd;
/*    unsigned int *pixel; */

   sd = calloc(1, sizeof(Smart_Data));
   if (!sd) return;

/*    evas_object_event_callback_add(sd->obj, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move, sd); */
/*    evas_object_event_callback_add(sd->obj, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down, sd); */
   evas_object_smart_data_set(obj, sd);
}

static void
_smart_del(Evas_Object * obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   if (sd->backend) sd->module->file_close(sd->backend);
   _eyesight_module_close(sd->module, sd->backend);
   evas_object_del(sd->obj);
   free(sd->filename);
   free(sd->module_filename);
   free(sd);

   eina_log_domain_unregister(_eyesight_log_domain);
   _eyesight_log_domain = -1;
   eina_shutdown();
}

static void
_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   evas_object_move(sd->obj, x, y);
}

static void
_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   /* FIXME: not always image object */
   evas_object_image_fill_set(sd->obj, 0, 0, w, h);
   evas_object_resize(sd->obj, w, h);
}

static void
_smart_show(Evas_Object * obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   evas_object_show(sd->obj);

}

static void
_smart_hide(Evas_Object * obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   evas_object_hide(sd->obj);
}

static void
_smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   evas_object_color_set(sd->obj, r, g, b, a);
}

static void
_smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   evas_object_clip_set(sd->obj, clip);
}

static void
_smart_clip_unset(Evas_Object * obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;

   evas_object_clip_unset(sd->obj);
}

