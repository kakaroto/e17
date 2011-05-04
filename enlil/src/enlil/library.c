#include "enlil_private.h"

/*
 * The file name, the path and the name are saved into library/data.eet. Consequently if the name of the album changed do not forget to update the list of album (enlil_library_eet_albums_save()).
 */

static void _enlil_library_monitor_cb(void               *data,
                                      Ecore_File_Monitor *em,
                                      Ecore_File_Event    event,
                                      const char         *path);
static Eet_Data_Descriptor *_enlil_library_albums_file_name_edd_new(Eet_Data_Descriptor *edd_file_name);
static Eet_Data_Descriptor *_enlil_library_collections_edd_new(Eet_Data_Descriptor *collection_edd);
static Eet_Data_Descriptor *_enlil_library_tags_edd_new(Eet_Data_Descriptor *tag_edd);
static Eet_Data_Descriptor *_enlil_library_header_edd_new(void);
static int                  _sort_albums_name_cb(const void *d1,
                                                 const void *d2);
static int                  _sort_collections_name_cb(const void *d1,
                                                      const void *d2);
static int                  _sort_tags_name_cb(const void *d1,
                                               const void *d2);

static int  _library_eet_header_save(const Enlil_Library *library);
static void _library_eet_header_load(Enlil_Library *library);

struct enlil_library
{
   const char         *path;
   Eina_List          *albums;

   //generally  a sync is associated to a photo manager
   //that's why we allow the user to set a sync here
   Enlil_Sync         *sync;

   Ecore_File_Monitor *monitor;
   Enlil_Configuration conf;

   const char         *photo1; //photo use to describe the library
   const char         *photo2;

   //list of Enlil_Collection *
   Eina_List          *collections;
   //list of Enlil_Tag *
   Eina_List          *tags;

   //associated netsync
   struct
   {
      const char *username;
      const char *password;
      const char *host;
      const char *path;
   } netsync;

   Eina_Bool                              header_load_is : 1;

   Enlil_Album_Version_Header_Increase_Cb album_version_header_increase_cb;
   void                                  *album_version_header_increase_data;

   Enlil_Photo_Version_Header_Increase_Cb photo_version_header_increase_cb;
   void                                  *photo_version_header_increase_data;

   Enlil_Photo_Version_Tags_Increase_Cb   photo_version_tags_increase_cb;
   void                                  *photo_version_tags_increase_data;
};

#define ROOT_HEADER_LOAD(library)         \
  if(!library->header_load_is)            \
    {                                     \
       _library_eet_header_load(library); \
       library->header_load_is = 1;       \
    }

/*
 * Create a new library node. The callbacks arguments named monitor_* can be NULL if the folder
 * is not monitored. The callbacks named col_* can be null if you do not want to be notified when a change occurs in the collections.
 *
 * @param album_new_cb  Callback called by the monitoring when a new album appears
 * @param album_delete_cb  Callback called by the monitoring when a album is deleted
 * @param enlil_delete_cb  Callback called by the monitoring when the library folder is deleted
 * @param photo_new_cb  Callback called by the monitoring when a new photo appears
 * @param photo_delete_cb  Callback called by the monitoring when a photo is deleted
 * @param photo_update_cb  Callback called by the monitoring when a photo is updated
 * @param user_data Data sent in the callbacks
 * @param Returns the new library struct
 */
Enlil_Library *
enlil_library_new(
  Enlil_Album_New_Cb               monitor_album_new_cb,
  Enlil_Album_Delete_Cb            monitor_album_delete_cb,
  Enlil_Delete_Cb                  monitor_enlil_delete_cb,
  Enlil_Photo_New_Cb               monitor_photo_new_cb,
  Enlil_Photo_Delete_Cb            monitor_photo_delete_cb,
  Enlil_Photo_Update_Cb            monitor_photo_update_cb,
  Enlil_Collection_New_Cb          col_new_cb,
  Enlil_Collection_Delete_Cb       col_delete_cb,
  Enlil_Collection_Album_New_Cb    col_album_new_cb,
  Enlil_Collection_Album_Delete_Cb col_album_delete_cb,
  Enlil_Tag_New_Cb                 tag_new_cb,
  Enlil_Tag_Delete_Cb              tag_delete_cb,
  Enlil_Tag_Photo_New_Cb           tag_photo_new_cb,
  Enlil_Tag_Photo_Delete_Cb        tag_photo_delete_cb,
  void                            *user_data)
{
   Enlil_Library *library = calloc(1, sizeof(Enlil_Library));
   library->conf.monitor.album_new_cb = monitor_album_new_cb;
   library->conf.monitor.album_delete_cb = monitor_album_delete_cb;
   library->conf.monitor.enlil_delete_cb = monitor_enlil_delete_cb;
   library->conf.monitor.photo_new_cb = monitor_photo_new_cb;
   library->conf.monitor.photo_delete_cb = monitor_photo_delete_cb;
   library->conf.monitor.photo_update_cb = monitor_photo_update_cb;

   library->conf.collection.new_cb = col_new_cb;
   library->conf.collection.delete_cb = col_delete_cb;
   library->conf.collection.album_new_cb = col_album_new_cb;
   library->conf.collection.album_delete_cb = col_album_delete_cb;

   library->conf.tag.new_cb = tag_new_cb;
   library->conf.tag.delete_cb = tag_delete_cb;
   library->conf.tag.photo_new_cb = tag_photo_new_cb;
   library->conf.tag.photo_delete_cb = tag_photo_delete_cb;

   library->conf.data = user_data;

   return library;
}

