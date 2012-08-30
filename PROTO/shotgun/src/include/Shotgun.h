#ifndef SHOTGUN_H
#define SHOTGUN_H

#include <Eina.h>

#define SHOTGUN_DBUS_INTERFACE "org.shotgun"
#define SHOTGUN_DBUS_PATH "/org/shotgun/remote"
#define SHOTGUN_DBUS_METHOD_BASE "org.shotgun"
/**
 * DBUS API:
 *
 * @brief Disconnect Shotgun
 * void org.shotgun.core.quit(void)
 *
 * @brief Retrieve the string array of online contact JIDs (username@server.com)
 * @return An array of strings containing all online contacts from the user's contact list
 * Array<String> org.shotgun.list.get(void)
 *
 * @brief Retrieve the string array of all contact JIDs (username@server.com)
 * @return An array of strings containing all contacts from the user's contact list
 * Array<String> org.shotgun.list.get(void)
 *
 * @brief Retrieve the full status of a contact's current presence (based on priority)
 * @return The contact's status message (if set)
 * @param JID The contact's JID
 * @param st The contact's status on return
 * @param priority The contact's priority on return
 * String org.shotgun.contact.status(String JID, Shotgun_User_Status *st, int *priority)
 *
 * @brief Retrieve the display name, icon, and full status of a contact's current presence (based on priority)
 * @return The contact's display name
 * @param JID The contact's JID
 * @param icon The contact's user avatar (if set) on return
 * @param st The contact's status on return
 * @param priority The contact's priority on return
 * String org.shotgun.contact.info(String JID, const char **icon, Shotgun_User_Status *st, int *priority)
 *
 * @brief Retrieve the contact's icon's eet key
 * @return The key to use for retrieving the eet key of the icon belonging to contact represented
 * by @p JID
 * @param JID The contact's JID
 * String org.shotgun.contact.icon(String JID)
 *
 * @brief Send a message and message status to a contact
 * @return TRUE on successful send, else FALSE
 * @param JID The contact's JID
 * @param msg The message to send
 * @param st The (optional) #Shotgun_Message_Status to set along with the message
 * Bool org.shotgun.contact.send(String JID, String msg, Shotgun_Message_Status st)
 *
 * @brief Send a message and message status to a contact, echoing the message to
 * the contact's chat window
 * @return TRUE on successful send, else FALSE
 * @param JID The contact's JID
 * @param msg The message to send
 * @param st The (optional) #Shotgun_Message_Status to set along with the message
 * Bool org.shotgun.contact.send_echo(String JID, String msg, Shotgun_Message_Status st)
 * ---------------------------------------------------------------------------------------------
 * SIGNALS
 *
 * @brief Signal for new message
 * @param msg The message on return
 * @return The jid of the message's sender
 * String org.shotgun.core.new_msg(const char **msg)
 *
 * @brief Signal for new message sent by user (DOES NOT INCLUDE DBUS-SENT MESSAGES)
 * @param msg The message on return
 * @return The full jid of the message's recipient
 * String org.shotgun.core.new_msg_self(const char **msg)
 *
 * @brief Signal for when the user changes his own status
 * @param st The message on return
 * @param priority The user's priority on return
 * @return The status message (if set)
 * String org.shotgun.core.status_self(Shotgun_User_Status *st, int *priority)
*/


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

EAPI extern int SHOTGUN_EVENT_CONNECT; /* Shotgun_Auth */
EAPI extern int SHOTGUN_EVENT_CONNECTION_STATE;
EAPI extern int SHOTGUN_EVENT_DISCONNECT; /* Shotgun_Auth */
EAPI extern int SHOTGUN_EVENT_IQ; /* Shotgun_Event_Iq */
EAPI extern int SHOTGUN_EVENT_MESSAGE; /* Shotgun_Event_Message */
EAPI extern int SHOTGUN_EVENT_PRESENCE; /* Shotgun_Event_Presence */

typedef struct Shotgun_Auth Shotgun_Auth;

