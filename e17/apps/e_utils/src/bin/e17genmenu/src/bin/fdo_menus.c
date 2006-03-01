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
#include "xmlame.h"

extern double convert_time;

struct _fdo_menus_expand_apps_data
{
   char *path;
   Ecore_Hash *pool;
   int length;
};

struct _fdo_menus_unxml_data
{
   char *file;
   char *base;
   char *path;
   Dumb_Tree *stack, *merge_stack;
   int unallocated, level;
};

struct _fdo_menus_generate_data
{
   char *name, *path;
   Dumb_Tree *rules;
   Ecore_Hash *pool, *apps;
   int unallocated;

   Dumb_Tree *rule;
   int include;
};

struct _fdo_menus_legacy_data
{
   Dumb_Tree *merge;
   Dumb_Tree *current;
   char *prefix;
   char *path;
   int   length;
};


static int _fdo_menus_unxml(const void *data, Dumb_Tree * tree, int element, int level);
static int _fdo_menus_check_directory(const void *data, char *path);
static int _fdo_menus_check_menu(const void *data, char *path);
static int _fdo_menus_legacy_menu_dir(const void *data, char *path);
static int _fdo_menus_legacy_menu(const void *data, char *path);
static void _fdo_menus_unxml_rules(Dumb_Tree * rules, Dumb_Tree * tree, char type, char sub_type);
static void _fdo_menus_unxml_moves(Dumb_Tree * menu, Dumb_Tree * tree);
static void _fdo_menus_add_dirs(Dumb_Tree * tree, Dumb_Tree * paths, char *pre, char *post, char *extra, int element);
static int _fdo_menus_expand_apps(struct _fdo_menus_unxml_data *unxml_data, char *app_dir, Ecore_Hash * pool);
static int _fdo_menus_check_app(const void *data, char *path);

static int _fdo_menus_merge(const void *data, Dumb_Tree * tree, int element, int level);
static int _fdo_menus_expand_default_dirs(const void *data, Dumb_Tree * tree, int element, int level);

static int _fdo_menus_generate(const void *data, Dumb_Tree * tree, int element, int level);
static void _fdo_menus_inherit_apps(void *value, void *user_data);
static void _fdo_menus_select_app(void *value, void *user_data);
static int _fdo_menus_apply_rules(struct _fdo_menus_generate_data *generate_data, Dumb_Tree * rule, char *key, Desktop * desktop);

Dumb_Tree *
fdo_menus_get(char *file, Dumb_Tree * merge_stack, int level)
{
   Dumb_Tree *menu_xml;
   struct _fdo_menus_unxml_data data;
   double begin;
   int oops = 0;

   /* Preperation. */
   begin = ecore_time_get();
   data.stack = dumb_tree_new(NULL);
   data.base = ecore_file_strip_ext(ecore_file_get_file(file));
   data.path = ecore_file_get_dir(file);
   if ((level == 0) && (merge_stack == NULL))
      merge_stack = dumb_tree_new(NULL);
   menu_xml = xmlame_get(file);
   if ((data.stack) && (data.base) && (data.path) && (merge_stack) && (menu_xml))
     {
        int i;

        data.file = file;
        data.level = level;
        data.merge_stack = merge_stack;

        /* Setup the merge stack. */
        if (merge_stack->size <= level)
          {
             while (merge_stack->size < level)
                dumb_tree_add(merge_stack, "");
             dumb_tree_add(merge_stack, file);
          }
        else
           merge_stack->elements[level].element = file;

        /* Find out if we are looping. */
        for (i = 0; i < level; i++)
          {
             char *text;

             /* I can safely assume that they are all strings. */
             text = (char *)merge_stack->elements[i].element;
             if (strcmp(text, file) == 0)
               {
                  fprintf(stderr, "\n### Oops, infinite menu merging loop detected at %s\n", file);
                  oops++;
               }
          }

        if (oops == 0)
          {
             /* Get on with it. */
             dumb_tree_foreach(menu_xml, 0, _fdo_menus_unxml, &data);
             dumb_tree_foreach(menu_xml, 0, _fdo_menus_merge, &data);

             /* The rest of this is only done after ALL the menus have been merged. */
             if (level == 0)
               {
                  dumb_tree_foreach(menu_xml, 0, _fdo_menus_expand_default_dirs, &data);

                  convert_time += ecore_time_get() - begin;
                  dumb_tree_dump(menu_xml, 0);
                  printf("\n\n");
                  begin = ecore_time_get();

                  data.unallocated = FALSE;
                  dumb_tree_foreach(menu_xml, 0, _fdo_menus_generate, &data);
                  data.unallocated = TRUE;
                  dumb_tree_foreach(menu_xml, 0, _fdo_menus_generate, &data);

                  convert_time += ecore_time_get() - begin;
                  dumb_tree_dump(menu_xml, 0);
                  printf("\n\n");
                  begin = ecore_time_get();
               }
          }
     }
   else
      oops++;

   if (oops)
     {
        E_FN_DEL(dumb_tree_del, (menu_xml));
        if (level == 0)
          {
             E_FN_DEL(dumb_tree_del, (merge_stack));
          }
        E_FREE(data.path);
        E_FREE(data.base);
        E_FN_DEL(dumb_tree_del, (data.stack));
     }

   if (level == 0)
      convert_time += ecore_time_get() - begin;

   return menu_xml;
}

