#ifndef EXP_EB_H
#define EXP_EB_H

#include "Express.h"

#define Exp_EB_PREFERENCE_ROOT "mainprefs"
#define Exp_EB_ACCOUNTS_ROOT "accounts"

typedef enum Exp_Eb_Capabilities Exp_Eb_Capabilities;
enum Exp_Eb_Capabilities 
{
    Exp_EB_CAN_OFFLINE = 0x1,
    Exp_EB_CAN_GROUPCHAT = 0x2,
    Exp_EB_CAN_NAME_CHAT = 0x4
};

typedef enum Exp_Eb_Actions Exp_Eb_Actions;
enum Exp_Eb_Actions 
{
    Exp_EB_ACTION_BUDDY,
    Exp_EB_ACTION_GROUPCHAT,
    Exp_EB_ACTION_GROUP_USERS
};

typedef enum Exp_Eb_Components Exp_Eb_Components;
enum Exp_Eb_Component 
{
    Exp_EB_COMPONENT_LABEL,
    Exp_EB_COMPONENT_STRING,
    Exp_EB_COMPONENT_PASSWD,
    Exp_EB_COMPONENT_TOGGLE,
    Exp_EB_COMPONENT_OPTION,
    Exp_EB_COMPONENT_BUTTON
};

typedef enum Exp_Eb_Event_Dialog_Type Exp_Eb_Event_Dialog_Type;
enum Exp_Eb_Event_Dialog_Type 
{
    Exp_EB_EVENT_DIALOG_ERROR,
    Exp_EB_EVENT_DIALOG_YES_NO,
    Exp_EB_EVENT_DIALOG_LIST,
    Exp_EB_EVENT_DIALOG_TEXT,
    Exp_EB_EVENT_DIALOG_CLIENT_ERROR
};

typedef struct Exp_Eb_Event_Dialog Exp_Eb_Event_Dialog;
struct Exp_Eb_Event_Dialog
{
    Exp_Eb_Event_Dialog_Type type;
    char *tag;
    char *title;
    char *msg;

    int num_opts;
    char ** opts;
};

typedef struct Exp_Eb_Event_Buddy_Login Exp_Eb_Event_Buddy_Login;
struct Exp_Eb_Event_Buddy_Login
{
    char *local_buddy;
    char *service;
    char *buddy;
};

typedef struct Exp_Eb_Event_Buddy_Login Exp_Eb_Event_Buddy_Logout;
struct Exp_Eb_Event_Buddy_Logout
{
    char *local_buddy;
    char *service;
    char *buddy;
};

int Exp_EB_EVENT_DIALOG;
int Exp_EB_EVENT_BUDDY_LOGIN;
int Exp_EB_EVENT_BUDDY_LOGOUT;

int exp_eb_init(const char *eb_dir);
int exp_eb_connect(Exp *exp, const char *srv);
int exp_eb_shutdown(void);

void exp_eb_cmd_list_services(Exp *exp);
void exp_eb_cmd_list_contacts(Exp *exp);
void exp_eb_cmd_group_add(Exp *exp, const char *name);
void exp_eb_cmd_contact_add(Exp *exp, const char *group, const char *name);
void exp_eb_cmd_account_add(Exp *exp, const char *group, const char *contact,
                            const char *lbuddy, const char *service, 
                            const char *handle);
void exp_eb_cmd_group_del(Exp *exp, const char *name);
void exp_eb_cmd_contact_del(Exp *exp, const char *group, const char *name);
void exp_eb_cmd_account_del(Exp *exp, const char *handle, const char *service,
                            const char *buddy);
void exp_eb_cmd_contact_rename(Exp *exp, const char *group, 
                            const char *old_name, const char *new_name);
void exp_eb_cmd_contact_move(Exp *exp, const char *old_group, 
                            const char *name, const char *new_group);
void exp_eb_cmd_account_move(Exp *exp, const char *handle, 
                            const char *service, const char *bhandle, 
                            const char *group, const char *name);
void exp_eb_cmd_contact_ignore(Exp *exp, const char *group, 
                            const char *name, const char *msg);
void exp_eb_cmd_contact_unignore(Exp *exp, const char *group, 
                            const char *name);
void exp_eb_cmd_local_accounts_list(Exp *exp);
void exp_eb_cmd_local_account_add(Exp *exp, const char *uname, 
                            const char *service);
void exp_eb_cmd_local_account_del(Exp *exp, const char *uname, 
                            const char *service);
void exp_eb_cmd_away_msg_set(Exp *exp, const char *title, const char *body);
void exp_eb_cmd_away_msg_unset(Exp *exp);
void exp_eb_cmd_dialog_error_resolve(Exp *exp, const char *tag);
void exp_eb_cmd_dialog_yesno_resolve(Exp *exp, const char *tag, 
                            int response);
void exp_eb_cmd_dialog_list_resolve(Exp *exp, const char *tag,
                            const char *selection);
void exp_eb_cmd_dialog_text_resolve(Exp *exp, const char *tag,
                            const char *txt);
void exp_eb_cmd_sign_on_all(Exp *exp);
void exp_eb_cmd_sign_off_all(Exp *exp);
void exp_eb_cmd_local_account_status_set(Exp *exp, const char *handle,
                            const char *service, const char *status);
void exp_eb_cmd_message_send(Exp *exp, const char *group, const char *name,
                            const char *service, const char *buddy,
                            const char *msg);
void exp_eb_cmd_email_strip_set(Exp *exp, int strip);
void exp_eb_cmd_message_hold_set(Exp *exp, int hold);
void exp_eb_cmd_messages_held_get(Exp *exp);
void exp_eb_cmd_group_chat_join(Exp *exp, const char *handle,
                            const char *service, const char *name);
void exp_eb_cmd_group_chat_invite(Exp *exp, int id, const char *msg);
void exp_eb_cmd_group_chat_send(Exp *exp, int id, const char *msg);
void exp_eb_cmd_group_chat_close(Exp *exp, int id);
void exp_eb_cmd_group_chat_hide(Exp *exp, int id);
void exp_eb_cmd_pref_page_list(Exp *exp, const char *page);
void exp_eb_cmd_pref_value_set(Exp *exp, const char *page, const char *name,
                            const char *value);
void exp_eb_cmd_actions_list(Exp *exp);
void exp_eb_cmd_buddy_action_preform(Exp *exp, int generic, const char *name,
                            const char *account, const char *service,
                            const char *buddy);
void exp_eb_cmd_groupchat_action_preform(Exp *exp, const char *action,
                            const char *id);
void exp_eb_cmd_groupchat_user_action_preform(Exp *exp, const char *action,
                            const char *id, const char *user);
void exp_eb_cmd_group_action_preform(Exp *exp, const char *action, 
                            const char *name);
void exp_eb_cmd_contact_action_preform(Exp *exp, const char *action,
                            const char *group, const char *name);
void exp_eb_cmd_general_action_preform(Exp *exp, const char *action);
void exp_eb_data_get(Exp *exp, const char *key, const char *location);
void exp_eb_data_set(Exp *exp, const char *key, const char *value,
                            const char *location);

#endif

