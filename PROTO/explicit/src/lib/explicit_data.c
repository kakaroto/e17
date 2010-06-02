#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "explicit_server.h"

Eet_Data_Descriptor *_explicit_cache_descriptor = NULL;
static Eet_Data_Descriptor *_explicit_file_descriptor = NULL;
static Eet_Data_Descriptor *_explicit_file_reference_descriptor = NULL;

void
explicit_edd_file_shutdown(void)
{
   explicit_edd_conn_shutdown();

   ECLOSE(eet_data_descriptor_free, _explicit_cache_descriptor);
   ECLOSE(eet_data_descriptor_free, _explicit_file_descriptor);
   ECLOSE(eet_data_descriptor_free, _explicit_file_reference_descriptor);
}

void
explicit_edd_file_init(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Explicit_Reference);
   _explicit_file_reference_descriptor = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_file_reference_descriptor,
				 Explicit_Reference,
				 "offset", offset, EET_T_INT);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Explicit_File);
   _explicit_file_descriptor = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_file_descriptor,
				 Explicit_File,
				 "url", url, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_file_descriptor,
				 Explicit_File,
				 "filename", filename, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_file_descriptor,
				 Explicit_File,
				 "size", size, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_file_descriptor,
				 Explicit_File,
				 "id", id, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_file_descriptor,
				 Explicit_File,
				 "empty", empty, EET_T_UCHAR);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Explicit_Cache);
   _explicit_cache_descriptor = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_cache_descriptor,
				 Explicit_Cache,
				 "private_path", private_path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_cache_descriptor,
				 Explicit_Cache,
				 "public_path", public_path, EET_T_STRING);

   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_cache_descriptor,
				 Explicit_Cache,
				 "soft_limit", soft_limit, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_cache_descriptor,
				 Explicit_Cache,
				 "hard_limit", hard_limit, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_cache_descriptor,
				 Explicit_Cache,
				 "file_limit", file_limit, EET_T_ULONG_LONG);

   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_cache_descriptor,
				 Explicit_Cache,
				 "count", count, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_cache_descriptor,
				 Explicit_Cache,
				 "id", id, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_LIST(_explicit_cache_descriptor,
				Explicit_Cache,
				"lru", lru, _explicit_file_reference_descriptor);
   EET_DATA_DESCRIPTOR_ADD_LIST(_explicit_cache_descriptor,
				Explicit_Cache,
				"empty", empty, _explicit_file_reference_descriptor);
   EET_DATA_DESCRIPTOR_ADD_HASH(_explicit_cache_descriptor,
				Explicit_Cache,
				"done", done, _explicit_file_reference_descriptor);
   EET_DATA_DESCRIPTOR_ADD_LIST(_explicit_cache_descriptor,
				Explicit_Cache,
				"files", files, _explicit_file_descriptor);

   explicit_edd_conn_init();
}
