/*
 * elev8 - javascript for EFL
 *
 * The script's job is to prepare for the main loop to run
 * then exit
 */

#include <list>
#include <map>
#include <string>

#include "elev8_elm.h"
#include "CEvasObject.h"
#include "CEvasImage.h"
#include "CElmBasicWindow.h"
#include "CElmButton.h"
#include "CElmLayout.h"
#include "CElmBackground.h"
#include "CElmRadio.h"
#include "CElmBox.h"
#include "CElmLabel.h"
#include "CElmFlip.h"
#include "CElmActionSlider.h"
#include "CElmIcon.h"
#include "CElmScroller.h"
#include "CElmSlider.h"
#include "CElmImage.h"
#include "CElmGrid.h"
#include "CElmNaviframe.h"
#include "CElmNotify.h"
#include "CElmInwin.h"
#include "CElmFileSelectorEntry.h"
#include "CElmFileSelectorButton.h"
#include "CElmHover.h"
#include "CElmToggle.h"
#include "CElmPhotocam.h"
#include "CElmClock.h"

int elev8_elm_log_domain = -1;

using namespace v8;

class CElmGenList : public CEvasObject {
   FACTORY(CElmGenList)
protected:
   CPropHandler<CElmGenList> prop_handler;

   struct GenListItemClass {
       Persistent<Value> type;
       Persistent<Value> data;
       Persistent<Value> on_text;
       Persistent<Value> on_content;
       Persistent<Value> on_state;
       Persistent<Value> on_delete;
       Persistent<Value> on_select;
       std::string item_style;
       Elm_Genlist_Item_Class eitc;
       CElmGenList *genlist;
   };

public:
   CElmGenList(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
     {
        eo = elm_genlist_add(parent->top_widget_get());
        construct(eo, obj);
        get_object()->Set(String::New("append"), FunctionTemplate::New(append)->GetFunction());
        get_object()->Set(String::New("clear"), FunctionTemplate::New(clear)->GetFunction());
     }


   /* GenList functions that are going to do the heavy weight lifting */
   static char *text_get(void *data, Evas_Object *, const char *part)
     {
        GenListItemClass *itc = (GenListItemClass *)data;
        Handle<Function> fn(Function::Cast(*(itc->on_text)));
        Local<Object> temp = Object::New();
        temp->Set(String::New("part"), String::New(part));
        temp->Set(String::New("data"), itc->data);
        Handle<Value> args[1] = { temp };
        Local<Value> text = fn->Call(temp, 1, args);

        if (text->IsString())
          {
             String::Utf8Value str(text->ToString());
             return strdup(*str);
          }
        else
          return NULL;
     }

   static Evas_Object *content_get(void *data, Evas_Object *, const char *part)
     {
        GenListItemClass *itc = (GenListItemClass *)data;
        Handle<Function> fn(Function::Cast(*(itc->on_content)));
        Local<Object> temp = Object::New();
        temp->Set(String::New("part"), String::New(part));
        temp->Set(String::New("data"), itc->data);
        Handle<Value> args[1] = { temp };
        Local<Value> retval = fn->Call(temp, 1, args);

        if (retval->IsObject())
          {
             CEvasObject *content = make_or_get(itc->genlist, retval->ToObject());
             if (content)
               return content->get();
          }

        return NULL;
     }

   static Eina_Bool state_get(void *, Evas_Object *, const char *)
     {
        return EINA_TRUE;
     }

   static void del(void *data, Evas_Object *)
     {
        delete static_cast<GenListItemClass *>(data);
     }

   static void sel(void *data, Evas_Object *, void *)
     {
        GenListItemClass *itc = (GenListItemClass *)data;
        if (itc->on_select.IsEmpty())
          return;
        Handle<Function> fn(Function::Cast(*(itc->on_select)));
        Local<Object> temp = Object::New();
        temp->Set(String::New("data"), itc->data);
        fn->Call(temp, 0, 0);
     }
   /* End of GenList functions */

   static Handle<Value> clear(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        elm_genlist_clear(static_cast<CElmGenList *>(self)->get());
        return Undefined();
     }

   static Handle<Value> append(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmGenList *genlist = static_cast<CElmGenList *>(self);
        if (args[0]->IsObject())
          {
             GenListItemClass *itc = new GenListItemClass();

             Local<Object> obj = args[0]->ToObject();
             itc->type = Persistent<Value>::New(obj->Get(String::New("type")));
             itc->on_text = Persistent<Value>::New(obj->Get(String::New("on_text")));
             itc->on_content = Persistent<Value>::New(obj->Get(String::New("on_content")));
             itc->on_state = Persistent<Value>::New(obj->Get(String::New("on_state")));
             itc->on_select = Persistent<Value>::New(obj->Get(String::New("on_select")));
             itc->data = Persistent<Value>::New(obj->Get(String::New("data")));
             //TODO : Check genlist class type and modify or add
			 
             itc->eitc.item_style = "default";
             itc->eitc.func.text_get = text_get;
             itc->eitc.func.content_get = content_get;
             itc->eitc.func.state_get = state_get;
             itc->eitc.func.del = del;
             itc->genlist = genlist;
             elm_genlist_item_append(genlist->get(), &itc->eitc, itc, NULL,
                                        ELM_GENLIST_ITEM_NONE,
                                        sel, itc);
          }
        return Undefined();
     }

   virtual Handle<Value> multi_select_get() const
     {
        return Boolean::New(elm_genlist_multi_select_get(eo));
     }

   virtual void multi_select_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_genlist_multi_select_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> reorder_mode_get() const
     {
        return Boolean::New(elm_genlist_reorder_mode_get(eo));
     }

   virtual void reorder_mode_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_genlist_reorder_mode_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> height_for_width_mode_get() const
     {
        return Boolean::New(elm_genlist_height_for_width_mode_get(eo));
     }

   virtual void height_for_width_mode_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_genlist_height_for_width_mode_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> compress_mode_get() const
     {
        return Boolean::New(elm_genlist_compress_mode_get(eo));
     }

   virtual void compress_mode_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_genlist_compress_mode_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> no_select_mode_get() const
     {
        return Boolean::New(elm_genlist_no_select_mode_get(eo));
     }

   virtual void no_select_mode_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_genlist_no_select_mode_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> always_select_mode_get() const
     {
        return Boolean::New(elm_genlist_always_select_mode_get(eo));
     }

   virtual void always_select_mode_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_genlist_always_select_mode_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> block_count_get() const
     {
        return Number::New(elm_genlist_block_count_get(eo));
     }

   virtual void block_count_set(Handle<Value> value)
     {
        if (value->IsNumber())
          elm_genlist_block_count_set(eo, value->IntegerValue());
     }

   virtual Handle<Value>longpress_timeout_get() const
     {
        return Number::New(elm_genlist_longpress_timeout_get(eo));
     }

   virtual void longpress_timeout_set(Handle<Value> value)
     {
        if (value->IsNumber())
          elm_genlist_longpress_timeout_set(eo, value->IntegerValue());
     }
};

template<> CEvasObject::CPropHandler<CElmGenList>::property_list
CEvasObject::CPropHandler<CElmGenList>::list[] = {
  PROP_HANDLER(CElmGenList, multi_select),
  PROP_HANDLER(CElmGenList, always_select_mode),
  PROP_HANDLER(CElmGenList, no_select_mode),
  PROP_HANDLER(CElmGenList, compress_mode),
  PROP_HANDLER(CElmGenList, reorder_mode),
  PROP_HANDLER(CElmGenList, height_for_width_mode),
  PROP_HANDLER(CElmGenList, block_count),
  PROP_HANDLER(CElmGenList, longpress_timeout),
  { NULL, NULL, NULL },
};

class CElmList : public CEvasObject {
   FACTORY(CElmList)
protected:
   class Item {
   public:
     Local<Value> on_clicked;
     Handle<Value> label;
     Handle<Value> icon;
     bool disabled;
   };

   class ListItem : public Item {
   public:
     CEvasObject  *icon_left;
     CEvasObject  *icon_right;
     Elm_Object_Item *li;
     Handle<Value> end;
     Handle<Value> tooltip;
   };

protected:
   Persistent<Value> items;
   CPropHandler<CElmList> prop_handler;
   std::list<ListItem*> list;

