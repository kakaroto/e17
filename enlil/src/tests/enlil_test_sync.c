#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>

#include "enlil_suite.h"
#include "Enlil.h"

#include "sync_cb.c"

static char *path_library = "/tmp/enlil_tests_library/";
#define PATH_LIBRARY_DATA "/home/watchwolf/Projects/e/trunk/enlil/data/tests/library/"

typedef struct
{
   const char *name;
   const char *file_name;
} Test_Photo;

typedef struct
{
   const char *name;
   Eina_List *photos; //list of Test_Photo
} Test_Album;

typedef struct
{
   Eina_List *albums; //list of Test_Album
} Test_Library;

Test_Library *test_library;


static void
shutdown_library()
{
   ecore_file_recursive_rm(path_library);
}

static Test_Album *_test_album_new(const char *album_name)
{
   Test_Album *album = calloc(1, sizeof(Test_Album));
   album->name = album_name;
   return album;
}

static Test_Photo *_test_photo_new(const char *file_name, const char *photo_name)
{
   Test_Photo *photo = calloc(1, sizeof(Test_Photo));
   photo->name = photo_name;
   photo->file_name = file_name;
   return photo;
}

#define ADD_PHOTO(ALBUM, FILE_NAME, PHOTO_NAME) \
         photo = _test_photo_new(FILE_NAME, PHOTO_NAME); \
         ALBUM->photos = eina_list_append(ALBUM->photos, photo)

static void
init_library()
{
   Test_Album *album;
   Test_Photo *photo;

   Eina_List *dirs, *files;
   char *dir, *file;
   char buf[BUFSIZ], buf2[BUFSIZ], buf3[BUFSIZ], buf4[BUFSIZ];

   //
   test_library = calloc(1, sizeof(Test_Library));

   album = _test_album_new("album_1");
   test_library->albums = eina_list_append(test_library->albums, album);
   ADD_PHOTO(album, "photo_1.jpg", "photo_1");
   ADD_PHOTO(album, "photo_2.jpg", "photo_2");
   ADD_PHOTO(album, "photo_3.jpg", "photo_3");

   album = _test_album_new("album_2");
   test_library->albums = eina_list_append(test_library->albums, album);
   ADD_PHOTO(album, "photo_1.jpg", "photo_1");
   ADD_PHOTO(album, "photo_2.jpg", "photo_2");
   ADD_PHOTO(album, "photo_3.jpg", "photo_3");
   ADD_PHOTO(album, "photo_4.jpg", "photo_4");

   album = _test_album_new("album_3");
   test_library->albums = eina_list_append(test_library->albums, album);
   ADD_PHOTO(album, "photo_1.jpg", "photo_1");
   ADD_PHOTO(album, "photo_2.jpg", "photo_2");
   ADD_PHOTO(album, "photo_3.jpg", "photo_3");
   ADD_PHOTO(album, "photo_4.jpg", "photo_4");


   album = _test_album_new("album_4");
   test_library->albums = eina_list_append(test_library->albums, album);
   ADD_PHOTO(album, "photo_1.jpg", "photo_1");
   //


   shutdown_library();

   fail_if(ecore_file_mkpath(path_library) == EINA_FALSE);

   dirs = ecore_file_ls(PATH_LIBRARY_DATA);
   //copy albums
   EINA_LIST_FREE(dirs, dir)
   {
      snprintf(buf, sizeof(buf), "%s/%s/", path_library, dir);
      fail_if(ecore_file_mkdir(buf) == EINA_FALSE);

      //copy photos
      snprintf(buf2, sizeof(buf2), "%s/%s", PATH_LIBRARY_DATA, dir);
      files = ecore_file_ls(buf2);
      EINA_LIST_FREE(files, file)
      {
         snprintf(buf3, sizeof(buf3), "%s/%s", buf2, file);
         snprintf(buf4, sizeof(buf4), "%s/%s", buf, file);
         fail_if(ecore_file_cp(buf3, buf4) == EINA_FALSE);

         free(file);
      }

      free(dir);
   }
}

static void _check_library(Enlil_Library *library)
{
   Eina_List *l, *l2;
   Enlil_Album *album;
   Enlil_Photo *photo;

   fail_if(eina_list_count(enlil_library_albums_get(library)) != eina_list_count(test_library->albums));

   int i = 0;
   EINA_LIST_FOREACH(enlil_library_albums_get(library), l, album)
   {
      Test_Album *test_album = eina_list_nth(test_library->albums, i);
      fail_if(strcmp(enlil_album_name_get(album), test_album->name));

      fail_if(eina_list_count(enlil_album_photos_get(album)) != eina_list_count(test_album->photos));
      int j = 0;
      EINA_LIST_FOREACH(enlil_album_photos_get(album), l2, photo)
      {
        Test_Photo *test_photo = eina_list_nth(test_album->photos, j);
        fail_if(strcmp(enlil_photo_name_get(photo), test_photo->name));
        fail_if(strcmp(enlil_photo_file_name_get(photo), test_photo->file_name));

        j++;
      }
      i++;
   }
}

//static void
//_load_done_cb(void       *data,
//              Enlil_Load *load,
//              int         nb_albums,
//              int         nb_photos)
//{
//   Enlil_Library *library = data;
//
//   enlil_load_free(&load);
//   enlil_library_free(&library);
//
//   fail_if(nb_albums != 4);
//   fail_if(nb_photos != 12);
//
//   ecore_main_loop_quit();
//}
//
//
//static void
//_load_error_cb(void       *data __UNUSED__,
//               Enlil_Load *load __UNUSED__,
//               Load_Error  error __UNUSED__,
//               const char *msg)
//{
//   fail();
//}


static void
_sync_done_cb(void       *data __UNUSED__,
              Enlil_Sync *_sync __UNUSED__)
{
   Enlil_Library *library = data;
   _check_library(library);
   ecore_main_loop_quit();
}


static void
_sync_error_cb(void       *data __UNUSED__,
               Enlil_Sync *_sync __UNUSED__,
               Sync_Error  error __UNUSED__,
               const char *msg)
{
   fail();
}



START_TEST(load)
{
   init_library();

   Enlil_Library *library = enlil_library_new(NULL, NULL, NULL, NULL, NULL, NULL,
                                     NULL, NULL, NULL, NULL, NULL, NULL,
                                     NULL, NULL, NULL);
   enlil_library_path_set(library, path_library);
   fail_if(library == NULL);

   Enlil_Sync *_sync = enlil_sync_new(enlil_library_path_get(library),
                                         _sync_album_new_cb, _sync_album_update_cb, _sync_album_disappear_cb,
                                         _sync_photo_new_cb, _sync_photo_update_cb, _sync_photo_disappear_cb,
                                         _sync_done_cb, _sync_start_cb, _sync_error_cb, library);
   enlil_library_sync_set(library, _sync);

   enlil_sync_job_all_add(_sync);

//   Enlil_Load *enlil_load = enlil_load_new(library, _load_album_done_cb, _load_done_cb, _load_error_cb, library);
//   enlil_load_run(enlil_load);

   ecore_main_loop_begin();

   shutdown_library();
}END_TEST

void
enlil_test_sync(TCase *tc)
{
   tcase_add_test(tc, load);
}
