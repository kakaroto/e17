/**************************************************************************
 * Name: util.c 
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * Date: October 10, 2001
 * Description: DB Utility functions.  Parsing of menu dbs to a and Ewd_List
 *  of defined structure, emenu_item,  from emenu_item.h.  Parsing is also
 *  done of the user's actions db.  And an Ewd_List with nodes of a defined
 *  type, eaction_item, from ekeybindings.h.  Reading and writing to the
 *  database for both actions and menus is done here.
 *************************************************************************/

#include"util.h"

static int menu_count;

/* 
 * used to keep track of submenu's submenus in recursive database writing
 * for menus 
 */

/**************************************************************************
 * Actions db
 *************************************************************************/

/* Reads the user's action.db into the global structure action_container
 */
int
parse_user_actions_db(void)
{
   E_DB_File *db;
   char dbname[1024];
   int num, i;
   eaction_item *item;

   action_container_init();
   /* setup the two lists and their cbs */
   snprintf(dbname, 1024, "%s/.e/behavior/actions.db", getenv("HOME"));
   /* This could be back if user export HOME wrong, snprintf it */
   db = e_db_open_read(dbname);
   if (!db)
   {
      fprintf(stderr,
              "ERROR: Unable to read your actions database.\n"
              "%s is the file I'm looking for\nIs the "
              "environmental variable HOME set wrong?.\n", dbname);
      return 1;
   }

   e_db_int_get(db, "/actions/count", &num);
   for (i = 0; i < num; i++)
   {
      char buf[1024];

      item = eaction_item_new();

      sprintf(buf, "/actions/%i/name", i);
      item->name = e_db_str_get(db, buf);
      sprintf(buf, "/actions/%i/action", i);
      item->action = e_db_str_get(db, buf);
      sprintf(buf, "/actions/%i/params", i);
      item->params = e_db_str_get(db, buf);
      sprintf(buf, "/actions/%i/key", i);
      item->key = e_db_str_get(db, buf);

      sprintf(buf, "/actions/%i/modifiers", i);
      e_db_int_get(db, buf, &item->modifiers);
      sprintf(buf, "/actions/%i/button", i);
      e_db_int_get(db, buf, &item->button);
      sprintf(buf, "/actions/%i/event", i);
      e_db_int_get(db, buf, &item->event);

      if ((item->key) && (strlen(item->key) > 0))
         ewd_list_append(action_container.keys, item);
      else
         ewd_list_append(action_container.focus, item);
   }
   e_db_close(db);
   return 0;
}

/* Write the user's changes to their actions.db file.  Clear out the old db
 * and write.
 */
