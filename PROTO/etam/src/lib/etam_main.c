#include <Eina.h>
#include <Eet.h>
#include <Ecore.h>

#include "Etam.h"

#include "etam_private.h"

int _etam_default_log_dom = -1;
static int _etam_init_count = 0;

EAPI int
etam_init(void)
{
   if (_etam_init_count++ != 0)
     return _etam_init_count;

   eina_init();
   eet_init();
   ecore_init();

   _etam_default_log_dom = eina_log_domain_register("etam", ETAM_DEFAULT_LOG_COLOR);

   etam_eet_data_descriptor_init();
   etam_db_local_init();

   return _etam_init_count;
}

EAPI int
etam_shutdown(void)
{
   if (_etam_init_count <= 0)
     {
        EINA_LOG_ERR("Init count not greater than 0 in shutdown of etam.");
        return 0;
     }

   if (--_etam_init_count != 0)
     return _etam_init_count;

   etam_db_local_shutdown();
   etam_eet_data_descriptor_shutdown();

   ecore_shutdown();
   eet_shutdown();
   eina_shutdown();

   return _etam_init_count;
}

EAPI Etam_DB *
etam_db_open(const char *uri)
{
   if (!uri) return NULL;
}

EAPI void
etam_db_flush(Etam_DB *db)
{
   if (db->local) etam_db_local_flush(db->local);
}

EAPI void
etam_db_close(Etam_DB *db)
{
   if (!db) return ;

   if (db->local) etam_db_local_close(db->local);
   /* FIXME: setup a callback on local db destruction, when done
      do the real free. */
   free(db);
}