static int
_fdo_menus_unxml(const void *data, Dumb_Tree * tree, int element, int level)
{
   struct _fdo_menus_unxml_data *unxml_data;

   unxml_data = (struct _fdo_menus_unxml_data *)data;
   if (tree->elements[element].type == DUMB_TREE_ELEMENT_TYPE_STRING)
     {
        if (strncmp((char *)tree->elements[element].element, "<!", 2) == 0)
          {
             tree->elements[element].type = DUMB_TREE_ELEMENT_TYPE_NULL;
             tree->elements[element].element = NULL;
          }
        else if (strcmp((char *)tree->elements[element].element, "<Menu") == 0)
          {
             Dumb_Tree *menu, *rules;
             Ecore_Hash *pool, *apps;

             menu = dumb_tree_new(NULL);
             rules = dumb_tree_new(NULL);
             pool = ecore_hash_new(ecore_str_hash, ecore_str_compare);
             apps = ecore_hash_new(ecore_str_hash, ecore_str_compare);
             if ((menu) && (rules) && (pool) && (apps))
               {
                  int i;
                  char *flags = "    ", *name = "", *directory = "", *menu_path = "";
                  char temp[MAX_PATH];

                  ecore_hash_set_free_key(pool, free);
                  ecore_hash_set_free_value(pool, free);
                  ecore_hash_set_free_key(apps, free);
                  ecore_hash_set_free_value(apps, free);
                  sprintf(temp, "<MENU <%.4s> <%s> <%s>", flags, name, directory);
                  dumb_tree_extend(menu, temp);
                  sprintf(temp, "<MENU_PATH %s", menu_path);
                  dumb_tree_extend(menu, temp);
                  flags = (char *)menu->elements[0].element;
                  flags += 7;
                  dumb_tree_add_hash(menu, pool);
                  dumb_tree_add_child(menu, rules);
                  dumb_tree_add_hash(menu, apps);
                  tree->elements[element].element = menu;
                  tree->elements[element].type = DUMB_TREE_ELEMENT_TYPE_TREE;
                  for (i = element + 1; i < tree->size; i++)
                    {
                       int result = 0;

                       if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_STRING)
                         {
                            if (strncmp((char *)tree->elements[i].element, "<!", 2) == 0)
                              {
                                 tree->elements[i].type = DUMB_TREE_ELEMENT_TYPE_NULL;
                                 tree->elements[i].element = NULL;
                              }
                            else if (strcmp((char *)tree->elements[i].element, "<Deleted/") == 0)
                              {
                                 flags[1] = 'D';
                                 result = 1;
                              }
                            else if (strcmp((char *)tree->elements[i].element, "<NotDeleted/") == 0)
                              {
                                 flags[1] = ' ';
                                 result = 1;
                              }
                            else if (strcmp((char *)tree->elements[i].element, "<OnlyUnallocated/") == 0)
                              {
                                 flags[2] = 'O';
                                 result = 1;
                              }
                            else if (strcmp((char *)tree->elements[i].element, "<NotOnlyUnallocated/") == 0)
                              {
                                 flags[2] = ' ';
                                 result = 1;
                              }
                            else if (strcmp((char *)tree->elements[i].element, "</Menu") == 0)
                              {
                                 result = 1;
                              }
                            else
                              {
                                 dumb_tree_extend(menu, (char *)tree->elements[i].element);
                                 result = 1;
                              }
                         }
                       else if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_TREE)
                         {
                            Dumb_Tree *sub;

                            sub = (Dumb_Tree *) tree->elements[i].element;
                            if ((sub) && (sub->size))
                              {
                                 if (sub->elements[0].type == DUMB_TREE_ELEMENT_TYPE_STRING)
                                   {
                                      if (strcmp((char *)sub->elements[0].element, "<Name") == 0)
                                        {
                                           int i, length = 0;

                                           name = strdup((char *)sub->elements[1].element);
                                           sprintf(temp, "<MENU <%.4s> <%s> <%s>", flags, name, directory);
                                           menu->elements[0].element = strdup(temp);
                                           dumb_tree_track(menu, name);
                                           dumb_tree_track(menu, menu->elements[0].element);
                                           flags = (char *)menu->elements[0].element;
                                           flags += 7;
                                           /* The rest of this is probably not needed, except to ease debugging. */
                                           if (unxml_data->stack->size <= level)
                                             {
                                                while (unxml_data->stack->size < level)
                                                   dumb_tree_add(unxml_data->stack, "");
                                                dumb_tree_add(unxml_data->stack, name);
                                             }
                                           else
                                              unxml_data->stack->elements[level].element = name;
                                           temp[0] = '\0';
                                           for (i = 0; i <= level; i++)
                                             {
                                                char *text;

                                                /* I can safely assume that they are all strings. */
                                                text = (char *)unxml_data->stack->elements[i].element;
                                                if (text[0] != '\0')
                                                  {
                                                     sprintf(&temp[length], "%s%s", ((length) ? "/" : ""), text);
                                                     length += strlen(text) + ((length) ? 1 : 0);
                                                  }
                                             }
                                           menu_path = strdup(temp);
                                           sprintf(temp, "<MENU_PATH %s", menu_path);
                                           menu->elements[1].element = strdup(temp);
                                           dumb_tree_track(menu, menu_path);
                                           dumb_tree_track(menu, menu->elements[1].element);
                                           result = 1;
                                        }
                                      /* FIXME: Move this to later in the sequence. */
                                      else if (strcmp((char *)sub->elements[0].element, "<Directory") == 0)
                                        {
                                           directory = strdup((char *)sub->elements[1].element);
                                           sprintf(temp, "<MENU <%.4s> <%s> <%s>", flags, name, directory);
                                           menu->elements[0].element = strdup(temp);
                                           dumb_tree_track(menu, directory);
                                           dumb_tree_track(menu, menu->elements[0].element);
                                           flags = (char *)menu->elements[0].element;
                                           flags += 7;
                                           result = 1;
                                        }
                                      else if (strcmp((char *)sub->elements[0].element, "<Menu") == 0)
                                        {
                                           _fdo_menus_unxml(data, sub, 0, level + 1);
                                           dumb_tree_add_child(menu, (Dumb_Tree *) sub->elements[0].element);
                                           /* FIXME: Dunno if this causes a memory leak, but for now we play it safe. */
                                           tree->elements[i].type = DUMB_TREE_ELEMENT_TYPE_NULL;
                                           tree->elements[i].element = NULL;
//                                                 result = 1;
                                        }
                                      else if (strcmp((char *)sub->elements[0].element, "<Move") == 0)
                                        {
                                           _fdo_menus_unxml_moves(menu, sub);
                                           result = 1;
                                        }
                                      else
                                        {
                                           if ((sub->size == 3) && (sub->elements[1].type == DUMB_TREE_ELEMENT_TYPE_STRING)
                                               && (((char *)sub->elements[1].element)[0] != '<'))
                                             {
                                                char temp[MAX_PATH];

                                                sprintf(temp, "%s %s", (char *)sub->elements[0].element, (char *)sub->elements[1].element);
                                                dumb_tree_extend(menu, temp);
                                                result = 1;
                                             }
                                           else
                                             {
                                                dumb_tree_add_child(menu, sub);
                                                tree->elements[i].type = DUMB_TREE_ELEMENT_TYPE_NULL;
                                                tree->elements[i].element = NULL;
                                             }
                                        }
                                   }
                              }
                         }
                       if (result)
                         {
                            if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_TREE)
                               dumb_tree_del((Dumb_Tree *) tree->elements[i].element);
                            tree->elements[i].type = DUMB_TREE_ELEMENT_TYPE_NULL;
                            tree->elements[i].element = NULL;
                         }
                    }

                  /* Add it if it has not been deleted. */
                  if (flags[1] != 'D')
                    {
                       tree->elements[element].element = menu;
                       tree->elements[element].type = DUMB_TREE_ELEMENT_TYPE_TREE;
                    }
               }
             else
               {
                  if (apps)
                     ecore_hash_destroy(apps);
                  if (pool)
                     ecore_hash_destroy(pool);
                  if (rules)
                     dumb_tree_del(rules);
                  if (menu)
                     dumb_tree_del(menu);
               }
          }
     }
   return 0;
}

