#include "elsa.h"

typedef void (*Elsa_Action_Cb)(void *data);

static void _elsa_action_shutdown(void *data);
static void _elsa_action_reboot(void *data);
static void _elsa_action_suspend(void *data);
static Eina_Bool _elsa_action_exe_event_del_cb(void *data, int type, void *event);
#ifdef HAVE_GRUB2
#define GRUB2_FILE "/boot/grub/grub.cfg"

static void _elsa_action_grub2_get(void);
#endif

static Eina_List *_elsa_actions = NULL;

typedef struct Elsa_Action_Data__
{
   int id;
   const char *label;
   Elsa_Action_Cb func;
   void *data;
} Elsa_Action_Data;

static Ecore_Exe *_action_exe = NULL;
static Ecore_Event_Handler *_handle = NULL;

static Elsa_Action_Data *
_elsa_action_add(const char *label, Elsa_Action_Cb func, void *data)
{
   Elsa_Action_Data *ead;
   ead = calloc(1, sizeof(Elsa_Action_Data));
   ead->label = eina_stringshare_add(label);
   ead->func = func;
   ead->data = data;
   ead->id = _elsa_actions ? (eina_list_count(_elsa_actions)) : 0;
   _handle = ecore_event_handler_add(ECORE_EXE_EVENT_DEL,
                                     _elsa_action_exe_event_del_cb, NULL);
   return ead;
}


void
elsa_action_init(void)
{
   _elsa_actions = eina_list_append(_elsa_actions,
      _elsa_action_add("Shutdown", _elsa_action_shutdown, NULL));
   _elsa_actions = eina_list_append(_elsa_actions,
      _elsa_action_add("Reboot", _elsa_action_reboot, NULL));
   _elsa_actions = eina_list_append(_elsa_actions,
      _elsa_action_add("Suspend", _elsa_action_suspend, NULL));
#ifdef HAVE_GRUB2
   _elsa_action_grub2_get();
#endif
}

Eina_List *
elsa_action_get(void)
{
   Elsa_Action_Data *ead;
   Elsa_Action *ea;
   Eina_List *l, *ret = NULL;

   EINA_LIST_FOREACH(_elsa_actions, l, ead)
     {
        ea = calloc(1, sizeof(Elsa_Action));
        ea->label = eina_stringshare_add(ead->label);
        ea->id = ead->id;
        ret = eina_list_append(ret, ea);
     }
   return ret;
}

void
elsa_action_shutdown(void)
{
   Elsa_Action_Data *ead;
   EINA_LIST_FREE(_elsa_actions, ead)
     {
        eina_stringshare_del(ead->label);
        free(ead);
     }
}


void
elsa_action_run(int action)
{
   Elsa_Action_Data *ead;

   ead = eina_list_nth(_elsa_actions, action);
   if (ead)
     ead->func(ead->data);
}

static void
_elsa_action_suspend(void *data __UNUSED__)
{
   _action_exe = NULL;
   ecore_exe_run(elsa_config->command.suspend, NULL);
}

static void
_elsa_action_shutdown(void *data __UNUSED__)
{
   _action_exe = ecore_exe_run(elsa_config->command.shutdown, NULL);
}

static void
_elsa_action_reboot(void *data __UNUSED__)
{
   _action_exe = ecore_exe_run(elsa_config->command.reboot, NULL);
}

static Eina_Bool
_elsa_action_exe_event_del_cb(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Exe_Event_Del *ev;
   Eina_Bool ret = ECORE_CALLBACK_PASS_ON;
   ev = event;
   if (ev->exe == _action_exe)
     {
        ecore_main_loop_quit();
        ret = ECORE_CALLBACK_DONE;
     }
   return ret;
}

/////* grub2 action *//////
#ifdef HAVE_GRUB2

static void
_elsa_action_grub2(void *data)
{
   int i;
   char buf[PATH_MAX];
   i = data;

   snprintf(buf, sizeof(buf),
            "grub-reboot %d && %s", i, elsa_config->command.reboot);
   _action_exe = ecore_exe_run(buf, NULL);

}

static void
_elsa_action_grub2_get(void)
{
   FILE *f;
   unsigned char grub2_ok;
   char buf[1000];
   char action[200];
   int menuentry = 0;
   char *p, *s;

   f = fopen(GRUB2_FILE, "r");
   while ((p = fgets(buf, sizeof(buf), f)))
     {
        if (*p == '#')
          continue;
        if (!grub2_ok)
          {
             if ((p = strstr(p, "default=\"")))
               {
                  if (strstr(p + 9, "$saved"))
                    {
                       grub2_ok = 1;
                       printf("ok\n");
                    }
               }
             continue;
          }
        else
          {
             if ((s = strstr(buf, "menuentry")))
               {
                  s += 10;
                  while(s && *s != '\'') ++s;
                  ++s;
                  p = s;
                  while(*p != '\'') ++p;
                  *p = '\0';
                  snprintf(action, sizeof(action),
                           "Reboot on %s", s);
                  _elsa_actions =
                     eina_list_append(
                        _elsa_actions,
                        _elsa_action_add(action,
                                         _elsa_action_grub2, menuentry++));
               }
          }
     }
   fclose(f);
}

#endif
