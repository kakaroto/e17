/*
 * This conforms with the freedesktop.org Desktop Menu Specification version 0.92
 *
 * This is gonna be repetative and slow.  The idea is to first get it to correctly
 * follow the spec in a way that is easy to check.  Only then can we start to 
 * optomize into "ugly but fast".
 *
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

struct _fdo_menus_unxml_data
{
   Dumb_List *menus;
   char *file;
   char *base;
   char *path;
};

static int _fdo_menus_unxml(const void *data, Dumb_List *list, int element, int level);
static void _fdo_menus_unxml_rules(Dumb_List *rules, Dumb_List *list, char type, char sub_type);
static void _fdo_menus_unxml_moves(Dumb_List *menu, Dumb_List *list);
static void _fdo_menus_add_dirs(Dumb_List *list, Dumb_List *paths, char *pre, char *post, char *extra, int element);
static int _fdo_menus_expand_apps(const void *data, Dumb_List *list, int element, int level);
static int _fdo_menus_check_app(const void *data, char *path);


Dumb_List *
fdo_menus_get(char *file, Dumb_List *xml)
{
   struct _fdo_menus_unxml_data data;

   data.file = file;
   data.menus = dumb_list_new(NULL);
   if (data.menus)
      {
         data.base = ecore_file_strip_ext(ecore_file_get_file(file));
         data.path = ecore_file_get_dir(file);
	 if ((data.base) && (data.path))
	    {
	       /* FIXME: There is some scope for merging some of these. */
               dumb_list_foreach(xml, 0, _fdo_menus_unxml, &data);
               dumb_list_foreach(xml, 0, _fdo_menus_expand_apps, &data);

//dumb_list_dump(xml, 0);
	    }
         E_FREE(data.path);
         E_FREE(data.base);
      }
   return data.menus;
}


