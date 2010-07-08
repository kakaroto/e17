#include <Elementary.h>
#ifndef ELM_LIB_QUICKLAUNCH
static void
on_win_del_req(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

typedef struct _Slide Slide;

struct _Slide
{
   const char *title;
   const char *text;
};

static Evas_Object *win = NULL;
static Evas_Object *pg = NULL;
static Evas_Object *pge = NULL;
static Evas_Object *ten = NULL;
static Evas_Object *ben = NULL;

static Evas_Object *content = NULL;
static Evas_Object *layout = NULL;
static Eina_List *slide_queue = NULL;
static int showing = 0;
static int hiding = 0;

static void show_done(void *data, Evas_Object *obj, const char *emission, const char *source);
static void slide_do_show(Slide *s);
static void hide_done(void *data, Evas_Object *obj, const char *emission, const char *source);
static void slide_do_hide(void);

static void slide_show(Slide *s);

static void add_slide(void *data, Evas_Object *obj, void *event_info);
static void del_slide(void *data, Evas_Object *obj, void *event_info);
static void edit_slide(void *data, Evas_Object *obj, void *event_info);
static void edit_toggle(void);

static void
slide_content(Slide *s)
{
   Evas_Object *o = elm_layout_edje_get(layout);
   edje_object_part_text_set(o, "title", s->title);
   if (content)
     {
        evas_object_del(content);
        content = NULL;
     }
   if (!strncmp(s->text, "<img src=", 9))
     {
        char *t = strdup(s->text + 9);
        if (t)
          {
             char *p = strrchr(t, '>');
             if (p) *p = 0;
             content = elm_icon_add(win);
             edje_object_part_text_set(o, "text", "");
             elm_icon_prescale_set(content, 0);
             elm_icon_file_set(content, t, NULL);
             elm_icon_scale_set(content, 1, 1);
             elm_icon_no_scale_set(content, 1);
             elm_layout_content_set(layout, "text-swallow", content);
             evas_object_show(content);
             free(t);
          }
     }
   else
     edje_object_part_text_set(o, "text", s->text);
}

static void
show_done(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   showing--;
   if (slide_queue) slide_do_hide();
}
static void
slide_do_show(Slide *s)
{
   Evas_Object *o = elm_layout_edje_get(layout);
   slide_content(s);
   edje_object_signal_emit(o, "show", "do");
   showing++;
}

static void
hide_done(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   hiding--;
   if (hiding == 0)
     {
        if (slide_queue)
          {
             slide_do_show(slide_queue->data);
             slide_queue = eina_list_remove_list(slide_queue, slide_queue);
          }
     }
}
static void
slide_do_hide(void)
{
   Evas_Object *o = elm_layout_edje_get(layout);
   edje_object_signal_emit(o, "hide", "do");
   hiding++;
}

static void
slide_show(Slide *s)
{
   slide_queue = eina_list_append(slide_queue, s);
   if ((hiding == 0) && (showing == 0)) slide_do_hide();
}

static char *slidefile = NULL;
static Eina_List *slides = NULL;
int slide_num = -1;

static void
save_slides(void)
{
   Eina_List *l;
   Slide *s;
   FILE *f;
   if (!slidefile) return;
   f = fopen(slidefile, "wb");
   if (!f) return;
   EINA_LIST_FOREACH(slides, l, s)
     {
        fprintf(f, "title: %s\n", s->title);
        fprintf(f, "b: %s\n", s->text);
        fprintf(f, "\n");
     }
   fclose(f);
}
static void
load_slides(void)
{
   FILE *f;
   char buf[16384];
   Slide *s = NULL;
   if (!slidefile) return;
   f = fopen(slidefile, "rb");
   if (!f) return;
   while (fgets(buf, sizeof(buf), f))
     {
        int len = strlen(buf);
        if (len <= 1) continue;
        buf[len - 1] = 0;
        if (!strncmp(buf, "title:", 6))
          {
             if (!s) s = calloc(1, sizeof(Slide));
             else
               {
                  slides = eina_list_append(slides, s);
                  s = calloc(1, sizeof(Slide));
               }
             s->title = eina_stringshare_add(buf + 7);
          }
        else if (!strncmp(buf, "b:", 2))
          {
             if (!s) s = calloc(1, sizeof(Slide));
             if (s->text)
               {
                  char *t = malloc(strlen(s->text) + 1 + strlen(buf + 4) + 1);
                  if (t)
                    {
                       strcpy(t, s->text);
                       strcat(t, buf + 3);
                       eina_stringshare_del(s->text);
                       s->text = eina_stringshare_add(t);
                       free(t);
                    }
               }
             else
               s->text = eina_stringshare_add(buf + 3);
          }
     }
   if (s)
     {
        if (!s->title) free(s);
        else slides = eina_list_append(slides, s);
     }
   fclose(f);
}

static void
add_slide(void *data, Evas_Object *obj, void *event_info)
{
   Slide *s, *srel;
   
   s = calloc(1, sizeof(Slide));
   srel = eina_list_nth(slides, slide_num);
   slides = eina_list_append_relative(slides, s, srel);
   slide_num++;
   s->title = eina_stringshare_add("Edit title");
   s->text = eina_stringshare_add("Edit body");
   slide_show(s);
   edit_slide(data, obj, event_info);
}
static void
del_slide(void *data, Evas_Object *obj, void *event_info)
{
   Slide *s, *sdel;
   
   sdel = eina_list_nth(slides, slide_num);
   if (!sdel) return;
   slides = eina_list_remove(slides, sdel);
   eina_stringshare_del(sdel->title);
   eina_stringshare_del(sdel->text);
   free(sdel);
   s = eina_list_nth(slides, slide_num);
   if (!s)
     {
        slide_num--;
        s = eina_list_nth(slides, slide_num);
     }
   slide_show(s);
   edit_toggle();
}
static void
edit_done(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *o;
   Slide *s;
   char *t;
   
   if (!ten) return;
   s = eina_list_nth(slides, slide_num);
   eina_stringshare_del(s->title);
   t = elm_entry_markup_to_utf8(elm_entry_entry_get(ten));
   s->title = eina_stringshare_add(t);
   free(t);
   t = elm_entry_markup_to_utf8(elm_entry_entry_get(ben));
   s->text = eina_stringshare_add(t);
   free(t);
   slide_content(s);
   ten = NULL;
   ben = NULL;
   elm_pager_content_pop(pge);
}
static void
edit_slide(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *sc, *en, *bx, *bt;
   Slide *s;
   char *t;
   
   edit_toggle();
   
   s = eina_list_nth(slides, slide_num);
   if (!s) return;

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, 1.0, 0.0);
   evas_object_size_hint_align_set(bx, -1.0, 0.5);
   evas_object_show(bx);
   
   sc = elm_scroller_add(win);
   elm_scroller_content_min_limit(sc, 0, 1);
   evas_object_size_hint_weight_set(sc, 1.0, 0.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);

   en = elm_entry_add(win);
   ten = en;
   elm_entry_single_line_set(en, 1);
   t = elm_entry_utf8_to_markup(s->title);
   elm_entry_entry_set(en, t);
   free(t);
   evas_object_size_hint_weight_set(en, 1.0, 0.0);
   evas_object_size_hint_align_set(en, -1.0, 0.0);
   elm_entry_select_all(en);
   elm_scroller_content_set(sc, en);
   evas_object_show(en);
   elm_box_pack_end(bx, sc);
   evas_object_show(sc);

   sc = elm_scroller_add(win);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   
   en = elm_entry_add(win);
   ben = en;
   t = elm_entry_utf8_to_markup(s->text);
   elm_entry_entry_set(en, t);
   free(t);
   evas_object_size_hint_weight_set(en, 1.0, 1.0);
   evas_object_size_hint_align_set(en, -1.0, -1.0);
   elm_scroller_content_set(sc, en);
   evas_object_show(en);
   elm_box_pack_end(bx, sc);
   evas_object_show(sc);
   
   bt = elm_button_add(win);
   evas_object_size_hint_weight_set(bt, 0.0, 0.0);
   evas_object_size_hint_align_set(bt, 0.5, 0.5);
   elm_button_label_set(bt, "Done");
   evas_object_smart_callback_add(bt, "clicked", edit_done, NULL);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   
   elm_pager_content_push(pge, bx);
}
static void
save_slide(void *data, Evas_Object *obj, void *event_info)
{
   edit_toggle();
   save_slides();
}
static void
load_slide(void *data, Evas_Object *obj, void *event_info)
{
   edit_toggle();
   // FIXME: select file
//   load_slides();
}
static Ecore_Timer *scale_timeout = NULL;
static Eina_Bool
scale_timeout_func(void *data)
{
   elm_scale_set(elm_slider_value_get(data));
   scale_timeout = NULL;
   return NULL;
}
static Ecore_Timer *finger_size_timeout = NULL;
static Eina_Bool
finger_size_timeout_func(void *data)
{
   elm_finger_size_set(elm_slider_value_get(data));
   finger_size_timeout = NULL;
   return NULL;
}
static void
scale_change(void *data, Evas_Object *obj, void *event_info)
{
   if (scale_timeout) ecore_timer_del(scale_timeout);
   scale_timeout = ecore_timer_add(0.1, scale_timeout_func, obj);
}
static void
finger_change(void *data, Evas_Object *obj, void *event_info)
{
   if (finger_size_timeout) ecore_timer_del(finger_size_timeout);
   finger_size_timeout = ecore_timer_add(0.1, finger_size_timeout_func, obj);
}
static void
settings_done(void *data, Evas_Object *obj, void *event_info)
{
   if (scale_timeout) ecore_timer_del(scale_timeout);
   scale_timeout = NULL;
   if (finger_size_timeout) ecore_timer_del(finger_size_timeout);
   finger_size_timeout = NULL;
   elm_pager_content_pop(pg);
}
static void
settings(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *bx, *sl, *bt;
   
   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, 1.0, 0.0);
   evas_object_size_hint_align_set(bx, -1.0, 0.5);
   evas_object_show(bx);
   
   sl = elm_slider_add(win);
   elm_slider_label_set(sl, "Scale");
   elm_slider_unit_format_set(sl, "Times");
   elm_slider_indicator_format_set(sl, "%1.1f");
   elm_slider_min_max_set(sl, 0.5, 5.0);
   elm_slider_value_set(sl, elm_scale_get());
   elm_slider_span_size_set(sl, 160);
   elm_box_pack_end(bx, sl);
   evas_object_smart_callback_add(bt, "changed", scale_change, NULL);
   evas_object_show(sl);
   
   sl = elm_slider_add(win);
   elm_slider_label_set(sl, "Finger");
   elm_slider_unit_format_set(sl, "Pixels");
   elm_slider_indicator_format_set(sl, "%1.0f");
   elm_slider_min_max_set(sl, 0, 200);
   elm_slider_value_set(sl, elm_finger_size_get());
   elm_slider_span_size_set(sl, 160);
   elm_box_pack_end(bx, sl);
   evas_object_smart_callback_add(bt, "changed", finger_change, NULL);
   evas_object_show(sl);
   
   bt = elm_button_add(win);
   evas_object_size_hint_weight_set(bt, 0.0, 0.0);
   evas_object_size_hint_align_set(bt, 0.5, 0.5);
   elm_button_label_set(bt, "Done");
   evas_object_smart_callback_add(bt, "clicked", settings_done, NULL);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   
   elm_pager_content_push(pg, bx);
}