/**
 * @brief Free a library struct
 * @param library the library struct
 */
void
enlil_library_free(Enlil_Library **library)
{
   Enlil_Album *album;
   Enlil_Collection *col;
   Enlil_Tag *tag;
   Eina_List *l, *l_next;

   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID((*library) != NULL);

   EINA_STRINGSHARE_DEL((*library)->path);
   EINA_STRINGSHARE_DEL((*library)->photo1);
   EINA_STRINGSHARE_DEL((*library)->photo2);
   EINA_LIST_FOREACH_SAFE((*library)->albums, l, l_next, album)
     enlil_album_free(&album);

   enlil_library_monitor_stop(*library);

   EINA_LIST_FREE((*library)->collections, col)
     enlil_collection_free(&col);

   EINA_LIST_FREE((*library)->tags, tag)
     enlil_tag_free(&tag);

   FREE(*library);
}

/**
 * Start to monitor the library folder
 *
 * @param library the library struct
 */
void
enlil_library_monitor_start(Enlil_Library *library)
{
   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(library->monitor == NULL);

   library->monitor = ecore_file_monitor_add(enlil_library_path_get(library), _enlil_library_monitor_cb, library);
   ASSERT_RETURN_VOID(library->monitor != NULL);
}

/**
 * Stop to monitor the library folder
 *
 * @param library the library struct
 */
void
enlil_library_monitor_stop(Enlil_Library *library)
{
   ASSERT_RETURN_VOID(library != NULL);

   if(library->monitor)
     ecore_file_monitor_del(library->monitor);
   library->monitor = NULL;
}

#define FCT_NAME    enlil_library
#define STRUCT_TYPE Enlil_Library

SET(sync, Enlil_Sync *)

GET(path, const char *)
GET(albums, Eina_List *)
GET(sync, Enlil_Sync *)
GET(collections, const Eina_List *)
GET(tags, const Eina_List *)

#undef FCT_NAME
#undef STRUCT_TYPE

/**
 * Set the path
 *
 * @param library the library struct
 * @param path the new path
 */
void
enlil_library_path_set(Enlil_Library *library,
                       const char    *path)
{
   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(path != NULL);

   EINA_STRINGSHARE_DEL(library->path);

   char *_path = strdup(path);
   int len = strlen(_path) - 1;
   if(_path[len] == '/')
     _path[len] = '\0';

   library->path = eina_stringshare_add(_path);
   FREE(_path);
}

void
enlil_library_album_version_header_increase_cb_set(Enlil_Library                         *library,
                                                   Enlil_Album_Version_Header_Increase_Cb cb,
                                                   void                                  *data)
{
   ASSERT_RETURN_VOID(library != NULL);

   library->album_version_header_increase_cb = cb;
   library->album_version_header_increase_data = data;
}

void
enlil_library_photo_version_header_increase_cb_set(Enlil_Library                         *library,
                                                   Enlil_Photo_Version_Header_Increase_Cb cb,
                                                   void                                  *data)
{
   ASSERT_RETURN_VOID(library != NULL);

   library->photo_version_header_increase_cb = cb;
   library->photo_version_header_increase_data = data;
}

void
enlil_library_photo_version_tags_increase_cb_set(Enlil_Library                       *library,
                                                 Enlil_Photo_Version_Tags_Increase_Cb cb,
                                                 void                                *data)
{
   ASSERT_RETURN_VOID(library != NULL);

   library->photo_version_tags_increase_cb = cb;
   library->photo_version_tags_increase_data = data;
}

Enlil_Album_Version_Header_Increase_Cb
_enlil_library_album_version_header_increase_cb_get(Enlil_Library *library)
{
   return library->album_version_header_increase_cb;
}

void *
_enlil_library_album_version_header_increase_data_get(Enlil_Library *library)
{
   return library->album_version_header_increase_data;
}

Enlil_Photo_Version_Header_Increase_Cb
_enlil_library_photo_version_header_increase_cb_get(Enlil_Library *library)
{
   return library->photo_version_header_increase_cb;
}

void *
_enlil_library_photo_version_header_increase_data_get(Enlil_Library *library)
{
   return library->photo_version_header_increase_data;
}

Enlil_Photo_Version_Tags_Increase_Cb
_enlil_library_photo_version_tags_increase_cb_get(Enlil_Library *library)
{
   return library->photo_version_tags_increase_cb;
}

void *
_enlil_library_photo_version_tags_increase_data_get(Enlil_Library *library)
{
   return library->photo_version_tags_increase_data;
}

/**
 * Set the default photo of the album
 *
 * @param library the library struct
 * @param photo the photo
 */
