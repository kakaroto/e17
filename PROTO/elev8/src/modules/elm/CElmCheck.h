#ifndef C_ELM_CHECK_H
#define C_ELM_CHECK_H

#include "elm.h"
#include "CElmLayout.h"

namespace elm {

class CElmCheck : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmCheck(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmCheck();

   struct {
      Persistent<Value> icon;
   } cached;

   struct {
      Persistent<Value> change;
   } cb;

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   void OnChange(void *);
   static void OnChangeWrapper(void *, Evas_Object *, void *);

   void on_change_set(Handle<Value> val);
   Handle<Value> on_change_get(void) const;

   Handle<Value> state_get() const;
   void state_set(Handle<Value> value);

   Handle<Value> icon_get() const;
   void icon_set(Handle<Value> value);

   Handle<Value> onlabel_get(void) const;
   void onlabel_set(Handle<Value> val);

   Handle<Value> offlabel_get(void) const;
   void offlabel_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmCheck>(const Arguments& args);
};

}
#endif
