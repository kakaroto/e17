#ifndef _ELM_H
#define _ELM_H

#define ELM_DBG(...) EINA_LOG_DOM_DBG(log_domain, __VA_ARGS__)
#define ELM_INF(...) EINA_LOG_DOM_INFO(log_domain, __VA_ARGS__)
#define ELM_WRN(...) EINA_LOG_DOM_WARN(log_domain, __VA_ARGS__)
#define ELM_ERR(...) EINA_LOG_DOM_ERR(log_domain, __VA_ARGS__)
#define ELM_CRT(...) EINA_LOG_DOM_CRITICAL(log_domain, __VA_ARGS__)

#include <Elementary.h>
#include <v8.h>
#include <stdarg.h>

namespace elm {

using namespace v8;

template<class T> inline T *GetObjectFromAccessorInfo(const AccessorInfo &info)
{
     return static_cast<T *>(info.This()->GetPointerFromInternalField(0));
}

inline void RegisterProperties(Handle<ObjectTemplate> prototype, ...)
{
   va_list arg;

   va_start(arg, prototype);
   for (const char *name = va_arg(arg, const char *); name; name = va_arg(arg, const char *))
     {
        AccessorGetter get_callback = va_arg(arg, AccessorGetter);
        AccessorSetter set_callback = va_arg(arg, AccessorSetter);
        prototype->SetAccessor(String::NewSymbol(name),
                               get_callback, set_callback);
     }
   va_end(arg);
}

extern int log_domain;

}

#define PROPERTY(name_) \
   #name_, Callback_## name_ ##_get, Callback_## name_ ##_set

#define GENERATE_PROPERTY_CALLBACKS(class_,name_) \
   static Handle<Value> Callback_## name_ ##_get(Local<String>, const AccessorInfo &info) { \
      HandleScope scope; \
      return scope.Close(GetObjectFromAccessorInfo<class_>(info)->name_ ##_get()); \
   } \
   static void Callback_## name_ ##_set(Local<String>, Local<Value> value, const AccessorInfo &info) { \
      HandleScope scope; \
      GetObjectFromAccessorInfo<class_>(info)->name_ ##_set(value); \
   }

#define GENERATE_TEMPLATE_FULL(super_class_,this_class_,...) \
   Handle<FunctionTemplate> this_class_::GetTemplate() \
   { \
      if (!tmpl.IsEmpty()) return tmpl; \
      \
      HandleScope scope; \
      Handle<FunctionTemplate> parentTmpl = super_class_::GetTemplate(); \
      tmpl = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New<this_class_>)); \
      tmpl->Inherit(parentTmpl); \
      tmpl->InstanceTemplate()->SetInternalFieldCount(1); \
      RegisterProperties(tmpl->PrototypeTemplate(), ##__VA_ARGS__, NULL); \
      return scope.Close(tmpl); \
   } \
   Persistent<FunctionTemplate> this_class_::tmpl

#define GENERATE_TEMPLATE(this_class_,...) \
   GENERATE_TEMPLATE_FULL(CElmObject, this_class_, ##__VA_ARGS__)


#endif