void
enlil_library_photo_set(Enlil_Library     *library,
                        const Enlil_Photo *photo,
                        int                first_second)
{
   char buf[PATH_MAX];

   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(photo != NULL);

   ROOT_HEADER_LOAD(library);

   snprintf(buf, sizeof(buf), "%s/%s", enlil_album_file_name_get(enlil_photo_album_get(photo)), enlil_photo_file_name_get(photo));

   if(first_second == 1)
     {
        EINA_STRINGSHARE_DEL(library->photo1);
        library->photo1 = eina_stringshare_add(buf);
     }
   else
     {
        EINA_STRINGSHARE_DEL(library->photo2);
        library->photo2 = eina_stringshare_add(buf);
     }

   _library_eet_header_save(library);
}

void
enlil_library_netsync_account_set(Enlil_Library *library,
                                  const char    *username)
{
   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(username != NULL);

   ROOT_HEADER_LOAD(library);

   EINA_STRINGSHARE_DEL(library->netsync.username);

   library->netsync.username = eina_stringshare_add(username);
   _library_eet_header_save(library);
}

const char *
enlil_library_netsync_account_get(Enlil_Library *library)
{
   ASSERT_RETURN(library != NULL);

   ROOT_HEADER_LOAD(library);

   return library->netsync.username;
}

void
enlil_library_netsync_host_set(Enlil_Library *library,
                               const char    *host)
{
   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(host != NULL);

   ROOT_HEADER_LOAD(library);

   EINA_STRINGSHARE_DEL(library->netsync.host);

   library->netsync.host = eina_stringshare_add(host);
   _library_eet_header_save(library);
}

const char *
enlil_library_netsync_host_get(Enlil_Library *library)
{
   ASSERT_RETURN(library != NULL);

   ROOT_HEADER_LOAD(library);

   return library->netsync.host;
}

void
enlil_library_netsync_path_set(Enlil_Library *library,
                               const char    *path)
{
   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(path != NULL);

   ROOT_HEADER_LOAD(library);

   EINA_STRINGSHARE_DEL(library->netsync.path);

   library->netsync.path = eina_stringshare_add(path);
   _library_eet_header_save(library);
}

const char *
enlil_library_netsync_path_get(Enlil_Library *library)
{
   ASSERT_RETURN(library != NULL);

   ROOT_HEADER_LOAD(library);

   return library->netsync.path;
}

void
enlil_library_netsync_password_set(Enlil_Library *library,
                                   const char    *password)
{
   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(password != NULL);

   ROOT_HEADER_LOAD(library);

   EINA_STRINGSHARE_DEL(library->netsync.password);

   library->netsync.password = eina_stringshare_add(password);
   _library_eet_header_save(library);
}

const char *
enlil_library_netsync_password_get(Enlil_Library *library)
{
   ASSERT_RETURN(library != NULL);

   ROOT_HEADER_LOAD(library);

   return library->netsync.password;
}

/**
 * @brief
 * @param file_name The album file name. The string has to be in eina_stringshare
 */
Enlil_Album *
enlil_library_album_search_file_name(Enlil_Library *library,
                                     const char    *file_name)
{
   Eina_List *l;
   Enlil_Album *album;
   ASSERT_RETURN(library != NULL);
   ASSERT_RETURN(file_name != NULL);

   EINA_LIST_FOREACH(library->albums, l, album)
     {
        if(enlil_album_file_name_get(album) == file_name)
          return album;
     }

   return NULL;
}

/**
 * @brief
 * @param name The album name. The string has to be in eina_stringshare
 */
Enlil_Album *
enlil_library_album_search_name(Enlil_Library *library,
                                const char    *name)
{
   Eina_List *l;
   Enlil_Album *album;
   ASSERT_RETURN(library != NULL);
   ASSERT_RETURN(name != NULL);

   EINA_LIST_FOREACH(library->albums, l, album)
     {
        if(enlil_album_name_get(album) == name)
          return album;
     }

   return NULL;
}

/*
 * Add an album
 *
 * @param library the library struct
 * @param album the album to add
 */
void
enlil_library_album_add(Enlil_Library *library,
                        Enlil_Album   *album)
{
   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(album != NULL);

   enlil_album_library_set(album, library);
   library->albums = eina_list_sorted_insert(library->albums, _sort_albums_name_cb, album);
}

void
_enlil_library_album_add_end(Enlil_Library *library,
                             Enlil_Album   *album)
{
   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(album != NULL);

   enlil_album_library_set(album, library);
   library->albums = eina_list_append(library->albums, album);
}

void
_enlil_library_collection_add_end(Enlil_Library    *library,
                                  Enlil_Collection *collection,
                                  Eina_Bool         notify)
{
   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(collection != NULL);

   library->collections = eina_list_append(library->collections, collection);

   if (notify)
     if(library->conf.collection.new_cb)
       library->conf.collection.new_cb(library->conf.data, library, collection);
}

void
_enlil_library_tag_add_end(Enlil_Library *library,
                           Enlil_Tag     *tag,
                           Eina_Bool      notify)
{
   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(tag != NULL);

   library->tags = eina_list_append(library->tags, tag);

   if (notify)
     if(library->conf.tag.new_cb)
       library->conf.tag.new_cb(library->conf.data, library, tag);
}

Enlil_Album *
enlil_library_album_prev_get(Enlil_Library *library,
                             Enlil_Album   *album)
{
   ASSERT_RETURN(library != NULL);
   ASSERT_RETURN(album != NULL);

   Eina_List *l = eina_list_data_find_list(enlil_library_albums_get(library), album);
   return eina_list_data_get(eina_list_prev(l));
}

