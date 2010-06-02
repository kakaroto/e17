#ifndef EXPLICIT_PRIVATE_H_
# define  EXPLICIT_PRIVATE_H_

#include <Eet.h>
#include <Evas.h>

#include "Explicit.h"

#define EXPLICIT_REMOTE_SERVER "::1"
#define EXPLICIT_REMOTE_PORT 42781

#define ECLOSE(Func, Value)			\
  if (Value)					\
    {						\
       Func(Value);				\
       Value = NULL;				\
    }

/* Communication protocol */
typedef struct _Explicit_Request Explicit_Request;
typedef struct _Explicit_Reply Explicit_Reply;

struct _Explicit_Request
{
   int id;

   const char *url;

   Eina_Bool cancel;
};

struct _Explicit_Reply
{
   Explicit_Callback_Type type;
   int id;

   unsigned int downloaded;
   unsigned int size;

   int status;
};

extern Eet_Data_Descriptor *_explicit_cache_descriptor;
extern Eet_Data_Descriptor *_explicit_request_descriptor;
extern Eet_Data_Descriptor *_explicit_reply_descriptor;

void explicit_edd_conn_shutdown(void);
void explicit_edd_conn_init(void);

#endif