static void
edit_toggle(void)
{
   static Evas_Object *bx = NULL, *bx2;
   Evas_Object *bt;
   
   if (bx)
     {
        elm_pager_content_pop(pg);
        bx = NULL;
        return;
     }
   bx = elm_box_add(win);
   elm_box_homogenous_set(bx, 1);
   evas_object_size_hint_weight_set(bx, 0.0, 0.0);
   evas_object_size_hint_align_set(bx, 0.5, 0.5);
   evas_object_show(bx);
   
   bx2 = elm_box_add(win);
   elm_box_homogenous_set(bx2, 1);
   elm_box_horizontal_set(bx2, 1);
   evas_object_size_hint_weight_set(bx2, 1.0, 1.0);
   evas_object_size_hint_align_set(bx2, -1.0, -1.0);
   elm_box_pack_end(bx, bx2);
   evas_object_show(bx2);
   
   bt = elm_button_add(win);
   evas_object_size_hint_weight_set(bt, 1.0, 1.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_button_label_set(bt, "Add");
   evas_object_smart_callback_add(bt, "clicked", add_slide, NULL);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);
   
   bt = elm_button_add(win);
   evas_object_size_hint_weight_set(bt, 1.0, 1.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_button_label_set(bt, "Delete");
   evas_object_smart_callback_add(bt, "clicked", del_slide, NULL);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);
   
   bt = elm_button_add(win);
   evas_object_size_hint_weight_set(bt, 1.0, 1.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_button_label_set(bt, "Edit");
   evas_object_smart_callback_add(bt, "clicked", edit_slide, NULL);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);
   
   bx2 = elm_box_add(win);
   elm_box_homogenous_set(bx2, 1);
   elm_box_horizontal_set(bx2, 1);
   evas_object_size_hint_weight_set(bx2, 1.0, 1.0);
   evas_object_size_hint_align_set(bx2, -1.0, -1.0);
   elm_box_pack_end(bx, bx2);
   evas_object_show(bx2);
   
   bt = elm_button_add(win);
   evas_object_size_hint_weight_set(bt, 1.0, 1.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_button_label_set(bt, "Save");
   evas_object_smart_callback_add(bt, "clicked", save_slide, NULL);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);
   
   bt = elm_button_add(win);
   evas_object_size_hint_weight_set(bt, 1.0, 1.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_button_label_set(bt, "Load");
   evas_object_smart_callback_add(bt, "clicked", load_slide, NULL);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);
    
   bx2 = elm_box_add(win);
   elm_box_homogenous_set(bx2, 1);
   elm_box_horizontal_set(bx2, 1);
   evas_object_size_hint_weight_set(bx2, 1.0, 1.0);
   evas_object_size_hint_align_set(bx2, -1.0, -1.0);
   elm_box_pack_end(bx, bx2);
   evas_object_show(bx2);
   
   bt = elm_button_add(win);
   evas_object_size_hint_weight_set(bt, 1.0, 1.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_button_label_set(bt, "Settings");
   evas_object_smart_callback_add(bt, "clicked", settings, NULL);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);
   
   elm_pager_content_push(pg, bx);
}

