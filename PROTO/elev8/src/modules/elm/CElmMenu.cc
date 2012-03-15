#include "CElmMenu.h"

CElmMenu::CElmMenu(CEvasObject *par, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_menu_add(par->top_widget_get());
   root = NULL;
   construct(eo, obj);
   items_set(NULL, obj->Get(String::New("items")));
   get_object()->Set(String::New("addchild"), FunctionTemplate::New(addchild)->GetFunction());

   get_object()->Set(String::New("child"), FunctionTemplate::New(child)->GetFunction());
   get_object()->Set(String::New("parent"), FunctionTemplate::New(parent)->GetFunction());
   get_object()->Set(String::New("child_count"), FunctionTemplate::New(child_count)->GetFunction());
}

Handle<Value> CElmMenu::addchild(const Arguments&)
{
   return Undefined();
}

Handle<Value> CElmMenu::parent(const Arguments&)
{
   return Undefined();
}

Handle<Value> CElmMenu::child(const Arguments&)
{
   return Undefined();
}

Handle<Value> CElmMenu::child_count(const Arguments&)
{
   return Undefined();
}

void CElmMenu::eo_on_click(void *data, Evas_Object *, void *)
{
   if (data)
     {
        Item *it = reinterpret_cast<Item *>(data);

        if (*it->on_clicked != NULL)
          {
             if (it->on_clicked->IsFunction())
               {
                  Handle<Function> fn(Function::Cast(*(it->on_clicked)));
                  fn->Call(fn, 0, NULL);
               }
          }
     }
}

void CElmMenu::items_set(MenuItem *parent, Handle<Value> val)
{
   /* add a list of children */
   if (!val->IsObject())
     {
        ELM_ERR( "not an object!");
        return;
     }

   Local<Object> in = val->ToObject();
   Local<Array> props = in->GetPropertyNames();
   /* iterate through elements and instantiate them */
   for (unsigned int i = 0; i < props->Length(); i++)
     {

        Local<Value> x = props->Get(Integer::New(i));
        String::Utf8Value val(x);

        Local<Value> item = in->Get(x->ToString());
        if (!item->IsObject())
          {
             ELM_ERR( "list item is not an object");
             continue;
          }

        MenuItem *par = new_item_set(parent, item);

        Local<Value> items_object = item->ToObject()->Get(String::New("items"));
        if (items_object->IsObject())
          {
             items_set(par, items_object);
          }
     }
}

CElmMenu::MenuItem *CElmMenu::new_item_set(CElmMenu::MenuItem *parent, Handle<Value> item)
{
   if (!item->IsObject())
     {
        // FIXME: permit adding strings here?
        ELM_ERR( "list item is not an object");
        return NULL;
     }
   Elm_Object_Item *par = NULL;
   if (parent!=NULL)
     {
        par = parent->mi;
     }

   Local<Value> sep_object = item->ToObject()->Get(String::New("seperator"));

   if ( sep_object->IsBoolean() )
     {
        // FIXME add if seperator : true, what if false
        if (sep_object->ToBoolean()->Value())
          {
             elm_menu_item_separator_add(eo, par);
          }
        return parent;
     }
   else
     {
        MenuItem *it = NULL;

        it = new MenuItem();
        it->next = NULL;
        it->prev = NULL;
        it->child = NULL;
        it->parent = NULL;
        it->label = v8::Persistent<Value>::New(item->ToObject()->Get(String::New("label")));
        it->icon = v8::Persistent<Value>::New(item->ToObject()->Get(String::New("icon")));
        it->on_clicked = Local<Value>::New(item->ToObject()->Get(String::New("on_clicked")));
        it->parent = parent;


        // either a label with icon
        if ( !it->label->IsString() && !it->icon->IsString() )
          {
             ELM_ERR( "Not a label or seperator");
             delete it;
             return NULL;
          }

        String::Utf8Value label(it->label->ToString());
        String::Utf8Value icon(it->icon->ToString());

        Evas_Smart_Cb cb;
        void *data = NULL;

        if ( it->on_clicked->IsFunction() )
          {
             cb = &eo_on_click;
             data = reinterpret_cast<void *>(it);
          }

        it->mi = elm_menu_item_add(eo, par, *icon, *label, cb, data);

        //FIXME :: Refactor
        if (this->root==NULL)
          {
             this->root = it;
          }
        else
          {
             if (parent)
               {
                  it->parent = parent;
                  if (parent->child==NULL)
                    {
                       parent->child = it;
                    }
                  else
                    {
                       MenuItem *ptr = parent->child;

                       while(ptr->next)
                         {
                            ptr = ptr->next;
                         }

                       ptr->next = it;
                       it->prev = ptr;
                    }
               }
             else
               {
                  MenuItem *ptr = this->root;
                  while(ptr->next)
                    {
                       ptr = ptr->next;
                    }
                  ptr->next = it;
                  it->prev = ptr;
               }
          }

        Local<Value> disabled_object = item->ToObject()->Get(String::New("disabled"));

        if ( disabled_object->IsBoolean() )
          {
             elm_object_item_disabled_set(it->mi, disabled_object->ToBoolean()->Value());
          }
        return it;
     }
}

Handle<Value> CElmMenu::move_get() const
{
   return Undefined();
}

void CElmMenu::move_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;
   Local<Object> obj = val->ToObject();
   Local<Value> x = obj->Get(String::New("x"));
   Local<Value> y = obj->Get(String::New("y"));
   if (!x->IsNumber() || !y->IsNumber())
     return;
   Evas_Coord x_out = x->NumberValue();
   Evas_Coord y_out = y->NumberValue();
   elm_menu_move (eo, x_out, y_out);
}

PROPERTIES_OF(CElmMenu) = {
     PROP_HANDLER(CElmMenu, move),
     { NULL }
};
