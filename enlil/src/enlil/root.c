#include "enlil_private.h"

/*
 * The file name, the path and the name are saved into root/data.eet. Consequently if the name of the album changed do not forget to update the list of album (enlil_root_eet_albums_save()).
 */

static void _enlil_root_monitor_cb(void *data, Ecore_File_Monitor *em, Ecore_File_Event event, const char *path);
static Eet_Data_Descriptor *_enlil_root_albums_file_name_edd_new(Eet_Data_Descriptor *edd_file_name);
static Eet_Data_Descriptor * _enlil_root_collections_edd_new(Eet_Data_Descriptor *collection_edd);
static Eet_Data_Descriptor * _enlil_root_tags_edd_new(Eet_Data_Descriptor *tag_edd);
static Eet_Data_Descriptor * _enlil_root_header_edd_new();
static int _sort_albums_name_cb(const void *d1, const void *d2);
static int _sort_collections_name_cb(const void *d1, const void *d2);
static int _sort_tags_name_cb(const void *d1, const void *d2);

static int _root_eet_header_save(const Enlil_Root *root);
static void _root_eet_header_load(Enlil_Root *root);

struct enlil_root
{
   const char *path;
   Eina_List *albums;

   //generally  a sync is associated to a photo manager
   //that's why we allow the user to set a sync here
   Enlil_Sync *sync;

   Ecore_File_Monitor *monitor;
   Enlil_Configuration conf;

   //list of Enlil_Collection *
   Eina_List *collections;
   //list of Enlil_Tag *
   Eina_List *tags;

   //associated Flickr account.
   const char *flickr_account;
   const char *flickr_auth_token;

   Eina_Bool header_load_is : 1;
};

#define ROOT_HEADER_LOAD(root) \
    if(!root->header_load_is) \
    { \
        _root_eet_header_load(root); \
        root->header_load_is = 1; \
    }


/*
 * Create a new root node. The callbacks arguments named monitor_* can be NULL if the folder
 * is not monitored. The callbacks named col_* can be null if you do not want to be notified when a change occurs in the collections.
 *
 * @param album_new_cb  Callback called by the monitoring when a new album appears
 * @param album_delete_cb  Callback called by the monitoring when a album is deleted
 * @param enlil_delete_cb  Callback called by the monitoring when the root folder is deleted
 * @param photo_new_cb  Callback called by the monitoring when a new photo appears
 * @param photo_delete_cb  Callback called by the monitoring when a photo is deleted
 * @param photo_update_cb  Callback called by the monitoring when a photo is updated
 * @param user_data Data sent in the callbacks
 * @param Returns the new root struct
 */
Enlil_Root *enlil_root_new(
      Enlil_Album_New_Cb monitor_album_new_cb, Enlil_Album_Delete_Cb monitor_album_delete_cb,
      Enlil_Delete_Cb monitor_enlil_delete_cb,
      Enlil_Photo_New_Cb monitor_photo_new_cb, Enlil_Photo_Delete_Cb monitor_photo_delete_cb,
      Enlil_Photo_Update_Cb monitor_photo_update_cb,
      Enlil_Collection_New_Cb col_new_cb, Enlil_Collection_Delete_Cb col_delete_cb,
      Enlil_Collection_Album_New_Cb col_album_new_cb,
      Enlil_Collection_Album_Delete_Cb col_album_delete_cb,
      Enlil_Tag_New_Cb tag_new_cb, Enlil_Tag_Delete_Cb tag_delete_cb,
      Enlil_Tag_Photo_New_Cb tag_photo_new_cb,
      Enlil_Tag_Photo_Delete_Cb tag_photo_delete_cb,
      void *user_data)
{
   Enlil_Root *root = calloc(1,sizeof(Enlil_Root));
   root->conf.monitor.album_new_cb = monitor_album_new_cb;
   root->conf.monitor.album_delete_cb = monitor_album_delete_cb;
   root->conf.monitor.enlil_delete_cb = monitor_enlil_delete_cb;
   root->conf.monitor.photo_new_cb = monitor_photo_new_cb;
   root->conf.monitor.photo_delete_cb = monitor_photo_delete_cb;
   root->conf.monitor.photo_update_cb = monitor_photo_update_cb;

   root->conf.collection.new_cb = col_new_cb;
   root->conf.collection.delete_cb = col_delete_cb;
   root->conf.collection.album_new_cb = col_album_new_cb;
   root->conf.collection.album_delete_cb = col_album_delete_cb;

   root->conf.tag.new_cb = tag_new_cb;
   root->conf.tag.delete_cb = tag_delete_cb;
   root->conf.tag.photo_new_cb = tag_photo_new_cb;
   root->conf.tag.photo_delete_cb = tag_photo_delete_cb;

   root->conf.data = user_data;

   return root;
}

