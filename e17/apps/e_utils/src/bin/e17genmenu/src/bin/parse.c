#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "global.h"
#include "config.h"
#include "category.h"
#include "eaps.h"
#include "icons.h"
#include "order.h"
#include "parse.h"

char *
get_t(char *icon)
{
   char *ptr;
   char *d;
   char *dir;
   int i = 0;

   ptr = icon;
   d = strrchr(ptr, '"');
   if (!d)
      return NULL;

   dir = malloc(strlen(icon) + 1);
   while (ptr != d)
     {
        dir[i] = *ptr;
        ptr++;
        i++;
     }
   dir[i] = '\0';
   return strdup(dir);
}

char *
parse_buffer(char *b, char *section)
{
   char *oldtoken, *token, *d;
   char t[PATH_MAX], p[PATH_MAX];
   int length, i;

   oldtoken = strdup(b);
   token = strstr(oldtoken, section);

   if (token == NULL)
      return b;

   token = strstr(token, "=");
   token = strtok(token, "=");
   snprintf(t, sizeof(t), "%s", token);

   d = strrchr(t, '\\');
   if (d)
     {
        if (t[(length = strlen(t) - 1)] == '\\')
           t[length] = '\0';
     }

   d = strrchr(t, '\"');
   if (d)
     {
        if (t[(length = strlen(t) - 1)] == '\"')
           t[length] = '\0';
     }

   d = strchr(t, '"');
   if (d)
     {
        if (t[0] == '"')
          {
             for (i = 0; i < strlen(t); i++)
               {
                  t[i] = t[i + 1];
               }
          }
     }

   if (strrchr(t, '"'))
      d = get_t(t);
   if (d)
     {
        snprintf(p, sizeof(p), "%s", d);
     }
   else
     {
        snprintf(p, sizeof(p), "%s", t);
     }
   return strdup(p);
}

char *
parse_exec(char *exec)
{
   char *token;

   token = strdup(exec);
   /* Strip Caption From Exec */
   if (strstr(token, "caption") != NULL)
     {
        token = strdup(token);
        token = strtok(token, " ");
     }

   /* Strip %U from Exec */
   if (strstr(token, "%") != NULL)
     {
        token = strdup(token);
        token = strtok(token, " ");
     }
   return strdup(token);
}

void
parse_desktop_file(char *path)
{
   char *home, *eap_name, *key, *value;
   char buffer[PATH_MAX];
   int length, overwrite;
   FILE *file;
   G_Eap *eap;

   home = get_home();
   overwrite = get_overwrite();

#ifdef DEBUG
   fprintf(stderr, "Parsing Desktop File %s\n", path);
#endif

   file = fopen(path, "r");
   if (!file)
     {
        fprintf(stderr, "ERROR: Cannot Open File %s \n", path);
        return;
     }

   eap = calloc(1, sizeof(G_Eap));
   key = NULL;
   value = NULL;
   *buffer = 0;

   /* Read All Entries, Store In Struct */
   while (fgets(buffer, sizeof(buffer), file) != NULL)
     {
        /* Skip Blank Lines */
        if (!(*buffer) || (*buffer == '\n'))
           continue;
        /* Strip New Line Char */
        if (buffer[(length = strlen(buffer) - 1)] == '\n')
           buffer[length] = '\0';
        key = strtok(buffer, "=");
        value = strstr(buffer, "=");
        value = strtok(value, "=");
        if ((value != NULL) && (key != NULL))
          {
             if (!strcmp(key, "Name"))
                eap->name = strdup(value);
             if (!strcmp(key, "GenericName"))
                eap->generic = strdup(value);
             if (!strcmp(key, "Comment"))
                eap->comment = strdup(value);
             if (!strcmp(key, "Categories"))
                eap->categories = strdup(value);
             if (!strcmp(key, "Exec"))
                eap->exec = strdup(value);
             if (!strcmp(key, "Icon"))
                eap->icon = strdup(value);
             if (!strcmp(key, "StartupNotify"))
                eap->startup = (!strcmp(value, "true")) ? "1" : "0";
             if (!strcmp(key, "X-KDE-StartupNotify"))
                eap->startup = (!strcmp(value, "true")) ? "1" : "0";
             if (!strcmp(key, "Type"))
                eap->type = strdup(value);
          }
        value = NULL;
        key = NULL;
     }
   fclose(file);

   eap_name = get_eap_name(path);
   if (eap_name)
      eap->eap_name = strdup(eap_name);

   /* Check If We Process */
   if ((!strcmp(eap->type, "Application")) || (!eap->type))
      process_file(path, eap);

   /* Write Out Mapping File ? */
   if (get_mapping())
      write_mapping_file(eap);

   free(eap);
}