   const static int LABEL = 1;
   const static int ICON = 2;
   const static int END = 3;
   const static int TOOLTIP = 4;

public:
   CElmList(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
     {
        eo = elm_list_add(parent->top_widget_get());

        construct(eo, obj);
        items_set(obj->Get(String::New("items")));

        get_object()->Set(String::New("append"), FunctionTemplate::New(append)->GetFunction());

        get_object()->Set(String::New("prepend"), FunctionTemplate::New(prepend)->GetFunction());
        get_object()->Set(String::New("get_label"), FunctionTemplate::New(get_label)->GetFunction());
        get_object()->Set(String::New("get_icon"), FunctionTemplate::New(get_icon)->GetFunction());
        get_object()->Set(String::New("get_end"), FunctionTemplate::New(get_end)->GetFunction());
        get_object()->Set(String::New("get_tooltip"), FunctionTemplate::New(get_tooltip)->GetFunction());
        get_object()->Set(String::New("set_label"), FunctionTemplate::New(set_label)->GetFunction());
        get_object()->Set(String::New("set_icon"), FunctionTemplate::New(set_icon)->GetFunction());
        get_object()->Set(String::New("set_end"), FunctionTemplate::New(set_end)->GetFunction());
        get_object()->Set(String::New("set_tooltip"), FunctionTemplate::New(set_tooltip)->GetFunction());
        get_object()->Set(String::New("insert_after"), FunctionTemplate::New(insert_after)->GetFunction());
        get_object()->Set(String::New("insert_before"), FunctionTemplate::New(insert_before)->GetFunction());
        get_object()->Set(String::New("selected_item_get"), FunctionTemplate::New(selected_item_get)->GetFunction());
        get_object()->Set(String::New("selected_item_set"), FunctionTemplate::New(selected_item_set)->GetFunction());
        get_object()->Set(String::New("del"), FunctionTemplate::New(del)->GetFunction());
        get_object()->Set(String::New("num_items"), FunctionTemplate::New(num_items)->GetFunction());
        get_object()->Set(String::New("disabled"), FunctionTemplate::New(num_items)->GetFunction());

     }

   static Handle<Value> append(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmList *list = static_cast<CElmList *>(self);
        if (args[0]->IsObject())
          {
             list->new_item_set(-1, args[0]);
          }
        return Undefined();
     }
   static Handle<Value> prepend(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmList *list = static_cast<CElmList *>(self);
        if (args[0]->IsObject())
          {
             list->new_item_set(0, args[0]);
          }
        return Undefined();
     }
   static Handle<Value> disabled(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmList *list = static_cast<CElmList *>(self);
        if (!list->list.empty() && args[0]->IsNumber() && args[1]->IsBoolean())
          {
              unsigned int val = args[0]->IntegerValue();

              if (val <= list->list.size())
                {
                   std::list<ListItem*>::iterator i = list->list.begin();
                   for ( ; val>0; val--)
                     i++;
                   elm_object_item_disabled_set((*i)->li, args[1]->BooleanValue());
                }
          }
        return Undefined();
     }
   static Handle<Value> get_label(const Arguments& args)
     {
         return get_item(LABEL, args);
     }
   static Handle<Value> get_icon(const Arguments& args)
     {
         return get_item(ICON, args);
     }
   static Handle<Value> get_end(const Arguments& args)
     {
         return get_item(END, args);
     }
   static Handle<Value> get_tooltip(const Arguments& args)
     {
         return get_item(TOOLTIP, args);
     }
   static Handle<Value> get_item(int field, const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmList *list = static_cast<CElmList *>(self);
        if (!list->list.empty() && args[0]->IsNumber())
          {
              unsigned int val = args[0]->IntegerValue();
              if (val <= list->list.size())
                {
                   std::list<ListItem*>::iterator i = list->list.begin();

                   for (;val>0; val--)
                     i++;

                   switch(field)
                     {
                        case LABEL:
                           return (*i)->label;
                        case ICON:
                           return (*i)->icon;
                        case END:
                           return (*i)->end;
                        case TOOLTIP:
                           return (*i)->tooltip;
                        default:
                           return Undefined();
                     }
                }
          }
        return Undefined();
     }

   static Handle<Value> set_label(const Arguments& args)
     {
         return set_item(LABEL, args);
     }
   static Handle<Value> set_icon(const Arguments& args)
     {
         return set_item(ICON, args);
     }
   static Handle<Value> set_end(const Arguments& args)
     {
         return set_item(END, args);
     }
   static Handle<Value> set_tooltip(const Arguments& args)
     {
         return set_item(TOOLTIP, args);
     }
   static Handle<Value> set_item(int field, const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmList *list = static_cast<CElmList *>(self);
        if (!list->list.empty() && args[0]->IsNumber())
          {
             unsigned int val = args[0]->IntegerValue();
             if (val <= list->list.size())
               {
                  std::list<ListItem*>::iterator i = list->list.begin();

                  for (;val>0; val--)
                    i++;
                  ListItem *it = *i;

                  switch(field)
                    {
                       case LABEL:
                          if ( args[1]->IsString())
                            {
                               static_cast<Persistent<Value> >(it->label).Dispose();
                               it->label = v8::Persistent<Value>::New(args[1]->ToString());
                               String::Utf8Value str(it->label->ToString());
                               elm_object_item_text_set(it->li, *str);
                           }
                           break;
                       case ICON:
                          if ( args[1]->IsObject())
                            {
                               static_cast<Persistent<Value> >(it->icon).Dispose();
                               it->icon = v8::Persistent<Value>::New(args[1]);
                               it->icon_left = make_or_get(list, it->icon);
                               if (it->icon_left)
                                 {
                                    elm_icon_scale_set(it->icon_left->get(), 0, 0);
                                    evas_object_size_hint_align_set(it->icon_left->get(), 0.0, 0.0);
                                    elm_object_item_part_content_set(it->li, NULL, it->icon_left->get());
                                 }
                            }
                          break;
                       case END:
                          if ( args[1]->IsObject())
                            {
                               static_cast<Persistent<Value> >(it->end).Dispose();
                               it->end = v8::Persistent<Value>::New(args[1]);
                               it->icon_right = make_or_get(list, it->end);

                               if (it->icon_right)
                                 {
                                    elm_icon_scale_set(it->icon_right->get(), 0, 0);
                                    evas_object_size_hint_align_set(it->icon_right->get(), 0.0, 0.0);
                                    elm_object_item_part_content_set(it->li, "end", it->icon_right->get());
                                 }
                            }
                          break;
                       case TOOLTIP:
                          if (args[1]->IsString())
                            {
                               static_cast<Persistent<Value> >(it->tooltip).Dispose();
                               it->tooltip = v8::Persistent<Value>::New(args[1]->ToString());
                               String::Utf8Value str(it->tooltip->ToString());
                               elm_object_tooltip_text_set(elm_object_item_widget_get(it->li), *str);
                            }
                          break;
                       default:
                          return Undefined();
                    }
                  elm_list_go(list->get());
               }
          }
        return Undefined();
     }
   static Handle<Value> insert_after(const Arguments&)
     {
        return Undefined();
     }
   static Handle<Value> insert_before(const Arguments&)
     {
        return Undefined();
     }
   static Handle<Value> selected_item_get(const Arguments&)
     {
        return Undefined();
     }
   static Handle<Value> selected_item_set(const Arguments&)
     {
        return Undefined();
     }
   static Handle<Value> del(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmList *list = static_cast<CElmList *>(self);

        if (!list->list.empty() && args[0]->IsNumber())
          {
             int val = args[0]->IntegerValue();

             if (val == -1) // delete last one
               val = list->list.size();

             if (val < (int) list->list.size())
               {
                  std::list<ListItem*>::iterator i = list->list.begin();

                  for (; val > 0; val--)
                    i++;

                  ListItem *it = *i;

                  elm_object_item_del(it->li);
                  elm_list_go(list->get());

                  list->list.erase(i);

                  delete it;
               }
          }
        return Undefined();
     }
   static Handle<Value> num_items(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmList *list = static_cast<CElmList *>(self);
        return v8::Number::New(list->list.size());
     }

   static void eo_on_click(void *data, Evas_Object *, void *)
     {
       if (data)
         {
            ListItem *it = static_cast<ListItem *>(data);

            if (*it->on_clicked != NULL)
              {
                 if (it->on_clicked->IsFunction())
                   {
                      Handle<Function> fn(Function::Cast(*(it->on_clicked)));
                      Local<Object> obj = Object::New();
                      obj->Set(String::New("label"), it->label);
                      obj->Set(String::New("icon"), it->icon);
                      obj->Set(String::New("end"), it->end);
                      obj->Set(String::New("tooltip"), it->tooltip);
                      fn->Call(obj, 0, NULL);
                   }
              }
         }
     }

   virtual Handle<Value> items_get(void) const
     {
        return items;
     }

   virtual void items_set(Handle<Value> val)
     {
        if (!val->IsObject())
          {
             ELM_ERR( "not an object!");
             return;
          }

        Local<Object> in = val->ToObject();
        Local<Array> props = in->GetPropertyNames();

        items.Dispose();
        items = Persistent<Value>::New(val);

        /* iterate through elements and instantiate them */
        // there can be no elements in the list
        for (unsigned int i = 0; i < props->Length(); i++)
          {
             Local<Value> x = props->Get(Integer::New(i));
             String::Utf8Value val(x);

             Local<Value> item = in->Get(x->ToString());

             // -1 means end of list
             ListItem *it = new_item_set(-1, item);
             if (it!=NULL)
             {
                ELM_INF( "New list item added.");
             }
          }

     }

   /*
    * -1 = end of list
    *  >=0 appropriate index
    */

   virtual ListItem * new_item_set(int pos, Handle<Value> item)
     {
        if (!item->IsObject())
          {
             // FIXME: permit adding strings here?
             ELM_ERR( "list item is not an object");
             return NULL;
          }

        if (items==Null())
          {
             ELM_ERR( "Please add atleast empty \"items\" to list");
             return NULL;
          }

        ListItem *it = list.front();
        if ((pos == 0) || (pos==-1)) //seperate insert and removal case
          {
             it = new ListItem();

             it->label = v8::Persistent<Value>::New(item->ToObject()->Get(String::New("label")));
             it->icon = v8::Persistent<Value>::New(item->ToObject()->Get(String::New("icon")));
             it->end = v8::Persistent<Value>::New(item->ToObject()->Get(String::New("end")));
             it->tooltip = v8::Persistent<Value>::New(item->ToObject()->Get(String::New("tooltip")));

             if ( !it->label->IsString() && !it->icon->IsObject()
                      && !it->end->IsObject())
               {

                  ELM_ERR( "Basic elements missing");
                  delete it;
                  return NULL;
               }
          }

        if (-1 == pos)
          {
             // either a label with icon
             it->li = elm_list_item_append(eo,NULL,NULL,NULL,&eo_on_click,(void*)it);
             list.push_back(it);
          }
        else if (0 == pos)
          {
             // either a label with icon
             it->li = elm_list_item_prepend(eo,NULL,NULL,NULL,&eo_on_click,(void*)it);
             list.push_front(it);
          }
        else
          {
             // get the Eina_List
             const Eina_List *iter = elm_list_items_get (get());
             std::list<ListItem*>::iterator i = list.begin();

             for (;pos>0; pos--)
               {
                  i++;
                  iter = iter->next;
               }
             it->label = v8::Persistent<Value>::New(item->ToObject()->Get(String::New("label")));
             it->icon = v8::Persistent<Value>::New(item->ToObject()->Get(String::New("icon")));
             it->end = v8::Persistent<Value>::New(item->ToObject()->Get(String::New("end")));
             it->tooltip = v8::Persistent<Value>::New(item->ToObject()->Get(String::New("tooltip")));
             list.insert(i, it);
          }

        if ( it->label->IsString())
          {
             String::Utf8Value str(it->label->ToString());
             elm_object_item_text_set(it->li, *str);
          }
        if ( it->icon->IsObject())
          {
             it->icon_left = make_or_get(this, it->icon);
             if (it->icon_left)
               {
                  elm_icon_scale_set(it->icon_left->get(), 0, 0);
                  evas_object_size_hint_align_set(it->icon_left->get(), 0.0, 0.0);
                  elm_object_item_part_content_set(it->li, "left", it->icon_left->get());
               }
          }
        if ( it->end->IsObject())
          {
             it->icon_right = make_or_get(this, it->end);

             if (it->icon_right)
               {
                  elm_icon_scale_set(it->icon_right->get(), 0, 0);
                  evas_object_size_hint_align_set(it->icon_right->get(), 0.0, 0.0);
                  elm_object_item_part_content_set(it->li, "right", it->icon_right->get());
               }
          }
        if (it->tooltip->IsString())
          {
             String::Utf8Value str(it->tooltip->ToString());
             elm_object_tooltip_text_set(elm_object_item_widget_get(it->li), *str);
          }

        if (item->ToObject()->Get(String::New("on_clicked"))->IsFunction())
          {
             it->on_clicked = Local<Value>::New(
                       item->ToObject()->Get(String::New("on_clicked")));
          }
        elm_list_go(eo);
        return it;
     }

   virtual Handle<Value> mode_get() const
     {
        int mode = elm_list_mode_get(eo);
        return Number::New(mode);
     }

   virtual void mode_set(Handle<Value> value)
     {
        if (value->IsNumber())
          {
             int mode = value->NumberValue();
             elm_list_mode_set(eo, (Elm_List_Mode)mode);
          }
     }
};

template<> CEvasObject::CPropHandler<CElmList>::property_list
CEvasObject::CPropHandler<CElmList>::list[] = {
  PROP_HANDLER(CElmList, mode),
  { NULL, NULL, NULL },
};

class CElmEntry : public CEvasObject {
   FACTORY(CElmEntry)
protected:
   CPropHandler<CElmEntry> prop_handler;
public:
   CElmEntry(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
     {
        eo = elm_entry_add(parent->get());
        construct(eo, obj);
     }

   virtual Handle<Value> password_get() const
     {
        return Boolean::New(elm_entry_password_get(eo));
     }

   virtual void password_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_entry_password_set(eo, value->BooleanValue());
     }
   virtual Handle<Value> editable_get() const
     {
        return Boolean::New(elm_entry_editable_get(eo));
     }

