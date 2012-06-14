#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <v8.h>

using namespace v8;

namespace storage {

void RegisterModule(Handle<ObjectTemplate>);

}
#endif

