#include <Eina.h>
#include <Ecore.h>

#include "Etam.h"

#include "etam_private.h"

static Eina_Hash *_open_db = NULL;
static Eina_List *_dying = NULL;

void
etam_db_local_free(Etam_DB_Local *db)
{
   /* FIXME: cleanup callbacks */
   eina_hash_free(db->collections);
   eina_stringshare_del(db->filename);
   eina_rwlock_free(&db->lock);
   free(db);
}

void
etam_db_local_flush(Etam_DB_Local *db)
{
   /* FIXME: refcount and start a thread that will lock and convert all collections */
}

void
etam_db_local_close(Etam_DB_Local *db)
{
   /* Database are not supposed to be accessed by two process at the same time.
      So it is assumed that if we unref it enough, it should be freeed. And their
      is no point on opening a different one.
    */
   EINA_REFCOUNT_UNREF(db)
     etam_db_local_free(db);
}

Etam_DB_Local *
etam_db_local_open(const char *filename,
		   Etam_DB_Validate_Cb cb,
		   const void *data)
{
   Etam_DB_Local *r;
   char *realfile;

   realfile = eina_file_path_sanitize(filename);

   r = eina_hash_find(_open_db, realfile);
   if (r)
     {
        EINA_REFCOUNT_REF(r);

        return r;
     }

   r = calloc(1, sizeof (Etam_DB_Local));
   if (!r) return NULL;

   r->filename = eina_stringshare_add(realfile);
   free(realfile);

   /* FIXME: handle backup file ! Do it asynchronously ! */
   r->ef = eet_open(r->filename, EET_FILE_MODE_READ_WRITE);
   if (!r->ef) goto cleanup;

   eina_rwlock_new(&r->lock);

   r->collections = eina_hash_string_superfast_new(NULL);

   EINA_REFCOUNT_INIT(r);

   eina_hash_direct_add(_open_db, r->filename, r);

   return r;

 cleanup:
   eina_stringshare_del(r->filename);
   free(r);
   return NULL;
}

void
etam_db_local_init(void)
{
   eina_init();
   eet_init();
   ecore_init();

   _open_db = eina_hash_string_small_new(NULL);
}

void
etam_db_local_shutdown(void)
{
   /* FIXME: shutdown all opened database and wait for all request to finish */

   eina_hash_free(_open_db);
   _open_db = NULL;

   ecore_shutdown();
   eet_shutdown();
   eina_shutdown();
}

