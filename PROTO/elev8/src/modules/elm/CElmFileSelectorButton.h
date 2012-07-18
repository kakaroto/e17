#ifndef C_ELM_FILE_SELECTOR_BUTTON_H
#define C_ELM_FILE_SELECTOR_BUTTON_H

#include "elm.h"
#include "CElmButton.h"

namespace elm {

using namespace v8;

class CElmFileSelectorButton : public CElmButton {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmFileSelectorButton(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();
   virtual ~CElmFileSelectorButton();

   struct {
      Persistent<Value> file_choose;
   } cb;

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> win_title_get() const;
   void win_title_set(Handle<Value> val);

   Handle<Value> path_get() const;
   void path_set(Handle<Value> val);

   void win_size_set(Handle<Value> val);
   Handle<Value> win_size_get(void) const;

   Handle<Value> expandable_get() const;
   void expandable_set(Handle<Value> val);

   Handle<Value> folder_only_get() const;
   void folder_only_set(Handle<Value> val);

   Handle<Value> is_save_get() const;
   void is_save_set(Handle<Value> val);

   Handle<Value> inwin_mode_get() const;
   void inwin_mode_set(Handle<Value> val);

   static void OnFileChooseWrapper(void *data, Evas_Object *, void *event_info);
   void OnFileChoose(void *event_info);
   void on_file_choose_set(Handle<Value> val);
   Handle<Value> on_file_choose_get(void) const;

   friend Handle<Value> CElmObject::New<CElmFileSelectorButton>(const Arguments& args);
};

}

#endif
