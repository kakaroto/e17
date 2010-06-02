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

typedef enum _Explicit_Callback_Type
{
  EXPLICIT_CALLBACK_DONE,
  EXPLICIT_CALLBACK_PROGRESS,
  EXPLICIT_CALLBACK_CANCEL,
  EXPLICIT_CALLBACK_APPROVAL,
} Explicit_Callback_Type;

#define EXPLICIT_CANCEL_OVERWRITE  -10
#define EXPLICIT_CANCEL_OBJECT_DEL -11
#define EXPLICIT_CANCEL_NOT_STARTED -12
#define EXPLICIT_CANCEL_UNRECOGNIZED_FILE -13
#define EXPLICIT_CANCEL_LINK_DOWN -14

typedef struct _Explicit Explicit;
typedef struct _Explicit_Event_Done Explicit_Event_Done;
typedef struct _Explicit_Event_Progress Explicit_Event_Progress;
typedef struct _Explicit_Event_Cancel Explicit_Event_Cancel;
typedef struct _Explicit_Event_Approval Explicit_Event_Approval;

typedef struct _Explicit_Target Explicit_Target;

struct _Explicit_Target
{
   const char *url;
   const char *key;
};

struct _Explicit_Event_Done
{
   const Explicit_Target target;

   size_t downloaded;
   size_t size;
   int status;
};

struct _Explicit_Event_Progress
{
   const Explicit_Target target;

   size_t downloaded;
   size_t size;
};

struct _Explicit_Event_Cancel
{
   const Explicit_Target target;

   int status;
};

struct _Explicit_Event_Approval
{
   Explicit_Target source;
   Explicit_Target destination;
};

EAPI Explicit *explicit_init(const char *name);
EAPI void explicit_shutdown(Explicit *context);

#endif
