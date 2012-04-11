#ifndef _CELM_WINDOW_H
#define _CELM_WINDOW_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmWindow : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmWindow(Local<Object> _jsObject, CElmObject *parent, const char *name, Elm_Win_Type type);
   static Handle<FunctionTemplate> GetTemplate();

   static Handle<Value> New(const Arguments& args);
   static void Delete(Persistent<Value>, void *parameter);

   static void quit(void *, Evas_Object *, void *);

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> Gettitle() const;
   void Settitle(Handle<Value> val);

   Handle<Value> Getconformant() const;
   void Setconformant(Handle<Value> val);
};

}

#endif
