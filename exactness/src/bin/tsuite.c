#define _GNU_SOURCE 1

#include <stdlib.h>
#include <Elementary.h>
#include "tsuite.h"

#include <dlfcn.h>
#include "tsuite_file_data.h"
#include <Ecore.h>
#include <Ecore_Getopt.h>

void test_3d(void);
void test_actionslider(void);
void test_anchorblock(void);
void test_anchorview(void);
void test_bg_plain(void);
void test_bg_image(void);
void test_bg_options(void);
void test_box_vert(void);
void test_box_vert2(void);
void test_box_horiz(void);
void test_button(void);
void test_bubble(void);
void test_calendar(void);
void test_check(void);
void test_colorselector(void);
void test_conformant(void);
void test_conformant2(void);
void test_ctxpopup(void);
void test_diskselector(void);
void test_entry(void);
void test_entry_scrolled(void);
void test_entry3(void);
void test_entry4(void);
void test_entry5(void);
void test_entry_notepad(void);
void test_fileselector(void);
void test_fileselector_button(void);
void test_fileselector_entry(void);
void test_focus(void);
void test_gengrid(void);
void test_gengrid2(void);
void test_genlist(void);
void test_genlist2(void);
void test_genlist3(void);
void test_genlist4(void);
void test_genlist5(void);
void test_genlist6(void);
void test_genlist7(void);
void test_genlist8(void);
void test_genlist9(void);
void test_genlist10(void);
void test_genlist11(void);
void test_glview(void);
void test_glview_simple(void);
void test_grid(void);
void test_hover(void);
void test_hover2(void);
void test_hoversel(void);
void test_index(void);
void test_index2(void);
void test_inwin(void);
void test_inwin2(void);
void test_label(void);
void test_list(void);
void test_list_horizontal(void);
void test_list2(void);
void test_list3(void);
void test_list4(void);
void test_list5(void);
void test_menu(void);
void test_naviframe(void);
void test_notify(void);
void test_pager(void);
void test_pager_slide(void);
void test_panel(void);
void test_panes(void);
void test_progressbar(void);
void test_radio(void);
void test_scaling(void);
void test_scaling2(void);
void test_scroller(void);
void test_scroller2(void);
void test_segment_control(void);
void test_separator(void);
void test_slider(void);
void test_spinner(void);
void test_table(void);
void test_table2(void);
void test_table3(void);
void test_table4(void);
void test_table5(void);
void test_table6(void);
void test_toggle(void);
void test_toolbar(void);
void test_toolbar2(void);
void test_toolbar3(void);
void test_toolbar4(void);
void test_toolbar5(void);
void test_tooltip(void);
void test_tooltip2(void);

/* START - Some globals - may want to move these to Timer_Data struct later */
static Lists_st *vr_list = NULL;
/* END   - Some globals - may want to move these to Timer_Data struct later */

/**
 * @internal
 *
 * @struct _Test_Item
 * Struct holds test-suite test / func pair.
 *
 * @ingroup Tsuite
 */
struct _Test_Item
{  /* Item type for array of test-names, test-func-ptr */
   char *name;
   void (* func) (void);
   Eina_Bool test;
};
typedef struct _Test_Item Test_Item;

/* static Tsuite_Data ts; moved to tsuite_evas_hook */
static int recording = 0;

void
tsuite_init(Evas_Object *win, char *name, api_data *d)
{
   return;
}

Eina_List *
_add_test(Eina_List *list, char *n, void (* f) (void), Eina_Bool t)
{
   Test_Item *item = malloc(sizeof(Test_Item));
   item->name = n;
   item->func = f;
   item->test = t;
   return eina_list_append(list, item);
}

int test_name_cmp(const void *data1, const void *data2)
{
   return strcmp(((Test_Item *) data1)->name, (char *) data2);
}

Test_Item *
_set_test(Eina_List *list, char *n, Eina_Bool t)
{
   Test_Item *item = eina_list_search_unsorted(list, test_name_cmp, n);
   if (item)
     item->test = t;
   else
     printf("Unknown test name: %s\n", n);

   return item;
}

