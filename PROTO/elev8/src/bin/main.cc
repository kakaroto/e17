/*
 * elev8 - javascript for EFL
 *
 * The script's job is to prepare for the main loop to run
 * then exit
 */

#include <Elementary.h>
#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Con.h>
#include <v8.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

using namespace v8;
int elev8_log_domain = -1;

typedef struct
{
   Ecore_Con_Url *url_con;
   Eina_Binbuf *temp_data;
   Ecore_Event_Handler *url_complete;
   char *http_response;
   char *http_request;
   FILE *fp;
   char filename[PATH_MAX];
   Evas_Object *win;
   Evas_Object *bg; 
   Evas_Object *title; 
   Evas_Object *log; 
   Evas_Object *content; 
   Evas_Object *pb; 
   Evas_Object *bx; 
   Evas_Object *bt; 
   int length;
   int fd;
} downloader;

/* FIXME */
void elm_v8_setup(Handle<ObjectTemplate> global);
int xmlhttp_v8_setup(Handle<ObjectTemplate> global);
int dbus_v8_setup(Handle<ObjectTemplate> global);
void start_download(void *ptr, Evas_Object *obj, void *data);
void download_resource(downloader *dl_temp);
void compile_and_run(Handle<String> source);


int shebang_length(const char *p, int len)
{
   int i = 0;

   if ((len > 2) && (p[0] == '#') && (p[1] == '!'))
     {
        for (i = 2; i < len && p[i] != '\n'; i++)
          ;
        /* leave first newline in so line numbers are correct */
     }

   return i;
}


Handle<Value>
Print(const Arguments& args)
{
   for (int i = 0; i < args.Length(); i++)
     {
        HandleScope handle_scope;
        String::Utf8Value str(args[i]);
        printf("%s%s", i ? " ":"", *str);
     }
   printf("\n");
   fflush(stdout);
   return Undefined();
}

Local<String>
string_from_file(const char *filename)
{
   Local<String> ret;
   int fd, len = 0;
   char *bad_ret = reinterpret_cast<char*>(MAP_FAILED);
   char *p = bad_ret;
   int n;

   fd = open(filename, O_RDONLY);
   if (fd < 0)
     goto fail;

   len = lseek(fd, 0, SEEK_END);
   if (len <= 0)
     goto fail;

   p = reinterpret_cast<char*>(mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0));
   if (p == bad_ret)
     goto fail;

   n = shebang_length(p, len);

   ret = String::New(&p[n], len - n);

fail:
   if (p == bad_ret)
     munmap(p, len);

   if (fd >= 0)
     close(fd);

   return ret;
}


void ui_kill(void *ptr, Evas_Object *obj, void *data)
{
   evas_object_del(((downloader *)ptr)->bg);
   evas_object_del(((downloader *)ptr)->bx);
   evas_object_del(((downloader *)ptr)->bt);
   evas_object_del(((downloader *)ptr)->pb);
   evas_object_del(((downloader *)ptr)->win);
}

void launch_script(void *ptr, Evas_Object *obj, void *data)
{
   downloader *dl_temp = (downloader *)ptr;

   ui_kill(dl_temp, NULL, NULL);
   EINA_LOG_DOM_INFO(elev8_log_domain, "FileName = %s", dl_temp->filename);
   Handle<String> source = string_from_file(dl_temp->filename);
   compile_and_run(source);
}

