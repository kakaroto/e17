#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <Etk.h>
#include "main.h"
#include "inout.h"


#define MAIN_EDC_NAME "edje_editor_link.edc"

Etk_Widget *Load_TextView;
Etk_Widget *Load_ProgBar;
Etk_Widget *Load_Win;
Etk_Widget *Save_ProgBar;
Etk_Widget *Save_TextView;
Ecore_Timer *progress_bar_timer;


void 
etk_textview_append(Etk_Widget *tv, char *str)
{
   Etk_Textblock_Iter *iter;
   iter = etk_text_view_cursor_get (ETK_TEXT_VIEW(tv));
   etk_textblock_insert_markup(ETK_TEXT_VIEW(tv)->textblock, iter, 
                              str, strlen(str));
}

void
create_load_window()
{
   Etk_Widget *vbox;
   Etk_Widget *scrolled_view;
   Etk_Widget *button;
   Etk_Widget *label;
   
   //Load Win
   Load_Win = etk_window_new();
   etk_widget_size_request_set(Load_Win, 400, 300);
   etk_window_title_set(ETK_WINDOW(Load_Win), "Loading...");
   etk_signal_connect("destroyed", ETK_OBJECT(Load_Win), ETK_CALLBACK(etk_main_quit), NULL);
 
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(Load_Win), vbox);
   
   label = etk_label_new("<font_size=20><b>Loading file</b></>");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_NONE, 5);
   
   //Progress bar
   Load_ProgBar = etk_progress_bar_new_with_text("Working...");
   etk_progress_bar_pulse_step_set(ETK_PROGRESS_BAR(Load_ProgBar), 0.03);
   etk_box_append(ETK_BOX(vbox), Load_ProgBar, ETK_BOX_START, ETK_BOX_NONE, 5);
 
   //Text_view
   Load_TextView = etk_text_view_new ();
   //etk_textblock_object_cursor_visible_set(ETK_TEXT_VIEW(LS_TextView)->textblock, ETK_FALSE); //TODO questa riga non funziona
   scrolled_view = etk_scrolled_view_new();
   etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_POLICY_HIDE, ETK_POLICY_SHOW);
   etk_bin_child_set( ETK_BIN(scrolled_view), Load_TextView);
   etk_box_append(ETK_BOX(vbox), scrolled_view, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);
 
   //buttons
   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_CLOSE);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 5);
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(etk_main_quit), NULL);
   
   etk_widget_show_all(Load_Win);
}

void 
create_save_window()
{
   Etk_Widget *Save_Win;
   Etk_Widget *vbox;
   Etk_Widget *scrolled_view;
   Etk_Widget *button;
   Etk_Widget *label;
   
   //Save Win
   Save_Win = etk_window_new();
   etk_widget_size_request_set(Save_Win, 400, 300);
   etk_window_title_set(ETK_WINDOW(Save_Win), "Saving...");
   //etk_signal_connect("delete_event_signal", ETK_OBJECT(Save_Win), ETK_CALLBACK(NULL), NULL);
 
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(Save_Win), vbox);
   
   label = etk_label_new("<font_size=20><b>Saving file</b></>");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_NONE, 5);
   
   //Progress bar
   Save_ProgBar = etk_progress_bar_new_with_text("Working...");
   etk_progress_bar_pulse_step_set(ETK_PROGRESS_BAR(Save_ProgBar), 0.03);
   etk_box_append(ETK_BOX(vbox), Save_ProgBar, ETK_BOX_START, ETK_BOX_NONE, 5);
 
   //Text_view
   Save_TextView = etk_text_view_new ();
   //etk_textblock_object_cursor_visible_set(ETK_TEXT_VIEW(LS_TextView)->textblock, ETK_FALSE); //TODO questa riga non funziona
   scrolled_view = etk_scrolled_view_new();
   etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_POLICY_HIDE, ETK_POLICY_SHOW);
   etk_bin_child_set( ETK_BIN(scrolled_view), Save_TextView);
   etk_box_append(ETK_BOX(vbox), scrolled_view, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);
 
   //buttons
   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_CLOSE);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 5);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button), ETK_CALLBACK(etk_widget_hide), Save_Win);

   etk_widget_show_all(Save_Win); 
}

int
backup_file(char *file_name)
{
   char bkname[4096];
   if (!ecore_file_exists(file_name))
      return 0;
   
   snprintf(bkname,4095,"%s.bkp",file_name);
   
   if (ecore_file_cp(file_name,bkname))
      return TRUE;
   else
      return FALSE;
}

