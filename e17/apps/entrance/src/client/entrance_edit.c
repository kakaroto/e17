/**
@file entrance_edit.c
@brief An app that configures entrance
*/
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <dirent.h>
#include <Ecore_Config.h>
#include <Edje.h>
#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Text_Entry.h>
#include "entrance.h"
#include "entrance_config.h"
#include "entrance_x_session.h"

#define DEBUG 0
#define WINW 800
#define WINH 600
#define SCROLL_TIME 1.0

static struct
{
   Evas_Hash *hashes;
   Evas_Hash *entries;
   Evas_Object *edje;
   Evas_Object *entry;
   Entrance_Config *config;
   Entrance_User *current_user;
   Entrance_X_Session *current_session;
   Ecore_Timer *scroll_timer;
   struct
   {
      Evas_Object *users, *sessions, *themes;
   } container;
   struct
   {
      char *key, *signal;
   } focus;
} ecco;

static void ecco_dir_list_init(Evas_Object * container, const char *dir);
static void ecco_users_list_init(Evas_Object * container);
static void ecco_sessions_list_init(Evas_Object * container);
void user_selected_cb(void *data, Evas_Object * o, const char *emission,
                      const char *source);
void user_unselected_cb(void *data, Evas_Object * o, const char *emission,
                        const char *source);
void session_item_selected_cb(void *data, Evas_Object * o,
                              const char *emission, const char *source);
void session_item_unselected_cb(void *data, Evas_Object * o,
                                const char *emission, const char *source);

/**
 * what to do if we SIGINT(^c) it
 * @param data - no clue
 * @param ev_type - kill event ?
 * @param ev - event data
 * @return 1
 * Obviously I want to exit here.
 */
static int
exit_cb(void *data, int ev_type, void *ev)
{
   ecore_main_loop_quit();
   return 1;
}

/**
 * @param ee - the Ecore_Evas that received the event
 */
static void
window_pre_render_cb(Ecore_Evas * ee)
{
   edje_thaw();
}

/**
 * @param ee - the Ecore_Evas that received the event
 */
static void
window_post_render_cb(Ecore_Evas * ee)
{
   edje_freeze();
}

/**
 * what to do when we receive a window delete event
 * @param ee - the Ecore_Evas that received the event
 */
static void
window_del_cb(Ecore_Evas * ee)
{
   ecore_main_loop_quit();
}

/**
 * handle when the ecore_evas needs to be resized
 * @param ee - The Ecore_Evas we're resizing 
 */
static void
window_resize_cb(Ecore_Evas * ee)
{
   Evas_Object *o = NULL;

   if ((o = evas_object_name_find(ecore_evas_get(ee), "ui")))
   {
      int w, h;

      ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
      evas_object_resize(o, w, h);
   }
}

/*=========================================================================
 * Input Validators
 *========================================================================*/
static int
is_valid_theme_eet(Evas_Object * o, const char *file)
{
   int result = 0;
   struct stat status;
   Evas_Object *oo = NULL;

   if (!stat(file, &status))
   {
      oo = edje_object_add(evas_object_evas_get(o));
      result = edje_object_file_set(oo, file, "Main");
      evas_object_del(oo);
   }
   return (result);
}

#if 0
static int
is_valid_avatar_theme_eet(Evas_Object * o, const char *file)
{
   int result = 0;
   struct stat status;
   Evas_Object *oo = NULL;

   if (!stat(file, &status))
   {
      oo = edje_object_add(evas_object_evas_get(o));
      result = edje_object_file_set(oo, file, "Icon");
      evas_object_del(oo);
   }
   return (result);

}
#endif

/**
 * when Enter is hit on the keyboard we end up here
 * @param data - The smart object that is this Entry
 * @param str - The string that was in the buffer when Enter was pressed
 */
static void
interp_return_key(void *data, const char *str)
{
   char buf[PATH_MAX];
   char *old = NULL;
   char *new_str = NULL;
   Evas_Object *o = NULL;

   int size = 0;

   o = (Evas_Object *) data;
   size = strlen(str);
#if DEBUG
   if (!str)
      fprintf(stderr, "STRING IS NULL!!!!\n");
   else
      fprintf(stderr, "Entry Sent %s(%d)\n", str, size);
#endif
   if (esmart_text_entry_edje_part_get(o))
   {
#if DEBUG
      fprintf(stderr, "%s set its text\n",
              esmart_text_entry_edje_part_get(o));
#endif
      if ((old =
           evas_hash_find(ecco.hashes, esmart_text_entry_edje_part_get(o))))
      {
         evas_hash_del(ecco.hashes, esmart_text_entry_edje_part_get(o), old);
      }
      else
      {
         fprintf(stderr, "Unable to find old entry for %s\n",
                 esmart_text_entry_edje_part_get(o));
      }
      if (size > 0)
      {
         new_str = strdup(str);
      }
      else
      {
         new_str = strdup("");
      }
      if ((old =
           evas_hash_find(ecco.entries, esmart_text_entry_edje_part_get(o))))
      {
         if (!strcmp(old, "ecco,entry,focus,in,greeting,before"))
         {
            if (ecco.config->before.string)
               free(ecco.config->before.string);
            ecco.config->before.string = new_str;
            ecco.hashes =
               evas_hash_add(ecco.hashes, esmart_text_entry_edje_part_get(o),
                             new_str);
         }
         else if (!strcmp(old, "ecco,entry,focus,in,greeting,after"))
         {
            if (ecco.config->after.string)
               free(ecco.config->after.string);
            ecco.config->after.string = new_str;
            ecco.hashes =
               evas_hash_add(ecco.hashes, esmart_text_entry_edje_part_get(o),
                             new_str);
         }
         else if (!strcmp(old, "ecco,entry,focus,in,date"))
         {
            if (ecco.config->date.string)
               free(ecco.config->date.string);
            ecco.config->date.string = new_str;
            ecco.hashes =
               evas_hash_add(ecco.hashes, esmart_text_entry_edje_part_get(o),
                             new_str);
         }
         else if (!strcmp(old, "ecco,entry,focus,in,time"))
         {
            if (ecco.config->time.string)
               free(ecco.config->time.string);
            ecco.config->time.string = new_str;
            ecco.hashes =
               evas_hash_add(ecco.hashes, esmart_text_entry_edje_part_get(o),
                             new_str);
         }
         else if (!strcmp(old, "ecco,entry,focus,in,session,current,session"))
         {
            if (ecco.current_session)
            {
               if (ecco.current_session->session)
                  free(ecco.current_session->session);
               ecco.current_session->session = new_str;
               ecco.hashes =
                  evas_hash_add(ecco.hashes,
                                esmart_text_entry_edje_part_get(o), new_str);
            }
         }
         else if (!strcmp(old, "ecco,entry,focus,in,session,current,name"))
         {
            Evas_List *l = NULL;

            if (ecco.current_session)
            {
               fprintf(stderr, "%s:%s:%s\n", str, new_str,
                       ecco.current_session->name);
               if (strcmp(new_str, ecco.current_session->name))
               {
                  ecco.config->sessions.hash =
                     evas_hash_del(ecco.config->sessions.hash,
                                   ecco.current_session->name,
                                   ecco.current_session);
                  if ((l =
                       evas_list_find_list(ecco.config->sessions.keys,
                                           ecco.current_session->name)))
                  {
                     free(l->data);
                     l->data = new_str;
                  }
                  ecco.current_session->name = new_str;
                  ecco.config->sessions.hash =
                     evas_hash_add(ecco.config->sessions.hash,
                                   ecco.current_session->name,
                                   ecco.current_session);

                  esmart_container_empty(ecco.container.sessions);
                  edje_object_signal_emit(ecco.edje, "ecco,show,sessions",
                                          "");
                  ecco.hashes =
                     evas_hash_add(ecco.hashes,
                                   esmart_text_entry_edje_part_get(o),
                                   new_str);
               }
               else
               {
                  free(new_str);
               }
            }
            else
            {
               free(new_str);
            }
         }
         else if (!strcmp(old, "ecco,entry,focus,in,user,current,name"))
         {
            if (ecco.current_user)
            {

               Evas_List *l = NULL;

               if (strcmp(new_str, ecco.current_user->name))
               {
                  fprintf(stderr, "%s:%s:%s\n", str, new_str,
                          ecco.current_user->name);
                  ecco.config->users.hash =
                     evas_hash_del(ecco.config->users.hash,
                                   ecco.current_user->name,
                                   ecco.current_user);
                  if ((l =
                       evas_list_find_list(ecco.config->users.keys,
                                           ecco.current_user->name)))
                  {
                     fprintf(stderr, "AAAH %s\n", (char *) l->data);
                     free(l->data);
                     l->data = new_str;
                  }
                  else
                  {
                     fprintf(stderr, "LEAKAGE !!!\n");
                  }
                  ecco.current_user->name = new_str;
                  ecco.config->users.hash =
                     evas_hash_add(ecco.config->users.hash,
                                   ecco.current_user->name,
                                   ecco.current_user);

                  esmart_container_empty(ecco.container.users);
                  edje_object_signal_emit(ecco.edje, "ecco,show,users", "");
                  ecco.hashes =
                     evas_hash_add(ecco.hashes,
                                   esmart_text_entry_edje_part_get(o),
                                   new_str);
               }
               else
               {
                  free(new_str);
               }
            }
            else
            {
               free(new_str);
            }
         }
         else if (!strcmp(old, "ecco,entry,focus,in,user,current,session"))
         {
            if (ecco.current_user)
            {
               if (ecco.current_user->session)
                  free(ecco.current_user->session);
               ecco.current_user->session = new_str;
               ecco.hashes =
                  evas_hash_add(ecco.hashes,
                                esmart_text_entry_edje_part_get(o), new_str);
            }
         }
         else if (!strcmp(old, "ecco,entry,focus,in,remember,n"))
         {
            snprintf(buf, PATH_MAX, "%s", str);
            ecco.config->users.remember_n = atoi(buf);
            ecco.hashes =
               evas_hash_add(ecco.hashes, esmart_text_entry_edje_part_get(o),
                             new_str);
         }
         else if (!strcmp(old, "ecco,entry,focus,in,theme"))
         {
            snprintf(buf, PATH_MAX, PACKAGE_DATA_DIR "/themes/%s", new_str);
            if (is_valid_theme_eet(o, buf))
            {
               fprintf(stderr, "You found a valid edje\n");
               if (ecco.config->theme)
                  free(ecco.config->theme);
               ecco.config->theme = new_str;
               ecco.hashes =
                  evas_hash_add(ecco.hashes,
                                esmart_text_entry_edje_part_get(o), new_str);
            }
            else
            {
               fprintf(stderr, "You found a invalid edje\n");
               esmart_text_entry_text_set(o, ecco.config->theme);
               free(new_str);
            }
         }
         else
         {
            fprintf(stderr, "Unknown signal, %s\n", old);
         }
      }
      else
      {
         fprintf(stderr, "Unknown signal for %s\n",
                 esmart_text_entry_edje_part_get(o));
      }
   }
}