Eina_Bool file_data_callback(void *data, int type, void *event)
{
   Ecore_Con_Event_Url_Data *url_data = (Ecore_Con_Event_Url_Data *)event;
   void *ptr = ecore_con_url_data_get(url_data->url_con);
   downloader *dl_temp = (downloader *)data;

   if (ptr != dl_temp->http_request)
     {
        EINA_LOG_DOM_INFO(elev8_log_domain,"Ignore the event - Not for this URL");
	return ECORE_CALLBACK_PASS_ON;
     }

   if ( url_data->size > 0)
     {
        double progress;
        progress = elm_progressbar_value_get (dl_temp->pb);
        if (progress < 1.0) progress += 0.0123;
        else progress = 0.0;
        elm_progressbar_value_set(dl_temp->pb, progress);

        eina_binbuf_append_length(dl_temp->temp_data, 
			url_data->data, url_data->size);
	EINA_LOG_DOM_INFO(elev8_log_domain,"Appended %d data", url_data->size);
     }
   return EINA_FALSE;
}

static Eina_Bool
_url_complete_cb(void *data, int type, void *event_info)
{
   downloader *dl_temp = (downloader *)data;

   Ecore_Con_Event_Url_Complete *url_complete = 
	                     (Ecore_Con_Event_Url_Complete *)event_info;

   const char *url = ecore_con_url_url_get(url_complete->url_con);
   char buf[PATH_MAX];
   EINA_LOG_DOM_INFO(elev8_log_domain,"Completed %s - %d", url , url_complete->status);
   snprintf(buf, PATH_MAX, "<br>Completed %s - %d<br>", url , url_complete->status);
   elm_entry_entry_append(dl_temp->content, buf);


   double progress;
   progress = elm_progressbar_value_get (dl_temp->pb);
   if (progress < 1.0) progress += 0.0123;
   else progress = 0.0;
   elm_progressbar_value_set(dl_temp->pb, progress);
   //ecore_con_url_free(dl_temp->url_con);
   close(dl_temp->fd);
   download_resource(dl_temp);
   return EINA_TRUE;
}

