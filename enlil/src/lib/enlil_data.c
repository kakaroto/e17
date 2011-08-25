#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Eet.h>

#include "Enlil.h"
#include "enlil_private.h"

static Eet_Data_Descriptor *_enlil_edd_library = NULL;
Eet_Data_Descriptor *enlil_edd_libraries = NULL;

static Eet_Data_Descriptor *_enlil_edd_exif = NULL;
static Eet_Data_Descriptor *_enlil_edd_iptc = NULL;
static Eet_Data_Descriptor *_enlil_edd_collection = NULL;
static Eet_Data_Descriptor *_enlil_edd_tag = NULL;
static Eet_Data_Descriptor *_enlil_edd_photo = NULL;
static Eet_Data_Descriptor *_enlil_edd_album = NULL;
Eet_Data_Descriptor *enlil_edd_loaded_library = NULL;

Eina_Bool
enlil_edd_init(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Library);
   _enlil_edd_library = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_library, Enlil_Library, "path", path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_library, Enlil_Library, "name", name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_library, Enlil_Library, "read_only", read_only, EET_T_UCHAR);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Libraries);
   enlil_edd_libraries = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(enlil_edd_libraries, Enlil_Libraries, "version.major", version.major, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(enlil_edd_libraries, Enlil_Libraries, "version.minor", version.minor, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_LIST(enlil_edd_libraries, Enlil_Libraries, "libraries", libraries, _enlil_edd_library);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Album);
   _enlil_edd_album = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_album, Enlil_Album, "path", path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_album, Enlil_Album, "name", name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_album, Enlil_Album, "description", description, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_album, Enlil_Album, "netsync.id", netsync.id, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_album, Enlil_Album, "netsync.version_header", netsync.version_header, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_album, Enlil_Album, "netsync.version_header_net", netsync.version_header_net, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_album, Enlil_Album, "netsync.timestamp_last_update_collections", netsync.timestamp_last_update_collections, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_album, Enlil_Album, "filesystem.last_modified", filesystem.last_modified, EET_T_LONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_album, Enlil_Album, "filesystem.size", filesystem.size, EET_T_LONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_album, Enlil_Album, "filesystem.inode", filesystem.inode, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_LIST_STRING(_enlil_edd_album, Enlil_Album, "photos", photos);
   EET_DATA_DESCRIPTOR_ADD_LIST_STRING(_enlil_edd_album, Enlil_Album, "collections", collections);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Exif);
   _enlil_edd_exif = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_exif, Enlil_Exif, "tag", tag, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_exif, Enlil_Exif, "value", value, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_exif, Enlil_Exif, "description", description, EET_T_STRING);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_IPTC);
   _enlil_edd_iptc = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_iptc, Enlil_IPTC, "record", record, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_iptc, Enlil_IPTC, "tag", tag, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_iptc, Enlil_IPTC, "name", name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_iptc, Enlil_IPTC, "title", title, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_iptc, Enlil_IPTC, "format", format, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_iptc, Enlil_IPTC, "value", value, EET_T_STRING);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Photo);
   _enlil_edd_photo = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "filesystem.path", filesystem.path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "filesystem.last_modified", filesystem.last_modified, EET_T_LONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "filesystem.size", filesystem.size, EET_T_LONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "filesystem.inode", filesystem.inode, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "description.name", description.name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "description.description", description.description, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "description.author", description.author, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "description.w", description.w, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "description.h", description.h, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "netsync.id", netsync.id, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "netsync.version_header", netsync.version_header, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "netsync.version_file", netsync.version_file, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "netsync.version_tags", netsync.version_tags, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "netsync.version_header_net", netsync.version_header_net, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "netsync.version_file_net", netsync.version_file_net, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "netsync.version_tags_net", netsync.version_tags_net, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "exif.DateTimeOriginal", exif.DateTimeOriginal, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "exif.longitude", exif.longitude, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_photo, Enlil_Photo, "exif.latitude", exif.latitude, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_HASH(_enlil_edd_photo, Enlil_Photo, "exif.data", exif.data, _enlil_edd_exif);
   EET_DATA_DESCRIPTOR_ADD_HASH(_enlil_edd_photo, Enlil_Photo, "iptc.data", iptc.data, _enlil_edd_iptc);
   EET_DATA_DESCRIPTOR_ADD_LIST_STRING(_enlil_edd_photo, Enlil_Photo, "tags", tags);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Collection);
   _enlil_edd_collection = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_collection, Enlil_Collection, "name", name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_collection, Enlil_Collection, "description", description, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_LIST_STRING(_enlil_edd_collection, Enlil_Collection, "album", album);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Tag);
   _enlil_edd_tag = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_tag, Enlil_Tag, "name", name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_enlil_edd_tag, Enlil_Tag, "description", description, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_LIST_STRING(_enlil_edd_tag, Enlil_Tag, "photos", photos);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Loaded_Library);
   enlil_edd_loaded_library = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(enlil_edd_loaded_library, Enlil_Loaded_Library, "version.major", version.major, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(enlil_edd_loaded_library, Enlil_Loaded_Library, "version.minor", version.minor, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_LIST_STRING(enlil_edd_loaded_library, Enlil_Loaded_Library, "paths.albums", paths.albums);
   EET_DATA_DESCRIPTOR_ADD_LIST_STRING(enlil_edd_loaded_library, Enlil_Loaded_Library, "paths.tags", paths.tags);
   EET_DATA_DESCRIPTOR_ADD_LIST_STRING(enlil_edd_loaded_library, Enlil_Loaded_Library, "paths.collections", paths.collections);
   EET_DATA_DESCRIPTOR_ADD_HASH(enlil_edd_loaded_library, Enlil_Loaded_Library, "data.albums", data.albums, _enlil_edd_album);
   EET_DATA_DESCRIPTOR_ADD_HASH(enlil_edd_loaded_library, Enlil_Loaded_Library, "data.collections", data.collections, _enlil_edd_collection);
   EET_DATA_DESCRIPTOR_ADD_HASH(enlil_edd_loaded_library, Enlil_Loaded_Library, "data.tags", data.tags, _enlil_edd_tag);
   EET_DATA_DESCRIPTOR_ADD_HASH(enlil_edd_loaded_library, Enlil_Loaded_Library, "data.photos", data.photos, _enlil_edd_photo);

   return EINA_TRUE;
}

Eina_Bool
enlil_edd_shutdown(void)
{
#define DF(Name)                                        \
   eet_data_descriptor_free(Name);                      \
   Name = NULL;

   DF(enlil_edd_libraries);
   DF(_enlil_edd_library);
   DF(_enlil_edd_exif);
   DF(_enlil_edd_iptc);
   DF(_enlil_edd_collection);
   DF(_enlil_edd_tag);
   DF(_enlil_edd_photo);
   DF(_enlil_edd_album);
   DF(enlil_edd_loaded_library);

#undef DF

   return EINA_TRUE;
}