/*
 * Sort the list of albums by name
 */
void
enlil_library_albums_sort(Enlil_Library *library)
{
   ASSERT_RETURN_VOID(library != NULL);

   library->albums = eina_list_sort(library->albums, eina_list_count(library->albums), _sort_albums_name_cb);
}

/*
 * Remove an album
 *
 * @param library the library struct
 */
void
enlil_library_album_remove(Enlil_Library *library,
                           Enlil_Album   *album)
{
   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(album != NULL);

   enlil_album_library_set(album, NULL);
   library->albums = eina_list_remove(library->albums, album);
}

void
_enlil_library_album_name_changed(Enlil_Library *library,
                                  Enlil_Album   *album)
{
   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(album != NULL);

   library->albums = eina_list_remove(library->albums, album);
   enlil_library_album_add(library, album);
   enlil_library_eet_albums_save(library);
}

/*
 * Sort the list of collections by name
 */
void
enlil_library_collections_sort(Enlil_Library *library)
{
   ASSERT_RETURN_VOID(library != NULL);

   library->collections = eina_list_sort(library->collections, eina_list_count(library->collections), _sort_collections_name_cb);
}

/*
 * Sort the list of tags by name
 */
void
enlil_library_tags_sort(Enlil_Library *library)
{
   ASSERT_RETURN_VOID(library != NULL);

   library->tags = eina_list_sort(library->tags, eina_list_count(library->tags), _sort_tags_name_cb);
}

/*
 * Print the list of album in stdout
 *
 * @param library the library struct
 */
void
enlil_library_print(Enlil_Library *library)
{
   Eina_List *l;
   Enlil_Album *album;

   ASSERT_RETURN_VOID(library != NULL);

   printf("#######################################\n");
   printf("####### Path : %s\n", library->path);
   printf("#######################################\n");
   EINA_LIST_FOREACH(library->albums, l, album)
     enlil_album_print(album);
}

/**
 * Add the album in the collection named @p album_col. If the collection does not exists, it is created.
 *
 * @param library The library struct
 * @param album_col The album collection struct.
 * @param album The album
 */
void
_enlil_library_collection_album_add(Enlil_Library          *library,
                                    Enlil_Album_Collection *album_col,
                                    Enlil_Album            *album)
{
   Eina_List *l;
   Enlil_Collection *col;

   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(album_col != NULL);
   ASSERT_RETURN_VOID(album != NULL);

   EINA_LIST_FOREACH(library->collections, l, col)
     {
        if(album_col->name == enlil_collection_name_get(col))
          {
             enlil_collection_album_add(col, album);
             album_col->collection = col;
             if(library->conf.collection.album_new_cb)
               library->conf.collection.album_new_cb(library->conf.data, library, col, album);
             return;
          }
     }

   col = enlil_collection_new();
   enlil_collection_name_set(col, album_col->name);
   enlil_collection_album_add(col, album);
   library->collections = eina_list_append(library->collections, col);
   album_col->collection = col;

   if(library->conf.collection.new_cb)
     library->conf.collection.new_cb(library->conf.data, library, col);
   if(library->conf.collection.album_new_cb)
     library->conf.collection.album_new_cb(library->conf.data, library, col, album);
}

static int
_sort_albums_name_cb(const void *d1,
                     const void *d2)
{
   const Enlil_Album *album1 = d1;
   const Enlil_Album *album2 = d2;
   const char *name1;
   const char *name2;

   name1 = enlil_album_name_get(album1);
   if(!name1) return 1;
   name2 = enlil_album_name_get(album2);
   if(!name2) return -1;

   return strcmp(name1, name2);
}

static int
_sort_collections_name_cb(const void *d1,
                          const void *d2)
{
   const Enlil_Collection *collection1 = d1;
   const Enlil_Collection *collection2 = d2;
   const char *name1;
   const char *name2;

   name1 = enlil_collection_name_get(collection1);
   if(!name1) return 1;
   name2 = enlil_collection_name_get(collection2);
   if(!name2) return -1;

   return strcmp(name1, name2);
}

static int
_sort_tags_name_cb(const void *d1,
                   const void *d2)
{
   const Enlil_Tag *tag1 = d1;
   const Enlil_Tag *tag2 = d2;
   const char *name1;
   const char *name2;

   name1 = enlil_tag_name_get(tag1);
   if(!name1) return 1;
   name2 = enlil_tag_name_get(tag2);
   if(!name2) return -1;

   return strcmp(name1, name2);
}

/**
 * Remove the album from the collection named @p album_col.
 *
 * @param library The library struct
 * @param album_col The album collection struct.
 * @param album The album
 */
void
_enlil_library_collection_album_remove(Enlil_Library          *library,
                                       Enlil_Album_Collection *album_col,
                                       Enlil_Album            *album)
{
   Eina_List *l;
   Enlil_Collection *col;

   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(album_col != NULL);
   ASSERT_RETURN_VOID(album != NULL);

   EINA_LIST_FOREACH(library->collections, l, col)
     {
        if(album_col->name == enlil_collection_name_get(col))
          {
             enlil_collection_album_remove(col, album);
             album_col->collection = NULL;
             if(library->conf.collection.album_delete_cb)
               library->conf.collection.album_delete_cb(library->conf.data, library, col, album);
             return;
          }
     }
}