static int
_fdo_menus_check_directory(const void *data, char *path)
{
   char *p;
   Dumb_Tree *merge;

   merge = (Dumb_Tree *) data;
   p = strrchr(path, '.');
   if (p)
     {
        if (strcmp(p, ".directory") == 0)
          {
             char merge_file[MAX_PATH];

             sprintf(merge_file, "<Directory %s", path);
             dumb_tree_extend(merge, merge_file);
          }
     }

   return 0;
}

static int
_fdo_menus_check_menu(const void *data, char *path)
{
   char *p;
   Dumb_Tree *merge;

   merge = (Dumb_Tree *) data;
   p = strrchr(path, '.');
   if (p)
     {
        if (strcmp(p, ".menu") == 0)
          {
             char merge_file[MAX_PATH];

             sprintf(merge_file, "<MergeFile type=\"path\" %s", path);
             dumb_tree_extend(merge, merge_file);
          }
     }

   return 0;
}

static int
_fdo_menus_legacy_menu_dir(const void *data, char *path)
{
   struct _fdo_menus_legacy_data *legacy_data;

   legacy_data = (struct _fdo_menus_legacy_data *)data;
   printf("  LEGACYDIR DIR  - %s - %s  %s\n", legacy_data->prefix, legacy_data->path, &path[legacy_data->length]);
   return 0;
}

