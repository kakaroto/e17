#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "Elixir.h"

static elixir_parameter_t               _file_parameter = {
   "FILE", JOBJECT, NULL
};

static const elixir_parameter_t*        _file_params[2] = {
   &_file_parameter,
   NULL
};

static const elixir_parameter_t*        _file_int_params[3] = {
   &_file_parameter,
   &int_parameter,
   NULL
};

typedef struct mmaped_file_s    mmaped_file_t;
struct mmaped_file_s
{
  const char    *data;
  int            fd;
  int            length;
  int            offset;
  const char     filename[1];
};

static JSBool
elixir_fopen(JSContext *cx, uintN argc, jsval *vp)
{
   mmaped_file_t *fl;
   const char *file;
   char *safe_file;
   int fd = -1;
   struct stat stf;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   file = elixir_get_string_bytes(val[0].v.str, NULL);

   safe_file = elixir_file_canonicalize(file);
   if (!safe_file) goto error_safe_file;
   fl = malloc(sizeof (mmaped_file_t) + strlen(safe_file) + 1);
   if (!fl) goto error_fl;

   fd = open(safe_file, O_RDONLY);
   if (fd == -1)
     goto failed_fd;

   if (fstat(fd, &stf) != 0)
     goto failed;

   if (!S_ISREG(stf.st_mode))
     goto failed;

   fl->length = stf.st_size;
   fl->offset = 0;
   fl->fd = fd;
   fl->data = mmap(NULL, fl->length, PROT_READ, MAP_SHARED, fd, 0);
   memcpy((char*) fl->filename, safe_file, strlen(safe_file) + 1);

   if (!fl->data)
     goto failed;

   madvise((void*) fl->data, fl->length, MADV_SEQUENTIAL);

   free(safe_file);
   elixir_return_ptr(cx, vp, fl, elixir_class_request("FILE", NULL));
   return JS_TRUE;

failed:
   close(fd);
failed_fd:
   free(fl);
   free(safe_file);
   elixir_return_ptr(cx, vp, NULL, elixir_class_request("FILE", NULL));
   return JS_TRUE;

error_fl:
   free(safe_file);
error_safe_file:
   return JS_FALSE;
}

static JSBool
elixir_fclose(JSContext *cx, uintN argc, jsval *vp)
{
   mmaped_file_t *fl;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _file_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, fl);

   if (!fl)
     return JS_FALSE;

   munmap((void*) fl->data, fl->length);
   close(fl->fd);
   fl->data = NULL;
   fl->length = 0;
   free(fl);

   JS_SetPrivate(cx, val[0].v.obj, NULL);

   return JS_TRUE;
}

static JSBool
elixir_freadall(JSContext *cx, uintN argc, jsval *vp)
{
   mmaped_file_t *fl;
   const char *tmp = NULL;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _file_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, fl);

   if (!fl)
     return JS_FALSE;

   tmp = fl->data;

   elixir_return_str(cx, vp, tmp);
   return JS_TRUE;
}

static JSBool
elixir_fgets(JSContext *cx, uintN argc, jsval *vp)
{
   mmaped_file_t *fl;
   const char *tmp = NULL;
   const char *lookup;
   int delta;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _file_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, fl);

   if (!fl)
     return JS_FALSE;

   if (fl->offset >= fl->length) {
      JS_SET_RVAL(cx, vp, JSVAL_NULL);
   } else {
      tmp = fl->data + fl->offset;
      lookup = memchr(tmp, '\n', fl->length - fl->offset);
      if (!lookup)
        lookup = fl->data + fl->length;

      delta = (lookup - tmp) + 1;

      elixir_return_str(cx, vp, strndupa(tmp, delta));

      fl->offset += delta;
   }

   return JS_TRUE;
}

static JSBool
elixir_fread(JSContext *cx, uintN argc, jsval *vp)
{
   mmaped_file_t *fl;
   const char *tmp = NULL;
   int size;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _file_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, fl);
   size = val[1].v.num;

   if (!fl)
     return JS_FALSE;

   if (fl->offset >= fl->length) {
      JS_SET_RVAL(cx, vp, JSVAL_NULL);
   } else {
      tmp = fl->data + fl->offset;
      if (size > fl->length - fl->offset)
        size = fl->length - fl->offset;

      elixir_return_strn(cx, vp, tmp, size);

      fl->offset += size;
   }

   return JS_TRUE;
}

