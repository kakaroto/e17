/*
 * vim:ts=4:cino=t0
 */

#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "exp_eb.h"

static int init_count = 0;
static char *everybody_dir = NULL;
static Ecore_Event_Handler *add = NULL, *del = NULL, *data = NULL;

static int exp_eb_cookie_get(Exp *exp);
static void exp_eb_command_send(Exp *exp, const char **msg, int params);
static void exp_eb_command_handle(Exp *exp, char **cmd);

static int exp_eb_cb_server_add(void *data, int type, void *ev);
static int exp_eb_cb_server_del(void *data, int type, void *ev);
static int exp_eb_cb_server_data(void *data, int type, void *ev);

static void exp_eb_cb_event_dialog_free(void *data, void *ev);
static void exp_eb_cb_event_buddy_login_free(void *data, void *ev);
static void exp_eb_cb_event_buddy_logout_free(void *data, void *ev);
static void exp_eb_cb_event_message_free(void *data, void *ev);

int
exp_eb_init(const char *eb_dir)
{
  if (!ecore_init()) return 0;
  if (!ecore_con_init()) return 0;

  init_count++;
  if (!add)
  {
    add = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD,
        exp_eb_cb_server_add, NULL);
    del = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL,
        exp_eb_cb_server_del, NULL);
    data = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
        exp_eb_cb_server_data, NULL);

    if (!add || !del || !data)
    {
      init_count--;
      return 0;
    }

    EXP_EB_EVENT_DIALOG = ecore_event_type_new();
    EXP_EB_EVENT_BUDDY_LOGIN = ecore_event_type_new();
    EXP_EB_EVENT_BUDDY_LOGOUT = ecore_event_type_new();
    EXP_EB_EVENT_MESSAGE = ecore_event_type_new();

    if (*eb_dir == '~')
    {
      char *home_dir;
      int len;

      home_dir = getenv("HOME");
      len = strlen(home_dir) + strlen(eb_dir);

      everybody_dir = calloc(len, sizeof(char));
      snprintf(everybody_dir, len, "%s%s", home_dir, eb_dir + 1);
    } 
    else
      eb_dir = strdup(eb_dir);
  }
  return 1;
}

int
exp_eb_shutdown(void)
{
  if (init_count > 0)
  {
    init_count--;
    if (init_count > 0) return init_count;

    if (add) ecore_event_handler_del(add);
    if (del) ecore_event_handler_del(del);
    if (data) ecore_event_handler_del(data);

    if (everybody_dir) free(everybody_dir);

    ecore_con_shutdown();
    ecore_shutdown();
  }
  return 0;
}

int
exp_eb_connect(Exp *exp, const char *srv)
{
  if (!exp_eb_cookie_get(exp))
  {
    printf("failed to get cookie\n");
    return 0;
  }

  exp->server.name = strdup(srv);
  if (!(exp->server.server = 
        ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, 
          exp->server.name, exp->server.port, exp)))
  {
    printf("Unable to connect to (%s:%d)\n", exp->server.name, exp->server.port);
    return 0;
  }
  return 1;
}

static int
exp_eb_cb_server_add(void *data, int type, void *ev)
{
  Ecore_Con_Event_Server_Add *e;
  Exp *exp;

  e = ev;
  exp = ecore_con_server_data_get(e->server);
  exp->server.server = e->server;

  ecore_con_server_send(e->server, exp->server.cookie, strlen(exp->server.cookie));

  return 1;
  data = NULL;
  type = 0;
}

static int
exp_eb_cb_server_del(void *data, int type, void *ev)
{
  Ecore_Con_Event_Server_Del *e;
  Exp *exp;

  e = ev;
  exp = ecore_con_server_data_get(e->server);

  printf("disconnected from eb\n");

  return 1;
  data = NULL;
  type = 0;
}

