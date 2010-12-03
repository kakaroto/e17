#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define _FILE_OFFSET_BITS 64

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <jsapi.h>

#include <eina_hash.h>

#ifdef ENABLE_MAGIC
# include <magic.h>
#endif

#include "elixir_private.h"

#include "Elixir.h"

#define FILES_ROOT "/"

static char *_elixir_file_escape(const char* path, int* length);

struct type_s
{
  int           count;
  int           hash;
  const char    type_name[1];
};

#ifdef ENABLE_MAGIC
static Eina_Hash    *magics = NULL;
static magic_t       magic_cookie = NULL;
#endif

static Eina_Hash    *extentions = NULL;
static Eina_Hash    *filters = NULL;

EAPI FILE*   out = NULL;

char*   files_root = NULL;
int     files_root_length = -1;

static int
_elixir_hash_unsensitiv(const char *key, int length)
{
   unsigned int hash_num = 5381;
   const unsigned char *ptr;

   if (!key) return 0;
   for (ptr = (unsigned char *)key; *ptr && length > 0; ptr++, length--)
     hash_num = ((hash_num << 5) + hash_num) ^ (tolower(*ptr));

   return (int)hash_num;
}

static int
_elixir_cmp_unsensitiv(const char *k1, __UNUSED__ int l1,
		       const char *k2, __UNUSED__ int l2)
{
   return strcasecmp(k1, k2);
}

static int
_elixir_length_unsensitiv(const char *key)
{
   if (!key) return 0;
   return strlen(key) + 1;
}

Eina_Bool
elixir_file_init(void)
{
   if (files_root)
     free(files_root);

#ifdef ENABLE_MAGIC
   if (magic_cookie)
     return EINA_FALSE;
   magic_cookie = magic_open(MAGIC_SYMLINK | MAGIC_MIME | MAGIC_PRESERVE_ATIME);

   magics = eina_hash_string_superfast_new(free);
#endif

   files_root = strdup(FILES_ROOT);
   files_root_length = strlen(files_root);

   extentions = eina_hash_new(EINA_KEY_LENGTH(_elixir_length_unsensitiv),
			      EINA_KEY_CMP(_elixir_cmp_unsensitiv),
			      EINA_KEY_HASH(_elixir_hash_unsensitiv),
			      free,
			      7);
   filters = eina_hash_string_djb2_new(NULL);

   return EINA_TRUE;
}

Eina_Bool
elixir_file_shutdown(void)
{
   eina_hash_free(extentions);
   extentions = NULL;

   eina_hash_free(filters);
   filters = NULL;

#ifdef ENABLE_MAGIC
   eina_hash_free(magics);
   magics = NULL;

   if (magic_cookie)
     magic_close(magic_cookie);
   magic_cookie = NULL;
#endif

   if (files_root)
     free(files_root);
   files_root = NULL;
   files_root_length = -1;

   return EINA_TRUE;
}

const char*
elixir_file_virtual_chroot_get(void)
{
   return files_root;
}

