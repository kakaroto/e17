#ifndef EXPLICIT_CONTEXT_H_
# define EXPLICIT_CONTEXT_H_

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

#include <Eina.h>

typedef struct _Explicit_Context Explicit_Context;
typedef struct _Explicit_Request Explicit_Request;

EAPI Explicit_Context *explicit_init(const char *name);
EAPI void explicit_shutdown(Explicit_Context *context);

EAPI void explicit_rewrite_add(const char *match,
			       Rewrite_Cb *rewrite,
			       const void *data);
EAPI void explicit_rewrite_del(const char *match,
			       Rewrite_Cb *rewrite,
			       const void *data);

EAPI void explicit_context_rewrite_add(Explicit_Context *context,
				       const char *match,
				       Rewrite_Cb *rewrite,
				       const void *data);
EAPI void explicit_context_rewrite_del(Explicit_Context *context,
				       const char *match,
				       Rewrite_Cb *rewrite,
				       const void *data);

EAPI Explicit_Request explicit_download_request(const char *uri,
						Progress_Cb progress,
						Done_Cb done,
						Error_Cb error,
						const void *data);
EAPI void explicit_download_cancel(Explicit_Request *request);

#endif