static int
exp_eb_cb_server_data(void *data, int type, void *ev)
{
  Ecore_Con_Event_Server_Data *e;
  Exp *exp;
  char *d, *msg;
  char ** cmds;
  int num_params = 0, i, remaining = 0;

  e = ev;
  exp = ecore_con_server_data_get(e->server);

  if (e->size == 0) return 1;

  remaining = e->size;

  /* grab any stored data and prepend it */
  if (exp->server.store.len > 0)
  {
    remaining += exp->server.store.len;

    msg = malloc(sizeof(char) * (remaining));
    memcpy(msg, exp->server.store.msg, exp->server.store.len);
    memcpy(msg + exp->server.store.len, e->data, e->size);

    exp->server.store.len = 0;
    free(exp->server.store.msg);

  }
  else
  {
    msg = malloc(sizeof(char) * (remaining));
    memcpy(msg, e->data, e->size);
  }
  d = msg;

  for ( ;; )
  {
    int done = 0, start_len = 0;
    char *start;

    start = d;
    start_len = remaining;

    num_params = d[0];
    d++;
    remaining--;

    cmds = (char **)calloc(sizeof(char *), num_params + 1);
    for (i = 0; i < num_params; i++)
    {
      int size;

      size = ((unsigned char)d[0] << 8) + (unsigned char)d[1];
      remaining -= 2;
      d += 2;

      /* store this command away till we get more data */
      if (remaining < size)
      {
        done = 1;
        exp->server.store.msg = malloc(sizeof(char) * start_len);
        memcpy(exp->server.store.msg, start, start_len);

        exp->server.store.len = start_len;
        break;
      }

      cmds[i] = calloc(size + 1, sizeof(char));
      memcpy(cmds[i], d, size);

      d += size;
      remaining -= size;
    }
    if (!done) exp_eb_command_handle(exp, cmds);

    /* cleanup */
    for (i = 0; i < num_params; i++)
      if (cmds[i]) free(cmds[i]);

    free(cmds);

    if (done || remaining == 0) break;
  }
  free(msg);

  return 1;
  data = NULL;
  type = 0;
}

static int
exp_eb_cookie_get(Exp *exp)
{
  char cookie_file[PATH_MAX];
  char cookie[9];
  FILE *cfile = NULL;
  unsigned int i;

  snprintf(cookie_file, sizeof(cookie_file), "%s/authcookie", everybody_dir);

  cfile = fopen(cookie_file, "r");
  if (!cfile) return 0;

  exp->server.port = (fgetc(cfile) << 8) + fgetc(cfile);
  for (i = 0; i < 8; i++)
    cookie[i] = fgetc(cfile);
  cookie[8] = '\0';
  fclose(cfile);

  exp->server.cookie = strdup(cookie);
  return 1;
}