static int
_fdo_menus_legacy_menu(const void *data, char *path)
{
   struct _fdo_menus_legacy_data *legacy_data;

   legacy_data = (struct _fdo_menus_legacy_data *)data;
   printf("  LEGACYDIR FILE - %s - %s  %s\n", legacy_data->prefix, legacy_data->path, &path[legacy_data->length]);
   return 0;
}

static void
_fdo_menus_unxml_rules(Dumb_Tree * rules, Dumb_Tree * tree, char type, char sub_type)
{
   int i;
   char temp[MAX_PATH];

   for (i = 0; i < tree->size; i++)
     {
        if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_STRING)
          {
             if (strcmp((char *)tree->elements[i].element, "<All/") == 0)
               {
                  sprintf(temp, "%c%cA", type, sub_type);
                  dumb_tree_extend(rules, temp);
               }
             else if (strcmp((char *)tree->elements[i].element, "<Filename") == 0)
               {
                  sprintf(temp, "%c%cF %s", type, sub_type, (char *)tree->elements[i + 1].element);
                  dumb_tree_extend(rules, temp);
               }
             else if (strcmp((char *)tree->elements[i].element, "<Category") == 0)
               {
                  sprintf(temp, "%c%cC %s", type, sub_type, (char *)tree->elements[i + 1].element);
                  dumb_tree_extend(rules, temp);
               }
             else if (strcmp((char *)tree->elements[i].element, "<Or") == 0)
               {
                  _fdo_menus_unxml_rules(rules, (Dumb_Tree *) tree->elements[i + 1].element, type, sub_type);
               }
             else if ((strcmp((char *)tree->elements[i].element, "<And") == 0) || (strcmp((char *)tree->elements[i].element, "<Not") == 0))
               {
                  char this_type;
                  Dumb_Tree *sub;

                  this_type = ((char *)tree->elements[i].element)[1];
                  sub = dumb_tree_new(NULL);
                  if (sub)
                    {
                       dumb_tree_add_child(rules, sub);
                       for (i++; i < tree->size; i++)
                         {
                            if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_TREE)
                               _fdo_menus_unxml_rules(sub, (Dumb_Tree *) tree->elements[i].element, type, this_type);
                         }
                    }
               }
          }
        else if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_TREE)
          {
             _fdo_menus_unxml_rules(rules, (Dumb_Tree *) tree->elements[i].element, type, sub_type);
          }
     }
}

