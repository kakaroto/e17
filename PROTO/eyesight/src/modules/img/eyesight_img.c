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
#include <Ecore_File.h>

#include "Eyesight_Module_Img.h"
#include "Eyesight.h"
#include "eyesight_private.h"
#include "eyesight_img.h"


#define DBG(...) EINA_LOG_DOM_DBG(_eyesight_img_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_eyesight_img_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_eyesight_img_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_eyesight_img_log_domain, __VA_ARGS__)
#define CRIT(...) EINA_LOG_DOM_CRIT(_eyesight_img_log_domain, __VA_ARGS__)


static int _eyesight_img_count = 0;
static int _eyesight_img_log_domain = -1;


static Eina_Bool
_eyesight_cb_is_valid(Eyesight_Backend_Img *ebi)
{
  char buf[4096];
  Eyesight_Img_Archive arch;
  int ret;
  Eina_Bool use_rar = EINA_FALSE;

  ret = snprintf(buf, 4096, "unzip -t '%s'", ebi->filename);
  if (ret >= 4096)
    goto test_unrar;
  ret = system(buf);
  if (ret != 0)
    goto test_unrar;
  else
    {
      ebi->archive = EYESIGHT_IMG_ARCHIVE_CBZ;
      return use_rar;
    }

 test_unrar:
  ret = snprintf(buf, 4096, "unrar t '%s'", ebi->filename);
  if (ret >= 4096)
    goto test_rar;
  ret = system(buf);
  if (ret != 0)
    goto test_rar;
  else
    {
      ebi->archive = EYESIGHT_IMG_ARCHIVE_CBR;
      return use_rar;
    }

 test_rar:
  ret = snprintf(buf, 4096, "rar t '%s'", ebi->filename);
  if (ret >= 4096)
    goto test_ace;
  ret = system(buf);
  if (ret != 0)
    goto test_ace;
  else
    {
      use_rar = EINA_TRUE;
      ebi->archive = EYESIGHT_IMG_ARCHIVE_CBR;
      return use_rar;
    }

 test_ace:
  ret = snprintf(buf, 4096, "unace t '%s'", ebi->filename);
  if (ret >= 4096)
    goto test_tar;
  ret = system(buf);
  if (ret != 0)
    goto test_tar;
  else
    {
      ebi->archive = EYESIGHT_IMG_ARCHIVE_CBA;
      return use_rar;
    }

 test_tar:
  ret = snprintf(buf, 4096, "tar -tvf '%s'", ebi->filename);
  if (ret >= 4096)
    goto test_7z;
  ret = system(buf);
  if (ret != 0)
    goto test_7z;
  else
    {
      ebi->archive = EYESIGHT_IMG_ARCHIVE_CBT;
      return use_rar;
    }

 test_7z:
  ret = snprintf(buf, 4096, "7z t '%s'", ebi->filename);
  if (ret >= 4096)
    {
      ebi->archive = EYESIGHT_IMG_ARCHIVE_NONE;
      return use_rar;
    }
  ret = system(buf);
  if (ret != 0)
    {
      ebi->archive = EYESIGHT_IMG_ARCHIVE_NONE;
      return use_rar;
    }
  else
    ebi->archive = EYESIGHT_IMG_ARCHIVE_CBZ;

  return use_rar;
}

static void
_eyesight_cb_tmp_mkdir(char **dir)
{
  char buf[4096];
  char *tmp_dir;
  char *res;
  int ret;

#ifdef HAVE_EVIL
  tmp_dir = (char *)evil_tmpdir_get();
#else
  tmp_dir = "/tmp";
#endif
  ret = snprintf(buf, 4096, "%s/eyesight_img-tmp-XXXXXX", tmp_dir);
  if (ret >= 4096)
    {
      *dir = NULL;
      return;
    }

  tmp_dir = mktemp(buf);
  if (!*tmp_dir)
    {
      *dir = NULL;
      return;
    }

  if (!ecore_file_mkdir(buf))
    {
      *dir = NULL;
      return;
    }

  res = strdup(buf);
  *dir = res;
}