static void
exp_eb_command_handle(Exp *exp, char **cmds)
{
  printf("cmd (%s)\n", cmds[0]);

  /* cookie functions */
  if (!strcmp(cmds[0], "cookie_accepted"))
  {
    exp_eb_cmd_list_services(exp);
  }
  else if (!strcmp(cmds[0], "cookie_rejected"))
  {
    /* ah, ah, ... */
  }

  /* service functions */
  else if (!strcmp(cmds[0], "list_service"))
  {
    char *service_name, *theme_colour;
    int capabilities;
    Exp_Service *service;

    service_name = cmds[1];
    theme_colour = cmds[2];
    capabilities = atoi(cmds[3]);

    service = exp_service_find(service_name);
    if (!service)
    {
        service = exp_service_new(service_name);
    }
    exp_service_colour_set(service, theme_colour);
    exp_service_capabilities_set(service, capabilities);
  }
  else if (!strcmp(cmds[0], "list_service_done"))
  {
    /* don't need to do anything */
  }
  else if (!strcmp(cmds[0], "list_service_actions"))
  {
    int count;
    count = atoi(cmds[3]);

    /* don't know what these are, eb-lite gives a count of 0 all the time
     * for them */
    if (count > 0)
      printf("%s %s %d\n", cmds[1], cmds[2], count);
  }
  else if (!strcmp(cmds[0], "list_service_states"))
  {
    int count, i;

    /* we probably need to store these to display to the use as states they
     * can set the account into */
    count = atoi(cmds[2]);
    if (count > 0)
    {
      printf("%s %d\n", cmds[1], count);
      for (i = 3; i < count + 3; i++)
        printf("\t%s\n", cmds[i]);
    }
  }
  else if (!strcmp(cmds[0], "list_services_done"))
  {
    /* ignore it ... */
  }

  /* contact functions */
  else if (!strcmp(cmds[0], "list_group"))
  {

  }
  else if (!strcmp(cmds[0], "list_contacts_done"))
  {

  }
  else if (!strcmp(cmds[0], "list_contact"))
  {

  }
  else if (!strcmp(cmds[0], "list_account"))
  {


  }

  /* adding and deleting groups/contacts/accounts */
  else if (!strcmp(cmds[0], "add_group"))
  {

  }
  else if (!strcmp(cmds[0], "add_contact")) 
  {

  }
  else if (!strcmp(cmds[0], "add_account")) 
  {

  }
  else if (!strcmp(cmds[0], "del_group")) 
  {

  }
  else if (!strcmp(cmds[0], "del_contact")) 
  {

  }
  else if (!strcmp(cmds[0], "del_account")) 
  {

  }
  else if (!strcmp(cmds[0], "rename_contact")) 
  {

  }
  else if (!strcmp(cmds[0], "move_contact")) 
  {

  }
  else if (!strcmp(cmds[0], "move_account"))
  {

  }

  /* ignore/unignore */
  else if (!strcmp(cmds[0], "ignore_contact"))
  {

  }
  else if (!strcmp(cmds[0], "unignore_contact"))
  {

  }

  /* accounts management */
  else if (!strcmp(cmds[0], "list_local_account"))
  {

  }
  else if (!strcmp(cmds[0], "list_local_accounts_done"))
  {

  }
  else if (!strcmp(cmds[0], "add_local_account"))
  {

  }
  else if (!strcmp(cmds[0], "del_local_account"))
  {

  }

  /* away message */
  else if (!strcmp(cmds[0], "set_away")) 
  {

  }
  else if (!strcmp(cmds[0], "unset_away")) 
  {

  }

  /* dialogs */
  else if (!strcmp(cmds[0], "dialog_resolved")) 
  {
    printf("dialog resolved (%s)\n", cmds[1]);

  }
  else if (!strcmp(cmds[0], "error_dialog")) 
  {
    Exp_Eb_Event_Dialog *ev;

    ev = calloc(1, sizeof(Exp_Eb_Event_Dialog));
    ev->type = EXP_EB_EVENT_DIALOG_ERROR;
    ev->tag = strdup(cmds[1]);
    ev->title = strdup(cmds[2]);
    ev->msg = strdup(cmds[3]);

    ecore_event_add(EXP_EB_EVENT_DIALOG, ev,
        exp_eb_cb_event_dialog_free, NULL);

  }
  else if (!strcmp(cmds[0], "yesno_dialog")) 
  {
    Exp_Eb_Event_Dialog *ev;

    ev = calloc(1, sizeof(Exp_Eb_Event_Dialog));
    ev->type = EXP_EB_EVENT_DIALOG_YES_NO;
    ev->tag = strdup(cmds[1]);
    ev->title = strdup(cmds[2]);
    ev->msg = strdup(cmds[3]);

    ecore_event_add(EXP_EB_EVENT_DIALOG, ev,
        exp_eb_cb_event_dialog_free, NULL);
              
    /* FIXME FAKE IT FOR NOW */
    exp_eb_cmd_dialog_yesno_resolve(exp, ev->tag, 0);

  }
  else if (!strcmp(cmds[0], "list_dialog")) 
  {
    Exp_Eb_Event_Dialog *ev;
    int i;

    ev = calloc(1, sizeof(Exp_Eb_Event_Dialog));
    ev->type = EXP_EB_EVENT_DIALOG_LIST;
    ev->tag = strdup(cmds[1]);
    ev->title = strdup(cmds[2]);
    ev->msg = strdup(cmds[3]);
    ev->num_opts = atoi(cmds[4]);
    ev->opts = (char **)malloc(sizeof(char *) * ev->num_opts);

    for(i = 0; i < ev->num_opts; i++)
      ev->opts[i] = strdup(cmds[i + 5]);

    ecore_event_add(EXP_EB_EVENT_DIALOG, ev,
        exp_eb_cb_event_dialog_free, NULL);

  } 
  else if (!strcmp(cmds[0], "text_dialog")) 
  {
    Exp_Eb_Event_Dialog *ev;

    ev = calloc(1, sizeof(Exp_Eb_Event_Dialog));
    ev->type = EXP_EB_EVENT_DIALOG_TEXT;
    ev->tag = strdup(cmds[1]);
    ev->msg = strdup(cmds[2]);

    ecore_event_add(EXP_EB_EVENT_DIALOG, ev,
        exp_eb_cb_event_dialog_free, NULL);

  }
  else if (!strcmp(cmds[0], "client_error")) 
  {
    Exp_Eb_Event_Dialog *ev;

    ev = calloc(1, sizeof(Exp_Eb_Event_Dialog));
    ev->type = EXP_EB_EVENT_DIALOG_CLIENT_ERROR;
    ev->msg = strdup(cmds[1]);

    ecore_event_add(EXP_EB_EVENT_DIALOG, ev,
        exp_eb_cb_event_dialog_free, NULL);
  }

  /* status notification */
  else if (!strcmp(cmds[0], "buddy_logout")) 
  {
    Exp_Eb_Event_Buddy_Logout *ev;

    ev = calloc(1, sizeof(Exp_Eb_Event_Buddy_Logout));
    ev->local_buddy = strdup(cmds[1]);
    ev->service = strdup(cmds[2]);
    ev->buddy = strdup(cmds[3]);

    ecore_event_add(EXP_EB_EVENT_BUDDY_LOGOUT, ev,
        exp_eb_cb_event_buddy_logout_free, NULL);

  }
  else if (!strcmp(cmds[0], "buddy_login")) 
  {
    Exp_Eb_Event_Buddy_Login *ev;

    ev = calloc(1, sizeof(Exp_Eb_Event_Buddy_Login));
    ev->local_buddy = strdup(cmds[1]);
    ev->service = strdup(cmds[2]);
    ev->buddy = strdup(cmds[3]);

    ecore_event_add(EXP_EB_EVENT_BUDDY_LOGIN, ev,
        exp_eb_cb_event_buddy_login_free, NULL);

  }
  else if (!strcmp(cmds[0], "buddy_status")) 
  {

  }
  else if (!strcmp(cmds[0], "local_account_update")) 
  {

  }

  /* sending/receiving messages */
  else if (!strcmp(cmds[0], "message_receive")) 
  {
    Exp_Eb_Event_Message *ev;

    ev = calloc(1, sizeof(Exp_Eb_Event_Message));
    ev->group = strdup(cmds[1]);
    ev->contact = strdup(cmds[2]);
    ev->local_handle = strdup(cmds[3]);
    ev->service = strdup(cmds[4]);
    ev->buddy = strdup(cmds[5]);
    ev->msg = strdup(cmds[6]);

    ecore_event_add(EXP_EB_EVENT_MESSAGE, ev,
        exp_eb_cb_event_message_free, NULL);

  }
  else if (!strcmp(cmds[0], "notify_3rdperson")) 
  {
    Exp_Eb_Event_Message *ev;

    ev = calloc(1, sizeof(Exp_Eb_Event_Message));
    ev->group = strdup(cmds[1]);
    ev->contact = strdup(cmds[2]);
    ev->msg = strdup(cmds[3]);

    ecore_event_add(EXP_EB_EVENT_MESSAGE, ev,
        exp_eb_cb_event_message_free, NULL);

  }
  else if (!strcmp(cmds[0], "message_send")) 
  {

  }

  /* queuing */
  else if (!strcmp(cmds[0], "message_hold")) 
  {

  }
  else if (!strcmp(cmds[0], "held_message")) 
  {

  }
  else if (!strcmp(cmds[0], "held_sent_message")) 
  {

  }
  else if (!strcmp(cmds[0], "held_3rdperson")) 
  {

  }
  else if (!strcmp(cmds[0], "held_messages_done")) 
  {

  }
  else if (!strcmp(cmds[0], "message_hold")) 
  {

  }
  else if (!strcmp(cmds[0], "message_waiting")) 
  {

  }

  /* group chat */
  else if (!strcmp(cmds[0], "new_group_chat")) 
  {

  }
  else if (!strcmp(cmds[0], "list_group_user")) 
  {

  }
  else if (!strcmp(cmds[0], "list_group_chat_done")) 
  {

  }
  else if (!strcmp(cmds[0], "group_chat_send")) 
  {

  }
  else if (!strcmp(cmds[0], "group_chat_recv")) 
  {

  }
  else if (!strcmp(cmds[0], "group_chat_3rdperson")) 
  {

  }
  else if (!strcmp(cmds[0], "group_chat_update")) 
  {

  }
  else if (!strcmp(cmds[0], "group_chat_joined")) 
  {

  }
  else if (!strcmp(cmds[0], "group_chat_left")) 
  {

  }
  else if (!strcmp(cmds[0], "close_group_chat")) 
  {

  }

  /* preferences */
  else if (!strcmp(cmds[0], "list_pref_page")) 
  {

  }
  else if (!strcmp(cmds[0], "list_subpages")) 
  {

  }
  else if (!strcmp(cmds[0], "list_subpage")) 
  {

  }
  else if (!strcmp(cmds[0], "list_subpages_done")) 
  {

  }
  else if (!strcmp(cmds[0], "list_components")) 
  {

  }
  else if (!strcmp(cmds[0], "list_component")) 
  {

  }
  else if (!strcmp(cmds[0], "option_data")) 
  {

  }
  else if (!strcmp(cmds[0], "list_components_done")) 
  {

  }
  else if (!strcmp(cmds[0], "list_pref_page_done")) 
  {

  }

  /* list actions */
  else if (!strcmp(cmds[0], "list_actions")) 
  {

  }
  else if (!strcmp(cmds[0], "list_actions_done")) 
  {

  }

  /* data actions */
  else if (!strcmp(cmds[0], "return_data")) 
  {

  }

  /* catch all */
  else 
    printf("Uhandled command (%s)\n", cmds[0]);
}