static void
_fdo_menus_unxml_moves(Dumb_Tree * menu, Dumb_Tree * tree)
{
   int i;
   char *old = NULL;
   char *new = NULL;

   for (i = 0; i < tree->size; i++)
     {
        if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_TREE)
          {
             Dumb_Tree *sub;

             sub = (Dumb_Tree *) tree->elements[i].element;
             if ((sub) && (sub->size))
               {
                  if (sub->elements[0].type == DUMB_TREE_ELEMENT_TYPE_STRING)
                    {
                       if (strcmp((char *)sub->elements[0].element, "<Old") == 0)
                          old = strdup((char *)sub->elements[1].element);
                       if (strcmp((char *)sub->elements[0].element, "<New") == 0)
                          new = strdup((char *)sub->elements[1].element);
                    }
               }
          }
        if ((old) && (new))
          {
             char temp[MAX_PATH * 2];

             sprintf(temp, "<MOVE <%s> <%s>", old, new);
             dumb_tree_extend(menu, temp);
             free(old);
             old = NULL;
             free(new);
             new = NULL;
          }
     }
}

static void
_fdo_menus_add_dirs(Dumb_Tree * tree, Dumb_Tree * paths, char *pre, char *post, char *extra, int element)
{
   int i;
   char t[MAX_PATH];

   /* reverse the order of the dirs. */
   for (i = paths->size - 1; i >= 0; i--)
     {
        if (extra)
           sprintf(t, "%s %s%s-merged/", pre, (char *)paths->elements[i].element, extra);
        else
           sprintf(t, "%s %s", pre, (char *)paths->elements[i].element);
        if (tree)
           dumb_tree_extend(tree, t);
     }
}

static int
_fdo_menus_expand_apps(struct _fdo_menus_unxml_data *unxml_data, char *app_dir, Ecore_Hash * pool)
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
        fdo_paths_recursive_search(dir, NULL, NULL, _fdo_menus_check_app, &our_data);
     }
   return 0;
}

static int
_fdo_menus_check_app(const void *data, char *path)
{
   char *p;
   struct _fdo_menus_expand_apps_data *our_data;

   our_data = (struct _fdo_menus_expand_apps_data *)data;
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
                  for (i = 0; file[i] != '\0'; i++)
                     if (file[i] == '/')
                        file[i] = '-';
                  ecore_hash_set(our_data->pool, file, strdup(path));
               }
          }
     }

   return 1;
}

