#include <Elementary.h>

//////////////////////////////////////////////////////////////////////////////
typedef unsigned int Data_Message_Id; // 4 billion msg's is good enough imho
typedef enum {
   DATA_MESSAGE_NONE = 0,
   DATA_MESSAGE_SENT = (1 << 0), // message was sent from user
   DATA_MESSAGE_TRASH = (1 << 1) // message is in trash
} Data_Message_Flags;
typedef time_t Data_Message_Timestamp; // time_t good enough for now

typedef struct _Data_Message Data_Message;
struct _Data_Message
{
   Data_Message_Id         id; // id of message - 0 == unknown.
   Data_Message_Id         reply_to_id; // if != 0 this is a reply to message id
   Data_Message_Flags      flags; // message flags
   Data_Message_Timestamp  timestamp; // time created
   const char             *from_to; // phone number - canonicalised +61 123 456 etc.
   const char             *body; // utf8 message (converted when received)
};

void data_init(void);
void data_shutdown(void);
void data_message_sent_store(Data_Message *reply_to, const char *contact, const char *message);
void data_message_add_callback_add(void (*func) (void *data, Data_Message *msg), const void *data);
void data_message_del_callback_del(void (*func) (void *data, Data_Message *msg), const void *data);
void data_message_add_callback_add(void (*func) (void *data, Data_Message *msg), const void *data);
void data_message_del_callback_del(void (*func) (void *data, Data_Message *msg), const void *data);
const Eina_List *data_message_all_list(void);
void data_message_trash(Data_Message *msg);
void data_message_del(Data_Message *msg);
    
//////////////////////////////////////////////////////////////////////////////
Evas_Object *
  create_message(Evas_Object *win,
                 const char *title, const char *date, const char *icon,
                 Evas_Bool is_me, const char *text, void *handle);
void
  create_main_win(void);
const char *
  find_contact_icon(const char *contact);
   