static void
exp_eb_command_send(Exp *exp, const char **msg, int params)
{
  int i;
  unsigned char c;

  c = params;
  ecore_con_server_send(exp->server.server, &c, sizeof(c));

  for (i = 0; i < params; i++) {
    int len;

    len = strlen(msg[i]);

    c = len >> 8;
    ecore_con_server_send(exp->server.server, &c, sizeof(c));

    c = len & 0x0f;
    ecore_con_server_send(exp->server.server, &c, sizeof(c));

    ecore_con_server_send(exp->server.server, (void *)msg[i], len);
  }
}

void
exp_eb_cmd_list_services(Exp *exp)
{
  const char *cmd[] = {"list_services", NULL};
  exp_eb_command_send(exp, cmd, 1);
}

void
exp_eb_cmd_list_contacts(Exp *exp)
{
  const char *cmd[] = {"list_contacts", NULL};
  exp_eb_command_send(exp, cmd, 1);
}

void
exp_eb_cmd_group_add(Exp *exp, const char *name)
{
  const char *cmd[] = {"add_group", NULL, NULL};
  cmd[1] = name;
  exp_eb_command_send(exp, cmd, 2);
}

void
exp_eb_cmd_contact_add(Exp *exp, const char *group, const char *name)
{
  const char *cmd[] = {"add_contact", NULL, NULL, NULL};
  cmd[1] = group;
  cmd[2] = name;
  exp_eb_command_send(exp, cmd, 3);
}

