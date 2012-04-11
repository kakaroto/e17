#include <unistd.h>
#include <string.h>
#include <elev8_utils.h>

using namespace v8;

int shebang_length(const char *p, int len)
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

void
boom(TryCatch &try_catch)
{
   Handle<Message> msg = try_catch.Message();
   String::Utf8Value error(try_catch.Exception());

   if (msg.IsEmpty())
     ERR("%s", *error);
   else
     {
        String::Utf8Value file(msg->GetScriptResourceName());
        int line = msg->GetLineNumber();
        ERR("%s:%d %s", *file, line, *error);

        Handle<StackTrace> trace = msg->GetStackTrace();
        if (trace.IsEmpty())
          {
             ERR("   No stack trace available.");
             goto end;
          }

        unsigned frame_count = trace->GetFrameCount();
        if (!frame_count)
          {
             ERR("   Stack trace is empty.");
             goto end;
          }

        ERR("Stack trace:");
        for (unsigned i = 0; i < frame_count; i++)
          {
             Local<StackFrame> frame = trace->GetFrame(i);
             ERR("   %s:%d,%d: %s",
                *String::AsciiValue(frame->GetScriptName()),
                frame->GetLineNumber(),
                frame->GetColumn(),
                *String::AsciiValue(frame->GetFunctionName()));
          }
     }

end:
   exit(1);
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
