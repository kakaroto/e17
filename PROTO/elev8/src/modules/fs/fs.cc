#include <iostream>

#include <v8.h>
#include <Eio.h>
#include <Ecore.h>
#include <fnmatch.h>
#include <vector>
#include <string>

using namespace v8;

namespace fs {

static int log_domain;

#define DBG(...) EINA_LOG_DOM_DBG(log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(log_domain, __VA_ARGS__)
#define CRT(...) EINA_LOG_DOM_CRITICAL(log_domain, __VA_ARGS__)

class ListFiles {
public:
   ListFiles(const Arguments& args, const Local<Object>& thisObj_) {
      obj = Persistent<Object>::New(ListFiles::GetTemplate()->GetFunction()->NewInstance());
      obj->SetPointerInInternalField(0, this);
      obj.MakeWeak(this, ListFiles::Delete);

      thisObj = Persistent<Object>::New(thisObj_);
      callback = Persistent<Value>::New(args[1]);
      batch = Persistent<Array>::New(Array::New());

      Eio_Filter_Direct_Cb filter = NULL;
      if (args[2]->IsArray())
        {
           Array *a = Array::Cast(*args[2]->ToObject());

           patterns.reserve(a->Length());
           for (unsigned int i = 0; i < a->Length(); i++)
             {
                patterns.push_back(*String::Utf8Value(a->Get(i)));
             }
           filter = onFilter;
        }

      Eina_Bool recursive = args[3]->IsBoolean() ? args[3]->BooleanValue() : 0;
      period = args[4]->IsNumber() ? args[4]->NumberValue() : 0.5;
      allowHidden = args[5]->IsBoolean() ? args[5]->BooleanValue() : false;

      if (!allowHidden)
        filter = onFilter;

      Eio_File *(*worker)(const char *, Eio_Filter_Direct_Cb, Eio_Main_Direct_Cb, Eio_Done_Cb, Eio_Error_Cb, const void *);
      if (recursive)
        worker = eio_dir_stat_ls;
      else
        worker = eio_file_stat_ls;

      lastTimeout = ecore_loop_time_get();

      eio = worker(*String::Utf8Value(args[0]),
                   filter, onFile, onDone, onError, this);
   }

   Handle<Object> ToObject() {
      return obj;
   }

   void Stop() {
      if (eio)
        eio_file_cancel(eio);
      eio = NULL;
   }

   static Handle<Value> listFiles(const Arguments& args)
   {
      HandleScope scope;

      if (!args[0]->IsString())
        return Undefined();

      if (!args[1]->IsFunction())
        return Undefined();

      return (new ListFiles(args, args.This()))->ToObject();
   }

   static Handle<Value> StopWrapper(const Arguments& args)
   {
      ListFiles *self = static_cast<ListFiles *>(args.This()->GetPointerFromInternalField(0));
      self->Stop();
      return Undefined();
   }

   static Persistent<FunctionTemplate> GetTemplate() {
      if (ListFiles::tmpl.IsEmpty())
        {
           ListFiles::tmpl = Persistent<FunctionTemplate>::New(FunctionTemplate::New(listFiles));
           ListFiles::tmpl->InstanceTemplate()->SetInternalFieldCount(1);
           ListFiles::tmpl->SetClassName(String::NewSymbol("listFiles"));
           ListFiles::tmpl->PrototypeTemplate()->Set(String::NewSymbol("stop"), FunctionTemplate::New(StopWrapper));
        }

      return ListFiles::tmpl;
   }

   ~ListFiles() {
      HandleScope scope;

      if (eio) eio_file_cancel(eio);

      thisObj.Dispose();
      callback.Dispose();
      patterns.clear();

      for (int top = batch->Length() - 1; top >= 0; --top)
        batch->Delete(top);
      batch.Dispose();

      obj->Set(String::NewSymbol("disposed_already"), Boolean::New(true));
   }

private:
   Persistent<Object> obj;
   Persistent<Object> thisObj;
   Persistent<Value> callback;
   std::vector<std::string> patterns;
   Persistent<Array> batch;
   Eio_File *eio;
   double period;
   double lastTimeout;
   bool allowHidden;

   static Persistent<FunctionTemplate> tmpl;

   static void Delete(Persistent<Value> obj, void *data) {
      HandleScope scope;

      if (!obj->ToObject()->Has(String::NewSymbol("disposed_already")))
        delete static_cast<ListFiles *>(data);

      obj.Clear();
      obj.Dispose();
   }

   static Eina_Bool onFilter(void *data, Eio_File *, const Eina_File_Direct_Info *info)
   {
      const ListFiles *t = static_cast<ListFiles *>(data);
      const char *name = info->path + info->name_start;

      if ((!t->allowHidden) && (name[0] == '.'))
        return EINA_FALSE;

      if (info->type == EINA_FILE_DIR)
        return EINA_TRUE;
      if (info->type != EINA_FILE_REG)
        return EINA_FALSE;

      std::vector<std::string>::const_iterator itr;
      for (itr = t->patterns.begin(); itr != t->patterns.end(); itr++)
        if (fnmatch(itr->c_str(), name, 0) == 0)
          return EINA_TRUE;

      return EINA_FALSE;
   }

   static void onError(void *data, Eio_File *, int error)
   {
      ListFiles *t = static_cast<ListFiles *>(data);
      t->eio = NULL;
      t->patterns.clear();

      ERR("listfiles error: %d %s", error, strerror(error));
      t->dispatchBatch(true, true);
   }

   static void onDone(void *data, Eio_File *)
   {
      ListFiles *t = static_cast<ListFiles *>(data);
      t->eio = NULL;
      t->patterns.clear();

      INF("listFiles done.");
      t->dispatchBatch(true, false);
   }

   static void onFile(void *data, Eio_File *, const Eina_File_Direct_Info *info)
   {
      ListFiles *t = static_cast<ListFiles *>(data);

      DBG("listFiles: path=%s", info->path);

      Local<Object> file = Object::New();
      Local<String> kPath = String::NewSymbol("path");
      Local<String> kName = String::NewSymbol("name");
      Local<String> kIsFile = String::NewSymbol("isFile");

      file->Set(kPath, String::New(info->path));
      file->Set(kName, String::New(info->path + info->name_start));
      file->Set(kIsFile, Boolean::New((info->type == EINA_FILE_REG)));
      t->batch->Set(t->batch->Length(), file);

      double now = ecore_loop_time_get();
      if (now - t->lastTimeout >= t->period)
        {
           t->dispatchBatch(false, false);
           t->lastTimeout = now;
        }
   }

   void dispatchBatch(bool isDone, bool isError)
   {
      Handle<Value> args[3] = {
        batch,
        Boolean::New(isDone),
        Boolean::New(isError)
      };

      Handle<Function> func(Function::Cast(*callback));
      func->Call(thisObj, 3, args);

      for (int top = batch->Length() - 1; top >= 0; --top)
        batch->Delete(top);
   }
};

Persistent<FunctionTemplate> ListFiles::tmpl;

extern "C" {

void RegisterModule(Handle<Object> target);

void RegisterModule(Handle<Object> target)
{
   log_domain = eina_log_domain_register("elev8-fs", EINA_COLOR_ORANGE);
   if (!log_domain)
     {
        ERR("could not register elev8-fs log domain.");
        log_domain = EINA_LOG_DOMAIN_GLOBAL;
     }

   INF("elev8-fs Logging initialized. %d", log_domain);

   eio_init();
   target->Set(String::NewSymbol("listFiles"), ListFiles::GetTemplate()->GetFunction());
}
}

}
