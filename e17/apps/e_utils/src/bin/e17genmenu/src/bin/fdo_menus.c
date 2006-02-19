/*
 * This conforms with the freedesktop.org Desktop Menu Specification version 0.92
 *
 * This is gonna be repetative and slow.  The idea is to first get it to correctly
 * follow the spec in a way that is easy to check.  Only then can we start to 
 * optomize into "ugly but fast".
 *
 */
#include <dirent.h>
#include <string.h>
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
   Dumb_List *stack;
   Dumb_List *merge_stack;
   int unallocated;
};

struct _fdo_menus_generate_data
{
   char *name, *path;
   Dumb_List *rules;
   Ecore_Hash *pool, *apps;
   int unallocated;

   Dumb_List *rule;
   int include;
};

static int _fdo_menus_unxml(const void *data, Dumb_List *list, int element, int level);
static void _fdo_menus_unxml_rules(Dumb_List *rules, Dumb_List *list, char type, char sub_type);
static void _fdo_menus_unxml_moves(Dumb_List *menu, Dumb_List *list);
static void _fdo_menus_add_dirs(Dumb_List *list, Dumb_List *paths, char *pre, char *post, char *extra, int element);
static int _fdo_menus_expand_apps(struct _fdo_menus_unxml_data *unxml_data, char *app_dir, Ecore_Hash *pool);
static int _fdo_menus_check_app(const void *data, char *path);
static int _fdo_menus_generate(const void *data, Dumb_List *list, int element, int level);
static void _fdo_menus_inherit_apps(void *value, void *user_data);
static void _fdo_menus_select_app(void *value, void *user_data);
static int _fdo_menus_apply_rules(struct _fdo_menus_generate_data *generate_data, Dumb_List *rule, char *key, Desktop *desktop);