typedef enum
{
   SHOTGUN_IQ_EVENT_TYPE_UNKNOWN,
   SHOTGUN_IQ_EVENT_TYPE_ROSTER, /* Eina_List *Shotgun_User */
   SHOTGUN_IQ_EVENT_TYPE_INFO, /* Shotgun_User_Info */
   SHOTGUN_IQ_EVENT_TYPE_SERVER_QUERY, /* NULL */
   SHOTGUN_IQ_EVENT_TYPE_SETTINGS, /* NULL */
   SHOTGUN_IQ_EVENT_TYPE_MAILNOTIFY, /* NULL */
   SHOTGUN_IQ_EVENT_TYPE_OTR_QUERY, /* Eina_List *Shotgun_User_Setting */
   SHOTGUN_IQ_EVENT_TYPE_ARCHIVE_COLLECTION, /* Shotgun_Archive_Collection */
   SHOTGUN_IQ_EVENT_TYPE_FILE_TRANSFER,   /* Shotgun_Incoming_File */
   SHOTGUN_IQ_EVENT_TYPE_BYTESTREAM,   /* Shotgun_Incoming_File, type BYTESTREAM */ 
   SHOTGUN_IQ_EVENT_TYPE_IBB, /* Shotgun_Incoming_File, type IBB */
   SHOTGUN_IQ_EVENT_TYPE_IDLE,   /* Shotgun_Iq_Last */
   SHOTGUN_IQ_EVENT_TYPE_PING /* NULL */
} Shotgun_Iq_Event_Type;

typedef enum
{
   SHOTGUN_USER_SUBSCRIPTION_NONE,
   SHOTGUN_USER_SUBSCRIPTION_FROM, /* user receives events from you */
   SHOTGUN_USER_SUBSCRIPTION_TO, /* you receive events from user */
   SHOTGUN_USER_SUBSCRIPTION_BOTH,  /* both you and user receive events */
   SHOTGUN_USER_SUBSCRIPTION_REMOVE /* user doesn't receive events anymore */
} Shotgun_User_Subscription;

typedef enum
{
   SHOTGUN_USER_STATUS_NONE, /* unavailable */
   SHOTGUN_USER_STATUS_NORMAL,
   SHOTGUN_USER_STATUS_AWAY,
   SHOTGUN_USER_STATUS_CHAT,
   SHOTGUN_USER_STATUS_DND,
   SHOTGUN_USER_STATUS_XA /* eXtended Away */
} Shotgun_User_Status;

typedef enum
{
   SHOTGUN_MESSAGE_STATUS_NONE,
   SHOTGUN_MESSAGE_STATUS_ACTIVE,
   SHOTGUN_MESSAGE_STATUS_COMPOSING,
   SHOTGUN_MESSAGE_STATUS_PAUSED,
   SHOTGUN_MESSAGE_STATUS_INACTIVE,
   SHOTGUN_MESSAGE_STATUS_GONE
} Shotgun_Message_Status;

typedef enum
{
   SHOTGUN_CONNECTION_STATE_NONE,
   SHOTGUN_CONNECTION_STATE_TLS,
   SHOTGUN_CONNECTION_STATE_FEATURES,
   SHOTGUN_CONNECTION_STATE_SASL,
   SHOTGUN_CONNECTION_STATE_BIND,
   SHOTGUN_CONNECTION_STATE_SESSION,
   SHOTGUN_CONNECTION_STATE_FINALIZING,
   SHOTGUN_CONNECTION_STATE_CONNECTED
} Shotgun_Connection_State;

typedef enum
{
   SHOTGUN_PRESENCE_TYPE_NONE,
   SHOTGUN_PRESENCE_TYPE_SUBSCRIBE,
   SHOTGUN_PRESENCE_TYPE_UNSUBSCRIBE,
   SHOTGUN_PRESENCE_TYPE_UNAVAILABLE
} Shotgun_Presence_Type;

typedef enum
{
   SHOTGUN_FILE_UNKNOWN,
   SHOTGUN_FILE_OPEN,   /* opening file transfer */
   SHOTGUN_FILE_DATA,   /* receiving data */
   SHOTGUN_FILE_CLOSE   /* ending file transfer */
} Shotgun_File_Status;

typedef struct
{
   const char *jid;
   Eina_Bool value;
} Shotgun_User_Setting;

