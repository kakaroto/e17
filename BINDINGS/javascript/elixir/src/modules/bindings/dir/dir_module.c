#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define _FILE_OFFSET_BITS 64
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define __USE_BSD
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>

#include <eina_list.h>

#include "elixir_private.h"

#include "Elixir.h"

struct elixir_file_dir_s
{
   struct elixir_file_s*        file;
   char*			safe;
   char*			filename;
   char*                        lower_filename;
   int				filename_length;
   int                          is_directory;
};

struct elixir_path_s
{
   const char           *name;

   dev_t                 dev;
   ino_t                 ino;
   time_t                mtime;

   struct elixir_path_s *next;
};

struct elixir_directory_s
{
   char                 *name;
   int                   references;

   struct elixir_path_s *paths;

   Eina_List            *list;
};

struct elixir_virtual_dir_s
{
   char*                        real;
   char*                        virtual;

   struct elixir_virtual_dir_s* next;
};

static elixir_parameter_t       _dir_parameter = {
   "DIR", JOBJECT, NULL
};

static const elixir_parameter_t*        _dir_params[2] = {
   &_dir_parameter,
   NULL
};

static const elixir_parameter_t*        _dir_2int_params[4] = {
   &_dir_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};

static const elixir_parameter_t*        _2string_params[3] = {
   &string_parameter,
   &string_parameter,
   NULL
};

/* That's bad, should put this somewhere else (inside parent object for example). */
static struct elixir_virtual_dir_s*    virtuals = NULL;

static int
file_cmp(const struct elixir_file_dir_s* f1, const struct elixir_file_dir_s* f2)
{
   if (f1->is_directory != f2->is_directory)
     return f2->is_directory - f1->is_directory;

   if (f1->filename[f1->filename_length - 3] == '/'
       && f1->filename[f1->filename_length - 2] == '.'
       && f1->filename[f1->filename_length - 1] == '.')
     return 1;

   if (f2->filename[f2->filename_length - 3] == '/'
       && f2->filename[f2->filename_length - 2] == '.'
       && f2->filename[f2->filename_length - 1] == '.')
     return -1;

   return strcmp(f1->lower_filename, f2->lower_filename);
}

static Eina_List *
file_insert(Eina_List *list, const char *safe_name, const char *name, int is_directory, struct elixir_file_s *file)
{
   struct elixir_file_dir_s *new;
   char                      *dst;
   int                        length;
   int                        length_safe;

   length_safe = strlen(safe_name) + 1;
   length = strlen(name) + 1;

   new = malloc(sizeof(struct elixir_file_dir_s) + length * 2 + length_safe);
   if (!new) return list;

   new->file = file;
   new->filename = (char*) (new + 1);
   new->lower_filename = new->filename + length;
   new->safe = new->lower_filename + length;
   new->filename_length = length;
   new->is_directory = is_directory ? 1 : 0;

   memcpy(new->filename, name, length);
   memcpy(new->lower_filename, name, length);
   memcpy(new->safe, safe_name, length_safe);

   for (dst = new->lower_filename;
	length > 0;
	++dst, --length)
     *dst = tolower(*dst);

   return eina_list_prepend(list, new);
}

static Eina_List *
dir_content(Eina_List *files, const char *filename)
{
   struct dirent*               dent;
   DIR*                         dir;
   char				path[4096];
   int                          keep_back;
   int                          filename_length;

   filename_length = strlen(filename);
   if (filename_length > 4095) return NULL;
   memcpy(path, filename, filename_length);
   path[filename_length] = '/';

   dir = opendir(filename);
   if (!dir)
     return files;

   keep_back = files ? 1 : 0;

   while ((dent = readdir(dir)))
     {
        struct elixir_file_s*   new = NULL;
	const char*             type;
	char*                   file;
        int                     is_directory = 0;
	int                     name_length;

        /* Don't list hidden file. */
        if (dent->d_name[0] == '.' && dent->d_name[1] != '.')
          continue ;

        /* No escape from files_root. */
        if (strcmp(dent->d_name, "..") == 0)
          if (keep_back || strcmp(filename, elixir_file_virtual_chroot_get()) == 0)
            continue ;

	name_length = strlen(dent->d_name);
	if (name_length + filename_length + 2 > 4096) continue ;

	memcpy(path + filename_length + 1, dent->d_name, name_length);
	path[filename_length + name_length + 1] = '\0';
	file = elixir_file_canonicalize(path);
	if (!file) continue ;

	switch (dent->d_type)
	  {
	   case DT_UNKNOWN:
	      /* Bad file system, need to do a stat. */
	   case DT_LNK:
	      /* Need to check the link. */
	      new = elixir_file_stat(path);
	      if (!new)
		goto end;

	      if (new->size == -1)
		is_directory = 1;
	      break;
	   case DT_FIFO:
	   case DT_CHR:
	   case DT_BLK:
	   case DT_SOCK:
	   case DT_WHT:
	      continue;

	   case DT_REG:
	      type = elixir_file_type(file);
	      if (!type) goto end;

	      break;
	   case DT_DIR:
	      is_directory = 1;
	      break;
	  }

        files = file_insert(files, file, path, is_directory, new);

     end:
	free(file);
     }
   closedir(dir);

   return files;
}

