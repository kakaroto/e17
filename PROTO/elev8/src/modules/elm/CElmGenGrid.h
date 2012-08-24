#ifndef C_ELM_GEN_GRID_H
#define C_ELM_GEN_GRID_H

#include "elm.h"
#include "CElmGen.h"
#include "CElmObject.h"

namespace elm {

class CElmGenGrid : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmGenGrid(Local<Object> _jsObject, CElmObject *parent);
   ~CElmGenGrid();

   struct {
      Persistent<Object> classes;
   } cached;

   Persistent<Value> page_relative;
   Persistent<Value> align;

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);
   static void UpdateItem(Handle<Value> value);

   Handle<Value> clear(const Arguments& args);
   Handle<Value> append(const Arguments& args);
   Handle<Value> realized_items_update(const Arguments& args);
   Handle<Value> bring_in_item(const Arguments& args);

   virtual Handle<Value> Pack(Handle<Value>, Handle<Value>);
   virtual Handle<Value> Unpack(Handle<Value>);

   Handle<Value> multi_select_get() const;
   void multi_select_set(Handle<Value> value);

   Handle<Value> reorder_mode_get() const;
   void reorder_mode_set(Handle<Value> value);

   Handle<Value> select_mode_get() const;
   void select_mode_set(Handle<Value> value);

   Handle<Value> block_count_get() const;
   void block_count_set(Handle<Value> value);

   Handle<Value> highlight_mode_get() const;
   void highlight_mode_set(Handle<Value> value);

   Handle<Value> item_size_horizontal_get() const;
   void item_size_horizontal_set(Handle<Value> value);

   Handle<Value> item_size_vertical_get() const;
   void item_size_vertical_set(Handle<Value> value);

   Handle<Value> classes_get() const;
   void classes_set(Handle<Value> value);

   Handle<Value> horizontal_get() const;
   void horizontal_set(Handle<Value> value);

   Handle<Value> filled_get() const;
   void filled_set(Handle<Value> value);

   Handle<Value> item_align_get() const;
   void item_align_set(Handle<Value> value);

   Handle<Value> group_item_size_get() const;
   void group_item_size_set(Handle<Value> value);

   Handle<Value> items_count_get() const;

   Handle<Value> realized_items_get() const;

   friend Handle<Value> CElmObject::New<CElmGenGrid>(const Arguments&);
};

}

#endif

