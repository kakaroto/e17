#ifndef _ENLIL_PRIVATE_H
# define _ENLIL_PRIVATE_H

#include <Eina.h>
#include <Eet.h>

#define ENLIL_FILE_VERSION_MAJOR 1
#define ENLIL_FILE_VERSION_MINOR 0

typedef struct _Enlil_Free Enlil_Free;
struct _Enlil_Free
{
   void         *data;
   Enlil_Free_Cb cb;
};

typedef struct _Enlil_Loaded_Library Enlil_Loaded_Library;
typedef struct _Enlil_Libraries Enlil_Libraries;
typedef struct _Enlil_Exif Enlil_Exif;
typedef struct _Enlil_IPTC Enlil_IPTC;

struct _Enlil_Tag
{
   EINA_REFCOUNT;
   const char   *name;
   const char   *description;

   Eina_List    *photos; /* List of path ! */

   /* Not saved data */
   Enlil_Free    free;
};

struct _Enlil_Collection
{
   EINA_REFCOUNT;
   const char *name;
   const char *description;

   Eina_List  *album;

   /* Not saved data */
   Enlil_Free  free;
};

struct _Enlil_Photo
{
   EINA_REFCOUNT;

   struct
   {
      const char *path;

      long long last_modified;
      long long size;
      unsigned long long inode;
   } filesystem;

   struct {
      const char *name;
      const char *description;
      const char *author;
      int w, h;
      /* Enlil_Photo_Type type; */
   } description;

   struct
   {
      int id;
      int version_header;
      int version_file;
      int version_tags;
      int version_header_net;
      int version_file_net;
      int version_tags_net;
   } netsync;

   struct
   {
      const char *DateTimeOriginal;
      double longitude;
      double latitude;

      Eina_Hash *data;
   } exif;

   struct
   {
      Eina_Hash *data;
   } iptc;

   Eina_List *tags; /* list of tag (const char*) */

   /* Not saved data */
   Enlil_Thumb *thumb; /* really needed, can't use elm_thumb ? */

   Enlil_Free free;

   Eina_Bool found : 1;
};

struct _Enlil_Exif
{
   const char *tag;
   const char *value;
   const char *description;

   /* Not saved data */
   Enlil_Free free;
};

struct _Enlil_IPTC
{
   double record;
   double tag;

   const char *name;
   const char *title;
   const char *format;
   const char *value;

   /* Not saved data */
   Enlil_Free free;
};

struct _Enlil_Album
{
   EINA_REFCOUNT;

   const char *path;
   const char *name;
   const char *description;

   struct
   {
      int id;
      int version_header;
      int version_header_net;
      int timestamp_last_update_collections;
   } netsync;

   struct
   {
      long long last_modified;
      long long size;
      unsigned long long inode;
   } filesystem;

   Eina_List *photos; /* List of path */
   Eina_List *collections;

   /* Enlil_Photo_Sort order; */
   /* Enlil_Access_Type access; */

   /* Not saved data, usefull during sync */
   Enlil_Library *library;

   Enlil_Free free;
};

struct _Enlil_Loaded_Library
{
   struct
   {
      int major, minor;
   } version;

   struct
   {
      Eina_List *albums; /* list of paths */
      Eina_List *tags; /* list of tag */
      Eina_List *collections; /* list of collection */
   } paths;

   struct
   {
      Eina_Hash *albums; /* Hash of Enlil_Album */
      Eina_Hash *collections; /* Hash of Enlil_Collection */
      Eina_Hash *tags; /* Hash of Enlil_Tag */
      Eina_Hash *photos; /* Hash of Enlil_Photo */
   } data;

   /* Not saved data, usefull during sync */
   Eet_File *source;

   Enlil_Free free;

   Eina_Bool save_needed : 1;
};

struct _Enlil_Library
{
   EINA_REFCOUNT;

   const char *name;
   const char *path;

   Eina_Bool read_only;

   /* Not saved data, usefull during sync */
   Enlil_Free free;

   Enlil_Loaded_Library *loaded;
   Eina_List *process; /* list of ecore_thread loading this library */
   Eina_List *io_process; /* list of pending io request */
};

struct _Enlil_Libraries
{
   struct
   {
      int major, minor;
   } version;

   Eina_List *libraries;
};

extern Eet_Data_Descriptor *enlil_edd_libraries;
extern Eet_Data_Descriptor *enlil_edd_loaded_library;

extern const char *_enlil_library_config;

Eina_Bool enlil_edd_init(void);
Eina_Bool enlil_edd_shutdown(void);

Enlil_Libraries *_enlil_libraries_new(void);
void _enlil_libraries_free(Enlil_Libraries *free);

Enlil_Library *_enlil_library_new(const char *path, const char *name);
Eina_Bool _enlil_library_load(Enlil_Library *lib);
void _enlil_library_close(Enlil_Library *lib);
void _enlil_library_free(Enlil_Library *lib);
void _enlil_library_save(Enlil_Library *lib);


#endif /* _ENLIL_PRIVATE_H */
