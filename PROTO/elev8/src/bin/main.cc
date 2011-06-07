/*
 * elev8 - javascript for EFL
 *
 * The script's job is to prepare for the main loop to run
 * then exit
 */

#include "main.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

Evas_Object *main_win;

void
eo_on_click(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  v8::Persistent<v8::Function> fn(static_cast<v8::Function*>(data));
  v8::Handle<v8::Value> args[] = { v8::String::New("arg") };
  v8::Local<v8::Value> result = fn->Call(fn, 1, args);
}

Eina_Bool
eo_on_animate(void *data)
{
  v8::Persistent<v8::Function> fn(static_cast<v8::Function*>(data));
  v8::Handle<v8::Value> args[] = { v8::String::New("arg") };
  v8::Local<v8::Value> result = fn->Call(fn, 1, args);
}

void
realize_one(v8::Local<v8::Object> obj)
{
   Evas_Object *eo = elm_button_add(main_win);

   v8::Local<v8::Value> label_val = obj->Get(v8::String::New("label"));
   v8::Local<v8::Value> width_val = obj->Get(v8::String::New("width"));
   v8::Local<v8::Value> height_val = obj->Get(v8::String::New("height"));
   v8::Local<v8::Value> x_val = obj->Get(v8::String::New("x"));
   v8::Local<v8::Value> y_val = obj->Get(v8::String::New("y"));
   v8::String::Utf8Value str(label_val);

   evas_object_resize(eo, width_val->ToInteger()->Value(), height_val->ToInteger()->Value());
   evas_object_move(eo, x_val->ToInteger()->Value(), y_val->ToInteger()->Value());

   v8::Local<v8::Value> val;

   val = obj->Get(v8::String::New("on_clicked"));
   if (val->IsFunction())
     {
        v8::Local<v8::Function> local_func = v8::Local<v8::Function>::Cast(val);
        v8::Persistent<v8::Function> func = v8::Persistent<v8::Function>::New(local_func);
        evas_object_event_callback_add(eo, EVAS_CALLBACK_MOUSE_DOWN,
                                       &eo_on_click, static_cast<void*>(*func));
     }

   /* set up animator */
   val = obj->Get(v8::String::New("on_animate"));
   if (val->IsFunction())
     {
        v8::Local<v8::Function> local_func = v8::Local<v8::Function>::Cast(val);
        v8::Persistent<v8::Function> persist_func = v8::Persistent<v8::Function>::New(local_func);
        ecore_animator_add(&eo_on_animate, static_cast<void*>(*persist_func));
     }

   elm_button_label_set(eo, *str);
   evas_object_show(eo);
}

v8::Handle<v8::Value>
Realize(const v8::Arguments& args)
{
   if (args.Length() != 1)
      return v8::ThrowException(v8::String::New("Bad parameters"));
   realize_one(args[0]->ToObject());
   return v8::Undefined();
}

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

void
realize_objects(v8::Handle<v8::Object> elements)
{
   if (elements.IsEmpty())
      return;

   v8::Handle<v8::Array> props = elements->GetPropertyNames();

   /* iterate through elements and instantiate them */
   for (unsigned int i = 0; i < props->Length(); i++)
     {
        v8::Handle<v8::Value> x = props->Get(v8::Integer::New(i));
        v8::String::Utf8Value val(x);

        realize_one(elements->Get(x->ToString())->ToObject());
     }
}

v8::Handle<v8::String>
string_from_file(const char *filename)
{
   v8::Handle<v8::String> ret = v8::Handle<v8::String>();
   int fd, len;
   void *bad_ret = reinterpret_cast<void*>(-1);
   void *p = bad_ret;

   fd = open(filename, O_RDONLY);
   if (fd < 0)
      goto fail;

   len = lseek(fd, 0, SEEK_END);
   if (len <= 0)
      goto fail;

   p = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
   if (p == bad_ret)
      goto fail;

   ret = v8::String::New(reinterpret_cast<char*>(p), len);

fail:
   if (p == bad_ret)
      munmap(p, len);

   if (fd >= 0)
      close(fd);

   return ret;
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

   v8::Handle<v8::Script> script = v8::Script::Compile(source, origin);
   if (script.IsEmpty())
      fprintf(stderr, "compile failed\n");
   v8::Handle<v8::Value> result = script->Run();
}

static void
es_window_delete(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

void
elev8_run(const char *script)
{
   v8::HandleScope handle_scope;
   v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();

   global->Set(v8::String::New("realize"), v8::FunctionTemplate::New(Realize));
   global->Set(v8::String::New("print"), v8::FunctionTemplate::New(Print));

   /* create an empty window first */
   main_win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(main_win, basename(script));
   evas_object_focus_set(main_win, 1);
   evas_object_smart_callback_add(main_win, "delete,request", es_window_delete, NULL);
   evas_object_resize(main_win, 320, 480);
   Evas_Object *bg = elm_bg_add(main_win);
   elm_win_resize_object_add(main_win, bg);
   evas_object_show(bg);

   /* setup V8 */
   v8::Persistent<v8::Context> context = v8::Context::New(NULL, global);
   v8::Context::Scope context_scope(context);
   run_script(script);

   v8::Handle<v8::Object> glob = context->Global();
   realize_objects(glob->Get(v8::String::New("elements"))->ToObject());

   evas_object_show(main_win);
   elm_run();

   context.Dispose();
}

int
elm_main(int argc, char **argv)
{
   v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
   elev8_run(argv[1]);
   v8::V8::Dispose();

   return 0;
}

ELM_MAIN()
