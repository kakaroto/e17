#include <v8.h>

#include <Elementary.h>

using namespace v8;

#include "elm.h"
#include "CElmObject.h"
#include "CElmWindow.h"
#include "CElmBackground.h"

namespace elm {

int log_domain;
extern "C" void RegisterModule(Handle<Object> target);

#ifdef USE_NODE
static const char *log_domain_name = "node-elm";
NODE_MODULE(elm, elm::RegisterModule);
#else
static const char *log_domain_name = "elev8-elm";
#endif

extern "C"
void RegisterModule(Handle<Object> target)
{
   int argc = 0;
   char *argv[] = {};

   log_domain = eina_log_domain_register(log_domain_name, EINA_COLOR_GREEN);
   if (!log_domain) {
      ELM_ERR("Could not register %s log domain.", log_domain_name);
      log_domain = EINA_LOG_DOMAIN_GLOBAL;
   }
   ELM_INF("%s log domain initialized %d", log_domain_name, log_domain);
   elm_init(argc, argv);

   target->Set(String::New("realise"), FunctionTemplate::New(CElmObject::Realise)->GetFunction());
   CElmWindow::Initialize(target);
   CElmBackground::Initialize(target);
}

}
