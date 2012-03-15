#ifndef C_ELM_MENU_H
#define C_ELM_MENU_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmMenu : public CEvasObject {
   FACTORY(CElmMenu)

protected:
   CElmMenu(CEvasObject *par, Local<Object> obj);

   ~CElmMenu();

   class Item {
   public:
      Persistent<Value> on_clicked;
      Persistent<Value> label;
      Persistent<Value> icon;
      bool disabled;
   };

   //FIXME: refactor this struct entirely...
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

   MenuItem *new_item_set(MenuItem *parent, Handle<Object> item);
   void items_set(MenuItem *parent, Handle<Value> val);

   static void eo_on_click(void *data, Evas_Object *, void *);
};

#endif
