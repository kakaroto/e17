#include <unistd.h>
#include <string.h>
#include <utils.h>

using namespace v8;

static int shebang_length(const char *p, int len)
{
   if ((len >= 2) && (p[0] == '#') && (p[1] == '!'))
     return (const char *)memchr(&p[2], '\n', len) - p;
   return 0;
}

Handle<String>
string_from_file(const char *filename)
{
   Handle<String> ret;
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

#define JSERR(...) eina_log_print(elev8_log_domain,\
                             EINA_LOG_LEVEL_ERR, ## __VA_ARGS__)

void
boom(Handle<Message> msg, const char *error)
{
   if (msg.IsEmpty())
     ERR("%s", error);
   else
     {
        String::Utf8Value file(msg->GetScriptResourceName());
        int line = msg->GetLineNumber();
        JSERR(*file, "", line, "%s", error);

        Handle<StackTrace> trace = msg->GetStackTrace();
        if (trace.IsEmpty())
          {
             JSERR(*file, "", line, "   No stack trace available.");
             goto end;
          }

        unsigned frame_count = trace->GetFrameCount();
        if (!frame_count)
          {
             JSERR(*file, "", line, "   Stack trace is empty.");
             goto end;
          }

        JSERR(*file, "", line, "   Stack trace:");
        for (unsigned i = 0; i < frame_count; i++)
          {
             HandleScope scope;
             Local<StackFrame> frame = trace->GetFrame(i);
             Local<String> function_name = frame->GetFunctionName();

             JSERR(*file, "", line, "     %s (ln %d, col %d) %s%s %s%s",
                   *String::Utf8Value(frame->GetScriptName()),
                   frame->GetLineNumber(),
                   frame->GetColumn(),
                   function_name->Length() ? *String::Utf8Value(function_name) : "global context",
                   function_name->Length() ? "()" : "",
                   frame->IsEval() ? "[eval]" : "",
                   frame->IsConstructor() ? "[constructor]" : "");
          }
     }

end:
   exit(1);
}

void
boom(const TryCatch &try_catch)
{
   boom(try_catch.Message(), *String::Utf8Value(try_catch.Exception()));
}

void compile_and_run(Handle<String> source, const char *filename)
{
   TryCatch try_catch;

   /* compile */
   Local<Script> script = Script::Compile(source,
                                          String::New(filename));
   if (try_catch.HasCaught())
     boom(try_catch);

   /* run */
   Local<Value> result = script->Run();
   if (try_catch.HasCaught())
     boom(try_catch);

   if (result->IsObject())
     {
        String::Utf8Value res(result->ToDetailString());
        INF("Result of script run = %s", *res);
     }
}
