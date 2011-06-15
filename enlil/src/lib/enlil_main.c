#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Eina.h>
#include <Eet.h>
#include <Eio.h>
#include <Ecore.h>
#include <Efreet.h>
#include <Ecore_File.h>

#include "Enlil.h"
#include "enlil_private.h"

const char *_enlil_library_config = NULL;

static int _enlil_init_count = 0;
static Eet_File *_enlil_config = NULL;
static Enlil_Libraries *_enlil_libraries = NULL;

EAPI int ENLIL_ALBUM_ADD;
EAPI int ENLIL_ALBUM_DEL;
EAPI int ENLIL_ALBUMS_LOADED;
EAPI int ENLIL_COLLECTION_ADD;
EAPI int ENLIL_COLLECTION_DEL;
EAPI int ENLIL_TAG_ADD;
EAPI int ENLIL_TAG_DEL;
EAPI int ENLIL_TAG_SET;
EAPI int ENLIL_TAG_UNSET;
EAPI int ENLIL_PHOTO_ADD;
EAPI int ENLIL_PHOTO_DEL;
EAPI int ENLIL_LIBRARY_LOADED;

EAPI Eina_Bool
enlil_init(void)
{
   static const char *_enlil_paths[] = {
     "enlil",
     NULL
   };
   char buffer[PATH_MAX];

   if (_enlil_init_count++ != 0) return EINA_TRUE;

   eina_init();
   eet_init();
   ecore_init();
   efreet_init();
   eio_init();

   ENLIL_ALBUM_ADD= ecore_event_type_new();
   ENLIL_ALBUM_DEL= ecore_event_type_new();
   ENLIL_ALBUMS_LOADED= ecore_event_type_new();
   ENLIL_COLLECTION_ADD= ecore_event_type_new();
   ENLIL_COLLECTION_DEL= ecore_event_type_new();
   ENLIL_TAG_ADD= ecore_event_type_new();
   ENLIL_TAG_DEL= ecore_event_type_new();
   ENLIL_TAG_SET= ecore_event_type_new();
   ENLIL_TAG_UNSET= ecore_event_type_new();
   ENLIL_PHOTO_ADD= ecore_event_type_new();
   ENLIL_PHOTO_DEL= ecore_event_type_new();
   ENLIL_LIBRARY_LOADED = ecore_event_type_new();

   _enlil_edd_init();

   _enlil_library_config = eina_stringshare_add(efreet_config_home_get());

   ecore_file_mksubdirs(_enlil_library_config,
                        _enlil_paths);

   snprintf(buffer, PATH_MAX,
            "%s/enlil/config", _enlil_library_config);
   _enlil_config = eet_open(buffer, EET_FILE_MODE_READ_WRITE);

   _enlil_libraries = eet_data_read(_enlil_config, enlil_edd_libraries, "enlil/librairies");
   if (!_enlil_libraries)
     _enlil_libraries = _enlil_libraries_new();

   return EINA_TRUE;
}

EAPI Eina_Bool
enlil_shutdown(void)
{
   if (--_enlil_init_count != 0) return EINA_TRUE;

   _enlil_libraries_free(_enlil_libraries);
   _enlil_libraries = NULL;

   eet_close(_enlil_config);

   _enlil_edd_shutdown();

   eio_init();
   efreet_shutdown();
   ecore_shutdown();
   eet_shutdown();
   eina_shutdown();

   return EINA_TRUE;
}

