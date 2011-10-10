#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Con.h>
#include <v8.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <vector>
#include <string>

v8::Handle<v8::Value>response_text_getter(v8::Local<v8::String> property, const v8::AccessorInfo& info);
v8::Handle<v8::Value>status_getter(v8::Local<v8::String> property, const v8::AccessorInfo& info);
Eina_Bool completion_callback(void *data, int type, void *event);
Eina_Bool data_callback(void *data, int type, void *event);
v8::Handle<v8::Value> ecore_con_open(const v8::Arguments& args);
v8::Handle<v8::Value> get_response_header(const v8::Arguments& args);
v8::Handle<v8::Value> set_request_header(const v8::Arguments& args);
v8::Handle<v8::Value> ecore_con_send(const v8::Arguments& args);

enum 
{
	HTTP_GET,
	HTTP_POST
};

class XMLHttpRequest
{
   private:
      static int fd_counter;
   public:
      std::vector<std::string> responseHeaders;
      Ecore_Con_Url *url_con;
      Ecore_Event_Handler *url_complete_handle;
      Ecore_Event_Handler *url_data_handle;
      int http_method; //GET, POST
      Eina_Binbuf *data;
      v8::Local<v8::String> result;
      v8::Persistent<v8::Object> obj;
      v8::Persistent<v8::String> responseText;
      v8::Persistent<v8::Integer> status;
      v8::Persistent<v8::Integer> readyState;
      v8::Persistent<v8::String> localpath;
      v8::Persistent<v8::Value> onreadystatechange;

      XMLHttpRequest()
        {
          result = v8::String::Empty();
	  data = eina_binbuf_new();
        }
      ~XMLHttpRequest()
        {
	   eina_binbuf_free(data);
        }
      static int addFdCount()
        {
           fd_counter++;
	   return fd_counter;
	}

};
