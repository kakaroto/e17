#ifndef _ENLIL_H_
# define _ENLIL_H_

#include <Eina.h>

EAPI extern int ENLIL_ALBUM_ADD;
EAPI extern int ENLIL_ALBUM_DEL;
EAPI extern int ENLIL_ALBUMS_LOADED;
EAPI extern int ENLIL_COLLECTION_ADD;
EAPI extern int ENLIL_COLLECTION_DEL;
EAPI extern int ENLIL_TAG_ADD;
EAPI extern int ENLIL_TAG_DEL;
EAPI extern int ENLIL_TAG_SET;
EAPI extern int ENLIL_TAG_UNSET;
EAPI extern int ENLIL_PHOTO_ADD;
EAPI extern int ENLIL_PHOTO_DEL;
EAPI extern int ENLIL_LIBRARY_LOADED;

typedef struct _Enlil_Tag        Enlil_Tag;
typedef struct _Enlil_Photo      Enlil_Photo;
typedef struct _Enlil_Album      Enlil_Album;
typedef struct _Enlil_Collection Enlil_Collection;
typedef struct _Enlil_Library    Enlil_Library;
typedef struct _Enlil_Thumb      Enlil_Thumb;

typedef void (*Enlil_Free_Cb)(void *data, void *user_data);

/* Ecore_Event ENLIL_ALBUM_ADD & ENLIL_ALBUM_DEL */
typedef struct _Enlil_Album_Op Enlil_Album_Add;
typedef struct _Enlil_Album_Op Enlil_Album_Del;

struct _Enlil_Album_Op
{
   Enlil_Library *library;
   Enlil_Album *album;

   const char *name;
   const char *path;
   const char *description;
};

/* Ecore_Event ENLIL_COLLECTION_ADD & ENLIL_COLLECTION_DEL */
typedef struct _Enlil_Collection_Op Enlil_Collection_Add;
typedef struct _Enlil_Collection_Op Enlil_Collection_Del;

struct _Enlil_Collection_Op
{
   Enlil_Library *library;
   Enlil_Collection *collection;

   const char *name;
   const char *description;
};

/* Ecore_Event ENLIL_TAG_ADD & ENLIL_TAG_DEL */
typedef struct _Enlil_Tag_Op Enlil_Tag_Add;
typedef struct _Enlil_Tag_Op Enlil_Tag_Del;

struct _Enlil_Tag_Op
{
   Enlil_Library *library;
   Enlil_Tag *tag;

   const char *name;
   const char *description;
};

/* Ecore_Event ENLIL_TAG_SET & ENLIL_TAG_UNSET */
typedef struct _Enlil_Tag_Photo_Op Enlil_Tag_Set;
typedef struct _Enlil_Tag_Photo_Op Enlil_Tag_Unset;

struct _Enlil_Tag_Photo_Op
{
   Enlil_Library *library;
   Enlil_Photo *photo;
   Enlil_Tag *tag;
};

/* Ecore_Event ENLIL_LIBRARY_LOADED */
typedef struct _Enlil_Library_Op Enlil_Library_Loaded;

struct _Enlil_Library_Op
{
   Enlil_Library *library;
};

/* Ecore_Event ENLIL_PHOTO_ADD & ENLIL_PHOTO_DEL */
typedef struct _Enlil_Photo_Op Enlil_Photo_Add;
typedef struct _Enlil_Photo_Op Enlil_Photo_Del;

struct _Enlil_Photo_Op
{
   Enlil_Library *library;
   Enlil_Album *album;
   Enlil_Photo *photo;

   const char *path;
};

EAPI Eina_Bool enlil_init(void);
EAPI Eina_Bool enlil_shutdown(void);

EAPI Eina_Iterator *enlil_libraries_get(void);

EAPI Enlil_Library *enlil_library_add(const char *path, const char *name);
EAPI void enlil_library_del(Enlil_Library *library);

EAPI Eina_Bool enlil_library_load(Enlil_Library *library);
EAPI void enlil_library_unload(Enlil_Library *library);

EAPI const char *enlil_library_path_get(Enlil_Library *library);

EAPI Eina_Bool enlil_library_netsync_set(Enlil_Library *library, const char *account, const char *password, const char *host, const char *path);
EAPI const char *enlil_library_netsync_account_get(Enlil_Library *library);
EAPI const char *enlil_library_netsync_password_get(Enlil_Library *library);
EAPI const char *enlil_library_netsync_host_get(Enlil_Library *library);
EAPI const char *enlil_library_netsync_path_get(Enlil_Library *library);

EAPI Eina_Iterator *enlil_albums_get(Enlil_Library *library);
EAPI Eina_Iterator *enlil_tags_get(Enlil_Library *library);
EAPI Eina_Iterator *enlil_collections_get(Enlil_Library *library);

EAPI Eina_Iterator *enlil_photos_get(Enlil_Album *album);

#endif /* _ENLIL_H_ */
