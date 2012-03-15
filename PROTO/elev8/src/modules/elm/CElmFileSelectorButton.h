#ifndef C_ELM_FILE_SELECTOR_BUTTON_H
#define C_ELM_FILE_SELECTOR_BUTTON_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmFileSelectorButton : public CEvasObject {
   FACTORY(CElmFileSelectorButton)

protected:
   CPropHandler<CElmFileSelectorButton> prop_handler;

   virtual void on_click(void *event_info);
   CElmFileSelectorButton(CEvasObject *parent, Local<Object> obj);
public:
   virtual Handle<Value> win_title_get() const;
   virtual void win_title_set(Handle<Value> val);

   virtual Handle<Value> path_get() const;
   virtual void path_set(Handle<Value> val);

   virtual void win_size_set(Handle<Value> val);
   virtual Handle<Value> win_size_get(void) const;

   virtual Handle<Value> expandable_get() const;
   virtual void expandable_set(Handle<Value> val);

   virtual Handle<Value> folder_only_get() const;
   virtual void folder_only_set(Handle<Value> val);

   virtual Handle<Value> is_save_get() const;
   virtual void is_save_set(Handle<Value> val);

   virtual Handle<Value> inwin_mode_get() const;
   virtual void inwin_mode_set(Handle<Value> val);

   virtual void on_clicked_set(Handle<Value> val);
   virtual Handle<Value> on_clicked_get(void) const;
};
#endif
