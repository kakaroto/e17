#ifndef C_ELM_POPUP_H
#define C_ELM_POPUP_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmPopup : public CElmObject {
private:
  static Persistent<FunctionTemplate> tmpl;
  Evas_Object *popup_parent;

  struct {
      Persistent<Value> button_press;
      Persistent<Value> dismiss;
      Persistent<Value> item_select;
      Persistent<Value> timeout;
      Persistent<Value> realize_content;
  } cb;

  struct {
      Persistent<Value> content;
      Persistent<Object> items;
  } cached;

protected:
  CElmPopup(Local<Object> _jsObject, CElmObject *parent);
  virtual ~CElmPopup();

  static Handle<FunctionTemplate> GetTemplate();

public:
  static void Initialize(Handle<Object> target);
  void Hide();
  void Dismiss();
  void Timeout();
  void DidRealizeContent();
  void PressButton(Handle<String> text);
  void ItemSelected(int item_index);

  Handle<Value> toast(const Arguments&);
  Handle<Value> message(const Arguments&);

  void on_button_press_set(Handle<Value> val);
  Handle<Value> on_button_press_get() const;

  void on_item_select_set(Handle<Value> val);
  Handle<Value> on_item_select_get() const;

  void on_dismiss_set(Handle<Value> val);
  Handle<Value> on_dismiss_get() const;

  void on_timeout_set(Handle<Value> val);
  Handle<Value> on_timeout_get() const;

  void on_realize_content_set(Handle<Value> val);
  Handle<Value> on_realize_content_get() const;

  friend Handle<Value> CElmObject::New<CElmPopup>(const Arguments& args);
};

}

#endif