   virtual void editable_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_entry_editable_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> line_wrap_get() const
     {
        return Integer::New(elm_entry_line_wrap_get(eo));
     }

   virtual void line_wrap_set(Handle<Value> value)
     {
        if (value->IsNumber())
          elm_entry_line_wrap_set(eo, (Elm_Wrap_Type)value->Int32Value());
     }

   virtual Handle<Value> scrollable_get() const
     {
        return Boolean::New(elm_entry_scrollable_get(eo));
     }

   virtual void scrollable_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_entry_scrollable_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> single_line_get() const
     {
        return Integer::New(elm_entry_single_line_get(eo));
     }

   virtual void single_line_set(Handle<Value> value)
     {
        if (value->IsNumber())
          elm_entry_single_line_set(eo, (Elm_Wrap_Type)value->Int32Value());
     }
};

template<> CEvasObject::CPropHandler<CElmEntry>::property_list
CEvasObject::CPropHandler<CElmEntry>::list[] = {
  PROP_HANDLER(CElmEntry, password),
  PROP_HANDLER(CElmEntry, editable),
  PROP_HANDLER(CElmEntry, line_wrap),
  PROP_HANDLER(CElmEntry, scrollable),
  PROP_HANDLER(CElmEntry, single_line),
  { NULL, NULL, NULL },
};

class CElmCheck : public CEvasObject {
   FACTORY(CElmCheck)
protected:
   CPropHandler<CElmCheck> prop_handler;
   Persistent<Value> the_icon;

public:
   CElmCheck(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
     {
        eo = elm_check_add(parent->get());
        construct(eo, obj);
     }

   virtual ~CElmCheck()
     {
        the_icon.Dispose();
     }

   virtual void state_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_check_state_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> state_get() const
     {
        return Boolean::New(elm_check_state_get(eo));
     }

   virtual Handle<Value> icon_get() const
     {
        return the_icon;
     }

   virtual void icon_set(Handle<Value> value)
     {
        the_icon.Dispose();
        CEvasObject *icon = make_or_get(this, value);
        elm_object_content_set(eo, icon->get());
        the_icon = Persistent<Value>::New(icon->get_object());
     }
};

template<> CEvasObject::CPropHandler<CElmCheck>::property_list
CEvasObject::CPropHandler<CElmCheck>::list[] = {
  PROP_HANDLER(CElmCheck, state),
  PROP_HANDLER(CElmCheck, icon),
  { NULL, NULL, NULL },
};

class CElmProgressBar : public CEvasObject {
   FACTORY(CElmProgressBar)
protected:
   CPropHandler<CElmProgressBar> prop_handler;
   Persistent<Value> the_icon;

