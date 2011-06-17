#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>
#include <libgen.h>

#include <Eina.h>
#include <Ecore.h>

#include "Enlil.h"
#include "enlil_private.h"

static void
_enlil_free_cb(Enlil_Free *f, void *data)
{
   if (f->cb)
     f->cb(data, f->data);
}

static void
_enlil_library_ss_free(void *data, void *user_data __UNUSED__)
{
   Enlil_Library *lib = data;

   eina_stringshare_del(lib->name);
   eina_stringshare_del(lib->path);
}

static void
_enlil_library_loaded_paths_free(void *data, void *user_data __UNUSED__)
{
   Enlil_Loaded_Library *lib = data;
   Eet_Dictionary *ed;
   const char *tmp;

   ed = eet_dictionary_get(lib->source);

#define _ENLIL_LIST_EET_STRING(Ed, List, Tmp)   \
   EINA_LIST_FREE(List, Tmp)                    \
     if (!eet_dictionary_string_check(Ed, Tmp)) \
       eina_stringshare_del(Tmp);

   _ENLIL_LIST_EET_STRING(ed, lib->paths.albums, tmp);
   _ENLIL_LIST_EET_STRING(ed, lib->paths.tags, tmp);
   _ENLIL_LIST_EET_STRING(ed, lib->paths.collections, tmp);
}

static void
_enlil_album_free(void *data)
{
   Enlil_Album *album = data;

   _enlil_free_cb(&album->free, album);
   album->photos = eina_list_free(album->photos);
   album->collections = eina_list_free(album->collections);
   free(album);
}

static void
_enlil_collection_free(void *data)
{
   Enlil_Collection *collection = data;

   _enlil_free_cb(&collection->free, collection);
   collection->album = eina_list_free(collection->album);
   free(collection);
}

static void
_enlil_tag_free(void *data)
{
   Enlil_Tag *tag = data;

   _enlil_free_cb(&tag->free, tag);
   tag->photos = eina_list_free(tag->photos);
   free(tag);
}

static void
_enlil_photo_free(void *data)
{
   Enlil_Photo *photo = data;

   _enlil_free_cb(&photo->free, photo);
   eina_hash_free(photo->exif.data);
   eina_hash_free(photo->iptc.data);
   photo->tags = eina_list_free(photo->tags);
   free(photo);
}

Enlil_Libraries *
_enlil_libraries_new(void)
{
   Enlil_Libraries *result;

   result = calloc(1, sizeof (Enlil_Libraries));
   result->version.major = ENLIL_FILE_VERSION_MAJOR;
   result->version.minor = ENLIL_FILE_VERSION_MINOR;

   return result;
}

void
_enlil_libraries_free(Enlil_Libraries *libs)
{
   Enlil_Library *lib;

   EINA_LIST_FREE(libs->libraries, lib)
     _enlil_library_free(lib);
   free(libs);
}

Enlil_Library *
_enlil_library_new(const char *path, const char *name)
{
   Enlil_Library *lib;

   if (!path || strlen(path) < 1 || !name || strlen(name) < 1) return NULL;

   lib = calloc(1, sizeof (Enlil_Library));
   if (!lib) return NULL;

   lib->name = eina_stringshare_add(name);
   lib->path = eina_stringshare_add(path);
   lib->free.cb = _enlil_library_ss_free;

   EINA_REFCOUNT_INIT(lib);

   return lib;
}