/**
 * Delete the collection. The album inside the collection will be removed one by one before the collection is deleted. library->conf.collection.album_delete_cb() will be called for each album before library->conf.collection.delete_cb() is called.
 *
 * @param library The library struct
 * @param album_col The album collection struct.
 * @param album The album
 */
void
enlil_library_collection_del(Enlil_Library    *library,
                             Enlil_Collection *col)
{
   Eina_List *l, *l_next;
   const Eina_List *l2;
   Enlil_Album *album;
   Enlil_Album_Collection *album_col;

   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(col != NULL);

   EINA_LIST_FOREACH_SAFE(enlil_collection_albums_get(col), l, l_next, album)
     {
        EINA_LIST_FOREACH(enlil_album_collections_get(album), l2, album_col)
          {
             if(album_col->name == enlil_collection_name_get(col))
               break;
          }
        enlil_album_collection_remove(album, album_col);
     }

   if(library->conf.collection.delete_cb)
     library->conf.collection.delete_cb(library->conf.data, library, col);

   library->collections = eina_list_remove(library->collections, col);
   enlil_collection_free(&col);
}

/**
 * @brief
 * @param name The collection  name. The string has to be in eina_stringshare
 */
Enlil_Collection *
enlil_library_collection_search_name(Enlil_Library *library,
                                     const char    *name)
{
   Eina_List *l;
   Enlil_Collection *collection;
   ASSERT_RETURN(library != NULL);
   ASSERT_RETURN(name != NULL);

   EINA_LIST_FOREACH(library->collections, l, collection)
     {
        if(enlil_collection_name_get(collection) == name)
          return collection;
     }

   return NULL;
}

/**
 * Add the photo in the tag named @p photo_tag. If the tag does not exists, it is created.
 *
 * @param library The library struct
 * @param photo_tag The photo tag struct.
 * @param photo The photo
 */
void
_enlil_library_tag_photo_add(Enlil_Library   *library,
                             Enlil_Photo_Tag *photo_tag,
                             Enlil_Photo     *photo)
{
   Eina_List *l;
   Enlil_Tag *tag;

   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(photo_tag != NULL);
   ASSERT_RETURN_VOID(photo != NULL);

   EINA_LIST_FOREACH(library->tags, l, tag)
     {
        if(photo_tag->name == enlil_tag_name_get(tag))
          {
             enlil_tag_photo_add(tag, photo);
             photo_tag->tag = tag;
             if(library->conf.tag.photo_new_cb)
               library->conf.tag.photo_new_cb(library->conf.data, library, tag, photo);
             return;
          }
     }

   tag = enlil_tag_new();
   enlil_tag_name_set(tag, photo_tag->name);
   enlil_tag_photo_add(tag, photo);
   library->tags = eina_list_append(library->tags, tag);
   photo_tag->tag = tag;

   if(library->conf.tag.new_cb)
     library->conf.tag.new_cb(library->conf.data, library, tag);
   if(library->conf.tag.photo_new_cb)
     library->conf.tag.photo_new_cb(library->conf.data, library, tag, photo);
}

/**
 * Remove the photo from the tag named @p photo_tag.
 *
 * @param library The library struct
 * @param photo_tag The photo tag struct.
 * @param photo The photo
 */
void
_enlil_library_tag_photo_remove(Enlil_Library   *library,
                                Enlil_Photo_Tag *photo_tag,
                                Enlil_Photo     *photo)
{
   Eina_List *l;
   Enlil_Tag *tag;

   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(photo_tag != NULL);
   ASSERT_RETURN_VOID(photo != NULL);

   EINA_LIST_FOREACH(library->tags, l, tag)
     {
        if(photo_tag->name == enlil_tag_name_get(tag))
          {
             enlil_tag_photo_remove(tag, photo);
             photo_tag->tag = NULL;
             if(library->conf.tag.photo_delete_cb)
               library->conf.tag.photo_delete_cb(library->conf.data, library, tag, photo);
             return;
          }
     }
}

/**
 * Delete the tag. The photo inside the tag will be removed one by one before the tag is deleted. library->conf.tag.photo_delete_cb() will be called for each photo before library->conf.tag.delete_cb() is called.
 *
 * @param library The library struct
 * @param photo_tag The photo tag struct.
 * @param photo The photo
 */
void
enlil_library_tag_del(Enlil_Library *library,
                      Enlil_Tag     *tag)
{
   Eina_List *l, *l_next;
   const Eina_List *l2;
   Enlil_Photo *photo;
   Enlil_Photo_Tag *photo_tag;

   ASSERT_RETURN_VOID(library != NULL);
   ASSERT_RETURN_VOID(tag != NULL);

   EINA_LIST_FOREACH_SAFE(enlil_tag_photos_get(tag), l, l_next, photo)
     {
        EINA_LIST_FOREACH(enlil_photo_tags_get(photo), l2, photo_tag)
          {
             if(photo_tag->name == enlil_tag_name_get(tag))
               break;
          }
        enlil_photo_tag_remove(photo, photo_tag);
     }

   if(library->conf.tag.delete_cb)
     library->conf.tag.delete_cb(library->conf.data, library, tag);

   library->tags = eina_list_remove(library->tags, tag);
   enlil_tag_free(&tag);
}

