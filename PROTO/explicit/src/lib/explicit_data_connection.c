#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "explicit_client.h"

static int edd_init = 0;

Eet_Data_Descriptor *_explicit_request_descriptor = NULL;
Eet_Data_Descriptor *_explicit_reply_descriptor = NULL;

void
explicit_edd_conn_shutdown(void)
{
   edd_init--;
   if (edd_init != 0) return ;

   ECLOSE(eet_data_descriptor_free, _explicit_reply_descriptor);
   ECLOSE(eet_data_descriptor_free, _explicit_request_descriptor);
}

void
explicit_edd_conn_init(void)
{
   Eet_Data_Descriptor_Class eddc;

   edd_init++;
   if (edd_init != 1) return ;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Explicit_Request);
   _explicit_request_descriptor = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_request_descriptor, Explicit_Request, "id", id, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_request_descriptor, Explicit_Request, "url", url, EET_T_INLINED_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_request_descriptor, Explicit_Request, "cancel", cancel, EET_T_UCHAR);

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Explicit_Reply);
   _explicit_reply_descriptor = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_reply_descriptor, Explicit_Reply, "type", type, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_reply_descriptor, Explicit_Reply, "id", id, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_reply_descriptor, Explicit_Reply, "downloaded", downloaded, EET_T_UINT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_reply_descriptor, Explicit_Reply, "size", size, EET_T_UINT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_explicit_reply_descriptor, Explicit_Reply, "status", status, EET_T_INT);
}