static void (* _tsuite_evas_hook_init) (Lists_st *) = NULL;
static void (* _tsuite_evas_hook_reset) (void) = NULL;

static void
_init_recording_funcs(void)
{
   _tsuite_evas_hook_init = dlsym(RTLD_DEFAULT, "tsuite_evas_hook_init");
   _tsuite_evas_hook_reset = dlsym(RTLD_DEFAULT, "tsuite_evas_hook_reset");
}
#if 0
/**
 * @internal
 *
 * This function initiates Tsuite_Data
 * @param name defines test-name
 * @param Pointer_Event Pointer to PE.
 *
 * @ingroup Tsuite
 */
void
tsuite_init(Evas_Object *win, char *name, api_data *d)
{
   tsuite_cleanup();
   if (!name)
     return;

   ts.name = strdup(name);  /* Freed by tsuite_cleanup */
   ts.win = win;
   if (d)
     {  /* This field is not used when testing APPs */
        ts.api = d;
        ts.api->win = win;
     }

   ts.e = evas_object_evas_get(ts.win);
}

/**
 * @internal
 *
 * This function do cleanup for Tsuite
 * @param Tsuite_Data * data for cleanup.
 *
 * @ingroup Tsuite
 */
void
tsuite_cleanup(void)
{
   if (ts.name)
     free(ts.name);

   if (ts.api)
     {
        if (ts.api->data && ts.api->free_data)
          free(ts.api->data);

        free(ts.api);
     }

   if (ts.win)
     evas_object_del(ts.win);

   memset(&ts, 0, sizeof(ts));
}
char *tsuite_test_name_get()
{
   return ts.name;
}
#endif
#if 0 /* moved to tsuite_evas_hook */
/**
 * @internal
 *
 * This function takes actual shot and saves it in PNG
 * @param data Tsuite_Data pointer initiated by user
 * @param obj  Window pointer
 * @param obj  name file name. Will use name_+serial if NULL
 *
 * @ingroup Tsuite
 */