/**
 * @brief
 * @param name The tag name. The string has to be in eina_stringshare
 */
Enlil_Tag *
enlil_library_tag_search_name(Enlil_Library *library,
                              const char    *name)
{
   Eina_List *l;
   Enlil_Tag *tag;
   ASSERT_RETURN(library != NULL);
   ASSERT_RETURN(name != NULL);

   EINA_LIST_FOREACH(library->tags, l, tag)
     {
        if(enlil_tag_name_get(tag) == name)
          return tag;
     }

   return NULL;
}

/**
 * Save the path in the file ~/EET_FOLDER_ROOT_DB/EET_FILE_ROOT_DB
 *
 * @param library The library struct
 */
int
enlil_library_eet_path_save(Enlil_Library *library)
{
   int res;
   Eet_Data_Descriptor *edd;
   Eet_File *f;
   char path[PATH_MAX];
   char key[PATH_MAX];
   Enlil_String string;

   ASSERT_RETURN(library != NULL);
   ASSERT_RETURN(enlil_library_path_get(library) != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FOLDER_ROOT_DB, getenv("HOME"));
   if(!ecore_file_exists(path))
     ecore_file_mkdir(path);

   snprintf(path, PATH_MAX, "%s/" EET_FOLDER_ROOT_DB "/" EET_FILE_ROOT_DB, getenv("HOME"));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   edd = enlil_string_edd_new();

   snprintf(key, PATH_MAX, "/library %s", enlil_library_path_get(library));
   string.string = enlil_library_path_get(library);

   res = eet_data_write(f, edd, key, &string, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd);

   return res;
}

/**
 * Delete the library path
 *
 * @param library The library struct
 */
int
enlil_library_eet_path_delete(Enlil_Library *library)
{
   int res;
   Eet_File *f;
   char path[PATH_MAX];
   char key[PATH_MAX];

   ASSERT_RETURN(library != NULL);
   ASSERT_RETURN(enlil_library_path_get(library) != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FOLDER_ROOT_DB, getenv("HOME"));
   if(!ecore_file_exists(path))
     ecore_file_mkdir(path);

   snprintf(path, PATH_MAX, "%s/" EET_FOLDER_ROOT_DB "/" EET_FILE_ROOT_DB, getenv("HOME"));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   snprintf(key, PATH_MAX, "/library %s", enlil_library_path_get(library));
   res = eet_delete(f, key);

   enlil_file_manager_close(path);

   return res;
}

/**
 * Return the list of libraries saved in ~/EET_FOLDER_ROOT_DB/EET_FILE_ROOT_DB
 *
 * @return Returns a list of String* containing the library path
 */
Eina_List *
enlil_library_eet_path_load(void)
{
   Eet_Data_Descriptor *edd;
   Eet_File *f;
   Eina_List *l = NULL;
   Enlil_String *data;
   char path[PATH_MAX];
   char **list;
   int len, i;

   snprintf(path, PATH_MAX, "%s/" EET_FOLDER_ROOT_DB "/" EET_FILE_ROOT_DB, getenv("HOME"));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   edd = enlil_string_edd_new();

   list = eet_list(f, "/library *", &len);

   for(i = 0; list && i < len; i++)
     {
        data = eet_data_read(f, edd, list[i]);
        l = eina_list_append(l, data);
     }
   FREE(list);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd);

   return l;
}

/*
 * Load the list of albums from the Eet file.
 * This list contains only the path and the file name of each album.
 * You should use it to load the albums with enlil_library_eet_album_load()
 *
 * @param library the library struct
 * @return Returns a new library with the list of albums
 */
Enlil_Library *
enlil_library_eet_albums_load(Enlil_Library *library)
{
   Eet_Data_Descriptor *edd, *edd_album;
   Eet_File *f;
   Enlil_Library *data;
   char path[PATH_MAX];

   ASSERT_RETURN(library != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FILE, enlil_library_path_get(library));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   edd_album = _enlil_album_file_name_edd_new();
   edd = _enlil_library_albums_file_name_edd_new(edd_album);

   data = eet_data_read(f, edd, "/album_list");

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_album);
   eet_data_descriptor_free(edd);

   return data;
}

/*
 * Save the list of albums
 *
 * @param library the library struct
 */
int
enlil_library_eet_albums_save(Enlil_Library *library)
{
   int res;
   Eet_Data_Descriptor *edd, *edd_album;
   Eet_File *f;
   char path[PATH_MAX];

   ASSERT_RETURN(library != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FILE, enlil_library_path_get(library));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   edd_album = _enlil_album_file_name_edd_new();
   edd = _enlil_library_albums_file_name_edd_new(edd_album);

   res = eet_data_write(f, edd, "/album_list", library, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_album);
   eet_data_descriptor_free(edd);

   return res;
}

/*
 * Load an album from the eet file
 *
 * @param library the library struct
 * @param name the file name of the album to load
 * @return Returns the album or NULL if the album is not in the Eet file
 */
