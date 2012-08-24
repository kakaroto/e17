#include <v8.h>
#include <Ecore.h>
#include <Ecore_Con.h>
#include <iostream>
#include <fstream>

#define METHOD_GET  1
#define METHOD_POST 2

using namespace v8;

namespace elm {

extern "C" void RegisterModule(Handle<Object> target);
int log_domain;

#define HTTP_DBG(...) EINA_LOG_DOM_DBG(log_domain, __VA_ARGS__)
#define HTTP_INF(...) EINA_LOG_DOM_INFO(log_domain, __VA_ARGS__)
#define HTTP_WRN(...) EINA_LOG_DOM_WARN(log_domain, __VA_ARGS__)
#define HTTP_ERR(...) EINA_LOG_DOM_ERR(log_domain, __VA_ARGS__)
#define HTTP_CRT(...) EINA_LOG_DOM_CRITICAL(log_domain, __VA_ARGS__)

template<class T> T *GetObjectFromInfo(const AccessorInfo &info)
{
   return static_cast<T *>(info.Holder()->GetPointerFromInternalField(0));
}

template<class T> inline T *GetObjectFromArguments(const Arguments &args)
{
     return static_cast<T *>(args.This()->GetPointerFromInternalField(0));
}

class XMLHttpRequest
{
   Eina_Binbuf *data;
   Ecore_Con_Url *url;
   Ecore_Event_Handler *data_handler;
   Ecore_Event_Handler *complete_handler;
   Persistent<Value> onreadystatechange;
   Persistent<Object> jsObject;
   int method;
   static Handle<FunctionTemplate> tmpl;

   XMLHttpRequest(Local<Object> _jsObject)
     {
        url = NULL;
        data_handler = NULL;
        complete_handler = NULL;
        data = eina_binbuf_new();

        jsObject = Persistent<Object>::New(_jsObject);
        jsObject->SetPointerInInternalField(0, this);
        jsObject.MakeWeak(this, Delete);
     }

   void reset()
     {
        if (complete_handler)
          ecore_event_handler_del(complete_handler);

        if (data_handler)
          ecore_event_handler_del(data_handler);

        complete_handler = NULL;
        data_handler = NULL;

        eina_binbuf_reset(data);
     }

   static Handle<Value> New(const Arguments& args)
     {
        HandleScope scope;
        new XMLHttpRequest(args.This());
        return Undefined();
     }

   static void Delete(Persistent<Value> object, void *parameter)
     {
        XMLHttpRequest *self = static_cast<XMLHttpRequest *>(parameter);
        self->reset();
        eina_binbuf_free(self->data);
        ecore_con_url_free(self->url);
        self->onreadystatechange.Dispose();
        delete self;
        object.Clear();
     }

   void setReadyState(int state)
     {
        jsObject->Set(String::NewSymbol("readyState"), Integer::New(state));

        if (onreadystatechange.IsEmpty() || !onreadystatechange->IsFunction())
          return;

        Handle<Function> callback(Function::Cast(*onreadystatechange));
        callback->Call(jsObject, 0, NULL);
     }

   static Eina_Bool ProgressCb(void *data, int, void *event)
     {
        HandleScope scope;
        Ecore_Con_Event_Url_Data *url_data = (Ecore_Con_Event_Url_Data *)event;
        XMLHttpRequest *self = static_cast<XMLHttpRequest *>
           (ecore_con_url_data_get(url_data->url_con));

        if (self != data)
          return ECORE_CALLBACK_PASS_ON;

        eina_binbuf_append_length(self->data, url_data->data, url_data->size);
        return ECORE_CALLBACK_DONE;
     }

   static Eina_Bool CompletionCb(void *data, int, void *event)
     {
        HandleScope handle_scope;
        Ecore_Con_Event_Url_Complete *url_complete = (Ecore_Con_Event_Url_Complete *)event;
        XMLHttpRequest *self = static_cast<XMLHttpRequest *>
           (ecore_con_url_data_get(url_complete->url_con));

        if (self != data)
          return ECORE_CALLBACK_PASS_ON;

        const char *value = (const char *)eina_binbuf_string_get(self->data);
        int length = eina_binbuf_length_get(self->data);

        const char *content_type = self->getResponseHeader("Content-Type");
        if (!content_type)
          self->jsObject->Set(String::NewSymbol("responseText"), String::New(""));
        else if (strstr(content_type, "image/"))
          {
             char buf[256];
             snprintf(buf, sizeof(buf), "%s/elev8-http-%p",PACKAGE_TMP_DIR, self);
             std::ofstream out(buf, std::ios::out | std::ios::binary);

             out.write(value, length);
             out.close();

             self->jsObject->Set(String::NewSymbol("responseText"),
                                 String::New(buf));
          }
        else
          {
             self->jsObject->Set(String::NewSymbol("responseText"),
                                 String::New(value, length));
          }

        self->jsObject->Set(String::NewSymbol("status"),
                            Integer::New(url_complete->status));

        self->setReadyState(4);
        self->reset();

        return ECORE_CALLBACK_DONE;
     }