static Eina_Bool
_eyesight_cb_deflate(Eyesight_Backend_Img *ebi, Eina_Bool use_rar)
{
  char buf[4096];
  Eina_List *files;
  char *deflate;
  int ret;

  switch(ebi->archive)
    {
    case EYESIGHT_IMG_ARCHIVE_CBZ:
      deflate = "unzip";
      break;
    case EYESIGHT_IMG_ARCHIVE_CBR:
      if (use_rar)
        deflate = "rar e";
      else
        deflate = "unrar e";
      break;
    case EYESIGHT_IMG_ARCHIVE_CBA:
      deflate = "unace e";
      break;
    case EYESIGHT_IMG_ARCHIVE_CBT:
      deflate = "tar xvf";
      break;
    case EYESIGHT_IMG_ARCHIVE_CB7:
      deflate = "7z e";
      break;
    default:
      return EINA_FALSE;
    }

  ret = snprintf(buf, 4096, "cd %s && %s '%s'", ebi->archive_path, deflate, ebi->filename);
  if (ret >= 4096)
    return EINA_FALSE;

  ret = system(buf);
  if (ret != 0)
    return EINA_FALSE;

  /* check if the files are in a directory */
  files = ecore_file_ls(ebi->archive_path);
  if (eina_list_count(files) == 1)
    {
      char *archive_path;
      size_t l1;
      size_t l2;

      l1 = strlen(ebi->archive_path);
      l2 = strlen(eina_list_data_get(files));
      archive_path = (char *)malloc((l1 + l2 + 2) * sizeof(char));
      if (!archive_path)
        return EINA_FALSE;
      memcpy(archive_path, ebi->archive_path, l1);
      archive_path[l1] = '/';
      memcpy(archive_path + l1 + 1, eina_list_data_get(files), l2 + 1);
      if (ecore_file_is_dir(archive_path))
        {
          free(ebi->archive_path);
          ebi->archive_path = archive_path;
        }
    }

  return EINA_TRUE;
}

static Eina_Bool
_eyesight_cb_page_set(Eyesight_Backend_Img *ebi, int page)
{
  char buf[4096];
  Eyesight_Index_Item *item;
  int ret;

  item = eina_list_nth(ebi->doc.toc, page);
  if (!item)
    return EINA_FALSE;

  ret = snprintf(buf, 4096, "%s/%s", ebi->archive_path, item->title);
  if (ret >= 4096)
    return EINA_FALSE;

  evas_object_image_file_set(ebi->obj, buf, NULL);
  if (evas_object_image_load_error_get(ebi->obj) != EVAS_LOAD_ERROR_NONE)
    {
      ERR("Could not open file %s", buf);
      return EINA_FALSE;
    }

  return EINA_TRUE;
}

static Eina_Bool
em_init (Evas *evas, Evas_Object **obj, void **eyesight_backend)
{
  Eyesight_Backend_Img *ebi;

  if (!eyesight_backend)
    return EINA_FALSE;

  ebi = calloc(1, sizeof(Eyesight_Backend_Img));
  if (!ebi)
    return EINA_FALSE;

  ebi->obj = evas_object_image_add(evas);
  if (!ebi->obj)
    goto free_ebi;


  *obj = ebi->obj;
  *eyesight_backend = ebi;

  return EINA_TRUE;

 free_ebi:
  free(ebi);

  return EINA_FALSE;
}

static void
em_shutdown(void *eb)
{
  Eyesight_Backend_Img *ebi;

  if (!eb)
    return;

  ebi = (Eyesight_Backend_Img *)eb;
  evas_object_del(ebi->obj);
  free(ebi);
}

