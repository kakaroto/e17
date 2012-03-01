#ifndef __ELEV8_UTILS_H__
#define __ELEV8_UTILS_H__

#include <sys/mman.h>
#include <fcntl.h>
#include <elev8_common.h>

using namespace v8;
Local<String> string_from_file(const char *filename);
int shebang_length(const char *p, int len);
void boom(TryCatch &try_catch);
void compile_and_run(Handle<String> source);

#endif