/**
 * @brief Free a root struct
 * @param root the root struct
 */
void enlil_root_free(Enlil_Root **root)
{
   Enlil_Album *album;
   Enlil_Collection *col;
   Enlil_Tag *tag;
   Eina_List *l, *l_next;

   ASSERT_RETURN_VOID(root!=NULL);
   ASSERT_RETURN_VOID((*root)!=NULL);

   EINA_STRINGSHARE_DEL((*root)->path);
   EINA_LIST_FOREACH_SAFE( (*root)->albums, l, l_next, album)
      enlil_album_free(&album);

   enlil_root_monitor_stop(*root);

   EINA_LIST_FREE( (*root)->collections, col)
      enlil_collection_free(&col);

   EINA_LIST_FREE( (*root)->tags, tag)
      enlil_tag_free(&tag);

   FREE(*root);
}

/**
 * Start to monitor the root folder
 *
 * @param root the root struct
 */
void enlil_root_monitor_start(Enlil_Root *root)
{
   ASSERT_RETURN_VOID(root!=NULL);
   ASSERT_RETURN_VOID(root->monitor==NULL);

   root->monitor = ecore_file_monitor_add(enlil_root_path_get(root), _enlil_root_monitor_cb, root);
   ASSERT_RETURN_VOID(root->monitor!=NULL);
}

/**
 * Stop to monitor the root folder
 *
 * @param root the root struct
 */
void enlil_root_monitor_stop(Enlil_Root *root)
{
   ASSERT_RETURN_VOID(root!=NULL);

   if(root->monitor)
     ecore_file_monitor_del(root->monitor);
}

#define FCT_NAME enlil_root
#define STRUCT_TYPE Enlil_Root

SET(sync, Enlil_Sync *)

   GET(path, const char*)
   GET(albums, Eina_List *)
   GET(sync, Enlil_Sync *)
   GET(collections, const Eina_List *)
GET(tags, const Eina_List *)


#undef FCT_NAME
#undef STRUCT_TYPE

   /**
    * Set the path
    *
    * @param root the root struct
    * @param path the new path
    */
void enlil_root_path_set(Enlil_Root *root, const char *path)
{
   ASSERT_RETURN_VOID(root!=NULL);
   ASSERT_RETURN_VOID(path!=NULL);

   EINA_STRINGSHARE_DEL(root->path);

   char *_path = strdup(path);
   int len = strlen(_path) -1;
   if(_path[len] == '/')
     _path[len] = '\0';

   root->path = eina_stringshare_add(_path);
   FREE(_path);
}

void enlil_root_flickr_account_set(Enlil_Root *root, const char *account)
{
   ASSERT_RETURN_VOID(root!=NULL);
   ASSERT_RETURN_VOID(account!=NULL);

   if(account == root->flickr_account) return ;

   EINA_STRINGSHARE_DEL(root->flickr_account);

   root->flickr_account = eina_stringshare_add(account);
   _root_eet_header_save(root);
}


const char *enlil_root_flickr_account_get(const Enlil_Root *_root)
{
   Enlil_Root *root = (Enlil_Root*) _root;
   ASSERT_RETURN(root != NULL);

   ROOT_HEADER_LOAD(root);

   return root->flickr_account;
}

void enlil_root_flickr_auth_token_set(Enlil_Root *root, const char *auth_token)
{
   ASSERT_RETURN_VOID(root!=NULL);

   if(auth_token == root->flickr_auth_token) return ;

   EINA_STRINGSHARE_DEL(root->flickr_auth_token);

   if(auth_token)
     root->flickr_auth_token = eina_stringshare_add(auth_token);
   else
     root->flickr_auth_token = NULL;
   _root_eet_header_save(root);
}


