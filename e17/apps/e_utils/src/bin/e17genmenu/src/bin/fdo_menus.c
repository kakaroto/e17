/*
 * This conforms with the freedesktop.org Desktop Menu Specification version 0.92
 */
#include <dirent.h>
#include <string.h>             //string funcs
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>

#include <Ecore.h>

#include "global.h"
#include "fdo_paths.h"
#include "parse.h"

struct _fdo_menus_expand_apps_data
{
   char *path;
   Ecore_Hash *pool;
   int length;
};


static void _fdo_menus_expand_defaults(const void *data, Dumb_List *list, int element, int level);
static void _fdo_menus_add_dirs(Dumb_List *list, Dumb_List *paths, char *pre, char *post, char *extra, int element);
static void _fdo_menus_expand_apps(const void *data, Dumb_List *list, int element, int level);
static int _fdo_menus_check_app(const void *data, char *path);

static void _fdo_menus_make_apps(const void *data, Dumb_List *list, int element, int level);
static void _fdo_menus_dump_each_hash_node(void *value, void *user_data);


Dumb_List *
fdo_menus_get(char *file, Dumb_List *xml)
{
   Dumb_List *menus;

   menus = dumb_list_new(NULL);
   if (menus)
      {
         char *base;
	 char *path;

         base = ecore_file_strip_ext(ecore_file_get_file(file));
         path = ecore_file_get_dir(file);
	 if ((base) && (path))
	    {
	       /* FIXME: There is some scope for merging some of these. */
               dumb_list_foreach(xml, 0, _fdo_menus_expand_defaults, base);
               dumb_list_foreach(xml, 0, _fdo_menus_expand_apps, path);

//dumb_list_dump(xml, 0);
	    }
         E_FREE(path);
         E_FREE(base);
      }
   return menus;
}

static void
_fdo_menus_expand_defaults(const void *data, Dumb_List *list, int element, int level)
{
   char *file;

   file = (char *) data;
   if (list->elements[element].type == DUMB_LIST_ELEMENT_TYPE_STRING)
      {
         if (strcmp((char *) list->elements[element].element, "<DefaultAppDirs/") == 0)
	    {
	       /* Replace this element with a list copied from fdo_paths_desktops as <AppDir>s */
               _fdo_menus_add_dirs(list, fdo_paths_desktops, "<AppDir", "</AppDir", NULL, element);
	    }
         else if (strcmp((char *) list->elements[element].element, "<DefaultDirectoryDirs/") == 0)
	    {
	       /* Replace this element with a list copied from fdo_paths_directories as <DirectoryDir>s */
               _fdo_menus_add_dirs(list, fdo_paths_directories, "<DirectoryDir", "</DirectoryDir", NULL, element);
	    }
         else if (strcmp((char *) list->elements[element].element, "<DefaultMergeDirs/") == 0)
	    {
	       if (file)
	          {
	             /* Replace this element with a list copied from fdo_paths_menus as <MergeDir>s */
	             /* These dirs have "filename-merged/" added to the end of them, 
	              * where filename is the basename (without extension) of the 
		      * file the menu came from. 
		      */
                     _fdo_menus_add_dirs(list, fdo_paths_menus, "<MergeDir", "</MergeDir", file, element);
		  }
	    }
      }
}

static void
_fdo_menus_add_dirs(Dumb_List *list, Dumb_List *paths, char *pre, char *post, char *extra, int element)
{
   int i;
   Dumb_List *new_list;

   new_list = dumb_list_new(NULL);
   if (new_list)
      {
         /* reverse the order of the dirs. */
         for (i = paths->size - 1; i >= 0; i--)
            {
               Dumb_List *_list;

               _list = dumb_list_new(NULL);
	       if (_list)
	          {
	             dumb_list_add_child(new_list, _list);
	             /* There is a subtlety going on here.
	              * The strings in paths are only from the fdo_paths created at 
	      	      * the start of execution, and freed at the end of execution.
	      	      * Thus we can reley on them being around all the time, and
	      	      * can safely use dumb_list_add().
		      *
		      * The pre and post strings are hardcoded into the binary, thus
		      * we can rely on them sticking around.
		      *
		      * The other string we create on the stack, thus we need to use
		      * dumb_list_extend(), which strdups() the element and frees it 
		      * when the Dumb_List is freed.
		      */
                     dumb_list_add(_list, pre);
	             if (extra)
	                {
	                   char t[MAX_PATH];

	                   sprintf(t, "%s%s-merged/", (char *) paths->elements[i].element, extra);
                           dumb_list_extend(_list, t);
		        }
	             else
                        dumb_list_add(_list, paths->elements[i].element);
                     dumb_list_add(_list, post);
		  }
            }
	 /* The old element was allocated by xmlame.c, and is part of a string
	  * that is already being tracked by the Dumb_List created there, so we
	  * Don't have to free it.
	  */
         list->elements[element].element = new_list;
         list->elements[element].type = DUMB_LIST_ELEMENT_TYPE_LIST;
      }
}