/*=========================================================================
 * Edje Callbacks 
 *========================================================================*/
/**
 * when an edje part requests key focus be sent to it
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_key_focus_in(void *data, Evas_Object * o, const char *emission,
              const char *source)
{
   char buf[PATH_MAX];
   const char *str = NULL;
   const char *bstr = NULL;

#if DEBUG
   fprintf(stderr, "Key Focus In(%s)(%s)\n", emission, source);
#endif
   if ((str = evas_hash_find(ecco.hashes, emission)))
   {
#if DEBUG
      fprintf(stderr, "%s is the entry part name\n", str);
#endif
      if (ecco.entry)
      {
         evas_object_focus_set(ecco.entry, 1);
         if ((edje_object_part_exists(ecco.edje, str)))
         {
            if ((bstr = edje_object_part_text_get(ecco.edje, str)))
            {
#if DEBUG
               fprintf(stderr, "%s is the string\n", bstr);
#endif
               if (ecco.focus.key && ecco.focus.signal)
               {
#if DEBUG
                  fprintf(stderr, "Emitting %s to %s\n", ecco.focus.signal,
                          ecco.focus.key);
#endif
                  edje_object_signal_emit(ecco.edje, ecco.focus.signal,
                                          ecco.focus.key);
                  if (ecco.focus.key)
                     free(ecco.focus.key);
                  ecco.focus.key = NULL;
                  edje_object_signal_emit(ecco.edje, ecco.focus.signal,
                                          source);
               }
               esmart_text_entry_edje_part_set(ecco.entry, ecco.edje,
                                               (char *) str);
               esmart_text_entry_text_set(ecco.entry, bstr);
               snprintf(buf, PATH_MAX, "%s-2", source);
               ecco.focus.key = strdup(buf);
            }
         }
         else
         {
            fprintf(stderr, "Entry Part does not exist\n");
         }
      }
   }
   else
   {
      fprintf(stderr, "FAILED: ecco,entry,focus,in\n");
   }
}

/**
 * when an edje part requests key focus be taken away from it
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_key_focus_out(void *data, Evas_Object * o, const char *emission,
               const char *source)
{
   const char *str = NULL;
   const char *bstr = NULL;
   const char *cstr = NULL;

   if ((str = evas_hash_find(ecco.hashes, emission)))
   {
      if (ecco.entry)
      {
         if ((edje_object_part_exists(ecco.edje, str)))
         {
            if ((bstr = edje_object_part_text_get(ecco.edje, str)))
            {
               if ((cstr = evas_hash_find(ecco.hashes, str)))
               {
                  if (strcmp(cstr, bstr))
                  {
#if DEBUG
                     fprintf(stderr, "Changed... Ignoring%s\n", bstr);
#endif
                     edje_object_part_text_set(ecco.edje, str, cstr);
                  }
               }
            }
         }
         else
         {
            fprintf(stderr, "Entry Part does not exist\n");
         }
      }
   }
#if DEBUG
   fprintf(stderr, "Key Focus Out(%s)(%s)\n", emission, source);
#endif
}

/**
 * when a different part of ecco is shown, or a way of reinitializing the
 * contents of the current ui when backend memory needs updating
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_show_cb(void *data, Evas_Object * o, const char *emission,
         const char *source)
{
   int scroll;
   double dx, dy;
   Evas_List *l = NULL;
   char *str = NULL;
   Evas_Object *oo = NULL;
   Evas_Coord cx, cy, cw, ch;

   if (ecco.focus.key && ecco.focus.signal)
   {
      edje_object_signal_emit(ecco.edje, ecco.focus.signal, ecco.focus.key);
      free(ecco.focus.key);
      ecco.focus.key = NULL;
   }
   if (!strcmp(emission, "ecco,show,preview"))
   {
      switch (fork())
      {
        case 0:
           execl(PACKAGE_BIN_DIR "/entrance", "entrance", "-T", NULL);
           exit(0);
           break;
        default:
           break;
      }
   }
   else if (!strcmp(emission, "ecco,show,users"))
   {

      session_item_unselected_cb(ecco.current_session, NULL, "", "");
      if ((l = esmart_container_elements_get(ecco.container.users)) == NULL)
      {
         ecco_users_list_init(ecco.container.users);
      }

      if (!ecco.current_user && ecco.config->users.keys
          && ecco.config->users.keys->data)
      {

         if ((ecco.current_user =
              evas_hash_find(ecco.config->users.hash,
                             (char *) ecco.config->users.keys->data)))
         {
            user_selected_cb(ecco.current_user, NULL, "", "");
         }
      }

      str = evas_hash_find(ecco.hashes, "ecco,container,user,dragbar");
      edje_object_part_drag_value_get(o, str, &dx, &dy);
      oo = ecco.container.users;
      evas_object_geometry_get(oo, &cx, &cy, &cw, &ch);
      if (esmart_container_direction_get(oo) > 0)
      {
         scroll =
            (int) (dy * (esmart_container_elements_length_get(oo) - ch));
      }
      else
      {
         scroll =
            (int) (dx * (esmart_container_elements_length_get(oo) - cw));
      }
#if DEBUG
      fprintf(stderr, "%d %d\n", scroll,
              esmart_container_scroll_offset_get(oo));
#endif
      esmart_container_scroll_offset_set(oo, -scroll);
   }
   else if (!strcmp(emission, "ecco,show,sessions"))
   {
      user_unselected_cb(ecco.current_user, NULL, "", "");
      if ((l =
           esmart_container_elements_get(ecco.container.sessions)) == NULL)
      {
         ecco_sessions_list_init(ecco.container.sessions);
      }
      if (!ecco.current_session && ecco.config->sessions.keys
          && ecco.config->sessions.keys->data)
      {
         if ((ecco.current_session =
              evas_hash_find(ecco.config->sessions.hash,
                             (char *) ecco.config->sessions.keys->data)))
         {
            session_item_selected_cb(ecco.current_session, NULL, "", "");
         }
      }
      str = evas_hash_find(ecco.hashes, "ecco,container,session,dragbar");
      edje_object_part_drag_value_get(o, str, &dx, &dy);
      oo = ecco.container.sessions;
      evas_object_geometry_get(oo, &cx, &cy, &cw, &ch);
      if (esmart_container_direction_get(oo) > 0)
      {
         scroll =
            (int) (dy * (esmart_container_elements_length_get(oo) - ch));
      }
      else
      {
         scroll =
            (int) (dx * (esmart_container_elements_length_get(oo) - cw));
      }
#if DEBUG
      fprintf(stderr, "%d %d\n", scroll,
              esmart_container_scroll_offset_get(oo));
#endif
      esmart_container_scroll_offset_set(oo, -scroll);
   }
#if DEBUG
   fprintf(stderr, "Show: (%s)(%s)\n", emission, source);
#endif
}

/**
 * when a toggle in echo requests that it be set to "On"
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_system_toggle_on_cb(void *data, Evas_Object * o, const char *emission,
                     const char *source)
{
   char *nstr = NULL;
   char *estr = NULL;
   char buf[PATH_MAX];
   const char *file = NULL;
   static double last_time = 0.0;

   snprintf(buf, PATH_MAX, "%s,next", emission);
   edje_object_file_get(o, &file, NULL);
   if (file)
   {
      if ((nstr = edje_file_data_get(file, buf)))
      {
         if ((estr = edje_file_data_get(file, "ecco,toggle,on,emission")))
         {
            fprintf(stderr, "On is %s(%s)(%0.2f)\n", nstr, estr,
                    ecore_time_get() - last_time);
            if ((ecore_time_get() - last_time) > 0.1)
            {
               last_time = ecore_time_get();
               edje_object_signal_emit(o, estr, nstr);
            }
            free(estr);
         }
         free(nstr);
      }
   }
   last_time = ecore_time_get();
   if (!strcmp(emission, "ecco,toggle,on,system,auth,pam"))
   {
      ecco.config->auth = ENTRANCE_USE_PAM;
   }
   else if (!strcmp(emission, "ecco,toggle,on,system,auth,shadow"))
   {
      ecco.config->auth = ENTRANCE_USE_SHADOW;
   }
   else if (!strcmp(emission, "ecco,toggle,on,system,reboot"))
   {
      ecco.config->reboot = 1;
   }
   else if (!strcmp(emission, "ecco,toggle,on,system,halt"))
   {
      ecco.config->halt = 1;
   }
   else if (!strcmp(emission, "ecco,toggle,on,system,gl"))
   {
      ecco.config->engine = 1;
   }
   else if (!strcmp(emission, "ecco,toggle,on,system,users,remember"))
   {
      ecco.config->users.remember = 1;
   }
#if DEBUG
   fprintf(stderr, "Toggle On: (%s)(%s)\n", emission, source);
#endif
}

/**
 * when a toggle in echo requests that it be set to "Off"
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_system_toggle_off_cb(void *data, Evas_Object * o, const char *emission,
                      const char *source)
{
   char *nstr = NULL;
   char *estr = NULL;
   char buf[PATH_MAX];
   const char *file = NULL;
   static double last_time = 0.0;

   snprintf(buf, PATH_MAX, "%s,next", emission);
   edje_object_file_get(o, &file, NULL);
   if (file)
   {
      if ((nstr = edje_file_data_get(file, buf)))
      {
         if ((estr = edje_file_data_get(file, "ecco,toggle,off,emission")))
         {
            fprintf(stderr, "On is %s(%s)(%0.2f)\n", nstr, estr,
                    ecore_time_get() - last_time);
            if ((ecore_time_get() - last_time) > 0.1)
            {
               last_time = ecore_time_get();
               edje_object_signal_emit(o, estr, nstr);
            }
            free(estr);
         }
         free(nstr);
      }
   }
   last_time = ecore_time_get();
   if (!strcmp(emission, "ecco,toggle,off,system,auth,pam"))
   {
      ecco.config->auth = ENTRANCE_USE_SHADOW;
   }
   else if (!strcmp(emission, "ecco,toggle,off,system,auth,shadow"))
   {
      ecco.config->auth = ENTRANCE_USE_PAM;
   }
   else if (!strcmp(emission, "ecco,toggle,off,system,reboot"))
   {
      ecco.config->reboot = 0;
   }
   else if (!strcmp(emission, "ecco,toggle,off,system,halt"))
   {
      ecco.config->halt = 0;
   }
   else if (!strcmp(emission, "ecco,toggle,off,system,gl"))
   {
      ecco.config->engine = 0;
   }
   else if (!strcmp(emission, "ecco,toggle,off,system,users,remember"))
   {
      ecco.config->users.remember = 0;
   }
#if DEBUG
   fprintf(stderr, "Toggle Off: (%s)(%s)\n", emission, source);
#endif
}

/**
 * when a button having global scope requests an action
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_button_global_cb(void *data, Evas_Object * o, const char *emission,
                  const char *source)
{
   char buf[PATH_MAX];

   if (!strcmp("ecco,button,global,cancel", emission))
      ecore_main_loop_quit();
   else if (!strcmp("ecco,button,global,save", emission))
   {
      snprintf(buf, PATH_MAX, "%s/entrance_config.cfg", PACKAGE_CFG_DIR);
      if (!entrance_config_save(ecco.config, buf))
      {
         fprintf(stderr, "Wrote %s\n", buf);
      }
      else
      {
         fprintf(stderr, "Unable to write %s\n", buf);
      }
   }
#if DEBUG
   fprintf(stderr, "Button: (%s)(%s)\n", emission, source);
#endif
}

/**
 * when a button requesting a file dialog emits its request
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_button_browse_cb(void *data, Evas_Object * o, const char *emission,
                  const char *source)
{
#if DEBUG
   fprintf(stderr, "Browse: (%s)(%s)\n", emission, source);
#endif
}

/**
 * when a button requesting an addition to ecco is emits this signal
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_button_add_cb(void *data, Evas_Object * o, const char *emission,
               const char *source)
{
   char *str = NULL;
   char buf[PATH_MAX];
   Evas_Object *oo = NULL;
   Entrance_User *eu = NULL;

   if (!strcmp(emission, "ecco,button,add,user"))
   {
      if ((eu = evas_hash_find(ecco.config->users.hash, "New User")))
         return;

      if (ecco.config->sessions.keys && ecco.config->sessions.keys->data)
         str = strdup(ecco.config->sessions.keys->data);
      if ((eu = entrance_user_new(strdup("New User"), NULL, str)))
      {
         snprintf(buf, PATH_MAX, PACKAGE_DATA_DIR "/themes/%s",
                  ecco.config->theme);
         if ((oo = entrance_user_edje_get(eu, ecco.edje, buf)))
         {
            esmart_container_element_append(ecco.container.users, oo);
            ecco.config->users.hash =
               evas_hash_add(ecco.config->users.hash, eu->name, eu);
            ecco.config->users.keys =
               evas_list_append(ecco.config->users.keys, eu->name);
            user_selected_cb(eu, oo, "", "");
         }
      }

   }
#if DEBUG
   fprintf(stderr, "Add: (%s)(%s)\n", emission, source);
#endif
}

/**
 * when a button requests something be deleted from ecco
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_button_del_cb(void *data, Evas_Object * o, const char *emission,
               const char *source)
{
   char buf[PATH_MAX];
   Entrance_User *eu = NULL;
   Entrance_X_Session *exs = NULL;
   Evas_Hash *hash = NULL;
   Evas_List *list = NULL;

   if (!strcmp(emission, "ecco,button,delete,user"))
   {
      hash = ecco.config->users.hash;
      list = ecco.config->users.keys;
      if (!ecco.current_user)
         return;
      if ((eu = evas_hash_find(hash, ecco.current_user->name)))
      {
         ecco.config->users.hash = evas_hash_del(hash, eu->name, eu);
         ecco.config->users.keys = evas_list_remove(list, eu->name);

         entrance_user_free(eu);
         ecco.current_user = NULL;
         esmart_container_empty(ecco.container.users);
         edje_object_signal_emit(ecco.edje, "ecco,show,users", "");
      }
      ecco.config->users.hash = hash;
      ecco.config->users.keys = list;
   }
   else if (!strcmp(emission, "ecco,button,delete,session"))
   {
      hash = ecco.config->sessions.hash;
      list = ecco.config->sessions.keys;
      if (!ecco.current_session)
         return;
      if ((exs = evas_hash_find(hash, ecco.current_session->name)))
      {
         fprintf(stderr, "DELETEING !!!: %s\n", exs->name);
         fprintf(stderr, "DELETEING !!!: %s\n", exs->session);
         ecco.config->sessions.hash = evas_hash_del(hash, exs->name, exs);
         ecco.config->sessions.keys = evas_list_remove(list, exs->name);

         entrance_x_session_free(exs);
         ecco.current_session = NULL;
         esmart_container_empty(ecco.container.sessions);
         edje_object_signal_emit(ecco.edje, "ecco,show,sessions", "");
      }
   }
   else if (!strcmp(emission, "ecco,button,delete,theme"))
   {
   }
   else if (!strcmp(emission, "ecco,button,delete,theme,yes"))
   {
      if (ecco.config->theme)
      {
         snprintf(buf, PATH_MAX, PACKAGE_DATA_DIR "/themes/%s",
                  ecco.config->theme);
         fprintf(stderr, "Should be unlinking %s\n", buf);
#if 0
         unlink(buf);
#endif
      }
   }
#if DEBUG
   fprintf(stderr, "Delete: (%s)(%s)\n", emission, source);
#endif
}

/**
 * when a scrollbar dragable changes its value this adjusts the container
 * accordingly
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_scrollbar_cb(void *data, Evas_Object * o, const char *emission,
              const char *source)
{
   Evas_Coord cx, cy, cw, ch;
   double dx, dy;
   double scroll;
   Evas_Object *oo = NULL;
   char *tmp = NULL;
   char *src = NULL;
   char buf[PATH_MAX];
   char buf2[PATH_MAX];

#if DEBUG
   fprintf(stderr, "Scroll Drag: (%s)(%s)\n", emission, source);
#endif

   if ((tmp = strrchr(emission, ',')))
   {
      if (!strcmp(++tmp, "step"))
      {
         snprintf(buf, tmp - emission, "%s", emission);
         if ((tmp = strrchr(buf, ',')))
         {
            snprintf(buf2, ++tmp - buf, "%s", buf);
            if ((src = strrchr(buf2, ',')))
            {
               src++;
               edje_object_part_drag_value_get(o, source, &dx, &dy);
               if (!strcmp(src, "session"))
               {
                  oo = ecco.container.sessions;
               }
               else if (!strcmp(src, "user"))
               {
                  oo = ecco.container.users;
               }
               else if (!strcmp(src, "theme"))
               {
                  oo = ecco.container.themes;
               }
               else
               {
                  return;
               }
#if DEBUG
               fprintf(stderr, "Stepping %s:%0.2f:%0.2f\n", src, dx, dy);
#endif
            }
         }
#if 0
         edje_object_part_drag_step_get(o, source, &dx, &dy);
#endif
         /* 
            edje_object_part_drag_step(o, source, dx, dy); */
      }
      else if (!strcmp(tmp, "session"))
      {
         edje_object_part_drag_value_get(o, source, &dx, &dy);
#if DEBUG
         fprintf(stderr, "Drag Value is %0.02f %0.02f ", dx, dy);
#endif
         oo = ecco.container.sessions;
      }
      else if (!strcmp(tmp, "user"))
      {
         edje_object_part_drag_value_get(o, source, &dx, &dy);
#if DEBUG
         fprintf(stderr, "Drag Value is %0.02f %0.02f ", dx, dy);
#endif
         oo = ecco.container.users;
      }
      else if (!strcmp(tmp, "theme"))
      {
      }
      else
      {
         fprintf(stderr, "Drag Failed on %s\n", tmp);
         return;
      }
   }
   else
   {
      fprintf(stderr, "ERRRRRR\n");
      return;
   }
   evas_object_geometry_get(oo, &cx, &cy, &cw, &ch);
   if (esmart_container_direction_get(oo) > 0)
   {
      scroll = (int) (dy * (esmart_container_elements_length_get(oo) - ch));
   }
   else
   {
      scroll = (int) (dx * (esmart_container_elements_length_get(oo) - cw));
   }
