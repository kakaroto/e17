#include "email_private.h"

Eina_Hash *_email_contacts_hash = NULL;

static void
_email_contact_free(Email_Contact *ec)
{
   eina_stringshare_del(ec->address);
   eina_stringshare_del(ec->name);
   free(ec);
}

Email_Contact *
email_contact_new(const char *address)
{
   Email_Contact *ec;
   EINA_SAFETY_ON_NULL_RETURN_VAL(address, NULL);

   if (_email_contacts_hash)
     {
        ec = eina_hash_find(_email_contacts_hash, address);
        if (ec)
          {
             ec->refcount++;
             return ec;
          }
     }

   ec = calloc(1, sizeof(Email_Contact));
   EINA_SAFETY_ON_NULL_RETURN_VAL(ec, NULL);
   ec->address = eina_stringshare_add(address);
   ec->refcount = 1;
   if (EINA_UNLIKELY(!_email_contacts_hash))
     _email_contacts_hash = eina_hash_string_superfast_new((Eina_Free_Cb)_email_contact_free);
   eina_hash_add(_email_contacts_hash, address, ec);
   return ec;
}

Email_Contact *
email_contact_ref(Email_Contact *ec)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(ec, NULL);
   ec->refcount++;
   return ec;
}

void
email_contact_free(Email_Contact *ec)
{
   if (!ec) return;

   if (--ec->refcount) return;
   eina_hash_del_by_key(_email_contacts_hash, ec->address);
}

void
email_contact_name_set(Email_Contact *ec, const char *name)
{
   EINA_SAFETY_ON_NULL_RETURN(ec);
   eina_stringshare_replace(&ec->name, name);
}

void
email_contact_address_set(Email_Contact *ec, const char *address)
{
   EINA_SAFETY_ON_NULL_RETURN(ec);
   eina_hash_move(_email_contacts_hash, ec->address, address);
   eina_stringshare_replace(&ec->address, address);
}

const char *
email_contact_name_get(Email_Contact *ec)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(ec, NULL);
   return ec->name;
}

const char *
email_contact_address_get(Email_Contact *ec)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(ec, NULL);
   return ec->address;
}
