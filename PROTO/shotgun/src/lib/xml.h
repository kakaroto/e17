#ifndef SHOTGUN_XML_H
#define SHOTGUN_XML_H

#include "shotgun_private.h"

#define XML_STARTTLS "<starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>"

#ifdef __cplusplus
extern "C" {
#endif

char *xml_stream_init_create(Shotgun_Auth *auth, const char *lang, size_t *len);
Eina_Bool xml_stream_init_read(Shotgun_Auth *auth, char *xml, size_t size);
Eina_Bool xml_starttls_read(char *xml, size_t size);
char *xml_sasl_digestmd5_write(const char *sasl, size_t *len);
char *xml_sasl_write(Shotgun_Auth *auth, const char *sasl, size_t *len);
int xml_sasl_read(Shotgun_Auth *auth, char *xml, size_t size);

char *xml_iq_write_contact_add(const char *jid, const char *alias, Eina_List *groups, size_t *len);
char *xml_iq_write_contact_del(const char *user, size_t *len);
char *xml_iq_write_contact_otr_set(const char *user, Eina_Bool enable, size_t *len);
char *xml_iq_write_preset(Shotgun_Auth *auth, Shotgun_Iq_Preset p, size_t *len);
char *xml_iq_write_get_vcard(const char *to, size_t *len);
char *xml_iq_write_archive_get(const char *to, unsigned int max, size_t *len);
char *xml_iq_write_get_si(const char *from, const char *to, const char *id, size_t *len);
char *xml_iq_write_get_bytestream(const char *from, const char *to, const char *id, const char *sid, size_t *len);
Shotgun_Event_Iq *xml_iq_read(Shotgun_Auth *auth, char *xml, size_t size);
char * xml_iq_ibb_error(const char *from, const char *to, const char *id, size_t *len);
char * xml_iq_write_vcard(Shotgun_Auth *auth, size_t *len);
void xml_vcard_write(Shotgun_User_Info *info, void *vcard, Eina_Bool lcasetags);
Eina_Bool xml_iq_ping_write(Shotgun_Auth *auth);
char * xml_iq_activity_query(const char *from, const char *to, const char *id, size_t *len);

char *xml_message_write(Shotgun_Auth *auth, const char *to, const char *msg, Shotgun_Message_Status status, size_t *len);
Shotgun_Event_Message *xml_message_read(Shotgun_Auth *auth, char *xml, size_t size);


char *xml_presence_write(Shotgun_Auth *auth, size_t *len);
char *xml_presence_write_subscription(const char *jid, Eina_Bool subscribe, size_t *len);
char *xml_presence_write_subscription_answer(const char *jid, Eina_Bool subscribed, size_t *len);
Shotgun_Event_Presence *xml_presence_read(Shotgun_Auth *auth, char *xml, size_t size);

#ifdef __cplusplus
}
#endif

#endif