void
exp_eb_cmd_account_add(Exp *exp, const char *group, const char *contact,
    const char *lbuddy, const char *service, 
    const char *handle)
{
  const char *cmd[] = {"add_account", NULL, NULL, NULL, NULL, NULL, NULL};
  cmd[1] = group;
  cmd[2] = contact;
  cmd[3] = lbuddy;
  cmd[4] = service;
  cmd[5] = handle;
  exp_eb_command_send(exp, cmd, 6);
}

void
exp_eb_cmd_group_del(Exp *exp, const char *name)
{
  const char *cmd[] = {"del_group", NULL, NULL};
  cmd[1] = name;
  exp_eb_command_send(exp, cmd, 2);
}

void
exp_eb_cmd_contact_del(Exp *exp, const char *group, const char *name)
{
  const char *cmd[] = {"del_contact", NULL, NULL, NULL};
  cmd[1] = group;
  cmd[2] = name;
  exp_eb_command_send(exp, cmd, 3);
}

void
exp_eb_cmd_account_del(Exp *exp, const char *handle, const char *service,
    const char *buddy)
{
  const char *cmd[] = {"del_account", NULL, NULL, NULL, NULL};
  cmd[1] = handle;
  cmd[2] = service;
  cmd[3] = buddy;
  exp_eb_command_send(exp, cmd, 4);
}

void
exp_eb_cmd_contact_rename(Exp *exp, const char *group, const char *old_name,
    const char *new_name)
{
  const char *cmd[] = {"rename_contact", NULL, NULL, NULL, NULL};
  cmd[1] = group;
  cmd[2] = old_name;
  cmd[3] = new_name;
  exp_eb_command_send(exp, cmd, 4);
}

