#include <Elementary.h>
#include <Efreet.h>

Evas_Object *
elfe_utils_fdo_icon_add(Evas_Object *parent, const char *icon, int size)
{
   Evas_Object *ic;
   const char *path;

   ic = elm_icon_add(parent);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_BOTH, 1, 1);

   if (path && path[0] == '/')
       {
           elm_icon_file_set(ic, path, NULL);
           return ic;
       }

   path = efreet_icon_path_find(getenv("E_ICON_THEME"), icon, size);
   if (!path)
     {
	path = efreet_icon_path_find("default", icon, size);
	if (!path)
	  {
	     path = efreet_icon_path_find("hicolor", icon, size);
	     if (!path)
	       {
		  path = efreet_icon_path_find("gnome", icon, size);
		  if (!path)
		    {
		       path = efreet_icon_path_find("Human", icon, size);
		    }
	       }
	  }
     }

   if (path)
	elm_icon_file_set(ic, path, NULL);

   return ic;
}

const char *
elfe_utils_fdo_icon_path_get(Efreet_Menu *menu, int size)
{
    const char *path = NULL;

    if (menu->icon && menu->icon[0] == '/')
      return eina_stringshare_add(menu->icon);

    path = efreet_icon_path_find(getenv("E_ICON_THEME"), menu->icon, size);
    if (!path)
     {
	path = efreet_icon_path_find("default", menu->icon, size);
	if (!path)
	  {
	     path = efreet_icon_path_find("hicolor", menu->icon, size);
	     if (!path)
	       {
		  path = efreet_icon_path_find("gnome", menu->icon, size);
		  if (!path)
		    {
		       path = efreet_icon_path_find("Human", menu->icon, size);
		    }
	       }
	  }
     }

    if (path)
      return eina_stringshare_add(path);
    else
      return NULL;

}
