#include "emote.h"

EM_INTERN Evas_Object *
em_util_icon_add(Evas_Object *obj, const char *icon) 
{
   Evas_Object *ow;
   char *path;

   ow = elm_icon_add(obj);
   evas_object_size_hint_aspect_set(ow, EVAS_ASPECT_CONTROL_BOTH, 1, 1);

#ifdef ELM_EFREET
   /* tell elm that we need efreet */
   elm_need_efreet();
   if (!(path = efreet_icon_path_find(getenv("E_ICON_THEME"), icon, 48))) 
     {
        if (!(path = efreet_icon_path_find("default", icon, 48))) 
          if (!(path = efreet_icon_path_find("hicolor", icon, 48))) 
            if (!(path = efreet_icon_path_find("gnome", icon, 48))) 
              path = efreet_icon_path_find("Human", icon, 48);
     }
   if (path) 
     {
        elm_icon_file_set(ow, path, NULL);
        free(path);
     }
#endif

   return ow;
}

EM_INTERN const char *
em_util_user_dir_get(void) 
{
   static char dir[PATH_MAX];

   if (!dir[0]) 
     {
        const char *home;

        if (!(home = getenv("HOME"))) home = "/tmp";
        snprintf(dir, sizeof(dir), "%s/.emote", home);
     }

   return dir;
}