static int
_fdo_menus_unxml(const void *data, Dumb_List *list, int element, int level)
{
   struct _fdo_menus_unxml_data *unxml_data;
   Dumb_List *menus;

   unxml_data = (struct _fdo_menus_unxml_data *) data;
   menus = (Dumb_List *) unxml_data->menus;
   if (list->elements[element].type == DUMB_LIST_ELEMENT_TYPE_STRING)
      {
         if (strcmp((char *) list->elements[element].element, "<Menu") == 0)
	    {
               Dumb_List *menu, *rules;
               Ecore_Hash *pool;

               menu = dumb_list_new(NULL);
               rules = dumb_list_new(NULL);
               pool = ecore_hash_new(ecore_str_hash, ecore_str_compare);
               if ((menu) && (rules) && (pool))
                  {
		     int i;
		     char *flags = "   ", *name = "", *directory = "";
		     char temp[MAX_PATH];

                     ecore_hash_set_free_key(pool, free);
                     ecore_hash_set_free_value(pool, free);
		     sprintf(temp, "<MENU <%.3s> <%s> <%s>", flags, name, directory);
	             dumb_list_extend(menu, temp);
		     flags = (char *) menu->elements[0].element;
		     flags += 7;
	             dumb_list_add_hash(menu, pool);
	             dumb_list_add_child(menu, rules);
                     list->elements[element].element = menu;
                     list->elements[element].type = DUMB_LIST_ELEMENT_TYPE_LIST;
		     for (i = element + 1; i < list->size; i++)
		        {
                           int result = 0;

                           if (list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_STRING)
                              {
                                 if (strcmp((char *) list->elements[i].element, "<Deleted/") == 0)
				    {
				       flags[1] = 'D';
				       result = 1;
				    }
                                 else if (strcmp((char *) list->elements[i].element, "<NotDeleted/") == 0)
				    {
				       flags[1] = ' ';
				       result = 1;
				    }
                                 else if (strcmp((char *) list->elements[i].element, "<OnlyUnallocated/") == 0)
				    {
				       flags[2] = 'O';
				       result = 1;
				    }
                                 else if (strcmp((char *) list->elements[i].element, "<NotOnlyUnallocated/") == 0)
				    {
				       flags[2] = ' ';
				       result = 1;
				    }
                                 else if (strcmp((char *) list->elements[i].element, "<DefaultAppDirs/") == 0)
	                            {
                                       _fdo_menus_add_dirs(menu, fdo_paths_desktops, "<AppDir", "</AppDir", NULL, i);
				       result = 1;
	                            }
                                 else if (strcmp((char *) list->elements[i].element, "<DefaultDirectoryDirs/") == 0)
	                            {
                                       _fdo_menus_add_dirs(menu, fdo_paths_directories, "<DirectoryDir", "</DirectoryDir", NULL, i);
				       result = 1;
	                            }
                                 else if (strcmp((char *) list->elements[i].element, "<DefaultMergeDirs/") == 0)
	                            {
	                               if (unxml_data->base)
	                                  {
                                             _fdo_menus_add_dirs(menu, fdo_paths_menus, "<MergeDir", "</MergeDir", unxml_data->base, i);
				             result = 1;
		                          }
	                            }
                                 else if (strcmp((char *) list->elements[i].element, "</Menu") == 0)
				    {
				       result = 1;
				    }
				 else
				    {
	                               dumb_list_extend(menu, (char *) list->elements[i].element);
				       result = 1;
				    }
			      }
                           else if (list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_LIST)
                              {
                                 Dumb_List *sub;

                                 sub = (Dumb_List *) list->elements[i].element;
				 if ((sub) && (sub->size))
				    {
                                       if (sub->elements[0].type == DUMB_LIST_ELEMENT_TYPE_STRING)
                                          {
                                             if (strcmp((char *) sub->elements[0].element, "<Name") == 0)
	                                        {
						   name = strdup((char *) sub->elements[1].element);
		                                   sprintf(temp, "<MENU <%.3s> <%s> <%s>", flags, name, directory);
	                                           menu->elements[0].element = strdup(temp);
                                                   dumb_list_track(menu, name);
                                                   dumb_list_track(menu, menu->elements[0].element);
						   flags = (char *) menu->elements[0].element;
						   flags += 7;
	                                           result = 1;
	                                        }
                                             else if (strcmp((char *) sub->elements[0].element, "<Directory") == 0)
	                                        {
						   directory = strdup((char *) sub->elements[1].element);
		                                   sprintf(temp, "<MENU <%.3s> <%s> <%s>", flags, name, directory);
	                                           menu->elements[0].element = strdup(temp);
                                                   dumb_list_track(menu, directory);
                                                   dumb_list_track(menu, menu->elements[0].element);
						   flags = (char *) menu->elements[0].element;
						   flags += 7;
	                                           result = 1;
	                                        }
                                             else if ((strcmp((char *) sub->elements[0].element, "<Include") == 0) || 
					              (strcmp((char *) sub->elements[0].element, "<Exclude") == 0))
	                                        {
						   _fdo_menus_unxml_rules(rules, sub, ((char *) sub->elements[0].element)[1], 'O');
	                                           result = 1;
	                                        }
                                             else if (strcmp((char *) sub->elements[0].element, "<Menu") == 0)
					        {
                                                   _fdo_menus_unxml(data, sub, 0, level);
	                                           dumb_list_add_child(menu, (Dumb_List *) sub->elements[0].element);
						   /* FIXME: Dunno if this causes a memory leak, but for now we play it safe. */
                                                   list->elements[i].type = DUMB_LIST_ELEMENT_TYPE_NULL;
                                                   list->elements[i].element = NULL;
//	                                           result = 1;
						}
                                             else if (strcmp((char *) sub->elements[0].element, "<Move") == 0)
	                                        {
                                                   _fdo_menus_unxml_moves(menu, sub);
	                                           result = 1;
	                                        }
                                             else
				                {
						   if ((sub->size == 3) && (sub->elements[1].type == DUMB_LIST_ELEMENT_TYPE_STRING))
						      {
						         char temp[MAX_PATH];

                                                         sprintf(temp, "%s %s", (char *) sub->elements[0].element, (char *) sub->elements[1].element);
	                                                 dumb_list_extend(menu, temp);
							 result = 1;
						      }
						   else
						      {
	                                                 dumb_list_add_child(menu, sub);
                                                         list->elements[i].type = DUMB_LIST_ELEMENT_TYPE_NULL;
                                                         list->elements[i].element = NULL;
						      }
				                }
                                          }
				    }
			      }
			   if (result)
			      {
                                 if (list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_LIST)
                                    dumb_list_del((Dumb_List *) list->elements[i].element);
                                 list->elements[i].type = DUMB_LIST_ELEMENT_TYPE_NULL;
                                 list->elements[i].element = NULL;
			      }
			}
		  }
	       else
	          {
		     if (pool)     ecore_hash_destroy(pool);
		     if (rules)    dumb_list_del(rules);
		     if (menu)     dumb_list_del(menu);
		  }
	    }
      }
   return 0;
}


