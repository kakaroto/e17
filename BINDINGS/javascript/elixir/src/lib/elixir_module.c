#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef __GNUC__
# ifndef __USE_GNU
#  define __USE_GNU
# endif
# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# ifndef _BSD_SOURCE
#  define _BSD_SOURCE
# endif
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <dlfcn.h>

#include <eina_hash.h>
#include <eina_module.h>

#include "Elixir.h"
#include "elixir_private.h"

static Eina_Hash *modules_bindings = NULL;
static Eina_Hash *modules_security = NULL;
static Eina_Hash *modules_loader = NULL;

static Eina_Array *eina;

static Eina_List *loaders = NULL;
static Eina_List *security = NULL;

static elixir_security_level_t current = ELIXIR_AUTH_REQUIRED;

#ifdef EINA_STATIC_BUILD_DIR
Eina_Bool dir_binding_init(void);
void dir_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_ECORE
Eina_Bool ecore_binding_init(void);
void ecore_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_ECORE_CON
Eina_Bool ecore_con_binding_init(void);
void ecore_con_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_ECORE_DOWNLOAD
Eina_Bool ecore_download_binding_init(void);
void ecore_download_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_ECORE_EVAS
Eina_Bool ecore_evas_binding_init(void);
void ecore_evas_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_ECORE_JOB
Eina_Bool ecore_job_binding_init(void);
void ecore_job_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_EDJE
Eina_Bool edje_binding_init(void);
void edje_binding_shutdown(void);

Eina_Bool edje_loader_init(void);
void edje_loader_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_ELIXIR
Eina_Bool elixir_binding_init(void);
void elixir_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_ETK
Eina_Bool etk_binding_init(void);
void etk_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_EVAS
Eina_Bool evas_binding_init(void);
void evas_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_FILE
Eina_Bool file_binding_init(void);
void file_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_MIX
Eina_Bool mixer_binding_init(void);
void mixer_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_SQLITE3
Eina_Bool sqlite_binding_init(void);
void sqlite_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_ECORE_CONFIG
#endif

#ifdef EINA_STATIC_BUILD_ECORE_FILE
#endif

#ifdef EINA_STATIC_BUILD_ECORE_IPC
#endif

#ifdef EINA_STATIC_BUILD_EMOTION
Eina_Bool emotion_binding_init(void);
void emotion_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_EET
Eina_Bool eet_loader_init(void);
void eet_loader_shutdown(void);
Eina_Bool eet_binding_init(void);
void eet_binding_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_TEXT
Eina_Bool text_loader_init(void);
void text_loader_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_CMP
Eina_Bool cmp_security_init(void);
void cmp_security_shutdown(void);
#endif

#ifdef EINA_STATIC_BUILD_LET
Eina_Bool let_security_init(void);
void let_security_shutdown(void);
#endif

typedef struct _Elixir_Modules_Static Elixir_Modules_Static;
struct _Elixir_Modules_Static
{
   Eina_Module_Init init;
   Eina_Module_Shutdown shutdown;
};

