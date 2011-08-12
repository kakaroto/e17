#include "elsa.h"

#define ELSA_SESSION_KEY "session"
#define ELSA_HISTORY_FILE "elsa.hst"

static void _elsa_history_read();
static void _elsa_history_write();
static void _elsa_user_init();
static void _elsa_user_shutdown();
const char *_elsa_history_match(const char *login);


static Eet_Data_Descriptor *_eddh;
static Elsa_History *_elsa_history;
static Eina_List *_lusers = NULL;
static Eina_Bool _history_update = EINA_FALSE;

void
elsa_history_init()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc, eddcl;
   printf("xpid1 %s\n", getenv("ELSA_XPID"));
   // TODO add idler to load history and thread stuff

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Elsa_Login);
   printf("xpid2 %s\n", getenv("ELSA_XPID"));
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Login, "login", login, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Elsa_Login, "session", session, EET_T_STRING);

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddcl, Elsa_History);
   _eddh = eet_data_descriptor_stream_new(&eddcl);
   EET_DATA_DESCRIPTOR_ADD_LIST(_eddh, Elsa_History, "history", history, edd);

   _elsa_history_read();
   _elsa_user_init();
   elsa_action_init();
}

Eina_List *elsa_history_get()
{
   return _lusers;
}

void
elsa_history_shutdown()
{
   _elsa_history_write();
   elsa_action_shutdown();
   _elsa_user_shutdown();
}

static void
_elsa_history_read()
{
   Eet_File *ef;
   Eina_List *l;
   Elsa_Login *el;

   ef = eet_open("/var/cache/"PACKAGE"/"ELSA_HISTORY_FILE,
                 EET_FILE_MODE_READ_WRITE);
   if (!(ef) || !(_elsa_history = eet_data_read(ef, _eddh, ELSA_SESSION_KEY)))
     {
        fprintf(stderr, PACKAGE": Error on reading last session login\n");
        _elsa_history = calloc(1, sizeof(Elsa_History));
     }
   EINA_LIST_FOREACH(_elsa_history->history, l, el)
      printf("I read last session %s - %s\n", el->login, el->session);

   eet_close(ef);
}

static void
_elsa_history_write()
{
   Eet_File *ef;
   Elsa_Login *el;


   if (_history_update)
     {
        fprintf(stderr, PACKAGE": writing history file\n");
        ef = eet_open("/var/cache/"PACKAGE"/"ELSA_HISTORY_FILE,
                      EET_FILE_MODE_READ_WRITE);
        if (!ef)
          ef = eet_open("/var/cache/"PACKAGE"/"ELSA_HISTORY_FILE,
                        EET_FILE_MODE_WRITE);

        if (!eet_data_write(ef, _eddh, ELSA_SESSION_KEY, _elsa_history, 1))
          fprintf(stderr, PACKAGE": Error on updating last session login\n");

        eet_close(ef);
     }
   EINA_LIST_FREE(_elsa_history->history, el)
     {
        eina_stringshare_del(el->login);
        eina_stringshare_del(el->session);
     }
}

void
elsa_history_push(const char *login, const char *session)
{
   Eina_List *l;
   Elsa_Login *el;

   EINA_LIST_FOREACH(_elsa_history->history, l, el)
     {
        if (!strcmp(login, el->login))
          {
             if (el->session && strcmp(session, el->session))
               {
                  eina_stringshare_replace(&el->session, session);
                  _history_update = EINA_TRUE;
               }
             break;
          }
     }
   if (!el)
     {
        if ((el = (Elsa_Login *) malloc(sizeof(Elsa_Login))))
          {
             el->login = eina_stringshare_add(login);
             el->session = eina_stringshare_add(session);
             _elsa_history->history =
                eina_list_append(_elsa_history->history, el);
             _history_update = EINA_TRUE;
          }
     }
}


const char *
_elsa_history_match(const char *login)
{
   Eina_List *l;
   Elsa_Login *el;
   const char *ret = NULL;
   EINA_LIST_FOREACH(_elsa_history->history, l, el)
     {
        if (!strcmp(el->login, login))
          ret = el->session;
     }
   return ret;
}

static void
_elsa_user_init()
{
   char buf[PATH_MAX];
   FILE *f;
   Elsa_User *eu;
   Eina_List *lu = NULL;
   char *token;
   char *user;
   int uid;

   f = fopen("/etc/passwd", "r");
   if (f)
     {
        while (fgets(buf, sizeof(buf), f))
          {
             user = strtok(buf, ":");
             strtok(NULL, ":");
             token = strtok(NULL, ":");
             uid = atoi(token);
             if (uid > 500 && uid < 3000)
               lu = eina_list_append(lu, eina_stringshare_add(user));
          }
     }
   EINA_LIST_FREE(lu, user)
     {
        if ((eu = (Elsa_User *) malloc(sizeof(Elsa_User))))
          {
             eu->login = eina_stringshare_add(user);
             snprintf(buf, sizeof(buf),
                      "/var/cache/"PACKAGE"/users/%s.edj", user);
             if (ecore_file_exists(buf))
               eu->image = eina_stringshare_add(buf);
             else
               eu->image = NULL;
             eu->lsess = _elsa_history_match(user);
             eina_stringshare_del(user);
             _lusers = eina_list_append(_lusers, eu);
          }
     }
}

static void
_elsa_user_shutdown()
{
   Elsa_User *eu;
   EINA_LIST_FREE(_lusers, eu)
     {
        eina_stringshare_del(eu->login);
        free(eu);
     }
   free(_elsa_history);
}

