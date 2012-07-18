#ifndef C_ELM_DATE_TIME_H
#define C_ELM_DATE_TIME_H

#include "elm.h"
#include "CElmLayout.h"

namespace elm {

class CElmDateTime : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmDateTime(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmDateTime();

   struct {
      Persistent<Value> change;
      Persistent<Value> lang_change;
   } cb;

   static Handle<FunctionTemplate> GetTemplate();

   Local<Object> TimeGet(struct tm *time) const;
   bool TimeSet(struct tm *time, Handle<Value> val) const;

   bool GetMinMaxFromObject(Handle<Value> val, int &min_out, int &max_out) const;

   Local<Object> GetFieldLimits(Elm_Datetime_Field_Type type) const;

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> format_get() const;
   void format_set(Handle<Value> val);

   Handle<Value> value_max_get() const;
   void value_max_set(Handle<Value> val);

   Handle<Value> value_min_get() const;
   void value_min_set(Handle<Value> val);

   Handle<Value> value_get() const;
   void value_set(Handle<Value> val);

   Handle<Value> field_limit_get() const;
   void field_limit_set(Handle<Value> val);

   Handle<Value> field_visible_get() const;
   void field_visible_set(Handle<Value> value);

   void OnChange(void *);
   static void OnChangeWrapper(void *, Evas_Object *, void *);

   void on_change_set(Handle<Value> val);
   Handle<Value> on_change_get(void) const;

   void OnLangChange(void *);
   static void OnLangChangeWrapper(void *, Evas_Object *, void *);

   void on_lang_change_set(Handle<Value> val);
   Handle<Value> on_lang_change_get(void) const;

   friend Handle<Value> CElmObject::New<CElmDateTime>(const Arguments& args);
};

}

#endif