int
SaveEDJ(char *file_name)
{
   char tmpn[1024];
   char ipart[1024], fpart[1024];
   char buf[4096];
   char cmd[2048];
   int fd = 0;
   const char *imgdir, *fontdir;
   
   create_save_window();
   
   //Start progress_bar
   progress_bar_timer = 
      ecore_timer_add(0.025, (int(*)(void *data))etk_progress_bar_pulse, Save_ProgBar);
   
   //backup existing file
   backup_file(file_name);
   
   //Create tmp file
   strcpy(tmpn, "/tmp/edje_editor_tmp.edc-XXXXXX");
   fd = mkstemp(tmpn);
   if (fd < 0)
   {
      snprintf(buf,4096,"<font color=#FF0000><b>Can't create temp file:</b> %s\n<b>Error:</b> %s</font>\n",tmpn,strerror(errno));
      etk_textview_append(Save_TextView,buf);
      stop_bar(Save_ProgBar);
      return 0;
   }
   close(fd);
   snprintf(buf,4096,"<b>Create temp file:</b> %s\n",tmpn);
   etk_textview_append(Save_TextView,buf);

   //TODO check engrave success
   engrave_edc_output(Cur.ef, tmpn);

   //Get image and font directory
   imgdir = engrave_file_image_dir_get(Cur.ef);
   fontdir = engrave_file_font_dir_get(Cur.ef);
   snprintf(buf,4096,"<b>Image dir:</b> %s\n<b>Font dir:</b> %s\n",imgdir,fontdir);
   etk_textview_append(Save_TextView,buf);
   
   if (imgdir) snprintf(ipart, sizeof(ipart), "-id %s", imgdir);
   else ipart[0] = '\0';

   if (fontdir) snprintf(fpart, sizeof(fpart), "-fd %s", fontdir);
   else fpart[0] = '\0';

   //Create edje_cc command line
   snprintf(cmd, sizeof(cmd), "edje_cc -v %s %s %s \"%s\"", 
                                      ipart, fpart, tmpn, file_name);
   snprintf(buf,4096,"<b>Executing: </b>%s\n",cmd);
   etk_textview_append(Save_TextView, buf);
   
   //Execute edje_cc through a pipe
   Ecore_Exe* exe;
   ecore_event_handler_add(ECORE_EXE_EVENT_DATA, exe_data, Save_TextView);
   ecore_event_handler_add(ECORE_EXE_EVENT_DEL, exe_exit, NULL);
   exe = ecore_exe_pipe_run(cmd,
      ECORE_EXE_PIPE_READ | ECORE_EXE_PIPE_READ_LINE_BUFFERED,(void*)SAVE_WIN); 
   
   return 1; //1 means success
}

void
LoadEDJ(char *file)
{
   create_load_window();
   
   ecore_timer_add(0.1,Decompile,file);
   
   etk_main();
   etk_widget_hide(Load_Win);
}

int Decompile(void *data) //data is the name of the file to open
{
   char *file = (char*)data;
   char buf[4096];
   char tmpn[4096];
   char cmd[4096];
   char *old_fname;
   char *ptr;
   char *work_dir = NULL;

   progress_bar_timer = 
      ecore_timer_add(0.025, (int(*)(void *data))etk_progress_bar_pulse, Load_ProgBar);

   //TODO Check source integrity (is an edje file?)
   Cur.open_file_name = strdup(file);
   
   //Create working dir
   memset(tmpn, '\0', sizeof(tmpn));
   strcpy(tmpn, "/tmp/edje_editor-tmp-XXXXXX");
#if defined (__SVR4) && defined (__sun)
   if (mkdir(tmpn, S_IRWXU | S_IRWXG) == NULL)
#else 
   if (mkdtemp(tmpn) == NULL)
#endif
   {
      snprintf(buf,4096,"<font color=#FF0000><b>Can't create working dir:</b> %s\n<b>Error:</b> %s</font>\n",tmpn,strerror(errno));
      etk_textview_append(Load_TextView,buf);
      stop_bar(Load_ProgBar);
      return 0;
   }
   work_dir = strdup(tmpn);
   
   //Change to the working dir
   snprintf(buf,4096,"<b>Change to working dir:</b> %s\n",work_dir);
   etk_textview_append(Load_TextView,buf);
   getcwd(tmpn, sizeof(tmpn));
   if (chdir(work_dir) == -1) {
      snprintf(buf,4096,"<font color=#FF0000><b>Can't change to dir:</b> %s\n<b>Error: </b> %s</font>\n",work_dir,strerror(errno));
      etk_textview_append(Load_TextView, buf);
      stop_bar(Load_ProgBar);
      return 0;
   }
   
   //Calculate Cur.source_dir, image_dir, font_dir, main_source_file
   old_fname = strdup(file);
   ptr = strstr(old_fname, ".edj");
   *ptr = '\0';
   ptr = strrchr(old_fname, '/');
   if (ptr == NULL) ptr = old_fname;
   snprintf(buf,4096,"%s%s",work_dir,ptr);
   Cur.source_dir = strdup(buf);
   snprintf(buf,4096,"%s/"MAIN_EDC_NAME,Cur.source_dir);
   Cur.main_source_file = strdup(buf);
   
   //Execute edje_decc through a pipe
   snprintf(cmd, 4096, "edje_decc \"%s\" -main-out "MAIN_EDC_NAME, file);
   snprintf(buf,4096,"<b>Executing: </b>%s\n",cmd);
   etk_textview_append(Load_TextView, buf);
   
   Ecore_Exe* exe;
   ecore_event_handler_add(ECORE_EXE_EVENT_DATA, exe_data, Load_TextView);
   ecore_event_handler_add(ECORE_EXE_EVENT_DEL, exe_exit, NULL);
   exe = ecore_exe_pipe_run(cmd,
      ECORE_EXE_PIPE_READ | ECORE_EXE_PIPE_READ_LINE_BUFFERED,(void*)LOAD_WIN); 
   
   return 0; //Stop executing timer
}