static JSBool
elixir_feof(JSContext *cx, uintN argc, jsval *vp)
{
   mmaped_file_t *fl;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _file_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, fl);

   if (!fl)
     return JS_FALSE;

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL((fl->offset >= fl->length)));
   return JS_TRUE;
}

static JSBool
elixir_fstat(JSContext *cx, uintN argc, jsval *vp)
{
   const char *path;
   struct elixir_file_s *efs;
   JSObject *new;
   elixir_value_t val[1];

   path = NULL;

   if (elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     path = elixir_get_string_bytes(val[0].v.str, NULL);
   else
     if (elixir_params_check(cx, _file_params, val, argc, JS_ARGV(cx, vp)))
       {
          mmaped_file_t* fl;

          GET_PRIVATE(cx, val[0].v.obj, fl);

          path = fl->filename;
       }

   if (!path)
     return JS_FALSE;

   efs = elixir_file_stat(path);
   new = elixir_file_stat_object(cx, efs);
   JS_SET_RVAL(cx, vp, (!new) ? JSVAL_NULL : OBJECT_TO_JSVAL(new));

   free(efs);
   return JS_TRUE;
}

static JSBool
elixir_unlink(JSContext *cx, uintN argc, jsval *vp)
{
   const char *path;
   int ret;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   path = elixir_get_string_bytes(val[0].v.str, NULL);

   ret = unlink(path);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ret));

   return JS_TRUE;
}

static JSFunctionSpec        file_functions[] = {
  ELIXIR_FN(fopen, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(freadall, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(fclose, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(fgets, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(fread, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(feof, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(fstat, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(unlink, 1, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

static const struct {
   const char *extention;
   const char *type_name;
} file_types[] = {
   { ".txt", "Text File" },
   { ".csv", "Text File" },
   { ".htm", "HTML File" },
   { ".html", "HTML File" },
   { ".m3u", "Playlist File" },
};

static Eina_Bool
module_open(Elixir_Module *em, JSContext *cx, JSObject *parent)
{
   void **tmp = NULL;
   unsigned int i;

   if (em->data)
     return EINA_TRUE;

   em->data = parent;
   tmp = &em->data;

   if (!elixir_object_register(cx, (JSObject**) tmp, NULL))
     goto on_error;

   if (!JS_DefineFunctions(cx, *((JSObject**) tmp), file_functions))
     goto on_error;

   for (i = 0; i < sizeof (file_types) / sizeof(*file_types); i++)
     elixir_file_register(file_types[i].extention, file_types[i].type_name);

   _file_parameter.class = elixir_class_request("FILE", NULL);

   return EINA_TRUE;

  on_error:
   if (tmp) elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;

   return EINA_FALSE;
}

static Eina_Bool
module_close(Elixir_Module *em, JSContext *cx)
{
   void **tmp;
   unsigned int i = 0;

   if (!em->data)
     return EINA_FALSE;

   tmp = &em->data;

   while (file_functions[i].name)
     JS_DeleteProperty(cx, *((JSObject**) tmp), file_functions[i++].name);

   for (i = 0; i < sizeof (file_types) / sizeof(*file_types); i++)
     elixir_file_unregister(file_types[i].extention, file_types[i].type_name);

   elixir_object_unregister(cx, (JSObject**) tmp);

   em->data = NULL;

   return EINA_TRUE;
}

static Elixir_Module_Api  module_api_elixir = {
   ELIXIR_MODULE_API_VERSION,
   ELIXIR_AUTH_REQUIRED,
   "file",
   "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_file = {
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
file_binding_init(void)
{
   return elixir_modules_register(&em_file);
}

void
file_binding_shutdown(void)
{
   elixir_modules_unregister(&em_file);
}

#ifndef EINA_STATIC_BUILD_FILE
EINA_MODULE_INIT(file_binding_init);
EINA_MODULE_SHUTDOWN(file_binding_shutdown);
#endif
