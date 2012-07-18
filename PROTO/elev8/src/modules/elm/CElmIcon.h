#ifndef C_ELM_ICON_H
#define C_ELM_ICON_H

#include "elm.h"
#include "CElmImage.h"

namespace elm {

using namespace v8;

class CElmIcon : public CElmImage {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmIcon(Local<Object> _jsObject, CElmObject *parent);

   static Handle<FunctionTemplate> GetTemplate();

   ~CElmIcon();

   Persistent<Value> thumb;
public:
   static void Initialize(Handle<Object> target);

   virtual void file_set(Handle<Value> val);
   virtual Handle<Value> file_get(void) const;

   Handle<Value> lookup_order_get() const;
   void lookup_order_set(Handle<Value> val);

   Handle<Value> thumb_get() const;
   void thumb_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmIcon>(const Arguments& args);
};

}

#endif // C_ELM_ICON_H
