/*
 * sync_cb.c
 *
 *  Created on: Jun 19, 2011
 *      Author: watchwolf
 *
 *  This code should be done by enlil but currently this is not the case.
 *  In a the future enlil will do the job
 */



static void
_load_album_done_cb(void          *data __UNUSED__,
                    Enlil_Load    *load __UNUSED__,
                    Enlil_Library *library __UNUSED__,
                    Enlil_Album   *album)
{}



static void
_sync_start_cb(void       *data __UNUSED__,
               Enlil_Sync *_sync __UNUSED__)
{
   Enlil_Library *library = data;
}


static void
_sync_album_new_cb(void          *data,
                   Enlil_Sync    *_sync __UNUSED__,
                   Enlil_Library *library __UNUSED__,
                   Enlil_Album   *album)
{
   Enlil_Library *_library = (Enlil_Library *)data;
   Enlil_Album *_album = enlil_album_copy_new(album);
   enlil_library_album_add(_library, _album);
   enlil_album_monitor_start(_album);
}

static void
_sync_album_update_cb(void          *data,
                      Enlil_Sync    *_sync __UNUSED__,
                      Enlil_Library *library __UNUSED__,
                      Enlil_Album   *album)
{
   Enlil_Library *_library = (Enlil_Library *)data;

   Enlil_Album *_album = enlil_library_album_search_file_name(_library, enlil_album_file_name_get(album));

   enlil_album_copy(album, _album);
}

static void
_sync_album_disappear_cb(void          *data,
                         Enlil_Sync    *_sync __UNUSED__,
                         Enlil_Library *library __UNUSED__,
                         Enlil_Album   *album)
{
   Enlil_Library *_library = (Enlil_Library *)data;

   Enlil_Album *_album = enlil_library_album_search_file_name(_library, enlil_album_file_name_get(album));

   enlil_library_album_remove(_library, _album);
   enlil_album_free(&_album);
}

static void
_sync_photo_new_cb(void        *data,
                   Enlil_Sync  *_sync __UNUSED__,
                   Enlil_Album *album,
                   Enlil_Photo *photo)
{
   Enlil_Library *_library = (Enlil_Library *)data;

   Enlil_Album *_album = enlil_library_album_search_file_name(_library, enlil_album_file_name_get(album));


   Enlil_Photo *_photo = enlil_photo_copy_new(photo);
   enlil_album_photo_add(_album, _photo);
}

static void
_sync_photo_update_cb(void        *data,
                      Enlil_Sync  *_sync __UNUSED__,
                      Enlil_Album *album,
                      Enlil_Photo *photo)
{
   Enlil_Library *_library = (Enlil_Library *)data;

   Enlil_Album *_album = enlil_library_album_search_file_name(_library, enlil_album_file_name_get(album));

   Enlil_Photo *_photo = enlil_album_photo_search_file_name(_album, enlil_photo_file_name_get(photo));
   enlil_photo_copy(photo, _photo);
}

static void
_sync_photo_disappear_cb(void        *data,
                         Enlil_Sync  *_sync __UNUSED__,
                         Enlil_Album *album,
                         Enlil_Photo *photo)
{
   Enlil_Library *_library = (Enlil_Library *)data;

   Enlil_Album *_album = enlil_library_album_search_file_name(_library, enlil_album_file_name_get(album));


   Enlil_Photo *_photo = enlil_album_photo_search_file_name(_album, enlil_photo_file_name_get(photo));
   enlil_album_photo_remove(_album, _photo);
   enlil_photo_free(&_photo);
}