void
tsuite_shot_do(Evas_Object *obj, char *name)
{
   Ecore_Evas *ee, *ee_orig;
   Evas_Object *o;
   unsigned int *pixels;
   int w, h,dir_name_len = 0;
   char *filename;
   if (dest_dir)
     dir_name_len = strlen(dest_dir) + 1; /* includes space of a '/' */

   if (name)
     {
        filename = malloc(strlen(name) + strlen(IMAGE_FILENAME_EXT) +
              dir_name_len + 4);

        if (dest_dir)
          sprintf(filename, "%s/", dest_dir);

        sprintf(filename + dir_name_len, "%s%s", name, IMAGE_FILENAME_EXT);
     }
   else
     {
        filename = malloc(strlen(ts.name) + strlen(IMAGE_FILENAME_EXT) +
              dir_name_len + 8); /* also space for serial */

        ts.serial++;
        if (dest_dir)
          sprintf(filename, "%s/", dest_dir);

        sprintf(filename + dir_name_len, "%s_%d%s", ts.name ,ts.serial,
              IMAGE_FILENAME_EXT);
     }

   /* A bit hackish, get the ecore_evas from the Evas canvas */
   ee_orig = evas_data_attach_get(evas_object_evas_get(obj));

   ecore_evas_manual_render(ee_orig);
   pixels = (void *)ecore_evas_buffer_pixels_get(ee_orig);
   if (!pixels) return;
   ecore_evas_geometry_get(ee_orig, NULL, NULL, &w, &h);
   if ((w < 1) || (h < 1)) return;
   ee = ecore_evas_buffer_new(1, 1);
   o = evas_object_image_add(ecore_evas_get(ee));
   evas_object_image_alpha_set(o, ecore_evas_alpha_get(ee_orig));
   evas_object_image_size_set(o, w, h);
   evas_object_image_data_set(o, pixels);

   if (!evas_object_image_save(o, filename, NULL, NULL))
     {
        printf("Cannot save widget to <%s>\n", filename);
     }
   ecore_evas_free(ee);
   free(filename);
}
#endif
void
_test_close_win(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
      void *event_info __UNUSED__)
{
//   ts.win = NULL;
   elm_exit();
}
#if 0  /* Moved to tsuite_file_data */
static Lists_st *
free_events(Lists_st *st)
{
   Variant_st *v;
   EINA_LIST_FREE(st->variant_list, v)
     {
        if (recording)
          {
             Tsuite_Event_Type e = tsuite_event_mapping_type_get(v->t.type);
             if ((e == TSUITE_EVENT_KEY_DOWN) || (e == TSUITE_EVENT_KEY_UP))
               {  /* Allocated in tsuite_evas_hook.c */
                  key_down_key_up *t = v->data;
                  eina_stringshare_del(t->keyname);
                  eina_stringshare_del(t->key);
                  eina_stringshare_del(t->string);
                  eina_stringshare_del(t->compose);
               }
          }

        free(v->data);
        free(v);
     }

   free(st);  /* Allocated when reading data from EET file */
   return NULL;
}
#endif
void
do_test(char *rec_dir, void (* func) (void))
{
   char buf[1024];
   char appname[1024];

//   vr_list = calloc(1, sizeof(*vr_list));
/*   if (recording) done now in ecore_init() hook
     _tsuite_evas_hook_init(vr_list); */

   if (func)
     {  /* Regular test with access func */
        func();
        sprintf(buf, "%s/%s.rec", rec_dir, "ts.name");
     }
   else
     {  /* Application, compose rec-file path and fulll appname */
        sprintf(buf, "%s/%s.rec", rec_dir, "basename(ts.name)");
#if 0
        if (basename(ts.name) == ts.name)
          {  /* No PATH given, use cwd */
             getcwd(appname, 1024);
             strcat(appname, "/");
             strcat(appname, ts.name);
          }
        else
#endif
          strcpy(appname, "ts.name"); /* PATH incuded in ts.name */
     }

   if (recording)
     {
        if (func)
          elm_run(); /* and run the program now  and handle all events etc. */
        else
          {
#ifdef DEBUG_TSUITE
             printf("%s calling <%s>\n", __func__, appname);
#endif
             system(appname);
          }

/*        if (vr_list)
          write_events(buf, vr_list); */
     }
   else
     {
        Timer_Data *td = NULL;
             if (func)
               elm_run(); /* run the program now and handle all events etc */
#if 0
        td = calloc(1, sizeof(Timer_Data));
#ifdef DEBUG_TSUITE
        printf("rec file is <%s>\n", buf);
#endif
        vr_list = read_events(buf, ts.e, td);
        if (td->current_event)
          {  /* Got first event in list, run test */
             tsuite_feed_event(td);
             if (func)
               elm_run(); /* run the program now and handle all events etc */
             else
               {
#ifdef DEBUG_TSUITE
                  printf("%s calling <%s>\n", __func__, appname);
#endif
                  system(appname);
               }
          }

#endif
        if (td)
          free(td);
     }


/*   if (recording) done in ecore_shutdown() hook now
     _tsuite_evas_hook_reset(); */

/*   if (vr_list) will be done in tsuite_evas_hook_reset()
     vr_list = free_events(vr_list); */
}

