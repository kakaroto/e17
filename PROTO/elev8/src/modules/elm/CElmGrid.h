#ifndef C_ELM_GRID_H
#define C_ELM_GRID_H

#include <v8.h>
#include <list>
#include "CEvasObject.h"

using namespace v8;

class CElmGrid : public CEvasObject {
   FACTORY(CElmGrid)

protected:
   CPropHandler<CElmGrid> prop_handler;
   std::list<CEvasObject *> grid_items;

public:
   CElmGrid(CEvasObject *parent, Local<Object> obj);

   static Handle<Value> add(const Arguments& args);

   static Handle<Value> clear(const Arguments& args);

   virtual void items_set(Handle<Value> val);

   void pack_set(Handle<Value> item);

   virtual Handle<Value> pack_get() const;

   void size_set(Handle<Value> val);

   virtual Handle<Value> size_get() const;
};
#endif
