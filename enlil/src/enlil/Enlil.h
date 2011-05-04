#ifndef  ENLIL_INC
#define  ENLIL_INC

#include <Eina.h>
#include <Eet.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ethumb_Client.h>

#ifdef EAPI
  # undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_ENLIL_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_ENLIL_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#define __UNUSED__ __attribute__((unused))

/** TODO
 * Browse albums recursively
 * Create an album for photos which are in the library directory
 *
 */

typedef struct enlil_library              Enlil_Library;
typedef struct enlil_collection           Enlil_Collection;
typedef struct enlil_tag                  Enlil_Tag;
typedef struct Enlil_Exif                 Enlil_Exif;
typedef struct Enlil_IPTC                 Enlil_IPTC;
typedef struct enlil_album                Enlil_Album;
typedef struct enlil_photo                Enlil_Photo;
typedef struct enlil_sync                 Enlil_Sync;
typedef struct enlil_album_collection     Enlil_Album_Collection;
typedef struct enlil_photo_tag            Enlil_Photo_Tag;
typedef struct Enlil_String               Enlil_String;
typedef struct Enlil_Geocaching           Enlil_Geocaching;
typedef struct Enlil_Geocaching_Log       Enlil_Geocaching_Log;
typedef struct Enlil_Geocaching_Travelbug Enlil_Geocaching_Travelbug;
typedef enum enlil_photo_sort             Enlil_Photo_Sort;
typedef enum Enlil_Photo_Type             Enlil_Photo_Type;
typedef enum Enlil_Album_Access_Type      Enlil_Album_Access_Type;

/* CB used when a change occurs in an library folder or an album */
typedef struct Enlil_Configuration Enlil_Configuration;
typedef void                     (*Enlil_Album_New_Cb)(void *data, Enlil_Library *library, const char *path);
typedef void                     (*Enlil_Album_Delete_Cb)(void *data, Enlil_Library *library, const char *path);
typedef void                     (*Enlil_Delete_Cb)(void *data, Enlil_Library *library);
typedef void                     (*Enlil_Photo_New_Cb)(void *data, Enlil_Library *library, Enlil_Album *album, const char *path);
typedef void                     (*Enlil_Photo_Delete_Cb)(void *data, Enlil_Library *library, Enlil_Album *album, const char *path);
typedef void                     (*Enlil_Photo_Update_Cb)(void *data, Enlil_Library *library, Enlil_Album *album, const char *path);
typedef void                     (*Enlil_Collection_New_Cb)(void *data, Enlil_Library *library, Enlil_Collection *collection);
typedef void                     (*Enlil_Collection_Delete_Cb)(void *data, Enlil_Library *library, Enlil_Collection *collection);
typedef void                     (*Enlil_Collection_Album_New_Cb)(void *data, Enlil_Library *library, Enlil_Collection *collection, Enlil_Album *album);
typedef void                     (*Enlil_Collection_Album_Delete_Cb)(void *data, Enlil_Library *library, Enlil_Collection *collection, Enlil_Album *album);
typedef void                     (*Enlil_Tag_New_Cb)(void *data, Enlil_Library *library, Enlil_Tag *tag);
typedef void                     (*Enlil_Tag_Delete_Cb)(void *data, Enlil_Library *library, Enlil_Tag *tag);
typedef void                     (*Enlil_Tag_Photo_New_Cb)(void *data, Enlil_Library *library, Enlil_Tag *tag, Enlil_Photo *photo);
typedef void                     (*Enlil_Tag_Photo_Delete_Cb)(void *data, Enlil_Library *library, Enlil_Tag *tag, Enlil_Photo *photo);
//
typedef void                     (*Enlil_Album_Free_Cb)(Enlil_Album *album, void *data);
typedef void                     (*Enlil_Photo_Free_Cb)(Enlil_Photo *photo, void *data);
typedef void                     (*Enlil_Collection_Free_Cb)(Enlil_Collection *col, void *data);
typedef void                     (*Enlil_Tag_Free_Cb)(Enlil_Tag *tag, void *data);
typedef void                     (*Enlil_Geocaching_Free_Cb)(Enlil_Geocaching *gp, void *data);

//when the version change
typedef void (*Enlil_Album_Version_Header_Increase_Cb)(void *data, Enlil_Album *album);
typedef void (*Enlil_Photo_Version_Header_Increase_Cb)(void *data, Enlil_Photo *photo);
typedef void (*Enlil_Photo_Version_Tags_Increase_Cb)(void *data, Enlil_Photo *photo);
//

enum enlil_photo_sort
{
   ENLIL_PHOTO_SORT_NONE,
   ENLIL_PHOTO_SORT_DATE,
   ENLIL_PHOTO_SORT_NAME
};

enum Enlil_Photo_Type
{
   ENLIL_PHOTO_TYPE_PHOTO,
   ENLIL_PHOTO_TYPE_VIDEO,
   ENLIL_PHOTO_TYPE_GPX
};

enum Enlil_Album_Access_Type
{
   ENLIL_ALBUM_ACCESS_TYPE_PUBLIC,
   ENLIL_ALBUM_ACCESS_TYPE_PRIVATE
};

/**
 * A structure with a simple string
 */
struct Enlil_String
{
   /* this field is in eina_stringshare, use eina_stringshare_del() to free it */
    const char *string;
};
EAPI Eet_Data_Descriptor *enlil_string_edd_new(void);

struct enlil_album_collection
{
   const char       *name;
   Enlil_Collection *collection;

   Enlil_Album      *album;
};

struct enlil_photo_tag
{
   const char  *name;
   Enlil_Tag   *tag;

   Enlil_Photo *photo;
};

EAPI int enlil_init(void);
EAPI int enlil_shutdown(void);
EAPI int enlil_eet_app_data_save(Eet_Data_Descriptor *edd,
				 const char *key,
				 void *data);
EAPI void *enlil_eet_app_data_load(Eet_Data_Descriptor *edd,
				   const char *key);
EAPI int enlil_library_eet_path_delete(Enlil_Library *library);

EAPI Enlil_Library *enlil_library_new(Enlil_Album_New_Cb monitor_album_new_cb,
				      Enlil_Album_Delete_Cb monitor_album_delete_cb,
				      Enlil_Delete_Cb monitor_enlil_delete_cb,
				      Enlil_Photo_New_Cb monitor_photo_new_cb,
				      Enlil_Photo_Delete_Cb monitor_photo_delete_cb,
				      Enlil_Photo_Update_Cb monitor_photo_update_cb,
				      Enlil_Collection_New_Cb col_new_cb,
				      Enlil_Collection_Delete_Cb col_delete_cb,
				      Enlil_Collection_Album_New_Cb col_album_new_cb,
				      Enlil_Collection_Album_Delete_Cb col_album_delete_cb,
				      Enlil_Tag_New_Cb tag_new_cb,
				      Enlil_Tag_Delete_Cb tag_delete_cb,
				      Enlil_Tag_Photo_New_Cb tag_photo_new_cb,
				      Enlil_Tag_Photo_Delete_Cb tag_photo_delete_cb,
				      void *user_data);

