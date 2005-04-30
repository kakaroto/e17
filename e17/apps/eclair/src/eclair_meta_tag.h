#ifndef _ECLAIR_META_TAG_H_
#define _ECLAIR_META_TAG_H_

#include "eclair_private.h"

void eclair_meta_tag_init(Eclair_Meta_Tag_Manager *meta_tag_manager, Eclair *eclair);
void eclair_meta_tag_shutdown(Eclair_Meta_Tag_Manager *meta_tag_manager);
void eclair_meta_tag_add_file_to_scan(Eclair_Meta_Tag_Manager *meta_tag_manager, Eclair_Media_File *media_file);
void eclair_meta_tag_read(Eclair *eclair, Eclair_Media_File *media_file);

#endif
