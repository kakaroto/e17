#include <v8.h>

#include <Elementary.h>

using namespace v8;

#include "elm.h"
#include "CElmObject.h"
#include "CElmWindow.h"

namespace elm {

int log_domain;

#ifdef USE_NODE
void InitElm(Handle<Object> target)
#else
extern "C"
void RegisterModule(Handle<Object> target)
#endif
{
   int argc = 0;
   char *argv[] = {};

   eina_init();
   log_domain = eina_log_domain_register("node-elm", EINA_COLOR_GREEN);
   if (!log_domain) {
      ELM_ERR("Could not register node-elm log domain.");
      log_domain = EINA_LOG_DOMAIN_GLOBAL;
   }
   ELM_INF("node-elm log domain initialized %d", log_domain);
   elm_init(argc, argv);

   target->Set(String::New("realise"), FunctionTemplate::New(CElmObject::Realise)->GetFunction());
   CElmWindow::Initialize(target);
   CElmBackground::Initialize(target);
}

}

#ifdef USE_NODE
NODE_MODULE(elm, elm::InitElm);
#endif