EAPI void enlil_library_free(Enlil_Library **library);
EAPI void enlil_library_path_set(Enlil_Library *library,
				 const char *path);
EAPI const char *enlil_library_path_get(const Enlil_Library *library);
EAPI void enlil_library_netsync_account_set(Enlil_Library *library,
					    const char *account);
EAPI const char *enlil_library_netsync_account_get(Enlil_Library *library);
EAPI void enlil_library_netsync_password_set(Enlil_Library *library,
                                   const char *password);
EAPI const char *enlil_library_netsync_password_get(Enlil_Library *library);
EAPI void enlil_library_netsync_host_set(Enlil_Library *library,
                               const char *host);
EAPI const char *enlil_library_netsync_host_get(Enlil_Library *library);
EAPI void enlil_library_netsync_path_set(Enlil_Library *library,
                                         const char *path);
EAPI const char *enlil_library_netsync_path_get(Enlil_Library *library);
EAPI void enlil_library_album_add(Enlil_Library *library,
                        Enlil_Album *album);
EAPI Enlil_Album *enlil_library_album_prev_get(Enlil_Library *library,
                             Enlil_Album *album);
EAPI Eina_List *enlil_library_albums_get(const Enlil_Library *library);
EAPI void enlil_library_sync_set(Enlil_Library *library,
                       Enlil_Sync *sync);
EAPI Enlil_Sync *enlil_library_sync_get(const Enlil_Library *library);
EAPI void enlil_library_print(Enlil_Library *library);
EAPI Enlil_Album *enlil_library_album_search_file_name(Enlil_Library *library,
                                                       const char *file_name);
EAPI Enlil_Album *enlil_library_album_search_name(Enlil_Library *library,
                                                  const char *name);
EAPI void enlil_library_album_remove(Enlil_Library *library,
                                     Enlil_Album *album);
EAPI void enlil_library_monitor_start(Enlil_Library *library);
EAPI void enlil_library_monitor_stop(Enlil_Library *library);
EAPI void enlil_library_albums_sort(Enlil_Library *library);
EAPI Enlil_Photo *enlil_library_search_photo_get(const char *library_path,
                                                  int first_second);
EAPI Enlil_Photo *enlil_library_photo_get(const char *library_path,
                                           int first_second);
EAPI void         enlil_library_photo_set(Enlil_Library *library,
                                           const Enlil_Photo *photo,
                                           int first_second);

EAPI const Eina_List *enlil_library_collections_get(const Enlil_Library *library);
EAPI void enlil_library_collection_del(Enlil_Library *library,
                                       Enlil_Collection *col);
EAPI Enlil_Collection *enlil_library_collection_search_name(Enlil_Library *library,
                                                            const char *name);

EAPI const Eina_List *enlil_library_tags_get(const Enlil_Library *library);
EAPI void enlil_library_tag_del(Enlil_Library *library,
                                Enlil_Tag *tag);
EAPI Enlil_Tag *enlil_library_tag_search_name(Enlil_Library *library,
                                              const char *name);

EAPI Enlil_Album *enlil_library_album_search_netsync_id(Enlil_Library *library,
                                                        const char *id);

EAPI void enlil_library_album_version_header_increase_cb_set(Enlil_Library *library,
                                                             Enlil_Album_Version_Header_Increase_Cb cb,
                                                             void *data);
EAPI void enlil_library_photo_version_header_increase_cb_set(Enlil_Library *library,
                                                             Enlil_Photo_Version_Header_Increase_Cb cb,
                                                             void *data);
EAPI void enlil_library_photo_version_tags_increase_cb_set(Enlil_Library *library,
                                                           Enlil_Photo_Version_Tags_Increase_Cb cb,
                                                           void *data);

EAPI Eina_List *enlil_library_eet_path_load(void);
EAPI int enlil_library_eet_path_save(Enlil_Library *library);
EAPI Enlil_Library *enlil_library_eet_albums_load(Enlil_Library *library);
EAPI int enlil_library_eet_albums_save(Enlil_Library *library);
EAPI Enlil_Album *enlil_library_eet_album_load(Enlil_Library *library,
                                               const char *key);
EAPI int enlil_library_eet_album_remove(Enlil_Library *library,
                                        const char *key);
EAPI Enlil_Library *enlil_library_eet_collections_load(Enlil_Library *library);
EAPI int enlil_library_eet_collections_save(Enlil_Library *library);
EAPI Enlil_Library *enlil_library_eet_tags_load(Enlil_Library *library);
EAPI int enlil_library_eet_tags_save(Enlil_Library *library);

EAPI Enlil_Collection * enlil_collection_new(void);
EAPI void enlil_collection_free(Enlil_Collection **col);
EAPI Enlil_Collection *enlil_collection_copy_new(const Enlil_Collection *tag);
EAPI void enlil_collection_copy(const Enlil_Collection *col_src,
                                Enlil_Collection *col_dest);
EAPI const char *enlil_collection_name_get(const Enlil_Collection *col);
EAPI void enlil_collection_name_set(Enlil_Collection *col,
                                    const char *name);
EAPI const char *enlil_collection_description_get(const Enlil_Collection *col);
EAPI void enlil_collection_description_set(Enlil_Collection *col,
                                           const char *description);
EAPI Eina_List *enlil_collection_albums_get(const Enlil_Collection *col);
EAPI int enlil_collection_albums_count_get(const Enlil_Collection *col);
EAPI void enlil_collection_album_add(Enlil_Collection *col,
                                     Enlil_Album *album);
EAPI void enlil_collection_album_remove(Enlil_Collection *col,
                                        Enlil_Album *album);
EAPI void enlil_collection_user_data_set(Enlil_Collection *col,
                                         void *user_data,
                                         Enlil_Collection_Free_Cb cb);
EAPI void *enlil_collection_user_data_get(const Enlil_Collection *col);

EAPI Enlil_Tag *enlil_tag_new(void);
EAPI void enlil_tag_free(Enlil_Tag **tag);
EAPI Enlil_Tag *enlil_tag_copy_new(const Enlil_Tag *tag);
EAPI void enlil_tag_copy(const Enlil_Tag *tag_src,
                         Enlil_Tag *tag_dest);
EAPI const char *enlil_tag_name_get(const Enlil_Tag *tag);
EAPI void enlil_tag_name_set(Enlil_Tag *tag,
                             const char *name);
