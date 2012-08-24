#include <v8.h>

#include <Elementary.h>

using namespace v8;

#include "elm.h"
#include "CElmActionSlider.h"
#include "CElmBackground.h"
#include "CElmBox.h"
#include "CElmBubble.h"
#include "CElmButton.h"
#include "CElmCalendar.h"
#include "CElmCheck.h"
#include "CElmClock.h"
#include "CElmColorSelector.h"
#include "CElmConform.h"
#include "CElmCtxPopup.h"
#include "CElmDaySelector.h"
#include "CElmDateTime.h"
#include "CElmEntry.h"
#include "CElmFileSelector.h"
#include "CElmFileSelectorButton.h"
#include "CElmFileSelectorEntry.h"
#include "CElmFlip.h"
#include "CElmGenGrid.h"
#include "CElmGenList.h"
#include "CElmGrid.h"
#include "CElmHover.h"
#include "CElmIcon.h"
#include "CElmImage.h"
#include "CElmInwin.h"
#include "CElmLabel.h"
#include "CElmLayout.h"
#include "CElmNaviframe.h"
#include "CElmNotify.h"
#include "CElmObject.h"
#include "CElmPane.h"
#include "CElmPhotocam.h"
#include "CElmPhoto.h"
#include "CElmPopup.h"
#include "CElmProgressBar.h"
#include "CElmRadio.h"
#include "CElmScroller.h"
#include "CElmSegment.h"
#include "CElmSlider.h"
#include "CElmSpinner.h"
#include "CElmTable.h"
#include "CElmThumb.h"
#include "CElmToolbar.h"
#include "CElmVideo.h"
#include "CElmVideoControllers.h"
#include "CElmWeb.h"
#include "CElmWindow.h"

namespace elm {

int log_domain;
extern "C" void RegisterModule(Handle<Object> target);

#ifdef USE_NODE
static const char *log_domain_name = "node-elm";
NODE_MODULE(elm, elm::RegisterModule);
#else
static const char *log_domain_name = "elev8-elm";
#endif

static Persistent<Value> datadir;
static Persistent<Value> tmpdir;
static Persistent<Value> theme;

static Handle<Value>
loop_time(const Arguments&)
{
   return Number::New(ecore_loop_time_get());
}

static Handle<Value>
exit(const Arguments&)
{
   elm_exit();
   return Undefined();
}

static Handle<Value>
datadir_getter(Local<String>, const AccessorInfo&)
{
   return datadir;
}

static void
datadir_setter(Local<String>, Local<Value> value, const AccessorInfo&)
{
   datadir.Dispose();
   datadir = Persistent<Value>::New(value);
}

static Handle<Value>
tmpdir_getter(Local<String>, const AccessorInfo&)
{
   return tmpdir;
}

static void
tmpdir_setter(Local<String>, Local<Value> value, const AccessorInfo&)
{
   tmpdir.Dispose();
   tmpdir = Persistent<Value>::New(value);
}

static Handle<Value>
theme_getter(Local<String>, const AccessorInfo&)
{
   return theme;
}

static void
theme_setter(Local<String>, Local<Value> value, const AccessorInfo&)
{
   HandleScope scope;
   theme.Dispose();
   setenv("ELM_THEME", *String::Utf8Value(value), 1);

   theme = Persistent<Value>::New(value);
}

static void Initialize(Handle<Object> target)
{
   HandleScope scope;

   target->Set(String::NewSymbol("exit"),
               FunctionTemplate::New(exit)->GetFunction());
   target->Set(String::NewSymbol("loop_time"),
               FunctionTemplate::New(loop_time)->GetFunction());
   target->SetAccessor(String::NewSymbol("tmpdir"),
                       tmpdir_getter, tmpdir_setter);
   target->SetAccessor(String::NewSymbol("datadir"),
                       datadir_getter, datadir_setter);
   target->SetAccessor(String::NewSymbol("theme"),
                       theme_getter, theme_setter);

   /* setup data directory */
   datadir = Persistent<String>::New(String::New(PACKAGE_DATA_DIR "/" ));
   tmpdir = Persistent<String>::New(String::New(PACKAGE_TMP_DIR "/" ));
}

extern "C"
void RegisterModule(Handle<Object> target)
{
   log_domain = eina_log_domain_register(log_domain_name, EINA_COLOR_GREEN);
   if (!log_domain) {
      ELM_ERR("Could not register %s log domain.", log_domain_name);
      log_domain = EINA_LOG_DOMAIN_GLOBAL;
   }
   ELM_INF("%s log domain initialized %d", log_domain_name, log_domain);
   elm_init(0, NULL);

   Initialize(target);
   CElmActionSlider::Initialize(target);
   CElmBackground::Initialize(target);
   CElmBox::Initialize(target);
   CElmBubble::Initialize(target);
   CElmButton::Initialize(target);
   CElmCalendar::Initialize(target);
   CElmCheck::Initialize(target);
   CElmClock::Initialize(target);
   CElmColorSelector::Initialize(target);
   CElmConform::Initialize(target);
   CElmCtxPopup::Initialize(target);
   CElmDaySelector::Initialize(target);
   CElmDateTime::Initialize(target);
   CElmEntry::Initialize(target);
   CElmFileSelector::Initialize(target);
   CElmFileSelectorButton::Initialize(target);
   CElmFileSelectorEntry::Initialize(target);
   CElmFlip::Initialize(target);
   CElmGenGrid::Initialize(target);
   CElmGenList::Initialize(target);
   CElmGrid::Initialize(target);
   CElmHover::Initialize(target);
   CElmIcon::Initialize(target);
   CElmImage::Initialize(target);
   CElmInwin::Initialize(target);
   CElmLabel::Initialize(target);
   CElmLayout::Initialize(target);
   CElmNaviframe::Initialize(target);
   CElmNotify::Initialize(target);
   CElmObject::Initialize(target);
   CElmPane::Initialize(target);
   CElmPhotocam::Initialize(target);
   CElmPhoto::Initialize(target);
   CElmPopup::Initialize(target);
   CElmProgressBar::Initialize(target);
   CElmRadio::Initialize(target);
   CElmScroller::Initialize(target);
   CElmSegment::Initialize(target);
   CElmSlider::Initialize(target);
   CElmSpinner::Initialize(target);
   CElmTable::Initialize(target);
   CElmThumb::Initialize(target);
   CElmToolbar::Initialize(target);
   CElmVideo::Initialize(target);
   CElmVideoControllers::Initialize(target);
   CElmWeb::Initialize(target);
   CElmWindow::Initialize(target);
}

}
