#ifndef C_ELM_CTX_POPUP_H
#define C_ELM_CTX_POPUP_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmCtxPopup : public CElmObject {
private:
  static Persistent<FunctionTemplate> tmpl;
  Evas_Object *ctxpopup_parent;

//  Persistent<Object> items;
  Persistent<Value> on_item_select;
  Persistent<Value> on_dismiss;
  Persistent<Value> hover_parent;
protected:
  CElmCtxPopup(Local<Object> _jsObject, CElmObject *parent);
  ~CElmCtxPopup();

  static Handle<FunctionTemplate> GetTemplate();

public:
  static void Initialize(Handle<Object> target);

  void ItemSelected(Handle<Value> item);
  void Dismissed();

  void on_item_select_set(Handle<Value> val);
  Handle<Value> on_item_select_get() const;

  void on_dismiss_set(Handle<Value> val);
  Handle<Value> on_dismiss_get() const;

  void horizontal_set(Handle<Value> val);
  Handle<Value> horizontal_get() const;

  void hover_parent_set(Handle<Value> val);
  Handle<Value> hover_parent_get() const;

  Handle<Value> direction_get() const;

  Handle<Value> AddItems(Handle<Value> val);

  Handle<Value> show(const Arguments &args);
  Handle<Value> dismiss(const Arguments &);

  friend Handle<Value> CElmObject::New<CElmCtxPopup>(const Arguments& args);
};

}

#endif
