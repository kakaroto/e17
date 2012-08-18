/*
 * Eyesight - EFL-based document renderer
 * Copyright (C) 2010 Vincent Torri <vtorri at univ-evry dot fr>
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

#include <Evas.h>

#include <libspectre/spectre.h>

#include "Eyesight_Module_Ps.h"
#include "Eyesight.h"
#include "eyesight_private.h"
#include "eyesight_ps.h"


#define EYESIGHT_PS_INFO_STRING_GET(member, info)   \
  do { \
    const char *data; \
    data = spectre_document_get_##info (ebp->document); \
    if (spectre_document_status(ebp->document) != SPECTRE_STATUS_SUCCESS) \
      doc->member = NULL; \
    else \
      doc->member = data; \
  } while(0)

#define EYESIGHT_PS_INFO_INT_GET(member, info)       \
  do { \
    int data; \
    data = spectre_document_##info(ebp->document); \
    if (spectre_document_status(ebp->document) != SPECTRE_STATUS_SUCCESS) \
      doc->member = 0; \
    else \
      doc->member = data; \
  } while(0)


#define DBG(...) EINA_LOG_DOM_DBG(_eyesight_ps_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_eyesight_ps_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_eyesight_ps_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_eyesight_ps_log_domain, __VA_ARGS__)
#define CRIT(...) EINA_LOG_DOM_CRIT(_eyesight_ps_log_domain, __VA_ARGS__)


static int _eyesight_ps_count = 0;
static int _eyesight_ps_log_domain = -1;

static Eina_Bool
em_init(Evas *evas, Evas_Object **obj, void **eyesight_backend)
{
  Eyesight_Backend_Ps *ebp;

  if (!eyesight_backend)
    return EINA_FALSE;

  ebp = (Eyesight_Backend_Ps *)calloc(1, sizeof(Eyesight_Backend_Ps));
  if (!ebp)
    return EINA_FALSE;

  ebp->document = spectre_document_new();
  if (!ebp->document)
    goto free_ebp;

  ebp->obj = evas_object_image_add(evas);
  if (!ebp->obj)
    goto free_document;

  *obj = ebp->obj;
  *eyesight_backend = ebp;

  return EINA_TRUE;

 free_document:
  free(ebp->document);
 free_ebp:
  free(ebp);

  return EINA_FALSE;
}

static void
em_shutdown(void *eb)
{
  Eyesight_Backend_Ps *ebp;

  if (!eb)
    return;

  ebp = (Eyesight_Backend_Ps *)eb;
  evas_object_del(ebp->obj);
  free(ebp->document);
  free(eb);
}

static void *
em_file_open(void *eb, const char *filename)
{
  Eyesight_Backend_Ps *ebp;
  SpectreStatus    status;
  Eyesight_Document_Ps *doc;

  if (!eb || !filename || !*filename)
    return NULL;

  DBG("Open file %s", filename);

  ebp = (Eyesight_Backend_Ps *)eb;
  ebp->filename = strdup(filename);
  if (!ebp->filename)
    return NULL;

  spectre_document_load(ebp->document, ebp->filename);
  status = spectre_document_status(ebp->document);
  if (status != SPECTRE_STATUS_SUCCESS)
    {
      ERR("Could not load %s (%s)", ebp->filename, spectre_status_to_string(status));
      goto free_filename;
   }

  ebp->page_count = spectre_document_get_n_pages(ebp->document);
  status = spectre_document_status(ebp->document);
  if (status != SPECTRE_STATUS_SUCCESS)
    {
      ERR("Could not get page count (%s)", spectre_status_to_string (status));
      goto free_filename;
   }

  doc = (Eyesight_Document_Ps *)malloc(sizeof(Eyesight_Document_Ps));
  if (!doc)
    goto free_filename;

  doc->filename = ebp->filename;

  EYESIGHT_PS_INFO_STRING_GET(title, title);
  EYESIGHT_PS_INFO_STRING_GET(author, creator);
  EYESIGHT_PS_INFO_STRING_GET(for_, for);
  EYESIGHT_PS_INFO_STRING_GET(format, format);
  EYESIGHT_PS_INFO_STRING_GET(date_creation, creation_date);

  EYESIGHT_PS_INFO_INT_GET(is_eps, is_eps);
  EYESIGHT_PS_INFO_INT_GET(language_level, get_language_level);
  ebp->doc = doc;

  return doc;

 free_filename:
  free(ebp->filename);
  ebp->filename = NULL;

  return NULL;
}

static void
em_file_close(void *eb)
{
  Eyesight_Backend_Ps *ebp;

  if (!eb)
    return;

  ebp = (Eyesight_Backend_Ps *)eb;

  DBG("Close file %s", ebp->filename);

  if (ebp->doc)
    {
      free(ebp->doc);
      ebp->doc = NULL;
    }
  if (ebp->filename)
    {
      free(ebp->filename);
      ebp->filename = NULL;
    }
}

static Eina_Bool
em_is_locked(void *eb __UNUSED__)
{
  return EINA_FALSE;
}

static Eina_Bool
em_password_set(void *eb __UNUSED__, const char *password __UNUSED__)
{
  return EINA_TRUE;
}

static const Eina_List *
em_toc_get(void *eb)
{
  return NULL;
}

static int
em_page_count(void *eb)
{
  Eyesight_Backend_Ps *ebp;

  if (!eb)
    return 0;

  ebp = (Eyesight_Backend_Ps *)eb;

  return ebp->page_count;
}

static void
em_page_set(void *eb, int page)
{
  Eyesight_Backend_Ps *ebp;
  SpectreStatus status;

  if (!eb)
    return;

  if (page < 0)
    {
       ERR("Page number is negative");
       return;
    }

  ebp = (Eyesight_Backend_Ps *)eb;

  if (page >= ebp->page_count)
    {
       ERR("Page number is beyond the maximal number of pages");
       return;
    }

  ebp->page = spectre_document_get_page(ebp->document, page);
  status = spectre_page_status (ebp->page);
  if (status != SPECTRE_STATUS_SUCCESS)
    {
      ERR("Could not create page #%d (%s)", page, spectre_status_to_string (status));
      ebp->page = NULL;
      return;
  }

   ebp->rc = spectre_render_context_new();
   if (!ebp->rc)
     {
       ERR("Could not create render context (%s)", spectre_status_to_string (status));
       free (ebp->page);
       ebp->page = NULL;
       return;
     }

   spectre_render_context_set_rotation (ebp->rc, EYESIGHT_ORIENTATION_PORTRAIT);
   spectre_render_context_set_scale (ebp->rc, 1.0, 1.0);

  DBG("page=%d", page);
}

static int
em_page_get(void *eb)
{
  Eyesight_Backend_Ps *ebp;

  if (!eb)
    return 0;

  ebp = (Eyesight_Backend_Ps *)eb;

  if (!ebp->page)
    return 0;

  return spectre_page_get_index(ebp->page);
}

static void
em_page_scale_set(void *eb, double hscale, double vscale)
{
  Eyesight_Backend_Ps *ebp;

  if (!eb)
    return;

  DBG("horizontal scale=%f vertical scale=%f", hscale, vscale);

  ebp = (Eyesight_Backend_Ps *)eb;

  spectre_render_context_set_scale (ebp->rc, hscale, vscale);
}

static void
em_page_scale_get(void *eb, double *hscale, double *vscale)
{
  Eyesight_Backend_Ps *ebp;
  double hs = 1.0;
  double vs = 1.0;

  if (!eb)
    {
      if (hscale) *hscale = 1.0;
      if (vscale) *vscale = 1.0;
    }

  ebp = (Eyesight_Backend_Ps *)eb;

  spectre_render_context_get_scale (ebp->rc, &hs, &vs);
  if (hscale) *hscale = hs;
  if (vscale) *vscale = vs;
}

static void
em_page_orientation_set(void *eb, Eyesight_Orientation orientation)
{
  Eyesight_Backend_Ps *ebp;

  if (!eb)
    return;

  DBG("orientation=%d", orientation);

  ebp = (Eyesight_Backend_Ps *)eb;

  spectre_render_context_set_rotation(ebp->rc, orientation);
}

static Eyesight_Orientation
em_page_orientation_get(void *eb)
{
  Eyesight_Backend_Ps *ebp;

  if (!eb)
    return EYESIGHT_ORIENTATION_PORTRAIT;

  ebp = (Eyesight_Backend_Ps *)eb;

  return spectre_render_context_get_rotation(ebp->rc);
}

void
em_page_size_get(void *eb, int *width, int *height)
{
  Eyesight_Backend_Ps *ebp;
  int rotate;
  int w;
  int h;

  if (!eb)
    {
      if (width) *width = 0;
      if (height) *height = 0;
      return;
    }

  ebp = (Eyesight_Backend_Ps *)eb;

  spectre_page_get_size(ebp->page, &w, &h);
  rotate = spectre_render_context_get_rotation(ebp->rc);
  switch (rotate)
    {
    case SPECTRE_ORIENTATION_REVERSE_PORTRAIT:
    case SPECTRE_ORIENTATION_PORTRAIT:
      if (width) *width = w;
      if (height) *height = h;
      return;
      case SPECTRE_ORIENTATION_LANDSCAPE:
      case SPECTRE_ORIENTATION_REVERSE_LANDSCAPE:
         if (width) *width = h;
         if (height) *height = w;
         return;
     }
}

static void
em_page_render(void *eb)
{
  Eyesight_Backend_Ps *ebp;
  unsigned char *data;
  unsigned char *d;
  unsigned int    *m = NULL;
  unsigned int    *tmp;
  double         hscale;
  double         vscale;
  int              width;
  int              height;
  int            stride;
  int            yy;
  SpectreStatus    status;

  if (!eb)
    return;

  ebp = (Eyesight_Backend_Ps *)eb;

  if (!ebp->page)
    em_page_set(eb, 0);
  if (!ebp->page)
    {
      ERR("Could not create page");
      return;
    }

  spectre_page_render(ebp->page, ebp->rc,
                      &data, &stride);
  status = spectre_page_status(ebp->page);
  if (status != SPECTRE_STATUS_SUCCESS)
    {
      ERR("Could not render page (%s)", spectre_status_to_string (status));
      return;
   }

  em_page_scale_get(ebp, &hscale, &vscale);
  em_page_size_get(ebp, &width, &height);

  width *= hscale;
  height *= vscale;

  evas_object_image_size_set(ebp->obj, width, height);
  evas_object_image_fill_set(ebp->obj, 0, 0, width, height);
  m = (unsigned int *)evas_object_image_data_get(ebp->obj, 1);
  if (!m)
    {
       ERR("Could not retrieve data from the Evas Object");
       return;
    }

  printf (" * %d %d\n", stride, 4*width);
  if (stride == (4 * width))
    memcpy(m, data, height * stride);
  else
    {
      d = data;
      tmp = m;
      for (yy = 0; yy < height; d += stride, tmp += width, ++yy)
        {
          memcpy (tmp, d, width * 4);
        }
    }
  evas_object_image_data_set(ebp->obj, m);
  evas_object_image_data_update_add(ebp->obj, 0, 0, width, height);
  evas_object_resize(ebp->obj, width, height);
}

char *
em_page_text_get(void *eb, Eina_Rectangle rect)
{
  return NULL;
}

Eina_List *
em_page_text_find(void *eb, const char *text, Eina_Bool is_case_sensitive, Eina_Bool backward)
{
  return NULL;
}

Eina_List *
em_page_links_get(void *eb)
{
  return NULL;
}

static Eyesight_Module _eyesight_module_ps =
{
  em_init,
  em_shutdown,
  em_file_open,
  em_file_close,
  em_is_locked,
  em_password_set,
  em_toc_get,
  em_page_count,
  em_page_set,
  em_page_get,
  em_page_scale_set,
  em_page_scale_get,
  em_page_orientation_set,
  em_page_orientation_get,
  em_page_size_get,
  em_page_render,
  em_page_text_get,
  em_page_text_find,
  em_page_links_get,

  NULL
};

static Eina_Bool
module_open(Evas *evas, Evas_Object **obj, const Eyesight_Module **module, void **backend)
{
   if (!module)
      return EINA_FALSE;

   if (_eyesight_ps_count == 0)
     {
       if (_eyesight_ps_log_domain < 0)
         {
           _eyesight_ps_log_domain = eina_log_domain_register("eyesight-ps", EINA_COLOR_LIGHTCYAN);
           if (_eyesight_ps_log_domain < 0)
             {
               EINA_LOG_CRIT("Could not register log domain 'eyesight-ps'");
               return EINA_FALSE;
             }
         }
     }

   _eyesight_ps_count++;

   if (!_eyesight_module_ps.init(evas, obj, backend))
     {
        ERR("Could not initialize module");
        eina_log_domain_unregister(_eyesight_ps_log_domain);
        _eyesight_ps_log_domain = -1;
        _eyesight_ps_count--;
       return EINA_FALSE;
     }

   *module = &_eyesight_module_ps;
   return EINA_TRUE;
}

static void
module_close(Eyesight_Module *module, void *backend)
{
  _eyesight_ps_count--;
  if (_eyesight_ps_count == 0)
    {
      eina_log_domain_unregister(_eyesight_ps_log_domain);
      _eyesight_ps_log_domain = -1;
    }
   _eyesight_module_ps.shutdown(backend);
}

Eina_Bool
ps_module_init(void)
{
   return _eyesight_module_register("ps", module_open, module_close);
}

void
ps_module_shutdown(void)
{
   _eyesight_module_unregister("ps");
}

#ifndef EYESIGHT_STATIC_BUILD_POPPLER

EINA_MODULE_INIT(ps_module_init);
EINA_MODULE_SHUTDOWN(ps_module_shutdown);

#endif