const char *enlil_root_flickr_auth_token_get(const Enlil_Root *_root)
{
   Enlil_Root *root = (Enlil_Root*) _root;
   ASSERT_RETURN(root != NULL);

   ROOT_HEADER_LOAD(root);

   return root->flickr_auth_token;
}

/**
 * @brief
 * @param file_name The album file name. The string has to be in eina_stringshare
 */
Enlil_Album *enlil_root_album_search_file_name(Enlil_Root *root, const char *file_name)
{
   Eina_List *l;
   Enlil_Album *album;
   ASSERT_RETURN(root!=NULL);
   ASSERT_RETURN(file_name!=NULL);

   EINA_LIST_FOREACH(root->albums, l, album)
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
Enlil_Album *enlil_root_album_search_name(Enlil_Root *root, const char *name)
{
   Eina_List *l;
   Enlil_Album *album;
   ASSERT_RETURN(root!=NULL);
   ASSERT_RETURN(name!=NULL);

   EINA_LIST_FOREACH(root->albums, l, album)
     {
	if(enlil_album_name_get(album) == name)
	  return album;
     }

   return NULL;
}

/**
 * @brief
 * @param id The flickr id. The string has to be in eina_stringshare
 */
Enlil_Album *enlil_root_album_search_flickr_id(Enlil_Root *root, const char *id)
{
   Eina_List *l;
   Enlil_Album *album;
   ASSERT_RETURN(root!=NULL);
   ASSERT_RETURN(id!=NULL);

   EINA_LIST_FOREACH(root->albums, l, album)
     {
	if(enlil_album_flickr_id_get(album) == id)
	  return album;
     }

   return NULL;
}

/*
 * Add an album
 *
 * @param root the root struct
 * @param album the album to add
 */
void enlil_root_album_add(Enlil_Root *root, Enlil_Album *album)
{
   ASSERT_RETURN_VOID(root!=NULL);
   ASSERT_RETURN_VOID(album!=NULL);

   enlil_album_root_set(album, root);
   root->albums = eina_list_sorted_insert(root->albums, _sort_albums_name_cb, album);
}

void _enlil_root_album_add_end(Enlil_Root *root, Enlil_Album *album)
{
   ASSERT_RETURN_VOID(root!=NULL);
   ASSERT_RETURN_VOID(album!=NULL);

   enlil_album_root_set(album, root);
   root->albums = eina_list_append(root->albums, album);
}

void _enlil_root_collection_add_end(Enlil_Root *root, Enlil_Collection *collection, Eina_Bool notify)
{
   ASSERT_RETURN_VOID(root != NULL);
   ASSERT_RETURN_VOID(collection != NULL);

   root->collections = eina_list_append(root->collections, collection);

   if (notify)
      if(root->conf.collection.new_cb)
	 root->conf.collection.new_cb(root->conf.data, root, collection);
}

void _enlil_root_tag_add_end(Enlil_Root *root, Enlil_Tag *tag, Eina_Bool notify)
{
   ASSERT_RETURN_VOID(root != NULL);
   ASSERT_RETURN_VOID(tag != NULL);

   root->tags = eina_list_append(root->tags, tag);

   if (notify)
      if(root->conf.tag.new_cb)
	 root->conf.tag.new_cb(root->conf.data, root, tag);
}

Enlil_Album *enlil_root_album_prev_get(Enlil_Root *root, Enlil_Album *album)
{
   ASSERT_RETURN(root != NULL);
   ASSERT_RETURN(album != NULL);

   Eina_List *l = eina_list_data_find_list(root->albums, album);
   return eina_list_data_get(eina_list_prev(l));
}

/*
 * Sort the list of albums by name
 */
void enlil_root_albums_sort(Enlil_Root *root)
{
   ASSERT_RETURN_VOID(root != NULL);

   root->albums = eina_list_sort(root->albums, eina_list_count(root->albums), _sort_albums_name_cb);
}

/*
 * Remove an album
 *
 * @param root the root struct
 */
void enlil_root_album_remove(Enlil_Root *root, Enlil_Album *album)
{
   ASSERT_RETURN_VOID(root!=NULL);
   ASSERT_RETURN_VOID(album!=NULL);

   enlil_album_root_set(album, NULL);
   root->albums = eina_list_remove(root->albums, album);
}

void _enlil_root_album_name_changed(Enlil_Root *root, Enlil_Album *album)
{
   ASSERT_RETURN_VOID(root!=NULL);
   ASSERT_RETURN_VOID(album!=NULL);

   root->albums = eina_list_remove(root->albums, album);
   enlil_root_album_add(root, album);
   enlil_root_eet_albums_save(root);
}

/*
 * Sort the list of collections by name
 */
void enlil_root_collections_sort(Enlil_Root *root)
{
   ASSERT_RETURN_VOID(root != NULL);

   root->collections = eina_list_sort(root->collections, eina_list_count(root->collections), _sort_collections_name_cb);
}

/*
 * Sort the list of tags by name
 */
void enlil_root_tags_sort(Enlil_Root *root)
{
   ASSERT_RETURN_VOID(root != NULL);

   root->tags = eina_list_sort(root->tags, eina_list_count(root->tags), _sort_tags_name_cb);
}

/*
 * Print the list of album in stdout
 *
 * @param root the root struct
 */
void enlil_root_print(Enlil_Root *root)
{
   Eina_List *l;
   Enlil_Album *album;

   ASSERT_RETURN_VOID(root!=NULL);

   printf("#######################################\n");
   printf("####### Path : %s\n",root->path);
   printf("#######################################\n");
   EINA_LIST_FOREACH(root->albums,l,album)
      enlil_album_print(album);
}


/**
 * Add the album in the collection named @p album_col. If the collection does not exists, it is created.
 *
 * @param root The root struct
 * @param album_col The album collection struct.
 * @param album The album
 */
void _enlil_root_collection_album_add(Enlil_Root *root, Enlil_Album_Collection *album_col, Enlil_Album *album)
{
   Eina_List *l;
   Enlil_Collection *col;

   ASSERT_RETURN_VOID(root != NULL);
   ASSERT_RETURN_VOID(album_col != NULL);
   ASSERT_RETURN_VOID(album != NULL);

   EINA_LIST_FOREACH(root->collections, l, col)
     {
	if(album_col->name == enlil_collection_name_get(col))
	  {
	     enlil_collection_album_add(col, album);
	     album_col->collection = col;
	     if(root->conf.collection.album_new_cb)
	       root->conf.collection.album_new_cb(root->conf.data, root, col, album);
	     return ;
	  }
     }

   col = enlil_collection_new();
   enlil_collection_name_set(col, album_col->name);
   enlil_collection_album_add(col, album);
   root->collections = eina_list_append(root->collections, col);
   album_col->collection = col;

   if(root->conf.collection.new_cb)
     root->conf.collection.new_cb(root->conf.data, root, col);
   if(root->conf.collection.album_new_cb)
     root->conf.collection.album_new_cb(root->conf.data, root, col, album);
}

static int _sort_albums_name_cb(const void *d1, const void *d2)
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

static int _sort_collections_name_cb(const void *d1, const void *d2)
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

static int _sort_tags_name_cb(const void *d1, const void *d2)
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
 * @param root The root struct
 * @param album_col The album collection struct.
 * @param album The album
 */
void _enlil_root_collection_album_remove(Enlil_Root *root, Enlil_Album_Collection *album_col, Enlil_Album *album)
{
   Eina_List *l;
   Enlil_Collection *col;

   ASSERT_RETURN_VOID(root != NULL);
   ASSERT_RETURN_VOID(album_col != NULL);
   ASSERT_RETURN_VOID(album != NULL);

   EINA_LIST_FOREACH(root->collections, l, col)
     {
	if(album_col->name == enlil_collection_name_get(col))
	  {
	     enlil_collection_album_remove(col, album);
	     album_col->collection = NULL;
	     if(root->conf.collection.album_delete_cb)
	       root->conf.collection.album_delete_cb(root->conf.data, root, col, album);
	     return ;
	  }
     }
}

/**
 * Delete the collection. The album inside the collection will be removed one by one before the collection is deleted. root->conf.collection.album_delete_cb() will be called for each album before root->conf.collection.delete_cb() is called.
 *
 * @param root The root struct
 * @param album_col The album collection struct.
 * @param album The album
 */
void enlil_root_collection_del(Enlil_Root *root, Enlil_Collection *col)
{
   Eina_List *l, *l_next;
   const Eina_List *l2;
   Enlil_Album *album;
   Enlil_Album_Collection *album_col;

   ASSERT_RETURN_VOID(root != NULL);
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

   if(root->conf.collection.delete_cb)
     root->conf.collection.delete_cb(root->conf.data, root, col);

   root->collections = eina_list_remove(root->collections, col);
   enlil_collection_free(&col);
}

/**
 * @brief
 * @param name The collection  name. The string has to be in eina_stringshare
 */
Enlil_Collection *enlil_root_collection_search_name(Enlil_Root *root, const char *name)
{
   Eina_List *l;
   Enlil_Collection *collection;
   ASSERT_RETURN(root!=NULL);
   ASSERT_RETURN(name!=NULL);

   EINA_LIST_FOREACH(root->collections, l, collection)
     {
	if(enlil_collection_name_get(collection) == name)
	  return collection;
     }

   return NULL;
}

/**
 * Add the photo in the tag named @p photo_tag. If the tag does not exists, it is created.
 *
 * @param root The root struct
 * @param photo_tag The photo tag struct.
 * @param photo The photo
 */
void _enlil_root_tag_photo_add(Enlil_Root *root, Enlil_Photo_Tag *photo_tag, Enlil_Photo *photo)
{
   Eina_List *l;
   Enlil_Tag *tag;

   ASSERT_RETURN_VOID(root != NULL);
   ASSERT_RETURN_VOID(photo_tag != NULL);
   ASSERT_RETURN_VOID(photo != NULL);

   EINA_LIST_FOREACH(root->tags, l, tag)
     {
	if(photo_tag->name == enlil_tag_name_get(tag))
	  {
	     enlil_tag_photo_add(tag, photo);
	     photo_tag->tag = tag;
	     if(root->conf.tag.photo_new_cb)
	       root->conf.tag.photo_new_cb(root->conf.data, root, tag, photo);
	     return ;
	  }
     }

   tag = enlil_tag_new();
   enlil_tag_name_set(tag, photo_tag->name);
   enlil_tag_photo_add(tag, photo);
   root->tags = eina_list_append(root->tags, tag);
   photo_tag->tag = tag;

   if(root->conf.tag.new_cb)
     root->conf.tag.new_cb(root->conf.data, root, tag);
   if(root->conf.tag.photo_new_cb)
     root->conf.tag.photo_new_cb(root->conf.data, root, tag, photo);
}

/**
 * Remove the photo from the tag named @p photo_tag.
 *
 * @param root The root struct
 * @param photo_tag The photo tag struct.
 * @param photo The photo
 */
void _enlil_root_tag_photo_remove(Enlil_Root *root, Enlil_Photo_Tag *photo_tag, Enlil_Photo *photo)
{
   Eina_List *l;
   Enlil_Tag *tag;

   ASSERT_RETURN_VOID(root != NULL);
   ASSERT_RETURN_VOID(photo_tag != NULL);
   ASSERT_RETURN_VOID(photo != NULL);

   EINA_LIST_FOREACH(root->tags, l, tag)
     {
	if(photo_tag->name == enlil_tag_name_get(tag))
	  {
	     enlil_tag_photo_remove(tag, photo);
	     photo_tag->tag = NULL;
	     if(root->conf.tag.photo_delete_cb)
	       root->conf.tag.photo_delete_cb(root->conf.data, root, tag, photo);
	     return ;
	  }
     }
}

/**
 * Delete the tag. The photo inside the tag will be removed one by one before the tag is deleted. root->conf.tag.photo_delete_cb() will be called for each photo before root->conf.tag.delete_cb() is called.
 *
 * @param root The root struct
 * @param photo_tag The photo tag struct.
 * @param photo The photo
 */
void enlil_root_tag_del(Enlil_Root *root, Enlil_Tag *tag)
{
   Eina_List *l, *l_next;
   const Eina_List *l2;
   Enlil_Photo *photo;
   Enlil_Photo_Tag *photo_tag;

   ASSERT_RETURN_VOID(root != NULL);
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

   if(root->conf.tag.delete_cb)
     root->conf.tag.delete_cb(root->conf.data, root, tag);

   root->tags = eina_list_remove(root->tags, tag);
   enlil_tag_free(&tag);
}

/**
 * @brief
 * @param name The tag name. The string has to be in eina_stringshare
 */
Enlil_Tag *enlil_root_tag_search_name(Enlil_Root *root, const char *name)
{
   Eina_List *l;
   Enlil_Tag *tag;
   ASSERT_RETURN(root!=NULL);
   ASSERT_RETURN(name!=NULL);

   EINA_LIST_FOREACH(root->tags, l, tag)
     {
	if(enlil_tag_name_get(tag) == name)
	  return tag;
     }

   return NULL;
}

/**
 * Save the path in the file ~/EET_FOLDER_ROOT_DB/EET_FILE_ROOT_DB
 *
 * @param root The root struct
 */
int enlil_root_eet_path_save(Enlil_Root *root)
{
   int res;
   Eet_Data_Descriptor *edd;
   Eet_File *f;
   char path[PATH_MAX];
   char key[PATH_MAX];
   Enlil_String string;

   ASSERT_RETURN(root!=NULL);
   ASSERT_RETURN(enlil_root_path_get(root)!=NULL);


   snprintf(path,PATH_MAX,"%s/"EET_FOLDER_ROOT_DB, getenv("HOME"));
   if(!ecore_file_exists(path))
     ecore_file_mkdir(path);

   snprintf(path,PATH_MAX,"%s/"EET_FOLDER_ROOT_DB"/"EET_FILE_ROOT_DB, getenv("HOME"));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   edd = enlil_string_edd_new();

   snprintf(key,PATH_MAX,"/root %s", enlil_root_path_get(root));
   string.string = enlil_root_path_get(root);

   res = eet_data_write(f, edd, key, &string, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd);

   return res;
}

/**
 * Return the list of albums saved in ~/EET_FOLDER_ROOT_DB/EET_FILE_ROOT_DB
 *
 * @return Returns a list of String* containing the root path
 */
Eina_List *enlil_root_eet_path_load()
{
   Eet_Data_Descriptor *edd;
   Eet_File *f;
   Eina_List *l = NULL;
   Enlil_String *data;
   char path[PATH_MAX];
   char **list;
   int len, i;

   snprintf(path,PATH_MAX,"%s/"EET_FOLDER_ROOT_DB"/"EET_FILE_ROOT_DB, getenv("HOME"));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   edd = enlil_string_edd_new();

   list = eet_list(f, "/root *", &len);

   for(i=0; list && i<len; i++)
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
 * You should use it to load the albums with enlil_root_eet_album_load()
 *
 * @param root the root struct
 * @return Returns a new root with the list of albums
 */
Enlil_Root *enlil_root_eet_albums_load(Enlil_Root *root)
{
   Eet_Data_Descriptor *edd, *edd_album;
   Eet_File *f;
   Enlil_Root *data;
   char path[PATH_MAX];

   ASSERT_RETURN(root!=NULL);

   snprintf(path,PATH_MAX,"%s/"EET_FILE,enlil_root_path_get(root));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   edd_album = _enlil_album_file_name_edd_new();
   edd = _enlil_root_albums_file_name_edd_new(edd_album);

   data = eet_data_read(f, edd, "/album_list");

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_album);
   eet_data_descriptor_free(edd);

   return data;
}

/*
 * Save the list of albums
 *
 * @param root the root struct
 */
int enlil_root_eet_albums_save(Enlil_Root *root)
{
   int res;
   Eet_Data_Descriptor *edd, *edd_album;
   Eet_File *f;
   char path[PATH_MAX];

   ASSERT_RETURN(root!=NULL);

   snprintf(path,PATH_MAX,"%s/"EET_FILE,enlil_root_path_get(root));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   edd_album = _enlil_album_file_name_edd_new();
   edd = _enlil_root_albums_file_name_edd_new(edd_album);

   res = eet_data_write(f, edd, "/album_list", root, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_album);
   eet_data_descriptor_free(edd);

   return res;
}

/*
 * Load an album from the eet file
 *
 * @param root the root struct
 * @param name the file name of the album to load
 * @return Returns the album or NULL if the album is not in the Eet file
 */
Enlil_Album *enlil_root_eet_album_load(Enlil_Root *root, const char* name)
{
   Eet_Data_Descriptor *edd, *edd_collection;
   Eet_File *f;
   Enlil_Album *data;
   char path[PATH_MAX];

   ASSERT_RETURN(root!=NULL);
   ASSERT_RETURN(name!=NULL);

   snprintf(path,PATH_MAX,"%s/%s/"EET_FILE,enlil_root_path_get(root),name);

   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   edd_collection = _enlil_album_collection_edd_new();
   edd = _enlil_album_header_edd_new(edd_collection);

   data = eet_data_read(f, edd, "header");

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_collection);
   eet_data_descriptor_free(edd);

   if(data && strcmp(name, enlil_album_file_name_get(data))!=0)
     enlil_album_free(&data);

   return data;
}