#if DEBUG
   fprintf(stderr, "%0.02f %d\n", scroll,
           esmart_container_scroll_offset_get(oo));
#endif
   esmart_container_scroll_offset_set(oo, -scroll);
}

/**
 * when a container requests a "next" scroll timer to start or stop
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_scroll_next_cb(void *data, Evas_Object * o, const char *emission,
                const char *source)
{
   char *tmp = NULL;
   char *str = NULL;
   Evas_Object *oo = NULL;      /* in nexus this looks in infinity ! */

   if ((str = evas_hash_find(ecco.hashes, emission)))
   {
      if ((oo = edje_object_part_swallow_get(ecco.edje, str)))
      {
         if ((tmp = strrchr(emission, ',')))
         {
            fprintf(stderr, "%s\n", tmp);
            if (!strcmp(tmp + 1, "start"))
               esmart_container_scroll_start(oo, -1);
            else
               esmart_container_scroll_stop(oo);
         }
      }
   }
#if DEBUG
   fprintf(stderr, "Scroll Next: (%s)(%s)\n", emission, source);
#endif
}

/**
 * when a container requests a "back" scroll timer to start or stop
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_scroll_back_cb(void *data, Evas_Object * o, const char *emission,
                const char *source)
{
   char *tmp = NULL;
   char *str = NULL;
   Evas_Object *oo = NULL;      /* in nexus this looks in infinity ! */

   if ((str = evas_hash_find(ecco.hashes, emission)))
   {
      if ((oo = edje_object_part_swallow_get(ecco.edje, str)))
      {
         if ((tmp = strrchr(emission, ',')))
         {
            fprintf(stderr, "%s\n", tmp);
            if (!strcmp(tmp + 1, "start"))
               esmart_container_scroll_start(oo, 1);
            else
               esmart_container_scroll_stop(oo);
         }
      }
   }
   else
   {
      fprintf(stderr, "Unable to find emission %s\n", emission);
   }