   static Handle<Value> do_pulse(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmProgressBar *progress = static_cast<CElmProgressBar *>(self);
        if (args[0]->IsBoolean())
          progress->pulse(args[0]->BooleanValue());
        return Undefined();
     }

public:
   CElmProgressBar(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
     {
        eo = elm_progressbar_add(parent->get());
        construct(eo, obj);
        get_object()->Set(String::New("pulse"), FunctionTemplate::New(do_pulse)->GetFunction());
     }

   virtual ~CElmProgressBar()
     {
        the_icon.Dispose();
     }

   virtual void pulse(bool on)
     {
        elm_progressbar_pulse(eo, on);
     }

   virtual Handle<Value> icon_get() const
     {
        return the_icon;
     }

   virtual void icon_set(Handle<Value> value)
     {
        the_icon.Dispose();
        CEvasObject *icon = make_or_get(this, value);
        elm_object_content_set(eo, icon->get());
        the_icon = Persistent<Value>::New(icon->get_object());
     }

   virtual Handle<Value> inverted_get() const
     {
        return Boolean::New(elm_progressbar_inverted_get(eo));
     }

   virtual void inverted_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_progressbar_inverted_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> horizontal_get() const
     {
        return Boolean::New(elm_progressbar_horizontal_get(eo));
     }

   virtual void horizontal_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_progressbar_horizontal_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> units_get() const
     {
        return String::New(elm_progressbar_unit_format_get(eo));
     }

   virtual void units_set(Handle<Value> value)
     {
        if (value->IsString())
          {
             String::Utf8Value str(value);
             elm_progressbar_unit_format_set(eo, *str);
          }
     }

   virtual Handle<Value> span_get() const
     {
        return Integer::New(elm_progressbar_span_size_get(eo));
     }

   virtual void span_set(Handle<Value> value)
     {
        if (value->IsInt32())
          {
             int span = value->Int32Value();
             elm_progressbar_span_size_set(eo, span);
          }
     }

   virtual Handle<Value> pulser_get() const
     {
        return Boolean::New(elm_progressbar_pulse_get(eo));
     }

   virtual void pulser_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_progressbar_pulse_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> value_get() const
     {
        return Number::New(elm_progressbar_value_get(eo));
     }

   virtual void value_set(Handle<Value> value)
     {
        if (value->IsNumber())
          elm_progressbar_value_set(eo, value->NumberValue());
     }
};

template<> CEvasObject::CPropHandler<CElmProgressBar>::property_list
CEvasObject::CPropHandler<CElmProgressBar>::list[] = {
  PROP_HANDLER(CElmProgressBar, icon),
  PROP_HANDLER(CElmProgressBar, inverted),
  PROP_HANDLER(CElmProgressBar, horizontal),
  PROP_HANDLER(CElmProgressBar, units),
  PROP_HANDLER(CElmProgressBar, span),
  PROP_HANDLER(CElmProgressBar, pulser),
  PROP_HANDLER(CElmProgressBar, value),
  { NULL, NULL, NULL },
};


class CElmPhoto : public CEvasObject {
   FACTORY(CElmPhoto)
protected:
  CPropHandler<CElmPhoto> prop_handler;

public:
  CElmPhoto(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
    {
       eo = elm_photo_add(parent->top_widget_get());
       construct(eo, obj);
    }

  virtual ~CElmPhoto()
    {
    }

  virtual Handle<Value> image_get() const
    {
       //No getter available
       return Undefined();
    }

  virtual void image_set(Handle<Value> val)
    {
       if (val->IsString())
         {
            String::Utf8Value str(val);

            if (0 > access(*str, R_OK))
              ELM_ERR( "warning: can't read image file %s", *str);

            Eina_Bool retval = elm_photo_file_set(eo, *str);
            if (retval == EINA_FALSE)
              ELM_ERR( "Unable to set the image");
         }
    }

  virtual Handle<Value> size_get() const
    {
       //No getter available
       return Undefined();
    }

  virtual void size_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int size = val->ToInt32()->Value();
            elm_photo_size_set(eo, size);
         }
    }

  virtual Handle<Value> fill_get() const
    {
       //No getter available
       return Undefined();
    }

  virtual void fill_set(Handle<Value> val)
    {
       if (val->IsBoolean())
         elm_photo_fill_inside_set(eo, val->BooleanValue());
    }
};

template<> CEvasObject::CPropHandler<CElmPhoto>::property_list
CEvasObject::CPropHandler<CElmPhoto>::list[] = {
  PROP_HANDLER(CElmPhoto, size),
  PROP_HANDLER(CElmPhoto, fill),
  { NULL, NULL, NULL },
};

class CElmSpinner : public CEvasObject {
   FACTORY(CElmSpinner)
protected:
  CPropHandler<CElmSpinner> prop_handler;

public:
  CElmSpinner(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
    {
       eo = elm_spinner_add(parent->top_widget_get());
       construct(eo, obj);
    }

  virtual ~CElmSpinner()
    {
    }

  virtual Handle<Value> label_format_get() const
    {
       return String::New(elm_spinner_label_format_get(eo));
    }

  virtual void label_format_set(Handle<Value> val)
    {
       if (val->IsString())
         {
            String::Utf8Value str(val);

            elm_spinner_label_format_set(eo, *str);
         }
    }

  virtual Handle<Value> step_get() const
    {
       return Number::New(elm_spinner_step_get(eo));
    }

  virtual void step_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int size = val->ToNumber()->Value();
            elm_spinner_step_set(eo, size);
         }
    }

   virtual Handle<Value> min_get() const
     {
        double min, max;
        elm_spinner_min_max_get(eo, &min, &max);
        return Number::New(min);
     }

   virtual void min_set(Handle<Value> value)
     {
        if (value->IsNumber())
          {
             double min, max;
             elm_spinner_min_max_get(eo, &min, &max);
             min = value->NumberValue();
             elm_spinner_min_max_set(eo, min, max);
          }
     }

   virtual Handle<Value> max_get() const
     {
        double min, max;
        elm_spinner_min_max_get(eo, &min, &max);
        return Number::New(max);
     }

   virtual void max_set(Handle<Value> value)
     {
        if (value->IsNumber())
          {
             double min, max;
             elm_spinner_min_max_get(eo, &min, &max);
             max = value->NumberValue();
             elm_spinner_min_max_set(eo, min, max);
          }
     }
  virtual Handle<Value> style_get() const
    {
       return String::New(elm_object_style_get(eo));
    }

  virtual void style_set(Handle<Value> val)
    {
       if (val->IsString())
         {
            String::Utf8Value str(val);
            elm_object_style_set(eo, *str);
         }
    }

  virtual Handle<Value> editable_get() const
    {
       return Number::New(elm_spinner_editable_get(eo));
    }

  virtual void editable_set(Handle<Value> val)
    {
       if (val->IsBoolean())
         {
            elm_spinner_editable_set(eo, val->BooleanValue());
         }
    }

  virtual Handle<Value> disabled_get() const
    {
       return Boolean::New(elm_spinner_editable_get(eo));
    }

  virtual void disabled_set(Handle<Value> val)
    {
       if (val->IsBoolean())
         {
            elm_object_disabled_set(eo, val->BooleanValue());
         }
    }

  virtual Handle<Value> special_value_get() const
    {
       //No getter available
       return Undefined();
    }