EAPI const char *enlil_tag_description_get(const Enlil_Tag *tag);
EAPI void enlil_tag_description_set(Enlil_Tag *tag,
                                    const char *description);
EAPI Eina_List *enlil_tag_photos_get(const Enlil_Tag *tag);
EAPI int enlil_tag_photos_count_get(const Enlil_Tag *tag);
EAPI void enlil_tag_photo_add(Enlil_Tag *tag,
                              Enlil_Photo *photo);
EAPI void enlil_tag_photo_remove(Enlil_Tag *tag,
                                 Enlil_Photo *photo);
EAPI void enlil_tag_user_data_set(Enlil_Tag *tag,
                                  void *user_data,
                                  Enlil_Tag_Free_Cb cb);
EAPI void *enlil_tag_user_data_get(const Enlil_Tag *tag);
EAPI void enlil_tags_list_print(const Eina_List *tags);
EAPI void enlil_tag_print(const Enlil_Tag *tag);

EAPI Enlil_Album *enlil_album_new(void);
EAPI Enlil_Album *enlil_album_copy_new(const Enlil_Album *album);
EAPI void enlil_album_copy(const Enlil_Album *album_src,
                           Enlil_Album *album_dest);
EAPI void enlil_album_free(Enlil_Album **album);
EAPI void enlil_album_name_set(Enlil_Album *album,
                               const char *name);
EAPI void enlil_album_access_type_set(Enlil_Album *album,
                                      Enlil_Album_Access_Type access_type);
EAPI void enlil_album_description_set(Enlil_Album *album,
                                      const char *desc);
EAPI void enlil_album_file_name_set(Enlil_Album *album,
                                    const char *file_name);
EAPI void enlil_album_path_set(Enlil_Album *album,
                               const char *path);
EAPI void enlil_album__time_set(Enlil_Album *album,
                                long long time);
EAPI void enlil_album_photos_set(Enlil_Album *album,
                                 Eina_List *photos);
EAPI void enlil_album_library_set(Enlil_Album *album,
                                  Enlil_Library *library);
EAPI Enlil_Library *enlil_album_library_get(const Enlil_Album *album);
EAPI const char *enlil_album_name_get(const Enlil_Album *album);
EAPI Enlil_Album_Access_Type enlil_album_access_type_get(const Enlil_Album *album);
EAPI const char *enlil_album_description_get(const Enlil_Album *album);
EAPI const char *enlil_album_file_name_get(const Enlil_Album *album);
EAPI const char *enlil_album_path_get(const Enlil_Album *album);
EAPI long long enlil_album__time_get(const Enlil_Album *album);
EAPI void enlil_album_list_print(Eina_List *library);
EAPI void enlil_album_print(Enlil_Album *album);
EAPI void enlil_album_photo_add(Enlil_Album *album,
                                Enlil_Photo *photo);
EAPI void enlil_album_photo_remove(Enlil_Album *album,
                                   Enlil_Photo *photo);
EAPI Eina_List *enlil_album_photos_get(Enlil_Album *album);
EAPI Enlil_Photo *enlil_album_photo_prev_get(Enlil_Album *album,
                                             Enlil_Photo *photo);
EAPI int enlil_album_photos_count_get(Enlil_Album *album);
EAPI Enlil_Photo *enlil_album_photo_search_file_name(Enlil_Album *album,
                                                     const char *file_name);
EAPI void enlil_album_monitor_start(Enlil_Album *album);
EAPI void enlil_album_monitor_stop(Enlil_Album *album);
EAPI void enlil_album_user_data_set(Enlil_Album *album,
                                    void *user_data,
                                    Enlil_Album_Free_Cb cb);
EAPI void *enlil_album_user_data_get(const Enlil_Album *album);

EAPI void enlil_album_photos_sort_set(Enlil_Album *album,
                                      Enlil_Photo_Sort photos_sort);
EAPI Enlil_Photo_Sort enlil_album_photos_sort_get(const Enlil_Album *album);

EAPI void enlil_album_collection_process(Enlil_Album *album);
EAPI void enlil_album_collection_add(Enlil_Album *album,
                                     const char *col_name);
EAPI const Eina_List *enlil_album_collections_get(const Enlil_Album *album);
EAPI void enlil_album_collection_remove(Enlil_Album *album,
                                        Enlil_Album_Collection *album_col);

EAPI const char *album_access_type_to_string(Enlil_Album_Access_Type access_type);
EAPI Enlil_Album_Access_Type string_to_album_access_type(const char *access_type);

EAPI void enlil_photo_tag_process(Enlil_Photo *photo);
EAPI void enlil_photo_tag_add(Enlil_Photo *photo,
                              const char *tag_name);
EAPI const Eina_List *enlil_photo_tags_get(const Enlil_Photo *photo);
EAPI void enlil_photo_tag_remove(Enlil_Photo *photo,
                                 Enlil_Photo_Tag *photo_tag);

EAPI int enlil_album_eet_global_header_save(Enlil_Album *album);
EAPI int enlil_album_eet_header_save(Enlil_Album *album);
EAPI int enlil_album_eet_photos_list_save(Enlil_Album *album);

EAPI int enlil_album_netsync_id_get(Enlil_Album *album);
EAPI int enlil_album_netsync_version_header_get(Enlil_Album *album);
EAPI int enlil_album_netsync_version_header_net_get(Enlil_Album *album);
EAPI int enlil_album_netsync_timestamp_last_update_collections_get(Enlil_Album *album);

EAPI Enlil_Photo *enlil_photo_new(void);
EAPI void enlil_photo_free(Enlil_Photo **photo);
EAPI void enlil_photo_album_set(Enlil_Photo *photo,
                                Enlil_Album *album);
EAPI void enlil_photo_name_set(Enlil_Photo *photo,
                               const char *name);
EAPI void enlil_photo_author_set(Enlil_Photo *photo,
                                 const char *author);
EAPI void enlil_photo_description_set(Enlil_Photo *photo,
                                      const char *desc);
EAPI void enlil_photo_path_set(Enlil_Photo *photo,
                               const char *path);
EAPI void enlil_photo_file_name_set(Enlil_Photo *photo,
                                    const char *file_name);
EAPI void enlil_photo_size_set(Enlil_Photo *photo,
                               int size);
EAPI void enlil_photo_size_w_set(Enlil_Photo *photo,
                                 int size_w);
EAPI void enlil_photo_size_h_set(Enlil_Photo *photo,
                                 int size_h);
EAPI Enlil_Album *enlil_photo_album_get(const Enlil_Photo *photo);
EAPI double enlil_photo_longitude_get(const Enlil_Photo *photo);
EAPI double enlil_photo_latitude_get(const Enlil_Photo *photo);
EAPI void enlil_photo__time_set(Enlil_Photo *photo,
                                long long time);
