#include "email_private.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#ifndef O_CLOEXEC
# define O_CLOEXEC
#endif

Email_Attachment *
email_attachment_new(const char *name, const char *content_type)
{
   Email_Attachment *at;
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, NULL);

   at = calloc(1, sizeof(Email_Attachment));
   EINA_SAFETY_ON_NULL_RETURN_VAL(at, NULL);
   at->name = strdup(name);
   at->content_type = eina_stringshare_add(content_type);
   return at;
}

Email_Attachment *
email_attachment_new_from_file(const char *filename, const char *content_type)
{
   Email_Attachment *at;
   Eina_File *f;
   void *file;

   EINA_SAFETY_ON_NULL_RETURN_VAL(filename, NULL);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!filename[0], NULL);

   f = eina_file_open(filename, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(f, NULL);
   file = eina_file_map_all(f, EINA_FILE_SEQUENTIAL);
   if (!file)
     {
        ERR("mmap of %s failed!", filename);
        goto error;
     }
   at = calloc(1, sizeof(Email_Attachment));
   if (!at) goto munmap;
   at->content = malloc(eina_file_size_get(f));
   if (!at->content)
     {
        free(at);
        goto munmap;
     }
   memcpy(at->content, file, eina_file_size_get(f));
   at->name = eina_stringshare_add(filename);
   at->content_type = eina_stringshare_add(content_type);
   eina_file_map_free(f, file);
   eina_file_close(f);
   return at;

munmap:
   ERR("mem alloc failed!");
   eina_file_map_free(f, file);
error:
   eina_file_close(f);
   return NULL;
}

void
email_attachment_free(Email_Attachment *at)
{
   if (!at) return;

   eina_stringshare_del(at->name);
   free(at->content);
   eina_stringshare_del(at->content_type);
   free(at);
}

const char *
email_attachment_name_get(Email_Attachment *at)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(at, NULL);
   return at->name;
}

const char *
email_attachment_content_type_get(Email_Attachment *at)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(at, NULL);
   return at->content_type;
}

Eina_Bool
email_attachment_content_set(Email_Attachment *at, const void *content, size_t size)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(at, EINA_FALSE);

   free(at->content);
   at->content = NULL;
   at->csize = 0;
   if (!content) return EINA_TRUE;
   at->content = malloc(size);
   if (!at->content) return EINA_FALSE;
   memcpy(at->content, content, size);
   at->csize = size;
   return EINA_TRUE;
}

Eina_Bool
email_attachment_content_steal(Email_Attachment *at, void *content, size_t size)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(at, EINA_FALSE);

   free(at->content);
   at->content = content;
   at->csize = size;
   return EINA_TRUE;
}

const unsigned char *
email_attachment_content_get(Email_Attachment *at, size_t *size)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(at, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(size, EINA_FALSE);
   if (!at->content) return NULL;
   *size = at->csize;
   return at->content;
}
