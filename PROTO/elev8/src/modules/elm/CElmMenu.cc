#include "CElmMenu.h"

CElmMenu::CElmMenu(CEvasObject *par, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
   , root(NULL)
{
   eo = elm_menu_add(par->top_widget_get());
   construct(eo, obj);
   items_set(NULL, obj->Get(String::New("items")));
}

CElmMenu::~CElmMenu()
{
   //FIXME: call ->Dispose() on Permanent<> variables
   //FIXME: delete menu items
}

void CElmMenu::eo_on_click(void *data, Evas_Object *, void *)
{
   if (!data)
     return;

   Item *it = reinterpret_cast<Item *>(data);

   if (it->on_clicked->IsUndefined())
     return;

   HandleScope scope;
   Local<Function> fn(Function::Cast(*(it->on_clicked)));
   fn->Call(fn, 0, NULL);
}

void CElmMenu::items_set(MenuItem *parent, Handle<Value> val)
{
   /* add a list of children */
   if (!val->IsObject())
     {
        ELM_ERR("not an object!");
        return;
     }

   HandleScope scope;
   Local<Object> in = val->ToObject();
   Local<Array> props = in->GetOwnPropertyNames();

   /* iterate through elements and instantiate them */
   for (unsigned int i = 0; i < props->Length(); i++)
     {
        Local<Value> propname = props->Get(i);
        Local<Value> item = in->Get(propname->ToString());
        if (!item->IsObject())
          {
             ELM_ERR( "list item is not an object");
             continue;
          }

        MenuItem *par = new_item_set(parent, item->ToObject());
        if (!par)
          continue;

        Local<Value> items_object = item->ToObject()->Get(String::New("items"));
        if (items_object->IsObject())
          items_set(par, items_object);
     }
}

CElmMenu::MenuItem *CElmMenu::new_item_set(CElmMenu::MenuItem *parent, Handle<Object> item)
{
   Elm_Object_Item *par = parent ? parent->mi : NULL;

   HandleScope scope;
   Local<Value> separator = item->Get(String::New("separator"));

   if (separator->IsBoolean() && separator->ToBoolean()->Value())
     {
        elm_menu_item_separator_add(eo, par);
        return NULL;
     }

   MenuItem *it = new MenuItem();

   it->next = NULL;
   it->prev = NULL;
   it->child = NULL;
   it->label = Persistent<Value>::New(item->Get(String::New("label")));
   it->icon = Persistent<Value>::New(item->Get(String::New("icon")));
   it->on_clicked = Persistent<Value>::New(item->Get(String::New("on_clicked")));
   it->parent = parent;

   if (!it->label->IsString() && !it->icon->IsString())
     {
        ELM_ERR("Define at least a label or icon");
        delete it;
        return NULL;
     }

   Evas_Smart_Cb cb;
   void *data;

   if (it->on_clicked->IsFunction())
     {
        cb = &eo_on_click;
        data = reinterpret_cast<void *>(it);
     }
   else
     {
        cb = NULL;
        data = NULL;
     }

   it->mi = elm_menu_item_add(eo, par, *String::Utf8Value(it->icon),
                              *String::Utf8Value(it->label), cb, data);

   //FIXME :: Refactor
   if (root==NULL)
     {
        this->root = it;
     }
   else
     {
        if (parent)
          {
             it->parent = parent;
             if (parent->child==NULL)
               parent->child = it;
             else
               {
                  MenuItem *ptr = parent->child;
                  while (ptr->next)
                    ptr = ptr->next;

                  ptr->next = it;
                  it->prev = ptr;
               }
          }
        else
          {
             MenuItem *ptr = this->root;
             while (ptr->next)
               ptr = ptr->next;

             ptr->next = it;
             it->prev = ptr;
          }
     }

   Local<Value> disabled = item->Get(String::New("disabled"));

   if (disabled->IsBoolean())
     elm_object_item_disabled_set(it->mi, disabled->ToBoolean()->Value());

   return it;
}

PROPERTIES_OF(CElmMenu) = NO_PROPERTIES;