void download_resource(downloader *dl_temp)
{
   EINA_LOG_DOM_INFO(elev8_log_domain,"Trying to download resources");
   if (dl_temp->fp==NULL)
     {
        dl_temp->fp = fopen(dl_temp->filename, "r");
	if (dl_temp->fp==NULL)
          {
             EINA_LOG_DOM_ERR(elev8_log_domain, "Cannot open script file");
	     return ;
	  }
     }

   char buf[PATH_MAX];
   memset(buf, '\0', PATH_MAX);

   while(fgets(buf, PATH_MAX, dl_temp->fp))
     {
        if (strstr(buf,"//ELEV8_IMAGE:"))
          {
             EINA_LOG_DOM_INFO(elev8_log_domain,"Found New Resource %s", buf);
	     char *local = strchr(buf, ':') + 1;
	     dl_temp->http_request = strchr(local, ':');
	     *dl_temp->http_request = '\0';
             EINA_LOG_DOM_INFO(elev8_log_domain,"LocalFile %s", local);
	     dl_temp->http_request++;
             char *nptr = strchr(dl_temp->http_request,'\n');
             *nptr = '\0';

             //FIXME : Cram everything in one place - folder will be cleaner
             char local_file[PATH_MAX];
             snprintf(local_file, PATH_MAX,
			      "%s/elev8-script-%d/%s",PACKAGE_TMP_DIR,getpid(),local);
	     EINA_LOG_DOM_INFO(elev8_log_domain,"Local File = %s URL = %s",
			        local_file, dl_temp->http_request);
	     mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
             dl_temp->fd =  open(local_file, O_CREAT|O_WRONLY|O_TRUNC, 0644);
             ecore_con_url_url_set(dl_temp->url_con, (char *)dl_temp->http_request);
             ecore_con_url_data_set(dl_temp->url_con, dl_temp->http_request);
             ecore_con_url_fd_set(dl_temp->url_con, dl_temp->fd);
             ecore_con_url_get(dl_temp->url_con);
	     break;
          }
	else if(strstr(buf,"elm.datadir = "))
          {
	     EINA_LOG_DOM_INFO(elev8_log_domain,"Skipping Added Line = %s", buf);
             continue;
	  }
	else if(strlen(buf) > 1)
          {
             EINA_LOG_DOM_INFO(elev8_log_domain,"End of Resources");
             ecore_con_url_free(dl_temp->url_con);
             elm_progressbar_value_set(dl_temp->pb, 1.0);
	     break;
          }
     }
   return;
}
Eina_Bool file_completion_callback(void *data, int type, void *event)
{
   Ecore_Con_Event_Url_Complete *url_complete = (Ecore_Con_Event_Url_Complete *)event;
   downloader *dl_temp = (downloader *)data;
   void *ptr = ecore_con_url_data_get(url_complete->url_con);

   if (ptr != dl_temp->http_request)
     {
	return ECORE_CALLBACK_PASS_ON;
     }

   EINA_LOG_DOM_INFO(elev8_log_domain,"Downloading Complete");
   dl_temp->http_response = (char *)eina_binbuf_string_get(dl_temp->temp_data);
   int length = eina_binbuf_length_get(dl_temp->temp_data);

   int shebang =  shebang_length(dl_temp->http_response, length);

   mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

   char set_datadir[PATH_MAX];
   snprintf(set_datadir, PATH_MAX,"%s/elev8-script-%d/",PACKAGE_TMP_DIR, getpid());
   int retval = mkdir(set_datadir, 0755);

   char *rptr = strrchr(dl_temp->http_request, '/');
   snprintf(dl_temp->filename, PATH_MAX, "%s%s",set_datadir,rptr);
   dl_temp->fd =  open(dl_temp->filename, O_RDWR | O_APPEND | O_CREAT , mode);

   if (dl_temp->fd<0)
     {
        EINA_LOG_DOM_ERR(elev8_log_domain,"error creating file %s ", strerror(errno));
        eina_binbuf_free(dl_temp->temp_data);
	return EINA_FALSE;
     }

   EINA_LOG_DOM_INFO(elev8_log_domain,"File Created for Script %d", retval, dl_temp->fd);
   EINA_LOG_DOM_INFO(elev8_log_domain,"File Write Size = %d-%d=%d",length,shebang,(length-shebang));

   retval = snprintf(set_datadir, PATH_MAX,
		             "elm.datadir = \"%s/elev8-script-%d/\";",
			     PACKAGE_TMP_DIR, 
			     getpid()
			);
   EINA_LOG_DOM_INFO(elev8_log_domain,"DataDir Is Set to  %s--", set_datadir);

   ptr = &set_datadir[0];
   retval = write(dl_temp->fd, ptr, retval);
   ptr = dl_temp->http_response + shebang;
   retval = write(dl_temp->fd, ptr, (length - shebang));
   if (retval>0)
     {
        EINA_LOG_DOM_INFO(elev8_log_domain,"Written %d bytes into file", retval);
     }
   else
     {
	EINA_LOG_DOM_ERR(elev8_log_domain,"error writing to file %s ", strerror(errno));
     }
	
   close(dl_temp->fd);

   eina_binbuf_free(dl_temp->temp_data);
   dl_temp->http_request = NULL;
   EINA_LOG_DOM_INFO(elev8_log_domain,"Done with File %s", dl_temp->filename);
   elm_object_disabled_set(dl_temp->bt, EINA_FALSE);
   elm_object_text_set(dl_temp->bt, "Launch");
   //evas_object_smart_callback_del(dl_temp->bt, "clicked", start_download);
   evas_object_show(dl_temp->bt);
   evas_object_smart_callback_add(dl_temp->bt, "clicked", launch_script, dl_temp);
   elm_progressbar_value_set(dl_temp->pb, 0.5);

   ecore_event_handler_del(dl_temp->url_complete);
   dl_temp->url_complete = 
	   ecore_event_handler_add(ECORE_CON_EVENT_URL_COMPLETE, 
			                      _url_complete_cb, dl_temp);

   download_resource(dl_temp);

   return EINA_TRUE;
}

