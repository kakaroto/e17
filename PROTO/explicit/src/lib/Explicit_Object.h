#ifndef EXPLICIT_OBJECT_H_
# define EXPLICIT_OBJECT_H_

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EVAS_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EVAS_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif /* ! _WIN32 */

#include <Evas.h>

#include "Explicit_Context.h"

typedef Eina_Bool (*Explicit_Callback)(Explicit *context,
				       Evas_Object *obj,
				       Explicit_Callback_Type type,
				       void *event,
				       void *data);

EAPI Eina_Bool explicit_callback_add(Explicit *context, Explicit_Callback_Type type, Explicit_Callback cb, const void *data);
EAPI Eina_Bool explicit_callback_del(Explicit *context, Explicit_Callback_Type type, Explicit_Callback cb, const void *data);

EAPI Eina_Bool explicit_object_url_get(Explicit *context, Evas_Object *obj, const char *url, const char *key);
/* Download multiple files without testing them, but report progress to obj */
EAPI Eina_Bool explicit_object_list_url_get(Explicit *context, Evas_Object *obj, Eina_List *urls);

EAPI Eina_Bool explicit_object_url_cancel(Evas_Object *obj, int status);

#endif
