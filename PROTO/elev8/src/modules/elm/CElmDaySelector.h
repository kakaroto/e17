#ifndef C_ELM_DAY_SELECTOR_H
#define C_ELM_DAY_SELECTOR_H

#include "elm.h"
#include "CElmLayout.h"

namespace elm {

class CElmDaySelector : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmDaySelector(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmDaySelector();

   struct {
      Persistent<Value> change;
      Persistent<Value> lang_change;
   } cb;

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> day_selected_get(int day) const;
   void day_selected_set(int day, Handle<Value> val);

   Handle<Value> monday_get() const;
   void monday_set(Handle<Value> val);

   Handle<Value> tuesday_get() const;
   void tuesday_set(Handle<Value> val);

   Handle<Value> wednesday_get() const;
   void wednesday_set(Handle<Value> val);

   Handle<Value> thursday_get() const;
   void thursday_set(Handle<Value> val);

   Handle<Value> friday_get() const;
   void friday_set(Handle<Value> val);

   Handle<Value> saturday_get() const;
   void saturday_set(Handle<Value> val);

   Handle<Value> sunday_get() const;
   void sunday_set(Handle<Value> val);

   Handle<Value> week_start_get() const;
   void week_start_set(Handle<Value> val);

   Handle<Value> weekend_start_get() const;
   void weekend_start_set(Handle<Value> val);

   Handle<Value> weekend_length_get() const;
   void weekend_length_set(Handle<Value> val);

   void OnChange(void *);
   static void OnChangeWrapper(void *, Evas_Object *, void *);

   void on_change_set(Handle<Value> val);
   Handle<Value> on_change_get(void) const;

   void OnLangChange(void *);
   static void OnLangChangeWrapper(void *, Evas_Object *, void *);

   void on_lang_change_set(Handle<Value> val);
   Handle<Value> on_lang_change_get(void) const;

   friend Handle<Value> CElmObject::New<CElmDaySelector>(const Arguments& args);
};

}
#endif