/*
 * Load the list of collections from the Eet file.
 * This list contains only the name and the description of each collection.
 * You should use it to load the collections with enlil_root_eet_collections_load()
 *
 * @param root the root struct
 * @return Returns a new root with the list of collections
 */
Enlil_Root *enlil_root_eet_collections_load(Enlil_Root *root)
{
   Eet_Data_Descriptor *edd, *edd_collection;
   Eet_File *f;
   Enlil_Root *data;
   char path[PATH_MAX];

   ASSERT_RETURN(root!=NULL);

   snprintf(path,PATH_MAX,"%s/"EET_FILE,enlil_root_path_get(root));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   edd_collection = _enlil_collection_edd_new();
   edd = _enlil_root_collections_edd_new(edd_collection);

   data = eet_data_read(f, edd, "/collection_list");

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_collection);
   eet_data_descriptor_free(edd);

   return data;
}

/*
 * Save the list of collections
 *
 * @param root the root struct
 */
int enlil_root_eet_collections_save(Enlil_Root *root)
{
   int res;
   Eet_Data_Descriptor *edd, *edd_collection;
   Eet_File *f;
   char path[PATH_MAX];

   ASSERT_RETURN(root!=NULL);

   snprintf(path,PATH_MAX,"%s/"EET_FILE,enlil_root_path_get(root));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   edd_collection = _enlil_collection_edd_new();
   edd = _enlil_root_collections_edd_new(edd_collection);

   res = eet_data_write(f, edd, "/collection_list", root, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_collection);
   eet_data_descriptor_free(edd);

   return res;
}








