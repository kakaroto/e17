#include <Eina.h>
#include <Ecore.h>

#include "Etam.h"

#include "etam_private.h"

EAPI Etam_Request *
etam_collection_request_new(Etam_Collection *coll, ...)
{
}

EAPI void
etam_collection_request_free(Etam_Request *request)
{
}

EAPI Etam_Process *
etam_request_entry_value_get(Etam_Collection *collection, const Eina_Value_Struct_Desc *desc,
			     Etam_Entry_Value_Cb value_cb, Etam_Entry_Error_Cb error_cb, void *user_data,
			     const char *key)
{
}

EAPI Etam_Process *
etam_request_entry_data_get(Etam_Collection *collection, const Etam_Data_Descriptor *desc,
			    Etam_Entry_Data_Cb value_cb, Etam_Entry_Error_Cb error_cb, void *user_data,
			    const char *key)
{
}

EAPI Etam_Process *
etam_request_entries_value_get(Etam_Request *request, const Eina_Value_Struct_Desc *desc,
			       Etam_Entry_Value_Cb value_cb, Etam_Entry_Error_Cb error_cb, void *user_data)
{
}

EAPI Etam_Process *
etam_request_entries_data_get(Etam_Request *request, const Etam_Data_Descriptor *desc,
			      Etam_Entry_Value_Cb value_cb, Etam_Entry_Error_Cb error_cb, void *user_data)
{
}

EAPI Etam_Process *
etam_request_entries_get(Etam_Request *request,
			 Etam_Entry_Cb key_cb, Etam_Entry_Error_Cb error_cb, void *user_data)
{
}

EAPI void
etam_process_cancel(Etam_Process *process)
{
}

EAPI void
etam_request_del(Etam_Request *request)
{
}