Enlil_Album *
enlil_library_eet_album_load(Enlil_Library *library,
                             const char    *name)
{
   Eet_Data_Descriptor *edd, *edd_collection;
   Eet_File *f;
   Enlil_Album *data;
   char path[PATH_MAX];

   ASSERT_RETURN(library != NULL);
   ASSERT_RETURN(name != NULL);

   snprintf(path, PATH_MAX, "%s/%s/" EET_FILE, enlil_library_path_get(library), name);

   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   edd_collection = _enlil_album_collection_edd_new();
   edd = _enlil_album_header_edd_new(edd_collection);

   data = eet_data_read(f, edd, "header");

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_collection);
   eet_data_descriptor_free(edd);

   if(data && strcmp(name, enlil_album_file_name_get(data)) != 0)
     enlil_album_free(&data);

   return data;
}

/*
 * Load the list of collections from the Eet file.
 * This list contains only the name and the description of each collection.
 * You should use it to load the collections with enlil_library_eet_collections_load()
 *
 * @param library the library struct
 * @return Returns a new library with the list of collections
 */
Enlil_Library *
enlil_library_eet_collections_load(Enlil_Library *library)
{
   Eet_Data_Descriptor *edd, *edd_collection;
   Eet_File *f;
   Enlil_Library *data;
   char path[PATH_MAX];

   ASSERT_RETURN(library != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FILE, enlil_library_path_get(library));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   edd_collection = _enlil_collection_edd_new();
   edd = _enlil_library_collections_edd_new(edd_collection);

   data = eet_data_read(f, edd, "/collection_list");

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_collection);
   eet_data_descriptor_free(edd);

   return data;
}

/*
 * Save the list of collections
 *
 * @param library the library struct
 */
int
enlil_library_eet_collections_save(Enlil_Library *library)
{
   int res;
   Eet_Data_Descriptor *edd, *edd_collection;
   Eet_File *f;
   char path[PATH_MAX];

   ASSERT_RETURN(library != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FILE, enlil_library_path_get(library));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   edd_collection = _enlil_collection_edd_new();
   edd = _enlil_library_collections_edd_new(edd_collection);

   res = eet_data_write(f, edd, "/collection_list", library, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_collection);
   eet_data_descriptor_free(edd);

   return res;
}

/*
 * Load the list of tags from the Eet file.
 * This list contains only the name and the description of each tag.
 * You should use it to load the tags with enlil_library_eet_tags_load()
 *
 * @param library the library struct
 * @return Returns a new library with the list of tags
 */
Enlil_Library *
enlil_library_eet_tags_load(Enlil_Library *library)
{
   Eet_Data_Descriptor *edd, *edd_tag;
   Eet_File *f;
   Enlil_Library *data;
   char path[PATH_MAX];

   ASSERT_RETURN(library != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FILE, enlil_library_path_get(library));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   edd_tag = _enlil_tag_edd_new();
   edd = _enlil_library_tags_edd_new(edd_tag);

   data = eet_data_read(f, edd, "/tag_list");

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_tag);
   eet_data_descriptor_free(edd);

   return data;
}

/*
 * Save the list of tags
 *
 * @param library the library struct
 */
int
enlil_library_eet_tags_save(Enlil_Library *library)
{
   int res;
   Eet_Data_Descriptor *edd, *edd_tag;
   Eet_File *f;
   char path[PATH_MAX];

   ASSERT_RETURN(library != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FILE, enlil_library_path_get(library));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   edd_tag = _enlil_tag_edd_new();
   edd = _enlil_library_tags_edd_new(edd_tag);

   res = eet_data_write(f, edd, "/tag_list", library, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_tag);
   eet_data_descriptor_free(edd);

   return res;
}

/*
 * Save the header of a library
 *
 * @param library the library struct
 */
static int
_library_eet_header_save(const Enlil_Library *library)
{
   int res;
   Eet_Data_Descriptor *edd;
   Eet_File *f;
   char path[PATH_MAX];

   ASSERT_RETURN(library != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FILE, enlil_library_path_get(library));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   edd = _enlil_library_header_edd_new();

   res = eet_data_write(f, edd, "/header", library, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd);

   return res;
}

/*
 * Load the header of a library
 *
 * @param library the library struct
 */
static void
_library_eet_header_load(Enlil_Library *library)
{
   Eet_Data_Descriptor *edd;
   Eet_File *f;
   Enlil_Library *data;
   char path[PATH_MAX];

   ASSERT_RETURN_VOID(library != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FILE, enlil_library_path_get(library));

   f = enlil_file_manager_open(path);
   ASSERT_RETURN_VOID(f != NULL);

   edd = _enlil_library_header_edd_new();

   data = eet_data_read(f, edd, "/header");

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd);

   if(data)
     {
        library->netsync.host = eina_stringshare_add(data->netsync.host);
        library->netsync.path = eina_stringshare_add(data->netsync.path);
        library->netsync.username = eina_stringshare_add(data->netsync.username);
        library->netsync.password = eina_stringshare_add(data->netsync.password);
        library->photo1 = eina_stringshare_add(data->photo1);
        library->photo2 = eina_stringshare_add(data->photo2);
     }

   enlil_library_free(&data);
}