  virtual void special_value_set(Handle<Value> val)
    {
       if (val->IsObject())
         {
             Local<Value> value = val->ToObject()->Get(String::New("value"));
             Local<Value> label = val->ToObject()->Get(String::New("label"));
             String::Utf8Value str(label);
             int int_value = value->ToInt32()->Value();
             elm_spinner_special_value_add(eo, int_value, *str);
         }
    }
};

template<> CEvasObject::CPropHandler<CElmSpinner>::property_list
CEvasObject::CPropHandler<CElmSpinner>::list[] = {
  PROP_HANDLER(CElmSpinner, label_format),
  PROP_HANDLER(CElmSpinner, step),
  PROP_HANDLER(CElmSpinner, min),
  PROP_HANDLER(CElmSpinner, max),
  PROP_HANDLER(CElmSpinner, style),
  PROP_HANDLER(CElmSpinner, disabled),
  PROP_HANDLER(CElmSpinner, editable),
  PROP_HANDLER(CElmSpinner, special_value),
  { NULL, NULL, NULL },
};

class CElmPane : public CEvasObject {
   FACTORY(CElmPane)
protected:
  CPropHandler<CElmPane> prop_handler;

public:
  CElmPane(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
    {
       eo = elm_panes_add(parent->top_widget_get());
       construct(eo, obj);
       CEvasObject *left, *right;
       left = make_or_get(this, obj->Get(String::New("content_left")));
       if (left)
         {
            elm_object_part_content_set(eo, "elm.swallow.left", left->get());
         }

       right = make_or_get(this, obj->Get(String::New("content_right")));
       if (right)
         {
            elm_object_part_content_set(eo, "elm.swallow.right", right->get());
         }
    }

  virtual ~CElmPane()
    {
    }

  virtual Handle<Value> horizontal_get() const
    {
       return Number::New(elm_panes_horizontal_get(eo));
    }

  virtual void horizontal_set(Handle<Value> val)
    {
       if (val->IsBoolean())
         {
            elm_panes_horizontal_set(eo, val->BooleanValue());
         }
    }

   virtual void on_press_set(Handle<Value> val)
     {
        on_clicked_set(val);
     }

   virtual Handle<Value> on_press_get(void) const
     {
        return on_clicked_val;
     }

};

template<> CEvasObject::CPropHandler<CElmPane>::property_list
CEvasObject::CPropHandler<CElmPane>::list[] = {
  PROP_HANDLER(CElmPane, horizontal),
  PROP_HANDLER(CElmPane, on_press),
  { NULL, NULL, NULL },
};

class CElmBubble : public CEvasObject {
   FACTORY(CElmBubble)
protected:
  CPropHandler<CElmBubble> prop_handler;

public:
  CElmBubble(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
    {
       eo = elm_bubble_add(parent->top_widget_get());
       construct(eo, obj);
       CEvasObject *content;
       content = make_or_get(this, obj->Get(String::New("content")));
       if ( content )
         {
            elm_object_content_set(eo, content->get());
         }
    }

  virtual ~CElmBubble()
    {
    }

  virtual Handle<Value> text_part_get() const
    {
       return Undefined();
    }

  virtual void text_part_set(Handle<Value> val)
    {
       if (!val->IsObject())
         {
            ELM_ERR( "%s: value is not an object!", __FUNCTION__);
            return;
         }
       Local<Object> obj = val->ToObject();
       Local<Value> it = obj->Get(String::New("item"));
       Local<Value> lbl = obj->Get(String::New("label"));
       String::Utf8Value item(it);
       String::Utf8Value label(lbl);
       elm_object_part_text_set(eo, *item,*label);
    }


  virtual Handle<Value> corner_get() const
    {
       return String::New(elm_bubble_corner_get(eo));
    }

  virtual void corner_set(Handle<Value> val)
    {
       if (val->IsString())
         {
            String::Utf8Value str(val);
            elm_bubble_corner_set(eo, *str);
         }
    }

};

template<> CEvasObject::CPropHandler<CElmBubble>::property_list
CEvasObject::CPropHandler<CElmBubble>::list[] = {
  PROP_HANDLER(CElmBubble, text_part),
  PROP_HANDLER(CElmBubble, corner),
  { NULL, NULL, NULL },
};

class CElmSegment : public CEvasObject {
   FACTORY(CElmSegment)
protected:
  CPropHandler<CElmSegment> prop_handler;

public:
   CElmSegment(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
     {
        eo = elm_segment_control_add(parent->get());
        construct(eo, obj);
        //items_set(obj->Get(String::New("items")));
     }

   Handle<Object> items_set(Handle<Value> val)
     {
        /* add an list of children */
        Local<Object> out = Object::New();

        if (!val->IsObject())
          {
             ELM_ERR( "not an object!");
             return out;
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
                  // FIXME: permit adding strings here?
                  ELM_ERR( "list item is not an object");
                  continue;
               }
             Local<Value> label = item->ToObject()->Get(String::New("label"));

             String::Utf8Value str(label);
             elm_segment_control_item_add(eo, NULL, *str);
          }

        return out;
     }

   virtual ~CElmSegment()
     {
     }

};

template<> CEvasObject::CPropHandler<CElmSegment>::property_list
CEvasObject::CPropHandler<CElmSegment>::list[] = {
  { NULL, NULL, NULL },
};

class CElmMenu : public CEvasObject {
   FACTORY(CElmMenu)
protected:
  CPropHandler<CElmMenu> prop_handler;

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

  MenuItem *root;

public:
  CElmMenu(CEvasObject *par, Local<Object> obj) :
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

  virtual ~CElmMenu()
    {
    }
   static Handle<Value> addchild(const Arguments&)
     {
        return Undefined();
     }

   static Handle<Value> parent(const Arguments&)
     {
        return Undefined();
     }

   static Handle<Value> child(const Arguments&)
     {
        return Undefined();
     }

   static Handle<Value> child_count(const Arguments&)
     {
        return Undefined();
     }

   static void eo_on_click(void *data, Evas_Object *, void *)
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

   void items_set(MenuItem *parent, Handle<Value> val)
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

   virtual MenuItem * new_item_set(MenuItem *parent, Handle<Value> item)
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

  virtual Handle<Value> move_get() const
    {
       return Undefined();
    }

  virtual void move_set(Handle<Value> val)
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
};

template<> CEvasObject::CPropHandler<CElmMenu>::property_list
CEvasObject::CPropHandler<CElmMenu>::list[] = {
  PROP_HANDLER(CElmMenu, move),
  { NULL, NULL, NULL },
};

class CElmColorSelector : public CEvasObject {
   FACTORY(CElmColorSelector)
protected:
   CPropHandler<CElmColorSelector> prop_handler;
   /* the on_clicked function */
   Persistent<Value> on_changed_val;

public:
  CElmColorSelector(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
    {
       eo = elm_colorselector_add(parent->top_widget_get());
       construct(eo, obj);
    }

  virtual ~CElmColorSelector()
    {
    }

  virtual Handle<Value> red_get() const
    {
       int r, g, b, a;
       elm_colorselector_color_get(eo, &r, &g, &b, &a);
       return Number::New(r);
    }

  virtual void red_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int r, g, b, a;
            elm_colorselector_color_get(eo, &r, &g, &b, &a);
            r = val->ToNumber()->Value();
            elm_colorselector_color_set(eo, r, g, b, a);
         }
    }

  virtual Handle<Value> green_get() const
    {
       int r, g, b, a;
       elm_colorselector_color_get(eo, &r, &g, &b, &a);
       return Number::New(g);
    }

  virtual void green_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int r, g, b, a;
            elm_colorselector_color_get(eo, &r, &g, &b, &a);
            g = val->ToNumber()->Value();
            elm_colorselector_color_set(eo, r, g, b, a);
         }
    }
  virtual Handle<Value> blue_get() const
    {
       int r, g, b, a;
       elm_colorselector_color_get(eo, &r, &g, &b, &a);
       return Number::New(b);
    }

