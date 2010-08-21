#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>

#include <Eet.h>
#include <jsxdrapi.h>

#include "Elixir.h"

typedef struct _Elixir_Eet_Filename Elixir_Eet_Filename;
struct _Elixir_Eet_Filename
{
   int reference;

   char *filename;
   Eet_File *eet;
};

struct _Elixir_Loader_File
{
   Elixir_Eet_Filename *file;
   char *section;

   char *content;
   char *compiled;

   unsigned int compiled_length;
   unsigned int content_length;

   Eina_Bool free_content;
   Eina_Bool free_compiled;
};

static const unsigned int _jsx_header[] = {
  JSXDR_MAGIC_SCRIPT_1,
  JSXDR_MAGIC_SCRIPT_2,
  JSXDR_MAGIC_SCRIPT_3,
  JSXDR_MAGIC_SCRIPT_4,
  JSXDR_MAGIC_SCRIPT_5,
#ifdef JSXDR_MAGIC_SCRIPT_6
  JSXDR_MAGIC_SCRIPT_6,
#endif
#ifdef JSXDR_MAGIC_SCRIPT_7
  JSXDR_MAGIC_SCRIPT_7,
#endif
#ifdef JSXDR_MAGIC_SCRIPT_8
  JSXDR_MAGIC_SCRIPT_8,
#endif
#ifdef JSXDR_MAGIC_SCRIPT_9
  JSXDR_MAGIC_SCRIPT_9,
#endif
  JSXDR_MAGIC_SCRIPT_CURRENT
};

static Eina_Array *stack = NULL;
static Eina_Hash *cache = NULL;
static Eina_List *lru = NULL;

static Eina_Bool
_elixir_eet_security_check(Eet_File *eet)
{
   const void *certificate;
   const void *signature;
   const void *sha1;
   int certificate_length;
   int signature_length;
   int sha1_length;

   signature = eet_identity_signature(eet, &signature_length);
   certificate = eet_identity_x509(eet, &certificate_length);
   sha1 = eet_identity_sha1(eet, &sha1_length);

   return elixir_security_check(sha1, sha1_length,
				signature, signature_length,
				certificate, certificate_length) == ELIXIR_CHECK_OK ? EINA_TRUE : EINA_FALSE;
}

static void
_elixir_eet_filename_free(Elixir_Eet_Filename *eef)
{
   if (eef->reference == 0)
     {
	eet_close(eef->eet);
	free(eef->filename);
	free(eef);
     }
}

/*
 * (params[0] | (top of stack)) == filename
 * params[1 | 0] == key
 * params[2 | 1] == (cipher pass)
 */
static const Elixir_Loader_File *
_elixir_eet_request(int param, const char **params)
{
   Elixir_Loader_File *result = NULL;
   Elixir_Eet_Filename *top = NULL;
   Elixir_Eet_Filename *lookup = NULL;
   Eet_File *eet = NULL;
   char *filename;
   char *section = NULL;
   char *compiled_key;
   char *content = NULL;
   char *compiled = NULL;
   const char *key;
   const char *cipher;
   Eina_Bool free_compiled = EINA_FALSE;
   Eina_Bool free_content = EINA_FALSE;
   int content_length;
   int compiled_length;
   unsigned int i;

   if (param < 1 || param > 3) return NULL;

   cipher = NULL;
   switch (param)
     {
      case 1:
	 if (stack && eina_array_count_get(stack) > 0)
	   {
	      top = eina_array_data_get(stack, eina_array_count_get(stack) - 1);
	      filename = top->filename;
	      key = params[0];
	   }
	 else
	   {
	      filename = (char*) params[0];
	      key = "elixir/main";
	   }
	 break;
      case 2:
	 filename = (char*) params[0];
	 key = params[1];
	 break;
      case 3:
	 filename = (char*) params[0];
	 key = params[1];
	 cipher = params[2];
	 break;
      default:
	 return NULL;
     }

   filename = elixir_exe_canonicalize(filename);
   if (!filename) return NULL;

   section = strdup(key);
   if (!section) goto on_error;

   eet = eet_open(filename, EET_FILE_MODE_READ);
   if (!eet) goto on_error;

   /* Use a cache to prevent useless security check. */
   lookup = eina_hash_find(cache, filename);
   if (lookup && lookup->eet == eet)
     {
	eet_close(eet);

	if (top != lookup)
	  {
	     eina_array_push(stack, lookup);

	     top = lookup;
	  }

	eet = lookup->eet;
	lru = eina_list_remove(lru, lookup);
     }
   else
     {
	/* Lookup is no longer valid, remove it from cache. */
	if (lookup)
	  {
	     lru = eina_list_remove(lru, lookup);
	     eina_hash_del(cache, filename, lookup);
	  }

	if (!_elixir_eet_security_check(eet)) goto on_error;
     }

   compiled_key = alloca(strlen(key) + 2);
   snprintf(compiled_key, strlen(key) + 2, "%sx", key);

   if (cipher)
     {
	free_content = EINA_TRUE;
	content = eet_read_cipher(eet, key, &content_length, cipher);
	if (!content) goto on_error;

	compiled = eet_read_cipher(eet, compiled_key, &compiled_length, cipher);
     }
   else
     {
	content = (char*) eet_read_direct(eet, key, &content_length);
	if (!content)
	  {
	     free_content = EINA_TRUE;
	     content = eet_read(eet, key, &content_length);
	  }
	if (!content) goto on_error;

	compiled = (char*) eet_read_direct(eet, compiled_key, &compiled_length);
	if (!compiled)
	  {
	     free_compiled = EINA_TRUE;
	     compiled = eet_read(eet, compiled_key, &compiled_length);
	  }
     }

   if (memchr(content, '\0', content_length))
     goto on_error;

   if (compiled)
     {
	for (i = 0; i < sizeof (_jsx_header) / sizeof (_jsx_header[0]); ++i)
	  if (*(unsigned int*) compiled == _jsx_header[i])
	    break;

	if (i == sizeof (_jsx_header) / sizeof (_jsx_header[0]))
	  if (free_compiled)
	    {
	       free(compiled);
	       free_compiled = EINA_FALSE;
	       compiled = NULL;
	    }
     }

   result = malloc(sizeof (Elixir_Loader_File));
   if (!result) goto on_error;

   result->content_length = content_length;
   result->content = content;
   result->free_content = free_content;

   result->compiled_length = compiled_length;
   result->compiled = compiled;
   result->free_compiled = free_compiled;

   /* Ref counting helper to find current open file. */
   if (top)
     {
	top->reference++;
	free(filename);
     }
   else
     {
	top = malloc(sizeof (Elixir_Eet_Filename) + strlen(filename));
	if (!top) goto on_error;
	top->reference = 1;
	top->filename = filename;
	top->eet = eet;

	eina_array_push(stack, top);
	eina_hash_add(cache, filename, top);
     }

   result->file = top;
   result->section = section;

   return result;

 on_error:
   if (result) free(result);
   if (content && free_content) free(content);
   if (compiled && free_compiled) free(compiled);
   if (eet) eet_close(eet);
   if (section) free(section);
   free(filename);

   return NULL;
}