static Elixir_Modules_Static ems[] = {
#ifdef EINA_STATIC_BUILD_DIR
  { dir_binding_init, dir_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_ECORE
  { ecore_binding_init, ecore_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_ECORE_CON
  { ecore_con_binding_init, ecore_con_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_ECORE_DOWNLOAD
  { ecore_download_binding_init, ecore_download_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_ECORE_EVAS
  { ecore_evas_binding_init, ecore_evas_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_ECORE_JOB
  { ecore_job_binding_init, ecore_job_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_EDJE
  { edje_binding_init, edje_binding_shutdown },
  { edje_loader_init, edje_loader_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_ELIXIR
  { elixir_binding_init, elixir_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_ETK
  { etk_binding_init, etk_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_EVAS
  { evas_binding_init, evas_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_FILE
  { file_binding_init, file_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_MIX
  { mixer_binding_init, mixer_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_SQLITE3
  { sqlite_binding_init, sqlite_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_ECORE_CONFIG
#endif
#ifdef EINA_STATIC_BUILD_ECORE_FILE
#endif
#ifdef EINA_STATIC_BUILD_ECORE_IPC
#endif
#ifdef EINA_STATIC_BUILD_EMOTION
  { emotion_binding_init, emotion_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_EET
  { eet_loader_init, eet_loader_shutdown },
  { eet_binding_init, eet_binding_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_TEXT
  { text_loader_init, text_loader_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_CMP
  { cmp_security_init, cmp_security_shutdown },
#endif
#ifdef EINA_STATIC_BUILD_LET
  { let_security_init, let_security_shutdown },
#endif
  { NULL, NULL }
};

void
elixir_security_set(elixir_security_level_t level)
{
   if (level < current)
     current = level;
}

elixir_security_level_t
elixir_security_get(void)
{
   return current;
}

void
elixir_modules_init(void)
{
   char *path;
   int i;

   eina_init();

   modules_bindings = eina_hash_string_superfast_new(NULL);
   modules_security = eina_hash_string_superfast_new(NULL);
   modules_loader = eina_hash_string_superfast_new(NULL);

   eina = eina_module_list_get(NULL, PACKAGE_MODULE_DIR "/elixir/", 1, NULL, NULL);

   path = eina_module_environment_path_get("HOME", "/.elixir/");
   eina = eina_module_list_get(eina, path, 0, NULL, NULL);
   if (path) free(path);

   path = eina_module_environment_path_get("ELIXIR_MODULES_BINDINGS_DIR", "/elixir/");
   eina = eina_module_list_get(eina, path, 0, NULL, NULL);
   if (path) free(path);

   path = eina_module_symbol_path_get(elixir_modules_init, "/elixir/");
   eina = eina_module_list_get(eina, path, 0, NULL, NULL);
   if (path) free(path);

   /* Loading dynamic module first make static one overloadable. */
   eina_module_list_load(eina);

   for (i = 0; ems[i].init; ++i)
     ems[i].init();
}

Elixir_Module*
elixir_modules_find(const char* name)
{
   Elixir_Module *module;

   module = eina_hash_find(modules_bindings, name);
   elixir_debug_print("looking for bindings: %s => %p.", name, module);
   return module;
}

int
elixir_modules_load(Elixir_Module* em, JSContext* cx, JSObject* root)
{
   if (!em) return -1;

   em->func.open(em, cx, root);
   return 0;
}

int
elixir_modules_unload(Elixir_Module* em, JSContext* cx)
{
   if (!em)
     return -1;

   if (em->func.reset)
     {
        em->func.reset(em, cx);

        return 0;
     }

   if (em->func.close)
     em->func.close(em, cx);

   return 0;
}

int
elixir_modules_shutdown(void)
{
   int i;

   for (i = 0; ems[i].init; ++i)
     ems[i].shutdown();

   eina_module_list_unload(eina);

   eina_hash_free(modules_bindings);
   modules_bindings = NULL;

   eina_hash_free(modules_security);
   modules_security = NULL;

   eina_hash_free(modules_loader);
   modules_loader = NULL;

   eina_shutdown();

   return 0;
}

static Eina_Bool
_elixir_register(Eina_Hash *hash, const char *key, const void *data, const char *desc)
{
   if (eina_hash_find(hash, key))
     return EINA_FALSE;

   elixir_debug_print("Registering %s: '%s'.", desc, key);
   return eina_hash_direct_add(hash, key, data);
}
static Eina_Bool
_elixir_unregister(Eina_Hash *hash, const char *key, const void *data, const char *desc)
{
   elixir_debug_print("Unregistering %s: '%s'.", desc, key);
   return eina_hash_del(hash, key, data);
}

EAPI Eina_Bool
elixir_modules_register(const Elixir_Module *em)
{
   if (em->api->security > current)
     return EINA_FALSE;
   return _elixir_register(modules_bindings, em->api->name, em, "bindings");
}

EAPI Eina_Bool
elixir_modules_unregister(const Elixir_Module *em)
{
   return _elixir_unregister(modules_bindings, em->api->name, em, "bindings");
}

EAPI Eina_Bool
elixir_loader_register(const Elixir_Loader *el)
{
   return _elixir_register(modules_loader, el->name, el, "loader");
}

EAPI Eina_Bool
elixir_loader_unregister(const Elixir_Loader *el)
{
   return _elixir_unregister(modules_loader, el->name, el, "loader");
}

EAPI Eina_Bool
elixir_security_register(const Elixir_Security *es)
{
   return _elixir_register(modules_security, es->name, es, "security");
}

EAPI Eina_Bool
elixir_security_unregister(const Elixir_Security *es)
{
   return _elixir_unregister(modules_security, es->name, es, "security");
}

EAPI int
elixir_loader_init(int length, const char **authorized)
{
   int i;

   loaders = eina_list_free(loaders);

   for (i = 0; i < length; ++i)
     {
	const Elixir_Loader *el;

	el = eina_hash_find(modules_loader, authorized[i]);
	if (!el) continue;

	loaders = eina_list_append(loaders, el);
     }

   return 1;
}

EAPI int
elixir_loader_shutdown(void)
{
   loaders = eina_list_free(loaders);

   return 1;
}

EAPI int
elixir_security_init(int length, const char **authorized)
{
   int i;

   security = eina_list_free(security);

   for (i = 0; i < length; ++i)
     {
	const Elixir_Security *es;

	es = eina_hash_find(modules_security, authorized[i]);
	if (!es) continue;

	security = eina_list_append(security, es);
     }

   return 1;
}

EAPI int
elixir_security_shutdown(void)
{
   security = eina_list_free(security);

   return 1;
}

EAPI Elixir_Loaded_File*
elixir_loader_load(int param, const char **params)
{
   const Elixir_Loader_File *load;
   Elixir_Loaded_File *result;
   const Elixir_Loader *el;
   Eina_List *it;

   EINA_LIST_FOREACH(loaders, it, el)
     {
	elixir_debug_print("Trying: '%s' with '%s' [%i < %i < %i].",
		el->name, params[0], el->min_param, param, el->max_param);
	if (param >= el->min_param
	    && param <= el->max_param)
	  {
	     load = el->func.request(param, params);
	     if (!load) continue ;

	     result = malloc(sizeof (Elixir_Loaded_File));
	     if (!result) continue ;

	     result->loader = el;
	     result->file = load;

	     return result;
	  }
     }

   elixir_debug_print("File `%s` not opened.", params[0]);
   return NULL;
}

EAPI const char *
elixir_loader_filename(const Elixir_Loaded_File *load)
{
   return load->loader->func.filename(load->file);
}

EAPI const char *
elixir_loader_section(const Elixir_Loaded_File *load)
{
   if (load->loader->func.section) {
     return load->loader->func.section(load->file);
   } else {
     return NULL;
   }
}

EAPI void
elixir_loader_unload(const Elixir_Loaded_File *load)
{
   load->loader->func.release(load->file);
   free((void*) load);
}

EAPI Eina_Bool
elixir_loader_id(const Elixir_Loaded_File *load, char signature[ELIXIR_SIGNATURE_SIZE])
{
   return load->loader->func.id(load->file, signature);
}

EAPI const void *
elixir_loader_content(const Elixir_Loaded_File *load, unsigned int *size)
{
   return load->loader->func.get(load->file, size);
}

EAPI const void *
elixir_loader_compiled(const Elixir_Loaded_File *load, unsigned int *size)
{
   if (load->loader->func.xget)
     return load->loader->func.xget(load->file, size);
   return NULL;
}

EAPI elixir_check_validity_t
elixir_security_check(const char *sha1, unsigned int sha1_length,
		      const void *signature, unsigned int signature_length,
		      const void *certificate, unsigned int certificate_length)
{
   const Elixir_Security *es;
   Eina_List *it;
   elixir_check_validity_t tmp;

   EINA_LIST_FOREACH(security, it, es)
     {
	tmp = es->func.check(sha1, sha1_length, signature, signature_length, certificate, certificate_length);
	switch (tmp)
	  {
	   case ELIXIR_CHECK_OK:
	   case ELIXIR_CHECK_NOK:
	      return tmp;
	   case ELIXIR_CHECK_DUNO:
	      break;
	  }
     }

   return ELIXIR_CHECK_DUNO;
}