Dumb_List *
fdo_menus_get(char *file, Dumb_List *xml)
{
   struct _fdo_menus_unxml_data data;

   data.file = file;
   data.menus = dumb_list_new(NULL);
   data.stack = dumb_list_new(NULL);
   if ((data.menus) && (data.stack))
      {
         data.base = ecore_file_strip_ext(ecore_file_get_file(file));
         data.path = ecore_file_get_dir(file);
	 if ((data.base) && (data.path))
	    {
               dumb_list_foreach(xml, 0, _fdo_menus_unxml, &data);
               dumb_list_dump(xml, 0);
               printf("\n\n");
               data.unallocated = FALSE;
               dumb_list_foreach(xml, 0, _fdo_menus_generate, &data);
               printf("\n\n");
               data.unallocated = TRUE;
               dumb_list_foreach(xml, 0, _fdo_menus_generate, &data);
               dumb_list_dump(xml, 0);
               printf("\n\n");
	    }
         E_FREE(data.path);
         E_FREE(data.base);
      }
   else
      {
         E_FN_DEL(dumb_list_del, (data.stack));
         E_FN_DEL(dumb_list_del, (data.menus));
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
         if (strncmp((char *) list->elements[element].element, "<!", 2) == 0)
	    {
               list->elements[element].type = DUMB_LIST_ELEMENT_TYPE_NULL;
               list->elements[element].element = NULL;
	    }
         else if (strcmp((char *) list->elements[element].element, "<Menu") == 0)
	    {
               Dumb_List *menu, *rules;
               Ecore_Hash *pool, *apps;

               menu = dumb_list_new(NULL);
               rules = dumb_list_new(NULL);
               pool = ecore_hash_new(ecore_str_hash, ecore_str_compare);
               apps = ecore_hash_new(ecore_str_hash, ecore_str_compare);
               if ((menu) && (rules) && (pool) && (apps))
                  {
		     int i;
		     char *flags = "   ", *name = "", *directory = "", *menu_path = "";
		     char temp[MAX_PATH];

                     ecore_hash_set_free_key(pool, free);
                     ecore_hash_set_free_value(pool, free);
                     ecore_hash_set_free_key(apps, free);
                     ecore_hash_set_free_value(apps, free);
		     sprintf(temp, "<MENU <%.3s> <%s> <%s>", flags, name, directory);
	             dumb_list_extend(menu, temp);
		     sprintf(temp, "<MENU_PATH %s", menu_path);
	             dumb_list_extend(menu, temp);
		     flags = (char *) menu->elements[0].element;
		     flags += 7;
	             dumb_list_add_hash(menu, pool);
	             dumb_list_add_child(menu, rules);
	             dumb_list_add_hash(menu, apps);
                     list->elements[element].element = menu;
                     list->elements[element].type = DUMB_LIST_ELEMENT_TYPE_LIST;
		     for (i = element + 1; i < list->size; i++)
		        {
                           int result = 0;

                           if (list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_STRING)
                              {
                                 if (strncmp((char *) list->elements[i].element, "<!", 2) == 0)
	                            {
                                       list->elements[i].type = DUMB_LIST_ELEMENT_TYPE_NULL;
                                       list->elements[i].element = NULL;
	                            }
                                 else if (strcmp((char *) list->elements[i].element, "<Deleted/") == 0)
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
                                 else if (strcmp((char *) list->elements[i].element, "<KDELegacyDirs/") == 0)
	                            {
                                       _fdo_menus_add_dirs(menu, fdo_paths_kde_legacy, "<LegacyDir prefix=\"kde-\"", "</LegacyDir", NULL, i);
				       result = 1;
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
						   int i, length = 0;

						   name = strdup((char *) sub->elements[1].element);
		                                   sprintf(temp, "<MENU <%.3s> <%s> <%s>", flags, name, directory);
	                                           menu->elements[0].element = strdup(temp);
                                                   dumb_list_track(menu, name);
                                                   dumb_list_track(menu, menu->elements[0].element);
						   flags = (char *) menu->elements[0].element;
						   flags += 7;
						   /* The rest of this is probably not needed, ekcept to ease debugging. */
						   if (unxml_data->stack->size <= level)
						      {
						         while (unxml_data->stack->size < level)
						            dumb_list_add(unxml_data->stack, "");
						         dumb_list_add(unxml_data->stack, name);
						      }
						   else
	                                                 unxml_data->stack->elements[level].element = name;
					           temp[0] = '\0';
                                                   for (i = 0; i <= level; i++)
                                                      {
						         char *text;

						         /* I can safely assume that they are all strings. */
							 text = (char *) unxml_data->stack->elements[i].element;
							 if (text[0] != '\0')
							    {
							       sprintf(&temp[length], "%s%s", ((length) ? "/" : ""), text);
	                                                       length += strlen(text) + ((length) ? 1 : 0);
							    }
						      }
						   menu_path = strdup(temp);
		                                   sprintf(temp, "<MENU_PATH %s", menu_path);
	                                           menu->elements[1].element = strdup(temp);
                                                   dumb_list_track(menu, menu_path);
                                                   dumb_list_track(menu, menu->elements[1].element);
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
                                                   Dumb_List *new_sub;

                                                   new_sub = dumb_list_new(NULL);
		                                   if (new_sub)
		                                      {
		                                         dumb_list_add_child(rules, new_sub);
						         _fdo_menus_unxml_rules(new_sub, sub, ((char *) sub->elements[0].element)[1], 'O');
	                                                 result = 1;
                                                      }
	                                        }
                                             else if (strcmp((char *) sub->elements[0].element, "<Menu") == 0)
					        {
                                                   _fdo_menus_unxml(data, sub, 0, level + 1);
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

		     for (i = 4; i < menu->size; i++)
		        {
			   int result = 0;

                           if (menu->elements[i].type == DUMB_LIST_ELEMENT_TYPE_STRING)
			      {
				 char *string;

				 string = (char *) menu->elements[i].element;
                                 if (strncmp(string, "<AppDir ", 8) == 0)
				    {
                                       _fdo_menus_expand_apps(unxml_data, &string[8], pool);
				       result = 1;
				    }
                                 else if (strncmp(string, "<DirectoryDir ", 8) == 0)
				    {
				    }
                                 else if (strncmp(string, "<LegacyDir ", 11) == 0)
				    {
				    }
                                 else if (strncmp(string, "<MergeDir ", 8) == 0)
				    {
				    }
                                 else if (strncmp(string, "<MergeFile ", 8) == 0)
				    {
				    }
			      }
			   if (result)
			      {
                                 menu->elements[i].type = DUMB_LIST_ELEMENT_TYPE_NULL;
                                 menu->elements[i].element = NULL;
			      }
			}

                     /* Add it if it has not been deleted. */
                     if (flags[1] != 'D')
		        {
                           list->elements[element].element = menu;
                           list->elements[element].type = DUMB_LIST_ELEMENT_TYPE_LIST;
			}
		  }
	       else
	          {
		     if (apps)     ecore_hash_destroy(apps);
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
		     sprintf(temp, "%c%cA", type, sub_type);
		     dumb_list_extend(rules, temp);
		  }
               else if (strcmp((char *) list->elements[i].element, "<Filename") == 0)
	          {
		     sprintf(temp, "%c%cF %s", type, sub_type, (char *) list->elements[i + 1].element);
		     dumb_list_extend(rules, temp);
		  }
               else if (strcmp((char *) list->elements[i].element, "<Category") == 0)
	          {
		     sprintf(temp, "%c%cC %s", type, sub_type, (char *) list->elements[i + 1].element);
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
_fdo_menus_expand_apps(struct _fdo_menus_unxml_data *unxml_data, char *app_dir, Ecore_Hash *pool)
{
   if (pool)
      {
         struct _fdo_menus_expand_apps_data our_data;
         char dir[MAX_PATH];

	 our_data.pool = pool;
         sprintf(dir, "%s", app_dir);
	 if (dir[0] != '/')
           sprintf(dir, "%s/%s", unxml_data->path, app_dir);
         our_data.path = dir;
	 our_data.length = strlen(dir);
         fdo_paths_recursive_search(dir, NULL, _fdo_menus_check_app, &our_data);
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
	       if ((file) && (path))
	          {
	             for (i = 0; file[i] != '\0'; i++ )
	                if (file[i] == '/')
		           file[i] = '-';
                     ecore_hash_set(our_data->pool, file, strdup(path));
		  }
	    }
      }

   return 1;
}


static int
_fdo_menus_generate(const void *data, Dumb_List *list, int element, int level)
{
   struct _fdo_menus_unxml_data *unxml_data;
   Dumb_List *menus;

   unxml_data = (struct _fdo_menus_unxml_data *) data;
   menus = (Dumb_List *) unxml_data->menus;
   if (list->elements[element].type == DUMB_LIST_ELEMENT_TYPE_STRING)
      {
         if (strncmp((char *) list->elements[element].element, "<MENU ", 6) == 0)
	    {
	       int i;
	       struct _fdo_menus_generate_data generate_data;

               generate_data.unallocated = unxml_data->unallocated;
               generate_data.name = (char *) list->elements[element].element;
               generate_data.path = (char *) list->elements[element + 1].element;
               generate_data.pool  = (Ecore_Hash *) list->elements[element + 2].element;
               generate_data.rules = (Dumb_List *) list->elements[element + 3].element;
               generate_data.apps  = (Ecore_Hash *) list->elements[element + 4].element;

               /* Inherit the pools on the first pass. */
	       if (!generate_data.unallocated)
	          {
	             if (unxml_data->stack->size <= level)
	                {
	                   while (unxml_data->stack->size < level)
	                      dumb_list_add_hash(unxml_data->stack, generate_data.pool);
	                   dumb_list_add_hash(unxml_data->stack, generate_data.pool);
	                }
	             else
	                {
                           unxml_data->stack->elements[level].type = DUMB_LIST_ELEMENT_TYPE_HASH;
                           unxml_data->stack->elements[level].element = generate_data.pool;
	            	}
                     for (i = level - 1; i >= 0; i--)
                        {
                           if (unxml_data->stack->elements[i].type == DUMB_LIST_ELEMENT_TYPE_HASH)
		              {
		                 Ecore_Hash *ancestor;

		                 ancestor = (Ecore_Hash *) unxml_data->stack->elements[i].element;
                                 ecore_hash_for_each_node(ancestor, _fdo_menus_inherit_apps, generate_data.pool);
			      }
	                }
		  }

               /* Process the rules. */
	       if (generate_data.name[9] == (generate_data.unallocated ? 'O' : ' '))
	          {
                     printf("MAKING MENU - %s \t\t%s\n", generate_data.path, generate_data.name);
                     for (i = 0; i < generate_data.rules->size; i++)
                        {
                           if (generate_data.rules->elements[i].type == DUMB_LIST_ELEMENT_TYPE_LIST)
		              {
                                 generate_data.rule = (Dumb_List *) generate_data.rules->elements[i].element;
			         if (generate_data.rule->size > 0)
			            {
			               if ( ((char *) generate_data.rule->elements[0].element)[0] == 'I' )
				          {
				             generate_data.include = TRUE;
                                             ecore_hash_for_each_node(generate_data.pool, _fdo_menus_select_app, &generate_data);
				          }
				       else
				          {
				             generate_data.include = FALSE;
                                             ecore_hash_for_each_node(generate_data.apps, _fdo_menus_select_app, &generate_data);
				          }
			            }
			      }
		           else
			      printf("Fuck, a bug in _fdo_menus.\n");
                        }
		  }
	    }
      }
   return 0;
}

static void
_fdo_menus_inherit_apps(void *value, void *user_data)
{
   Ecore_Hash_Node *node;
   Ecore_Hash *pool;
   char *key, *app;

   pool = (Ecore_Hash *) user_data;
   node = (Ecore_Hash_Node *) value;
   key = (char *) node->key;
   app = (char *) node->value;
   if (!ecore_hash_get(pool, key))
      ecore_hash_set(pool, strdup(key), strdup(app));
}

static void
_fdo_menus_select_app(void *value, void *user_data)
{
   Ecore_Hash_Node *node;
   Desktop *desktop;
   struct _fdo_menus_generate_data *generate_data;
   char *key, *app;

   node = (Ecore_Hash_Node *) value;
   generate_data = (struct _fdo_menus_generate_data *) user_data;
   key = (char *) node->key;
   app = (char *) node->value;

   desktop = parse_desktop_ini_file(app);

   if ((generate_data->unallocated) && (desktop->allocated))
      return;

   if (_fdo_menus_apply_rules(generate_data, generate_data->rule, key, desktop))
      {
         desktop->allocated = TRUE;
         if (generate_data->include)
	    {
	       ecore_hash_set(generate_data->apps, key, strdup(app));
	       printf("INCLUDING %s%s\n", ((generate_data->unallocated) ? "UNALLOCATED " : ""), key);
	    }
	 else
	    {
	       ecore_hash_remove(generate_data->apps, key);
	       printf("EXCLUDING %s%s\n", ((generate_data->unallocated) ? "UNALLOCATED " : ""), key);
	    }
      }
}

static int
_fdo_menus_apply_rules(struct _fdo_menus_generate_data *generate_data, Dumb_List *rule, char *key, Desktop *desktop)
{
   char type = 'O';
   int result = FALSE;
   int i;

   for (i = 0; i < rule->size; i++)
      {
         if (rule->elements[i].type == DUMB_LIST_ELEMENT_TYPE_LIST)
	    {
               result = _fdo_menus_apply_rules(generate_data, (Dumb_List *) rule->elements[i].element, key, desktop);
	    }
	 else
	    {
	       char *rul;
	       char match;
	       int sub_result = FALSE;

               rul = (char *) rule->elements[i].element;
	       type = rul[1];
	       match = rul[2];
	       switch (match)
	          {
		     case 'A' :
		        {
			   sub_result = TRUE;
			   break;
			}

		     case 'F' :
		        {
			   if (strcmp(key, &rul[4]) == 0)
			      sub_result = TRUE;
			   break;
			}

		     case 'C' :
		        {
			   /* Try to match a category. */
			   if (desktop->Categories)
			      {
                                 int j;

                                 for (j = 0; j < desktop->Categories->size; j++)
                                    {
			               if (strcmp((char *) (desktop->Categories->elements[j].element), &rul[4]) == 0)
				          {
			                     sub_result = TRUE;
			                     break;
					  }
				    }
			      }
			   break;
			}
		  }
	       switch (type)
	          {
		     case 'A' :
		        {
			   result = TRUE;
			   if (!sub_result)
			      return FALSE;
			   break;
			}

		     case 'N' :
		        {
			   result = TRUE;
			   if (sub_result)
			      return FALSE;
			   break;
			}

		     default :
		        {
			   if (sub_result)
			      return TRUE;
			   break;
			}
		  }
	    }
      }

   return result;
}


/*
OR (implied)
  loop through the rules
  as soon as one matches, return true
  otherwise return false.

SUB RULES
  process the sub rules, return the result

AND
  loop through the rules
  as soon as one doesn't match, return false
  otherwise return true.

NOT (implied OR)
  loop through the rules
  as soon as one matches, return false
  otherwise return true.

ALL
  return true

FILENAME
  if the rule string matches the desktop id return true
  otherwise return false

CATEGORY
  loop through the apps categories
  as soon as one matches the rule string, return true
  otherwise return false.
 */


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
*  for each <Menu> recursively
*    if there are <Deleted> elements that are not ovreridden by a <NotDelete> element
*      remove this <Menu> element and all it's children.

*generate menus
*  for each <Menu> that is <NotOnlyUnallocated> (which is the default)
*    for each <AppDir>
*      for each .desktop
*        if it exists in the pool, replace it.
*	 else add it to the pool.
*     for each parent <Menu>
*       for each .desktop in the pool
*          if it doesn't exist in the child <Menu> pool
*	    add it to the pool.
*     for each <Include> and <Exclude>
*        if rule is an <Include>
*           for each .desktop in pool
*              for each rule
*                 if rule matches .desktop in pool
*	           add .desktop to menu.
*	           mark it as allocated
*        if rule is an <Exclude>
*           for each .desktop in menu
*              for each rule
*                if rule matches .desktop in menu
*	           remove .desktop from menu.
*	           leave it as allocated.


<Menu (list)
  name
  flags = "   " or "MDO" the first letter of - Marked, Deleted, OnlyUnallocated 
  pool (hash)
    id = path
    id = path
  rules (list)
    rule
    rule
  menu (hash)
    id = path
    id = path
  <Menu (list)
  <Menu (list)

rules (list)
  include/exclude or all/file/category x
  and/not (list)
    include/exclude and/not all/file/category x


*generate unallocated menus
*  Same as for menus, but only the <OnlyUnallocated> ones.
*  Only the unallocated .desktop entries can be used.

generate menu layout
*/