#if DEBUG
   fprintf(stderr, "Scroll Back: (%s)(%s)\n", emission, source);
#endif
}

/**
 * when a user edje requests that its features be the new "current"
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
void
user_selected_cb(void *data, Evas_Object * o, const char *emission,
                 const char *source)
{
   Entrance_User *eu = NULL;
   char *str;
   char *pstr;

   if ((eu = (Entrance_User *) data))
   {
      ecco.current_user = eu;
      if ((str =
           evas_hash_find(ecco.hashes,
                          "ecco,entry,focus,in,user,current,name")))
      {
         if ((pstr = evas_hash_find(ecco.hashes, str)))
         {
            ecco.hashes = evas_hash_del(ecco.hashes, str, pstr);
#if DEBUG
            fprintf(stderr, "USER STR: %s\n", pstr);
#endif
         }
         if ((edje_object_part_exists(ecco.edje, str)))
         {
            edje_object_part_text_set(ecco.edje, str, eu->name);
            ecco.hashes = evas_hash_add(ecco.hashes, str, eu->name);
         }
         else
         {
            fprintf(stderr, "Entry Part does not exist\n");
         }
      }
      if ((str =
           evas_hash_find(ecco.hashes,
                          "ecco,entry,focus,in,user,current,icon")))
      {
         if ((pstr = evas_hash_find(ecco.hashes, str)))
         {
            ecco.hashes = evas_hash_del(ecco.hashes, str, pstr);
#if DEBUG
            fprintf(stderr, "USER STR: %s\n", pstr);
#endif
         }
         if ((edje_object_part_exists(ecco.edje, str)))
         {
            edje_object_part_text_set(ecco.edje, str, eu->icon);
            ecco.hashes = evas_hash_add(ecco.hashes, str, eu->icon);
         }
         else
         {
            fprintf(stderr, "Entry Part does not exist\n");
         }
      }
      if ((str =
           evas_hash_find(ecco.hashes,
                          "ecco,entry,focus,in,user,current,session")))
      {
         if ((pstr = evas_hash_find(ecco.hashes, str)))
         {
            ecco.hashes = evas_hash_del(ecco.hashes, str, pstr);
#if DEBUG
            fprintf(stderr, "USER STR: %s\n", pstr);
#endif
         }
         if ((edje_object_part_exists(ecco.edje, str)))
         {
            edje_object_part_text_set(ecco.edje, str, eu->session);
            ecco.hashes = evas_hash_add(ecco.hashes, str, eu->session);
         }
         else
         {
            fprintf(stderr, "Entry Part does not exist\n");
         }
      }
   }
}

/**
 * when a user edje requests that its features no longer be the "current"
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
void
user_unselected_cb(void *data, Evas_Object * o, const char *emission,
                   const char *source)
{
   int i = 0;
   Entrance_User *eu = NULL;
   char *str;
   char *keys[] = {
      "ecco,entry,focus,in,user,current,name",
      "ecco,entry,focus,in,user,current,icon",
      "ecco,entry,focus,in,user,current,session"
   };
   int size = sizeof(keys) / sizeof(char *);

   if ((eu = (Entrance_User *) data))
   {
      for (i = 0; i < size; i++)
      {
         if ((str = evas_hash_find(ecco.hashes, keys[i])))
         {
            if ((edje_object_part_exists(ecco.edje, str)))
               edje_object_part_text_set(ecco.edje, str, "");
            else
               fprintf(stderr, "Entry Part does not exist\n");
         }
      }
   }
   ecco.current_user = NULL;
}

/**
 * when a session edje requests that its features be the new "current"
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
void
session_item_selected_cb(void *data, Evas_Object * o, const char *emission,
                         const char *source)
{
   char *str = NULL;
   char *pstr = NULL;
   Entrance_X_Session *exs = NULL;

   if ((exs = (Entrance_X_Session *) data))
   {

      ecco.current_session = exs;
#if DEBUG
      fprintf(stderr, "Please gimme %s\n", exs->name);
#endif
      if ((str =
           evas_hash_find(ecco.hashes,
                          "ecco,entry,focus,in,session,current,name")))
      {
#if DEBUG
         fprintf(stderr, "str is %s\n", str);
#endif
         if ((pstr = evas_hash_find(ecco.hashes, str)))
         {
            ecco.hashes = evas_hash_del(ecco.hashes, str, pstr);
#if DEBUG
            fprintf(stderr, "SESSION NAME STR: %s\n", pstr);
#endif
         }
         if ((edje_object_part_exists(ecco.edje, str)))
         {
            edje_object_part_text_set(ecco.edje, str, exs->name);
            ecco.hashes = evas_hash_add(ecco.hashes, str, exs->name);
         }
         else
         {
            fprintf(stderr, "Entry Part does not exist\n");
         }
      }
      if ((str =
           evas_hash_find(ecco.hashes,
                          "ecco,entry,focus,in,session,current,session")))
      {
#if DEBUG
         fprintf(stderr, "str is %s\n", str);
#endif
         if ((pstr = evas_hash_find(ecco.hashes, str)))
         {
            ecco.hashes = evas_hash_del(ecco.hashes, str, pstr);
#if DEBUG
            fprintf(stderr, "SESSION STR: %s\n", pstr);
#endif
         }
         if ((edje_object_part_exists(ecco.edje, str)))
         {
            edje_object_part_text_set(ecco.edje, str, exs->session);
            ecco.hashes = evas_hash_add(ecco.hashes, str, exs->session);
         }
         else
         {
            fprintf(stderr, "Entry Part does not exist\n");
         }
      }
      if ((str =
           evas_hash_find(ecco.hashes,
                          "ecco,entry,focus,in,session,current,icon")))
      {
         if ((pstr = evas_hash_find(ecco.hashes, str)))
         {
            ecco.hashes = evas_hash_del(ecco.hashes, str, pstr);
#if DEBUG
            fprintf(stderr, "ICON NAME STR: %s\n", pstr);
#endif
         }
         if ((edje_object_part_exists(ecco.edje, str)))
         {
            edje_object_part_text_set(ecco.edje, pstr, exs->icon);
            ecco.hashes = evas_hash_add(ecco.hashes, str, exs->icon);
         }
         else
         {
            fprintf(stderr, "Entry Part does not exist\n");
         }
      }
   }
}

/**
 * when a session edje requests that its features no longer be the "current"
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
void
session_item_unselected_cb(void *data, Evas_Object * o, const char *emission,
                           const char *source)
{
   Entrance_X_Session *exs = NULL;

   if ((exs = (Entrance_X_Session *) data))
   {
      int i = 0;
      char *str;
      char *keys[] = {
         "ecco,entry,focus,in,session,current,name",
         "ecco,entry,focus,in,session,current,icon",
         "ecco,entry,focus,in,session,current,session"
      };
      int size = sizeof(keys) / sizeof(char *);

#if DEBUG
      fprintf(stderr, "Please take away %s\n", exs->name);
#endif
      for (i = 0; i < size; i++)
      {
         if ((str = evas_hash_find(ecco.hashes, keys[i])))
         {
            if ((edje_object_part_exists(ecco.edje, str)))
               edje_object_part_text_set(ecco.edje, str, "");
            else
               fprintf(stderr, "Entry Part does not exist\n");
         }
         else
         {
            fprintf(stderr, "HASH NOT SET!!!!\n");
         }
      }
   }
   ecco.current_session = NULL;
}

#if 0
/**
 * Set the "entrance.date" part's text
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 * Attempt to set the Part named "entrance.date" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
static void
set_date(void *data, Evas_Object * o, const char *emission,
         const char *source)
{
   if (edje_object_part_exists(o, "entrance.date"))
   {
      struct tm *now;
      char buf[PATH_MAX];
      time_t _t = time(NULL);

      now = localtime(&_t);
      strftime(buf, PATH_MAX, session->config->date.string, now);
      edje_object_part_text_set(o, "entrance.date", buf);
   }
}

/**
 * Set the "entrance.time" part's text
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 * Attempt to set the Part named "entrance.time" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
static void
set_time(void *data, Evas_Object * o, const char *emission,
         const char *source)
{
   if (edje_object_part_exists(o, "entrance.time"))
   {
      struct tm *now;
      char buf[PATH_MAX];
      time_t _t = time(NULL);

      now = localtime(&_t);
      strftime(buf, PATH_MAX, session->config->time.string, now);
      edje_object_part_text_set(o, "entrance.time", buf);
   }
}


/**
 * we handle this iteration outside of the theme, update date and time
 * @param data a pointer to the main edje in entrance
 * @return 1 so the ecore_timer keeps going and going and ...
 */