EAPI void enlil_photo_type_set(Enlil_Photo *photo,
                               Enlil_Photo_Type type);
EAPI void enlil_photo_mustNotBeSaved_set(Enlil_Photo *photo,
                                         Eina_Bool save);
EAPI const char *enlil_photo_name_get(const Enlil_Photo *photo);
EAPI const char *enlil_photo_author_get(const Enlil_Photo *photo);
EAPI const char *enlil_photo_description_get(const Enlil_Photo *photo);
EAPI void enlil_photo_eet_save_set(Enlil_Photo *photo,
                                   Eina_Bool save);
EAPI Eina_Bool enlil_photo_eet_save_get(const Enlil_Photo *photo);
EAPI Eina_Bool enlil_photo_mustNotBeSaved(const Enlil_Photo *photo);
EAPI const char *enlil_photo_path_get(const Enlil_Photo *photo);
EAPI const char * enlil_photo_file_name_get(const Enlil_Photo *photo);
EAPI Enlil_Photo_Type enlil_photo_type_get(const Enlil_Photo *photo);
EAPI int enlil_photo_size_get(const Enlil_Photo *photo);
EAPI int enlil_photo_size_w_get(const Enlil_Photo *photo);
EAPI int enlil_photo_size_h_get(const Enlil_Photo *photo);
EAPI long long enlil_photo__time_get(const Enlil_Photo *photo);
EAPI void enlil_photo_list_print(const Eina_List *l_photos);
EAPI void enlil_photo_print(const Enlil_Photo *photo);
EAPI int enlil_photo_is(const char *file);
EAPI int enlil_photo_jpg_is(const char *file);
EAPI int enlil_video_is(const char *file);
EAPI int enlil_gpx_is(const char *file);
EAPI Enlil_Photo *enlil_photo_copy_new(Enlil_Photo *photo);
EAPI void enlil_photo_copy(Enlil_Photo *photo_src,
                           Enlil_Photo *photo_dest);
EAPI void enlil_photo_user_data_set(Enlil_Photo *photo,
                                    void *user_data,
                                    Enlil_Photo_Free_Cb cb);
EAPI void *enlil_photo_user_data_get(const Enlil_Photo *photo);
EAPI void enlil_photo_thumb_fdo_large_set(Enlil_Photo *photo,
                                          const char *thumb);
EAPI const char *enlil_photo_thumb_fdo_large_get(const Enlil_Photo *photo);
EAPI void enlil_photo_thumb_fdo_normal_set(Enlil_Photo *photo,
                                           const char *thumb);
EAPI const char *enlil_photo_thumb_fdo_normal_get(const Enlil_Photo *photo);

EAPI void enlil_photo_tag_process(Enlil_Photo *photo);
EAPI void enlil_photo_tag_add(Enlil_Photo *photo,
                              const char *tag_name);
EAPI const Eina_List *enlil_photo_tags_get(const Enlil_Photo *photo);
EAPI void enlil_photo_tag_remove(Enlil_Photo *photo,
                                 Enlil_Photo_Tag *photo_tag);
EAPI void enlil_photo_tag_clear(Enlil_Photo *photo);

EAPI void enlil_photo_exifs_set(Enlil_Photo *photo,
                                Eina_List *exifs);
EAPI const Eina_List *enlil_photo_exifs_get(const Enlil_Photo *photo);
EAPI void enlil_photo_exif_loaded_set(Enlil_Photo *photo,
                                      Eina_Bool loaded);
EAPI Eina_Bool enlil_photo_exif_loaded_get(const Enlil_Photo *photo);
EAPI Enlil_Exif *enlil_photo_exif_get_from_tag(const Enlil_Photo *photo,
                                               const char *tag);
EAPI void enlil_photo_exif_clear(Enlil_Photo *photo);
EAPI void enlil_photo_copy_exif_in_file(Enlil_Photo *photo,
                                        const char *file);

EAPI void enlil_photo_iptcs_set(Enlil_Photo *photo,
                                Eina_List *iptcs);
EAPI const Eina_List *enlil_photo_iptcs_get(const Enlil_Photo *photo);
EAPI void enlil_photo_iptc_loaded_set(Enlil_Photo *photo,
                                      Eina_Bool loaded);
EAPI Eina_Bool enlil_photo_iptc_loaded_get(const Enlil_Photo *photo);
EAPI void enlil_photo_iptc_clear(Enlil_Photo *photo);
EAPI void enlil_photo_save_iptc_in_custom_file(Enlil_Photo *photo,
                                               const char *file);
EAPI void enlil_photo_save_iptc_in_file(Enlil_Photo *photo);
EAPI Enlil_IPTC *enlil_photo_iptc_get_from_name_and_value(const Enlil_Photo *photo,
                                                          const char *name,
                                                          const char *value);
EAPI Eina_Bool enlil_photo_iptc_add(Enlil_Photo *photo,
                                    const char *name,
                                    const char *value,
                                    Eina_Bool check_value);
EAPI Eina_Bool enlil_photo_iptc_remove(Enlil_Photo *photo,
                                       const char *name,
                                       const char *value);

EAPI Enlil_Photo *enlil_photo_eet_load(const char *eet_path,
                                       const char *key);
EAPI int enlil_photo_eet_remove(const char *eet_path,
                                const char *key);
EAPI int enlil_photo_eet_save(Enlil_Photo *photo);

EAPI int enlil_photo_netsync_id_get(Enlil_Photo *photo);
EAPI int enlil_photo_netsync_version_header_get(Enlil_Photo *photo);
EAPI int enlil_photo_netsync_version_tags_get(Enlil_Photo *photo);
EAPI int enlil_photo_netsync_version_file_get(Enlil_Photo *photo);
EAPI int enlil_photo_netsync_version_header_net_get(Enlil_Photo *photo);
EAPI int enlil_photo_netsync_version_tags_net_get(Enlil_Photo *photo);
EAPI int enlil_photo_netsync_version_file_net_get(Enlil_Photo *photo);

/* File manager */
EAPI Eet_File *enlil_file_manager_open(const char *file);
EAPI void enlil_file_manager_close(const char *file);
EAPI void enlil_file_manager_flush(void);

/* Synchronisation Files <-> .eet */

enum Sync_Error
{
   Sync_Error_Eet_Save_Failed
};

