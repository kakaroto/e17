#include "email_private.h"

#define MESSAGE_BOUNDARY "ASDF8AYWE0G8H31_//123R"

static Eina_Bool
_email_message_headers(const Eina_Hash *h __UNUSED__, const char *key, const char *data, Eina_Strbuf *buf)
{
   if (!strcasecmp(key, "content-type")) return EINA_TRUE;
   eina_strbuf_append_printf(buf, "%s: %s\r\n", key, data);
   return EINA_TRUE;
}

Eina_Binbuf *
email_message_serialize(Email_Message *msg)
{
   Eina_Strbuf *buf;
   Eina_List *l;
   Email_Contact *ec;
/*
   From: Michael Blumenkrantz <michael.blumenkrantz@gmail.com>
   To: Michael Blumenkrantz <michael.blumenkrantz@gmail.com>
   Subject: test
   Message-ID: <20110701200521.12e0b66c@darc.ath.cx>
   X-Mailer: Claws Mail 3.7.9 (GTK+ 2.24.3; i686-pc-linux-gnu)
   Mime-Version: 1.0
   Content-Type: multipart/mixed; boundary="MP_/6MDEYc2h_pXILCyZXK5yi=w"

   --MP_/6MDEYc2h_pXILCyZXK5yi=w
   Content-Type: text/plain; charset=US-ASCII
   Content-Transfer-Encoding: 8bit
   Content-Disposition: inline

   testsetstst
   --MP_/6MDEYc2h_pXILCyZXK5yi=w
   Content-Type: application/x-gzip
   Content-Transfer-Encoding: base64
   Content-Disposition: attachment; filename=azy_dl_up.tar.gz
*/

   buf = eina_strbuf_new();
   EINA_LIST_FOREACH(msg->from, l, ec)
     {
        if (ec->name)
          eina_strbuf_append_printf(buf, "From: %s <%s>%s", ec->name, ec->address, l->next ? "," : "\r\n");
        else
          eina_strbuf_append_printf(buf, "From: %s%s", ec->address, l->next ? "," : "\r\n");
     }
   if (msg->sender)
     {
        ec = msg->sender;
        if (ec->name)
          eina_strbuf_append_printf(buf, "Sender: %s <%s>\r\n", ec->name, ec->address);
        else
          eina_strbuf_append_printf(buf, "Sender: %s\r\n", ec->address);
     }
   EINA_LIST_FOREACH(msg->recipients, l, ec)
     {
        const char *type;
        switch (ec->type)
          {
           case EMAIL_MESSAGE_CONTACT_TYPE_TO:
             type = "To";
             break;
           case EMAIL_MESSAGE_CONTACT_TYPE_CC:
             type = "Cc";
             break;
           case EMAIL_MESSAGE_CONTACT_TYPE_BCC:
             type = "Bcc";
           default:
             break;
          }
        if (ec->name)
          eina_strbuf_append_printf(buf, "%s: %s <%s>%s", type, ec->name, ec->address, l->next ? "," : "\r\n");
        else
          eina_strbuf_append_printf(buf, "%s: %s%s", type, ec->address, l->next ? "," : "\r\n");
     }
   if (msg->subject) eina_strbuf_append_printf(buf, "Subject: %s\r\n", msg->subject);
   if (msg->headers) eina_hash_foreach(msg->headers, (Eina_Hash_Foreach)_email_message_headers, buf);
   if (msg->mimeversion < 1.0) msg->mimeversion = 1.0;
   eina_strbuf_append_printf(buf, "MIME-Version: %g\r\n", msg->mimeversion);
   if (msg->attachments)
     eina_strbuf_append_printf(buf, "Content-Type: multipart/mixed; boundary=\""MESSAGE_BOUNDARY"\"\r\n\r\n");
   else
     eina_strbuf_append_printf(buf, "Content-Type: text/plain; charset=%s\r\n\r\n", msg->charset ?: "US-ASCII");
   if (msg->attachments)
     {
        Email_Attachment *at;
        eina_strbuf_append(buf, MESSAGE_BOUNDARY "\r\n");
        if (msg->content)
          {
             eina_strbuf_append_printf(buf, "Content-Type: text/plain; charset=%s\r\n\r\n", msg->charset ?: "US-ASCII");
             eina_strbuf_append(buf, "Content-Disposition: inline\r\n\r\n");
             eina_strbuf_append(buf, msg->content);
             eina_strbuf_append(buf, "\r\n\r\n");
          }
        EINA_LIST_FOREACH(msg->attachments, l, at)
          {
             char *b64;
             int len;
             eina_strbuf_append(buf, MESSAGE_BOUNDARY "\r\n");
             eina_strbuf_append_printf(buf, "Content-Type: %s;\r\n", at->content_type);
             eina_strbuf_append_printf(buf, "Content-Transfer-Encoding: base64;\r\n"
                                            "Content-Disposition: attachment; filename=%s\r\n\r\n", at->name);
             b64 = email_base64_encode(at->content, (double)at->csize, &len);
             if (b64)
               eina_strbuf_append_length(buf, b64, len);
             eina_strbuf_append(buf, "\r\n");
          }
        eina_strbuf_append(buf, MESSAGE_BOUNDARY "\r\n");
     }
   else
     eina_strbuf_append(buf, msg->content);
   eina_strbuf_append(buf, "\r\n.\r\n");
   return buf;
}

