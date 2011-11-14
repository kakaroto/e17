#include <stdbool.h>
#include <stdio.h>

#include <Elementary.h>

#include "config.h"
#include "ensure.h"
#include "file.h"
#include "parser.h"
#include "enobj.h"
#include "results.h"

struct fileselector
{
   Evas_Object *win;
   Evas_Object *fs;
   bool         save;
};

static int  create_fileselector(struct ensure *ensure);
static void fs_done(void *ensurev, Evas_Object *obj, void *eventinfo);
static int  file_save_do(struct ensure *ensure, const char *file);
static int  file_load_do(struct ensure *ensure, const char *file);

/**
 * Load a saved data set
 */
void
file_save(void *ensurev, Evas_Object *button __UNUSED__, void *event_info __UNUSED__)
{
   struct ensure *ensure;
   struct fileselector *fs;

   printf("save");

   ensure = ensurev;

   if (!ensure->fileselector)
     create_fileselector(ensure);
   if (!ensure->fileselector)
     return;

   fs = ensure->fileselector;
   elm_fileselector_is_save_set(fs->fs, true);
   fs->save = true;
}

/**
 * Load a specified data set
 */
void
file_load(void *ensurev, Evas_Object *button __UNUSED__, void *event_info __UNUSED__)
{
   struct ensure *ensure;
   struct fileselector *fs;

   printf("load");

   ensure = ensurev;

   if (!ensure->fileselector)
     create_fileselector(ensure);
   if (!ensure->fileselector)
     return;

   fs = ensure->fileselector;

   elm_win_title_set(fs->win, "Ensure::Load file");
   elm_fileselector_is_save_set(fs->fs, false);
   fs->save = false;
}

static int
create_fileselector(struct ensure *ensure)
{
   struct fileselector *fs;
   Evas_Object *win, *bg, *fsw, *box;
   char *path;

   if (ensure->fileselector)
     return 0;

   fs = calloc(1, sizeof(struct fileselector));
   if (!fs)
     return -1;

   fs->win = win = elm_win_add(NULL, "fileselector", ELM_WIN_BASIC);
   evas_object_resize(win, 340, 500);
   elm_win_title_set(win, "File Selector");
//      elm_win_autodel_set(win, 1);

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   box = elm_box_add(win);
   elm_win_resize_object_add(win, box);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(box);

   fs->fs = fsw = elm_fileselector_add(win);
   elm_fileselector_expandable_set(fsw, EINA_FALSE);
   path = getcwd(NULL, 0);
   elm_fileselector_path_set(fsw, path);
   free(path);
   evas_object_size_hint_weight_set(fsw, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fsw, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(box, fsw);
   evas_object_show(fsw);

   evas_object_show(win);

   /* the 'done' cb is called when the user press ok/cancel */
   evas_object_smart_callback_add(fsw, "done", fs_done, ensure);

   ensure->fileselector = fs;

   return 0;
}

/**
 * Someone clicked 'okay' (or cancel)
 */
static void
fs_done(void *ensurev, Evas_Object *obj __UNUSED__, void *eventinfo)
{
   const char *path;
   struct ensure *ensure = ensurev;
   struct fileselector *fs;
   printf("fs done\n");

   if (!ensure || !ensure->fileselector)
     return;
   fs = ensure->fileselector;

   evas_object_hide(fs->win);

   if (eventinfo == NULL)
     {
        /* Cancel */
        return;
     }

   path = eventinfo;
   if (fs->save)
     {
        file_save_do(ensure, path);
     }
   else
     {
        file_load_do(ensure, path);
     }
}

static int
file_save_do(struct ensure *ensure, const char *file)
{
   struct result *cur;
   struct enwin *enwin;
   struct enobj *enobj;
   Eina_Iterator *iter;
   Eina_List *l;
   FILE *fp;

   cur = ensure->cur;

   fp = fopen(file, "w");
   if (!fp)
     return -1;

   EINA_LIST_FOREACH (cur->windows, l, enwin)
     {
        parser_save_window(enwin, fp);
        /* Save all objects in that window */
        iter = eina_hash_iterator_data_new(cur->objdb);
        EINA_ITERATOR_FOREACH (iter, enobj)
          {
             if (enobj->enwin != enwin)
               continue;
             parser_save_object(enobj, fp);
          }
        eina_iterator_free(iter);
     }

   return 0;
}

static int
file_load_do(struct ensure *ensure, const char *file)
{
   struct result *res;
   printf("Loading from %s\n", file);

   res = calloc(1, sizeof(struct result));
   res->title = strdup(file);
   res->tm = 0;
   res->windows = NULL;
   res->objdb = eina_hash_pointer_new(enobj_free);

   results_add(ensure, res);

   parser_readfile(ensure, file);
   return 0;
}

