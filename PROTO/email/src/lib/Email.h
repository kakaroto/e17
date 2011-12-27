#ifndef EMAIL_H
#define EMAIL_H

#include <Eina.h>

typedef struct Email Email;
typedef struct Email_Message Email_Message;
typedef struct Email_Contact Email_Contact;
typedef struct Email_Attachment Email_Attachment;
typedef void (*Email_Stat_Cb)(Email *, unsigned int, size_t);
typedef void (*Email_List_Cb)(Email *, Eina_List */* Email_List_Item */);
typedef void (*Email_Retr_Cb)(Email *, Eina_Binbuf *);
typedef void (*Email_Send_Cb)(Email_Message *, Eina_Bool);
typedef void (*Email_Cb)(Email *);


typedef struct
{
   unsigned int id;
   size_t size;
} Email_List_Item;

typedef enum
{
   EMAIL_OP_STAT = 1,
   EMAIL_OP_LIST,
   EMAIL_OP_RSET,
   EMAIL_OP_DELE,
   EMAIL_OP_RETR,
   EMAIL_OP_QUIT,
   EMAIL_OP_SEND,
} Email_Op;

typedef enum
{
   EMAIL_MESSAGE_CONTACT_TYPE_TO,
   EMAIL_MESSAGE_CONTACT_TYPE_CC,
   EMAIL_MESSAGE_CONTACT_TYPE_BCC
} Email_Message_Contact_Type;

extern int EMAIL_EVENT_CONNECTED;
extern int EMAIL_EVENT_DISCONNECTED;

int email_init(void);
Email *email_new(const char *username, const char *password, void *data);
void email_free(Email *e);
void email_data_set(Email *e, void *data);
void *email_data_get(Email *e);
void email_cert_add(Email *e, const char *file);

const Eina_List */* Email_Op */email_queue_get(Email *e);
Eina_Bool email_op_cancel(Email *e, unsigned int op_number);

Eina_Bool email_connect_pop3(Email *e, Eina_Bool secure, const char *addr);
Eina_Bool email_connect_smtp(Email *e, Eina_Bool secure, const char *addr, const char *from_domain);

Eina_Bool email_quit(Email *e, Email_Cb cb);
Eina_Bool email_stat(Email *e, Email_Stat_Cb cb);
Eina_Bool email_list(Email *e, Email_List_Cb cb);
Eina_Bool email_rset(Email *e, Email_Cb cb);
Eina_Bool email_delete(Email *e, unsigned int id, Email_Cb cb);
Eina_Bool email_retrieve(Email *e, unsigned int id, Email_Retr_Cb cb);


Email_Contact *email_contact_new(const char *address);
Email_Contact *email_contact_ref(Email_Contact *ec);
void email_contact_free(Email_Contact *ec);
void email_contact_name_set(Email_Contact *ec, const char *name);
void email_contact_address_set(Email_Contact *ec, const char *address);
const char *email_contact_name_get(Email_Contact *ec);
const char *email_contact_address_get(Email_Contact *ec);

Email_Attachment * email_attachment_new(const char *name, const char *content_type);
Email_Attachment * email_attachment_new_from_file(const char *filename, const char *content_type);
void email_attachment_free(Email_Attachment *at);
const char * email_attachment_name_get(Email_Attachment *at);
const char * email_attachment_content_type_get(Email_Attachment *at);
Eina_Bool email_attachment_content_set(Email_Attachment *at, const void *content, size_t size);
Eina_Bool email_attachment_content_steal(Email_Attachment *at, void *content, size_t size);
const unsigned char * email_attachment_content_get(Email_Attachment *at, size_t *size);

Email_Message * email_message_new(void);
void email_message_free(Email_Message *msg);
Eina_Bool email_message_send(Email *e, Email_Message *msg, Email_Send_Cb cb);
void email_message_contact_add(Email_Message *msg, Email_Contact *ec, Email_Message_Contact_Type type);
void email_message_contact_del(Email_Message *msg, Email_Contact *ec);
void email_message_contact_del_by_address(Email_Message *msg, const char *address);
void email_message_from_add(Email_Message *msg, Email_Contact *ec);
void email_message_from_del(Email_Message *msg, Email_Contact *ec);
void email_message_sender_set(Email_Message *msg, Email_Contact *ec);
Email_Contact * email_message_sender_get(Email_Message *msg);
void email_message_subject_set(Email_Message *msg, const char *subject);
void email_message_subject_manage(Email_Message *msg, char *subject);
void email_message_content_set(Email_Message *msg, const char *content, size_t size);
void email_message_content_manage(Email_Message *msg, char *content, size_t size);
void email_message_header_set(Email_Message *msg, const char *name, const char *value);
const char * email_message_header_get(Email_Message *msg, const char *name);
const char * email_message_charset_get(Email_Message *msg);
void email_message_charset_set(Email_Message *msg, const char *charset);
double email_message_mime_version_get(Email_Message *msg);
void email_message_mime_version_set(Email_Message *msg, double version);
void email_message_attachment_add(Email_Message *msg, Email_Attachment *at);
void email_message_attachment_del(Email_Message *msg, Email_Attachment *at);
void email_message_data_set(Email_Message *msg, const void *data);
void * email_message_data_get(Email_Message *msg);
Email *email_message_email_get(Email_Message *msg);
#endif