typedef enum Sync_Error Sync_Error;
typedef void          (*Enlil_Sync_Album_New_Cb)(void *data, Enlil_Sync *sync, Enlil_Library *library, Enlil_Album *album);
typedef void          (*Enlil_Sync_Album_Update_Cb)(void *data, Enlil_Sync *sync, Enlil_Library *library, Enlil_Album *album);
typedef void          (*Enlil_Sync_Album_Disappear_Cb)(void *data, Enlil_Sync *sync, Enlil_Library *library, Enlil_Album *album);
typedef void          (*Enlil_Sync_Photo_New_Cb)(void *data, Enlil_Sync *sync, Enlil_Album *album, Enlil_Photo *photo);
typedef void          (*Enlil_Sync_Photo_Update_Cb)(void *data, Enlil_Sync *sync, Enlil_Album *album, Enlil_Photo *photo);
typedef void          (*Enlil_Sync_Photo_Disappear_Cb)(void *data, Enlil_Sync *sync, Enlil_Album *album, Enlil_Photo *photo);
typedef void          (*Enlil_Sync_Done_Cb)(void *data, Enlil_Sync *sync);
typedef void          (*Enlil_Sync_Start_Cb)(void *data, Enlil_Sync *sync);
typedef void          (*Enlil_Sync_Error_Cb)(void *data, Enlil_Sync *sync, Sync_Error error, const char *msg);

EAPI Enlil_Sync *enlil_sync_new(const char *path,
                                Enlil_Sync_Album_New_Cb album_new_cb,
                                Enlil_Sync_Album_Update_Cb album_update_cb,
                                Enlil_Sync_Album_Disappear_Cb album_disappear_cb,
                                Enlil_Sync_Photo_New_Cb photo_new_cb,
                                Enlil_Sync_Photo_Update_Cb photo_update_cb,
                                Enlil_Sync_Photo_Disappear_Cb photo_disappear_cb,
                                Enlil_Sync_Done_Cb done_cb,
                                Enlil_Sync_Start_Cb start_cb,
                                Enlil_Sync_Error_Cb error_cb,
                                void *user_data);
EAPI void enlil_sync_free(Enlil_Sync **sync);
EAPI int enlil_sync_jobs_count_get(Enlil_Sync *sync);
EAPI void enlil_sync_job_all_add(Enlil_Sync *sync);
EAPI void enlil_sync_job_album_folder_add(Enlil_Sync *sync,
                                          const char *folder);
EAPI void enlil_sync_job_photo_file_add(Enlil_Sync *sync,
                                        const char *folder,
                                        const char *file);
//

/* Load the files from the .eet*/
enum Load_Error
{
   Load_Error_Eet_Save_Failed
};
typedef struct enlil_load               Enlil_Load;
typedef struct Enlil_Load_Configuration Enlil_Load_Configuration;
typedef enum Load_Error                 Load_Error;
typedef void                          (*Enlil_Load_Conf_Album_Done_Cb)(void *data, Enlil_Load *load, Enlil_Library *library, Enlil_Album *album);
typedef void                          (*Enlil_Load_Conf_Done_Cb)(void *data, Enlil_Load *load, int nb_albums, int nb_photos);
typedef void                          (*Enlil_Load_Conf_Error_Cb)(void *data, Enlil_Load *load, Load_Error error, const char *msg);

EAPI Enlil_Load *enlil_load_new(Enlil_Library *library,
                                Enlil_Load_Conf_Album_Done_Cb album_done_cb,
                                Enlil_Load_Conf_Done_Cb done_cb,
                                Enlil_Load_Conf_Error_Cb error_cb,
                                void *user_data);
EAPI void enlil_load_free(Enlil_Load **load);
EAPI void enlil_load_run(Enlil_Load *load);
EAPI void enlil_load_stop(Enlil_Load *load);
//

/* thumbnails */
typedef enum Enlil_Thumb_Job_Type Enlil_Thumb_Job_Type;
enum Enlil_Thumb_Job_Type
{
   Enlil_THUMB_FDO_NORMAL,
   Enlil_THUMB_FDO_LARGE
};

typedef void (*Enlil_Thumb_Done_Cb)(void *data, Enlil_Photo *photo, const char *file);
typedef void (*Enlil_Thumb_Error_Cb)(void *data, Enlil_Photo *photo);
EAPI const char *
enlil_thumb_photo_get(Enlil_Photo *photo,
                      Enlil_Thumb_Job_Type type,
                      Enlil_Thumb_Done_Cb done_cb,
                      Enlil_Thumb_Error_Cb error_cb,
                      void *data);
EAPI void enlil_thumb_clear(void);
EAPI void enlil_thumb_photo_clear(const Enlil_Photo *photo);

/* transformations */
typedef enum Enlil_Trans_Job_Type       Enlil_Trans_Job_Type;
typedef struct Enlil_Trans_Job          Enlil_Trans_Job;
typedef struct Enlil_Trans_History      Enlil_Trans_History;
typedef struct Enlil_Trans_History_Item Enlil_Trans_History_Item;
enum Enlil_Trans_Job_Type
{
   Enlil_TRANS_ROTATE_180,
   Enlil_TRANS_ROTATE_90,
   Enlil_TRANS_ROTATE_R90,
   Enlil_TRANS_FLIP_VERTICAL,
   Enlil_TRANS_FLIP_HORIZONTAL,
   Enlil_TRANS_BLUR,
   Enlil_TRANS_SHARPEN,
   Enlil_TRANS_GRAYSCALE,
   Enlil_TRANS_SEPIA
};
typedef void (*Enlil_Trans_Done_Cb)(void *data, Enlil_Trans_Job *job, const char *file);

EAPI Enlil_Trans_Job *enlil_trans_job_add(Enlil_Trans_History *h,
                                          const char *file,
                                          Enlil_Trans_Job_Type type,
                                          Enlil_Trans_Done_Cb cb,
                                          void *data);
EAPI void enlil_trans_job_del(Enlil_Trans_Job *job);

EAPI Enlil_Trans_History *enlil_trans_history_new(const char *file);
EAPI void enlil_trans_history_free(Enlil_Trans_History *h);
EAPI void enlil_trans_history_clear(Enlil_Trans_History *h);
EAPI const Eina_List *enlil_trans_history_get(const Enlil_Trans_History *h);
EAPI const Enlil_Trans_History_Item *enlil_trans_history_current_get(const Enlil_Trans_History *h);
EAPI const char *enlil_trans_history_item_file_get(const Enlil_Trans_History_Item *item);
EAPI Enlil_Trans_Job_Type enlil_trans_history_item_type_get(const Enlil_Trans_History_Item *item);
EAPI const char *enlil_trans_history_goto(Enlil_Trans_History *h, int pos);

/* Exif loader */
typedef struct Enlil_Exif_Job Enlil_Exif_Job;
typedef void                (*Enlil_Exif_Done_Cb)(void *data, Enlil_Exif_Job *job, Eina_List *exifs);

EAPI Enlil_Exif_Job *enlil_exif_job_append(Enlil_Photo *photo,
                                           Enlil_Exif_Done_Cb cb,
                                           void *data);