  virtual void blue_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int r, g, b, a;
            elm_colorselector_color_get(eo, &r, &g, &b, &a);
            b = val->ToNumber()->Value();
            elm_colorselector_color_set(eo, r, g, b, a);
         }
    }
  virtual Handle<Value> alpha_get() const
    {
       int r, g, b, a;
       elm_colorselector_color_get(eo, &r, &g, &b, &a);
       return Number::New(a);
    }

  virtual void alpha_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int r, g, b, a;
            elm_colorselector_color_get(eo, &r, &g, &b, &a);
            a = val->ToNumber()->Value();
            elm_colorselector_color_set(eo, r, g, b, a);
         }
    }
   virtual void on_changed(void *)
     {
        Handle<Object> obj = get_object();
        HandleScope handle_scope;
        Handle<Value> val = on_changed_val;
        assert(val->IsFunction());
        Handle<Function> fn(Function::Cast(*val));
        Handle<Value> args[1] = { obj };
        fn->Call(obj, 1, args);
     }

   static void eo_on_changed(void *data, Evas_Object *, void *event_info)
     {
        CElmColorSelector *changed = static_cast<CElmColorSelector*>(data);
        changed->on_changed(event_info);
     }

   virtual void on_changed_set(Handle<Value> val)
     {
        on_changed_val.Dispose();
        on_changed_val = Persistent<Value>::New(val);
        if (val->IsFunction())
          evas_object_smart_callback_add(eo, "changed", &eo_on_changed, this);
        else
          evas_object_smart_callback_del(eo, "changed", &eo_on_changed);
     }

   virtual Handle<Value> on_changed_get(void) const
     {
        return on_changed_val;
     }

};

template<> CEvasObject::CPropHandler<CElmColorSelector>::property_list
CEvasObject::CPropHandler<CElmColorSelector>::list[] = {
  PROP_HANDLER(CElmColorSelector, red),
  PROP_HANDLER(CElmColorSelector, green),
  PROP_HANDLER(CElmColorSelector, blue),
  PROP_HANDLER(CElmColorSelector, alpha),
  PROP_HANDLER(CElmColorSelector, on_changed),
  { NULL, NULL, NULL },
};

class CElmCalendar : public CEvasObject {
   FACTORY(CElmCalendar)
protected:
   CPropHandler<CElmCalendar> prop_handler;
   /* the on_clicked function */
   Persistent<Value> on_changed_val;

public:
  CElmCalendar(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
    {
       eo = elm_calendar_add(parent->top_widget_get());
       construct(eo, obj);
       marks_set(obj->Get(String::New("marks")));
    }

  virtual ~CElmCalendar()
    {
    }

   Handle<Object> marks_set(Handle<Value> val)
     {
        /* add an list of children */
        Local<Object> out = Object::New();

        if (!val->IsObject())
          {
             ELM_ERR( "not an object!");
             return out;
          }

        Local<Object> in = val->ToObject();
        Local<Array> props = in->GetPropertyNames();
        struct tm mark_time;

        /* iterate through elements and instantiate them */
        for (unsigned int i = 0; i < props->Length(); i++)
          {

             Local<Value> x = props->Get(Integer::New(i));
             String::Utf8Value val(x);

             Local<Value> item = in->Get(x->ToString());
             if (!item->IsObject())
               {
                  String::Utf8Value xval(x->ToString());
                  ELM_ERR( "item is not an object %s", *xval);
                  continue;
               }

             Local<Value> type = item->ToObject()->Get(String::New("mark_type"));
             String::Utf8Value mark_type(type);
             Local<Value> date = item->ToObject()->Get(String::New("mark_date"));
             mark_time.tm_mday = date->ToNumber()->Value();
             Local<Value> mon = item->ToObject()->Get(String::New("mark_mon"));
             mark_time.tm_mon = mon->ToNumber()->Value();
             Local<Value> year = item->ToObject()->Get(String::New("mark_year"));
             mark_time.tm_year = year->ToNumber()->Value() - 1900;
             Local<Value> repeat = item->ToObject()->Get(String::New("mark_repeat"));
             String::Utf8Value mark_repeat(repeat);
             Elm_Calendar_Mark_Repeat intRepeat = ELM_CALENDAR_UNIQUE;

             if ( !strcasecmp(*mark_repeat, "annually"))
               intRepeat = ELM_CALENDAR_ANNUALLY;
             else if ( !strcasecmp(*mark_repeat, "monthly"))
               intRepeat = ELM_CALENDAR_MONTHLY;
             else if ( !strcasecmp(*mark_repeat, "weekly"))
               intRepeat = ELM_CALENDAR_WEEKLY;
             else if ( !strcasecmp(*mark_repeat, "daily"))
               intRepeat = ELM_CALENDAR_DAILY;

             elm_calendar_mark_add(eo, *mark_type, &mark_time, intRepeat);
             elm_calendar_marks_draw (eo);
          }

        return out;
     }

   virtual void on_changed(void *)
     {
        Handle<Object> obj = get_object();
        HandleScope handle_scope;
        Handle<Value> val = on_changed_val;
        assert(val->IsFunction());
        Handle<Function> fn(Function::Cast(*val));
        Handle<Value> args[1] = { obj };
        fn->Call(obj, 1, args);
     }

   static void eo_on_changed(void *data, Evas_Object *, void *event_info)
     {
        CElmCalendar *changed = static_cast<CElmCalendar*>(data);
        changed->on_changed(event_info);
     }

   virtual void on_changed_set(Handle<Value> val)
     {
        on_changed_val.Dispose();
        on_changed_val = Persistent<Value>::New(val);
        if (val->IsFunction())
          {
             evas_object_smart_callback_add(eo, "changed", &eo_on_changed, this);
          }
        else
          evas_object_smart_callback_del(eo, "changed", &eo_on_changed);
     }

   virtual Handle<Value> on_changed_get(void) const
     {
        return on_changed_val;
     }

   virtual Handle<Value> weekday_names_get(void) const
     {
        Local<Object> obj = Object::New();

        const char **wds = elm_calendar_weekdays_names_get(eo);
        obj->Set(String::New("0"), String::New(wds[0]));
        obj->Set(String::New("1"), String::New(wds[1]));
        obj->Set(String::New("2"), String::New(wds[2]));
        obj->Set(String::New("3"), String::New(wds[3]));
        obj->Set(String::New("4"), String::New(wds[4]));
        obj->Set(String::New("5"), String::New(wds[5]));
        obj->Set(String::New("6"), String::New(wds[6]));

        return obj;
     }

   virtual void weekday_names_set(Handle<Value> val)
     {
        if ( val->IsObject() )
          {
             const char *weekdays[7];
             Local<Object> obj = val->ToObject();

             for (int i = 0; i < 7 ; i++)
               {
                  char fill[2];
                  sprintf(fill, "%d", i);
                  Handle<Value> value = obj->Get(String::New(fill));
                  if ( value->IsString())
                    {
                       String::Utf8Value str(value);
                       weekdays[i] = strdup(*str);
                    }
               }
             elm_calendar_weekdays_names_set(eo,weekdays);
          }
     }

   virtual Handle<Value> min_year_get(void) const
     {
        int year_min, year_max;
        elm_calendar_min_max_year_get(eo, &year_min, &year_max);
        return Number::New(year_min);
     }

   virtual void min_year_set(Handle<Value> val)
     {
        if ( val->IsNumber() )
          {
             int year_min, year_max;
             elm_calendar_min_max_year_get(eo, &year_min, &year_max);
             year_min = val->ToNumber()->Value();
             elm_calendar_min_max_year_set(eo, year_min, year_max);
          }
     }

   virtual Handle<Value> max_year_get(void) const
     {
        int year_min, year_max;
        elm_calendar_min_max_year_get(eo, &year_min, &year_max);
        return Number::New(year_max);
     }

   virtual void max_year_set(Handle<Value> val)
     {
        if ( val->IsNumber() )
          {
             int year_min, year_max;
             elm_calendar_min_max_year_get(eo, &year_min, &year_max);
             year_max = val->ToNumber()->Value();
             elm_calendar_min_max_year_set(eo, year_min, year_max);
          }
     }

   virtual Handle<Value> day_selection_enabled_get(void) const
     {
        Eina_Bool day_select = elm_calendar_day_selection_enabled_get(eo);
        return Boolean::New(day_select);
     }

   virtual void day_selection_enabled_set(Handle<Value> val)
     {
        if ( val->IsBoolean() )
          {
             Eina_Bool day_select = val->ToBoolean()->Value();
             elm_calendar_day_selection_enabled_set(eo, day_select);
          }
     }

   virtual Handle<Value> selected_date_get(void) const
     {
        struct tm selected_time;
        elm_calendar_selected_time_get (eo,&selected_time);
        return Number::New(selected_time.tm_mday);
     }

   virtual void selected_date_set(Handle<Value> val)
     {
        if ( val->IsNumber() )
          {
             struct tm selected_time;
             elm_calendar_selected_time_get (eo,&selected_time);
             selected_time.tm_mday = val->ToNumber()->Value();
             elm_calendar_selected_time_set (eo,&selected_time);
          }
     }

   virtual Handle<Value> selected_month_get(void) const
     {
        struct tm selected_time;
        elm_calendar_selected_time_get (eo,&selected_time);
        return Number::New(selected_time.tm_mon);
     }

   virtual void selected_month_set(Handle<Value> val)
     {
        if ( val->IsNumber() )
          {
             struct tm selected_time;
             elm_calendar_selected_time_get (eo,&selected_time);
             selected_time.tm_mon = val->ToNumber()->Value();
             //tm_mon is zero based - but hide that from user.
             //let them give a normal number
             selected_time.tm_mon = selected_time.tm_mon - 1;
             elm_calendar_selected_time_set (eo,&selected_time);
          }
     }

   virtual Handle<Value> selected_year_get(void) const
     {
        struct tm selected_time;
        elm_calendar_selected_time_get (eo,&selected_time);
        return Number::New(selected_time.tm_year);
     }

   virtual void selected_year_set(Handle<Value> val)
     {
        if ( val->IsNumber() )
          {
             struct tm selected_time;
             elm_calendar_selected_time_get (eo,&selected_time);
             selected_time.tm_year = val->ToNumber()->Value();
             //tm_year is years since 1900 - but hide that from user.
             //let them give a normal year
             selected_time.tm_year = selected_time.tm_year - 1900;
             elm_calendar_selected_time_set (eo,&selected_time);
          }
     }

   virtual Handle<Value> calendar_interval_get(void) const
     {
        return Number::New(elm_calendar_interval_get(eo));
     }

   virtual void calendar_interval_set(Handle<Value> val)
     {
        if ( val->IsNumber() )
          {
             double interval = val->ToNumber()->Value();
             elm_calendar_interval_set(eo, interval);
          }
     }
};

template<> CEvasObject::CPropHandler<CElmCalendar>::property_list
CEvasObject::CPropHandler<CElmCalendar>::list[] = {
     PROP_HANDLER(CElmCalendar, weekday_names),
     PROP_HANDLER(CElmCalendar, min_year),
     PROP_HANDLER(CElmCalendar, max_year),
     PROP_HANDLER(CElmCalendar, day_selection_enabled),
     PROP_HANDLER(CElmCalendar, selected_date),
     PROP_HANDLER(CElmCalendar, selected_month),
     PROP_HANDLER(CElmCalendar, selected_year),
     PROP_HANDLER(CElmCalendar, calendar_interval),
     PROP_HANDLER(CElmCalendar, on_changed),
     { NULL, NULL, NULL },
};

class CElmTable : public CEvasObject {
   FACTORY(CElmTable)
protected:
   CPropHandler<CElmTable> prop_handler;
   std::list<CEvasObject *> table_items;

public:
   CElmTable(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
     {
        eo = elm_table_add(parent->top_widget_get());
        construct(eo, obj);
        get_object()->Set(String::New("unpack"), FunctionTemplate::New(unpack)->GetFunction());
        get_object()->Set(String::New("pack"), FunctionTemplate::New(pack)->GetFunction());
        get_object()->Set(String::New("clear"), FunctionTemplate::New(clear)->GetFunction());
        items_set(obj->Get(String::New("subobjects")));
     }

   static Handle<Value> pack(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmTable *table = static_cast<CElmTable *>(self);
        if (args[0]->IsObject())
          {
             return table->new_item_set(args[0]);
          }
        return Undefined();
     }

   static Handle<Value> unpack(const Arguments&)
     {
        return Undefined();
     }

   static Handle<Value> clear(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmTable *table = static_cast<CElmTable *>(self);
        elm_table_clear(table->get(), true);
        table->table_items.clear();
        return Undefined();
     }

   virtual void items_set(Handle<Value> val)
     {
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

             new_item_set(item);
          }
     }

