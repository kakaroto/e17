#ifndef C_ELM_LAYOUT_H
#define C_ELM_LAYOUT_H

#include "elm.h"
#include "CElmObject.h"
#include "CElmContainer.h"

namespace elm {

using namespace v8;

class CElmLayout : public CElmContainer {
private:
   static Persistent<FunctionTemplate> tmpl;
   CElmLayout(Local<Object> _jsObject, CElmObject *parent);

   Persistent<Value> fileused;
   Persistent<Value> chosentheme;
   Persistent<Value> part_cursor;
   Persistent<Value> cursor_style;
   Persistent<Value> cursor_engine;

protected:
   CElmLayout(Local<Object> _jsObject, Evas_Object *child);
   static Handle<FunctionTemplate> GetTemplate();
   ~CElmLayout();

public:

   virtual Handle<Value> contents_get() const;
   virtual void contents_set(Handle<Value> val);

   virtual Handle<Value> file_get() const;
   virtual void file_set(Handle<Value> val);

   virtual Handle<Value> theme_get() const;
   virtual void theme_set(Handle<Value> val);
};

}

#endif // C_ELM_LAYOUT_H