EAPI Enlil_Exif_Job *enlil_exif_job_prepend(Enlil_Photo *photo,
                                            Enlil_Exif_Done_Cb cb,
                                            void *data);
EAPI void enlil_exif_job_del(Enlil_Exif_Job *job);

EAPI Enlil_Exif *enlil_exif_new(void);
EAPI Enlil_Exif *enlil_exif_copy_new(Enlil_Exif *exif);
EAPI void enlil_exif_copy(Enlil_Exif *exif_src,
                          Enlil_Exif *exif_dest);
EAPI void enlil_exif_free(Enlil_Exif **exif);
EAPI const char *enlil_exif_tag_get(Enlil_Exif *exif);
EAPI const char *enlil_exif_value_get(Enlil_Exif *exif);
EAPI const char *enlil_exif_description_get(Enlil_Exif *exif);

EAPI Eina_List *eina_list_sorted_diff(Eina_List *list1,
                                      Eina_List *list2,
                                      Eina_List **elt_list1,
                                      Eina_List **elt_list2,
                                      Eina_Compare_Cb func);
EAPI Eina_List *eina_list_right_sorted_diff(Eina_List *list,
                                            Eina_List *sorted_list,
                                            Eina_List **elt_list1,
                                            Eina_List **elt_list2,
                                            Eina_Compare_Cb func);
EAPI Eina_List *eina_list_right_sorted_diff(Eina_List *list,
                                            Eina_List *sorted_list,
                                            Eina_List **elt_list1,
                                            Eina_List **elt_list2,
                                            Eina_Compare_Cb func);

/* IPTC loader */
typedef struct Enlil_IPTC_Job Enlil_IPTC_Job;
typedef void                (*Enlil_IPTC_Done_Cb)(void *data, Enlil_IPTC_Job *job, Eina_List *iptcs);

EAPI Enlil_IPTC_Job *enlil_iptc_job_append(Enlil_Photo *photo,
                                           Enlil_IPTC_Done_Cb cb,
                                           void *data);
EAPI Enlil_IPTC_Job *enlil_iptc_job_prepend(Enlil_Photo *photo,
                                            Enlil_IPTC_Done_Cb cb,
                                            void *data);
EAPI void enlil_iptc_job_del(Enlil_IPTC_Job *job);

EAPI Enlil_IPTC *enlil_iptc_new(void);
EAPI Enlil_IPTC *enlil_iptc_new_from_name(const char *name);
EAPI Enlil_IPTC *enlil_iptc_copy_new(Enlil_IPTC *iptc);
EAPI void enlil_iptc_copy(Enlil_IPTC *iptc_src,
                          Enlil_IPTC *iptc_dest);
EAPI void enlil_iptc_free(Enlil_IPTC **iptc);
EAPI double enlil_iptc_record_get(const Enlil_IPTC *iptc);
EAPI double enlil_iptc_tag_get(const Enlil_IPTC *iptc);
EAPI const char *enlil_iptc_title_get(const Enlil_IPTC *iptc);
EAPI const char *enlil_iptc_name_get(const Enlil_IPTC *iptc);
EAPI const char *enlil_iptc_format_get(const Enlil_IPTC *iptc);
EAPI const char *enlil_iptc_value_get(const Enlil_IPTC *iptc);
EAPI void enlil_iptc_value_set(Enlil_IPTC *iptc,
                               const char *value);

//Geocaching
typedef void (*Enlil_Geocaching_Done_Cb)(void *data, Eina_Hash *db);

EAPI Enlil_Geocaching *enlil_geocaching_new(void);
EAPI void enlil_geocaching_free(Enlil_Geocaching *gp);
EAPI const char *enlil_geocaching_name_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_name_set(Enlil_Geocaching *gp,
                                    const char *name);
EAPI const char *enlil_geocaching_time_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_time_set(Enlil_Geocaching *gp,
                                    const char *time);
EAPI const char *enlil_geocaching_description_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_description_set(Enlil_Geocaching *gp,
                                           const char *description);
EAPI const char *enlil_geocaching_url_get(const Enlil_Geocaching *gp);
EAPI void  enlil_geocaching_url_set(Enlil_Geocaching *gp,
                                    const char *url);
EAPI const char *enlil_geocaching_url_name_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_url_name_set(Enlil_Geocaching *gp,
                                        const char *url_name);
EAPI const char *enlil_geocaching_sym_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_sym_set(Enlil_Geocaching *gp,
                                   const char *sym);
EAPI const char *enlil_geocaching_type_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_type_set(Enlil_Geocaching *gp,
                                    const char *type);

EAPI const char *enlil_geocaching_gp_name_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_name_set(Enlil_Geocaching *gp,
                                       const char *gp_name);
EAPI const char *enlil_geocaching_gp_short_desc_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_short_desc_set(Enlil_Geocaching *gp,
                                             const char *gp_short_desc);
EAPI const char *enlil_geocaching_gp_long_desc_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_long_desc_set(Enlil_Geocaching *gp,
                                            const char *gp_long_desc);
EAPI const char *enlil_geocaching_gp_placed_by_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_placed_by_set(Enlil_Geocaching *gp,
                                            const char *gp_placed_by);
EAPI const char *enlil_geocaching_gp_owner_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_owner_set(Enlil_Geocaching *gp,
                                        const char *gp_owner);
EAPI const char *enlil_geocaching_gp_type_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_type_set(Enlil_Geocaching *gp,
                                       const char *gp_type);
EAPI const char *enlil_geocaching_gp_container_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_container_set(Enlil_Geocaching *gp,
                                            const char *gp_container);
EAPI const char *enlil_geocaching_gp_difficulty_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_difficulty_set(Enlil_Geocaching *gp,
                                             const char *gp_difficulty);
EAPI const char *enlil_geocaching_gp_terrain_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_terrain_set(Enlil_Geocaching *gp,
                                          const char *gp_terrain);
EAPI const char *enlil_geocaching_gp_country_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_country_set(Enlil_Geocaching *gp,
                                          const char *gp_country);
EAPI double enlil_geocaching_longitude_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_longitude_set(Enlil_Geocaching *gp,
                                         double longitude);
EAPI double enlil_geocaching_latitude_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_latitude_set(Enlil_Geocaching *gp,
                                        double latitude);
EAPI const char *enlil_geocaching_gp_id_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_id_set(Enlil_Geocaching *gp,
                                     const char *gp_id);
EAPI const char *enlil_geocaching_gp_available_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_available_set(Enlil_Geocaching *gp,
                                            const char *gp_available);
EAPI const char *enlil_geocaching_gp_archived_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_archived_set(Enlil_Geocaching *gp,
                                           const char *gp_archived);
EAPI const char *enlil_geocaching_gp_hints_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_hints_set(Enlil_Geocaching *gp,
                                        const char *gp_hints);