void
exp_eb_cmd_contact_move(Exp *exp, const char *old_group, const char *name,
    const char *new_group)
{
  const char *cmd[] = {"move_contact", NULL, NULL, NULL, NULL};
  cmd[1] = old_group;
  cmd[2] = name;
  cmd[3] = new_group;
  exp_eb_command_send(exp, cmd, 4);
}

void
exp_eb_cmd_account_move(Exp *exp, const char *handle, const char *service,
    const char *bhandle, const char *group,
    const char *name)
{
  const char *cmd[] = {"move_account", NULL, NULL, NULL, NULL, NULL, NULL};
  cmd[1] = handle;
  cmd[2] = service;
  cmd[3] = bhandle;
  cmd[4] = group;
  cmd[5] = name;
  exp_eb_command_send(exp, cmd, 6);
}

void
exp_eb_cmd_contact_ignore(Exp *exp, const char *group, const char *name,
    const char *msg)
{
  const char *cmd[] = {"ignore_contact", NULL, NULL, NULL, NULL};
  cmd[1] = group;
  cmd[2] = name;
  cmd[3] = msg;
  exp_eb_command_send(exp, cmd, 4);
}

void
exp_eb_cmd_contact_unignore(Exp *exp, const char *group, const char *name)
{
  const char *cmd[] = {"unignore_contact", NULL, NULL, NULL};
  cmd[1] = group;
  cmd[2] = name;
  exp_eb_command_send(exp, cmd, 3);
}

void
exp_eb_cmd_local_accounts_list(Exp *exp)
{
  const char *cmd[] = {"list_local_accounts", NULL};
  exp_eb_command_send(exp, cmd, 1);
}

void
exp_eb_cmd_local_account_add(Exp *exp, const char *uname, const char *service)
{
  const char *cmd[] = {"add_local_account", NULL, NULL, NULL};
  cmd[1] = uname;
  cmd[2] = service;
  exp_eb_command_send(exp, cmd, 3);
}

void
exp_eb_cmd_local_account_del(Exp *exp, const char *uname, const char *service)
{
  const char *cmd[] = {"del_local_account", NULL, NULL, NULL};
  cmd[1] = uname;
  cmd[2] = service;
  exp_eb_command_send(exp, cmd, 3);
}

void
exp_eb_cmd_away_msg_set(Exp *exp, const char *title, const char *body)
{
  const char *cmd[] = {"set_away", NULL, NULL, NULL};
  cmd[1] = title;
  cmd[2] = body;
  exp_eb_command_send(exp, cmd, 3);
}

void
exp_eb_cmd_away_msg_unset(Exp *exp)
{
  const char *cmd[] = {"unset_away", NULL};
  exp_eb_command_send(exp, cmd, 1);
}

void
exp_eb_cmd_dialog_error_resolve(Exp *exp, const char *tag)
{
  const char *cmd[] = {"resolve_dialog", NULL, NULL};
  cmd[1] = tag;
  exp_eb_command_send(exp, cmd, 2);
}

void
exp_eb_cmd_dialog_yesno_resolve(Exp *exp, const char *tag, int response)
{
  const char *cmd[] = {"resolve_dialog", NULL, NULL, NULL};
  cmd[1] = tag;
  cmd[2] = (response ? "1" : "0");
  exp_eb_command_send(exp, cmd, 3);
}

void
exp_eb_cmd_dialog_list_resolve(Exp *exp, const char *tag, 
    const char *selection)
{
  const char *cmd[] = {"resolve_dialog", NULL, NULL, NULL};
  cmd[1] = tag;
  cmd[2] = selection;
  exp_eb_command_send(exp, cmd, 3);
}

void
exp_eb_cmd_dialog_text_resolve(Exp *exp, const char *tag, const char *txt)
{
  const char *cmd[] = {"resolve_dialog", NULL, NULL, NULL};
  cmd[1] = tag;
  cmd[2] = txt;
  exp_eb_command_send(exp, cmd, 3);
}

void
exp_eb_cmd_sign_on_all(Exp *exp)
{
  const char *cmd[] = {"sign_on_all", NULL};
  exp_eb_command_send(exp, cmd, 1);
}

void
exp_eb_cmd_sign_off_all(Exp *exp)
{
  const char *cmd[] = {"sign_off_all", NULL};
  exp_eb_command_send(exp, cmd, 1);
}

