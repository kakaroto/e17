#ifndef C_ELM_THUMB_H
#define C_ELM_THUMB_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

class CElmThumb : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmThumb(Local<Object> _jsObject, CElmObject *parent);
   ~CElmThumb();

   static Handle<FunctionTemplate> GetTemplate();

   struct {
      Persistent<Value> generate_start;
      Persistent<Value> generate_stop;
      Persistent<Value> generate_error;
      Persistent<Value> load_error;
   } cb;

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> reload(const Arguments);

   Handle<Value> file_get(void) const;
   void file_set(Handle<Value> val);

   Handle<Value> path_get() const;
   void path_set(Handle<Value> val);

   Handle<Value> animate_get() const;
   void animate_set(Handle<Value> val);

   Handle<Value> editable_get() const;
   void editable_set(Handle<Value> val);

   void OnGenerateStart(void *event_info);
   static void OnGenerateStartWrapper(void *data, Evas_Object *, void *event_info);

   Handle<Value> on_generate_start_get() const;
   void on_generate_start_set(Handle<Value> val);

   void OnGenerateStop(void *event_info);
   static void OnGenerateStopWrapper(void *data, Evas_Object *, void *event_info);

   Handle<Value> on_generate_stop_get() const;
   void on_generate_stop_set(Handle<Value> val);

   void OnGenerateError(void *event_info);
   static void OnGenerateErrorWrapper(void *data, Evas_Object *, void *event_info);

   Handle<Value> on_generate_error_get() const;
   void on_generate_error_set(Handle<Value> val);

   void OnLoadError(void *event_info);
   static void OnLoadErrorWrapper(void *data, Evas_Object *, void *event_info);

   Handle<Value> on_load_error_get() const;
   void on_load_error_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmThumb>(const Arguments& args);
};

}
#endif
