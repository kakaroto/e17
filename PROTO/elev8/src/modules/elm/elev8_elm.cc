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
#include "CElmCheck.h"
#include "CElmClock.h"
#include "CElmGenList.h"
#include "CElmEntry.h"
#include "CElmPhoto.h"
#include "CElmSpinner.h"
#include "CElmBubble.h"
#include "CElmTable.h"
#include "CElmCalendar.h"
#include "CElmColorSelector.h"
#include "CElmMenu.h"
#include "CElmSegment.h"

int elev8_elm_log_domain = -1;

using namespace v8;

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
   REGISTER("naviframe", CElmNaviframe);
   REGISTER("grid", CElmGrid);

#undef REGISTER
}
