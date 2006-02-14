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
   if (d)
      free(d);
   if (ptr)
      free(ptr);
   return strdup(dir);
}

char *
parse_buffer(char *b, char *section)
{
   char *oldtoken, *token, *d;
   char t[MAX_PATH], p[MAX_PATH];
   int length, i;

   oldtoken = strdup(b);
   token = strstr(oldtoken, section);

   if (token == NULL)
      return NULL;

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
                t[i] = t[i + 1];
          }
     }

   if (strrchr(t, '"'))
     {
        d = get_t(t);
        snprintf(p, sizeof(p), "%s", d);
     }
   else
     {
        snprintf(p, sizeof(p), "%s", t);
     }

   if (token)
      free(token);
   if (oldtoken)
      free(oldtoken);
   if (d)
      free(d);
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
   char *home, *eap_name;
   int overwrite;
   G_Eap *eap;
   Ecore_Hash *desktop;

   home = get_home();
   overwrite = get_overwrite();
   eap_name = get_eap_name(path);

#ifdef DEBUG
   fprintf(stderr, "Parsing Desktop File %s\n", path);
#endif

   eap = calloc(1, sizeof(G_Eap));
   eap->eap_name = strdup(eap_name);
   if (eap_name)
      free(eap_name);

   desktop = parse_ini_file(path);
   if (desktop)
      {
         Ecore_Hash *group;

         group = (Ecore_Hash *) ecore_hash_get(desktop, "Desktop Entry");
	 if (group)
	    {
	       char *value;

               value = (char *) ecore_hash_get(group, "Name");
	       if (value)   eap->name = strdup(value);
               value = (char *) ecore_hash_get(group, "GenericName");
	       if (value)   eap->generic = strdup(value);
               value = (char *) ecore_hash_get(group, "Comment");
	       if (value)   eap->comment = strdup(value);
               value = (char *) ecore_hash_get(group, "Type");
	       if (value)   eap->type = strdup(value);
               value = (char *) ecore_hash_get(group, "Categories");
	       if (value)   eap->categories = strdup(value);
               value = (char *) ecore_hash_get(group, "Exec");
	       if (value)   eap->exec = strdup(value);
               value = (char *) ecore_hash_get(group, "Icon");
	       if (value)   eap->icon = strdup(value);
               value = (char *) ecore_hash_get(group, "X-KDE-StartupNotify");
	       if (value)   eap->startup = (!strcmp(value, "true")) ? "1" : "0";
               value = (char *) ecore_hash_get(group, "StartupNotify");
	       if (value)   eap->startup = (!strcmp(value, "true")) ? "1" : "0";
            }
      }

   /* Check If We Process */
   if (!eap->type)
      process_file(path, eap);
   if (eap->type)
     {
        if (!strcmp(eap->type, "Application"))
           process_file(path, eap);
     }

   /* Write Out Mapping File ? */
   if (get_mapping())
     {
        if ((!eap->icon) || (!eap->name) ||
            (!eap->exec) || (!eap->window_class))
           return;

        FILE *f;
        char buff[MAX_PATH];

        snprintf(path, sizeof(path), "%s/MAPPING", home);
        f = fopen(path, "a+");
        if (!f)
          {
             fprintf(stderr, "ERROR: Cannot Open Mapping File\n");
             return;
          }
        snprintf(buff, sizeof(buff), "%s|!%s|!%s|!%s\n",
                 eap->icon, eap->name, eap->window_class, eap->exec);
        fwrite(buff, sizeof(char), strlen(buff), f);
        fclose(f);
     }
   free(eap);
}