int
write_user_actions_db(void)
{
   E_DB_File *db;
   char dbname[1024];
   char buf[4096];
   char **keys;
   int i, key_count;
   eaction_item *item;

   snprintf(dbname, 4096, "%s/.e/behavior/actions.db", getenv("HOME"));
   db = e_db_open(dbname);

   if (!db)
   {
      fprintf(stderr,
              "ERROR: Unable to write to your actions database.\n"
              "%s is the file I'm looking for\nIs the "
              "environmental variable HOME set wrong?.\n", dbname);
      return 1;
   }
   keys = e_db_dump_key_list(dbname, &key_count);
   if (keys)
   {
      int j;

      for (j = 0; j < key_count; j++)
      {
         e_db_data_del(db, keys[j]);
         free(keys[j]);
      }
      free(keys);
   }
   /* clear out the old stuff first */
   ewd_list_goto_first(action_container.keys);
   ewd_list_goto_first(action_container.focus);
   /* make sure we start at the beginning of our lists */

   for (i = 0; (item = (eaction_item *) ewd_list_next(action_container.focus));
        i++)
      /* write non keybind actions first */
   {
      sprintf(buf, "/actions/%i/name", i);
      e_db_str_set(db, buf, item->name);
      sprintf(buf, "/actions/%i/action", i);
      e_db_str_set(db, buf, item->action);
      sprintf(buf, "/actions/%i/params", i);
      if (item->params)
         e_db_str_set(db, buf, item->params);
      sprintf(buf, "/actions/%i/key", i);
      if (item->key)
         e_db_str_set(db, buf, item->key);

      sprintf(buf, "/actions/%i/modifiers", i);
      e_db_int_set(db, buf, item->modifiers);
      sprintf(buf, "/actions/%i/button", i);
      e_db_int_set(db, buf, item->button);
      sprintf(buf, "/actions/%i/event", i);
      e_db_int_set(db, buf, item->event);
   }
   for (; (item = (eaction_item *) ewd_list_next(action_container.keys)); i++)
   {
      /* write keybinds next */
      sprintf(buf, "/actions/%i/name", i);
      e_db_str_set(db, buf, item->name);
      sprintf(buf, "/actions/%i/action", i);
      e_db_str_set(db, buf, item->action);
      sprintf(buf, "/actions/%i/params", i);
      if (item->params)
         e_db_str_set(db, buf, item->params);
      sprintf(buf, "/actions/%i/key", i);
      if (item->key)
         e_db_str_set(db, buf, item->key);

      sprintf(buf, "/actions/%i/modifiers", i);
      e_db_int_set(db, buf, item->modifiers);
      sprintf(buf, "/actions/%i/button", i);
      e_db_int_set(db, buf, item->button);
      sprintf(buf, "/actions/%i/event", i);
      e_db_int_set(db, buf, item->event);
   }
   e_db_int_set(db, "/actions/count", i);

   e_db_close(db);
   e_db_flush();
   return 0;
}

/**************************************************************************
 * Menus parsed from here down =)
 *************************************************************************/

/* read_emenu_from_db: recursive function to read menu dbs
 * db: a E_DB_File open in read only mode, it doesn't try to write to it,
 * 	but if we're only retrieving data it's no big deal anways.
 * _l: an Ewd_List of emenu_items
 * :menu_count: parameter used in the recursive call to specify which submenu
 * 	should be parsed.
 */
static void
read_emenu_from_db(E_DB_File * db, Ewd_List * _l, int menu_count)
{
   char buf[4096];
   emenu_item *current = NULL;
   int j = 0, max, ok, submenu, seperator;

   sprintf(buf, "/menu/%d/count", menu_count);
   e_db_int_get(db, buf, &max);

   for (j = 0; j < max; j++)
   {
      submenu = seperator = ok = 0;
      current = emenu_item_new();

      sprintf(buf, "/menu/%i/%i/separator", menu_count, j);
      ok = e_db_int_get(db, buf, &seperator);
      if (ok)
      {
         current->type = E_MENU_SEPARATOR;
      }
      /* if it's a seperator we just append it */
      else
      {
         sprintf(buf, "/menu/%i/%i/command", menu_count, j);
         current->exec = e_db_str_get(db, buf);
         sprintf(buf, "/menu/%i/%i/icon", menu_count, j);
         current->icon = e_db_str_get(db, buf);
         sprintf(buf, "/menu/%i/%i/text", menu_count, j);
         current->text = e_db_str_get(db, buf);

         sprintf(buf, "/menu/%i/%i/submenu", menu_count, j);
         ok = e_db_int_get(db, buf, &submenu);
         if (ok)
         {
            current->type = E_MENU_SUBMENU;
            read_emenu_from_db(db, current->children, submenu);
         }

         /* handle scripts */
         sprintf(buf, "/menu/%i/%i/script", menu_count, j);
         ok = e_db_int_get(db, buf, &submenu);
         if (ok)
         {
            current->type = E_MENU_SCRIPT;
            sprintf(buf, "/menu/%i/%i/script", menu_count, j);
            current->exec = e_db_str_get(db, buf);
         }
      }
      ewd_list_append(_l, current);
   }
}