int
timer_cb(void *data)
{
   Evas_Object *o = NULL;

   if ((o = (Evas_Object *) data))
   {
      set_date(NULL, o, NULL, NULL);
      set_time(NULL, o, NULL, NULL);
   }
   return (1);
}
#endif

/**
 * when a theme preview requests itself to be the new current edje
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
_preview_selected(void *data, Evas_Object * o, const char *emission,
                  const char *source)
{
   char *str = NULL;
   const char *cstr = NULL;
   const char *file = NULL;
   const char *sig = "ecco,entry,focus,in,theme";

   if ((file = (char *) data))
   {
      esmart_container_empty(ecco.container.users);
      esmart_container_empty(ecco.container.sessions);
      if ((str = evas_hash_find(ecco.hashes, sig)))
      {
         if ((cstr = evas_hash_find(ecco.hashes, str)))
         {
            evas_hash_del(ecco.hashes, str, cstr);
            if (ecco.config->theme)
               free(ecco.config->theme);
            ecco.config->theme = strdup(file);
            edje_object_part_text_set(ecco.edje, str, ecco.config->theme);
            ecco.hashes = evas_hash_add(ecco.hashes, str, ecco.config->theme);
         }
      }
   }
}

/**
 * construct a theme list with files in the specified director
 * @param container - the esmart container theme objects should be swallowed
 * to
 * @param dir - the directory name we're going to read themes from
 */