int 
exe_data(void *data, int ev_type, void *ev) // Data is Load_TextView or Save_TextView
{
   Ecore_Exe_Event_Data *e;
   e = (Ecore_Exe_Event_Data *)ev;
   Etk_Widget* text_view = data;

   if (e->lines)
   {
      int i;
      for (i = 0; e->lines[i].line != NULL; i++)
      {
         //printf("++ %d %s\n", e->lines[i].size, e->lines[i].line);
         //Don't show edje_cc malicious warnings
         if (strcmp(e->lines[i].line,"*** CAUTION ***"))
         {
            etk_textview_append(text_view,"<font color=#0000FF>");
            etk_textview_append(text_view,e->lines[i].line);
            etk_textview_append(text_view,"</font>\n");
         }
         else
            i++;
      }
      //etk_textblock_object_yoffset_set(ETK_TEXT_VIEW(LS_TextView)->textblock, -200 );
   }
   
   return 1;
}

int 
exe_exit(void *data, int ev_type, void *ev) // Data is LOAD_WIN or SAVE_WIN
{
   char buf[1024];
   Ecore_Exe_Event_Del *e = (Ecore_Exe_Event_Del *)ev;
   
   if (e->exit_code){
      snprintf(buf,1024,"<font color=#FF0000><b>Error in edje_cc/decc, exit code: </b> %d</font>\n",e->exit_code);
      etk_textview_append(data,buf);
      return 1;
   }
  
   if (ecore_exe_data_get(e->exe) == (void*)LOAD_WIN) //if Load Win
   {
      Cur.ef = engrave_load_edc(Cur.main_source_file, Cur.source_dir, Cur.source_dir);
      if (!Cur.ef)
      {
         etk_textview_append(Load_TextView,"<font color=#FF0000><b>Error parsing edc file.</b>\n");
      }
      else
      {
         
         etk_textview_append(Load_TextView,"<font color=#00AA00><b>Operation completed.</b>\n");
      
         snprintf(buf,1024,"<b>Main source file:</b> %s\n",Cur.main_source_file);
         etk_textview_append(Load_TextView, buf);

         snprintf(buf,1024,"<b>Source dir:</b> %s\n",Cur.source_dir);
         etk_textview_append(Load_TextView, buf);

         snprintf(buf,1024,"<b>Image dir:</b> %s\n",engrave_file_image_dir_get(Cur.ef));
         etk_textview_append(Load_TextView, buf);

         snprintf(buf,1024,"<b>Font dir:</b> %s</font>\n",engrave_file_font_dir_get(Cur.ef));
         etk_textview_append(Load_TextView, buf);
      }
      stop_bar(Load_ProgBar);
   }
   else //SAVE_WIN
   {
      stop_bar(Save_ProgBar);
      etk_textview_append(Save_TextView,"<font color=#00AA00><b>Operation completed.</b>\n");
   }
   
   //TODO Check engrave load success
   return 0;
}

void
stop_bar(Etk_Widget* pbar)
{
   ecore_timer_del(progress_bar_timer);
   etk_progress_bar_text_set (ETK_PROGRESS_BAR(pbar), "Done!");
   etk_progress_bar_fraction_set (ETK_PROGRESS_BAR(pbar), 1.0);
}