Eina_Bool
_enlil_library_load(Enlil_Library *lib)
{
   Eet_File *ef = NULL;
   char *filename;
   char *tmp;
   int length;

   tmp = alloca(eina_stringshare_strlen(lib->path) + 1);
   memcpy(tmp, lib->path, eina_stringshare_strlen(lib->path) + 1);
   tmp = dirname(tmp);
   length = strlen(tmp);

   if (!lib->read_only)
     {
        filename = alloca(length + 11);
        snprintf(filename, length + 11, "%s/enlil.eet", tmp);

        ef = eet_open(filename, EET_FILE_MODE_READ_WRITE);
     }

   if (!ef)
     {
        int fl;

        lib->read_only = EINA_TRUE;

        fl = eina_stringshare_strlen(lib->name) +
          eina_stringshare_strlen(_enlil_library_config) +
          12;
        filename = alloca(fl);
        snprintf(filename, fl, "%s/enlil/%s.eet",
                 _enlil_library_config, lib->name);

        ef = eet_open(filename, EET_FILE_MODE_READ_WRITE);
     }

   if (!ef) return EINA_FALSE;

   lib->loaded = eet_data_read(ef, enlil_edd_loaded_library, "enlil/library");
   if (!lib->loaded)
     {
        lib->loaded = calloc(1, sizeof (Enlil_Loaded_Library));
        if (!lib->loaded)
          {
             eet_close(ef);
             return EINA_FALSE;
          }

        lib->loaded->version.major = ENLIL_FILE_VERSION_MAJOR;
        lib->loaded->version.minor = ENLIL_FILE_VERSION_MINOR;

        lib->loaded->free.cb = _enlil_library_loaded_paths_free;
     }

   if (!lib->loaded->data.albums)
     lib->loaded->data.albums = eina_hash_string_superfast_new(NULL);
   eina_hash_free_cb_set(lib->loaded->data.albums, _enlil_album_free);

   if (!lib->loaded->data.collections)
     lib->loaded->data.collections = eina_hash_string_superfast_new(NULL);
   eina_hash_free_cb_set(lib->loaded->data.collections, _enlil_collection_free);

   if (!lib->loaded->data.tags)
     lib->loaded->data.tags = eina_hash_string_superfast_new(NULL);
   eina_hash_free_cb_set(lib->loaded->data.tags, _enlil_tag_free);

   if (!lib->loaded->data.photos)
     lib->loaded->data.photos = eina_hash_string_superfast_new(NULL);
   eina_hash_free_cb_set(lib->loaded->data.photos, _enlil_photo_free);

   lib->loaded->source = ef;
   return EINA_TRUE;
}

void
_enlil_library_loaded_free(Enlil_Loaded_Library *lib)
{
   assert(lib->save_needed);

   _enlil_free_cb(&lib->free, lib);

   lib->paths.albums = eina_list_free(lib->paths.albums);
   lib->paths.tags = eina_list_free(lib->paths.tags);
   lib->paths.collections = eina_list_free(lib->paths.collections);

   eina_hash_free(lib->data.albums);
   eina_hash_free(lib->data.collections);
   eina_hash_free(lib->data.tags);
   eina_hash_free(lib->data.photos);

   eet_close(lib->source);
   free(lib);
}

void
_enlil_library_close(Enlil_Library *lib)
{
   if (lib->loaded)
     {
        _enlil_library_save(lib);
        _enlil_library_loaded_free(lib->loaded);
        lib->loaded = NULL;
     }
}

void
_enlil_library_free(Enlil_Library *lib)
{
   /* FIXME: use refcount */
   _enlil_library_free(lib);
   _enlil_free_cb(&lib->free, lib);
   free(lib);
}

void
_enlil_library_save(Enlil_Library *lib)
{
   if (lib->loaded && lib->loaded->save_needed)
     {
        char *buffer;
        int length;

        length = lib->name ? strlen(lib->name) : 0;

        buffer = alloca(sizeof (char) * 20 + length);
        snprintf(buffer, 20 + length, "enlil/library/%s",
                 lib->name ? lib->name : "default");
        eet_data_write(lib->loaded->source,
                       enlil_edd_loaded_library, buffer,
                       lib->loaded, 1);
        /* Maybe setup a timer to do the sync only when really needed */
        eet_sync(lib->loaded->source);
        lib->loaded->save_needed = EINA_FALSE;
     }
}