void
exp_eb_cmd_local_account_status_set(Exp *exp, const char *handle, 
    const char *service, const char *status)
{
  const char *cmd[] = {"set_local_account_status", NULL, NULL, NULL};
  cmd[1] = handle;
  cmd[2] = service;
  cmd[3] = status;
  exp_eb_command_send(exp, cmd, 4);
}

void
exp_eb_cmd_message_send(Exp *exp, const char *group, const char *name,
    const char *service, const char *buddy,
    const char *msg)
{
  const char *cmd[] = {"message_send", NULL, NULL, NULL, NULL, NULL, NULL};
  cmd[1] = group;
  cmd[2] = name;
  cmd[3] = service;
  cmd[4] = buddy;
  cmd[5] = msg;
  exp_eb_command_send(exp, cmd, 6);
}

void
exp_eb_cmd_email_strip_set(Exp *exp, int strip)
{
  const char *cmd[] = {"html_strip", NULL, NULL};
  cmd[1] = (strip ? "1" : "0");
  exp_eb_command_send(exp, cmd, 2);
}

void
exp_eb_cmd_message_hold_set(Exp *exp, int hold)
{
  const char *cmd[] = {"message_hold", NULL, NULL};
  cmd[1] = (hold ? "1" : "0");
  exp_eb_command_send(exp, cmd, 2);
}

void
exp_eb_cmd_messages_held_get(Exp *exp)
{
  const char *cmd[] = {"get_held_messages", NULL};
  exp_eb_command_send(exp, cmd, 1);
}

void
exp_eb_cmd_group_chat_join(Exp *exp, const char *handle, 
    const char *service, const char *name)
{
  const char *cmd[] = {"join_group_chat", NULL, NULL, NULL, NULL};
  cmd[1] = handle;
  cmd[2] = service;
  cmd[3] = name;
  exp_eb_command_send(exp, cmd, 4);
}

void
exp_eb_cmd_group_chat_invite(Exp *exp, int id, const char *msg)
{
  /* FIXME ... */
  printf("WARNING: exp_eb_cmd_group_chat_invite not done\n");

  return;
  exp = NULL;
  msg = NULL;
  id = 0;
}

void
exp_eb_cmd_group_chat_send(Exp *exp, int id, const char *msg)
{
  /* FIXME ... */
  printf("WARNING: exp_eb_cmd_group_chat_send not done\n");

  return;
  exp = NULL;
  msg = NULL;
  id = 0;
}

void
exp_eb_cmd_group_chat_close(Exp *exp, int id)
{
  /* FIXME ... */
  printf("WARNING: exp_eb_cmd_group_chat_close not done\n");

  return;
  exp = NULL;
  id = 0;
}

/* experimental */
void
exp_eb_cmd_group_chat_hide(Exp *exp, int id) 
{
  /* FIXME ... */
  printf("WARNING: exp_eb_cmd_group_chat_hide not done\n");

  return;
  exp = NULL;
  id = 0;
}
/* done experimental */

void
exp_eb_cmd_pref_page_list(Exp *exp, const char *page)
{
  const char *cmd[] = {"list_pref_page", NULL, NULL};
  cmd[1] = page;
  exp_eb_command_send(exp, cmd, 2);
}

void
exp_eb_cmd_pref_value_set(Exp *exp, const char *page, const char *name,
    const char *value)
{
  const char *cmd[] = {"set_pref_value", NULL, NULL, NULL, NULL};
  cmd[1] = page;
  cmd[2] = name;
  cmd[3] = value;
  exp_eb_command_send(exp, cmd, 4);
}

void
exp_eb_cmd_actions_list(Exp *exp)
{
  const char *cmd[] = {"list_actions", NULL};
  exp_eb_command_send(exp, cmd, 1);
}

void
exp_eb_cmd_buddy_action_preform(Exp *exp, int generic, const char *name,
    const char *account, const char *service,
    const char *buddy)
{
  const char *cmd[] = {"perform_action", NULL, NULL, NULL, NULL, NULL, NULL};
  cmd[1] = (generic ? "buddy_generic" : "buddy");
  cmd[2] = name;
  cmd[3] = account;
  cmd[4] = service;
  cmd[5] = buddy;
  exp_eb_command_send(exp, cmd, 6);
}