EAPI int
elm_main(int argc, char **argv)
{
   Eina_List *tests = NULL;
   Eina_Bool test_all;
   char *rec_dir = NULL;
   int i, first_arg;

   int opt_tests = 0;
   Eina_List *opt_destdir = NULL;
   Eina_List *opt_basedir = NULL;

   static const Ecore_Getopt optdesc = {
        "Test_suite",
        NULL,
        "0.0",
        "(C) 2011 Enlightenment",
        "Enlightenment",
        "The Elementary auto test suite",
        0,
        {
           ECORE_GETOPT_COUNT('r', "record", "Recording mode"),
           ECORE_GETOPT_APPEND_METAVAR('d',"destdir","Destir for PNG files",
                 "STRING", ECORE_GETOPT_TYPE_STR),
           ECORE_GETOPT_APPEND_METAVAR('b',"basedir","Directory of rec files",
                 "STRING", ECORE_GETOPT_TYPE_STR),
           ECORE_GETOPT_COUNT('t', "tests", "Tests names marker"),
           ECORE_GETOPT_SENTINEL
        }
   };

   Ecore_Getopt_Value values[] = {
        ECORE_GETOPT_VALUE_INT(recording),
        ECORE_GETOPT_VALUE_LIST(opt_destdir),
        ECORE_GETOPT_VALUE_LIST(opt_basedir),
        ECORE_GETOPT_VALUE_INT(opt_tests),
        ECORE_GETOPT_VALUE_NONE
   };

#ifdef DEBUG_TSUITE
   printf("Got args:\n");
   for(i = 0; i < argc; i++)
     printf("%d=<%s>\n", i, (char *) argv[i]);
#endif
   ecore_init();

   if (ecore_getopt_parse(&optdesc, values, argc, argv) < 0)
     {
        printf("Argument parsing failed\n");
        exit(1);
     }

#ifdef DEBUG_TSUITE
   printf("Values from command line:\n");
   printf("recording=<%d>\nopt_tests=<%d>\n", recording, opt_tests);
   if (opt_destdir)
     printf("opt_destdir=<%s>\n", opt_destdir->data);
   if (opt_basedir)
     printf("opt_basedir=<%s>\n", opt_basedir->data);
#endif

   if (!opt_tests)
     {  /* This in case user runs mannualy and mistakes */
        printf ("Tests Marker (arg) missing.\n");
        exit(1);
     }

   /* Find index of first-test name, for ALL first_arg = (argc-1) */
   for(first_arg = 1; first_arg < argc; first_arg++)
     if (!strcmp(argv[first_arg], "--tests"))
       break;

   first_arg++; /* First arg now is index of first test or == argc */

#ifdef DEBUG_TSUITE
   if (first_arg == argc)
     printf("TEST ALL: first_arg=<%d>\n", first_arg);
   else
     printf("test=<%s> first_arg=<%d>\n", argv[first_arg], first_arg);
#endif

   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   /* tell elm about our app so it can figure out where to get files */
   elm_app_compile_bin_dir_set(PACKAGE_BIN_DIR);
   elm_app_compile_data_dir_set(PACKAGE_DATA_DIR);
   elm_app_info_set(elm_main, "elementary", "images/logo.png");

/*   if (opt_destdir)
     dest_dir = opt_destdir->data;
*/
   if (opt_basedir)
     rec_dir = opt_basedir->data;


   /* if we got here, found test_arg, now find it's index */
   if (recording)
     {
        _init_recording_funcs();
     }

#ifdef DEBUG_TSUITE
   printf("argc=<%d> first_arg=<%d>\n", argc, first_arg);
#endif

   /* If no test specified in command line, set all */
   test_all = (argc - first_arg) == 0;
   tests = _add_test(tests, "test_3d", test_3d, test_all);
   tests = _add_test(tests, "test_actionslider", test_actionslider, test_all);
   tests = _add_test(tests, "test_anchorblock", test_anchorblock, test_all);
   tests = _add_test(tests, "test_anchorview", test_anchorview, test_all);
   tests = _add_test(tests, "test_bg_plain", test_bg_plain, test_all);
   tests = _add_test(tests, "test_bg_image", test_bg_image, test_all);
   tests = _add_test(tests, "test_bg_options", test_bg_options, test_all);
   tests = _add_test(tests, "test_box_vert", test_box_vert, test_all);
   tests = _add_test(tests, "test_box_vert2", test_box_vert2, test_all);
   tests = _add_test(tests, "test_box_horiz", test_box_horiz, test_all);
   tests = _add_test(tests, "test_button", test_button, test_all);
   tests = _add_test(tests, "test_bubble", test_bubble, test_all);
   tests = _add_test(tests, "test_calendar", test_calendar, test_all);
   tests = _add_test(tests, "test_check", test_check, test_all);
   tests = _add_test(tests, "test_colorselector", test_colorselector, test_all);
   tests = _add_test(tests, "test_conformant", test_conformant, test_all);
   tests = _add_test(tests, "test_conformant2", test_conformant2, test_all);
   tests = _add_test(tests, "test_ctxpopup", test_ctxpopup, test_all);
   tests = _add_test(tests, "test_diskselector", test_diskselector, test_all);
   tests = _add_test(tests, "test_entry", test_entry, test_all);
   tests = _add_test(tests, "test_entry_scrolled", test_entry_scrolled,
         test_all);
   tests = _add_test(tests, "test_entry3", test_entry3, test_all);
   tests = _add_test(tests, "test_entry4", test_entry4, test_all);
   tests = _add_test(tests, "test_entry5", test_entry5, test_all);
   tests = _add_test(tests, "test_entry_notepad", test_entry_notepad, test_all);
   tests = _add_test(tests, "test_fileselector", test_fileselector, test_all);
   tests = _add_test(tests, "test_fileselector_button",
         test_fileselector_button, test_all);
   tests = _add_test(tests, "test_fileselector_entry",
         test_fileselector_entry, test_all);
   tests = _add_test(tests, "test_focus", test_focus, test_all);
   tests = _add_test(tests, "test_gengrid", test_gengrid, test_all);
   tests = _add_test(tests, "test_gengrid2", test_gengrid2, test_all);
   tests = _add_test(tests, "test_genlist", test_genlist, test_all);
   tests = _add_test(tests, "test_genlist2", test_genlist2, test_all);
   tests = _add_test(tests, "test_genlist3", test_genlist3, test_all);
   tests = _add_test(tests, "test_genlist4", test_genlist4, test_all);
   tests = _add_test(tests, "test_genlist5", test_genlist5, test_all);
   tests = _add_test(tests, "test_genlist6", test_genlist6, test_all);
   tests = _add_test(tests, "test_genlist7", test_genlist7, test_all);
   tests = _add_test(tests, "test_genlist8", test_genlist8, test_all);
   tests = _add_test(tests, "test_genlist9", test_genlist9, test_all);
   tests = _add_test(tests, "test_genlist10", test_genlist10, test_all);
   tests = _add_test(tests, "test_genlist11", test_genlist11, test_all);
   tests = _add_test(tests, "test_grid", test_grid, test_all);
   tests = _add_test(tests, "test_hover", test_hover, test_all);
   tests = _add_test(tests, "test_hover2", test_hover2, test_all);
   tests = _add_test(tests, "test_hoversel", test_hoversel, test_all);
   tests = _add_test(tests, "test_index", test_index, test_all);
   tests = _add_test(tests, "test_index2", test_index2, test_all);
   tests = _add_test(tests, "test_inwin", test_inwin, test_all);
   tests = _add_test(tests, "test_inwin2", test_inwin2, test_all);
   tests = _add_test(tests, "test_label", test_label, test_all);
   tests = _add_test(tests, "test_list", test_list, test_all);
   tests = _add_test(tests, "test_list_horizontal", test_list_horizontal,
         test_all);

   tests = _add_test(tests, "test_list2", test_list2, test_all);
   tests = _add_test(tests, "test_list3", test_list3, test_all);
   tests = _add_test(tests, "test_list4", test_list4, test_all);
   tests = _add_test(tests, "test_list5", test_list5, test_all);
   tests = _add_test(tests, "test_menu", test_menu, test_all);
   tests = _add_test(tests, "test_naviframe", test_naviframe, test_all);
   tests = _add_test(tests, "test_notify", test_notify, test_all);
   tests = _add_test(tests, "test_pager", test_pager, test_all);
   tests = _add_test(tests, "test_pager_slide", test_pager_slide, test_all);
   tests = _add_test(tests, "test_panel", test_panel, test_all);
   tests = _add_test(tests, "test_panes", test_panes, test_all);
   tests = _add_test(tests, "test_progressbar", test_progressbar, test_all);
   tests = _add_test(tests, "test_radio", test_radio, test_all);
   tests = _add_test(tests, "test_scaling", test_scaling, test_all);
   tests = _add_test(tests, "test_scaling2", test_scaling2, test_all);
   tests = _add_test(tests, "test_scroller", test_scroller, test_all);
   tests = _add_test(tests, "test_scroller2", test_scroller2, test_all);
   tests = _add_test(tests, "test_segment_control", test_segment_control,
         test_all);

   tests = _add_test(tests, "test_separator", test_separator, test_all);
   tests = _add_test(tests, "test_slider", test_slider, test_all);
   tests = _add_test(tests, "test_spinner", test_spinner, test_all);
   tests = _add_test(tests, "test_table", test_table, test_all);
   tests = _add_test(tests, "test_table2", test_table2, test_all);
   tests = _add_test(tests, "test_table3", test_table3, test_all);
   tests = _add_test(tests, "test_table4", test_table4, test_all);
   tests = _add_test(tests, "test_table5", test_table5, test_all);
   tests = _add_test(tests, "test_table6", test_table6, test_all);
   tests = _add_test(tests, "test_toggle", test_toggle, test_all);
   tests = _add_test(tests, "test_toolbar", test_toolbar, test_all);
   tests = _add_test(tests, "test_toolbar2", test_toolbar2, test_all);
   tests = _add_test(tests, "test_toolbar3", test_toolbar3, test_all);
   tests = _add_test(tests, "test_toolbar4", test_toolbar4, test_all);
   tests = _add_test(tests, "test_toolbar5", test_toolbar5, test_all);
   tests = _add_test(tests, "test_tooltip", test_tooltip, test_all);
   tests = _add_test(tests, "test_tooltip2", test_tooltip2, test_all);


   Eina_List *l;
   Test_Item *item;
   int n_tests = 0;
   int n_total = 0;
   int n_no_rec_file = 0;

   /* Set tests from command line */
   for(i = first_arg; i < argc ; i++)
     {
        if (_set_test(tests, argv[i],  EINA_TRUE) == NULL)
          {  /* Failed to add test, trying to add application */
             if (access(argv[i], X_OK) == 0)
               {
                  tests =  _add_test(tests, argv[i], NULL, EINA_TRUE);
               }
             else
               printf("Unknown application: %s\n", argv[i]);
          }

        n_total++; /* Count argument */
     }

   EINA_LIST_FOREACH(tests, l, item)
      if (item->test)
        {  /* Run test and count tests committed */
           char buf[1024];
           if (item->func) /* Regular test with access func */
             sprintf(buf, "%s/%s.rec", rec_dir, item->name);
           else
             {  /* Application, set ts and compose rec-file path */
                tsuite_init(NULL /* win */, item->name, NULL);
                sprintf(buf, "%s/%s.rec", rec_dir, basename(item->name));
             }

           if (!recording)
             {  /* Avoid trying to commit tests with no ".rec" file */
                if (access(buf, R_OK))
                  {
                     printf("Skipped test, missing <%s> file.\n", buf);
                     n_no_rec_file++;
                     continue; /* Skip this, no access to ".rec" file */
                  }
             }

           do_test(rec_dir, item->func);
           n_tests++;
        }

   if (n_no_rec_file)
     printf("\n\nFailed to access %d record files.\n", n_no_rec_file);

   if (n_tests)
     {  /* Print completed message */
        printf("\n\n%d tests completed out of %d tests\n",
              n_tests, n_total);
     }
   else
     {  /* No tests committed, let user know test-name is wrong */
        printf ("\n\nNo test matching:\n");
        for(i = first_arg; i < argc; i++)
          printf("%s\n", argv[i]);

        printf ("\nPlease review test name; check record file.\n");
     }


   /* Free all tests in list */
   Test_Item *data = NULL;
   EINA_LIST_FREE(tests, data)
      free(data);

//   tsuite_cleanup();
   /* if the mainloop that elm_run() runs exist - we exit the app */
   elm_shutdown(); /* clean up and shut down */

   /* exit code, ZERO if all completed successfuly */
   if (test_all)
     return 0;
   else
     return ((argc - first_arg) - n_tests);
}
ELM_MAIN()