Eina_Bool
elixir_file_virtual_chroot_set(const char *directory)
{
   if (files_root)
     {
        free(files_root);
        files_root = NULL;
        files_root_length = -1;
     }
   if (!directory)
     return EINA_TRUE;

   files_root_length = strlen(directory);
   files_root = _elixir_file_escape(directory, &files_root_length);
   if (!files_root)
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
elixir_file_chroot_set(const char *directory)
{
   if (!directory)
     return EINA_FALSE;

   if (chroot(directory) == 0)
     return EINA_TRUE;
   return EINA_FALSE;
}

static struct type_s *
elixir_type_add(const char *name)
{
   struct type_s        *new;
   int length;

   length = strlen(name);

   new = malloc(sizeof (struct type_s) + sizeof (char) * length);
   if (!new)
     return NULL;

   new->count = 1;
   new->hash = eina_hash_djb2(name, length + 1);
   memcpy((char*) new->type_name, name, length + 1);

   return new;
}

Eina_Bool
elixir_magic_register(const char *mime, const char *name)
{
#ifdef ENABLE_MAGIC
   struct type_s        *new;

   new = eina_hash_find(magics, mime);
   if (new)
     {
        new->count++;
        return EINA_TRUE;
     }

   new = elixir_type_add(name);
   if (!new)
     return EINA_FALSE;

   eina_hash_add(magics, mime, new);
#endif

   return EINA_TRUE;
}

static void
_stolower(char *dst, const char *src)
{
   for (; *src != '\0'; ++src, ++dst)
     *dst = tolower(*src);
   *dst = '\0';
}

Eina_Bool
elixir_file_register(const char *extention, const char *name)
{
   struct type_s *new;
   char *tmp;

   tmp = alloca(strlen(name) + 1);
   if (!tmp) return EINA_FALSE;
   _stolower(tmp, extention);

   new = eina_hash_find(extentions, tmp);
   if (new)
     {
        new->count++;
        return EINA_TRUE;
     }

   new = elixir_type_add(name);
   if (!new) return EINA_FALSE;

   eina_hash_add(extentions, tmp, new);

   return EINA_TRUE;
}

Eina_Bool
elixir_filter_register(const char *filter)
{
   if (eina_hash_find(filters, filter)) return EINA_TRUE;

   eina_hash_add(filters, filter, (void*) 0x1);

   return EINA_TRUE;
}

Eina_Bool
elixir_magic_unregister(const char *mime, const char *name)
{
#ifdef ENABLE_MAGIC
   struct type_s *lookup;

   (void) name;

   lookup = eina_hash_find(magics, mime);
   if (!lookup)
     return EINA_FALSE;

   lookup->count--;

   if (lookup->count == 0)
     eina_hash_del(magics, mime);

   return EINA_TRUE;
#else
   return EINA_FALSE;
#endif
}

Eina_Bool
elixir_file_unregister(const char *extention, const char *name)
{
   struct type_s *lookup;

   (void) name;

   lookup = eina_hash_find(extentions, extention);
   if (!lookup)
     return EINA_FALSE;

   lookup->count--;

   if (lookup->count == 0)
     eina_hash_del(extentions, extention, lookup);

   return EINA_TRUE;
}

Eina_Bool
elixir_filter_unregister(const char *filter)
{
   eina_hash_del(filters, filter, NULL);

   return EINA_TRUE;
}

static const char*
_elixir_file_type(const char *filename, const char *mime)
{
   struct type_s *lookup = NULL;
   const char *ext;

#ifdef ENABLE_MAGIC
   if (mime)
     lookup = eina_hash_find(magics, mime);
#endif

   if (!lookup)
     {
        ext = strrchr(filename, '.');
        if (ext)
	  lookup = eina_hash_find(extentions, ext);
     }

   if (lookup)
     {
	if (filters)
	  {
	     if (eina_hash_find_by_hash(filters, lookup->type_name, strlen(lookup->type_name) + 1, lookup->hash))
	       return lookup->type_name;
	     return NULL;
	  }
	return lookup->type_name;
     }
   return NULL;
}

const char*
elixir_file_type(const char *filename)
{
   const char   *mime = NULL;

#ifdef ENABLE_MAGIC
   if (magic_cookie)
     mime = magic_file(magic_cookie, filename);
#endif

   return _elixir_file_type(filename, mime);
}

static char*
_elixir_file_escape(const char* path, int* length)
{
   char *result = strdup(path ? path : "");
   char *p = result;
   char *q = result;
   int len;

   if (!result)
     return NULL;

   if (length) len = *length;
   else len = strlen(result);

   while ((p = strchr(p, '/')))
     {
	// remove double `/'
	if (p[1] == '/')
	  {
	     memmove(p, p + 1, --len - (p - result));
	     result[len] = '\0';
	  }
	else
	  if (p[1] == '.'
	      && p[2] == '.')
	    {
	       // remove `/../'
	       if (p[3] == '/')
		 {
		    char tmp;

		    len -= p + 3 - q;
		    memmove(q, p + 3, len - (q - result));
		    result[len] = '\0';
		    p = q;

		    /* Update q correctly. */
		    tmp = *p;
		    *p = '\0';
		    q = strrchr(result, '/');
		    if (!q) q = result;
		    *p = tmp;
		 }
	       else
		 // remove '/..$'
		 if (p[3] == '\0')
		   {
		      len -= p + 2 - q;
		      result[len] = '\0';
		      q = p;
		      ++p;
		   }
		 else
		   {
		      q = p;
		      ++p;
		   }
	    }
	  else
	    {
	       q = p;
	       ++p;
	    }
     }

   if (length)
     *length = len;
   return result;
}

char*
elixir_file_canonicalize(const char* path)
{
   char *result = NULL;
   char *p;
   int len;

   if (!path) return NULL;

   len = strlen(path);

   if (*path != '/')
     {
        char cwd[PATH_MAX];
        char *tmp = NULL;

        cwd = getcwd(cwd, PATH_MAX);
        if (!cwd) return NULL;

        len += strlen(cwd) + 2;
        tmp = alloca(sizeof (char) * len);

        slprintf(tmp, len, "%s/%s", cwd, path);

        result = tmp;
     }

   result = _elixir_file_escape(result ? result : path, &len);
   if (!result)
     return NULL;

   // Add root start, '/', if needded
   if (strncmp(result, files_root, files_root_length))
     {
	char*	tmp = malloc(sizeof (char) * (len + 2 + files_root_length));

	strcpy(tmp, files_root);
	strcat(tmp, result);
	free(result);
	result = tmp;

        len += files_root_length;

	// remove double `/'
	while ((p = strstr(result, "//")))
	  {
	     memmove(p, p + 1, --len - (p - result));
	     result[len] = '\0';
	  }
     }

   return result;
}

static const char*
remove_files_root(const char* file, int* size)
{
   if (strncmp(file, files_root, files_root_length) == 0)
     {
        size -= files_root_length - (files_root[files_root_length - 1] == '/' ? 1 : 0);
        return file + files_root_length - (files_root[files_root_length - 1] == '/' ? 1 : 0);
     }
   return file;
}

struct elixir_file_s*
elixir_file_stat(const char* file)
{
   struct elixir_file_s         *new = NULL;

   const char                   *mime = NULL;

   const char                   *full_name;
   const char                   *path;

   const char                   *file_name;
   const char                   *short_name;

   const char                   *type;
   const char                   *class;

   char                         *safe_file;

   int                           safe_file_length;
   int                           file_name_length;
   int                           short_name_length;
   int                           full_name_length;
   int                           path_length;
   int				 type_length;

   int                           size;
   struct stat                   st;

   safe_file = elixir_file_canonicalize(file);
   if (!safe_file)
     return NULL;

   if (lstat(safe_file, &st) < 0)
     goto on_error;
   safe_file_length = strlen(safe_file);

   file_name = strdupa(basename(strdupa(file)));

   if (S_ISDIR(st.st_mode))
     {
        class = "elx_dir";
        type = "Directory";
        size = -1;

        short_name = strdupa(file_name);
        if (!short_name)
          goto on_error;

        path = alloca(sizeof(char) * (safe_file_length + 2));
        if (!path)
          goto on_error;

        if (safe_file[safe_file_length - 1] != '/')
          sprintf((char*) path, "%s/", safe_file);
        else
          strcpy((char*) path, safe_file);
     }
   else if (S_ISLNK(st.st_mode))
     {
        char            linkname[4096] = "";
        char*           safe_link;
        int             position;

        position = readlink(safe_file, linkname, sizeof(linkname));
        if (position < 0)
          goto on_error;

        linkname[position] = '\0';

        if (*linkname != '/')
          {
             safe_link = alloca(strlen(safe_file) + strlen(linkname) + 2);
             sprintf(safe_link, "%s/../%s", safe_file, linkname);
          }
        else
          safe_link = linkname;

        safe_link = _elixir_file_escape(safe_link, NULL);

        if (strncmp(safe_link, files_root, strlen(files_root)) != 0)
          {
             free(safe_link);
             goto on_error;
          }

	if (strcmp(safe_link, safe_file) == 0)
	  {
	     free(safe_link);
	     goto on_error;
	  }

        new = elixir_file_stat(safe_link);
        free(safe_link);

        return new;
     }
   else
     {
        const char      *tmp;

        class = "elx_file";

#ifdef ENABLE_MAGIC
        if (magic_cookie)
          mime = magic_file(magic_cookie, safe_file);
#endif

        type = _elixir_file_type(safe_file, mime);
        /* Avoid integer overflow by reporting size in KBytes */
        size = st.st_size >> 10;

        if (!type)
          goto on_error;

        path = strdupa(dirname(strdupa(safe_file)));
        if (!path)
          goto on_error;

        /* Remove extention */
        tmp = strrchr(file_name, '.');
        if (tmp)
          short_name = strndupa(file_name, tmp - file_name);
        else
          short_name = file_name;
     }

   full_name = safe_file;

   full_name_length = strlen(full_name) + 1;
   path_length = strlen(path) + 1;

   if (files_root)
     {
        path = remove_files_root(path, &path_length);
        full_name = remove_files_root(full_name, &full_name_length);
     }

   file_name_length = strlen(file_name) + 1;
   short_name_length = strlen(short_name) + 1;
   type_length = strlen(type) + 1;

   new = malloc(sizeof(struct elixir_file_s) + file_name_length + short_name_length + full_name_length + path_length + type_length);
   if (!new)
     goto on_error;

   new->file_name = (char*) ((struct elixir_file_s*) new + 1);
   new->short_name = new->file_name + file_name_length;
   new->full_name = new->short_name + short_name_length;
   new->path = new->full_name + full_name_length;
   new->type = new->path + path_length;

   strcpy((char*) new->file_name, file_name);
   strcpy((char*) new->short_name,  short_name);
   strcpy((char*) new->full_name, full_name);
   strcpy((char*) new->path, path);
   strcpy((char*) new->type, type);

   new->file_name_length = file_name_length;
   new->mime = mime;
   new->class = class;
   new->size = size;

  on_error:
   free(safe_file);
   return new;
}

JSObject*
elixir_file_stat_object(JSContext* cx, struct elixir_file_s* file)
{
   JSObject*    obj = NULL;

   if (!file)
     return NULL;

   obj = JS_NewObject(cx, elixir_class_request(file->class, NULL), NULL, NULL);

   if (!obj)
     goto on_error;

   if (elixir_object_register(cx, &obj, NULL) == JS_FALSE)
     goto on_error;

   elixir_add_str_prop(cx, obj, "type", file->type);
   elixir_add_str_prop(cx, obj, "name", file->file_name);
   elixir_add_str_prop(cx, obj, "short_name", file->short_name);
   elixir_add_str_prop(cx, obj, "full_name", file->full_name);
   elixir_add_str_prop(cx, obj, "path", file->path);
   elixir_add_int_prop(cx, obj, "size", file->size);

   if (file->mime)
     elixir_add_str_prop(cx, obj, "mime", file->mime);

  on_error:
   if (obj)
     elixir_object_unregister(cx, &obj);
   return obj;
}

