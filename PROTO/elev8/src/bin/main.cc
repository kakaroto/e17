/*
 * elev8 - javascript for EFL
 *
 * The script's job is to prepare for the main loop to run
 * then exit
 */

#include <Elementary.h>
#include <v8.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

/* FIXME */
void elm_v8_setup(v8::Handle<v8::ObjectTemplate> global);

v8::Handle<v8::Value>
Print(const v8::Arguments& args)
{
   for (int i = 0; i < args.Length(); i++)
     {
        v8::HandleScope handle_scope;
        v8::String::Utf8Value str(args[i]);
        printf("%s%s", i ? " ":"", *str);
     }
   printf("\n");
   fflush(stdout);
   return v8::Undefined();
}

int shebang_length(char *p, int len)
{
   int i = 0;

   if ((len > 2) && (p[0] == '#') && (p[1] == '!'))
     {
        for (i = 2; i < len && p[i] != '\n'; i++)
          ;
        /* leave first newline in so line numbers are correct */
     }

   return i;
}

v8::Local<v8::String>
string_from_file(const char *filename)
{
   v8::Local<v8::String> ret;
   int fd, len = 0;
   char *bad_ret = reinterpret_cast<char*>(MAP_FAILED);
   char *p = bad_ret;
   int n;

   fd = open(filename, O_RDONLY);
   if (fd < 0)
     goto fail;

   len = lseek(fd, 0, SEEK_END);
   if (len <= 0)
     goto fail;

   p = reinterpret_cast<char*>(mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0));
   if (p == bad_ret)
     goto fail;

   n = shebang_length(p, len);

   ret = v8::String::New(&p[n], len - n);

fail:
   if (p == bad_ret)
     munmap(p, len);

   if (fd >= 0)
     close(fd);

   return ret;
}

void
boom(v8::TryCatch &try_catch)
{
   v8::Handle<v8::Message> msg = try_catch.Message();
   v8::String::Utf8Value error(try_catch.Exception());

   if (msg.IsEmpty())
     {
        fprintf(stderr, "%s\n", *error);
     }
   else
     {
        v8::String::Utf8Value file(msg->GetScriptResourceName());
        int line = msg->GetLineNumber();

        fprintf(stderr, "%s:%d %s\n", *file, line, *error);
     }
   exit(1);
}

void
run_script(const char *filename)
{
   v8::HandleScope handle_scope;
   v8::TryCatch try_catch;

   /* load the script and run it */
   v8::Handle<v8::String> origin = v8::String::New(filename);
   v8::Handle<v8::String> source = string_from_file(filename);
   if (source.IsEmpty())
     {
        fprintf(stderr, "Failed to read source %s\n", filename);
        return;
     }

   /* compile */
   v8::Handle<v8::Script> script = v8::Script::Compile(source, origin);
   if (try_catch.HasCaught())
     boom(try_catch);

   /* run */
   v8::Handle<v8::Value> result = script->Run();
   if (try_catch.HasCaught())
     boom(try_catch);
}

void
elev8_run(const char *script)
{
   v8::HandleScope handle_scope;
   v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();

   global->Set(v8::String::New("print"), v8::FunctionTemplate::New(Print));

   elm_v8_setup(global);

   /* setup V8 */
   v8::Persistent<v8::Context> context = v8::Context::New(NULL, global);
   v8::Context::Scope context_scope(context);
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
   if (argc < 2)
     {
        fprintf(stderr, "%s: Error: no input file specified.\n", argv[0]);
        main_help(argv[0]);
        exit(-1);
     }
   v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
   elev8_run(argv[1]);
   v8::V8::Dispose();

   return 0;
}

ELM_MAIN()
