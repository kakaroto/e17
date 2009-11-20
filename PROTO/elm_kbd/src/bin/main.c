#include "elm_kbd.h"
#include <Ecore_Str.h>

#ifndef ELM_LIB_QUICKLAUNCH

static void _cb_win_del(void *data, Evas_Object *obj, void *event);
static void _load_keys(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h);

static Evas_Object *win = NULL;

EAPI int 
elm_main(int argc, char **argv) 
{
   Evas_Object *bg;
   int w, h;

   win = elm_win_add(NULL, "elm_kbd", ELM_WIN_BASIC);
   elm_win_title_set(win, "Elm Keyboard");
   evas_object_smart_callback_add(win, "delete-request", _cb_win_del, NULL);
   elm_win_keyboard_mode_set(win, ELM_WIN_KEYBOARD_ALPHA);
   elm_win_keyboard_win_set(win, EINA_TRUE);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   _load_keys(win, &w, &h);

   evas_object_resize(win, w, h);
   evas_object_show(win);

   elm_run();
   elm_shutdown();
   return 0;
}

static void 
_cb_win_del(void *data, Evas_Object *obj, void *event) 
{
   elm_exit();
}

static void 
_load_keys(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h) 
{
   Evas_Object *tbl, *btn;
   Eina_List *files = NULL, *kbs = NULL;
   size_t len;
   char buff[PATH_MAX], *file, *path;
   static int c = 0, r = 0;

   if (!obj) return;

   tbl = elm_table_add(obj);
   elm_win_resize_object_add(obj, tbl);
   evas_object_size_hint_weight_set(tbl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(tbl);

   len = snprintf(buff, sizeof(buff), 
                  "/usr/lib/enlightenment/modules/illume-keyboard/keyboards");
   files = ecore_file_ls(buff);
   buff[len] = '/';
   len++;

   EINA_LIST_FREE(files, file) 
     {
        char *p;

        p = strrchr(file, '.');
        if ((p) && (!strcmp(p, ".kbd"))) 
          {
             if (ecore_strlcpy(buff + len, file, sizeof(buff) - len) >= sizeof(buff) - len)
               continue;
             kbs = eina_list_append(kbs, eina_stringshare_add(buff));
          }
        free(file);
     }

   EINA_LIST_FREE(files, file)
     free(file);

   EINA_LIST_FREE(kbs, path) 
     {
        FILE *f;
        int ok = 0;

        if (!(f = fopen(path, "r"))) continue;
        while (fgets(buff, sizeof(buff), f)) 
          {
             int len;
             char str[PATH_MAX];

             if (!ok) 
               {
                  if (!strcmp(buff, "##KBDCONF-1.0\n")) ok = 1;
               }
             if (!ok) break;
             if (buff[0] == '#') continue;
             len = strlen(buff);
             if (len > 0) 
               {
                  if (buff[len - 1] == '\n') buff[len - 1] = 0;
               }
             if (sscanf(buff, "%4000s", str) != 1) continue;
             if (!strcmp(str, "type")) 
               {
                  sscanf(buff, "%*s %4000s\n", str);
                  if (!strcmp(str, "ALPHA")) 
                    {
                       file = strdup(path);
                       break;
                    }
               }
          }
        fclose(f);
     }

   FILE *f;
   char str[PATH_MAX];

   if (!file) return;
   if (!(f = fopen(file, "r"))) return;
   while (fgets(buff, sizeof(buff), f)) 
     {
        int ww, hh;

        if (buff[0] == '#') continue;
        len = strlen(buff);
        if (len > 0) 
          {
             if (buff[len - 1] == '\n') buff[len - 1] = 0;
          }
        if (sscanf(buff, "%4000s", str) != 1) continue;
        if (!strcmp(str, "kbd")) 
          {
             if (sscanf(buff, "%*s %i %i\n", &ww, &hh) != 2) continue;
             if (w) *w = ww;
             if (h) *h = hh;
          }
        if (!strcmp(str, "normal")) 
          {
             char *p;
             char label[PATH_MAX];
             int xx;

             if (sscanf(buff, "%*s %4000s", label) != 1) continue;
             p = strrchr(label, '.');
             if ((p) && (!strcmp(p, ".png"))) continue;
             if (sscanf(buff, "%*s %4000s", label) != 1) continue;
             btn = elm_button_add(obj);
             elm_button_label_set(btn, label);
             evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, 
                                              EVAS_HINT_EXPAND);
             evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
             elm_table_pack(tbl, btn, c, r, 1, 1);
             evas_object_show(btn);
             c++;
             if (!strcmp(label, "p")) 
               {
                  c = 0;
                  r++;
               }
             else if (!strcmp(label, "l")) 
               {
                  c = 0;
                  r++;
               }
          }
     }
   fclose(f);
}

#endif
ELM_MAIN();