// for now this looks like a callback - but only ptr is used
// allows for button based initiation later
void start_download(void *ptr, Evas_Object *obj, void *data)
{
   downloader *dl_temp = (downloader *)ptr;

   elm_object_disabled_set(dl_temp->bt, EINA_TRUE);

   dl_temp->url_con = ecore_con_url_new((char *)dl_temp->http_request);
   ecore_con_url_data_set(dl_temp->url_con, dl_temp->http_request );
   dl_temp->temp_data = eina_binbuf_new();
   ecore_event_handler_add( ECORE_CON_EVENT_URL_DATA,
                            file_data_callback,
                            dl_temp);
   dl_temp->url_complete = ecore_event_handler_add( ECORE_CON_EVENT_URL_COMPLETE,
                            file_completion_callback,
                            dl_temp);
   ecore_con_url_get(dl_temp->url_con);
}

void show_download_ui(void *data)
{

   downloader *dl = (downloader *)malloc(sizeof(downloader));;
   if (dl!=NULL)
     {
        memset(dl, 0, sizeof(downloader));
     }

   dl->http_request = (char *)data;

   EINA_LOG_DOM_INFO(elev8_log_domain, "Making request to %s", (char *)data);

   dl->win = elm_win_add(NULL, "elev8_viewer", ELM_WIN_BASIC);
   elm_win_title_set(dl->win, "ELEV8 VIEWER");
   evas_object_smart_callback_add(dl->win, "delete,request", ui_kill, dl);

   dl->bg = elm_bg_add(dl->win);
   char buf[PATH_MAX];
   snprintf(buf, PATH_MAX, "%s/data/images/bg.png", PACKAGE_DATA_DIR);
   EINA_LOG_DOM_INFO(elev8_log_domain,"Path = %s", buf);
   elm_bg_file_set(dl->bg, buf, NULL);
   evas_object_size_hint_align_set(dl->bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(dl->bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(dl->win, dl->bg);
   evas_object_show(dl->bg);

   dl->bx = elm_box_add(dl->win);
   elm_win_resize_object_add(dl->win, dl->bx);
   evas_object_size_hint_align_set(dl->bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(dl->bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(dl->bx);

   dl->title = elm_label_add(dl->win);
   elm_object_text_set(dl->title,"<title><b>ELEV8 VIEWER</b></title>");
   evas_object_size_hint_weight_set(dl->title, 0.0, 0.0);
   evas_object_size_hint_align_set(dl->title, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(dl->bx, dl->title);
   evas_object_show(dl->title);

   dl->pb = elm_progressbar_add(dl->win);
   evas_object_size_hint_weight_set(dl->pb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(dl->pb, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(dl->bx, dl->pb);
   evas_object_show(dl->pb);

   dl->log = elm_label_add(dl->win);
   elm_object_text_set(dl->log,"<title><b>Logs</b></title>");
   evas_object_size_hint_weight_set(dl->log, 0.0, 0.0);
   evas_object_size_hint_align_set(dl->log, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(dl->bx, dl->log);
   evas_object_show(dl->log);

   dl->content = elm_entry_add(dl->win);
   elm_entry_scrollable_set(dl->content, EINA_TRUE);
   evas_object_size_hint_weight_set(dl->content, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(dl->content, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_entry_entry_set(dl->content, "Downloading : ");
   elm_entry_entry_append(dl->content, dl->http_request);
   elm_entry_editable_set(dl->content, EINA_FALSE);
   elm_box_pack_end(dl->bx, dl->content);
   evas_object_show(dl->content);


   dl->bt = elm_button_add(dl->win);
   elm_object_text_set(dl->bt, "Fetch");
   evas_object_size_hint_align_set(dl->bt, EVAS_HINT_FILL, EVAS_HINT_EXPAND);
   evas_object_size_hint_weight_set(dl->bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   //evas_object_smart_callback_add(dl->bt, "clicked", start_download, dl);
   elm_box_pack_end(dl->bx, dl->bt);

   /* hide the button for now
    * In future we may want to enter a URL and download it 
    */
   evas_object_hide(dl->bt);
   start_download(dl,NULL, NULL);

   evas_object_resize(dl->win, 320, 480);
   evas_object_show(dl->win);
}

void
boom(TryCatch &try_catch)
{
   Handle<Message> msg = try_catch.Message();
   String::Utf8Value error(try_catch.Exception());

   if (msg.IsEmpty())
     {
        EINA_LOG_DOM_ERR(elev8_log_domain,"%s", *error);
     }
   else
     {
        String::Utf8Value file(msg->GetScriptResourceName());
        int line = msg->GetLineNumber();

        EINA_LOG_DOM_INFO(elev8_log_domain,"%s:%d %s", *file, line, *error);
     }
   exit(1);
}

void
run_script(const char *filename)
{
   HandleScope handle_scope;

   if (filename == strstr(filename, "http"))
     {
        EINA_LOG_DOM_INFO(elev8_log_domain,"Downloading elev8 Script");
        show_download_ui((void *)filename);
     }
   else
     {
        /* load the script and run it */
        Handle<String> origin = String::New(filename);
        Handle<String> source = string_from_file(filename);
        if (source.IsEmpty())
          {
             EINA_LOG_DOM_ERR(elev8_log_domain,"Failed to read source %s", filename);
             return;
          }
        compile_and_run(source);
      }
}

void compile_and_run(Handle<String> source)
{
   
   TryCatch try_catch;
   /* compile */
   Handle<Script> script = Script::Compile(source);
   if (try_catch.HasCaught())
     boom(try_catch);

   /* run */
   Handle<Value> result = script->Run();
   if (try_catch.HasCaught())
     boom(try_catch);
}

void
elev8_run(const char *script)
{
   HandleScope handle_scope;
   Handle<ObjectTemplate> global = ObjectTemplate::New();

   global->Set(String::New("print"), FunctionTemplate::New(Print));

   elm_v8_setup(global);
   int retval = xmlhttp_v8_setup(global);

   if (retval!=0)
     {
        EINA_LOG_DOM_ERR(elev8_log_domain,"Cannot initialize ecore_con_url");
	    //FIXME : Disable XMLHttpRequest support
     }

   retval = dbus_v8_setup(global);

   if (retval!=0)
     {
        EINA_LOG_DOM_ERR(elev8_log_domain,"Cannot initialize e_dbus");
	    //FIXME : Disable DBUS support
     }

   /* setup V8 */
   Persistent<Context> context = Context::New(NULL, global);
   Context::Scope context_scope(context);
   run_script(script);



   ecore_main_loop_begin();

   context.Dispose();
}

static void
main_help(const char *progname)
{
   printf("Usage:\n"
       "\t%s input_file.js\n"
       "\te.g. %s ../../data/javascript/button.js\n",
	progname, progname);
}

int
elm_main(int argc, char **argv)
{
   int i;

   eina_init();
   elev8_log_domain = eina_log_domain_register("elev8", EINA_COLOR_ORANGE);
   if (!elev8_log_domain)
     {
        printf("cannot set elev8 log domain\n");
        EINA_LOG_DOM_ERR(elev8_log_domain, "could not register elev8 log domain.");
        elev8_log_domain = EINA_LOG_DOMAIN_GLOBAL;
     }
   EINA_LOG_DOM_INFO(elev8_log_domain,"elev8 Logging initialized. %d", elev8_log_domain);

   if (argc < 2)
     {
        EINA_LOG_DOM_ERR(elev8_log_domain,"%s: Error: no input file specified.", argv[0]);
        main_help(argv[0]);
        exit(-1);
     }


   V8::SetFlagsFromCommandLine(&argc, argv, true);
   elev8_run(argv[1]);
   V8::Dispose();

   return 0;
}

ELM_MAIN()
