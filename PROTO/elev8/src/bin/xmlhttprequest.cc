#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>

#include "xmlhttprequest.h"

using namespace v8;
int elev8_http_log_domain = -1;

int XMLHttpRequest::fd_counter = 0;
Handle<ObjectTemplate> xmlHttpReqObj;

Eina_Bool data_callback(void *data, int type, void *event)
{
   Ecore_Con_Event_Url_Data *url_data = (Ecore_Con_Event_Url_Data *)event;
   void *ptr = ecore_con_url_data_get(url_data->url_con);

   if (ptr != data)
     {
        EINA_LOG_DOM_ERR(elev8_http_log_domain, "Ignore the event - Not for this URL\n");
	return ECORE_CALLBACK_PASS_ON;
     }

   XMLHttpRequest *reqObj = static_cast<XMLHttpRequest *>(data);
   if ( url_data->size > 0)
     {
        eina_binbuf_append_length(reqObj->data, 
			url_data->data, url_data->size);
	    EINA_LOG_DOM_INFO(elev8_http_log_domain,  "Appended %d data\n", url_data->size);
     }
   return EINA_FALSE;
}

Eina_Bool completion_callback(void *data, int type, void *event)
{
   HandleScope handle_scope;
   Ecore_Con_Event_Url_Complete *url_complete = (Ecore_Con_Event_Url_Complete *)event;
   void *ptr;

   ptr = ecore_con_url_data_get(url_complete->url_con);

   if (ptr != data)
     {
        EINA_LOG_DOM_INFO(elev8_http_log_domain,  "Ignore the event - Not for this URL\n");
	return ECORE_CALLBACK_PASS_ON;
     }

   XMLHttpRequest *reqObj = static_cast<XMLHttpRequest *>(ptr);

   const Eina_List *headers, *l;


   Local<Integer> status = Integer::New((int32_t)url_complete->status);
   reqObj->status = Persistent<Integer>(status);
   EINA_LOG_DOM_INFO(elev8_http_log_domain,  "Object Obtained =  %p\n", reqObj->data);

   // set class status here
   headers = ecore_con_url_response_headers_get(url_complete->url_con);

   EINA_LIST_FOREACH(headers, l, ptr)
   {
      char *str = (char *)ptr;
      //printf("header: %s\n", str);

      /* 
       * decide what to give to JS based on content type
       */
      if (strstr(str, "Content-Type:"))
        {
	   v8::Local<v8::String> addon;
	   /* binary - give the location of file */
	   if (strstr(str, "png") || (strstr(str, "jpeg")) || (strstr(str, "gif")))
             {
		EINA_LOG_DOM_INFO(elev8_http_log_domain,  "str = %s\n", str);
		char buf[100];
		sprintf(buf, "%s/elev8-http-%d",PACKAGE_TMP_DIR,
			             	XMLHttpRequest::addFdCount());


	        // make v8 string here
		std::ofstream out(buf, std::ios::out | std::ios::binary);
		EINA_LOG_DOM_INFO(elev8_http_log_domain,  "Image = %s\n", buf);
		out.write((char *)eina_binbuf_string_get(reqObj->data), 
				   eina_binbuf_length_get(reqObj->data));
		out.close();
   		EINA_LOG_DOM_INFO(elev8_http_log_domain,  "Size of response Data = %d bytes\n", 
						eina_binbuf_length_get(reqObj->data));
   		reqObj->responseText =  static_cast<Persistent<String> >(String::New(buf));
	     }
	   else
	     {
                reqObj->responseText =  static_cast<Persistent<String> >(
					String::New(
					(char *)eina_binbuf_string_get(reqObj->data)));
	     }
        }
      reqObj->responseHeaders.push_back(std::string(str));
   }

   Local<Integer> readyState = Integer::New(4);
   reqObj->readyState =  Persistent<Integer>(readyState);


   Local<String> funcName = String::New("onreadystatechange");
   //if (reqObj->obj->Has(funcName))
   if (reqObj->onreadystatechange->IsFunction())
     {
        /*Local<Value> tmp = reqObj->obj->Get(funcName);
        if (tmp->IsNull())
          {
             EINA_LOG_DOM_INFO(elev8_http_log_domain, "onreadystatechange is null for this instance\n");
          }
        else*/
          {
             Local<Function> func = Function::Cast(*(reqObj->onreadystatechange));
             if (!func.IsEmpty())
               {
                  Handle<String> path = reqObj->responseText;
                  Handle<String> obj = String::New(ecore_con_url_url_get(reqObj->url_con));
                  Handle<Value> args[2] = { obj, path };
                  func->Call(reqObj->obj,2, args);
               }
          }
     }
   ecore_event_handler_del(reqObj->url_data_handle);
   ecore_event_handler_del(reqObj->url_complete_handle);
   return true;
}