static void
key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;
   if ((!strcmp(ev->key, "space")) ||
       (!strcmp(ev->key, "Return")) ||
       (!strcmp(ev->key, "Right")) ||
       (!strcmp(ev->key, "Down")) ||
       (!strcmp(ev->key, "Next")))
     {
        Slide *s;
        if (!ten)
          {
             slide_num++;
             s = eina_list_nth(slides, slide_num);
             if (!s)
               {
                  slide_num = 0;
                  s = eina_list_nth(slides, slide_num);
               }
             if (s) slide_show(s);
          }
     }
   else if ((!strcmp(ev->key, "BackSpace")) ||
            (!strcmp(ev->key, "Left")) ||
            (!strcmp(ev->key, "Up")) ||
            (!strcmp(ev->key, "Prior")) ||
            (!strcmp(ev->key, "Delete")))
     {
        Slide *s;
        if (!ten)
          {
             slide_num--;
             s = eina_list_nth(slides, slide_num);
             if (!s)
               {
                  slide_num = eina_list_count(slides) - 1;
                  s = eina_list_nth(slides, slide_num);
               }
             if (s) slide_show(s);
          }
     }
   else if ((!strcmp(ev->key, "Escape")))
     {
        if (!ten)
          {
             printf("stop fs\n");
          }
     }
   else if ((!strcmp(ev->key, "f")))
     {
        if (!ten)
          {
             printf("fs\n");
          }
     }
   else if ((!strcmp(ev->key, "Insert")))
     {
        if (!ten) edit_toggle();
     }
}

