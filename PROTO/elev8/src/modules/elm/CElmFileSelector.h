#ifndef C_ELM_FILE_SELECTOR_H
#define C_ELM_FILE_SELECTOR_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmFileSelector : public CEvasObject {
   FACTORY(CElmFileSelector)

protected:
   CPropHandler<CElmFileSelector> prop_handler;

public:
   CElmFileSelector(CEvasObject *parent, Local<Object> obj);

   virtual Handle<Value> selected_get() const;

   virtual void selected_set(Handle<Value> val);

   virtual Handle<Value> path_get() const;

   virtual void path_set(Handle<Value> val);

   virtual Handle<Value> expandable_get() const;

   virtual void expandable_set(Handle<Value> val);

   virtual Handle<Value> folder_only_get() const;

   virtual void folder_only_set(Handle<Value> val);

   virtual Handle<Value> is_save_get() const;

   virtual void is_save_set(Handle<Value> val);

   virtual Handle<Value> mode_get() const;

   virtual void mode_set(Handle<Value> val);

   //TODO : Add support for more events.
   //"changed" 
   //"activated" 
   //"press" 
   //"longpressed" 
   //"clicked" 
   //"clicked,double" 
   //"focused" 
   //"unfocused" 
   //"selection,paste" 
   //"selection,copy" 
   //"selection,cut" 
   //"unpressed" 

   virtual void on_click(void *event_info);

   virtual void on_clicked_set(Handle<Value> val);

   virtual Handle<Value> on_clicked_get(void) const;
};



#endif
