#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "evfs.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <Eet.h>
#include <limits.h>

void evfs_metadata_file_set_key_value_edd(evfs_filereference* ref, char* key, 
		void* value, Eet_Data_Descriptor* edd) 
{
}

void evfs_metadata_file_set_key_value_string(evfs_filereference* ref, char* key,
		char* value) 
{
}