   virtual Handle<Value> new_item_set(Handle<Value> item)
     {
        CEvasObject *child = NULL;
        if (!item->IsObject())
          {
             // FIXME: permit adding strings here?
             ELM_ERR( "list item is not an object");
             return Undefined();
          }

        Local<Value> subobj = item->ToObject()->Get(String::New("subobject"));

        if ( subobj->IsObject())
          {
             child = make_or_get(this, subobj);
             if(!child)
                return Undefined();
          }
        else
          {
             return Undefined();
          }

        Local<Value> xpos = item->ToObject()->Get(String::New("x"));
        Local<Value> ypos = item->ToObject()->Get(String::New("y"));
        Local<Value> width = item->ToObject()->Get(String::New("w"));
        Local<Value> height = item->ToObject()->Get(String::New("h"));

	if (!xpos->IsNumber() || !ypos->IsNumber() || !width->IsNumber() ||
            !height->IsNumber())
          {
             ELM_ERR("Coordinates not set or not a number? x=%d, y=%d, w=%d or h=%d",
                     xpos->IsNumber(), ypos->IsNumber(), width->IsNumber(),
                     height->IsNumber());
             return Undefined();
          }

        int x,y,w,h;

        x = xpos->IntegerValue();
        y = ypos->IntegerValue();
        w = width->IntegerValue();
        h = height->IntegerValue();

        elm_table_pack(this->get(), child->get(), x, y, w, h);
        ELM_INF("Packing new table item at %d %d %d %d", x,y,w,h);
        table_items.push_back(child);
        return child->get_object();
     }

    void homogeneous_set(Handle<Value> val)
      {
         if (val->IsBoolean())
           elm_table_homogeneous_set(eo, val->BooleanValue());
      }

    virtual Handle<Value> homogeneous_get() const
      {
         return Boolean::New(elm_table_homogeneous_get(eo));
      }

    void padding_set(Handle<Value> val)
      {
         HandleScope handle_scope;
         if (!val->IsObject())
           return;
         Local<Object> obj = val->ToObject();
         Local<Value> x = obj->Get(String::New("x"));
         Local<Value> y = obj->Get(String::New("y"));
         if (!x->IsNumber() || !y->IsNumber())
           return;
         elm_table_padding_set(eo, x->NumberValue(), y->NumberValue());
      }

    virtual Handle<Value> padding_get() const
      {
         Evas_Coord x, y;
         elm_table_padding_get(eo, &x, &y);
         Local<Object> obj = Object::New();
         obj->Set(String::New("x"), Boolean::New(x));
         obj->Set(String::New("y"), Boolean::New(y));
         return obj;
      }
};

template<> CEvasObject::CPropHandler<CElmTable>::property_list
CEvasObject::CPropHandler<CElmTable>::list[] = {
  PROP_HANDLER(CElmTable, homogeneous),
  PROP_HANDLER(CElmTable, padding),
  { NULL, NULL, NULL },
};

#if 0

class CElmPager : public CEvasObject {
   FACTORY(CElmPager)
protected:
   std::list<CEvasObject *> pages;
   CPropHandler<CElmPager> prop_handler;

public:
   CElmPager(CEvasObject *parent, Local<Object> obj) :
       CEvasObject(),
       prop_handler(property_list_base)
     {
        eo = elm_pager_add(parent->top_widget_get());
        construct(eo, obj);
        get_object()->Set(String::New("pop"), FunctionTemplate::New(pop)->GetFunction());
        get_object()->Set(String::New("push"), FunctionTemplate::New(push)->GetFunction());
        get_object()->Set(String::New("promote"), FunctionTemplate::New(promote)->GetFunction());
     }

   static Handle<Value> pop(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmPager *pager = static_cast<CElmPager *>(self);
        CEvasObject *content = pager->pages.front();

        if (content)
          {
             elm_pager_content_pop(pager->get());
             pager->pages.pop_front();
          }

        return Undefined();
     }