Email_Message *
email_message_new(void)
{
   Email_Message *msg;
   msg = calloc(1, sizeof(Email_Message));
   if (!msg) return NULL;
   msg->mimeversion = 1.0;
   msg->charset = eina_stringshare_add("US-ASCII");
   return msg;
}

void
email_message_free(Email_Message *msg)
{
   Email_Contact *ec;
   if (!msg) return;

   free(msg->content);
   free(msg->subject);
   EINA_LIST_FREE(msg->from, ec)
     email_contact_free(ec);
   email_contact_free(msg->sender);
   EINA_LIST_FREE(msg->recipients, ec)
     email_contact_free(ec);
   eina_stringshare_del(msg->charset);
   free(msg);
}

Eina_Bool
email_message_send(Email *e, Email_Message *msg, Email_Send_Cb cb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(msg, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->state != EMAIL_STATE_CONNECTED, EINA_FALSE);
   if (!msg->recipients) return EINA_FALSE;

   msg->owner = e;
   e->op_ids = eina_list_append(e->op_ids, msg);
   e->cbs = eina_list_append(e->cbs, cb);
   e->ops = eina_list_append(e->ops, (uintptr_t*)EMAIL_OP_SEND);
   if (!e->current) send_smtp(e);
   return EINA_TRUE;
}

void
email_message_contact_add(Email_Message *msg, Email_Contact *ec, Email_Message_Contact_Type type)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(ec);

   ec->type = type;

   msg->recipients = eina_list_append(msg->recipients, email_contact_ref(ec));
}

void
email_message_contact_del(Email_Message *msg, Email_Contact *ec)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(ec);
   if (!msg->recipients) return;

   msg->recipients = eina_list_remove(msg->recipients, ec);
   email_contact_free(ec);
}

void
email_message_contact_del_by_address(Email_Message *msg, const char *address)
{
   Email_Contact *ec;

   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(address);
   EINA_SAFETY_ON_TRUE_RETURN(!address[0]);
   if (!msg->recipients) return;

   ec = eina_hash_find(_email_contacts_hash, address);
   if (!ec) return;
   msg->recipients = eina_list_remove(msg->recipients, ec);
   email_contact_free(ec);
}

void
email_message_from_add(Email_Message *msg, Email_Contact *ec)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(ec);
   if (!eina_list_data_find(msg->from, ec))
     msg->from = eina_list_append(msg->from, email_contact_ref(ec));
}

void
email_message_from_del(Email_Message *msg, Email_Contact *ec)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(ec);
   msg->from = eina_list_remove(msg->from, ec);
   email_contact_free(ec);
}

void
email_message_sender_set(Email_Message *msg, Email_Contact *ec)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   if (ec) email_contact_ref(ec);
   if (msg->sender) email_contact_free(msg->sender);
   msg->sender = ec;
}

Email_Contact *
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
   msg->content = malloc(sizeof(char) * size + 1);
   EINA_SAFETY_ON_NULL_RETURN(msg->content);
   strncpy(msg->content, content, size);
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
email_message_header_set(Email_Message *msg, const char *name, const char *value)
{
   char *old;

   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(name);

   if (!msg->headers) msg->headers = eina_hash_string_superfast_new(free);

   if (value)
     {
        old = eina_hash_set(msg->headers, name, strdup(value));
        free(old);
     }
   else
     eina_hash_del_by_key(msg->headers, name);
}

const char *
email_message_header_get(Email_Message *msg, const char *name)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(msg, NULL);
   if (!msg->headers) return NULL;
   return eina_hash_find(msg->headers, name);
}

const char *
email_message_charset_get(Email_Message *msg)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(msg, NULL);
   return msg->charset;
}

void
email_message_charset_set(Email_Message *msg, const char *charset)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   eina_stringshare_replace(&msg->charset, charset);
}

double
email_message_mime_version_get(Email_Message *msg)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(msg, 0.0);
   return msg->mimeversion;
}

void
email_message_mime_version_set(Email_Message *msg, double version)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   msg->mimeversion = version;
}

void
email_message_attachment_add(Email_Message *msg, Email_Attachment *at)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(at);
   msg->attachments = eina_list_append(msg->attachments, at);
}

void
email_message_attachment_del(Email_Message *msg, Email_Attachment *at)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   EINA_SAFETY_ON_NULL_RETURN(at);
   msg->attachments = eina_list_remove(msg->attachments, at);
}

void
email_message_data_set(Email_Message *msg, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN(msg);
   msg->data = (void*)data;
}

void *
email_message_data_get(Email_Message *msg)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(msg, NULL);
   return msg->data;
}

Email *
email_message_email_get(Email_Message *msg)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(msg, NULL);
   return msg->owner;
}
