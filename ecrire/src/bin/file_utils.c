#include <Elementary.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char *
_buf_append(char *buf, const char *str, int *len, int *alloc)
{
   int len2 = strlen(str);
   if ((*len + len2) >= *alloc)
     {
        char *buf2 = realloc(buf, *alloc + len2 + 512);
        if (!buf2) return NULL;
        buf = buf2;
        *alloc += (512 + len2);
     }
   strcpy(buf + *len, str);
   *len += len2;
   return buf;
}

char *
_load_file(const char *file)
{
   FILE *f;
   size_t size;
   int alloc = 0, len = 0;
   char *text = NULL, buf[16384 + 1];

   f = fopen(file, "rb");
   if (!f) return NULL;
   while ((size = fread(buf, 1, sizeof(buf) - 1, f)))
     {
        char *tmp_text;
        buf[size] = 0;
        tmp_text = _buf_append(text, buf, &len, &alloc);
        if (!tmp_text) break;
        text = tmp_text;
     }
   fclose(f);
   return text;
}

char *
_load_plain(const char *file)
{
   char *text;

   text = _load_file(file);
   if (text)
     {
        char *text2;

        text2 = elm_entry_utf8_to_markup(text);
        free(text);
        return text2;
     }
   return NULL;
}

Eina_Bool
_save_markup_utf8(const char *file, const char *text)
{
   FILE *f;

   f = fopen(file, "wb");
   if (!f)
     {
        // FIXME: report a write error
        return EINA_FALSE;
     }

   if (text)
     {
        fputs(text, f); // FIXME: catch error
     }
   fclose(f);
   return EINA_TRUE;
}

Eina_Bool
_save_plain_utf8(const char *file, const char *text)
{
   char *text2;

   text2 = elm_entry_markup_to_utf8(text);
   if (!text2)
     return EINA_FALSE;
   _save_markup_utf8(file, text2);
   free(text2);

   return EINA_TRUE;
}