static int
_fdo_menus_merge(const void *data, Dumb_Tree * tree, int element, int level)
{
   struct _fdo_menus_unxml_data *unxml_data;
   Dumb_Tree *merge;
   int result = 0;

   unxml_data = (struct _fdo_menus_unxml_data *)data;
   merge = dumb_tree_new(NULL);
   if (tree->elements[element].type == DUMB_TREE_ELEMENT_TYPE_STRING)
     {
        char *string;

        string = (char *)tree->elements[element].element;
        if (strcmp(string, "<DefaultMergeDirs/") == 0)
          {
             if (unxml_data->base)
                _fdo_menus_add_dirs(merge, fdo_paths_menus, "<MergeDir", "</MergeDir", unxml_data->base, element);
             result = 1;
          }
        else if (strcmp(string, "<KDELegacyDirs/") == 0)
          {
             _fdo_menus_add_dirs(merge, fdo_paths_kde_legacy, "<LegacyDir prefix=\"kde-\"", "</LegacyDir", NULL, element);
             result = 1;
          }
        else if (strncmp(string, "<MergeDir ", 10) == 0)
          {
             char merge_path[MAX_PATH];

             if (string[10] == '/')
                sprintf(merge_path, "%s", &string[10]);
             else
                sprintf(merge_path, "%s%s", unxml_data->path, &string[10]);
             fdo_paths_recursive_search(merge_path, NULL, NULL, _fdo_menus_check_menu, merge);
             result = 1;
          }
        else if (strncmp(string, "<LegacyDir ", 11) == 0)
          {
             char merge_path[MAX_PATH];
             struct _fdo_menus_legacy_data legacy_data;

             string += 11;
	     legacy_data.prefix = NULL;
	     legacy_data.merge = merge;
             if (strncmp(string, "prefix=\"", 8) == 0)
               {
                  string += 8;
		  legacy_data.prefix = string;
                  while ((*string != '"') && (*string != '\0'))
                     string++;
                  if (*string != '\0')
                     *string++ = '\0';
                  while ((*string == ' ') && (*string != '\0'))
                     string++;
               }
             if (string[0] == '/')
                sprintf(merge_path, "%s", string);
             else
                sprintf(merge_path, "%s%s", unxml_data->path, string);
	     legacy_data.path = merge_path;
	     legacy_data.length = strlen(merge_path);
	     printf("<LEGACYDIR> - %s - %s\n", legacy_data.prefix, merge_path);
             fdo_paths_recursive_search(merge_path, NULL, _fdo_menus_legacy_menu_dir, _fdo_menus_legacy_menu, &legacy_data);
             result = 1;
          }
        else if (strncmp(string, "<MergeFile ", 11) == 0)
          {
             char merge_path[MAX_PATH];
             int path_type = 1;

             /* FIXME: need to weed out duplicate <MergeFile's, use the last one. */
             string += 11;
             if (strncmp(string, "type=\"", 6) == 0)
               {
                  string += 6;
                  if (strncmp(string, "parent\"", 7) == 0)
                     path_type = 0;
                  while ((*string != '"') && (*string != '\0'))
                     string++;
                  if (*string != '\0')
                     string++;
                  while ((*string == ' ') && (*string != '\0'))
                     string++;
               }
             if (path_type)
               {
                  if (string[0] == '/')
                     sprintf(merge_path, "%s", string);
                  else
                     sprintf(merge_path, "%s/%s", unxml_data->path, string);
               }
             else               /* This is a parent type MergeFile. */
               {
                  /* The spec is a little unclear, and the examples may look like they
                   * contradict the description, but it all makes sense if you cross
                   * reference it with the XDG Base Directory Specification (version 0.6).
                   * To make things harder, parent type MergeFiles never appear on my box.
                   *
                   * What you do is this.
                   *
                   * Take the XDG_CONFIG_DIRS stuff as a whole ($XDG_CONFIG_HOME, then 
                   * $XDG_CONFIG_DIRS), in this code that will be fdo_paths_config.
                   *
                   * If this menu file is from one of the directories in fdo_paths_config,
                   * scan the rest of fdo_paths_config looking for the new menu.  In other 
                   * words start searching in the next fdo_paths_config entry after the one
                   * that this menu is in.
                   *
                   * The file to look for is the path to this menu with the portion from
                   * fdo_paths_config stripped off the beginning.  For instance, the top level
                   * menu file is typically /etc/xdg/menus/applications.menu, and /etc/xdg is
                   * typically in fdo_paths_config, so search for menus/applications.menu.
                   *
                   * If this menu file is NOT from one of the directories in fdo_paths_menus,
                   * insert nothing.
                   *
                   * The first one found wins, if none are found, don't merge anything.
                   */

                  /* FIXME: Actually implement this when I have some menus that will exercise it. */
                  merge_path[0] = '\0';
                  printf("\n### Didn't expect a MergeFile parent type\n");
               }
             if (merge_path[0] != '\0')
               {
                  Dumb_Tree *new_menu;

                  new_menu = fdo_menus_get(merge_path, unxml_data->merge_stack, level + 1);
                  if (new_menu)
                    {
                       if (new_menu->size > 1)
                         {
                            if (new_menu->elements[1].type == DUMB_TREE_ELEMENT_TYPE_TREE)
                              {
                                 new_menu = (Dumb_Tree *) new_menu->elements[1].element;
                                 if (new_menu->size > 0)
                                   {
                                      if (new_menu->elements[0].type == DUMB_TREE_ELEMENT_TYPE_TREE)
                                        {
                                           merge = (Dumb_Tree *) new_menu->elements[0].element;
                                           dumb_tree_remove(merge, 0);
                                           dumb_tree_remove(merge, 1);
                                           dumb_tree_remove(merge, 2);
                                           dumb_tree_remove(merge, 3);
                                           dumb_tree_remove(merge, 4);
                                           /* FIXME: The MENU_PATHs need to be prefixed. */
                                        }
                                      else
                                         printf("FUCK an error in _fdo_menus_merge(%s)\n", merge_path);
                                   }
                                 else
                                    printf("FUCK another error in _fdo_menus_merge(%s)\n", merge_path);
                              }
                            else
                               printf("FUCK ME! An error in _fdo_menus_merge(%s)\n", merge_path);
                         }
                    }
               }
             result = 1;
          }
     }

   if (result)
     {
        if ((merge) && (merge->size))
           dumb_tree_merge(tree, element + 1, merge);

        tree->elements[element].type = DUMB_TREE_ELEMENT_TYPE_NULL;
        tree->elements[element].element = NULL;
     }

   return 0;
}

