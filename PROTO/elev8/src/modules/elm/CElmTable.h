#ifndef C_ELM_TABLE_H
#define C_ELM_TABLE_H

#include <v8.h>
#include <list>
#include "CEvasObject.h"

class CElmTable : public CEvasObject {
   FACTORY(CElmTable)

protected:
   CPropHandler<CElmTable> prop_handler;
   std::list<CEvasObject *> table_items;

   CElmTable(CEvasObject *parent, Local<Object> obj);

   static Handle<Value> pack(const Arguments& args);
   static Handle<Value> unpack(const Arguments&);
   static Handle<Value> clear(const Arguments& args);
public:
   virtual void items_set(Handle<Value> val);
   virtual Handle<Value> new_item_set(Handle<Value> item);

   void homogeneous_set(Handle<Value> val);
   virtual Handle<Value> homogeneous_get() const;

   void padding_set(Handle<Value> val);
   virtual Handle<Value> padding_get() const;
};

#endif