static void
_fdo_menus_expand_apps(const void *data, Dumb_List *list, int element, int level)
{
   char *path;

   path = (char *) data;
   if (list->elements[element].type == DUMB_LIST_ELEMENT_TYPE_STRING)
      {
         if (strcmp((char *) list->elements[element].element, "<AppDir") == 0)
	    {
               Ecore_Hash *pool;

               pool = ecore_hash_new(ecore_str_hash, ecore_str_compare);
               if (pool)
                  {
		     struct _fdo_menus_expand_apps_data our_data;

		     our_data.pool = pool;
                     ecore_hash_set_free_key(pool, free);
                     ecore_hash_set_free_value(pool, free);
                     element++;
                     if ((list->size > element) && (list->elements[element].type == DUMB_LIST_ELEMENT_TYPE_STRING))
		        {
		           char dir[MAX_PATH];

                           sprintf(dir, "%s", (char *) list->elements[element].element);
			   if (dir[0] != '/')
                              sprintf(dir, "%s/%s", path, (char *) list->elements[element].element);
                           our_data.path = dir;
			   our_data.length = strlen(dir);
                           fdo_paths_recursive_search(dir, NULL, _fdo_menus_check_app, &our_data);
	                   /* The dir is the next element
		            * readdir dir/ *.desktop
		            *   name = subdir-subdir--file.desktop
		            *   hash add name, path/subdir/file.desktop
		            * replace element with this hash.
		            */
                           list->elements[element].element = pool;
                           list->elements[element].type = DUMB_LIST_ELEMENT_TYPE_HASH;
			}
	          }
	    }
      }
}

static int
_fdo_menus_check_app(const void *data, char *path)
{
   char *p;
   struct _fdo_menus_expand_apps_data *our_data;

   our_data = (struct _fdo_menus_expand_apps_data *) data;
   p = strrchr(path, '.');
   if (p)
      {
         if (strcmp(p, ".desktop") == 0)
	    {
	       int i;
	       char *file;

               file = strdup(path + our_data->length);
	       path = strdup(path);
	       if ((file) && (path))
	          {
	             for (i = 0; file[i] != '\0'; i++ )
	               if (file[i] == '/')
		         file[i] = '-';
                     if (!ecore_hash_set(our_data->pool, file, strdup(path)))
		        {
                           free(path);
                           free(file);
			}
		  }
	    }
      }

   return 1;
}

/*
merge menus
  for each <MergeFile>, <MergeDir>, and <LegacyDir> element
    get the root <Menu> elements from that elements file/s.
    remove the <Name> element from those root <Menu> elements.
    replace that element with the child elements of those root <Menu> elements.
    expand the <DefaultMergeDirs> with the name/s of that elements file/s
  loop until all <MergeFile>, <MergeDir>, and <LegacyDir> elements are done,
  careful to avoid infinite loops in files that reference each other.
*  for each <Menu> recursively
    consolidate duplicate child <Menu>s.
*    expand <DefaultAppDir>s and <DefaultDirectoryDir>s to <AppDir>s and <DirectoryDir>s.
    consolidate duplicate child <AppDir>s, <DirectoryDir>s, and <Directory>s.
    resolve duplicate <Move>s.
  for each <Menu> recursively
    for each <Move>
      do the move.
      mark any <Menu> that has been affected.
  for each marked <Menu> recursively
    consolidate duplicate child <Menu>s.
  for each <Menu> recursively
    if there are <Deleted> elements that are not ovreridden by a <NotDelete> element
      remove this <Menu> element and all it's children.

generate menus
*  for each <Menu> that is <NotOnlyUnallocated> (which is the default)
*    for each <AppDir>
*      for each .desktop
*        if it exists in the pool, replace it.
*	 else add it to the pool.
*    for each <Include>
*      for each .desktop in pool
*        for each rule
*          if rule matches .desktop in pool
*	     add .desktop to menu.
*	     mark it as allocated
    for each <Exclude>
      for each .desktop in pool
        for each rule
          if rule matches .desktop in menu
	    remove it from menu.
	    leave it as allocated.

generate unallocated menus
  Same as for menus, but only the <OnlyUnallocated> ones.
  Only the unallocated .desktop entries can be used.

generate menu layout
*/