void
process_file(char *file, G_Eap *eap)
{
   char *home, *window_class, *exec, *category, *icon;
   char path[MAX_PATH], order_path[MAX_PATH], buffer[MAX_PATH];
   int overwrite;

   if (!eap)
      return;

#ifdef DEBUG
   fprintf(stderr, "Processing File %s\n", file);
#endif

   home = get_home();
   overwrite = get_overwrite();

   snprintf(path, sizeof(path), "%s" EAPPDIR "/%s", home, eap->eap_name);
   window_class = get_window_class(path);

   if ((ecore_file_exists(path)) && (!overwrite))
     {
        category = NULL;
        if (eap->categories != NULL)
          {
             category = find_category(eap->categories);
             if (category != NULL)
               {
                  snprintf(order_path, sizeof(order_path), "%s" EFAVDIR "/%s",
                           home, category);
                  modify_order(order_path, eap->eap_name);
               }
          }
        return;
     }

   if (eap->icon != NULL)
      icon = find_icon(strdup(eap->icon));
   if (eap->icon == NULL)
      icon = strdup(DEFAULTICON);

   /* Start Making The Eap */
   write_icon(path, icon);

   /* Set Eap Values. Trap For Name Not Being Set */
   if (eap->name != NULL)
      write_eap(path, "app/info/name", eap->name);
   else if (eap->eap_name != NULL)
      write_eap(path, "app/info/name", eap->eap_name);

   if (eap->generic != NULL)
      write_eap(path, "app/info/generic", eap->generic);
   if (eap->comment != NULL)
      write_eap(path, "app/info/comments", eap->comment);

   /* Parse Exec string for %'s that messup eap write */
   exec = NULL;
   if (eap->exec != NULL)
     {
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

   snprintf(buffer, sizeof(buffer),
            "enlightenment_eapp_cache_gen %s/.e/e/applications/all -r",
            getenv("HOME"));
   system(buffer);

   category = NULL;
   if (eap->categories != NULL)
     {
        category = find_category(eap->categories);
        if (category != NULL)
          {
             snprintf(buffer, sizeof(buffer),
                      "enlightenment_eapp_cache_gen %s/.e/e/applications/favorite/%s -r",
                      getenv("HOME"), category);
             system(buffer);
             snprintf(order_path, sizeof(order_path), "%s" EFAVDIR "/%s", home,
                      category);
             modify_order(order_path, eap->eap_name);
          }
     }
   return;
}

void
parse_debian_file(char *file)
{
   char *eap_name, *name, *generic, *comment, *exec, *category, *icon;
   char buffer[MAX_PATH];
   int overwrite, length;
   FILE *f;
   G_Eap *eap;

   if (ecore_file_is_dir(file))
      return;

   overwrite = get_overwrite();
   eap_name = get_eap_name(file);

#ifdef DEBUG
   fprintf(stderr, "Parsing Debian File %s\n", file);
#endif

   f = fopen(file, "r");
   if (!f)
     {
        fprintf(stderr, "ERROR: Cannot Open File %s\n", file);
        return;
     }

   eap = calloc(1, sizeof(G_Eap));
   eap->eap_name = eap_name;
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

Ecore_Hash *
parse_ini_file(char *file)
{
   Ecore_Hash *result;

   /* FIXME: this should probably be optimised by caching the results, then looking in the cache first. */
   result = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   if (result)
     {
        FILE *f;
        char buffer[MAX_PATH];
        Ecore_Hash *current = NULL;

        f = fopen(file, "r");
        if (!f)
          {
             fprintf(stderr, "ERROR: Cannot Open File %s\n", file);
             ecore_hash_destroy(result);
             return NULL;
          }
        ecore_hash_set_free_key(result, free);
        ecore_hash_set_free_value(result, (Ecore_Free_Cb) ecore_hash_destroy);
        *buffer = '\0';
#ifdef DEBUG
        fprintf(stdout, "PARSING INI %s\n", file);
#endif
        while (fgets(buffer, sizeof(buffer), f) != NULL)
          {
             char *c;
             char *key;
             char *value;

             c = buffer;
             /* Strip preceeding blanks. */
             while (((*c == ' ') || (*c == '\t')) && (*c != '\n')
                    && (*c != '\0'))
                c++;
             /* Skip blank lines and comments */
             if ((*c == '\0') || (*c == '\n') || (*c == '#'))
                continue;
             if (*c == '[')     /* New group. */
               {
                  key = c + 1;
                  while ((*c != ']') && (*c != '\n') && (*c != '\0'))
                     c++;
                  *c++ = '\0';
                  current = ecore_hash_new(ecore_str_hash, ecore_str_compare);
                  if (current)
                    {
                       ecore_hash_set_free_key(current, free);
                       ecore_hash_set_free_value(current, free);
                       ecore_hash_set(result, strdup(key), current);
#ifdef DEBUG
                       fprintf(stdout, "  GROUP [%s]\n", key);
#endif
                    }
               }
             else if (current)  /* key=value pair of current group. */
               {
                  key = c;
                  /* Find trailing blanks or =. */
                  while ((*c != '=') && (*c != ' ') && (*c != '\t')
                         && (*c != '\n') && (*c != '\0'))
                     c++;
                  if (*c != '=')        /* Find equals. */
                    {
                       *c++ = '\0';
                       while ((*c != '=') && (*c != '\n') && (*c != '\0'))
                          c++;
                    }
                  if (*c == '=')        /* Equals found. */
                    {
                       *c++ = '\0';
                       /* Strip preceeding blanks. */
                       while (((*c == ' ') || (*c == '\t')) && (*c != '\n')
                              && (*c != '\0'))
                          c++;
                       value = c;
                       /* Find end. */
                       while ((*c != '\n') && (*c != '\0'))
                          c++;
                       *c++ = '\0';
                       /* FIXME: should strip space at end, then unescape value. */
                       ecore_hash_set(current, strdup(key), strdup(value));
#ifdef DEBUG
                       fprintf(stdout, "    %s=%s\n", key, value);
#endif
                    }
               }

          }
        buffer[0] = (char)0;

        fclose(f);
     }
   return result;
}
