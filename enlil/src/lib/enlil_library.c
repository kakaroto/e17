#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Eina.h>
#include <Eio.h>
#include <Ecore.h>

#include "Enlil.h"
#include "enlil_private.h"

static void
_enlil_event_library_loaded_free(void *user_data, void *func_data __UNUSED__)
{
   Enlil_Library_Loaded *ev = user_data;

   EINA_REFCOUNT_UNREF(ev->library, _enlil_library_free);

   free(ev);
}

static void
_enlil_threaded_load(void *data, Ecore_Thread *thread)
{
   Enlil_Library *library = data;

   if (!_enlil_library_load(library))
     ecore_thread_cancel(thread);
}

static void
_enlil_album_mark_photo(void *data, Ecore_Thread *thread)
{
   Enlil_Album *album = data;
   Enlil_Library *library = album->library;
   const Eina_List *l;
   const char *path;

   EINA_LIST_FOREACH(album->photos, l, path)
     {
        Enlil_Photo *photo;

        photo = eina_hash_find(library->loaded->data.photos, path);
        if (photo)
          photo->found = EINA_TRUE;
     }
}

static void
_enlil_album_mark_done(void *data, Ecore_Thread *thread)
{
   Enlil_Album *album = data;
   Enlil_Library *lib = album->library;

   lib->process = eina_list_remove(lib->process, thread);

   /* EINA_REFCOUNT_UNREF(album, ); */
   EINA_REFCOUNT_UNREF(lib, _enlil_library_free);
}

static Eina_Bool
_enlil_album_filter(void *data, Eio_File *handler, const Eina_File_Direct_Info *info)
{
   /* FIXME: take a lock on the album, retrieve the photo, refcount it,
      unlock, check it, mark it, if !changed, retake the lock and unref it,
      if changed notify it.
    */

   return EINA_FALSE;
}

static void
_enlil_album_main(void *data, Eio_File *handler, const Eina_File_Direct_Info *info)
{
   /* FIXME: update info */
}

static void
_enlil_album_done(void *data, Eio_File *handler)
{
   /* FIXME: unref album, library */
}

static void
_enlil_album_error(void *data, Eio_File *handler, int error)
{
   /* FIXME: unref and destroy the album */
}

static void
_enlil_album_stat(void *data, Eio_File *handler, const struct stat *st)
{
   Enlil_Album *album = data;
   Enlil_Library *library = album->library;

   library->io_process = eina_list_remove(library->io_process, handler);

   if (st->st_size != album->filesystem.size
       || st->st_ino != album->filesystem.inode
       || st->st_mtime != album->filesystem.last_modified)
     {
        Eio_File *ls;

        album->filesystem.size = st->st_size;
        album->filesystem.inode = st->st_ino;
        album->filesystem.last_modified = st->st_mtime;

        ls = eio_file_direct_ls(album->path,
                                _enlil_album_filter,
                                _enlil_album_main,
                                _enlil_album_done,
                                _enlil_album_error,
                                album);

        library->io_process = eina_list_append(library->io_process, ls);
     }
   else
     {
        Ecore_Thread *et;

        et = ecore_thread_run(_enlil_album_mark_photo,
                              _enlil_album_mark_done,
                              _enlil_album_mark_done,
                              album);
        library->process = eina_list_append(library->process, et);
     }
}

static void
_enlil_album_stat_failed(void *data, Eio_File *handler, int error)
{
   Enlil_Album *album = data;
   Enlil_Library *library = album->library;

   library->io_process = eina_list_remove(library->io_process, handler);
   library->loaded->paths.albums = eina_list_remove(library->loaded->paths.albums, album->path);
   eina_hash_del(library->loaded->data.albums, album->path, album);

   EINA_REFCOUNT_UNREF(library, _enlil_library_free);
}

static void
_enlil_threaded_load_end(void *data, Ecore_Thread *thread)
{
   Enlil_Library *library = data;
   Enlil_Library_Loaded *ev;
   Eina_Iterator *it;
   Enlil_Album *album;

   ev = calloc(1, sizeof (Enlil_Library_Loaded));
   ev->library = library;

   ecore_event_add(ENLIL_LIBRARY_LOADED, ev,
                   _enlil_event_library_loaded_free, NULL);

   library->process = eina_list_remove(library->process, thread);

   /* FIXME: start scanning sub directory if succeeded */
   it = eina_hash_iterator_data_new(library->loaded->data.albums);
   EINA_ITERATOR_FOREACH(it, album)
     {
        Eio_File *io;

        EINA_REFCOUNT_INIT(album);
        EINA_REFCOUNT_REF(album);
        EINA_REFCOUNT_REF(library);

        album->library = library;

        io = eio_file_direct_stat(album->path,
                                  _enlil_album_stat,
                                  _enlil_album_stat_failed,
                                  album);

        library->io_process = eina_list_append(library->io_process, io);
     }
   eina_iterator_free(it);
}

EAPI Eina_Bool
enlil_library_load(Enlil_Library *library)
{
   Ecore_Thread *et;

   if (library->process || library->io_process) return EINA_TRUE;
   if (library->loaded) return EINA_TRUE; /* Prevent rescan */

   EINA_REFCOUNT_REF(library);

   et = ecore_thread_run(_enlil_threaded_load,
                         _enlil_threaded_load_end,
                         _enlil_threaded_load_end,
                         library);

   library->process = eina_list_append(library->process, et);

   return EINA_TRUE;
}
