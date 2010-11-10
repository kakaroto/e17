#include <lightmediascanner.h>
#include "private.h"

/* FIXME: This shouldn't be hardcoded, but get it from config. */
static const char *_parsers[] = {
     "id3",
     "ogg",
     "asf",
     "flac",
     NULL
};

static const char *_charsets[] = {
     "UTF-8",
     "ISO8859-1",
     NULL
};

struct _Libmgr
{
   const char *dbpath;
   lms_t *lms;
   Eina_List *scanpaths;
   Ecore_Thread *thread;
   void (*scan_end_cb)(void *, Eina_Bool);
   void *scan_end_cb_data;
};

struct _Libmgr _mgr;

static Eina_Bool
_libmgr_lms_parsers_add(Libmgr *mgr)
{
   int i;
   Eina_Bool found_parser = 0;
   for (i = 0; _parsers[i]; i++)
     {
        if (lms_parser_find_and_add(mgr->lms, _parsers[i]))
          found_parser = 1;
     }

   return found_parser;
}

static void
_libmgr_lms_charsets_add(Libmgr *mgr)
{
   int i;
   for (i = 0; _charsets[i]; i++)
     lms_charset_add(mgr->lms, _charsets[i]);
}

static void
_libmgr_scan(Ecore_Thread *thread __UNUSED__, void *data)
{
   Libmgr *mgr = data;
   const char *scanpath;
   Eina_List *l;

   EINA_LIST_FOREACH(mgr->scanpaths, l, scanpath)
     {
        if (lms_check(mgr->lms, scanpath) != 0)
          ERR("couldn't check \"%s\".", scanpath);

        if (lms_process(mgr->lms, scanpath) != 0)
          ERR("couldn't process \"%s\".", scanpath);
     }
}

Libmgr *
libmgr_new(const char *dbpath)
{
   memset(&_mgr, 0, sizeof(_mgr));
   _mgr.dbpath = eina_stringshare_ref(dbpath);

   return &_mgr;
}

Eina_Bool
libmgr_scanpath_add(Libmgr *mgr, const char *path)
{
   if (mgr->thread)
     {
        WRN("Scan is already running, can't add a new path until finish.");
        return EINA_FALSE;
     }
   mgr->scanpaths = eina_list_append(mgr->scanpaths,
                                     eina_stringshare_add(path));

   return EINA_TRUE;
}

static void
_libmgr_scan_finish(Libmgr *mgr, Eina_Bool success __UNUSED__)
{
   const char *scanpath;
   mgr->scan_end_cb(mgr->scan_end_cb_data, EINA_TRUE);
   mgr->scan_end_cb_data = NULL;
   mgr->scan_end_cb = NULL;
   EINA_LIST_FREE(mgr->scanpaths, scanpath)
      eina_stringshare_del(scanpath);

   if (lms_free(mgr->lms) != 0)
     ERR("could not close light media scanner.");
   mgr->lms = NULL;
}

static void
_libmgr_scan_end(void *data)
{
   Libmgr *mgr = data;
   _libmgr_scan_finish(mgr, EINA_TRUE);
}

static void
_libmgr_scan_cancel(void *data)
{
   Libmgr *mgr = data;
   _libmgr_scan_finish(mgr, EINA_FALSE);
}

Eina_Bool
libmgr_scan_start(Libmgr *mgr, void (*func_end)(void *, Eina_Bool), void *data)
{
   mgr->scan_end_cb = func_end;
   mgr->scan_end_cb_data = data;

   if (!mgr->dbpath)
     {
        ERR("no database set.");
        goto error;
     }

   if (!mgr->scanpaths)
     {
        INF("no path to scan.");
        goto error;
     }

   mgr->lms = lms_new(mgr->dbpath);
   if (!mgr->lms)
     {
        ERR("could not create light media scanner for DB \"%s\".",
            mgr->dbpath);
        goto error;
     }

   if (!_libmgr_lms_parsers_add(mgr))
     {
        ERR("could not find any parser.");
        goto free_lms;
     }

   _libmgr_lms_charsets_add(mgr);

   mgr->thread = ecore_thread_run(_libmgr_scan, _libmgr_scan_end,
                                  _libmgr_scan_cancel, mgr);
   if (!mgr->thread)
     goto free_lms;

   return EINA_TRUE;

free_lms:
   if (lms_free(mgr->lms) != 0)
     ERR("could not close light media scanner.");
   mgr->lms = NULL;
error:
   return EINA_FALSE;
}