static void
ecco_dir_list_init(Evas_Object * container, const char *dir)
{
   DIR *mydir = NULL;
   struct dirent *di = NULL;
   char buf[PATH_MAX];
   Evas_Object *o = NULL;
   Evas_Coord w, h;

   if (!container || !dir)
      return;
   if ((mydir = opendir(dir)))
   {
      while ((di = readdir(mydir)))
      {
         snprintf(buf, PATH_MAX, "%s/%s", dir, di->d_name);
         o = edje_object_add(evas_object_evas_get(container));
         if (edje_object_file_set(o, buf, "Preview") > 0)
         {
#if DEBUG
            fprintf(stderr, "%s\n", buf);
#endif
            edje_object_size_min_get(o, &w, &h);
            if ((w > 0) && (h > 0))
               evas_object_resize(o, w, h);
            else
               evas_object_resize(o, 50, 50);
            evas_object_show(o);
            esmart_container_element_append(container, o);
            edje_object_signal_callback_add(o, "PreviewSelected", "*",
                                            _preview_selected,
                                            strdup(di->d_name));
         }
         else
         {
            evas_object_del(o);
         }
      }
      closedir(mydir);
   }
}

/**
 * construct a user list from the current configuration
 * @param container the esmart container where user objects should be swallowed
 */
static void
ecco_users_list_init(Evas_Object * container)
{
   char *str = NULL;
   char buf[PATH_MAX];
   Evas_List *l = NULL;
   Evas_Object *o = NULL;
   Entrance_User *eu = NULL;

   if (!container)
      return;

   snprintf(buf, PATH_MAX, "%s/themes/%s", PACKAGE_DATA_DIR,
            ecco.config->theme);
   for (l = ecco.config->users.keys; l; l = l->next)
   {
      str = (char *) l->data;
      if ((eu = evas_hash_find(ecco.config->users.hash, str)))
      {
         if ((o = entrance_user_edje_get(eu, ecco.edje, buf)))
            esmart_container_element_append(container, o);
      }
   }
}

/**
 * construct the session list from the current configuration
 * @param container the esmart container where session objects should be swallowed
 */
static void
ecco_sessions_list_init(Evas_Object * container)
{
   char *str = NULL;
   char buf[PATH_MAX];
   Evas_List *l = NULL;
   Evas_Object *o = NULL;
   Entrance_X_Session *exs = NULL;

   if (!container)
      return;

   snprintf(buf, PATH_MAX, "%s/themes/%s", PACKAGE_DATA_DIR,
            ecco.config->theme);
   for (l = ecco.config->sessions.keys; l; l = l->next)
   {
      str = (char *) l->data;
#if DEBUG
      fprintf(stderr, "Add session %s\n", str);
#endif
      if ((exs = evas_hash_find(ecco.config->sessions.hash, str)))
      {
         if ((o = entrance_x_session_edje_get(exs, container, buf)))
         {
            esmart_container_element_append(container, o);
         }
      }
   }
}

/**
 * setup the theme, user and session lists in the edje
 * @param o a pointer to an Evas_Object so we can add other objects to the
 * Evas
 */
static void
ecco_lists_init(Evas_Object * o)
{
   char buf[PATH_MAX];
   char *str = NULL;
   Evas_Object *oo = NULL;
   char *list_prefix = "ecco,container,";

   snprintf(buf, PATH_MAX, "%stheme", list_prefix);
   if ((str = evas_hash_find(ecco.hashes, buf)))
   {
      if ((oo = edje_object_part_swallow_get(o, str)))
      {
         ecco_dir_list_init(oo, PACKAGE_DATA_DIR "/themes");
         ecco.container.themes = oo;
      }
   }
   snprintf(buf, PATH_MAX, "%suser", list_prefix);
   if ((str = evas_hash_find(ecco.hashes, buf)))
   {
#if DEBUG
      fprintf(stderr, "Found %s\n", str);
#endif
      if ((oo = edje_object_part_swallow_get(o, str)))
      {
         ecco_users_list_init(oo);
         ecco.container.users = oo;
      }
   }
   snprintf(buf, PATH_MAX, "%ssession", list_prefix);
   if ((str = evas_hash_find(ecco.hashes, buf)))
   {
#if DEBUG
      fprintf(stderr, "Found %s\n", buf);
#endif
      if ((oo = edje_object_part_swallow_get(o, str)))
      {
         ecco_sessions_list_init(oo);
         ecco.container.sessions = oo;
      }
   }
}

/**
 * translate a source hash to a specific attribute in ecco
 * @param source the entry name associated with each attribute
 * @return a pointer to the associated theme string value
 */
