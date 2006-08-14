#include "global.h"
#include "config.h"
#include "eaps.h"
#include "order.h"
#include "parse.h"

extern int reject_count, not_over_count;

static char *_parse_buffer(char *b, char *section);
static void _parse_process_file(char *file, char *menu_path, G_Eap *eap);
char *_parse_find_category(char *category);


#if 0
static char *
_parse_get_t(char *icon)
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


static char *
_parse_buffer(char *b, char *section)
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
        d = _parse_get_t(t);
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
   Ecore_Desktop *desktop;

   home = ecore_desktop_home_get();

#ifdef DEBUG
   fprintf(stderr, "Parsing Desktop File %s\n", app);
#endif

   eap = calloc(1, sizeof(G_Eap));
   eap->eap_name = get_eap_name(app);

   desktop = ecore_desktop_get(app);
   if (desktop)
     {
        if (desktop->name)
           eap->name = strdup(desktop->name);
        if (desktop->generic)
           eap->generic = strdup(desktop->generic);
        if (desktop->comment)
           eap->comment = strdup(desktop->comment);
        if (desktop->type)
           eap->type = strdup(desktop->type);
        if (desktop->categories)
           eap->categories = strdup(desktop->categories);
        if (desktop->exec)
           eap->exec = strdup(desktop->exec);
        if (desktop->icon)
           eap->icon = strdup(desktop->icon);
        if (desktop->startup)
           eap->startup = strdup(desktop->startup);
        if (desktop->window_class)
           eap->window_class = strdup(desktop->window_class);
     }

   /* Check If We Process */
   if (!eap->type)
      _parse_process_file(app, menu_path, eap);
   if (eap->type)
     {
        if (!strcmp(eap->type, "Application"))
           _parse_process_file(app, menu_path, eap);
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


static void
_parse_process_file(char *file, char *menu_path, G_Eap *eap)
{
   char *home, *category, *icon_size, *icon_theme;
   char path[MAX_PATH], order_path[MAX_PATH];
   int overwrite;

   if (!eap)
      return;

#ifdef DEBUG
   fprintf(stderr, "Processing File %s\n", file);
#endif

   /* Get Icon Options */
   icon_size = get_icon_size();
   icon_theme = get_icon_theme();

   home = ecore_desktop_home_get();
   overwrite = get_overwrite();

   snprintf(path, sizeof(path), "%s" EAPPDIR "/%s", home, eap->eap_name);

   if ((ecore_file_exists(path)) && (!overwrite))
        not_over_count++;
   else
      {
         eap->icon_path = ecore_desktop_icon_find(eap->icon, icon_size, icon_theme);
         write_icon(path, eap);
      }

   category = NULL;
   if (menu_path != NULL)
     {
        snprintf(order_path, sizeof(order_path), "%s" EFAVDIR "/Converted Menus/%s", home, menu_path);
        modify_order(order_path, eap->eap_name);
     }
   else if (eap->categories != NULL)
     {
        category = _parse_find_category(eap->categories);
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
             name = _parse_buffer(buffer, "title=");
             eap->name = strdup(name);
          }
        if (strstr(buffer, "longtitle"))
          {
             generic = _parse_buffer(buffer, "longtitle=");
             eap->generic = strdup(generic);
          }
        if (strstr(buffer, "description"))
          {
             comment = _parse_buffer(buffer, "description=");
             eap->comment = strdup(comment);
          }
        if (strstr(buffer, "section"))
          {
             category = _parse_buffer(buffer, "section=");
             eap->categories = strdup(category);
          }
        if (strstr(buffer, "command"))
          {
             exec = _parse_buffer(buffer, "command=");
             eap->exec = strdup(exec);
          }
        if (strstr(buffer, "icon"))
          {
             icon = _parse_buffer(buffer, "icon128x128");
             if (!icon) icon = _parse_buffer(buffer, "icon96x96");
             if (!icon) icon = _parse_buffer(buffer, "icon48x48");
             if (!icon) icon = _parse_buffer(buffer, "icon32x32");
             if (!icon) icon = _parse_buffer(buffer, "icon");
             if (icon)
                eap->icon = strdup(icon);
             else
                eap->icon = NULL;
          }
     }
   buffer[0] = '\0';

   fclose(f);

   _parse_process_file(file, NULL, eap);
   free(eap);
}


#define CATEGORIES "Accessibility:Accessories:Amusement:AudioVideo:Core:Development:Education:Game:Graphics:Multimedia:Network:Office:Programming:Settings:System:TextEditor:Utility:Video"

char *
_parse_find_category(char *category)
{
   char *token, *cat;

   cat = NULL;
   token = strtok(strdup(CATEGORIES), ":");
   while (token)
     {
        /* Check If this token is in supplied $t */
        if (strstr(category, token) != NULL)
          {
             if (strstr(token, "Development") != NULL)
               {
                  cat = "Programming";
               }
             else if (strstr(token, "Game") != NULL)
               {
                  cat = "Games";
               }
             else if ((strstr(token, "AudioVideo") != NULL) ||
                      (strstr(token, "Sound") != NULL) || (strstr(token, "Video") != NULL) || (strstr(token, "Multimedia") != NULL))
               {
                  cat = "Multimedia";
               }
             else if (strstr(token, "Net") != NULL)
               {
                  cat = "Internet";
               }
             else if (strstr(token, "Education") != NULL)
               {
                  cat = "Edutainment";
               }
             else if (strstr(token, "Amusement") != NULL)
               {
                  cat = "Toys";
               }
             else if (strstr(token, "System") != NULL)
               {
                  cat = "System";
               }
             else if ((strstr(token, "Shells") != NULL) || (strstr(token, "Utility") != NULL) || (strstr(token, "Tools") != NULL))
               {
                  cat = "Utilities";
               }
             else if ((strstr(token, "Viewers") != NULL) || (strstr(token, "Editors") != NULL) || (strstr(token, "Text") != NULL))
               {
                  cat = "Editors";
               }
             else if (strstr(token, "Graphics") != NULL)
               {
                  cat = "Graphics";
               }
             else if ((strstr(token, "WindowManagers") != NULL) || (strstr(token, "Core") != NULL))
               {
                  cat = "Core";
               }
             else if ((strstr(token, "Settings") != NULL) || (strstr(token, "Accessibility") != NULL))
               {
                  cat = "Settings";
               }
             else if (strstr(token, "Office") != NULL)
               {
                  cat = "Office";
               }
             else
               {
                  cat = "Core";
               }
          }
        token = strtok(NULL, ":");
     }
   if (token)
      free(token);
   if (!cat)
      cat = "Core";
   return strdup(cat);
}
