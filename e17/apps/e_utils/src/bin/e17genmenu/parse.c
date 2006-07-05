#include "global.h"
#include "config.h"
#include "category.h"
#include "eaps.h"
#include "icons.h"
#include "order.h"
#include "parse.h"

extern int reject_count, not_over_count;

static void _parse_desktop_del(Desktop * desktop);

#if 0
char *
get_t(char *icon)
{
   char *ptr;
   char *d;
   char *dir;
   int i = 0;

   if ((!icon) || (icon == NULL)) 
      return NULL;
   
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
   if (ptr)
      free(ptr);
   return strdup(dir);
}
#endif

char *
parse_buffer(char *b, char *section)
{
   char *oldtoken, *token, *substr, *substr2, *str_ret;

   oldtoken = strdup(b);
   substr = strstr(oldtoken, section);

   if (substr == NULL)
      return NULL;

   substr2 = strstr(substr, "=");
   if (!substr2) return NULL;
   if (*(++substr2) == '\0') return NULL;
   token = strtok(substr2, "\"");
#if 0
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
#endif

   str_ret = strdup(token); 
   
   if (oldtoken)
      free(oldtoken);
   return str_ret;
}

void
parse_desktop_file(char *app, char *menu_path)
{
   char *home;
   G_Eap *eap;
   Desktop *desktop;

   home = get_home();

#ifdef DEBUG
   fprintf(stderr, "Parsing Desktop File %s\n", app);
#endif

   eap = calloc(1, sizeof(G_Eap));
   eap->eap_name = get_eap_name(app);

   desktop = parse_desktop_ini_file(app);
   if ((desktop) && (desktop->group))
     {
        char *value;

        value = (char *)ecore_hash_get(desktop->group, "Name");
        if (value)
           eap->name = strdup(value);
        value = (char *)ecore_hash_get(desktop->group, "GenericName");
        if (value)
           eap->generic = strdup(value);
        value = (char *)ecore_hash_get(desktop->group, "Comment");
        if (value)
           eap->comment = strdup(value);
        value = (char *)ecore_hash_get(desktop->group, "Type");
        if (value)
           eap->type = strdup(value);
        value = (char *)ecore_hash_get(desktop->group, "Categories");
        if (value)
           eap->categories = strdup(value);
        value = (char *)ecore_hash_get(desktop->group, "Exec");
        if (value)
           eap->exec = strdup(value);
        value = (char *)ecore_hash_get(desktop->group, "Icon");
        if (value)
           eap->icon = strdup(value);
        value = (char *)ecore_hash_get(desktop->group, "X-KDE-StartupNotify");
        if (value)
           eap->startup = (!strcmp(value, "true")) ? "1" : "0";
        value = (char *)ecore_hash_get(desktop->group, "StartupNotify");
        if (value)
           eap->startup = (!strcmp(value, "true")) ? "1" : "0";
        value = (char *)ecore_hash_get(desktop->group, "StartupWMClass");
        if (value)
           eap->window_class = strdup(value);
     }

   /* Check If We Process */
   if (!eap->type)
      process_file(app, menu_path, eap);
   if (eap->type)
     {
        if (!strcmp(eap->type, "Application"))
           process_file(app, menu_path, eap);
     }

   /* Write Out Mapping File ? */
   if (get_mapping())
     {
        if ((!eap->icon) || (!eap->name) || (!eap->exec) || (!eap->window_class))
          {
             reject_count++;
             return;
          }

        FILE *f;
        char buff[MAX_PATH];
        char map_path[MAX_PATH];

        snprintf(map_path, sizeof(map_path), "%s/MAPPING", home);
        f = fopen(map_path, "a+");
        if (!f)
          {
             fprintf(stderr, "ERROR: Cannot Open Mapping File\n");
             return;
          }
        snprintf(buff, sizeof(buff), "%s|!%s|!%s|!%s\n", eap->icon, eap->name, eap->window_class, eap->exec);
        fwrite(buff, sizeof(char), strlen(buff), f);
        fclose(f);
     }
   free(eap);
}

void
process_file(char *file, char *menu_path, G_Eap *eap)
{
   char *home, *category;
   char path[MAX_PATH], order_path[MAX_PATH];
   int overwrite;

   if (!eap)
      return;

#ifdef DEBUG
   fprintf(stderr, "Processing File %s\n", file);
#endif

   home = get_home();
   overwrite = get_overwrite();

   snprintf(path, sizeof(path), "%s" EAPPDIR "/%s", home, eap->eap_name);

   if (eap->icon != NULL)
      eap->icon_path = find_icon(eap->icon);

   if ((ecore_file_exists(path)) && (!overwrite))
        not_over_count++;
   else
        write_icon(path, eap);

   category = NULL;
   if (menu_path != NULL)
     {
        snprintf(order_path, sizeof(order_path), "%s" EFAVDIR "/Converted Menus/%s", home, menu_path);
        modify_order(order_path, eap->eap_name);
     }
   else if (eap->categories != NULL)
     {
        category = find_category(eap->categories);
        if (category != NULL)
          {
             snprintf(order_path, sizeof(order_path), "%s" EFAVDIR "/Generated Menus/%s", home, category);
             modify_order(order_path, eap->eap_name);
          }
     }
   return;
}

