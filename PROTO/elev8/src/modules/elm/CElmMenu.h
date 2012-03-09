#ifndef C_ELM_MENU_H
#define C_ELM_MENU_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmMenu : public CEvasObject {
    FACTORY(CElmMenu)

public:
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

    CElmMenu(CEvasObject *par, Local<Object> obj);

    virtual ~CElmMenu() { }

    static Handle<Value> addchild(const Arguments&);

    static Handle<Value> parent(const Arguments&);

    static Handle<Value> child(const Arguments&);

    static Handle<Value> child_count(const Arguments&);

    static void eo_on_click(void *data, Evas_Object *, void *);

    void items_set(MenuItem *parent, Handle<Value> val);

    virtual MenuItem * new_item_set(MenuItem *parent, Handle<Value> item);

    virtual Handle<Value> move_get() const;

    virtual void move_set(Handle<Value> val);

protected:
    CPropHandler<CElmMenu> prop_handler;
    MenuItem *root;
};

#endif
