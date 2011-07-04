#include <Elementary.h>
#include <unistd.h>

static void print_usage(const char *bin);

static void
my_win_del(void *data, Evas_Object *obj, void *event_info)
{
   (void) data;
   (void) obj;
   (void) event_info;
   /* called when my_win_main is requested to be deleted */
   elm_exit(); /* exit the program's main loop that runs in elm_run() */
}

static char *
_buf_append(char *buf, const char *str, int *len, int *alloc)
{
   int len2 = strlen(str);
   if ((*len + len2) >= *alloc)
     {
        char *buf2 = realloc(buf, *alloc + len2 + 512);
        if (!buf2) return NULL;
        buf = buf2;
        *alloc += (512 + len2);
     }
   strcpy(buf + *len, str);
   *len += len2;
   return buf;
}

static char *
_load_file(const char *file)
{
   FILE *f;
   size_t size;
   int alloc = 0, len = 0;
   char *text = NULL, buf[16384 + 1];

   f = fopen(file, "rb");
   if (!f) return NULL;
   while ((size = fread(buf, 1, sizeof(buf) - 1, f)))
     {
        char *tmp_text;
        buf[size] = 0;
        tmp_text = _buf_append(text, buf, &len, &alloc);
        if (!tmp_text) break;
        text = tmp_text;
     }
   fclose(f);
   return text;
}

static char *
_load_plain(const char *file)
{
   char *text;

   text = _load_file(file);
   if (text)
     {
        char *text2;

        text2 = elm_entry_utf8_to_markup(text);
        free(text);
        return text2;
     }
   return NULL;
}

int
main(int argc, char *argv[])
{
   Evas_Object *win, *bg, *tb = NULL;
   Evas_Textblock_Style *st;
   Evas_Coord w = 600, h = 600;
   const char *filename = "note.txt";
   int c;
   const char *type = "evas"; /* default */
   /* Wrap mode */
   Elm_Wrap_Type wrap = ELM_WRAP_WORD;
   const char *wrapt = "word";
   /* end of wrap mode */
   Eina_Unicode plain_utf8 = EINA_FALSE;

   opterr = 0;

   while ((c = getopt (argc, argv, "w:h:f:l:r:p")) != -1)
     {
        switch (c)
          {
           case 'w':
              w = atoi(optarg);
              break;
           case 'h':
              h = atoi(optarg);
              break;
           case 'f':
              filename = optarg;
              break;
           case 'l':
              if (strcmp(optarg, "elm") &&
                    strcmp(optarg, "edje") &&
                    strcmp(optarg, "evas"))
                {
                   print_usage(argv[0]);
                   return 1;
                }
              type = optarg;
              break;
           case 'r':
              wrapt = optarg;
              if (!strcmp(optarg, "none"))
                 wrap = ELM_WRAP_NONE;
              else if (!strcmp(optarg, "char"))
                 wrap = ELM_WRAP_CHAR;
              else if (!strcmp(optarg, "word"))
                 wrap = ELM_WRAP_WORD;
              else if (!strcmp(optarg, "mixed"))
                 wrap = ELM_WRAP_MIXED;
              else
                {
                   print_usage(argv[0]);
                   return 1;
                }
              break;
           case 'p':
              plain_utf8 = EINA_TRUE;
              break;
           case '?':
              print_usage(argv[0]);
              if (isprint (optopt))
                {
                   fprintf (stderr, "Unknown option or requires an argument `-%c'.\n", optopt);
                }
              else
                {
                   fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
                }
              return 1;
              break;
           default:
              abort();
          }
     }

   printf("Running using: '%s' with filename: '%s'\n"
         "size: %dx%d\n"
         "wrap mode: '%s'\n"
         "plain utf8: %d\n"
         "========================================================\n\n",
         type, filename, w, h, wrapt, plain_utf8);

   elm_init(argc, argv);

   win = elm_win_add(NULL, "textblock-test", ELM_WIN_BASIC);
   elm_win_title_set(win, "Textblock test");
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_smart_callback_add(win, "delete,request", my_win_del, NULL);

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   /* Interesting */


   if (!strcmp(type, "evas"))
     {
        char buf[2000];
        tb = evas_object_textblock_add(evas_object_evas_get(win));

        snprintf(buf,
              2000,
              "DEFAULT='font=Sans font_size=10 color=#000 wrap=%s text_class=entry'"
              "br='\n'"
              "ps='ps'"
              "tab='\t'",
              wrapt);

        st = evas_textblock_style_new();
        evas_textblock_style_set(st, buf);
        evas_object_textblock_style_set(tb, st);
     }
   else if (!strcmp(type, "edje"))
     {
        tb = edje_object_add(evas_object_evas_get(win));
        edje_object_file_set(tb, "tb.edj", wrapt);
        edje_object_signal_emit(tb, "elm,action,focus", "elm");
     }
   else if (!strcmp(type, "elm"))
     {
        tb = elm_entry_add(win);
        elm_entry_scrollable_set(tb, EINA_TRUE);
        elm_entry_line_wrap_set(tb, wrap);
     }

   evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, tb);

   evas_object_resize(win, w, h);
   evas_object_resize(tb, w, h);

   evas_object_show(tb);
   evas_object_show(win);

   /* Set the text */
     {
        char *buf;

        if (plain_utf8)
           buf = _load_plain(filename);
        else
           buf = _load_file(filename);

        if (!strcmp(type, "evas"))
          {
             evas_object_textblock_text_markup_set (tb, buf);
          }
        else if (!strcmp(type, "edje"))
          {
             edje_object_part_text_set(tb, "elm.text", buf);
             evas_object_focus_set(tb, EINA_TRUE);
          }
        else if (!strcmp(type, "elm"))
          {
             elm_entry_entry_set(tb, buf);
             elm_object_focus(win);
          }
        free(buf);
     }

   elm_run();
   return 0;
}

static void
print_usage(const char *bin)
{
   fprintf(stderr,
         "Usage: %s [-p] [-w width] [-h height] [-f filename] [-l lib] [-r wrap]\n"
         "-w,-h = size of window in pixels.\n"
         "-f = filename to use\n"
         "-l = lib to use: evas, edje, or elm.\n"
         "-r = wrap type: none, char, word, or mixed.\n"
         "-p = load as plain utf8 instead of markup.\n", bin);
}
