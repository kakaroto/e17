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

/* forward declaration of function used recursively */
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
  return ECORE_CALLBACK_RENEW;
}

struct CEvasObject;
void realize_one(CEvasObject *parent, v8::Local<v8::Object> obj);

class CEvasObject {
   /* realize_one is a factory for our class */
   friend void realize_one(CEvasObject *parent, v8::Local<v8::Object> obj);
protected:
   v8::Persistent<v8::Object> obj;
   Evas_Object *eo;
protected:
   CEvasObject() :
       eo(NULL)
     {
     }
   CEvasObject(v8::Local<v8::Object> temp_obj) :
       obj(temp_obj),
       eo(NULL)
     {
     }

   /*
    * Two phase constructor required because Evas_Object type needs
    * to be known to be created.
    */
   void construct(Evas_Object *new_eo)
     {
       eo = new_eo;
       assert(eo != NULL);
       resize(obj->Get(v8::String::New("width")),
              obj->Get(v8::String::New("height")));
       move(obj->Get(v8::String::New("x")),
            obj->Get(v8::String::New("y")));
       callback_set(obj->Get(v8::String::New("on_clicked")));
       animator_set(obj->Get(v8::String::New("on_animate")));
       label_set(obj->Get(v8::String::New("label")));
       image_set(obj->Get(v8::String::New("image")));
     }
   virtual void add_child(Evas_Object *child)
     {
     }
public:
   Evas_Object *get()
     {
       return eo;
     }

   // FIXME: could add to the parent here... raster to figure out
   Evas_Object *top_widget_get()
     {
       return elm_object_top_widget_get(eo);
     }

   virtual ~CEvasObject()
     {
       obj.Dispose();
       evas_object_unref(eo);
       eo = NULL;
     }
   void resize(v8::Local<v8::Value> width, v8::Local<v8::Value> height)
     {
       if (width->IsNumber() && height->IsNumber())
         evas_object_resize(eo, width->ToInteger()->Value(), height->ToInteger()->Value());
     }
   void move(v8::Local<v8::Value> x, v8::Local<v8::Value> y)
     {
       if (x->IsNumber() && y->IsNumber())
         evas_object_move(eo, x->ToInteger()->Value(), y->ToInteger()->Value());
     }
   void callback_set(v8::Local<v8::Value> val)
     {
       if (val->IsFunction())
         {
            v8::Local<v8::Function> local_func = v8::Local<v8::Function>::Cast(val);
            v8::Persistent<v8::Function> func = v8::Persistent<v8::Function>::New(local_func);
            evas_object_event_callback_add(eo, EVAS_CALLBACK_MOUSE_DOWN,
                                           &eo_on_click, static_cast<void*>(*func));
         }
     }
   void animator_set(v8::Local<v8::Value> val)
     {
       if (val->IsFunction())
         {
            v8::Local<v8::Function> local_func = v8::Local<v8::Function>::Cast(val);
            v8::Persistent<v8::Function> persist_func = v8::Persistent<v8::Function>::New(local_func);
            ecore_animator_add(&eo_on_animate, static_cast<void*>(*persist_func));
         }
     }
   virtual void label_set(v8::Local<v8::Value> val)
     {
       if (val->IsString())
         {
            v8::String::Utf8Value str(val);
            elm_button_label_set(eo, *str);
         }
     }
   virtual void image_set(v8::Local<v8::Value> val)
     {
       if (val->IsString())
         fprintf(stderr, "no image set\n");
     }
   virtual void show()
     {
        evas_object_show(eo);
     }
   void realize_objects(v8::Handle<v8::Object> elements)
     {
        if (elements.IsEmpty())
           return;

        v8::Handle<v8::Array> props = elements->GetPropertyNames();

        /* iterate through elements and instantiate them */
        for (unsigned int i = 0; i < props->Length(); i++)
          {
             v8::Handle<v8::Value> x = props->Get(v8::Integer::New(i));
             v8::String::Utf8Value val(x);

             realize_one(this, elements->Get(x->ToString())->ToObject());
          }
     }
};


class CElmBasicWindow : public CEvasObject {
public:
   CElmBasicWindow(const char *title)
     {
       eo = elm_win_add(NULL, "main", ELM_WIN_BASIC);
       elm_win_title_set(eo, title);
       evas_object_focus_set(eo, 1);
       evas_object_smart_callback_add(eo, "delete,request", &on_delete, NULL);
     }
   ~CElmBasicWindow()
     {
     }
   static void on_delete(void *data, Evas_Object *obj, void *event_info)
     {
       elm_exit();
     }
};

CElmBasicWindow *main_win;