EAPI const char *enlil_geocaching_gp_state_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_state_set(Enlil_Geocaching *gp,
                                        const char *gp_state);
EAPI const char *enlil_geocaching_gp_owner_id_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_gp_owner_id_set(Enlil_Geocaching *gp,
                                           const char *gp_owner_id);
EAPI Eina_List *enlil_geocaching_logs_get(const Enlil_Geocaching *gp);
EAPI Eina_List *enlil_geocaching_tbs_get(const Enlil_Geocaching *gp);

EAPI const char *enlil_geocaching_log_id_get(const Enlil_Geocaching_Log *log);
EAPI void enlil_geocaching_log_id_set(Enlil_Geocaching_Log *log,
                                      const char *log_id);
EAPI const char *enlil_geocaching_log_date_get(const Enlil_Geocaching_Log *log);
EAPI void enlil_geocaching_log_date_set(Enlil_Geocaching_Log *log,
                                        const char *log_date);
EAPI const char *enlil_geocaching_log_type_get(const Enlil_Geocaching_Log *log);
EAPI void enlil_geocaching_log_type_set(Enlil_Geocaching_Log *log,
                                        const char *log_type);
EAPI const char *enlil_geocaching_log_finder_get(const Enlil_Geocaching_Log *log);
EAPI void enlil_geocaching_log_finder_set(Enlil_Geocaching_Log *log,
                                          const char *log_finder);
EAPI const char *enlil_geocaching_log_finder_id_get(const Enlil_Geocaching_Log *log);
EAPI void enlil_geocaching_log_finder_id_set(Enlil_Geocaching_Log *log,
                                             const char *log_finder_id);
EAPI const char *enlil_geocaching_log_text_get(const Enlil_Geocaching_Log *log);
EAPI void enlil_geocaching_log_text_set(Enlil_Geocaching_Log *log,
                                        const char *log_text);
EAPI const char *enlil_geocaching_log_encoded_get(const Enlil_Geocaching_Log *log);
EAPI void enlil_geocaching_log_encoded_set(Enlil_Geocaching_Log *log,
                                           const char *log_encoded);

EAPI const char *enlil_geocaching_tb_id_get(const Enlil_Geocaching_Travelbug *tb);
EAPI void enlil_geocaching_tb_id_set(Enlil_Geocaching_Travelbug *tb,
                                     const char *tb_id);
EAPI const char *enlil_geocaching_tb_ref_get(const Enlil_Geocaching_Travelbug *tb);
EAPI void enlil_geocaching_tb_ref_set(Enlil_Geocaching_Travelbug *tb,
                                      const char *tb_ref);
EAPI const char *enlil_geocaching_tb_name_get(const Enlil_Geocaching_Travelbug *tb);
EAPI void enlil_geocaching_tb_name_set(Enlil_Geocaching_Travelbug *tb,
                                       const char *tb_name);

EAPI Enlil_Geocaching_Log *enlil_geocaching_log_new(void);
EAPI void enlil_geocaching_log_free(Enlil_Geocaching_Log *gp_log);
EAPI Enlil_Geocaching_Travelbug *enlil_geocaching_tb_new(void);
EAPI void enlil_geocaching_tb_free(Enlil_Geocaching_Travelbug *gp_tb);

EAPI void enlil_geocaching_import(const char *file,
                                  Enlil_Geocaching_Done_Cb done_cb,
                                  void *data);
EAPI void enlil_geocaching_get(Enlil_Geocaching_Done_Cb done_cb,
                               void *data);
EAPI void *enlil_geocaching_user_data_get(const Enlil_Geocaching *gp);
EAPI void enlil_geocaching_user_data_set(Enlil_Geocaching *gp,
                                         void *data,
                                         Enlil_Geocaching_Free_Cb free_cb);

//Net sync
typedef struct Enlil_NetSync_Job Enlil_NetSync_Job;

typedef void                   (*Enlil_NetSync_Login_Failed_Cb)(void *data, const char *username, const char *password);
typedef void                   (*Enlil_NetSync_Job_Add_Cb)(void *data, Enlil_NetSync_Job *job, Enlil_Album *album, Enlil_Photo *photo);
typedef void                   (*Enlil_NetSync_Job_Start_Cb)(void *data, Enlil_NetSync_Job *job, Enlil_Album *album, Enlil_Photo *photo);
typedef void                   (*Enlil_NetSync_Job_Done_Cb)(void *data, Enlil_NetSync_Job *job, Enlil_Album *album, Enlil_Photo *photo);
typedef void                   (*Enlil_NetSync_Job_Error_Cb)(void *data, Enlil_NetSync_Job *job, Enlil_Album *album, Enlil_Photo *photo, const char *error);

typedef void                   (*Enlil_NetSync_Album_New_Cb)(void *data, Enlil_Library *library, int album_id);
typedef void                   (*Enlil_NetSync_Album_NotInNetSync_Cb)(void *data, Enlil_Library *library, Enlil_Album *album);
typedef void                   (*Enlil_NetSync_Album_NotUpToDate_Cb)(void *data, Enlil_Library *library, Enlil_Album *album);
typedef void                   (*Enlil_NetSync_Album_NetSyncNotUpToDate_Cb)(void *data, Enlil_Library *library, Enlil_Album *album);
typedef void                   (*Enlil_NetSync_Album_UpToDate_Cb)(void *data, Enlil_Library *library, Enlil_Album *album);

typedef void                   (*Enlil_NetSync_Album_Header_Get_Cb)(void *data, Enlil_Library *library, Enlil_Album *album);

typedef void                   (*Enlil_NetSync_Photo_New_Cb)(void *data, Enlil_Album *album, int photo_id);
typedef void                   (*Enlil_NetSync_Photo_NotInNetSync_Cb)(void *data, Enlil_Album *album, Enlil_Photo *photo);
typedef void                   (*Enlil_NetSync_Photo_NotUpToDate_Cb)(void *data, Enlil_Album *album, Enlil_Photo *photo);
typedef void                   (*Enlil_NetSync_Photo_NetSyncNotUpToDate_Cb)(void *data, Enlil_Album *album, Enlil_Photo *photo);
typedef void                   (*Enlil_NetSync_Photo_UpToDate_Cb)(void *data, Enlil_Album *album, Enlil_Photo *photo);

typedef void                   (*Enlil_NetSync_Photo_Header_Get_Cb)(void *data, Enlil_Album *album, Enlil_Photo *photo);
typedef void                   (*Enlil_NetSync_Photo_Header_New_Get_Cb)(void *data, Enlil_Album *album, Enlil_Photo *photo, const char *url);

