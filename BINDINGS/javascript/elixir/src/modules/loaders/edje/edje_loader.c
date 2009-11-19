#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>

#include <Edje.h>

#include "Elixir.h"

struct _Elixir_Loader_File
{
   unsigned int   length;

   char *content;
   char *filename;
};

/*
 * params[0] == filename
 * params[1] == key
 */
static const Elixir_Loader_File *
_elixir_edje_request(int param, const char **params)
{
   Elixir_Loader_File *result;
   char *filename = NULL;
   char *content;

   if (param != 2) return NULL;

   filename = elixir_exe_canonicalize(params[0]);
   if (!filename) return NULL;

   content = edje_file_data_get(params[0], params[1]);
   if (!content) goto on_error;

   result = malloc(sizeof (Elixir_Loader_File) + strlen(params[0]) + strlen(params[1]) + 2);
   if (!result) goto on_error;

   result->length = strlen(content);
   result->content = content;
   result->filename = filename;

   elixir_security_set(ELIXIR_GRANTED);

   return result;

 on_error:
   if (content) free(content);
   free(filename);
   return NULL;
}

static Eina_Bool
_elixir_edje_release(const Elixir_Loader_File *file)
{
   free(file->filename);
   free(file->content);
   free((void*) file);

   return EINA_TRUE;
}

static const void *
_elixir_edje_get(const Elixir_Loader_File *file, unsigned int *length)
{
   if (length) *length = file->length;
   return file->content;
}

static const char *
_elixir_edje_filename(const Elixir_Loader_File *file)
{
   return file->filename;
}

static Eina_Bool
_elixir_edje_id(const Elixir_Loader_File *file, char sign[ELIXIR_SIGNATURE_SIZE])
{
   return elixir_id_compute(sign, file->length, file->content);
}

static const Elixir_Loader edje_loader = {
  "edje",
  "cedric.bail@free.fr",
  2, 2,
  {
    _elixir_edje_request,
    _elixir_edje_release,
    _elixir_edje_get,
    NULL,
    _elixir_edje_filename,
    _elixir_edje_id
  }
};

Eina_Bool
edje_loader_init(void)
{
   if (!elixir_loader_register(&edje_loader))
     return EINA_FALSE;

   edje_init();
   return EINA_TRUE;
}

void
edje_loader_shutdown(void)
{
   edje_shutdown();
   elixir_loader_unregister(&edje_loader);
}

#ifndef EINA_STATIC_BUILD_EDJE
EINA_MODULE_INIT(edje_loader_init);
EINA_MODULE_SHUTDOWN(edje_loader_shutdown);
#endif
