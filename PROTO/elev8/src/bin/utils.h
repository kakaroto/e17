#ifndef __ELEV8_UTILS_H__
#define __ELEV8_UTILS_H__

#include <sys/mman.h>
#include <fcntl.h>
#include <elev8_common.h>

using namespace v8;

Handle<String> string_from_file(const char *filename);
void boom(Handle<Message> msg, const char *exception);
void boom(const TryCatch &try_catch);
void compile_and_run(Handle<String> source, const char *filename);

#endif