void
process_file(char *file, G_Eap *eap)
{
   char *home, *window_class, *exec, *category, *icon;
   char path[PATH_MAX], order_path[PATH_MAX];
   int overwrite;

   if (!eap)
      return;

   fprintf(stderr, "Processing File %s\n", file);

   home = get_home();
   overwrite = get_overwrite();

   if (eap->eap_name == NULL)
     {
        printf("Eap Null\n");
        snprintf(path, sizeof(path), "%s.eap", eap->name);
        eap->eap_name = strdup(path);
     }

   snprintf(path, sizeof(path), "%s" EAPPDIR "/%s", home, eap->eap_name);
   window_class = get_window_class(path);

   if ((ecore_file_exists(path)) && (!overwrite))
     {
        if (eap->categories != NULL)
          {
             category = NULL;
             category = find_category(eap->categories);
             if (category != NULL)
               {
                  snprintf(order_path, sizeof(order_path), "%s" EFAVDIR "/%s",
                           home, category);
                  if (eap->eap_name != NULL)
                     modify_order(order_path, eap->eap_name);
               }
          }
        return;
     }

   /* Start Making The Eap */
   if (!eap->icon)
      eap->icon = strdup(DEFAULTICON);

   icon = find_icon(strdup(eap->icon));
   write_icon(path, icon);

   /* Set Eap Values. Trap For Name Not Being Set */
   if (eap->name != NULL)
     {
        write_eap(path, "app/info/name", eap->name);
     }
   else if (eap->eap_name != NULL)
     {
        write_eap(path, "app/info/name", eap->eap_name);
     }

   if (eap->generic != NULL)
      write_eap(path, "app/info/generic", eap->generic);
   if (eap->comment != NULL)
      write_eap(path, "app/info/comments", eap->comment);

   /* Parse Exec string for %'s that messup eap write */
   if (eap->exec != NULL)
     {
        exec = NULL;
        exec = parse_exec(eap->exec);
        if (exec != NULL)
          {
             write_eap(path, "app/info/exe", exec);
             write_eap(path, "app/icon/class", exec);
          }
     }

   if (eap->startup != NULL)
      write_eap(path, "app/info/startup_notify", eap->startup);
   if (window_class != NULL)
      write_eap(path, "app/window/class", window_class);

   if (eap->categories != NULL)
     {
        category = NULL;
        category = find_category(eap->categories);
        if (category != NULL)
          {
             snprintf(order_path, sizeof(order_path), "%s" EFAVDIR "/%s", home,
                      category);
             if (eap->eap_name != NULL)
                modify_order(order_path, eap->eap_name);
          }
     }
   return;
}

void
parse_debian_file(char *file)
{
   char *name, *generic, *comment, *exec, *category, *icon;
   char buffer[PATH_MAX];
   int overwrite, length;
   FILE *f;
   G_Eap *eap;

   if (ecore_file_is_dir(file))
      return;

   overwrite = get_overwrite();

   fprintf(stderr, "Parsing Debian File %s\n", file);

   f = fopen(file, "r");
   if (!f)
     {
        fprintf(stderr, "ERROR: Cannot Open File %s\n", file);
        return;
     }

   eap = calloc(1, sizeof(G_Eap));
   *buffer = 0;
   while (fgets(buffer, sizeof(buffer), f) != NULL)
     {
        /* Skip Blank Lines */
        if (!(*buffer) || (*buffer == '\n'))
           continue;
        /* Strip New Line Chars */
        if (buffer[(length = strlen(buffer) - 1)] == '\n')
           buffer[length] = '\0';
        if (strstr(buffer, "title"))
          {
             name = parse_buffer(strdup(buffer), "title=");
             eap->name = strdup(name);
          }
        if (strstr(buffer, "longtitle"))
          {
             generic = parse_buffer(strdup(buffer), "longtitle=");
             eap->generic = strdup(generic);
          }
        if (strstr(buffer, "description"))
          {
             comment = parse_buffer(strdup(buffer), "description=");
             eap->comment = strdup(comment);
          }
        if (strstr(buffer, "section"))
          {
             category = parse_buffer(strdup(buffer), "section=");
             eap->categories = strdup(category);
          }
        if (strstr(buffer, "command"))
          {
             exec = parse_buffer(strdup(buffer), "command=");
             eap->exec = strdup(exec);
          }
        if (strstr(buffer, "icon"))
          {
             icon = parse_buffer(strdup(buffer), "icon=");
             eap->icon = strdup(icon);
          }
     }
   buffer[0] = (char)0;

   fclose(f);

   process_file(file, eap);
   free(eap);
}