static void
_fdo_menus_unxml_rules(Dumb_List *rules, Dumb_List *list, char type, char sub_type)
{
   int i;
   char temp[MAX_PATH];

   for (i = 0; i < list->size; i++)
      {
         if (list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_STRING)
            {
               if (strcmp((char *) list->elements[i].element, "<All/") == 0)
	          {
		     sprintf(temp, "%c%c All", type, sub_type);
		     dumb_list_extend(rules, temp);
		  }
               else if (strcmp((char *) list->elements[i].element, "<File") == 0)
	          {
		     sprintf(temp, "%c%c File %s", type, sub_type, (char *) list->elements[i + 1].element);
		     dumb_list_extend(rules, temp);
		  }
               else if (strcmp((char *) list->elements[i].element, "<Category") == 0)
	          {
		     sprintf(temp, "%c%c Category %s", type, sub_type, (char *) list->elements[i + 1].element);
		     dumb_list_extend(rules, temp);
		  }
               else if (strcmp((char *) list->elements[i].element, "<Or") == 0)
	          {
                     _fdo_menus_unxml_rules(rules, (Dumb_List *) list->elements[i + 1].element, type, sub_type);
		  }
               else if ((strcmp((char *) list->elements[i].element, "<And") == 0) ||
                        (strcmp((char *) list->elements[i].element, "<Not") == 0))
	          {
		     char this_type;
                     Dumb_List *sub;

                     this_type = ((char *) list->elements[i].element)[1];
                     sub = dumb_list_new(NULL);
		     if (sub)
		        {
		           dumb_list_add_child(rules, sub);
                           for (i++; i < list->size; i++)
                              {
                                 if (list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_LIST)
                                    _fdo_menus_unxml_rules(sub, (Dumb_List *) list->elements[i].element, type, this_type);
			      }
			}
		  }
	    }
         else if (list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_LIST)
            {
               _fdo_menus_unxml_rules(rules, (Dumb_List *) list->elements[i].element, type, sub_type);
	    }
      }
}

static void
_fdo_menus_unxml_moves(Dumb_List *menu, Dumb_List *list)
{
   int i;
   char *old = NULL;
   char *new = NULL;

   for (i = 0; i < list->size; i++)
      {
         if (list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_LIST)
            {
               Dumb_List *sub;

               sub = (Dumb_List *) list->elements[i].element;
	       if ((sub) && (sub->size))
	          {
                     if (sub->elements[0].type == DUMB_LIST_ELEMENT_TYPE_STRING)
		        {
                           if (strcmp((char *) sub->elements[0].element, "<Old") == 0)
			      old = strdup((char *) sub->elements[1].element);
                           if (strcmp((char *) sub->elements[0].element, "<New") == 0)
			      new = strdup((char *) sub->elements[1].element);
		        }
	          }
	    }
         if ((old) && (new))
            {
               char temp[MAX_PATH * 2];

               sprintf(temp, "<MOVE <%s> <%s>", old, new);
	       dumb_list_extend(menu, temp);
	       free(old);
               old = NULL;
	       free(new);
               new = NULL;
	   }
      }
}

static void
_fdo_menus_add_dirs(Dumb_List *list, Dumb_List *paths, char *pre, char *post, char *extra, int element)
{
   int i;
   char t[MAX_PATH];

   /* reverse the order of the dirs. */
   for (i = paths->size - 1; i >= 0; i--)
      {
         if (extra)
	    sprintf(t, "%s %s%s-merged/", pre, (char *) paths->elements[i].element, extra);
         else
	    sprintf(t, "%s %s", pre, (char *) paths->elements[i].element);
         dumb_list_extend(list, t);
      }
}

static int
_fdo_menus_expand_apps(const void *data, Dumb_List *list, int element, int level)
{
   struct _fdo_menus_unxml_data *unxml_data;
   char *path;

   unxml_data = (struct _fdo_menus_unxml_data *) data;
   path = (char *) unxml_data->path;
   if (list->elements[element].type == DUMB_LIST_ELEMENT_TYPE_STRING)
      {
         if (strncmp((char *) list->elements[element].element, "<AppDir ", 8) == 0)
	    {
               Ecore_Hash *pool;

               pool = ecore_hash_new(ecore_str_hash, ecore_str_compare);
               if (pool)
                  {
		     struct _fdo_menus_expand_apps_data our_data;

		     our_data.pool = pool;
                     ecore_hash_set_free_key(pool, free);
                     ecore_hash_set_free_value(pool, free);
//                     element++;
//                     if ((list->size > element) && (list->elements[element].type == DUMB_LIST_ELEMENT_TYPE_STRING))
		        {
		           char dir[MAX_PATH];
		           char *app_dir;

                           app_dir = (char *) list->elements[element].element;
			   app_dir += 8;
                           sprintf(dir, "%s", app_dir);
			   if (dir[0] != '/')
                              sprintf(dir, "%s/%s", path, app_dir);
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
   return 0;
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
  expand <KDELegacyDirs> to <LegacyDir>.
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
    for each <Include> and <Exclude>
      for each .desktop in pool
        for each rule
          if rule matches .desktop in pool
	     if rule is an <Include>
	        add .desktop to menu.
	        mark it as allocated
	     if rule is an <Exclude>
	       remove .desktop from menu.
	       leave it as allocated.


<Menu (list)
  name
  flags = "   " or "MDO" the first letter of - Marked, Deleted, OnlyUnallocated 
  pool (hash)
    id = path
    id = path
  rules (list)
    rule
    rule
  <Menu (list)
  <Menu (list)

rules (list)
  include/exclude or all/file/category x
  and/not (list)
    include/exclude and/not all/file/category x


generate unallocated menus
  Same as for menus, but only the <OnlyUnallocated> ones.
  Only the unallocated .desktop entries can be used.

generate menu layout
*/

