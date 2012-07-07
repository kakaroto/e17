#ifdef EM_TYPEDEFS

typedef struct _Em_Gui Em_Gui;
typedef struct _Em_Gui_Conversation Em_Gui_Conversation;
typedef struct _Em_Gui_Server Em_Gui_Server;
typedef struct _Em_Gui_Channel Em_Gui_Channel;

typedef struct _Em_Gui_Text Em_Gui_Text;
typedef struct _Em_Gui_Text_Cell Em_Gui_Text_Cell;

#else
# ifndef EM_GUI_H
#  define EM_GUI_H

#include "Eina.h"

struct _Em_Gui
{
   Em_Object em_obj_inherit;

   Evas_Object *w_win;
   Evas_Object *w_tb;
   Evas_Object *w_box;
   Evas_Object *w_block;

   Elm_Genlist_Item_Class *w_chansel_itc;
   Evas_Object *w_chansel;

   Em_Gui_Text *text;

   Eina_Hash   *servers;
};

struct _Em_Gui_Text
{
   Evas_Object *tg;  // textgrid

   Eina_Array *rows;  // Each entry will hold an Eina_Inarray

   int top_row;
};

struct _Em_Gui_Text_Cell
{
   Eina_Unicode codepoint;
   int fg;
   int bg;
};

struct _Em_Gui_Conversation
{
   Em_Object em_obj_inherit;

   Evas_Object *w_frame;
   Evas_Object *w_box;
   Evas_Object *w_entry;
   Evas_Object *w_nick;
   Elm_Object_Item *w_select;
   Em_Gui_Text text;
   const char *name;
};

struct _Em_Gui_Server
{
   Em_Object em_obj_inherit;

   Evas_Object *w_frame;
   Evas_Object *w_box;
   Evas_Object *w_entry;
   Evas_Object *w_nick;
   Elm_Object_Item *w_select;
   Em_Gui_Text text;
   const char *name;

   /*
    * Entries above this point are generic and
    * must match _Em_Gui_Conversation
    */
   const char *nick;

   Eina_Hash   *channels;
   Emote_Protocol *protocol;
};

struct _Em_Gui_Channel
{
   Em_Object em_obj_inherit;

   Evas_Object *w_frame;
   Evas_Object *w_box;
   Evas_Object *w_entry;
   Evas_Object *w_nick;
   Elm_Object_Item *w_select;
   Em_Gui_Text text;
   const char *name;

   /*
    * Entries above this point are generic and
    * must match _Em_Gui_Conversation
    */
   Em_Gui_Server *server;
};

EM_INTERN Eina_Bool em_gui_init(void);
EM_INTERN Eina_Bool em_gui_shutdown(void);
EM_INTERN void em_gui_server_add(const char *server, Emote_Protocol *p);
EM_INTERN void em_gui_server_del(const char *server, Emote_Protocol *p __UNUSED__);
EM_INTERN void em_gui_channel_add(const char *server, const char *channel, Emote_Protocol *p);
EM_INTERN void em_gui_channel_del(const char *server, const char *channel, Emote_Protocol *p);
EM_INTERN void em_gui_message_add(const char *server, const char *channel, const char *user, const char *text);
EM_INTERN void em_gui_nick_update(const char *server, const char *old, const char *new);
EM_INTERN void em_gui_channel_status_update(const char *server, const char *channel, const char *user, Emote_Protocol *p, Eina_Bool joined);
EM_INTERN void em_gui_topic_show(const char *server, const char *channel, const char *user, const char *message);
EM_INTERN void em_gui_user_list_add(const char *server, const char *channel, const char *users);

# endif
#endif