static void *
em_file_open (void *eb, const char *filename)
{
  Eyesight_Backend_Img *ebi;
  Eina_Bool use_rar;
  Eyesight_Document_Img *doc;

  if (!eb || !filename || !*filename)
    return NULL;

  DBG("Open file %s", filename);

  ebi = (Eyesight_Backend_Img *)eb;
  ebi->filename = ecore_file_realpath(filename);
  if (!ebi->filename)
    return EINA_FALSE;

  doc = (Eyesight_Document_Img *)malloc(sizeof(Eyesight_Document_Img));
  if (!doc)
    goto free_filename;

  evas_object_image_file_set(ebi->obj, ebi->filename, NULL);
  if (evas_object_image_load_error_get(ebi->obj) == EVAS_LOAD_ERROR_NONE)
    goto open_success;

  DBG("Could not open file %s, trying Comic Books", ebi->filename);

  use_rar = _eyesight_cb_is_valid(ebi);
  if (ebi->archive == EYESIGHT_IMG_ARCHIVE_NONE)
    {
      DBG("Could not open Comic Books");
      goto free_doc;
    }

  {
    Eina_List *files;
    Eina_List *l;
    void *data;
    int page;

    _eyesight_cb_tmp_mkdir(&ebi->archive_path);
    if (!ebi->archive_path)
      goto free_doc;

    if (!_eyesight_cb_deflate(ebi, use_rar))
      goto free_doc;

    {
      char *archive_name = "none";
      switch (ebi->archive)
        {
        case EYESIGHT_IMG_ARCHIVE_CBZ:
          archive_name = "ZIP";
          break;
        case EYESIGHT_IMG_ARCHIVE_CBR:
          archive_name = "RAR";
          break;
        case EYESIGHT_IMG_ARCHIVE_CBA:
          archive_name = "ACE";
          break;
        case EYESIGHT_IMG_ARCHIVE_CBT:
          archive_name = "TAR";
          break;
        case EYESIGHT_IMG_ARCHIVE_CB7:
          archive_name = "7Z";
          break;
        }
        DBG("Comic Book archive: %s", archive_name);
    }

    ebi->is_archive = EINA_TRUE;
    files = ecore_file_ls(ebi->archive_path);
    page = 0;
    EINA_LIST_FOREACH(files, l, data)
      {
        Eyesight_Index_Item *item;

        item = eyesight_index_item_new();
        if (!item)
          {
            free(data);
            continue;
          }
        item->title = data;
        item->page = page++;
        item->action = EYESIGHT_LINK_ACTION_GOTO;
        item->children = NULL;
        ebi->doc.toc = eina_list_append(ebi->doc.toc, item);
      }
    eina_list_free(files);
    if (!_eyesight_cb_page_set(ebi, 0))
      goto free_archive_path;
  }

 open_success:
  doc->filename = ebi->filename;
  doc-> archive = ebi->archive;
  ebi->document = doc;
  ebi->page.hscale = 1.0;
  ebi->page.vscale = 1.0;
  ebi->page.orientation = EYESIGHT_ORIENTATION_PORTRAIT;

  return doc;

 free_archive_path:
  free(ebi->archive_path);
  ebi->archive_path = NULL;
 free_doc:
  free(doc);
  ebi->document = NULL;
 free_filename:
  free(ebi->filename);
  ebi->filename = NULL;

  return NULL;
}