static char *
ecco_source_to_key_translate(const char *source)
{
   char buf[PATH_MAX];

#if DEBUG
   fprintf(stderr, "Translate Source: %s\n", source);
#endif
   if (!strcmp("ecco,entry,focus,in,greeting,before", source))
      return (ecco.config->before.string);
   else if (!strcmp("ecco,entry,focus,in,greeting,after", source))
      return (ecco.config->after.string);
   else if (!strcmp("ecco,entry,focus,in,time", source))
      return (ecco.config->time.string);
   else if (!strcmp("ecco,entry,focus,in,date", source))
      return (ecco.config->date.string);
   else if (!strcmp("ecco,entry,focus,in,pointer", source))
      return (ecco.config->pointer);
   else if (!strcmp("ecco,entry,focus,in,theme", source))
      return (ecco.config->theme);
   else if (!strcmp("ecco,entry,focus,in,remember,n", source))
   {
      snprintf(buf, PATH_MAX, "%d", ecco.config->users.remember_n);
      return (strdup(buf));
   }
   return (NULL);
}

/**
 * initialize our entry/toggle/button hashes when ecco starts up
 * @param o An evas object so we can add more objects
 * This probably won't make a lick of sense. :)
 */
static void
ecco_hashes_init(Evas_Object * o)
{
   int i = 0;
   char buf[PATH_MAX];
   char *str = NULL;
   char *cstr = NULL;
   char *part = NULL;
   const char *file = NULL;
   const char *txt = NULL;
   Evas_Object *oo = NULL;
   Evas_Coord cw, ch;
   char *entries[] = { "theme",
      "greeting,before", "greeting,after", "date", "time", "pointer",
      "session,current,session", "session,current,icon",
      "session,current,name", "user,current,session",
      "user,current,icon", "user,current,name", "remember,n"
   };
   char *toggles[] = { "system,gl", "system,halt", "system,reboot",
      "system,users,remember", "system,auth,pam", "system,auth,shadow"
   };
   int toggles_vals[] = { ecco.config->engine, ecco.config->halt,
      ecco.config->reboot, ecco.config->users.remember,
      ecco.config->auth == 1, ecco.config->auth == 2
   };
   char *containers[] = { "user", "session", "theme" };
   int entries_count = sizeof(entries) / sizeof(char *);
   int toggles_count = sizeof(toggles) / sizeof(char *);
   int containers_count = sizeof(containers) / sizeof(char *);

   edje_object_file_get(o, &file, NULL);
   if (!file)
   {
      fprintf(stderr, "File fetching failed, err !!!\n");
      return;
   }
   /* setup entries */
   for (i = 0; i < entries_count; i++)
   {
      snprintf(buf, PATH_MAX, "ecco,entry,focus,in,%s", entries[i]);
      if ((part = edje_file_data_get(file, buf)))
      {
         if (edje_object_part_exists(o, part))
         {
            ecco.hashes = evas_hash_add(ecco.hashes, buf, part);
            ecco.entries = evas_hash_add(ecco.entries, part, strdup(buf));
            if ((txt = ecco_source_to_key_translate(buf)))
            {
               ecco.hashes = evas_hash_add(ecco.hashes, part, txt);
               edje_object_part_text_set(ecco.edje, part, txt);
#if DEBUG
               fprintf(stderr, "Part text is %s:%s\n", part, str);
#endif
            }
         }
         else
         {
            fprintf(stderr, "%s does not exist fix your theme\n", str);
         }
#if DEBUG
         fprintf(stderr, "%s : %s\n", buf, str);
#endif
      }
   }
   for (i = 0; i < entries_count; i++)
   {
      snprintf(buf, PATH_MAX, "ecco,entry,focus,out,%s", entries[i]);
      if ((str = edje_file_data_get(file, buf)))
      {
         ecco.hashes = evas_hash_add(ecco.hashes, buf, str);
#if DEBUG
         fprintf(stderr, "%s : %s\n", buf, str);
#endif
      }
      snprintf(buf, PATH_MAX, "ecco,entry,focus,out,emission");
      if ((str = edje_file_data_get(file, buf)))
      {
         ecco.focus.signal = str;
#if DEBUG
         fprintf(stderr, "%s : %s\n", buf, str);
#endif
      }
   }

   /* setup toggles */
   for (i = 0; i < toggles_count; i++)
   {
      snprintf(buf, PATH_MAX, "ecco,toggle,on,%s", toggles[i]);
      if ((str = edje_file_data_get(file, buf)))
      {
         if (edje_object_part_exists(o, str))
         {
            ecco.hashes = evas_hash_add(ecco.hashes, buf, str);
            if (toggles_vals[i])
            {
#if DEBUG
               fprintf(stderr, "Toggling on %s %d\n", str,
                       edje_object_play_get(ecco.edje));
#endif
               edje_object_signal_emit(ecco.edje, "mouse,clicked,1", str);
            }
         }
#if DEBUG
         fprintf(stderr, "%s : %s\n", buf, str);
#endif
      }
   }
   for (i = 0; i < toggles_count; i++)
   {
      snprintf(buf, PATH_MAX, "ecco,toggle,off,%s", toggles[i]);
      if ((str = edje_file_data_get(file, buf)))
      {
         ecco.hashes = evas_hash_add(ecco.hashes, buf, str);
#if DEBUG
         fprintf(stderr, "%s : %s\n", buf, str);
#endif
      }
   }

   /* setup containers */
   for (i = 0; i < containers_count; i++)
   {
      snprintf(buf, PATH_MAX, "ecco,container,%s", containers[i]);
#if DEBUG
      fprintf(stderr, "%s\n", buf);
#endif
      if ((str = edje_file_data_get(file, buf)))
      {
         ecco.hashes = evas_hash_add(ecco.hashes, buf, str);
         if (edje_object_part_exists(o, str))
         {
            snprintf(buf, PATH_MAX, "ecco,container,scroll,back,%s,start",
                     containers[i]);
            ecco.hashes = evas_hash_add(ecco.hashes, buf, str);
            snprintf(buf, PATH_MAX, "ecco,container,scroll,back,%s,stop",
                     containers[i]);
            ecco.hashes = evas_hash_add(ecco.hashes, buf, str);
            snprintf(buf, PATH_MAX, "ecco,container,scroll,next,%s,start",
                     containers[i]);
            ecco.hashes = evas_hash_add(ecco.hashes, buf, str);
            snprintf(buf, PATH_MAX, "ecco,container,scroll,next,%s,stop",
                     containers[i]);
            ecco.hashes = evas_hash_add(ecco.hashes, buf, str);
            if ((oo = esmart_container_new(evas_object_evas_get(o))))
            {
#if 0
               esmart_container_layout_plugin_set(oo, "entice");
#endif
               evas_object_layer_set(oo, 0);
               edje_object_part_geometry_get(o, str, NULL, NULL, &cw, &ch);
               esmart_container_fill_policy_set(oo,
                                                CONTAINER_FILL_POLICY_KEEP_ASPECT
                                                |
                                                CONTAINER_FILL_POLICY_HOMOGENOUS);
               esmart_container_padding_set(oo, 0, 0, 0, 0);
               esmart_container_spacing_set(oo, 4);
               snprintf(buf, PATH_MAX, "ecco,container,%s,direction",
                        containers[i]);
               if ((cstr = edje_file_data_get(file, buf)))
               {
#if DEBUG
                  fprintf(stderr, "Direction %s %s\n", buf, cstr);
#endif
                  if (!strcmp(cstr, "horizontal"))
                     esmart_container_direction_set(oo, 0);
                  else
                     esmart_container_direction_set(oo, 1);
                  free(cstr);
               }
#if DEBUG
               fprintf(stderr, "Swallowing to %s\n", str);
#endif
               if (!strcmp(containers[i], "user"))
                  ecco.container.users = oo;
               else if (!strcmp(containers[i], "session"))
                  ecco.container.sessions = oo;
               else if (!strcmp(containers[i], "theme"))
                  ecco.container.themes = oo;
               evas_object_show(o);
               edje_object_part_swallow(o, str, oo);
            }
         }
#if DEBUG
         fprintf(stderr, "%s : %s\n", buf, str);
#endif
      }
      snprintf(buf, PATH_MAX, "ecco,container,%s,dragbar", containers[i]);
      if ((str = edje_file_data_get(file, buf)))
      {
         ecco.hashes = evas_hash_add(ecco.hashes, buf, str);
#if DEBUG
         fprintf(stderr, "%s : %s\n", buf, str);
#endif
      }
   }
}