static Eina_Bool
_elixir_eet_release(const Elixir_Loader_File *file)
{
   Elixir_Eet_Filename *lookup;
   Eina_List *last;

   /* LRU must never be bigger than 8 file. */
   if (eina_list_count(lru) > 8)
     {
	last = eina_list_last(lru);
	lookup = eina_list_data_get(last);

	eina_hash_del(cache, lookup->filename, lookup);

	lru = eina_list_remove_list(lru, last);
     }

   file->file->reference--;

   lookup = eina_hash_find(cache, file->file->filename);
   if (lookup == file->file)
     lru = eina_list_prepend(lru, file->file);
   else
     if (file->file->reference < 0)
       {
	  eet_close(file->file->eet);
	  free(file->file->filename);
	  free(file->file);
       }

   if (stack && eina_array_count_get(stack) > 0)
     eina_array_pop(stack);

   if (file->free_content) free(file->content);
   if (file->free_compiled) free(file->compiled);
   free(file->section);
   free((void*) file);

   return EINA_TRUE;
}

static const void *
_elixir_eet_get(const Elixir_Loader_File *file, unsigned int *length)
{
   if (length) *length = file->content_length;
   return file->content;
}

static const void *
_elixir_eet_xget(const Elixir_Loader_File *file, unsigned int *length)
{
   if (length) *length = file->compiled_length;
   return file->compiled;
}

static const char *
_elixir_eet_filename(const Elixir_Loader_File *file)
{
   return file->file->filename;
}

static const char *
_elixir_eet_section(const Elixir_Loader_File *file)
{
   return file->section;
}

static Eina_Bool
_elixir_eet_id(const Elixir_Loader_File *file, char sign[ELIXIR_SIGNATURE_SIZE])
{
   /* Fill sign with the content of the eet signature. */
   const char *signature;
   int length;
   int j;
   int i;

   signature = eet_identity_signature(file->file->eet, &length);
   if (!signature) return elixir_id_compute(sign, file->content_length, file->content);

   for (i = 0; i < length && i < (int) ELIXIR_SIGNATURE_SIZE; ++i) sign[i] = signature[i];

   if (i < (int) ELIXIR_SIGNATURE_SIZE)
     while (i < (int) ELIXIR_SIGNATURE_SIZE)
       for (j = 0; j < length && i < (int) ELIXIR_SIGNATURE_SIZE; ++i, ++j)
	 sign[i] = signature[j];
   else
     while (i < length)
       for (j = 0; i < length && j < (int) ELIXIR_SIGNATURE_SIZE; ++i, ++j)
	 sign[j] ^= signature[i];

   return EINA_TRUE;
}

static const Elixir_Loader eet_loader = {
  "eet",
  "cedric.bail@free.fr",
  1, 3,
  {
    _elixir_eet_request,
    _elixir_eet_release,
    _elixir_eet_get,
    _elixir_eet_xget,
    _elixir_eet_filename,
    _elixir_eet_id,
    _elixir_eet_section
  }
};

Eina_Bool
eet_loader_init(void)
{
   if (!elixir_loader_register(&eet_loader))
     return EINA_FALSE;

   eina_init();
   eet_init();

   stack = eina_array_new(4);
   cache = eina_hash_string_superfast_new(EINA_FREE_CB(_elixir_eet_filename_free));

   return EINA_TRUE;
}

void
eet_loader_shutdown(void)
{
   eina_hash_free(cache);
   cache = NULL;

   if (stack) eina_array_free(stack);
   stack = NULL;

   lru = eina_list_free(lru);

   eet_shutdown();
   eina_shutdown();

   elixir_loader_unregister(&eet_loader);
}

#ifndef EINA_STATIC_BUILD_EET
EINA_MODULE_INIT(eet_loader_init);
EINA_MODULE_SHUTDOWN(eet_loader_shutdown);
#endif