Handle<Value> 
response_text_getter(Local<String> property, 
const AccessorInfo& info)
{
   Local<Object> self = info.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void* ptr = wrap->Value();
   XMLHttpRequest *reqObj = (XMLHttpRequest *)ptr;

   return reqObj->responseText;
}

Handle<Value> 
status_getter(Local<String> property, 
const AccessorInfo& info)
{
   Local<Object> self = info.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void* ptr = wrap->Value();
   XMLHttpRequest *reqObj = (XMLHttpRequest *)ptr;
   return reqObj->status;
}

Handle<Value> 
readystate_getter(Local<String> property, 
const AccessorInfo& info)
{
   Local<Object> self = info.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void* ptr = wrap->Value();
   XMLHttpRequest *reqObj = (XMLHttpRequest *)ptr;
   return reqObj->readyState;
}

void onreadystatechange_setter(Local<String> property,
		 Local<Value> value,
		 const AccessorInfo& info)
{
   Local<Object> self = info.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void* ptr = wrap->Value();
   XMLHttpRequest *reqObj = (XMLHttpRequest *)ptr;

   String::Utf8Value prop_name(property);
   //EINA_LOG_DOM_INFO(elev8_http_log_domain,  "************************************\n");
   //EINA_LOG_DOM_INFO(elev8_http_log_domain,  "Setting callback to %p for %p\n", *value, ptr);
   //EINA_LOG_DOM_INFO(elev8_http_log_domain,  "************************************\n");

   reqObj->onreadystatechange.Dispose();
   reqObj->onreadystatechange = Persistent<Value>::New(value);

}

Handle<Value> get_response_header(const Arguments& args)
{
   Local<Object> self = args.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void* ptr = wrap->Value();
   XMLHttpRequest *reqObj = (XMLHttpRequest *)ptr;
   std::vector<std::string>::iterator it;

   String::Utf8Value requested_header(args[0]->ToString());
   //EINA_LOG_DOM_INFO(elev8_http_log_domain, "Making request to %s\n", *requested_header);

   for ( it=reqObj->responseHeaders.begin() ; it < reqObj->responseHeaders.end(); it++ )
      {
         if (std::string::npos != ((*it)).find(*requested_header))
	   {
//	      printf("Found header : %s \n", (*it).c_str());
              return Handle<Value>(String::New((*it).c_str()));
	   }
      }
   return Null();
}

Handle<Value> get_response_headers(const Arguments& args)
{
   Local<Object> self = args.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void* ptr = wrap->Value();
   XMLHttpRequest *reqObj = (XMLHttpRequest *)ptr;
   std::vector<std::string>::iterator it;
   int length = reqObj->responseHeaders.size();
   Handle<Array> response_headers = Array::New(length);

   for ( it=reqObj->responseHeaders.begin() ; it < reqObj->responseHeaders.end(); it++ )
     {
	Handle<String>  hdr = String::New((*it).c_str()); 
	int index = it-reqObj->responseHeaders.begin();
        //printf("Adding header : %d : %s \n", index, (*it).c_str());
	response_headers->Set(index, hdr);
     }
   return static_cast<Handle<Value> >(response_headers);
}

Handle<Value> set_request_header(const Arguments& args)
{
   Local<Object> self = args.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void* ptr = wrap->Value();
   XMLHttpRequest *reqObj = (XMLHttpRequest *)ptr;

   String::Utf8Value name(args[0]->ToString());
   String::Utf8Value value(args[1]->ToString());

   ecore_con_url_additional_header_add (reqObj->url_con, *name, *value);

   return Undefined();
}

/* as of now we only support (method, url), user and password can be added later */
/* args[2] is dummy for compliance with XMLHttpRequest */
Handle<Value>
ecore_con_open(const Arguments& args)
{
   HandleScope scope;
   EINA_LOG_DOM_INFO(elev8_http_log_domain, "Calling Open API\n");
   Local<Object> self = args.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void* ptr = wrap->Value();
   XMLHttpRequest *reqObj = (XMLHttpRequest *)ptr;

   if (args[0]->IsString())
     {
	String::Utf8Value method(args[0]->ToString());

	if (strstr(*method, "GET"))
          {
             reqObj->http_method = HTTP_GET;
	  }
	else if(strstr(*method, "POST"))
          {
             reqObj->http_method = HTTP_POST;
          }
	else
          {
             EINA_LOG_DOM_ERR(elev8_http_log_domain, "Only GET and POST supported\n");
             return Undefined();
	  }

        String::Utf8Value url(args[1]->ToString());
        EINA_LOG_DOM_INFO(elev8_http_log_domain, "Making request to %s\n", *url);
        Ecore_Con_Url *url_con = ecore_con_url_new(*url);
        if (url_con==NULL)
          {
             EINA_LOG_DOM_ERR(elev8_http_log_domain, "Cannot open connection to %s\n", *url);
             return Undefined();
          }
        reqObj->url_con = url_con;
	ecore_con_url_data_set(reqObj->url_con, reinterpret_cast<void *>(reqObj));
     }
    return Undefined();
}