/*
 * Load the list of tags from the Eet file.
 * This list contains only the name and the description of each tag.
 * You should use it to load the tags with enlil_root_eet_tags_load()
 *
 * @param root the root struct
 * @return Returns a new root with the list of tags
 */
Enlil_Root *enlil_root_eet_tags_load(Enlil_Root *root)
{
   Eet_Data_Descriptor *edd, *edd_tag;
   Eet_File *f;
   Enlil_Root *data;
   char path[PATH_MAX];

   ASSERT_RETURN(root!=NULL);

   printf("%s/\n", enlil_root_path_get(root));
   snprintf(path,PATH_MAX,"%s/"EET_FILE,enlil_root_path_get(root));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   edd_tag = _enlil_tag_edd_new();
   edd = _enlil_root_tags_edd_new(edd_tag);

   data = eet_data_read(f, edd, "/tag_list");

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_tag);
   eet_data_descriptor_free(edd);

   return data;
}

/*
 * Save the list of tags
 *
 * @param root the root struct
 */
int enlil_root_eet_tags_save(Enlil_Root *root)
{
   int res;
   Eet_Data_Descriptor *edd, *edd_tag;
   Eet_File *f;
   char path[PATH_MAX];

   ASSERT_RETURN(root!=NULL);

   snprintf(path,PATH_MAX,"%s/"EET_FILE,enlil_root_path_get(root));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   edd_tag = _enlil_tag_edd_new();
   edd = _enlil_root_tags_edd_new(edd_tag);

   res = eet_data_write(f, edd, "/tag_list", root, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_tag);
   eet_data_descriptor_free(edd);

   return res;
}



