/* recursive write function call */
static void
write_emenu_to_db_with_ewd_list(E_DB_File * db, Ewd_List * l, int menu_level)
{
   emenu_item *e;
   char buf[4096];
   int i;

   if (!l)
      return;
   ewd_list_goto_first(l);

   for (i = 0; (e = (emenu_item *) ewd_list_next(l)); i++)
   {
      if (!e)
         continue;
      if (!e->text && e->type != E_MENU_SEPARATOR)
      {
         --i;
         continue;
      }
      switch (e->type)
      {
        case E_MENU_SCRIPT:
           sprintf(buf, "/menu/%i/%i/script", menu_level, i);
           e_db_str_set(db, buf, e->exec);
           sprintf(buf, "/menu/%i/%i/text", menu_level, i);
           e_db_str_set(db, buf, e->text);
           if (e->icon)
           {
              sprintf(buf, "/menu/%i/%i/icon", menu_level, i);
              e_db_str_set(db, buf, e->icon);
           }
           break;
        case E_MENU_SEPARATOR:
           sprintf(buf, "/menu/%i/%i/separator", menu_level, i);
           e_db_int_set(db, buf, 1);
           break;
        case E_MENU_SUBMENU:
           sprintf(buf, "/menu/%i/%i/submenu", menu_level, i);
           e_db_int_set(db, buf, ++menu_count);

           if (e->icon)
           {
              sprintf(buf, "/menu/%i/%i/icon", menu_level, i);
              e_db_str_set(db, buf, e->icon);
           }
           if (e->text)
           {
              sprintf(buf, "/menu/%i/%i/text", menu_level, i);
              e_db_str_set(db, buf, e->text);
           }
           write_emenu_to_db_with_ewd_list(db, e->children, menu_count);
           break;
        default:
           sprintf(buf, "/menu/%i/%i/command", menu_level, i);
           e_db_str_set(db, buf, e->exec);
           sprintf(buf, "/menu/%i/%i/text", menu_level, i);
           e_db_str_set(db, buf, e->text);
           if (e->icon)
           {
              sprintf(buf, "/menu/%i/%i/icon", menu_level, i);
              e_db_str_set(db, buf, e->icon);
           }
           break;
      }
      e = NULL;
   }
   sprintf(buf, "/menu/%i/count", menu_level);
   e_db_int_set(db, buf, i);
   e_db_int_set(db, "/menu/count", menu_count + 1);
}

/* Populates the user's menu db to this Ewd_List pass to it. */
int
read_user_menu_db_into_ewd_list(Ewd_List * l)
{
   E_DB_File *db;
   char dbname[1024];

   snprintf(dbname, 1024, "%s/.e/behavior/apps_menu.db", getenv("HOME"));
   /* This could be back if user export HOME wrong, snprintf it */

   db = e_db_open_read(dbname);
   if (!db)
   {
      fprintf(stderr,
              "ERROR: Unable to read your menu database.\n"
              "%s is the file I'm looking for\nIs the "
              "environmental variable HOME set wrong?.\n", dbname);
      return 1;
   }

   read_emenu_from_db(db, l, 0);
   e_db_close(db);
   return 0;
}

/* writes the user's menu db from the Ewd_List passed to it. */
int
write_user_menu_db_with_ewd_list(Ewd_List * l)
{
   E_DB_File *db;
   char dbname[1024];

   sprintf(dbname, "%s/.e/behavior/apps_menu.db", getenv("HOME"));

   db = e_db_open(dbname);
   if (!db)
   {
      fprintf(stderr,
              "ERROR: Unable to write to your menu database.\n"
              "%s is the file I'm looking for\nIs the "
              "environmental variable HOME set wrong?.\n", dbname);
      return 1;
   }
   else
   {
      char **keys;
      int key_count;

      /* clean out the current db first to prevent weirdness */
      keys = e_db_dump_key_list(dbname, &key_count);
      if (keys)
      {
         int i;

         for (i = 0; i < key_count; i++)
         {
            e_db_data_del(db, keys[i]);
            free(keys[i]);
         }

         free(keys);
      }

      menu_count = 0;
      write_emenu_to_db_with_ewd_list(db, l, menu_count);
      e_db_close(db);
      e_db_flush();
      return 0;
   }
}
