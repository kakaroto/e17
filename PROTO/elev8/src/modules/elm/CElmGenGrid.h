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
   Persistent<Value> page_size;
   Persistent<Value> scroller_policy;

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> clear(const Arguments& args);
   Handle<Value> append(const Arguments& args);
   Handle<Value> delete_item(const Arguments& args);
   Handle<Value> update_item(const Arguments& args);
   Handle<Value> page_show(const Arguments& args);

   Handle<Value> multi_select_get() const;
   void multi_select_set(Handle<Value> value);

   Handle<Value> reorder_mode_get() const;
   void reorder_mode_set(Handle<Value> value);

   Handle<Value> select_mode_get() const;
   void select_mode_set(Handle<Value> value);

   Handle<Value> block_count_get() const;
   void block_count_set(Handle<Value> value);

   void vertical_bounce_set(Handle<Value> val);
   Handle<Value> vertical_bounce_get() const;

   void horizontal_bounce_set(Handle<Value> val);
   Handle<Value> horizontal_bounce_get() const;

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

   Handle<Value> page_relative_get() const;
   void page_relative_set(Handle<Value> value);

   Handle<Value> align_get() const;
   void align_set(Handle<Value> value);

   Handle<Value> group_item_size_get() const;
   void group_item_size_set(Handle<Value> value);

   Handle<Value> page_size_get() const;
   void page_size_set(Handle<Value> value);

   Handle<Value> scroller_policy_get() const;
   void scroller_policy_set(Handle<Value> value);

   Handle<Value> items_count_get() const;

   Handle<Value> vertical_current_page_get() const;
   Handle<Value> horizontal_current_page_get() const;

   Handle<Value> vertical_last_page_get() const;
   Handle<Value> horizontal_last_page_get() const;

   friend Handle<Value> CElmObject::New<CElmGenGrid>(const Arguments&);
};

}

#endif