/*
 * Save the header of a library
 *
 * @param root the root struct
 */
static int _root_eet_header_save(const Enlil_Root *root)
{
   int res;
   Eet_Data_Descriptor *edd;
   Eet_File *f;
   char path[PATH_MAX];

   ASSERT_RETURN(root!=NULL);

   snprintf(path,PATH_MAX,"%s/"EET_FILE,enlil_root_path_get(root));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   edd = _enlil_root_header_edd_new();

   res = eet_data_write(f, edd, "/header", root, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd);

   return res;
}

/*
 * Load the header of a library
 *
 * @param root the root struct
 */
static void _root_eet_header_load(Enlil_Root *root)
{
   Eet_Data_Descriptor *edd;
   Eet_File *f;
   Enlil_Root *data;
   char path[PATH_MAX];

   ASSERT_RETURN_VOID(root!=NULL);

   snprintf(path,PATH_MAX,"%s/"EET_FILE,enlil_root_path_get(root));

   f = enlil_file_manager_open(path);
   ASSERT_RETURN_VOID(f!=NULL);

   edd = _enlil_root_header_edd_new();

   data = eet_data_read(f, edd, "/header");

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd);

   if(data)
     {
	enlil_root_flickr_account_set(root, data->flickr_account);
	enlil_root_flickr_auth_token_set(root, data->flickr_auth_token);
     }

   enlil_root_free(&data);
}