/*
 * Remove an album from the Eet file
 *
 * @param library the library struct
 * @param key the file name of the album to delete
 */
int
enlil_library_eet_album_remove(Enlil_Library *library,
                               const char    *key)
{
   Eet_File *f;
   char buf[PATH_MAX], path[PATH_MAX];

   ASSERT_RETURN(library != NULL);
   ASSERT_RETURN(key != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FILE, enlil_library_path_get(library));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   if(key[0] != '/')
     snprintf(buf, PATH_MAX, "/album %s", key);
   else
     snprintf(buf, PATH_MAX, "%s", key);

   snprintf(buf, PATH_MAX, "/album %s", buf);

   eet_delete(f, buf);

   enlil_file_manager_close(path);

   return 1;
}

static void
_enlil_library_monitor_cb(void                          *data,
                          __UNUSED__ Ecore_File_Monitor *em,
                          Ecore_File_Event               event,
                          const char                    *path)
{
   Enlil_Library *library = (Enlil_Library *)data;
   switch(event)
     {
      case ECORE_FILE_EVENT_CREATED_DIRECTORY:
        LOG_INFO("New album: %s", path);
        library->conf.monitor.album_new_cb(library->conf.data, library, path);
        break;

      case ECORE_FILE_EVENT_DELETED_DIRECTORY:
        LOG_INFO("Deleted album; %s", path);
        library->conf.monitor.album_delete_cb(library->conf.data, library, path);
        break;

      case ECORE_FILE_EVENT_DELETED_SELF:
        LOG_INFO("Deleted enlil library: %s", path);
        library->conf.monitor.enlil_delete_cb(library->conf.data, library);
        break;

      default:;
        break;
     }
}

static Eet_Data_Descriptor *
_enlil_library_albums_file_name_edd_new(Eet_Data_Descriptor *edd_file_name)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Library);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Library, "albums", albums, edd_file_name);

   return edd;
}

static Eet_Data_Descriptor *
_enlil_library_collections_edd_new(Eet_Data_Descriptor *collection_edd)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Library);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Library, "collections", collections, collection_edd);

   return edd;
}

static Eet_Data_Descriptor *
_enlil_library_tags_edd_new(Eet_Data_Descriptor *tag_edd)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Library);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Library, "tags", tags, tag_edd);

   return edd;
}

static Eet_Data_Descriptor *
_enlil_library_header_edd_new(void)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Library);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Library, "netsync.username", netsync.username, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Library, "netsync.password", netsync.password, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Library, "netsync.host", netsync.host, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Library, "netsync.path", netsync.path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Library, "photo1", photo1, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Library, "photo2", photo2, EET_T_STRING);

   return edd;
}

Enlil_Configuration
enlil_conf_get(Enlil_Library *library)
{
   return library->conf;
}

Enlil_Photo *
enlil_library_photo_get(const char *library_path,
                        int         first_second)
{
   Enlil_Photo *photo = NULL;
   char buf[PATH_MAX];
   const char *photo_file;

   ASSERT_RETURN(library_path != NULL);
   ASSERT_RETURN(first_second > 0);
   ASSERT_RETURN(first_second < 3);

   Enlil_Library *library = enlil_library_new(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   enlil_library_path_set(library, library_path);
   ROOT_HEADER_LOAD(library);

   if(first_second == 1)
     photo_file = library->photo1;
   else
     photo_file = library->photo2;

   if(photo_file)
     {
        snprintf(buf, sizeof(buf), "%s/%s", library_path, photo_file);

        if(ecore_file_exists(buf))
          {
             photo = enlil_photo_new();
             enlil_photo_path_set(photo, library_path);
             enlil_photo_file_name_set(photo, photo_file);
          }
     }

   if(!photo)
     photo = enlil_library_search_photo_get(library_path, first_second);

   enlil_library_free(&(library));
   return photo;
}

Enlil_Photo *
enlil_library_search_photo_get(const char *library_path,
                               int         first_second)
{
   char buf[PATH_MAX];
   Eina_Bool found = EINA_TRUE, found2 = EINA_TRUE;
   Eina_Bool first_found = EINA_FALSE;
   Enlil_Photo *photo = NULL;
   char *folder, *file;

   ASSERT_RETURN(library_path != NULL);
   ASSERT_RETURN(first_second > 0);
   ASSERT_RETURN(first_second < 3);

   Eina_List *folders = ecore_file_ls(library_path);

   EINA_LIST_FREE(folders, folder)
     {
        if(found)
          {
             snprintf(buf, PATH_MAX, "%s/%s", library_path, folder);
             Eina_List *files = ecore_file_ls(buf);

             EINA_LIST_FREE(files, file)
               {
                  if(found2)
                    {
                       if( enlil_photo_is(file))
                         {
                            if(first_found || first_second == 1)
                              {
                                 photo = enlil_photo_new();
                                 enlil_photo_path_set(photo, buf);
                                 enlil_photo_file_name_set(photo, file);
                                 found2 = EINA_FALSE;
                                 found = EINA_FALSE;
                              }
                            first_found = EINA_TRUE;
                         }
                    }
                  FREE(file);
               }
          }
        FREE(folder);
     }

   return photo;
}

