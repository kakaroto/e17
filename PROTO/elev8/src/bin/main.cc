/*
 * elev8 - javascript for EFL
 *
 * The script's job is to prepare for the main loop to run
 * then exit
 */


#include <sys/mman.h>
#include <fcntl.h>
#include <downloader.h>
#include <elev8_utils.h>
#include <elev8_common.h>
#include <module.h>

using namespace v8;
int elev8_log_domain = -1;

/* FIXME */
void elm_v8_setup(Handle<ObjectTemplate> global);
int xmlhttp_v8_setup(Handle<ObjectTemplate> global);
int dbus_v8_setup(Handle<ObjectTemplate> global);

Handle<Value>
Print(const Arguments& args)
{
   for (int i = 0; i < args.Length(); i++)
     {
        HandleScope handle_scope;
        String::Utf8Value str(args[i]);
        printf("%s%s", i ? " ":"", *str);
     }
   printf("\n");
   fflush(stdout);
   return Undefined();
}

void show_download_ui(void *data)
{

   downloader *dl = (downloader *)malloc(sizeof(downloader));;
   if (dl!=NULL)
     {
        memset(dl, 0, sizeof(downloader));
     }

   dl->http_request = (char *)data;

   INF( "Making request to %s", (char *)data);

   dl->win = elm_win_add(NULL, "elev8_viewer", ELM_WIN_BASIC);
   elm_win_title_set(dl->win, "ELEV8 VIEWER");
   evas_object_smart_callback_add(dl->win, "delete,request", ui_kill, dl);

   dl->bg = elm_bg_add(dl->win);
   char buf[PATH_MAX];
   snprintf(buf, PATH_MAX, "%s/data/images/bg.png", PACKAGE_DATA_DIR);
   INF("Path = %s", buf);
   elm_bg_file_set(dl->bg, buf, NULL);
   evas_object_size_hint_align_set(dl->bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(dl->bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(dl->win, dl->bg);
   evas_object_show(dl->bg);

   dl->bx = elm_box_add(dl->win);
   elm_win_resize_object_add(dl->win, dl->bx);
   evas_object_size_hint_align_set(dl->bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(dl->bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(dl->bx);

   dl->title = elm_label_add(dl->win);
   elm_object_text_set(dl->title,"<title><b>ELEV8 VIEWER</b></title>");
   evas_object_size_hint_weight_set(dl->title, 0.0, 0.0);
   evas_object_size_hint_align_set(dl->title, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(dl->bx, dl->title);
   evas_object_show(dl->title);

   dl->pb = elm_progressbar_add(dl->win);
   evas_object_size_hint_weight_set(dl->pb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(dl->pb, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(dl->bx, dl->pb);
   evas_object_show(dl->pb);

   dl->log = elm_label_add(dl->win);
   elm_object_text_set(dl->log,"<title><b>Logs</b></title>");
   evas_object_size_hint_weight_set(dl->log, 0.0, 0.0);
   evas_object_size_hint_align_set(dl->log, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(dl->bx, dl->log);
   evas_object_show(dl->log);

   dl->content = elm_entry_add(dl->win);
   elm_entry_scrollable_set(dl->content, EINA_TRUE);
   evas_object_size_hint_weight_set(dl->content, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(dl->content, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_entry_entry_set(dl->content, "Downloading : ");
   elm_entry_entry_append(dl->content, dl->http_request);
   elm_entry_editable_set(dl->content, EINA_FALSE);
   elm_box_pack_end(dl->bx, dl->content);
   evas_object_show(dl->content);


   dl->bt = elm_button_add(dl->win);
   elm_object_text_set(dl->bt, "Fetch");
   evas_object_size_hint_align_set(dl->bt, EVAS_HINT_FILL, EVAS_HINT_EXPAND);
   evas_object_size_hint_weight_set(dl->bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   //evas_object_smart_callback_add(dl->bt, "clicked", start_download, dl);
   elm_box_pack_end(dl->bx, dl->bt);

   /* hide the button for now
    * In future we may want to enter a URL and download it 
    */
   evas_object_hide(dl->bt);
   start_download(dl,NULL, NULL);

   evas_object_resize(dl->win, 320, 480);
   evas_object_show(dl->win);
}

void
run_script(const char *filename)
{
   HandleScope handle_scope;

   if (filename == strstr(filename, "http"))
     {
        INF("Downloading elev8 Script");
        show_download_ui((void *)filename);
     }
   else
     {
        /* load the script and run it */
        //Handle<String> origin = String::New(filename);
        Handle<String> source = string_from_file(filename);
        if (source.IsEmpty())
          {
             ERR("Failed to read source %s", filename);
             return;
          }
        compile_and_run(source);
      }
}

void
elev8_run(const char *script)
{
   HandleScope handle_scope;
   Handle<ObjectTemplate> global = ObjectTemplate::New();

   global->Set(String::New("print"), FunctionTemplate::New(Print));

   load_modules();
   init_modules(global);

   /* setup V8 */
   Persistent<Context> context = Context::New(NULL, global);
   Context::Scope context_scope(context);
   run_script(script);

   ecore_main_loop_begin();

   context.Dispose();
}

static void
main_help(const char *progname)
{
   printf("Usage:\n"
       "\t%s input_file.js\n"
       "\te.g. %s ../../data/javascript/button.js\n",
	progname, progname);
}

int
elm_main(int argc, char **argv)
{
   eina_init();
   elev8_log_domain = eina_log_domain_register("elev8", EINA_COLOR_ORANGE);
   if (!elev8_log_domain)
     {
        printf("cannot set elev8 log domain\n");
        ERR( "could not register elev8 log domain.");
        elev8_log_domain = EINA_LOG_DOMAIN_GLOBAL;
     }
   INF("elev8 Logging initialized. %d", elev8_log_domain);

   if (argc < 2)
     {
        ERR("%s: Error: no input file specified.", argv[0]);
        main_help(argv[0]);
        exit(-1);
     }


   V8::SetFlagsFromCommandLine(&argc, argv, true);
   elev8_run(argv[1]);
   V8::Dispose();

   return 0;
}

ELM_MAIN()
