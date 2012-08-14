#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

#include <v8.h>

using namespace v8;

namespace environment {

void RegisterModule(Handle<ObjectTemplate>);

}
#endif