void
exp_eb_cmd_groupchat_action_preform(Exp *exp, const char *action, 
    const char *id)
{
  const char *cmd[] = {"preform_action", "groupchat", NULL, NULL, NULL};
  cmd[2] = action;
  cmd[3] = id;
  exp_eb_command_send(exp, cmd, 4);
}

void
exp_eb_cmd_groupchat_user_action_preform(Exp *exp, const char *action, 
    const char *id, const char *user)
{
  const char *cmd[] = {"preform_action", "group_users", NULL, NULL, NULL, NULL};
  cmd[2] = action;
  cmd[3] = id;
  cmd[4] = user;
  exp_eb_command_send(exp, cmd, 5);
}

void
exp_eb_cmd_group_action_preform(Exp *exp, const char *action, const char *name)
{
  const char *cmd[] = {"preform_action", "group", NULL, NULL, NULL};
  cmd[2] = action;
  cmd[3] = name;
  exp_eb_command_send(exp, cmd, 4);
}

void
exp_eb_cmd_contact_action_preform(Exp *exp, const char *action, 
    const char *group, const char *name)
{
  const char *cmd[] = {"preform_action", "contact", NULL, NULL, NULL};
  cmd[2] = action;
  cmd[3] = group;
  cmd[4] = name;
  exp_eb_command_send(exp, cmd, 5);
}

void
exp_eb_cmd_general_action_preform(Exp *exp, const char *action)
{
  const char *cmd[] = {"preform_action", "general", NULL, NULL};
  cmd[2] = action;
  exp_eb_command_send(exp, cmd, 3);
}

void
exp_eb_data_get(Exp *exp, const char *key, const char *location)
{
  const char *cmd[] = {"get_data", NULL, NULL, NULL};
  cmd[1] = key;
  cmd[2] = location;
  exp_eb_command_send(exp, cmd, 3);
}

void
exp_eb_data_set(Exp *exp, const char *key, const char *value, 
    const char *location)
{
  const char *cmd[] = {"put_data", NULL, NULL, NULL, NULL};
  cmd[1] = key;
  cmd[2] = value;
  cmd[3] = location;
  exp_eb_command_send(exp, cmd, 4);
}

static void
exp_eb_cb_event_dialog_free(void *data, void *ev)
{
  Exp_Eb_Event_Dialog *e;

  e = ev;
  if (!e) return;

  if (e->tag) free(e->tag);
  e->tag = NULL;

  if (e->title) free(e->title);
  e->title = NULL;

  if (e->msg) free(e->msg);
  e->msg = NULL;

  if (e->opts) {
    int i = 0;
    for (i = 0; i < e->num_opts; i++) {
      if (e->opts[i]) free(e->opts[i]);
      e->opts[i] = NULL;
    }
    free(e->opts);
    e->opts = NULL;
  }
  free(e);
  e = NULL;

  return;
  data = NULL;
}

static void
exp_eb_cb_event_buddy_login_free(void *data, void *ev)
{
  Exp_Eb_Event_Buddy_Login *e;

  e = ev;
  if (!e) return;

  if (e->local_buddy) free(e->local_buddy);
  e->local_buddy = NULL;

  if (e->service) free(e->service);
  e->service = NULL;

  if (e->buddy) free(e->buddy);
  e->buddy = NULL;

  free(e);
  e = NULL;

  return;
  data = NULL;
}

static void
exp_eb_cb_event_buddy_logout_free(void *data, void *ev)
{
  Exp_Eb_Event_Buddy_Logout *e;

  e = ev;
  if (!e) return;

  if (e->local_buddy) free(e->local_buddy);
  e->local_buddy = NULL;

  if (e->service) free(e->service);
  e->service = NULL;

  if (e->buddy) free(e->buddy);
  e->buddy = NULL;

  free(e);
  e = NULL;

  return;
  data = NULL;
}

static void
exp_eb_cb_event_message_free(void *data, void *ev)
{
  Exp_Eb_Event_Message *e;

  e = ev;
  if (!e) return;

  if (e->group) free(e->group);
  e->group = NULL;

  if (e->contact) free(e->contact);
  e->contact = NULL;

  if (e->local_handle) free(e->local_handle);
  e->local_handle = NULL;

  if (e->service) free(e->service);
  e->service = NULL;

  if (e->buddy) free(e->buddy);
  e->buddy = NULL;

  if (e->msg) free(e->msg);
  e->msg = NULL;

  free(e);
  e = NULL;

  return;
  data = NULL;
}


