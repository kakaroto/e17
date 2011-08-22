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
   intptr_t i = 0;
   char buf[PATH_MAX];
   i = (intptr_t)data;

   snprintf(buf, sizeof(buf),
            "grub-reboot %d && %s", i, elsa_config->command.reboot);
   _action_exe = ecore_exe_run(buf, NULL);

}

static char *
_elsa_memstr(char *data, size_t length, char *look, unsigned int size)
{
   char *tmp;

   while (length >= size)
     {
        tmp = memchr(data, *look, length);
        if (!tmp) return NULL;

        if (strncmp(tmp + 1, look + 1, size - 1) == 0)
          return tmp;
        length = tmp - data;
        data = tmp;
     }

   return NULL;
}


static void
_elsa_action_grub2_get(void)
{
   Eina_File *f;
   unsigned char grub2_ok = 0;
   intptr_t menuentry = 0;
   char *data;
   char *r, *r2;
   char *s;
   int i;

   fprintf(stderr, PACKAGE": trying to open "GRUB2_FILE);
   f = eina_file_open(GRUB2_FILE, EINA_FALSE);
   if (!f) return ;
   fprintf(stderr, " o");

   data = eina_file_map_all(f, EINA_FILE_SEQUENTIAL);
   if (!data) goto on_error;
   fprintf(stderr, "k\n");

   s = data;
   r2 = NULL;
   for (i = eina_file_size_get(f); i > 0; --i, s++)
     {
        int size;

        /* working line by line */
        r = memchr(s, '\n', i);
        if (!r)
          {
             r = s + i;
             i = 0;
          }
        size = r - s;

        if (*s == '#')
          goto end_line;

        /* look if the word is in this line */
        if (!grub2_ok)
          r2 = _elsa_memstr(s, size, "default=\"${saved_entry}\"", 24);
        else
          r2 = _elsa_memstr(s, size, "menuentry", 9);

        /* still some lines to read */
        if (!r2) goto end_line;

        if (!grub2_ok)
          {
             grub2_ok = 1;
             fprintf(stderr, PACKAGE": GRUB2 save mode found \n");
          }
        else
          {
             char *action;
             char *local;
             char *tmp;

             r2 += 10;
             size -= 10;

             tmp = memchr(r2, '\'', size);
             if (!tmp) goto end_line;

             size -= tmp - r2 + 1;
             r2 = tmp + 1;
             tmp = memchr(r2, '\'', size);
             if (!tmp) goto end_line;

             local = alloca(tmp - r2 + 1);
             memcpy(local, r2, tmp - r2);
             local[tmp - r2] = '\0';

             action = malloc((tmp - r2 + 1 + 11) * sizeof (char));
             if (!action) goto end_line;

             sprintf(action, "Reboot on %s", local);
             fprintf(stderr, PACKAGE": GRUB2 '%s'\n", action);
             _elsa_actions =
                eina_list_append(_elsa_actions,
                                 _elsa_action_add(action,
                                                  _elsa_action_grub2,
                                                  (void*)(menuentry++)));

          }

     end_line:
        i -= size;
        s = r;
     }

   eina_file_map_free(f, data);
 on_error:
   eina_file_close(f);
}

#endif