static void
em_file_close (void *eb)
{
  Eyesight_Backend_Img *ebi;
  void *data;

  if (!eb)
    return;

  ebi = (Eyesight_Backend_Img *)eb;

  DBG("Close file %s", ebi->filename);

  EINA_LIST_FREE(ebi->doc.toc, data)
    {
      Eyesight_Index_Item *item;

      item = (Eyesight_Index_Item *)data;

      free(item->title);
      free(item);
    }

  if (ebi->archive_path)
    {
      ecore_file_recursive_rm(ebi->archive_path);
      free(ebi->archive_path);
      ebi->archive_path = NULL;
    }
  if (ebi->document)
    {
      free(ebi->document);
      ebi->document = NULL;
    }
  if (ebi->filename)
    {
      free(ebi->filename);
      ebi->filename = NULL;
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
  Eyesight_Backend_Img *ebi;

  if (!eb)
    return NULL;

  ebi = (Eyesight_Backend_Img *)eb;

  return ebi->doc.toc;
}

static int
em_page_count(void *eb)
{
  Eyesight_Backend_Img *ebi;

  if (!eb)
    return 0;

  ebi = (Eyesight_Backend_Img *)eb;

  if (ebi->is_archive)
    return eina_list_count(ebi->doc.toc);

  return 1;
}

static void
em_page_set(void *eb, int page)
{
  Eyesight_Backend_Img *ebi;

  if (!eb)
    return;

  ebi = (Eyesight_Backend_Img *)eb;

  if (!ebi->is_archive)
    return;

  if (!ebi->doc.toc)
    return;

  if (page < 0)
    {
       ERR("Page number is negative");
       return;
    }

  if (page >= (int)eina_list_count(ebi->doc.toc))
    {
      ERR("Page number is beyond the maximal number of pages");
      return;
    }

  if (ebi->page.page == page)
    return;

  ebi->page.page = page;

  DBG("page=%d", page);

  if (!_eyesight_cb_page_set(ebi, page))
    ERR("Could not set page %d", page);
}

static int
em_page_get(void *eb)
{
  Eyesight_Backend_Img *ebi;

  if (!eb)
    return 0;

  ebi = (Eyesight_Backend_Img *)eb;

  return ebi->page.page;
}

static void
em_page_scale_set(void *eb, double hscale, double vscale)
{
  Eyesight_Backend_Img *ebi;

  if (!eb)
    return;

  DBG("horizontal scale=%f vertical scale=%f", hscale, vscale);

  ebi = (Eyesight_Backend_Img *)eb;

  ebi->page.hscale = hscale;
  ebi->page.vscale = vscale;
}

static void
em_page_scale_get(void *eb, double *hscale, double *vscale)
{
  Eyesight_Backend_Img *ebi;

  if (!eb)
    {
      if (hscale) *hscale = 1.0;
      if (vscale) *vscale = 1.0;
    }

  ebi = (Eyesight_Backend_Img *)eb;

  if (hscale) *hscale = ebi->page.hscale;
  if (vscale) *vscale = ebi->page.vscale;
}

static void
em_page_orientation_set(void *eb, Eyesight_Orientation orientation)
{
  Eyesight_Backend_Img *ebi;

  if (!eb)
    return;

  DBG("orientation=%d", orientation);

  ebi = (Eyesight_Backend_Img *)eb;

  ebi->page.orientation = orientation;
}

static Eyesight_Orientation
em_page_orientation_get(void *eb)
{
  Eyesight_Backend_Img *ebi;

  if (!eb)
    return EYESIGHT_ORIENTATION_PORTRAIT;

  ebi = (Eyesight_Backend_Img *)eb;

  return ebi->page.orientation;
}

void
em_page_size_get(void *eb, int *width, int *height)
{
  Eyesight_Backend_Img *ebi;

  if (!eb)
    return;

  ebi = (Eyesight_Backend_Img *)eb;
  evas_object_image_size_get(ebi->obj, width, height);
}

static void
em_page_render(void *eb)
{
  Eyesight_Backend_Img *ebi;
  int width;
  int height;

  if (!eb)
    return;

  ebi = (Eyesight_Backend_Img *)eb;

  evas_object_image_size_get(ebi->obj, &width, &height);
  evas_object_image_fill_set(ebi->obj, 0, 0, width, height);
  evas_object_image_size_set(ebi->obj, width, height);
  evas_object_resize(ebi->obj, (int)(width * ebi->page.hscale), (int)(height * ebi->page.vscale));
}

char *
em_page_text_get(void *eb __UNUSED__, Eina_Rectangle rect __UNUSED__)
{
  return NULL;
}

Eina_List *
em_page_text_find(void *eb __UNUSED__, const char *text __UNUSED__, Eina_Bool is_case_sensitive __UNUSED__, Eina_Bool backward __UNUSED__)
{
  return NULL;
}

Eina_List *
em_page_links_get(void *eb __UNUSED__)
{
  return NULL;
}

static Eyesight_Module _eyesight_module_img =
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

   if (_eyesight_img_count == 0)
     {
       if (_eyesight_img_log_domain < 0)
         {
           _eyesight_img_log_domain = eina_log_domain_register("eyesight-img", EINA_COLOR_LIGHTCYAN);
           if (_eyesight_img_log_domain < 0)
             {
               EINA_LOG_CRIT("Could not register log domain 'eyesight-img'");
               return EINA_FALSE;
             }
         }
     }

   _eyesight_img_count++;

   if (!_eyesight_module_img.init(evas, obj, backend))
     {
        ERR("Could not initialize module");
        eina_log_domain_unregister(_eyesight_img_log_domain);
        _eyesight_img_log_domain = -1;
        _eyesight_img_count--;
       return EINA_FALSE;
     }

   *module = &_eyesight_module_img;
   return EINA_TRUE;
}

static void
module_close(Eyesight_Module *module, void *backend)
{
  _eyesight_img_count--;
  if (_eyesight_img_count == 0)
    {
      eina_log_domain_unregister(_eyesight_img_log_domain);
      _eyesight_img_log_domain = -1;
    }
   _eyesight_module_img.shutdown(backend);
}

Eina_Bool
img_module_init(void)
{
   return _eyesight_module_register("img", module_open, module_close);
}

void
img_module_shutdown(void)
{
   _eyesight_module_unregister("img");
}

#ifndef EYESIGHT_STATIC_BUILD_IMG

EINA_MODULE_INIT(img_module_init);
EINA_MODULE_SHUTDOWN(img_module_shutdown);

#endif