static void
dir_cleanup(struct elixir_directory_s* result)
{
   struct elixir_directory_s *cached_dir;
   struct elixir_file_dir_s  *die;
   struct elixir_path_s      *paths;
   struct elixir_path_s      *it;

   if (result->references == 0)
     {
	free(result->name);

	while (result->list)
	  {
	     die = eina_list_data_get(result->list);

	     if (die->file) free(die->file);
	     free(die);

	     result->list = eina_list_remove_list(result->list, result->list);
	  }

	it = result->paths;
	while (it)
	  {
	     paths = it;
	     it = it->next;

	     free(paths);
	  }

	cached_dir = elixir_context_find("cached_directory");
	if (cached_dir == result) elixir_context_delete("cached_directory");
	free(result);
     }
}

static JSBool
elixir_flush(JSContext *cx, uintN argc, jsval *vp)
{
   struct elixir_directory_s*   cached_dir;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   cached_dir = elixir_context_find("cached_directory");
   elixir_context_delete("cached_directory");
   if (cached_dir)
     {
	cached_dir->references--;
	dir_cleanup(cached_dir);
     }

   return JS_TRUE;
}


static JSBool
elixir_open(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List                   *directory = NULL;
   struct elixir_path_s        *lists = NULL;
   struct elixir_path_s        *paths;
   struct elixir_virtual_dir_s *lookup;
   struct elixir_directory_s   *result;
   struct elixir_directory_s   *cached_dir;
   const char                  *path;
   char                         tmp[PATH_MAX];
   char                        *safe_file;
   struct stat			buffer;
   int                          lists_count = 0;
   elixir_value_t               val[2];

   path = elixir_file_virtual_chroot_get();
   if (elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     path = elixir_get_string_bytes(val[0].v.str, NULL);

   snprintf(tmp, PATH_MAX, "%s/", path);

   safe_file = elixir_file_canonicalize(tmp);
   if (!safe_file)
     return JS_FALSE;

   if (!stat(safe_file, &buffer))
     {
	paths = malloc(sizeof (struct elixir_path_s) + strlen(safe_file) + 1);
	if (paths)
	  {
	     paths->name = (const char*) (paths + 1);

	     strcpy((char*) paths->name, safe_file);

	     paths->dev = buffer.st_dev;
	     paths->ino = buffer.st_ino;
	     paths->mtime = buffer.st_mtime;

	     paths->next = lists;
	     lists = paths;

	     lists_count++;
	  }
     }

   for (lookup = virtuals; lookup; lookup = lookup->next)
     {
	if (strcmp(lookup->real, safe_file) == 0)
	  {
	     if (stat(lookup->virtual, &buffer))
	       continue ;

	     paths = malloc(sizeof (struct elixir_path_s) + strlen(lookup->virtual) + 1);
	     if (!paths) continue ;
	     paths->name = (const char*) (paths + 1);

	     strcpy((char*) paths->name, lookup->virtual);

	     paths->dev = buffer.st_dev;
	     paths->ino = buffer.st_ino;
	     paths->mtime = buffer.st_mtime;

	     paths->next = lists;
	     lists = paths;

	     lists_count++;
	  }
     }

   cached_dir = elixir_context_find("cached_directory");
   if (cached_dir
       && strcmp(cached_dir->name, safe_file) == 0)
     {
	struct elixir_path_s *it;
	int cached_count = 0;

	for (it = cached_dir->paths; it; it = it->next)
	  {
	     for (paths = lists; paths; paths = paths->next)
	       {
		  if (strcmp(paths->name, it->name) == 0)
		    {
		       if (it->dev != paths->dev
			   || it->ino != paths->ino
			   || it->mtime != paths->mtime)
			 break;

		       cached_count++;
		    }
	       }
	  }

	if (cached_count == lists_count)
	  {
	     it = lists;
	     while (it)
	       {
		  paths = it;
		  it = it->next;
		  free(paths);
	       }

	     result = cached_dir;
	     goto ok;
	  }
     }
   result = calloc(1, sizeof (struct elixir_directory_s));
   if (!result)
     {
	free(safe_file);
	return JS_FALSE;
     }

   directory = dir_content(NULL, safe_file);
   /* Process virtual directory */
   for (lookup = virtuals; lookup; lookup = lookup->next)
     if (strcmp(lookup->real, safe_file) == 0)
       directory = dir_content(directory, lookup->virtual);

   result->list = eina_list_sort(directory, -1, EINA_COMPARE_CB(file_cmp));
   result->name = safe_file;
   result->paths = lists;

   if (cached_dir)
     {
	cached_dir->references--;
	dir_cleanup(cached_dir);
     }
   elixir_context_delete("cached_directory");
   elixir_context_add("cached_directory", result);
   result->references++;

 ok:
   result->references++;

   elixir_return_ptr(cx, vp, result, elixir_class_request("DIR*", NULL));
   return JS_TRUE;
}

static JSBool
elixir_filter(JSContext *cx, uintN argc, jsval *vp)
{
   const char     *filter;
   elixir_value_t  val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   filter = elixir_get_string_bytes(val[0].v.str, NULL);
   elixir_filter_register(filter);

   return JS_TRUE;
}

static JSBool
elixir_close(JSContext *cx, uintN argc, jsval *vp)
{
   struct elixir_directory_s*   result;
   elixir_value_t               val[1];

   if (!elixir_params_check(cx, _dir_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE_S(cx, val[0].v.obj, result);

   if (!result)
     return JS_FALSE;

   result->references--;
   dir_cleanup(result);

   JS_SetPrivate(cx, val[0].v.obj, NULL);

   return JS_TRUE;
}

static JSBool
elixir_list(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List                 *directory;
   struct elixir_directory_s *result;
   JSObject                  *array;
   int                        offset;
   int                        length;
   int                        index;
   elixir_value_t             val[3];

   if (!elixir_params_check(cx, _dir_2int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   JS_SET_RVAL(cx, vp, JSVAL_NULL);
   GET_PRIVATE_S(cx, val[0].v.obj, result);
   offset = val[1].v.num;
   length = val[2].v.num;

   if (!result)
     return JS_FALSE;

   directory = result->list;

   array = elixir_return_array(cx, vp);
   if (!array)
     return JS_FALSE;

   directory = eina_list_nth_list(directory, offset);

   for (index = 0; directory && (length == -1 || index < length); ++index, directory = eina_list_next(directory))
     {
	struct elixir_file_dir_s *current;
	struct elixir_file_s     *new;

	current = eina_list_data_get(directory);

	if (current->file) new = current->file;
	else new = elixir_file_stat(current->filename);
	if (!new)
	  {
	     --index;
	     continue ;
	  }

	JS_DefineElement(cx, array, index, OBJECT_TO_JSVAL(elixir_file_stat_object(cx, new)),
			 NULL, NULL, JSPROP_INDEX | JSPROP_ENUMERATE | JSPROP_READONLY);

	current->file = new;
     }

   return JS_TRUE;
}

static JSBool
elixir_count(JSContext *cx, uintN argc, jsval *vp)
{
   struct elixir_directory_s *result;
   int                        count;
   elixir_value_t             val[1];

   if (!elixir_params_check(cx, _dir_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE_S(cx, val[0].v.obj, result);

   count = eina_list_count(result->list);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(count));
   return JS_TRUE;
}

static JSBool
elixir_virtual(JSContext *cx, uintN argc, jsval *vp)
{
   struct elixir_virtual_dir_s* new;
   const char*                  real;
   const char*                  virtual;
   char                         tmp[PATH_MAX];
   elixir_value_t               val[2];

   if (!elixir_params_check(cx, _2string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   real = elixir_get_string_bytes(val[0].v.str, NULL);
   virtual = elixir_get_string_bytes(val[1].v.str, NULL);

   if (!real || !virtual)
     return JS_FALSE;

   new = malloc(sizeof(struct elixir_virtual_dir_s));
   if (!new)
     return JS_FALSE;

   snprintf(tmp, PATH_MAX, "%s/", real);
   new->real = elixir_file_canonicalize(tmp);

   snprintf(tmp, PATH_MAX, "%s/", virtual);
   new->virtual = elixir_file_canonicalize(tmp);

   new->next = virtuals;
   virtuals = new;

   return JS_TRUE;
}

static JSFunctionSpec        dir_functions[] = {
  ELIXIR_FN(open, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(close, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(list, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(virtual, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(count, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(filter, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(flush, 0, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

static Eina_Bool
module_open(Elixir_Module* em, JSContext* cx, JSObject* parent)
{
   Elixir_Sub_Module*   esmd;

   if (em->data)
     return EINA_TRUE;

   esmd = malloc(sizeof(Elixir_Sub_Module));
   if (!esmd)
     return EINA_FALSE;
   esmd->parent = parent;
   if (!elixir_object_register(cx, (JSObject**) &esmd->parent, NULL))
     goto on_error;

   esmd->item = JS_DefineObject(cx, esmd->parent, "dir", elixir_class_request("elixir-dir", NULL), NULL, JSPROP_READONLY | JSPROP_ENUMERATE);
   if (!elixir_object_register(cx, (JSObject**) &esmd->item, NULL))
     goto on_error;

   if (!JS_DefineFunctions(cx, esmd->item, dir_functions))
     goto on_error;

   em->data = esmd;
   _dir_parameter.class = elixir_class_request("DIR*", NULL);

   return EINA_TRUE;

  on_error:
   if (esmd->item)
     {
        elixir_object_unregister(cx, (JSObject**) &esmd->item);
        JS_DeleteProperty(cx, esmd->parent, "dir");
        elixir_object_unregister(cx, (JSObject**) &esmd->parent);
     }
   free(esmd);
   return EINA_FALSE;
}

static Eina_Bool
module_close(Elixir_Module* em, JSContext* cx)
{
   struct elixir_directory_s *cached_dir;
   Elixir_Sub_Module *esmd;
   int i = 0;

   if (!em->data)
     return EINA_FALSE;
   esmd = em->data;

   while (dir_functions[i].name)
     JS_DeleteProperty(cx, esmd->item, dir_functions[i++].name);

   while (virtuals)
     {
        struct elixir_virtual_dir_s*    die;

        die = virtuals;
        virtuals = virtuals->next;

        free(die->real);
        free(die->virtual);
        free(die);
     }

   elixir_object_unregister(cx, (JSObject**) &esmd->item);
   JS_DeleteProperty(cx, esmd->parent, "dir");
   elixir_object_unregister(cx, (JSObject**) &esmd->parent);

   cached_dir = elixir_context_find("cached_directory");
   if (cached_dir && cached_dir->references > 0)
	cached_dir->references = 1;

   free(esmd);
   em->data = NULL;

   return EINA_TRUE;
}

static const Elixir_Module_Api  module_api_elixir = {
   ELIXIR_MODULE_API_VERSION,
   ELIXIR_AUTH,
   "dir",
   "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_dir = {
  &module_api_elixir,
  NULL,
  EINA_FALSE,
  {
    module_open,
    NULL,
    module_close
  }
};

Eina_Bool
dir_binding_init(void)
{
   eina_init();

   return elixir_modules_register(&em_dir);
}

void
dir_binding_shutdown(void)
{
   elixir_modules_unregister(&em_dir);

   eina_shutdown();
}

#ifndef EINA_STATIC_BUILD_DIR
EINA_MODULE_INIT(dir_binding_init);
EINA_MODULE_SHUTDOWN(dir_binding_shutdown);
#endif
