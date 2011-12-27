#include "email_private.h"

Email_Message *
email_message_new(void)
{
   return calloc(1, sizeof(Email_Message));
}

void
email_message_free(Email_Message *msg)
{
   const char *s;
   if (!msg) return;

   free(msg->content);
   free(msg->subject);
   eina_stringshare_del(msg->sender);
   EINA_LIST_FREE(msg->recipients, s)
     eina_stringshare_del(s);
   free(msg);
}

void
email_message_rcpt_add(Email_Message *msg, const char *address)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(address);

   msg->recipients = eina_list_append(msg->recipients, eina_stringshare_add(address));
}

void
email_message_rcpt_del(Email_Message *msg, const char *address)
{
   Eina_List *l, *ll;
   const char *s;
   
   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(address);

   EINA_LIST_FOREACH_SAFE(msg->recipients, l, ll, s)
     {
        if ((s == address) || (!strcmp(s, address)))
          {
             eina_stringshare_del(s);
             msg->recipients = eina_list_remove_list(msg->recipients, l);
             return;
          }
     }
}

void
email_message_sender_set(Email_Message *msg, const char *address)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(address);

   msg->sender = eina_stringshare_add(address);
}

const char *
email_message_sender_get(Email_Message *msg)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(msg, NULL);
   return msg->sender;
}

void
email_message_subject_set(Email_Message *msg, const char *subject)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(subject);

   free(msg->subject);
   msg->subject = strdup(subject);
}

void
email_message_subject_manage(Email_Message *msg, char *subject)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(subject);

   free(msg->subject);
   msg->subject = subject;
}

void
email_message_content_set(Email_Message *msg, const char *content, size_t size)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);

   free(msg->content);
   msg->content = malloc(sizeof(char) * size);
   EINA_SAFETY_ON_NULL_RETURN(msg->content);
   memcpy(msg->content, content, size);
   msg->csize = size;
}

void
email_message_content_manage(Email_Message *msg, char *content, size_t size)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);

   free(msg->content);
   msg->content = content;
   msg->csize = size;
}

void
email_message_data_set(Email_Message *msg, void *data)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   msg->data = data;
}

void *
email_message_data_get(Email_Message *msg)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(msg, NULL);
   return msg->data;
}
