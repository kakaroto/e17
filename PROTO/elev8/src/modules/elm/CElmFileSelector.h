#ifndef C_ELM_FILE_SELECTOR_H
#define C_ELM_FILE_SELECTOR_H

#include "elm.h"
#include "CElmLayout.h"

namespace elm {

using namespace v8;

class CElmFileSelector : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmFileSelector(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();
   virtual ~CElmFileSelector();

   struct {
      Persistent<Value> done;
   } cb;

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> selected_get() const;
   void selected_set(Handle<Value> val);

   Handle<Value> path_get() const;
   void path_set(Handle<Value> val);

   Handle<Value> expandable_get() const;
   void expandable_set(Handle<Value> val);

   Handle<Value> folder_only_get() const;
   void folder_only_set(Handle<Value> val);

   Handle<Value> is_save_get() const;
   void is_save_set(Handle<Value> val);

   Handle<Value> mode_get() const;
   void mode_set(Handle<Value> val);

   static void OnDoneWrapper(void *data, Evas_Object *, void *event_info);
   void OnDone(void *event_info);
   void on_done_set(Handle<Value> val);
   Handle<Value> on_done_get(void) const;

   Handle<Value> buttons_get() const;
   void buttons_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmFileSelector>(const Arguments& args);
};

}

#endif