   static void onreadystatechange_setter(Local<String>, Local<Value> value,
                                         const AccessorInfo& info)
     {
        XMLHttpRequest *self = GetObjectFromInfo<XMLHttpRequest>(info);
        self->onreadystatechange.Dispose();
        self->onreadystatechange = Persistent<Value>::New(value);
     }

   const char *getResponseHeader(const char *header)
     {
        void *p;
        const Eina_List *l;

        EINA_LIST_FOREACH(ecore_con_url_response_headers_get(url), l, p)
           if (strstr((char *)p, header) == p)
             return &(strchr((char*)p, ' '))[1];

        return NULL;
     }

   static Handle<Value> getResponseHeader(const Arguments& args)
     {
        HandleScope scope;
        String::Utf8Value header(args[0]);
        XMLHttpRequest *self = GetObjectFromArguments<XMLHttpRequest>(args);
        const char *value = self->getResponseHeader(*header);
        return scope.Close(value ? String::New(value) : Undefined());
     }

   static Handle<Value> getAllResponseHeaders(const Arguments& args)
     {
        void *p;
        const Eina_List *l;
        HandleScope scope;
        XMLHttpRequest *self = GetObjectFromArguments<XMLHttpRequest>(args);
        Handle<String> headers = String::Empty();

        EINA_LIST_FOREACH(ecore_con_url_response_headers_get(self->url), l, p)
           headers = String::Concat(headers, String::New((char *)p));

        return scope.Close(headers);
     }

   static Handle<Value> setRequestHeader(const Arguments& args)
     {
        HandleScope scope;

        XMLHttpRequest *self = GetObjectFromArguments<XMLHttpRequest>(args);
        ecore_con_url_additional_header_add(self->url, *String::Utf8Value(args[0]),
                                            *String::Utf8Value(args[1]));
        return Undefined();
     }

   static Eina_Bool doOpen(XMLHttpRequest *self, Handle<String> method, Handle<String> url)
     {
        HandleScope scope;

        String::Utf8Value m(method);
        if (!strcmp(*m, "GET"))
          self->method = METHOD_GET;
        else if (!strcmp(*m, "POST"))
          self->method = METHOD_POST;
        else
          return EINA_FALSE;

        if (self->url)
          ecore_con_url_free(self->url);
        self->url = ecore_con_url_new(*String::Utf8Value(url->ToString()));
        ecore_con_url_data_set(self->url, reinterpret_cast<void *>(self));

        return EINA_TRUE;
     }

   /* as of now we only support (method, url), user and password can be added later */
   /* args[2] is dummy for compliance with XMLHttpRequest */
   static Handle<Value> open(const Arguments& args)
     {
        HandleScope scope;

        if (!args[0]->IsString() || !args[1]->IsString())
          return Undefined();

        XMLHttpRequest *self = GetObjectFromArguments<XMLHttpRequest>(args);

        if (XMLHttpRequest::doOpen(self, args[0]->ToString(), args[1]->ToString()))
          return args.This();

        return Undefined();
     }

   static Handle<String> UrlEncode(Handle<String> str)
     {
       HandleScope scope;

       Handle<Object> global = Context::GetCurrent()->Global();
       Handle<Value> value = global->Get(String::NewSymbol("encodeURIComponent"));
       Handle<Function> func = Handle<Function>::Cast(value);
       Handle<Value> args[1];
       args[0] = str;
       Handle<String> result = func->Call(global, 1, args)->ToString();

       return scope.Close(result);
     }

   static void JSArrayToURL(Eina_Strbuf *out, Handle<Array> array, Handle<String> key)
     {
        HandleScope scope;

        for (unsigned int i = 0; i < array->Length(); i++)
          {
            if (i > 0)
              eina_strbuf_append(out, "&");
            eina_strbuf_append(out, *String::Utf8Value(UrlEncode(key)));
            eina_strbuf_append_printf(out, "%d=", i + 1);
            eina_strbuf_append(out, *String::Utf8Value(UrlEncode(array->Get(i)->ToString())));
          }

     }

