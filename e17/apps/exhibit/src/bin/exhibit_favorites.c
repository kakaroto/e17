#include "exhibit.h"

void
_ex_favorites_add(Exhibit *e, char *path)
{
   char  new_path[PATH_MAX];
   char *realpath;      
   
   if(!ecore_file_is_dir(e->fav_path))
     {
	if(ecore_file_exists(e->fav_path))
	  return;
	if(!ecore_file_mkdir(e->fav_path))
	  return;
     }
   
   realpath = ecore_file_realpath(path);
   snprintf(new_path, sizeof(new_path), "%s/%s", e->fav_path, path);

   ecore_file_symlink(realpath, new_path);
   if(realpath)
     free(realpath);   
}

void
_ex_favorites_del(Exhibit *e, char *path)
{
   char new_path[PATH_MAX];
   
   snprintf(new_path, sizeof(new_path), "%s/%s", e->fav_path, path);
   ecore_file_unlink(new_path);
}
