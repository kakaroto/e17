#include <downloader.h>

using namespace v8;
extern int elev8_log_domain;

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
   INF( "FileName = %s", dl_temp->filename);
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
        INF("Ignore the event - Not for this URL");
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
	INF("Appended %d data", url_data->size);
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
   INF("Completed %s - %d", url , url_complete->status);
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
   INF("Trying to download resources");
   if (dl_temp->fp==NULL)
     {
        dl_temp->fp = fopen(dl_temp->filename, "r");
	if (dl_temp->fp==NULL)
          {
             ERR( "Cannot open script file");
	     return ;
	  }
     }

   char buf[PATH_MAX];
   memset(buf, '\0', PATH_MAX);

   while(fgets(buf, PATH_MAX, dl_temp->fp))
     {
        if (strstr(buf,"//ELEV8_IMAGE:"))
          {
             INF("Found New Resource %s", buf);
	     char *local = strchr(buf, ':') + 1;
	     dl_temp->http_request = strchr(local, ':');
	     *dl_temp->http_request = '\0';
             INF("LocalFile %s", local);
	     dl_temp->http_request++;
             char *nptr = strchr(dl_temp->http_request,'\n');
             *nptr = '\0';

             //FIXME : Cram everything in one place - folder will be cleaner
             char local_file[PATH_MAX];
             snprintf(local_file, PATH_MAX,
			      "%s/elev8-script-%d/%s",PACKAGE_TMP_DIR,getpid(),local);
	     INF("Local File = %s URL = %s",
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
	     INF("Skipping Added Line = %s", buf);
             continue;
	  }
	else if(strlen(buf) > 1)
          {
             INF("End of Resources");
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

   INF("Downloading Complete");
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
        ERR("error creating file %s ", strerror(errno));
        eina_binbuf_free(dl_temp->temp_data);
	return EINA_FALSE;
     }

   INF("File Created for Script %d", retval, dl_temp->fd);
   INF("File Write Size = %d-%d=%d",length,shebang,(length-shebang));

   retval = snprintf(set_datadir, PATH_MAX,
		             "elm.datadir = \"%s/elev8-script-%d/\";",
			     PACKAGE_TMP_DIR, 
			     getpid()
			);
   INF("DataDir Is Set to  %s--", set_datadir);

   ptr = &set_datadir[0];
   retval = write(dl_temp->fd, ptr, retval);
   ptr = dl_temp->http_response + shebang;
   retval = write(dl_temp->fd, ptr, (length - shebang));
   if (retval>0)
     {
        INF("Written %d bytes into file", retval);
     }
   else
     {
	ERR("error writing to file %s ", strerror(errno));
     }
	
   close(dl_temp->fd);

   eina_binbuf_free(dl_temp->temp_data);
   dl_temp->http_request = NULL;
   INF("Done with File %s", dl_temp->filename);
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

   INF( "Making request to %s", (char *)data);

   dl->win = elm_win_add(NULL, "elev8_viewer", ELM_WIN_BASIC);
   elm_win_title_set(dl->win, "ELEV8 VIEWER");
   evas_object_smart_callback_add(dl->win, "delete,request", ui_kill, dl);

   dl->bg = elm_bg_add(dl->win);
   char buf[PATH_MAX];
   snprintf(buf, PATH_MAX, "%s/data/images/bg.png", PACKAGE_DATA_DIR);
   INF("Path = %s", buf);
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