typedef struct
{
   const char *jid;
   const char *name; /* nickname (alias) */
   Eina_List *groups;
   Shotgun_User_Subscription subscription;
   Eina_Bool subscription_pending : 1;
   Shotgun_Auth *account;
} Shotgun_User;

typedef struct
{
   const char *jid;
   const char *full_name;
   struct
     {
        const char *type;
        const char *sha1;
        void *data;
        size_t size;
     } photo;
} Shotgun_User_Info;

typedef struct
{
   const char *jid;
   char *msg;
   Shotgun_Message_Status status;
   unsigned char otr_enabled; /* 0 for not present, 1 for newly disabled, 2 for newly enabled */
   Shotgun_Auth *account;
} Shotgun_Event_Message;

typedef struct
{
   Shotgun_Iq_Event_Type type;
   void *ev;
   Shotgun_Auth *account;
} Shotgun_Event_Iq;

typedef struct
{
   const char *name,
              *hash,
              *from,
              *id,
              *sid,
              *path,
              *data;
   size_t size; 
   size_t blocsize;
   size_t recvsize;
   Shotgun_File_Status status;
   FILE* fd;
   Shotgun_Auth *account;
} Shotgun_Incoming_File;

typedef struct
{
   unsigned int last;
   const char *message,
              *jid;
} Shotgun_Iq_Last;

typedef struct
{
   const char *jid;
   const char *photo;
   const char *description;
   int priority;
   unsigned int idle;
   double timestamp;
   Shotgun_User_Status status;
   Shotgun_Presence_Type type;
   Eina_Bool vcard : 1;

   Shotgun_Auth *account;
} Shotgun_Event_Presence;

