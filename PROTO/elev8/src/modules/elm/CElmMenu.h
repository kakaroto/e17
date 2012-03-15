#ifndef C_ELM_MENU_H
#define C_ELM_MENU_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmMenu : public CEvasObject {
   FACTORY(CElmMenu)

protected:
   CElmMenu(CEvasObject *par, Local<Object> obj);

   class Item {
   public:
      Local<Value> on_clicked;
      Handle<Value> label;
      Handle<Value> icon;
      bool disabled;
   };

   class MenuItem : public Item {
   public:
      Elm_Object_Item *mi;
      MenuItem *next;
      MenuItem *prev;
      MenuItem *parent;
      MenuItem *child;
   };

   CPropHandler<CElmMenu> prop_handler;
   MenuItem *root;

   MenuItem *new_item_set(MenuItem *parent, Handle<Value> item);
   void items_set(MenuItem *parent, Handle<Value> val);

   static void eo_on_click(void *data, Evas_Object *, void *);

public:
   virtual Handle<Value> move_get() const;
   virtual void move_set(Handle<Value> val);
};

#endif