/**
 * main - where it all starts !
 * @param argc - the number of arguments ecco was called with
 * @param argv - the args ecco was called with 
 */
int
main(int argc, char *argv[])
{
   char buf[PATH_MAX];
   Evas *evas = NULL;
   Ecore_Evas *e = NULL;
   Evas_Object *o = NULL, *edje = NULL;
   Evas_Coord edjew, edjeh;

   /* nullify the original struct */
   ecco.edje = NULL;
   ecco.hashes = NULL;
   ecco.entry = NULL;
   ecco.config = NULL;
   ecco.focus.key = NULL;
   ecco.focus.signal = NULL;
   ecco.scroll_timer = NULL;
   ecco.current_user = NULL;
   ecco.current_session = NULL;

   /* Basic ecore initialization */
   if (!ecore_init())
      return (-1);
   if (ecore_config_init("entrance") != ECORE_CONFIG_ERR_SUCC)
   {
      ecore_shutdown();
      return -1;
   }
   ecore_app_args_set(argc, (const char **) argv);

   if ((ecco.config =
        entrance_config_load(PACKAGE_CFG_DIR "/entrance_config.cfg")) == NULL)
   {
      fprintf(stderr, "Unable to open %s/entrance_config.cfg\n",
              PACKAGE_CFG_DIR);
      exit(1);
   }
#if DEBUG
   entrance_config_print(ecco.config);
#endif

   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, exit_cb, NULL);
   if (ecore_evas_init())
   {
      /* init edje and set frametime to 60 fps ? */
      edje_init();
      edje_frametime_set(2.0 / 60.0);

#if 0
      e = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 320, 240);
#else
      e = ecore_evas_software_x11_new(NULL, 0, 0, 0, 320, 240);
#endif

      ecore_evas_title_set(e, "Entrance Edit - Configuring Entrance");
      ecore_evas_callback_delete_request_set(e, window_del_cb);
      ecore_evas_callback_resize_set(e, window_resize_cb);
      ecore_evas_callback_post_render_set(e, window_post_render_cb);
      ecore_evas_callback_pre_render_set(e, window_pre_render_cb);
      ecore_evas_cursor_set(e, PACKAGE_DATA_DIR "/images/pointer.png", 12, 0,
                            0);
      ecore_evas_move(e, 0, 0);

      /* Evas specific callbacks */
      evas = ecore_evas_get(e);
      evas_image_cache_set(evas, 8 * 1024 * 1024);
      evas_font_cache_set(evas, 1 * 1024 * 1024);
      evas_font_path_append(evas, PACKAGE_DATA_DIR "/fonts");
      evas_font_path_append(evas, PACKAGE_DATA_DIR "/data/fonts");

      edje = edje_object_add(evas);
      snprintf(buf, PATH_MAX, "%s/ecco/ecco.edj", PACKAGE_DATA_DIR);
      if (edje_object_file_set(edje, buf, "Main") > 0)
      {
         evas_object_move(edje, 0, 0);
         evas_object_name_set(edje, "ui");
         evas_object_layer_set(edje, 0);
         edje_object_size_min_get(edje, &edjew, &edjeh);
         if ((edjew > 0) && (edjeh > 0))
            ecore_evas_size_min_set(e, (int) edjew, (int) edjeh);
         ecore_evas_resize(e, (int) edjew, (int) edjeh);
         evas_object_resize(edje, edjew, edjeh);
         edje_object_size_max_get(edje, &edjew, &edjeh);
         if ((edjew > 0) && (edjeh > 0))
         {
            if (edjew > INT_MAX)
               edjew = INT_MAX;
            if (edjeh > INT_MAX)
               edjeh = INT_MAX;
            ecore_evas_size_max_set(e, (int) edjew, (int) edjeh);
         }
         evas_object_show(edje);
         ecco.edje = edje;
         ecco_hashes_init(edje);
         ecco_lists_init(edje);
         edje_object_signal_callback_add(edje, "ecco,entry,focus,in,*", "*",
                                         _key_focus_in, e);
         edje_object_signal_callback_add(edje, "ecco,entry,focus,out,*", "*",
                                         _key_focus_out, e);
         edje_object_signal_callback_add(edje, "ecco,show,*", "*", _show_cb,
                                         e);
         edje_object_signal_callback_add(edje, "ecco,toggle,on,*", "*",
                                         _system_toggle_on_cb, e);
         edje_object_signal_callback_add(edje, "ecco,toggle,off,*", "*",
                                         _system_toggle_off_cb, e);
         edje_object_signal_callback_add(edje, "ecco,button,global,*", "*",
                                         _button_global_cb, e);
         edje_object_signal_callback_add(edje, "ecco,button,browse,*", "*",
                                         _button_browse_cb, e);
         edje_object_signal_callback_add(edje, "ecco,button,add,*", "*",
                                         _button_add_cb, e);
         edje_object_signal_callback_add(edje, "ecco,button,delete,*", "*",
                                         _button_del_cb, e);
         edje_object_signal_callback_add(edje, "ecco,container,scroll,back,*",
                                         "*", _scroll_back_cb, e);
         edje_object_signal_callback_add(edje, "ecco,container,scroll,next,*",
                                         "*", _scroll_next_cb, e);
         edje_object_signal_callback_add(edje, "ecco,container,scrollbar,*",
                                         "*", _scrollbar_cb, e);

         o = esmart_text_entry_new(evas);
         evas_object_move(o, 0, 0);
         evas_object_resize(o, 200, 200);
         evas_object_layer_set(o, -1);
         esmart_text_entry_max_chars_set(o, PATH_MAX);
         esmart_text_entry_is_password_set(o, 0);
         evas_object_focus_set(o, 1);
         esmart_text_entry_return_key_callback_set(o, interp_return_key, o);
         ecco.entry = o;

         ecore_evas_show(e);
         ecore_main_loop_begin();
      }

#if 0
      if (edje_object_part_exists(edje, "entrance.time"))
      {
         edje_object_signal_callback_add(edje, "Go", "entrance.time",
                                         set_time, o);
         edje_object_signal_emit(edje, "Go", "entrance.time");
         timer = ecore_timer_add(0.5, timer_cb, edje);
      }
      if (edje_object_part_exists(edje, "entrance.date"))
      {
         edje_object_signal_callback_add(edje, "Go", "entrance.date",
                                         set_date, o);
         edje_object_signal_emit(edje, "Go", "entrance.date");
         if (!timer)
            timer = ecore_timer_add(0.5, timer_cb, edje);
      }
#endif

      edje_shutdown();
      ecore_evas_shutdown();
      ecore_config_shutdown();
      ecore_shutdown();
   }
   else
   {
      fprintf(stderr, "Fatal error: Could not initialize ecore_evas!\n");
      exit(1);
   }
   return (0);
}
