#ifndef C_ELM_WEB_H
#define C_ELM_WEB_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmWeb : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmWeb(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();

   struct {
      Persistent<Value> on_load_progress;
      Persistent<Value> on_title_change;
      Persistent<Value> on_uri_change;
      Persistent<Value> on_link_hover_in;
      Persistent<Value> on_link_hover_out;
   } cb;

public:
   virtual ~CElmWeb();

   static void Initialize(Handle<Object> target);

   /* Read & Write properties */
   Handle<Value> useragent_get() const;
   void useragent_set(Handle<Value> val);

   Handle<Value> tab_propagate_get() const;
   void tab_propagate_set(Handle<Value> val);

   Handle<Value> uri_get() const;
   void uri_set(Handle<Value> val);

   Handle<Value> bg_color_get() const;
   void bg_color_set(Handle<Value> val);

   Handle<Value> text_matches_highlight_get() const;
   void text_matches_highlight_set(Handle<Value> val);

   Handle<Value> history_enabled_get() const;
   void history_enabled_set(Handle<Value> val);

   Handle<Value> zoom_get() const;
   void zoom_set(Handle<Value> val);

   Handle<Value> zoom_mode_get() const;
   void zoom_mode_set(Handle<Value> val);

   Handle<Value> inwin_mode_get() const;
   void inwin_mode_set(Handle<Value> val);

   /* Events */
   Handle<Value> on_title_change_get() const;
   void on_title_change_set(Handle<Value> val);
   void OnTitleChange();
   static void OnTitleChangeWrapper(void *data, Evas_Object *, void *);

   Handle<Value> on_load_progress_get() const;
   void on_load_progress_set(Handle<Value> val);
   void OnLoadProgress();
   static void OnLoadProgressWrapper(void *data, Evas_Object *, void *);

   Handle<Value> on_uri_change_get() const;
   void on_uri_change_set(Handle<Value> val);
   void OnUriChange();
   static void OnUriChangeWrapper(void *data, Evas_Object *, void *);

   Handle<Value> on_link_hover_in_get() const;
   void on_link_hover_in_set(Handle<Value> val);
   void OnLinkHoverIn(char *url, char *title);
   static void OnLinkHoverInWrapper(void *data, Evas_Object *, void *);

   Handle<Value> on_link_hover_out_get() const;
   void on_link_hover_out_set(Handle<Value> val);
   void OnLinkHoverOut();
   static void OnLinkHoverOutWrapper(void *data, Evas_Object *, void *);

   /* Read only properties */
   Handle<Value> forward_possible_get() const;
   Handle<Value> title_get() const;
   Handle<Value> selection_get() const;
   Handle<Value> load_progress_get() const;
   Handle<Value> back_possible_get() const;

   /* Methods */
   Handle<Value> popup_destroy(const Arguments &);
   Handle<Value> text_search(const Arguments &args);
   Handle<Value> text_matches_mark(const Arguments &args);
   Handle<Value> text_matches_unmark_all(const Arguments &);
   Handle<Value> stop(const Arguments &);
   Handle<Value> reload(const Arguments &);
   Handle<Value> reload_full(const Arguments &);
   Handle<Value> back(const Arguments &);
   Handle<Value> forward(const Arguments &);
   Handle<Value> navigate(const Arguments &args);
   Handle<Value> region_show(const Arguments &args);
   Handle<Value> region_bring_in(const Arguments &args);
   Handle<Value> navigate_possible(const Arguments &args);

   friend Handle<Value> CElmObject::New<CElmWeb>(const Arguments &args);
};

}

#endif