   static char *JSObjectToURL(Handle<Object> object)
     {
        HandleScope scope;

        Local<Array> properties = object->GetOwnPropertyNames();

        Eina_Strbuf *out = eina_strbuf_new();
        for (unsigned int i = 0; i < properties->Length(); i++)
          {
            Local<String> key = properties->Get(i)->ToString();
            Local<Value> value = object->Get(key);

            if (i > 0)
              eina_strbuf_append(out, "&");

            if (value->IsArray())
              JSArrayToURL(out, Handle<Array>::Cast(value), key);
            else
              {
                eina_strbuf_append(out, *String::Utf8Value(UrlEncode(key)));
                eina_strbuf_append(out, "=");
                eina_strbuf_append(out, *String::Utf8Value(UrlEncode(value->ToString())));
              }
          }

        char *result = strdup(eina_strbuf_string_get(out));
        eina_strbuf_free(out);

        return result;
     }

   static char *GetAssignableUtf8Value(Handle<String> string)
     {
        return strdup(*String::Utf8Value(string));
     }

   static void doSend(XMLHttpRequest *self, Handle<Value> paramData)
     {
         /* TODO - Set readyState properly */
        self->jsObject->Set(String::NewSymbol("readyState"), Integer::New(0));

        self->reset();

        self->data_handler = ecore_event_handler_add(ECORE_CON_EVENT_URL_DATA,
                                                     XMLHttpRequest::ProgressCb,
                                                     reinterpret_cast<void *>(self));

        self->complete_handler = ecore_event_handler_add(ECORE_CON_EVENT_URL_COMPLETE,
                                                         XMLHttpRequest::CompletionCb,
                                                         reinterpret_cast<void *>(self));
        if (self->method == METHOD_GET)
          {
            if (!ecore_con_url_get(self->url))
              {
                 HTTP_ERR( "Unable to send request");
                 self->reset();
              }
          }
        else
          {
            char *data;

            if (paramData->IsNull() || paramData->IsUndefined())
              data = NULL;
            else if (paramData->IsString())
              data = GetAssignableUtf8Value(paramData->ToString());
            else if (paramData->IsObject())
              data = JSObjectToURL(paramData->ToObject());
            else
              data = GetAssignableUtf8Value(UrlEncode(paramData->ToString()));

            if (!ecore_con_url_post(self->url, data, data == NULL ? 0 : strlen(data), NULL))
              {
                 HTTP_ERR( "Unable to send request");
                 self->reset();
              }

            free(data);
          }
     }

   static Handle<Value> send(const Arguments& args)
     {
        HandleScope scope;

        XMLHttpRequest *self = GetObjectFromArguments<XMLHttpRequest>(args);

        XMLHttpRequest::doSend(self, args[0]);
        return Undefined();
     }

   static Handle<Value> _do(const Arguments &args)
     {
        HandleScope scope;

        XMLHttpRequest *self = GetObjectFromArguments<XMLHttpRequest>(args);

        if (!args[0]->IsString() || !args[1]->IsString())
          return Undefined();

        XMLHttpRequest::doOpen(self, args[0]->ToString(), args[1]->ToString());
        XMLHttpRequest::doSend(self, args[2]);

        return Undefined();
     }

 public:

   static Handle<FunctionTemplate> GetTemplate()
     {
        if (!tmpl.IsEmpty())
          return tmpl;

        tmpl = FunctionTemplate::New(New);

        Handle<ObjectTemplate> it = tmpl->InstanceTemplate();

        it->SetInternalFieldCount(1);
        it->Set(String::NewSymbol("open"), FunctionTemplate::New(open));
        it->Set(String::NewSymbol("send"), FunctionTemplate::New(send));
        it->Set(String::NewSymbol("do"), FunctionTemplate::New(_do));
        it->Set(String::NewSymbol("setRequestHeader"),
                FunctionTemplate::New(setRequestHeader));
        it->Set(String::NewSymbol("getResponseHeader"),
                FunctionTemplate::New(getResponseHeader));
        it->Set(String::NewSymbol("getAllResponseHeaders"),
                FunctionTemplate::New(getAllResponseHeaders));
        it->SetAccessor(String::New("onreadystatechange"), NULL,
                        onreadystatechange_setter, Null());
        return tmpl;
     }
};

Handle<FunctionTemplate> XMLHttpRequest::tmpl;

extern "C"
void RegisterModule(Handle<Object> target)
{
   log_domain = eina_log_domain_register("elev8-http", EINA_COLOR_ORANGE);
   if (!log_domain)
     {
        HTTP_ERR( "could not register elev8-http log domain.");
        log_domain = EINA_LOG_DOMAIN_GLOBAL;
     }
   HTTP_INF("elev8-http Logging initialized. %d", log_domain);

   ecore_init();
   ecore_con_init();
   ecore_con_url_init();

   target->Set(String::NewSymbol("XMLHttpRequest"),
               XMLHttpRequest::GetTemplate()->GetFunction());
}

}