static int
_fdo_menus_expand_default_dirs(const void *data, Dumb_Tree * tree, int element, int level)
{
   struct _fdo_menus_unxml_data *unxml_data;
   Dumb_Tree *merge;
   int result = 0;

   unxml_data = (struct _fdo_menus_unxml_data *)data;
   merge = dumb_tree_new(NULL);
   if (tree->elements[element].type == DUMB_TREE_ELEMENT_TYPE_STRING)
     {
        char *string;

        string = (char *)tree->elements[element].element;
        if (strcmp(string, "<DefaultAppDirs/") == 0)
          {
             _fdo_menus_add_dirs(merge, fdo_paths_desktops, "<AppDir", "</AppDir", NULL, element);
             result = 1;
          }
        else if (strcmp(string, "<DefaultDirectoryDirs/") == 0)
          {
             _fdo_menus_add_dirs(merge, fdo_paths_directories, "<DirectoryDir", "</DirectoryDir", NULL, element);
             result = 1;
          }
     }
   if (result)
     {
        if ((merge) && (merge->size))
           dumb_tree_merge(tree, element + 1, merge);

        tree->elements[element].type = DUMB_TREE_ELEMENT_TYPE_NULL;
        tree->elements[element].element = NULL;
     }

   return 0;
}

static int
_fdo_menus_generate(const void *data, Dumb_Tree * tree, int element, int level)
{
   struct _fdo_menus_unxml_data *unxml_data;

   unxml_data = (struct _fdo_menus_unxml_data *)data;
   if (tree->elements[element].type == DUMB_TREE_ELEMENT_TYPE_STRING)
     {
        if (strncmp((char *)tree->elements[element].element, "<MENU ", 6) == 0)
          {
             int i;
             struct _fdo_menus_generate_data generate_data;

             generate_data.unallocated = unxml_data->unallocated;
             generate_data.name = (char *)tree->elements[element].element;
             generate_data.path = (char *)tree->elements[element + 1].element;
             generate_data.pool = (Ecore_Hash *) tree->elements[element + 2].element;
             generate_data.rules = (Dumb_Tree *) tree->elements[element + 3].element;
             generate_data.apps = (Ecore_Hash *) tree->elements[element + 4].element;

             /* generate and inherit the pools on the first pass, and preparse the include/exclude logic. */
             if (!generate_data.unallocated)
               {
                  int i;

                  for (i = element + 5; i < tree->size; i++)
                    {
                       int result = 0;
                       char *string;

                       if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_STRING)
                         {
                            string = (char *)tree->elements[i].element;
                            if (strncmp(string, "<AppDir ", 8) == 0)
                              {
                                 _fdo_menus_expand_apps(unxml_data, &string[8], generate_data.pool);
                                 result = 1;
                              }
                            else if (strncmp(string, "<DirectoryDir ", 14) == 0)
                              {
                                 char merge_path[MAX_PATH];
                                 Dumb_Tree *merge;

                                 if (string[14] == '/')
                                    sprintf(merge_path, "%s", &string[14]);
                                 else
                                    sprintf(merge_path, "%s%s", unxml_data->path, &string[14]);
                                 merge = dumb_tree_new(NULL);
                                 if (merge)
                                   {
                                      fdo_paths_recursive_search(merge_path, NULL, NULL, _fdo_menus_check_directory, merge);
                                      dumb_tree_merge(tree, i + 1, merge);
                                   }
                                 result = 1;
                              }
                         }
                       else if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_TREE)
                         {
                            Dumb_Tree *sub;

                            sub = (Dumb_Tree *) tree->elements[i].element;
                            if ((sub) && (sub->size))
                              {
                                 if (sub->elements[0].type == DUMB_TREE_ELEMENT_TYPE_STRING)
                                   {
                                      string = (char *)sub->elements[0].element;
                                      if ((strcmp(string, "<Include") == 0) || (strcmp(string, "<Exclude") == 0))
                                        {
                                           Dumb_Tree *new_sub;

                                           new_sub = dumb_tree_new(NULL);
                                           if (new_sub)
                                             {
                                                dumb_tree_add_child(generate_data.rules, new_sub);
                                                _fdo_menus_unxml_rules(new_sub, sub, string[1], 'O');
                                             }
                                           result = 1;
                                        }
                                   }
                              }
                         }

                       if (result)
                         {
                            tree->elements[i].type = DUMB_TREE_ELEMENT_TYPE_NULL;
                            tree->elements[i].element = NULL;
                         }
                    }

                  if (unxml_data->stack->size <= level)
                    {
                       while (unxml_data->stack->size < level)
                          dumb_tree_add_hash(unxml_data->stack, generate_data.pool);
                       dumb_tree_add_hash(unxml_data->stack, generate_data.pool);
                    }
                  else
                    {
                       unxml_data->stack->elements[level].type = DUMB_TREE_ELEMENT_TYPE_HASH;
                       unxml_data->stack->elements[level].element = generate_data.pool;
                    }
                  for (i = level - 1; i >= 0; i--)
                    {
                       if (unxml_data->stack->elements[i].type == DUMB_TREE_ELEMENT_TYPE_HASH)
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
                       if (generate_data.rules->elements[i].type == DUMB_TREE_ELEMENT_TYPE_TREE)
                         {
                            generate_data.rule = (Dumb_Tree *) generate_data.rules->elements[i].element;
                            if (generate_data.rule->size > 0)
                              {
                                 if (((char *)generate_data.rule->elements[0].element)[0] == 'I')
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
   key = (char *)node->key;
   app = (char *)node->value;
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
   generate_data = (struct _fdo_menus_generate_data *)user_data;
   key = (char *)node->key;
   app = (char *)node->value;

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
_fdo_menus_apply_rules(struct _fdo_menus_generate_data *generate_data, Dumb_Tree * rule, char *key, Desktop * desktop)
{
   char type = 'O';
   int result = FALSE;
   int i;

   for (i = 0; i < rule->size; i++)
     {
        if (rule->elements[i].type == DUMB_TREE_ELEMENT_TYPE_TREE)
          {
             result = _fdo_menus_apply_rules(generate_data, (Dumb_Tree *) rule->elements[i].element, key, desktop);
          }
        else
          {
             char *rul;
             char match;
             int sub_result = FALSE;

             rul = (char *)rule->elements[i].element;
             type = rul[1];
             match = rul[2];
             switch (match)
               {
               case 'A':
                  {
                     sub_result = TRUE;
                     break;
                  }

               case 'F':
                  {
                     if (strcmp(key, &rul[4]) == 0)
                        sub_result = TRUE;
                     break;
                  }

               case 'C':
                  {
                     /* Try to match a category. */
                     if (desktop->Categories)
                       {
                          int j;

                          for (j = 0; j < desktop->Categories->size; j++)
                            {
                               if (strcmp((char *)(desktop->Categories->elements[j].element), &rul[4]) == 0)
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
               case 'A':
                  {
                     result = TRUE;
                     if (!sub_result)
                        return FALSE;
                     break;
                  }

               case 'N':
                  {
                     result = TRUE;
                     if (sub_result)
                        return FALSE;
                     break;
                  }

               default:
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
*  expand <KDELegacyDir>'s to <LegacyDir>.
   expand <LegacyDir>'s
     for each dir (recursive)
       create recursively nested <MENU <   L> <dirname> <> element
    //   <AppDir>dirpath</AppDir>
       <DirectoryDir>dirpath</DirectoryDir>
       if exist .directory
         add <.directory> to name
       <Include>
       for each *.desktop
         if no categories in bar.desktop
	   <Filename>prefix-bar.desktop</Filename> 
         add "Legacy" to categories
         add any prefix to the desktop ID.
	 add it to the pool
       </Include>
*  for each <MergeFile>, and <MergeDir> element
*    get the root <Menu> elements from that elements file/s.
*    remove the <Name> element from those root <Menu> elements.
*    replace that element with the child elements of those root <Menu> elements.
*    expand the <DefaultMergeDirs> with the name/s of that elements file/s
*  loop until all <MergeFile>, <MergeDir>, and <LegacyDir> elements are done,
*  careful to avoid infinite loops in files that reference each other.
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

<Menu (tree)
  name
  flags = "   " or "MDO" the first letter of - Marked, Deleted, OnlyUnallocated 
  pool (hash)
    id = path
    id = path
  rules (tree)
    rule
    rule
  menu (hash)
    id = path
    id = path
  <Menu (tree)
  <Menu (tree)

rules (tree)
  include/exclude or all/file/category x
  and/not (tree)
    include/exclude and/not all/file/category x

*generate unallocated menus
*  Same as for menus, but only the <OnlyUnallocated> ones.
*  Only the unallocated .desktop entries can be used.

generate menu layout
*/
