#ifndef __ELEV8_DOWNLOADER_H_
#define __ELEV8_DOWNLOADER_H_
#include <sys/mman.h>
#include <fcntl.h>
#include <elev8_common.h>
#include <elev8_utils.h>

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

void compile_and_run(Handle<String> source);
void start_download(void *ptr, Evas_Object *obj, void *data);
void ui_kill(void *ptr, Evas_Object *obj, void *data);

#endif
