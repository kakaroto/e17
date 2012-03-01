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

#define MODLOAD_ENV "ELEv8_MODPATH"
#define MODLOAD_ENV_DEFAULT_DIRS ".:" PACKAGE_LIB_DIR

static Persistent<Object> module_cache;

static Handle<Value> require(const Arguments& args);
static Handle<Value> modules(const Arguments& args);
static Handle<Value> print(const Arguments& args);

static void
add_symbols_to_context_global(Handle<ObjectTemplate> global)
{
   global->Set(String::NewSymbol("require"), FunctionTemplate::New(require));
   global->Set(String::NewSymbol("modules"), FunctionTemplate::New(modules));
   global->Set(String::NewSymbol("print"), FunctionTemplate::New(print));
}

Handle<Value>
print(const Arguments& args)
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

   if (!strncmp(filename, "http://", 7))
     {
        INF("Downloading elev8 Script");
        show_download_ui((void *)filename);
        return;
     }

   /* load the script and run it */
   Handle<String> source = string_from_file(filename);
   if (source.IsEmpty())
     {
        ERR("Failed to read source %s", filename);
        return;
     }
   compile_and_run(source);
}

static char *
find_module_file_name(char *module_name, const char *prefix, const char *type)
{
   char *modpath = getenv(MODLOAD_ENV);
   char default_modpath[] = MODLOAD_ENV_DEFAULT_DIRS;

   if (!modpath) modpath = default_modpath;
    
   for (char *token, *rest, *ptr = modpath;
             (token = strtok_r(ptr, ":", &rest));
             ptr = rest)
      {
         char full_path[PATH_MAX];

         if (snprintf(full_path, PATH_MAX - 1, "%s/%s%s.%s", token, prefix, module_name, type) < 0)
             return NULL;

         if (!access(full_path, R_OK))
             return strdup(full_path);
      }
    
   return NULL;
}

inline static char *
find_native_module_file_name(char *module_name)
{
   return find_module_file_name(module_name, "lib", "so");
}

inline static char *
find_js_module_file_name(char *module_name)
{
   return find_module_file_name(module_name, "", "js");
}

static bool
module_native_load(Handle<String> module_name, Handle<Object> name_space)
{
   char *file_name = find_native_module_file_name(*String::AsciiValue(module_name));

   if (!file_name) return false;

   DBG("Loading native module: %s", file_name);

   // FIXME: Use Eina_Module here.
   void *handle = dlopen(file_name, RTLD_LAZY);
   if (!handle)
     {
        free(file_name);
        return false;
     }

   void (*init_func)(Handle<Object> name_space);
   init_func = (void (*)(Handle<Object>))dlsym(handle, "RegisterModule");
   if (!init_func)
     {
        free(file_name);
        dlclose(handle);
        return false;
     }

   init_func(name_space);

   name_space->Set(String::NewSymbol("__dl_handle"), External::Wrap(handle));
   name_space->Set(String::NewSymbol("__file_name"), String::New(file_name));

   free(file_name);
   return true;
}

static bool
module_js_load(Handle<String> module_name, Handle<Object> name_space)
{
   char *file_name = find_js_module_file_name(*String::AsciiValue(module_name));

   if (!file_name) return false;

   DBG("Loading JavaScript module: %s", file_name);

   Handle<Value> mod_source = string_from_file(file_name);
   if (mod_source->IsUndefined())
     {
        free(file_name);
        return false;
     }

   HandleScope handle_scope;
   Handle<ObjectTemplate> mod_global = ObjectTemplate::New();

   mod_global->Set(String::NewSymbol("exports"), name_space);
   add_symbols_to_context_global(mod_global);

   Persistent<Context> mod_context = Context::New(NULL, mod_global);
   Context::Scope mod_context_scope(mod_context);

   TryCatch try_catch;
   Local<Script> mod_script = Script::Compile(mod_source->ToString());
   if (try_catch.HasCaught())
     {
        mod_context.Dispose();
        free(file_name);
        return false;
     }
    
   mod_script->Run();

   name_space->Set(String::NewSymbol("__file_name"), String::New(file_name));
   // FIXME: How to wrap mod_context so that t can be Disposed() properly?

   free(file_name);
   return true;
}

static Handle<Value>
require(const Arguments& args)
{
   HandleScope scope;
   Local<Object> name_space;
   Local<String> module_name;
    
   if (args.Length() < 1)
     return scope.Close(ThrowException(Exception::Error(String::New("Module name missing"))));
    
   module_name = args[0]->ToString();

   if (module_cache->HasOwnProperty(module_name))
     return scope.Close(module_cache->Get(module_name));

   name_space = Object::New();
   if (!module_native_load(module_name, name_space))
     {
        if (!module_js_load(module_name, name_space))
          {
             Local<String> msg = String::Concat(String::New("Cannot load module: "), module_name);
             return scope.Close(ThrowException(Exception::Error(msg)));
          }
     }

   module_cache->Set(module_name, Persistent<Object>::New(name_space));
   return scope.Close(name_space);
}

static Handle<Value>
modules(const Arguments&)
{
   HandleScope scope;
   return scope.Close(module_cache);
}

void
elev8_run(const char *script)
{
   HandleScope handle_scope;
   Handle<ObjectTemplate> global = ObjectTemplate::New();

   add_symbols_to_context_global(global);

   Persistent<Context> context = Context::New(NULL, global);
   Context::Scope context_scope(context);

   module_cache = Persistent<Object>::New(Object::New());

   run_script(PACKAGE_LIB_DIR "/../init.js");
   run_script(script);

   ecore_main_loop_begin();

   context.Dispose();
   module_cache.Dispose();
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
