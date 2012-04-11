#ifndef C_ELM_GEN_LIST_H
#define C_ELM_GEN_LIST_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

class CElmGenList : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmGenList(Local<Object> _jsObject, CElmObject *parent);

   struct GenListItemClass {
       Persistent<Value> type;
       Persistent<Value> data;
       Persistent<Value> on_text;
       Persistent<Value> on_content;
       Persistent<Value> on_state;
       Persistent<Value> on_delete;
       Persistent<Value> on_select;
       const char *item_style;
       Elm_Genlist_Item_Class eitc;
       CElmGenList *genlist;
   };

   static char *text_get(void *data, Evas_Object *, const char *part);
   static Evas_Object *content_get(void *data, Evas_Object *, const char *part);
   static Eina_Bool state_get(void *, Evas_Object *, const char *);
   static void del(void *data, Evas_Object *);
   static void sel(void *data, Evas_Object *, void *);

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> clear(const Arguments& args);
   Handle<Value> append(const Arguments& args);

   Handle<Value> multi_select_get() const;
   void multi_select_set(Handle<Value> value);

   Handle<Value> reorder_mode_get() const;
   void reorder_mode_set(Handle<Value> value);

   Handle<Value> mode_get() const;
   void mode_set(Handle<Value> value);

   Handle<Value> select_mode_get() const;
   void select_mode_set(Handle<Value> value);

   Handle<Value> block_count_get() const;
   void block_count_set(Handle<Value> value);

   Handle<Value>longpress_timeout_get() const;
   void longpress_timeout_set(Handle<Value> value);

   void vertical_bounce_set(Handle<Value> val);
   Handle<Value> vertical_bounce_get() const;

   void horizontal_bounce_set(Handle<Value> val);
   Handle<Value> horizontal_bounce_get() const;

   Handle<Value> highlight_mode_get() const;
   void highlight_mode_set(Handle<Value> value);

   Handle<Value> tree_effect_get() const;
   void tree_effect_set(Handle<Value> value);

   Handle<Value> decorate_mode_get() const;
   void decorate_mode_set(Handle<Value> value);

   Handle<Value> homogeneous_get() const;
   void homogeneous_set(Handle<Value> value);

   friend Handle<Value> CElmObject::New<CElmGenList>(const Arguments&);
};

}

#endif