Handle<Value>
ecore_con_send(const Arguments& args)
{
   HandleScope scope;
   EINA_LOG_DOM_INFO(elev8_http_log_domain, "Calling Send API\n");
   Local<Object> self = args.Holder();
   Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
   void* ptr = wrap->Value();
   XMLHttpRequest *reqObj = (XMLHttpRequest *)ptr;
   Eina_Bool sentStatus = EINA_FALSE;

   if (args[0]->IsString() && (reqObj->http_method == HTTP_POST))
     {
        String::Utf8Value body(args[0]->ToString());
	sentStatus = ecore_con_url_post(reqObj->url_con, (void *)(*body), 
			   body.length(), "text/plain;charset=UTF-8");

     }
   else
     {
        sentStatus = ecore_con_url_get(reqObj->url_con);
     }

   if (!sentStatus)
     {
        EINA_LOG_DOM_ERR(elev8_http_log_domain, "Unable to send request\n");
     }

   return Undefined();
}

Handle<Value> createXMLHttpReqInstance(const Arguments& args)
{
   HandleScope scope;

   XMLHttpRequest *reqObj = new XMLHttpRequest();

   Local<FunctionTemplate> tmpOpen = FunctionTemplate::New();
   tmpOpen->SetCallHandler(ecore_con_open);
   xmlHttpReqObj->Set(String::New("open"), tmpOpen);

   Local<FunctionTemplate> tmpSend = FunctionTemplate::New();
   tmpSend->SetCallHandler(ecore_con_send);
   xmlHttpReqObj->Set(String::New("send"), tmpSend);

   Local<FunctionTemplate> tmpRequest = FunctionTemplate::New();
   tmpRequest->SetCallHandler(set_request_header);
   xmlHttpReqObj->Set(String::New("setRequestHeader"),tmpRequest);

   Local<FunctionTemplate> tmpResponse = FunctionTemplate::New();
   tmpResponse->SetCallHandler(get_response_header);
   xmlHttpReqObj->Set(String::New("getResponseHeader"),tmpResponse);

   Local<FunctionTemplate> tmpResponses = FunctionTemplate::New();
   tmpResponses->SetCallHandler(get_response_headers);
   xmlHttpReqObj->Set(String::New("getResponseHeaders"),tmpResponses);

   xmlHttpReqObj->SetAccessor(String::New("responseText"), 
                                &response_text_getter, 
                                 NULL , Null());
   xmlHttpReqObj->SetAccessor(String::New("status"), 
                                 &status_getter, 
                                 NULL , Null()); 
   xmlHttpReqObj->SetAccessor(String::New("readyState"), 
                                 &readystate_getter, 
                                 NULL , Null()); 

   xmlHttpReqObj->SetAccessor(String::New("onreadystatechange"), 
		   		NULL,
				onreadystatechange_setter,
				Null()
		             );

   reqObj->obj.Dispose();
   reqObj->obj = Persistent<Object>::New(xmlHttpReqObj->NewInstance());
   reqObj->obj->SetInternalField(0, External::New(reqObj));
   reqObj->url_complete_handle = ecore_event_handler_add( ECORE_CON_EVENT_URL_COMPLETE,
                            completion_callback,
                            reinterpret_cast<void *>(reqObj));
   reqObj->url_data_handle = ecore_event_handler_add( ECORE_CON_EVENT_URL_DATA,
                            data_callback,
                            reinterpret_cast<void *>(reqObj));

   EINA_LOG_DOM_INFO(elev8_http_log_domain,  "Http Request initialized %p\n", reqObj->data);
   return reqObj->obj; 
}

int xmlhttp_v8_setup(Handle<ObjectTemplate> global)
{
   elev8_http_log_domain = eina_log_domain_register("elev8-http", EINA_COLOR_ORANGE);
   if (!elev8_http_log_domain)
     {
        EINA_LOG_DOM_ERR(elev8_http_log_domain, "could not register elev8-http log domain.");
        elev8_http_log_domain = EINA_LOG_DOMAIN_GLOBAL;
     }
   EINA_LOG_DOM_INFO(elev8_http_log_domain,"elev8-http Logging initialized. %d\n", elev8_http_log_domain);


   if (!ecore_con_url_init())
     {
        EINA_LOG_DOM_ERR(elev8_http_log_domain, "Cannot Init to ECore_Url\n");
        return -1;
     }

   //EINA_LOG_DOM_INFO(elev8_http_log_domain,  "Creating XML Http Request Instance\n");

   /* Add support for XML HTTP Request */
   xmlHttpReqObj = ObjectTemplate::New();
   xmlHttpReqObj->SetInternalFieldCount(1);
   global->Set(String::New("XMLHttpRequest"), FunctionTemplate::New(createXMLHttpReqInstance));
   return 0;
}