   static Handle<Value> push(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmPager *pager = static_cast<CElmPager *>(self);
        if (args[0]->IsObject())
          {
             CEvasObject *content = make_or_get(pager, args[0]);
             if (content)
               {
                  elm_pager_content_push(pager->get(), content->get());
                  pager->pages.push_front(content);
               }
          }
        return Undefined();
     }
   static Handle<Value> promote(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmPager *pager = static_cast<CElmPager *>(self);
        if (args[0]->IsObject())
          {
             CEvasObject *promotee = eo_from_info(args[0]->ToObject());

             if (promotee)
               elm_pager_content_promote(pager->get(), promotee->get());

          }
        return Undefined();
     }
};

template<> CEvasObject::CPropHandler<CElmPager>::property_list
CEvasObject::CPropHandler<CElmPager>::list[] = {
  { NULL, NULL, NULL },
};

#endif

static CEvasObject *
_make(CEvasObject *parent, Local<Object> description)
{
   String::Utf8Value widget_type(description->Get(String::New("type")));
   CEvasObject *eo = CEvasObject::make(*widget_type, parent, description);

   if (!eo)
     ELM_ERR("Unknown object type: \"%s\"", *widget_type);

   return eo;
}

static CEvasObject *
_get_evas_object(Local<Object> obj)
{
   return static_cast<CEvasObject*>(External::Unwrap(obj->Get(String::New("_eo"))));
}

CEvasObject *
make_or_get(CEvasObject *parent, Handle<Value> object_val)
{
   if (!object_val->IsObject())
     {
        ELM_ERR("%s: value is not an object!", __FUNCTION__);
        return NULL;
     }

   Local<Object> obj = object_val->ToObject();
   return obj->HasOwnProperty(String::New("_eo")) ? _get_evas_object(obj) : _make(parent, obj);
}

CElmBasicWindow *main_win;
Persistent<Value> the_datadir;
Persistent<Value> the_tmpdir;
Persistent<Value> the_theme;

Handle<Value>
elm_widget(const Arguments& args)
{
   if (args.Length() != 1)
     return ThrowException(Exception::Error(String::New("Bad parameters")));

   if (!args[0]->IsObject())
     return Undefined();

   Local<Value> parent = args[0]->ToObject()->Get(String::New("parent"));
   if (parent.IsEmpty())
     return ThrowException(Exception::Error(String::New("Parent not set")));
   
   CEvasObject *parentObject = _get_evas_object(parent->ToObject());
   if (!parentObject)
     return ThrowException(Exception::Error(String::New("Parent is not a widget")));

   CEvasObject *object = make_or_get(parentObject, args[0]->ToObject());
   if (!object)
     return ThrowException(Exception::Error(String::New("Could not realize widget")));

   return object->get_object();
}

Handle<Value>
elm_main_window(const Arguments& args)
{
   Local<String> win_name;
   Local<Number> win_type;

   if (args.Length() != 1)
     return ThrowException(Exception::Error(String::New("Bad parameters")));

   if (!args[0]->IsObject())
     return Undefined();

   if (!args[1]->IsString())
     win_name = String::New("main");

   if (!args[2]->IsNumber())
     win_type = Number::New(ELM_WIN_BASIC);

   main_win = new CElmBasicWindow(NULL, args[0]->ToObject(),
                                         win_name, //win name/class
                                         win_type); //win type
   if (!main_win)
     return Undefined();

   /*Elm_Theme *theme = elm_theme_new();
   char *envtheme = getenv("ELM_THEME");
   elm_theme_set(theme, envtheme);
   elm_object_theme_set(main_win->get(), theme);*/

   return main_win->get_object();
}

Handle<Value>
elm_loop_time(const Arguments&)
{
   return Number::New(ecore_loop_time_get());
}

Handle<Value>
elm_exit(const Arguments&)
{
   elm_exit();
   return Undefined();
}

Handle<Value>
datadir_getter(Local<String>, const AccessorInfo&)
{
   return the_datadir;
}

void
datadir_setter(Local<String>, Local<Value> value, const AccessorInfo&)
{
   the_datadir.Dispose();
   the_datadir = Persistent<Value>::New(value);
}

Handle<Value>
tmpdir_getter(Local<String>, const AccessorInfo&)
{
   return the_tmpdir;
}

void
tmpdir_setter(Local<String>, Local<Value> value, const AccessorInfo&)
{
   the_tmpdir.Dispose();
   the_tmpdir = Persistent<Value>::New(value);
}

Handle<Value>
theme_getter(Local<String>, const AccessorInfo&)
{
   return the_theme;
}

void
theme_setter(Local<String>, Local<Value> value, const AccessorInfo&)
{
   the_theme.Dispose();
   setenv("ELM_THEME",  *String::Utf8Value(value->ToString()), 1);

   the_theme = Persistent<Value>::New(value);
}

extern "C"
void RegisterModule(Handle<Object> target)
{
   int argc = 0;
   char *argv[] = {};

   elev8_elm_log_domain = eina_log_domain_register("elev8-elm", EINA_COLOR_GREEN);
   if (!elev8_elm_log_domain)
     {
        ELM_ERR( "could not register elev8-elm log domain.");
        elev8_elm_log_domain = EINA_LOG_DOMAIN_GLOBAL;
     }
   ELM_INF("elev8-elm Logging initialized. %d", elev8_elm_log_domain);

   elm_init(argc, argv);

   target->Set(String::NewSymbol("window"), FunctionTemplate::New(elm_main_window)->GetFunction());
   target->Set(String::NewSymbol("loop_time"), FunctionTemplate::New(elm_loop_time)->GetFunction());
   target->Set(String::NewSymbol("exit"), FunctionTemplate::New(elm_exit)->GetFunction());
   target->Set(String::NewSymbol("widget"), FunctionTemplate::New(elm_widget)->GetFunction());
   target->SetAccessor(String::NewSymbol("datadir"), datadir_getter, datadir_setter);
   target->SetAccessor(String::NewSymbol("tmpdir"), tmpdir_getter, tmpdir_setter);
   target->SetAccessor(String::NewSymbol("theme"), theme_getter, theme_setter);

   /* setup data directory */
   the_datadir = Persistent<String>::New(String::New(PACKAGE_DATA_DIR "/" ));
   the_tmpdir = Persistent<String>::New(String::New(PACKAGE_TMP_DIR "/" ));

   /* register widget types */
   CEvasObject::init_factory();

#define REGISTER(name_,type_) CEvasObject::register_widget(name_, type_::make)

   REGISTER("actionslider", CElmActionSlider);
   REGISTER("button", CElmButton);
   REGISTER("layout", CElmLayout);
   REGISTER("background", CElmBackground);
   REGISTER("check", CElmCheck);
   REGISTER("clock", CElmClock);
   REGISTER("entry", CElmEntry);
   REGISTER("flip", CElmFlip);
   REGISTER("list", CElmList);
   REGISTER("genlist", CElmGenList);
   REGISTER("icon", CElmIcon);
   REGISTER("label", CElmLabel);
   REGISTER("radio", CElmRadio);
   REGISTER("box", CElmBox);
   REGISTER("progressbar", CElmProgressBar);
   REGISTER("scroller", CElmScroller);
   REGISTER("segment", CElmSegment);
   REGISTER("image", CEvasImage);
   REGISTER("slider", CElmSlider);
   REGISTER("photo", CElmPhoto);
   REGISTER("spinner", CElmSpinner);
   REGISTER("pane", CElmPane);
   REGISTER("bubble", CElmBubble);
   REGISTER("menu", CElmMenu);
   REGISTER("colorselector", CElmColorSelector);
   REGISTER("calendar", CElmCalendar);
   REGISTER("table", CElmTable);
   REGISTER("photocam", CElmPhotocam);
   REGISTER("toggle", CElmToggle);
   REGISTER("fileselectorbutton", CElmFileSelectorButton);
   REGISTER("fileselectorentry", CElmFileSelectorEntry);
   REGISTER("inwin", CElmInwin);
   REGISTER("notify", CElmNotify);
#if 0
   REGISTER("pager", CElmPager);
#endif
   REGISTER("naviframe", CElmNaviframe);
   REGISTER("grid", CElmGrid);

#undef REGISTER
}