typedef void                   (*Enlil_NetSync_Photo_Upload_Start_Cb)(void *data, Enlil_Photo *photo);
typedef void                   (*Enlil_NetSync_Photo_Upload_Done_Cb)(void *data, Enlil_Photo *photo);

EAPI Eina_Bool enlil_netsync_have(void);
EAPI void enlil_netsync_job_del(Enlil_NetSync_Job *job);

EAPI void enlil_netsync_account_set(const char *host,
                                    const char *path,
                                    const char *_account,
                                    const char *_password);
EAPI void enlil_netsync_login_failed_cb_set(Enlil_NetSync_Login_Failed_Cb login_failed_cb,
                                            void *data);
EAPI void enlil_netsync_job_add_cb_set(Enlil_NetSync_Job_Add_Cb add_cb,
                                       void *data);
EAPI void enlil_netsync_job_start_cb_set(Enlil_NetSync_Job_Start_Cb start_cb,
                                         void *data);
EAPI void enlil_netsync_job_done_cb_set(Enlil_NetSync_Job_Done_Cb done_cb,
                                        void *data);
EAPI void enlil_netsync_job_error_cb_set(Enlil_NetSync_Job_Error_Cb error_cb,
                                         void *data);
EAPI void enlil_netsync_disconnect(void);

EAPI void enlil_netsync_reinit(Enlil_Library *library);
EAPI const char *enlil_netsync_auth_url_get(void);
EAPI Enlil_NetSync_Job *enlil_netsync_job_reinit_prepend(Enlil_Library *library);

EAPI Enlil_NetSync_Job *enlil_netsync_job_sync_albums_append(Enlil_Library *library,
                                                             Enlil_NetSync_Album_New_Cb new_cb,
                                                             Enlil_NetSync_Album_NotInNetSync_Cb notinnetsync_cb,
                                                             Enlil_NetSync_Album_NotUpToDate_Cb notuptodate_cb,
                                                             Enlil_NetSync_Album_NetSyncNotUpToDate_Cb netsyncnotuptodate_cb,
                                                             Enlil_NetSync_Album_UpToDate_Cb uptodate_cb,
                                                             void *data);
EAPI Enlil_NetSync_Job *enlil_netsync_job_sync_album_append(Enlil_Library *library,
                                                            Enlil_Album *album,
                                                            Enlil_NetSync_Album_NotUpToDate_Cb notuptodate_cb,
                                                            Enlil_NetSync_Album_NetSyncNotUpToDate_Cb netsyncnotuptodate_cb,
                                                            Enlil_NetSync_Album_UpToDate_Cb uptodate_cb,
                                                            void *data);
EAPI Enlil_NetSync_Job *enlil_netsync_job_get_new_album_header_append(Enlil_Library *library,
                                                                      int id,
                                                                      Enlil_NetSync_Album_Header_Get_Cb new_cb,
                                                                      void *data);
EAPI Enlil_NetSync_Job *enlil_netsync_job_update_local_album_header_append(Enlil_Library *library,
                                                                           Enlil_Album *album,
                                                                           Enlil_NetSync_Album_Header_Get_Cb get_cb,
                                                                           void *data);
EAPI Enlil_NetSync_Job *enlil_netsync_job_update_netsync_album_header_append(Enlil_Library *library,
                                                                             Enlil_Album *album,
                                                                             Enlil_NetSync_Album_Header_Get_Cb get_cb,
                                                                             void *data);
EAPI Enlil_NetSync_Job *enlil_netsync_job_add_album_append(Enlil_Library *library,
                                                           Enlil_Album *album,
                                                           Enlil_NetSync_Album_Header_Get_Cb get_cb,
                                                           void *data);

EAPI Enlil_NetSync_Job *enlil_netsync_job_sync_photos_append(Enlil_Album *album,
                                                             Enlil_NetSync_Photo_New_Cb new_cb,
                                                             Enlil_NetSync_Photo_NotInNetSync_Cb notinnetsync_cb,
                                                             Enlil_NetSync_Photo_NotUpToDate_Cb notuptodate_cb,
                                                             Enlil_NetSync_Photo_NetSyncNotUpToDate_Cb netsyncnotuptodate_cb,
                                                             Enlil_NetSync_Photo_UpToDate_Cb uptodate_cb,
                                                             Enlil_NetSync_Photo_NotUpToDate_Cb tags_notuptodate_cb,
                                                             Enlil_NetSync_Photo_NetSyncNotUpToDate_Cb tags_netsyncnotuptodate_cb,
                                                             Enlil_NetSync_Photo_UpToDate_Cb tags_uptodate_cb,
                                                             void *data);
EAPI Enlil_NetSync_Job *enlil_netsync_job_get_new_photo_header_append(Enlil_Album *album,
                                                                      int id,
                                                                      Enlil_NetSync_Photo_Header_New_Get_Cb get_cb,
                                                                      void *data);
EAPI Enlil_NetSync_Job *enlil_netsync_job_update_local_photo_header_append(Enlil_Album *album,
                                                                           Enlil_Photo *photo,
                                                                           Enlil_NetSync_Photo_Header_Get_Cb get_cb,
                                                                           void *data);
EAPI Enlil_NetSync_Job *enlil_netsync_job_update_netsync_photo_header_append(Enlil_Album *album,
                                                                             Enlil_Photo *photo,
                                                                             Enlil_NetSync_Photo_Header_Get_Cb get_cb,
                                                                             void *data);
EAPI Enlil_NetSync_Job *enlil_netsync_job_add_photo_append(Enlil_Photo *photo,
                                                           Enlil_NetSync_Photo_Header_Get_Cb add_cb,
                                                           Enlil_NetSync_Photo_Upload_Start_Cb upload_start_cb,
                                                           Enlil_NetSync_Photo_Upload_Done_Cb upload_done_cb,
                                                           void *data);

//Download a photo
typedef struct Enlil_Download_Job Enlil_Download_Job;

typedef void                    (*Enlil_Download_Start_Cb)(void *data, Enlil_Photo *photo);
typedef void                    (*Enlil_Download_Done_Cb)(void *data, Enlil_Photo *photo, int status);
typedef int                     (*Enlil_Download_Progress_Cb)(void *data, Enlil_Photo *photo, long int dltotal, long int dlnow);

EAPI void enlil_download_add(Enlil_Photo *photo,
                             const char *source,
                             Enlil_Download_Start_Cb start_cb,
                             Enlil_Download_Progress_Cb progress_cb,
                             Enlil_Download_Done_Cb done_cb,
                             void *data);
EAPI Eina_Bool enlil_download_image_in_list(const char *src,
                             const Enlil_Album *album);
EAPI Eina_Bool enlil_download_photos_of_album_in_list(const Enlil_Album *album,
                                                      Eina_Bool test_current);

#endif   /* ----- #ifndef ENLIL_INC  ----- */