static void
create_main_win(void)
{
   Evas_Object *bg, *ly, *en, *bx, *ic;
   char buf[PATH_MAX];
   
   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(win, "Elementary Presenter");
   evas_object_smart_callback_add(win, "delete-request", on_win_del_req, NULL);
   
   evas_object_key_grab(win, "space", 0, 0, 0);
   evas_object_key_grab(win, "Return", 0, 0, 0);
   evas_object_key_grab(win, "Right", 0, 0, 0);
   evas_object_key_grab(win, "Down", 0, 0, 0);
   evas_object_key_grab(win, "Next", 0, 0, 0);

   evas_object_key_grab(win, "BackSpace", 0, 0, 0);
   evas_object_key_grab(win, "Left", 0, 0, 0);
   evas_object_key_grab(win, "Up", 0, 0, 0);
   evas_object_key_grab(win, "Prior", 0, 0, 0);
   evas_object_key_grab(win, "Delete", 0, 0, 0);
   
   evas_object_key_grab(win, "Escape", 0, 0, 0);
   
   evas_object_key_grab(win, "f", 0, 0, 0);

   evas_object_key_grab(win, "Insert", 0, 0, 0);
   
   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN, key_down, NULL);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0); // expand h/v 1/1 (for win this also fills)
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);
   
   ly = elm_layout_add(win);
   layout = ly;
   snprintf(buf, sizeof(buf), "%s/objects/pres.edj", PACKAGE_DATA_DIR);
   elm_layout_file_set(ly, buf, "layout");
   evas_object_size_hint_weight_set(ly, 1.0, 1.0);
   elm_win_resize_object_add(win, ly);
   evas_object_show(ly);
   edje_object_signal_callback_add(elm_layout_edje_get(ly), "hide", "done", hide_done, NULL);
   edje_object_signal_callback_add(elm_layout_edje_get(ly), "show", "done", show_done, NULL);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, 1.0, 1.0);
   elm_win_resize_object_add(win, bx);
   
   pg = elm_pager_add(win);
   evas_object_size_hint_weight_set(pg, 0.0, 0.0);
   evas_object_size_hint_align_set(pg, 0.5, 0.5);
   elm_box_pack_end(bx, pg);
   evas_object_show(pg);
   
   evas_object_show(bx);

   pge = elm_pager_add(win);
   elm_win_resize_object_add(win, pge);
   evas_object_size_hint_weight_set(pge, 1.0, 1.0);
   evas_object_size_hint_align_set(pge, 0.5, 0.5);
   elm_win_resize_object_add(win, pge);
   evas_object_show(pge);
   
   evas_object_resize(win, 800, 600);
   evas_object_show(win);
}

EAPI int
elm_main(int argc, char **argv)
{
   if (argc > 1)
     {
        slidefile = eina_stringshare_add(argv[1]);
        load_slides();
     }
   create_main_win();
   if (slides)
     {
        Slide *s;
        slide_num = 0;
        s = eina_list_nth(slides, slide_num);
        slide_show(s);
     }
   elm_run();
   elm_shutdown();
   return 0; 
}
#endif
ELM_MAIN()