/*
 * Remove an album from the Eet file
 *
 * @param root the root struct
 * @param key the file name of the album to delete
 */
int enlil_root_eet_album_remove(Enlil_Root *root, const char* key)
{
   Eet_File *f;
   char buf[PATH_MAX], path[PATH_MAX];

   ASSERT_RETURN(root!=NULL);
   ASSERT_RETURN(key!=NULL);


   snprintf(path, PATH_MAX, "%s/"EET_FILE, enlil_root_path_get(root));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   if(key[0]!='/')
     snprintf(buf,PATH_MAX,"/album %s",key);
   else
     snprintf(buf,PATH_MAX,"%s",key);

   snprintf(buf, PATH_MAX, "/album %s", buf);

   eet_delete(f, buf);

   enlil_file_manager_close(path);

   return 1;
}

static void _enlil_root_monitor_cb(void *data, Ecore_File_Monitor *em, Ecore_File_Event event, const char *path)
{
   Enlil_Root *root = (Enlil_Root *)data;
   switch(event)
     {
      case ECORE_FILE_EVENT_CREATED_DIRECTORY:
	 LOG_INFO("New album: %s", path);
	 root->conf.monitor.album_new_cb(root->conf.data, root, path);
	 break;
      case ECORE_FILE_EVENT_DELETED_DIRECTORY:
	 LOG_INFO("Deleted album; %s", path);
	 root->conf.monitor.album_delete_cb(root->conf.data, root, path);
	 break;
      case ECORE_FILE_EVENT_DELETED_SELF:
	 LOG_INFO("Deleted enlil root: %s", path);
	 root->conf.monitor.enlil_delete_cb(root->conf.data, root);
	 break;
      default: ;
	       break;
     }
}

static Eet_Data_Descriptor * _enlil_root_albums_file_name_edd_new(Eet_Data_Descriptor *edd_file_name)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Root);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Root, "albums", albums, edd_file_name);

   return edd;
}

static Eet_Data_Descriptor * _enlil_root_collections_edd_new(Eet_Data_Descriptor *collection_edd)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Root);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Root, "collections", collections, collection_edd);

   return edd;
}

static Eet_Data_Descriptor * _enlil_root_tags_edd_new(Eet_Data_Descriptor *tag_edd)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Root);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Root, "tags", tags, tag_edd);

   return edd;
}

static Eet_Data_Descriptor * _enlil_root_header_edd_new()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Root);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Root, "flickr_account", flickr_account, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Root, "flickr_auth_token", flickr_auth_token, EET_T_STRING);

   return edd;
}




Enlil_Configuration enlil_conf_get(Enlil_Root *root)
{
   return root->conf;
}


