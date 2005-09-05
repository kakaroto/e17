#ifndef EXPRESS_H
#define EXPRESS_H

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_Evas.h>
#include <Ecore_Con.h>
#include <Edje.h>
#include <Esmart/Esmart_Container.h>
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct _Exp Exp;
typedef struct _Exp_Buddy Exp_Buddy;
typedef struct _Exp_Conversation Exp_Conversation;
typedef struct _Exp_Message Exp_Message;

struct _Exp
{
  Ecore_Evas *ee;
  Evas *evas;
  
  Evas_Object *gui;
  Evas_Object *buddy_cont;
  Exp_Buddy *user;

  Evas_List *buddies;
  Evas_List *conversations;

  char *theme_path;

  Exp_Conversation *active_conversation;

  struct 
  {
    unsigned char size : 1;
    unsigned char active_conv : 1;
  } changes;

  struct 
  {
    char *name;
    int port;
    char *cookie;

    Ecore_Con_Server *server;

    struct
    {
      char *msg;
      int len;
    } store;
  } server;
};

struct _Exp_Buddy
{
  Exp *exp;
  char *name;
  char *username;

  Evas_Object *gui;
  Evas_Object *icon;
  void *icon_data;
  int icon_w, icon_h;

  unsigned char changed : 1;
  unsigned char realized : 1;
  unsigned char show_name : 1;

  unsigned char active : 1;
  unsigned char chatting : 1;
  unsigned char new_message : 1;

  struct 
  {
    unsigned char name : 1;
    unsigned char username : 1;
    unsigned char icon : 1;
    unsigned char state : 1;
  } changes;
};

struct _Exp_Conversation
{
  Exp *exp;
  Exp_Buddy *buddy, *user;
  Evas_Object *cont;
  
  Evas_List *messages;

  double pos;

  unsigned char active : 1;
  unsigned char destroy : 1;
  unsigned char realized : 1;
  unsigned char new_message : 1;
  unsigned char changed : 1;

  struct
  {
    unsigned char size : 1;
    unsigned char active : 1;
    unsigned char pos : 1;
  } changes;
};

struct _Exp_Message
{
  Exp_Conversation *conv;

  Evas_Object *gui;
  Evas_Object *text;
  Evas_Object *icon;

  Evas_Textblock_Style *style;
  
  char *msg_text;

  unsigned char sender : 1; /* 0 == user sent msg, 1 == buddy sent msg */
  unsigned char realized : 1;
  unsigned char changed : 1;

  struct {
    unsigned char size : 1;
  } changes;
};

#include "exp_services.h"

int exp_gui_init(Exp *exp);

Exp_Conversation * exp_conversation_new(Exp_Buddy *buddy);
void               exp_conversation_free(Exp_Conversation *conv);
void               exp_conversation_update(Exp_Conversation *conv);
void               exp_conversation_activate(Exp_Conversation *conv);
void               exp_conversation_pos_set(Exp_Conversation *conv, double pos);
Exp_Conversation * exp_conversation_find_by_buddy(Exp_Buddy *buddy);
void               exp_conversation_destroy(Exp_Conversation *conv);

Exp_Message *exp_message_new(Exp_Conversation *conv, char *text, int sender);
void         exp_message_free(Exp_Message *msg);
void         exp_message_update(Exp_Message *msg);

Exp_Buddy * exp_buddy_new(Exp *exp);
void        exp_buddy_free(Exp_Buddy *buddy);
void        exp_buddy_update(Exp_Buddy *buddy);
void        exp_buddy_activate(Exp_Buddy *buddy);
void        exp_buddy_deactivate(Exp_Buddy *buddy);

void          exp_buddy_name_set(Exp_Buddy *buddy, char *name); 
const char *  exp_buddy_name_get(Exp_Buddy *buddy); 
void          exp_buddy_username_set(Exp_Buddy *buddy, char *username);
const char *  exp_buddy_username_get(Exp_Buddy *buddy);
void          exp_buddy_icon_data_set(Exp_Buddy *buddy, void *image_data,
                                      int w, int h);
void          exp_buddy_show_name_set(Exp_Buddy *buddy, int show);
Evas_Object * exp_buddy_icon_new(Exp_Buddy *buddy);


#endif