void
parse_debian_file(char *file)
{
   char *name, *generic, *comment, *exec, *category, *icon;
   char buffer[MAX_PATH];
   int overwrite, length;
   FILE *f;
   G_Eap *eap;

   if (ecore_file_is_dir(file))
      return;

   memset(buffer, 0, sizeof(buffer));
   overwrite = get_overwrite();

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
   eap->eap_name = get_eap_name(file);
   *buffer = 0;
   while (fgets(buffer, sizeof(buffer), f) != NULL)
     {
        /* Skip Blank Lines */
        if (!(*buffer) || (*buffer == '\n'))
           continue;
        /* Strip New Line Chars */
        length = strlen(buffer);
        if (buffer[length - 1] == '\n')
           buffer[length - 1] = '\0';
        if (strstr(buffer, "title"))
          {
             name = parse_buffer(buffer, "title=");
             eap->name = strdup(name);
          }
        if (strstr(buffer, "longtitle"))
          {
             generic = parse_buffer(buffer, "longtitle=");
             eap->generic = strdup(generic);
          }
        if (strstr(buffer, "description"))
          {
             comment = parse_buffer(buffer, "description=");
             eap->comment = strdup(comment);
          }
        if (strstr(buffer, "section"))
          {
             category = parse_buffer(buffer, "section=");
             eap->categories = strdup(category);
          }
        if (strstr(buffer, "command"))
          {
             exec = parse_buffer(buffer, "command=");
             eap->exec = strdup(exec);
          }
        if (strstr(buffer, "icon"))
          {
             icon = parse_buffer(buffer, "icon128x128");
             if (!icon) icon = parse_buffer(buffer, "icon96x96");
             if (!icon) icon = parse_buffer(buffer, "icon48x48");
             if (!icon) icon = parse_buffer(buffer, "icon32x32");
             if (!icon) icon = parse_buffer(buffer, "icon");
             if (icon)
                eap->icon = strdup(icon);
             else
                eap->icon = NULL;
          }
     }
   buffer[0] = '\0';

   fclose(f);

   process_file(file, NULL, eap);
   free(eap);
}

void
parse_ini_init()
{
   if (!ini_file_cache)
     {
        ini_file_cache = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        if (ini_file_cache)
          {
             ecore_hash_set_free_key(ini_file_cache, free);
             ecore_hash_set_free_value(ini_file_cache, (Ecore_Free_Cb) ecore_hash_destroy);
          }
     }
   if (!desktop_cache)
     {
        desktop_cache = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        if (desktop_cache)
          {
             ecore_hash_set_free_key(desktop_cache, free);
             ecore_hash_set_free_value(desktop_cache, (Ecore_Free_Cb) _parse_desktop_del);
          }
     }
}

Ecore_Hash *
parse_ini_file(char *file)
{
   Ecore_Hash *result;

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
             while (((*c == ' ') || (*c == '\t')) && (*c != '\n') && (*c != '\0'))
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
                  while ((*c != '=') && (*c != ' ') && (*c != '\t') && (*c != '\n') && (*c != '\0'))
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
                       while (((*c == ' ') || (*c == '\t')) && (*c != '\n') && (*c != '\0'))
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
        ecore_hash_set(ini_file_cache, strdup(file), result);
     }
   return result;
}

Desktop *
parse_desktop_ini_file(char *file)
{
   Desktop *result;

   result = (Desktop *) ecore_hash_get(desktop_cache, file);
   if (!result)
     {
        result = calloc(1, sizeof(Desktop));
        if (result)
          {
             result->data = parse_ini_file(file);
             if (result->data)
               {
                  result->group = (Ecore_Hash *) ecore_hash_get(result->data, "Desktop Entry");
                  if (!result->group)
                     result->group = (Ecore_Hash *) ecore_hash_get(result->data, "KDE Desktop Entry");
                  if (result->group)
                    {
                       char *temp;

                       temp = (char *)ecore_hash_get(result->group, "Categories");
                       if (temp)
                          result->Categories = dumb_tree_from_paths(temp);
                       temp = (char *)ecore_hash_get(result->group, "OnlyShowIn");
                       if (temp)
                          result->OnlyShowIn = dumb_tree_from_paths(temp);
                       temp = (char *)ecore_hash_get(result->group, "NotShowIn");
                       if (temp)
                          result->NotShowIn = dumb_tree_from_paths(temp);
                    }
                  ecore_hash_set(desktop_cache, strdup(file), result);
               }
             else
               {
                  free(result);
                  result = NULL;
               }
          }
     }
   return result;
}

static void
_parse_desktop_del(Desktop * desktop)
{
   if (desktop->NotShowIn)
      dumb_tree_del(desktop->NotShowIn);
   if (desktop->OnlyShowIn)
      dumb_tree_del(desktop->OnlyShowIn);
   if (desktop->Categories)
      dumb_tree_del(desktop->Categories);
   free(desktop);
}

void
parse_ini_shutdown()
{
   if (ini_file_cache)
     {
        ecore_hash_destroy(ini_file_cache);
        ini_file_cache = NULL;
     }
   if (desktop_cache)
     {
        ecore_hash_destroy(desktop_cache);
        desktop_cache = NULL;
     }
}