class CElmButton : public CEvasObject {
public:
   CElmButton(CEvasObject *parent, v8::Local<v8::Object> obj) : CEvasObject(obj)
     {
       eo = elm_button_add(parent->top_widget_get());
       construct(eo);
     }
   virtual ~CElmButton()
     {
     }
};

class CElmBackground : public CEvasObject {
public:
   CElmBackground(CEvasObject *parent, v8::Local<v8::Object> obj) : CEvasObject(obj)
     {
       eo = elm_bg_add(parent->top_widget_get());
       elm_win_resize_object_add(parent->get(), eo);
       construct(eo);
     }
   virtual ~CElmBackground()
     {
     }
   virtual void image_set(v8::Local<v8::Value> val)
     {
       if (val->IsString())
         {
            v8::String::Utf8Value str(val);
            elm_bg_file_set(eo, *str, NULL);
         }
     }
};

class CElmRadio : public CEvasObject {
public:
   CElmRadio(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject(obj)
     {
       eo = elm_radio_add(parent->top_widget_get());
       construct(eo);
     }
   virtual void label_set(v8::Local<v8::Value> val)
     {
       if (val->IsString())
         {
            v8::String::Utf8Value str(val);
            elm_radio_label_set(eo, *str);
         }
     }
   virtual ~CElmRadio()
     {
     }
};

class CElmBox : public CEvasObject {
protected:
   virtual void add_child(Evas_Object *child)
     {
       elm_box_pack_end(eo, child);
     }
public:
   CElmBox(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject(obj)
     {
       eo = elm_box_add(parent->top_widget_get());
       realize_objects(obj->Get(v8::String::New("elements"))->ToObject());
       construct(eo);
     }
};

class CElmLabel : public CEvasObject {
public:
   CElmLabel(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject(obj)
     {
       eo = elm_label_add(parent->top_widget_get());
       construct(eo);
     }
   virtual void label_set(v8::Local<v8::Value> val)
     {
       if (val->IsString())
         {
            v8::String::Utf8Value str(val);
            elm_label_label_set(eo, *str);
         }
     }
};

void
realize_one(CEvasObject *parent, v8::Local<v8::Object> obj)
{
   CEvasObject *eo = NULL;

   v8::Local<v8::Value> val = obj->Get(v8::String::New("type"));
   v8::String::Utf8Value str(val);

   /* create the evas object */
   if (!strcmp(*str, "button"))
      {
        eo = new CElmButton(main_win, obj);
      }
   else if (!strcmp(*str, "background"))
      {
        eo = new CElmBackground(main_win, obj);
      }
   else if (!strcmp(*str, "label"))
      {
        eo = new CElmLabel(main_win, obj);
      }
   else if (!strcmp(*str, "radio"))
      {
        eo = new CElmRadio(main_win, obj);
      }
   else if (!strcmp(*str, "pack"))
      {
        eo = new CElmBox(main_win, obj);
      }

   if (!eo)
      {
        fprintf(stderr, "Bad object type %s\n", *str);
        return;
      }

   parent->add_child(eo->get());
   eo->show();
}

v8::Handle<v8::Value>
Realize(const v8::Arguments& args)
{
   if (args.Length() != 1)
      return v8::ThrowException(v8::String::New("Bad parameters"));
   realize_one(main_win, args[0]->ToObject());
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

int shebang_length(char *p, int len)
{
   int i = 0;

   if (len > 2 && p[0] == '#' && p[1] == '!')
      {
        for (i = 2; i < len && p[i] != '\n'; i++)
          ;
        if (i < len)
          i++;
      }

   return i;
}

v8::Handle<v8::String>
string_from_file(const char *filename)
{
   v8::Handle<v8::String> ret = v8::Handle<v8::String>();
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
     {
        fprintf(stderr, "compile failed\n");
        exit(1);
     }
   v8::Handle<v8::Value> result = script->Run();
}

void
elev8_run(const char *script)
{
   v8::HandleScope handle_scope;
   v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();

   global->Set(v8::String::New("realize"), v8::FunctionTemplate::New(Realize));
   global->Set(v8::String::New("print"), v8::FunctionTemplate::New(Print));

   main_win = new CElmBasicWindow(basename(script));

   evas_object_resize(main_win->get(), 320, 480);

   /* setup V8 */
   v8::Persistent<v8::Context> context = v8::Context::New(NULL, global);
   v8::Context::Scope context_scope(context);
   run_script(script);

   v8::Handle<v8::Object> glob = context->Global();
   main_win->realize_objects(glob->Get(v8::String::New("elements"))->ToObject());

   main_win->show();
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
