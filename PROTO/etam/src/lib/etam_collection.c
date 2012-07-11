#include <Eina.h>
#include <Eet.h>
#include <Ecore.h>

#include "Etam.h"
#include "etam_private.h"

EAPI Etam_Collection *
etam_collection_open(Etam_DB *db, const char *name, ...)
{
}

EAPI void
etam_collection_callback_add(Etam_Collection *coll,
			     Etam_Collection_Op event,
			     Etam_Coll_Validate_Cb cb,
			     const void *data)
{
}

EAPI void
etam_collection_close(Etam_Collection *coll)
{
}

EAPI Eina_Iterator *
etam_collection_keys_iterator(Etam_Collection *coll)
{
}

EAPI void
etam_collection_entry_value_set(Etam_Collection *coll, const char *key, Eina_Value *value)
{
   if (eina_main_loop_is())
     {
        // Do the request inline
     }
   else
     {
        // Do it in a thread
     }
}

EAPI void
etam_collection_entry_data_set(Etam_Collection *coll, const char *key, const Etam_Data_Descriptor *desc, void *data)
{
   if (eina_main_loop_is())
     {
        // Do the request inline
     }
   else
     {
        // Do it in a thread
     }
}

EAPI void
etam_collection_entry_typed_set(Etam_Collection *coll, const char *key, ...)
{
   if (eina_main_loop_is())
     {
        // Do the request inline
     }
   else
     {
        // Do it in a thread
     }
}

EAPI void
etam_collection_entry_del(Etam_Collection *coll, const char *key)
{
   if (eina_main_loop_is())
     {
        // Do the request inline
     }
   else
     {
        // Do it in a thread
     }
}

