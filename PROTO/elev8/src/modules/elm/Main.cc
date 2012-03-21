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
#include "CElmActionSlider.h"
#include "CElmBackground.h"
#include "CElmBasicWindow.h"
#include "CElmBox.h"
#include "CElmBubble.h"
#include "CElmButton.h"
#include "CElmCalendar.h"
#include "CElmCheck.h"
#include "CElmClock.h"
#include "CElmColorSelector.h"
#include "CElmEntry.h"
#include "CElmFileSelectorButton.h"
#include "CElmFileSelectorEntry.h"
#include "CElmFlip.h"
#include "CElmGenList.h"
#include "CElmGrid.h"
#include "CElmHover.h"
#include "CElmIcon.h"
#include "CElmImage.h"
#include "CElmInwin.h"
#include "CElmLabel.h"
#include "CElmLayout.h"
#include "CElmMenu.h"
#include "CElmNaviframe.h"
#include "CElmNotify.h"
#include "CElmPane.h"
#include "CElmPhotocam.h"
#include "CElmPhoto.h"
#include "CElmProgressBar.h"
#include "CElmRadio.h"
#include "CElmScroller.h"
#include "CElmSegment.h"
#include "CElmSlider.h"
#include "CElmSpinner.h"
#include "CElmTable.h"
#include "CElmDaySelector.h"
#include "CElmConform.h"
#include "CEvasImage.h"
#include "CEvasObject.h"

int elev8_elm_log_domain = -1;
extern "C" void RegisterModule(Handle<Object> target);

using namespace v8;

static CElmBasicWindow *main_win;
static Persistent<Value> the_datadir;
static Persistent<Value> the_tmpdir;
static Persistent<Value> the_theme;

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

static Handle<Value>
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

static Handle<Value>
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

static Handle<Value>
elm_loop_time(const Arguments&)
{
   return Number::New(ecore_loop_time_get());
}

static Handle<Value>
elm_exit(const Arguments&)
{
   elm_exit();
   return Undefined();
}

static Handle<Value>
datadir_getter(Local<String>, const AccessorInfo&)
{
   return the_datadir;
}

static void
datadir_setter(Local<String>, Local<Value> value, const AccessorInfo&)
{
   the_datadir.Dispose();
   the_datadir = Persistent<Value>::New(value);
}

static Handle<Value>
tmpdir_getter(Local<String>, const AccessorInfo&)
{
   return the_tmpdir;
}

static void
tmpdir_setter(Local<String>, Local<Value> value, const AccessorInfo&)
{
   the_tmpdir.Dispose();
   the_tmpdir = Persistent<Value>::New(value);
}

static Handle<Value>
theme_getter(Local<String>, const AccessorInfo&)
{
   return the_theme;
}

static void
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
   REGISTER("fileselectorbutton", CElmFileSelectorButton);
   REGISTER("fileselectorentry", CElmFileSelectorEntry);
   REGISTER("inwin", CElmInwin);
   REGISTER("notify", CElmNotify);
   REGISTER("naviframe", CElmNaviframe);
   REGISTER("grid", CElmGrid);
   REGISTER("dayselector", CElmDaySelector);
   REGISTER("conform", CElmConform);

#undef REGISTER
}
