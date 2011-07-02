#include <Ecore_Con.h>
#include <Efreet.h>
#include <Ecore_File.h>
#include "elsa.h"
#include "../event/elsa_event.h"

static void _elsa_server_scan_desktops_file(const char *path);
static void _elsa_server_scan_desktops(const char *dir);
static void _elsa_server_init_desktops();
static const char *_elsa_server_find_command(const char *session);
static Eina_Bool _elsa_server_add(void *data, int type, void *event);
static Eina_Bool _elsa_server_del(void *data, int type, void *event);
static Eina_Bool _elsa_server_data(void *data, int type, void *event);

Ecore_Con_Server *_elsa_server = NULL;
Eina_List *_handlers = NULL;
Eina_List *_xsessions = NULL;

static Eina_Bool
_elsa_server_add(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Con_Event_Client_Add *ev;
   Elsa_Event eev;
   ev = event;
   void *enc;
   int size;

   fprintf(stderr, PACKAGE": server client connected\n");

   eev.type = ELSA_EVENT_XSESSIONS;
   if (elsa_config->xsessions)
     {
        eev.event.xsessions.xsessions = _xsessions;
        enc = elsa_event_encode(&eev, &size);
        ecore_con_client_send(ev->client, enc, size);
     }

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_elsa_server_del(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Con_Event_Client_Del *ev;
   ev = event;
   fprintf(stderr, PACKAGE": server client disconnected\n");

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_elsa_server_data(void *data __UNUSED__, int type __UNUSED__, void *event)
{

   Ecore_Con_Event_Client_Data *ev;
   Elsa_Event *eev, neev;
   int size;
   void *enc;

   ev = event;
   eev = elsa_event_decode(ev->data, ev->size);
   if (eev->type == ELSA_EVENT_AUTH)
     {
        neev.type = ELSA_EVENT_STATUS;
        if (elsa_session_authenticate(eev->event.auth.login,
                                      eev->event.auth.password))
          {
             elsa_session_login(
                _elsa_server_find_command(eev->event.auth.session));
             neev.event.status.granted = EINA_TRUE;
          }
        else
          neev.event.status.granted = EINA_FALSE;
        enc = elsa_event_encode(&neev, &size);
        ecore_con_client_send(ev->client, enc, size);
     }
   else
     fprintf(stderr, PACKAGE": server authenticate error\n");

   return ECORE_CALLBACK_RENEW;
}

void
elsa_server_init()
{
   Ecore_Event_Handler *h;
   ecore_con_init();
   if (elsa_config->xsessions)
     _elsa_server_init_desktops();
   fprintf(stderr, PACKAGE": server init\n");
   _elsa_server = ecore_con_server_add(ECORE_CON_LOCAL_SYSTEM,
                                        "elsa", 42, NULL);
   if (_elsa_server)
     fprintf(stderr, PACKAGE": server init ok\n");
   else
     fprintf(stderr, PACKAGE": server init fail\n");

   h = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_ADD,
                               _elsa_server_add, NULL);
   _handlers = eina_list_append(_handlers, h);
   h = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DEL,
                               _elsa_server_del, NULL);
   _handlers = eina_list_append(_handlers, h);
   h = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DATA,
                               _elsa_server_data, NULL);
   _handlers = eina_list_append(_handlers, h);
}

void
elsa_server_shutdown()
{
   Ecore_Event_Handler *h;
   fprintf(stderr, PACKAGE": server shutdown\n");
   if (_elsa_server)
     ecore_con_server_del(_elsa_server);
   EINA_LIST_FREE(_handlers, h)
     ecore_event_handler_del(h);
   ecore_con_shutdown();
}

static void
_elsa_server_init_desktops()
{
   char buf[PATH_MAX];
   Eina_List *dirs, *l;
   const char *path;

   efreet_init();
   efreet_desktop_type_alias(EFREET_DESKTOP_TYPE_APPLICATION, "XSession");
   /* Maybee need to scan other directories ?
    * _elsa_server_scan_desktops("/etc/share/xsessions");
    */
   snprintf(buf, sizeof(buf), "%s/xsessions", efreet_data_home_get());
   _elsa_server_scan_desktops(buf);
   dirs = efreet_data_dirs_get();
   EINA_LIST_FOREACH(dirs, l, path)
     {
        snprintf(buf, sizeof(buf), "%s/xsessions", path);
        _elsa_server_scan_desktops(buf);
     }
   efreet_shutdown();
}

static void
_elsa_server_scan_desktops(const char *dir)
{
   Eina_List *files;
   char *filename;
   char path[PATH_MAX];

   fprintf(stderr, PACKAGE": scanning directory %s\n", dir);
   files = ecore_file_ls(dir);
   EINA_LIST_FREE(files, filename)
     {
        snprintf(path, sizeof(path), "%s/%s", dir, filename);
        _elsa_server_scan_desktops_file(path);
        free(filename);
     }
}

static void
_elsa_server_scan_desktops_file(const char *path)
{
   Efreet_Desktop *desktop;
   Eina_List *commands;
   Eina_List *l;
   Elsa_Xsession *xsession;
   char *command;

   desktop = efreet_desktop_get(path);
   if (!desktop) return;
   EINA_LIST_FOREACH(_xsessions, l, xsession)
     {
        if (!strcmp(xsession->name, desktop->name))
          {
             efreet_desktop_free(desktop);
             return;
          }
     }

   commands = efreet_desktop_command_local_get(desktop, NULL);
   if (commands)
     {
        char *tmp;
        command = eina_list_data_get(commands);
        commands = eina_list_next(commands);
        EINA_LIST_FREE(commands, tmp)
           free(tmp);
     }
   if (command && desktop->name)
     {
        xsession = calloc(1, sizeof(Elsa_Xsession));
        xsession->command = eina_stringshare_add(command);
        xsession->name = eina_stringshare_add(desktop->name);
        if (desktop->icon) xsession->icon = eina_stringshare_add(desktop->icon);
        _xsessions = eina_list_append(_xsessions, xsession);
        fprintf(stderr, PACKAGE": client find sessions %s\n", desktop->name);
        free(command);
     }
   efreet_desktop_free(desktop);
}

static const char *
_elsa_server_find_command(const char *session)
{
   Eina_List *l;
   Elsa_Xsession *xsession;
   if (session)
     {
        EINA_LIST_FOREACH(_xsessions, l, xsession)
          {
             if (!strcmp(xsession->name, session))
               return xsession->command;
          }
     }
   return (elsa_config->command.session_login);
}

