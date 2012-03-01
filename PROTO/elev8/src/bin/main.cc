/*
 * elev8 - javascript for EFL
 *
 * The script's job is to prepare for the main loop to run
 * then exit
 */

#include <dlfcn.h>
#include <elev8_common.h>
#include <elev8_utils.h>

using namespace v8;
int elev8_log_domain = -1;

#define MODLOAD_ENV "ELEV8_MODPATH"
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

static Handle<Value>
print(const Arguments& args)
{
   HandleScope handle_scope;
   int argument_count;

   switch ((argument_count = args.Length())) {
   case 0:
     goto end;
   case 1:
     fputs(*String::Utf8Value(args[0]), stdout);
     break;
   default:
     for (int i = 0; i < argument_count; i++)
       fputs(*String::Utf8Value(args[i]), stdout);
   }

end:
   putchar('\n');
   fflush(stdout);

   return handle_scope.Close(Undefined());
}

static bool
run_script(const char *filename)
{
   HandleScope handle_scope;

   Handle<String> source = string_from_file(filename);
   if (!source.IsEmpty())
     {
        compile_and_run(source);
        return true;
     }

   ERR("Failed to read source %s", filename);
   return false;
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
   if (try_catch.HasCaught())
     {
        mod_context.Dispose();
        free(file_name);
        return false;
     }

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
   if (module_native_load(module_name, name_space) || module_js_load(module_name, name_space))
     {
        module_cache->Set(module_name, Persistent<Object>::New(name_space));
        return scope.Close(name_space);
     }

   Local<String> msg = String::Concat(String::New("Cannot load module: "), module_name);
   return scope.Close(ThrowException(Exception::Error(msg)));
}

static Handle<Value>
modules(const Arguments&)
{
   HandleScope scope;
   return scope.Close(module_cache);
}

int
main(int argc, char **argv)
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
        printf("Usage:\n"
            "\t%s input_file.js\n"
            "\te.g. %s ../../data/javascript/button.js\n",
             argv[0], argv[0]);
        exit(-1);
     }

   V8::SetFlagsFromCommandLine(&argc, argv, true);

   HandleScope handle_scope;
   Handle<ObjectTemplate> global = ObjectTemplate::New();

   add_symbols_to_context_global(global);

   Persistent<Context> context = Context::New(NULL, global);
   Context::Scope context_scope(context);

   module_cache = Persistent<Object>::New(Object::New());

   if (!run_script(PACKAGE_LIB_DIR "/../init.js"))
     goto end;
   if (!run_script(argv[1]))
     goto end;

   ecore_main_loop_begin();

end:
   context.Dispose();
   module_cache.Dispose();
   V8::Dispose();

   return 0;
}
