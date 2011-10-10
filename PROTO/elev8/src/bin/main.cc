/*
 * elev8 - javascript for EFL
 *
 * The script's job is to prepare for the main loop to run
 * then exit
 */

#include <Elementary.h>
#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Con.h>
#include <v8.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

using namespace v8;

/* FIXME */
void elm_v8_setup(Handle<ObjectTemplate> global);
int xmlhttp_v8_setup(Handle<ObjectTemplate> global);
void compile_and_run(Handle<String> source);


int shebang_length(const char *p, int len)
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

Local<String>
string_from_file(const char *filename)
{
   Local<String> ret;
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

   ret = String::New(&p[n], len - n);

fail:
   if (p == bad_ret)
     munmap(p, len);

   if (fd >= 0)
     close(fd);

   return ret;
}

Local<String>
string_from_buffer(const char *buffer, int len)
{
   Local<String> ret;
   int n;

   n = shebang_length(buffer, len);
 
   ret = String::New(&buffer[n], len - n);

   return ret;
}

void
boom(TryCatch &try_catch)
{
   Handle<Message> msg = try_catch.Message();
   String::Utf8Value error(try_catch.Exception());

   if (msg.IsEmpty())
     {
        fprintf(stderr, "%s\n", *error);
     }
   else
     {
        String::Utf8Value file(msg->GetScriptResourceName());
        int line = msg->GetLineNumber();

        fprintf(stderr, "%s:%d %s\n", *file, line, *error);
     }
   exit(1);
}

void
run_script(const char *filename)
{
   HandleScope handle_scope;

   if (filename == strstr(filename, "http"))
     {
        fprintf(stderr, "Downloading elev8 Script\n");
     }
   else
     {
        /* load the script and run it */
        Handle<String> origin = String::New(filename);
        Handle<String> source = string_from_file(filename);
        if (source.IsEmpty())
          {
             fprintf(stderr, "Failed to read source %s\n", filename);
             return;
          }
        compile_and_run(source);
      }
}

void compile_and_run(Handle<String> source)
{
   
   TryCatch try_catch;
   /* compile */
   Handle<Script> script = Script::Compile(source);
   if (try_catch.HasCaught())
     boom(try_catch);

   /* run */
   Handle<Value> result = script->Run();
   if (try_catch.HasCaught())
     boom(try_catch);
}

void
elev8_run(const char *script)
{
   HandleScope handle_scope;
   Handle<ObjectTemplate> global = ObjectTemplate::New();

   global->Set(String::New("print"), FunctionTemplate::New(Print));

   elm_v8_setup(global);
   int retval = xmlhttp_v8_setup(global);

   if (retval!=0)
     {
        fprintf(stderr, "Cannot initialize ecore_con_url\n");
	    //FIXME : Disable XMLHttpRequest support
     }

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
   int i;

   if (argc < 2)
     {
        fprintf(stderr, "%s: Error: no input file specified.\n", argv[0]);
        main_help(argv[0]);
        exit(-1);
     }

   for (i = 0; i<argc; i++)
     printf("arg[%d] = %s\n", i, argv[i]);

   V8::SetFlagsFromCommandLine(&argc, argv, true);
   elev8_run(argv[1]);
   V8::Dispose();

   return 0;
}

ELM_MAIN()