#ifdef __cplusplus
extern "C" {
#endif

EAPI int shotgun_init(void);
EAPI Eina_Bool shotgun_connect(Shotgun_Auth *auth);
EAPI void shotgun_disconnect(Shotgun_Auth *auth);
EAPI Eina_Bool shotgun_ping_received(Shotgun_Auth *auth);
EAPI Shotgun_Auth *shotgun_new(const char *svr_name, const char *username, const char *domain);
EAPI void shotgun_ssl_verify_set(Shotgun_Auth *auth, Eina_Bool verify);
EAPI Eina_Bool shotgun_ssl_verify_get(Shotgun_Auth *auth);
EAPI void shotgun_free(Shotgun_Auth *auth);
EAPI Shotgun_Connection_State shotgun_connection_state_get(Shotgun_Auth *auth);
EAPI void shotgun_username_set(Shotgun_Auth *auth, const char *username);
EAPI const char *shotgun_username_get(Shotgun_Auth *auth);
EAPI const char *shotgun_password_get(Shotgun_Auth *auth);
EAPI void shotgun_resource_set(Shotgun_Auth *auth, const char *resource);
EAPI const char *shotgun_resource_get(Shotgun_Auth *auth);
EAPI void shotgun_domain_set(Shotgun_Auth *auth, const char *domain);
EAPI const char *shotgun_domain_get(Shotgun_Auth *auth);
EAPI void shotgun_servername_set(Shotgun_Auth *auth, const char *svr_name);
EAPI const char *shotgun_servername_get(Shotgun_Auth *auth);
EAPI const char *shotgun_jid_get(Shotgun_Auth *auth);
EAPI const char *shotgun_jid_full_get(Shotgun_Auth *auth);
EAPI void shotgun_settings_set(Shotgun_Auth *auth, void *settings);
EAPI void *shotgun_settings_get(Shotgun_Auth *auth);
EAPI void shotgun_data_set(Shotgun_Auth *auth, void *data);
EAPI void *shotgun_data_get(Shotgun_Auth *auth);
EAPI void shotgun_password_set(Shotgun_Auth *auth, const char *password);
EAPI void *shotgun_vcard_get(Shotgun_Auth *auth);
EAPI void shotgun_vcard_set(Shotgun_Auth *auth, void *data);
EAPI void shotgun_ping_delay_set(Shotgun_Auth *auth, double delay);
EAPI void shotgun_ping_timeout_set(Shotgun_Auth *auth, double timeout);
EAPI void shotgun_ping_max_attempts_set(Shotgun_Auth *auth, unsigned int max);
EAPI Eina_Bool shotgun_iq_roster_get(Shotgun_Auth *auth);
EAPI Eina_Bool shotgun_iq_contact_add(Shotgun_Auth *auth, const char *user, const char *alias, Eina_List */* const char * */groups);
EAPI Eina_Bool shotgun_iq_contact_del(Shotgun_Auth *auth, const char *user);
EAPI Eina_Bool shotgun_iq_contact_otr_set(Shotgun_Auth *auth, const char *jid, Eina_Bool enable);
EAPI Eina_Bool shotgun_iq_server_query(Shotgun_Auth *auth);
EAPI Eina_Bool shotgun_iq_gsettings_query(Shotgun_Auth *auth);
EAPI Eina_Bool shotgun_iq_vcard_get(Shotgun_Auth *auth, const char *user);
EAPI Eina_Bool shotgun_iq_archive_get(Shotgun_Auth *auth, const char *user, unsigned int max);
EAPI Eina_Bool shotgun_iq_gsettings_available(Shotgun_Auth *auth);
EAPI Eina_Bool shotgun_iq_otr_available(Shotgun_Auth *auth);
EAPI Eina_Bool shotgun_iq_otr_set(Shotgun_Auth *auth, Eina_Bool enable);
EAPI Eina_Bool shotgun_iq_otr_get(Shotgun_Auth *auth);
EAPI void shotgun_iq_gsettings_archiving_set(Shotgun_Auth *auth, Eina_Bool enable);
EAPI Eina_Bool shotgun_iq_gsettings_archiving_get(Shotgun_Auth *auth);
EAPI void shotgun_iq_gsettings_mailnotify_set(Shotgun_Auth *auth, Eina_Bool enable);
EAPI Eina_Bool shotgun_iq_gsettings_mailnotify_get(Shotgun_Auth *auth);
EAPI void shotgun_iq_gsettings_mailnotify_ping(Shotgun_Auth *auth);
EAPI Eina_Bool shotgun_iq_ibb_error(Shotgun_Event_Iq *ev);
EAPI Eina_Bool shotgun_iq_vcard_send(Shotgun_Auth *auth);
EAPI Eina_Bool shotgun_message_send(Shotgun_Auth *auth, const char *to, const char *msg, Shotgun_Message_Status status);
EAPI Shotgun_User_Status shotgun_presence_status_get(Shotgun_Auth *auth);
EAPI void shotgun_presence_status_set(Shotgun_Auth *auth, Shotgun_User_Status status);
EAPI int shotgun_presence_priority_get(Shotgun_Auth *auth);
EAPI void shotgun_presence_priority_set(Shotgun_Auth *auth, int priority);
EAPI const char *shotgun_presence_desc_get(Shotgun_Auth *auth);
EAPI void shotgun_presence_desc_set(Shotgun_Auth *auth, const char *desc);
EAPI void shotgun_presence_desc_manage(Shotgun_Auth *auth, const char *desc);
EAPI void shotgun_presence_set(Shotgun_Auth *auth, Shotgun_User_Status st, const char *desc, int priority);
EAPI const char *shotgun_presence_get(Shotgun_Auth *auth, Shotgun_User_Status *st, int *priority);
EAPI Eina_Bool shotgun_presence_send(Shotgun_Auth *auth);
EAPI Eina_Bool shotgun_presence_subscription_set(Shotgun_Auth *auth, const char *jid, Eina_Bool subscribe);
EAPI Eina_Bool shotgun_presence_subscription_answer_set(Shotgun_Auth *auth, const char *jid, Eina_Bool subscribed);
EAPI void shotgun_event_message_free(Shotgun_Event_Message *msg);
EAPI void shotgun_event_presence_free(Shotgun_Event_Presence *pres);
EAPI void shotgun_user_info_free(Shotgun_User_Info *info);
EAPI void shotgun_user_free(Shotgun_User *user);
EAPI void shotgun_user_setting_free(Shotgun_User_Setting *sus);
EAPI void shotgun_incoming_file_free(Shotgun_Incoming_File *file);
EAPI void shotgun_iq_last_free(Shotgun_Iq_Last *last);

#ifdef __cplusplus
}
#endif

#endif
