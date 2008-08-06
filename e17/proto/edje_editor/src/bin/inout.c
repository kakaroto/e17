/*  Copyright (C) 2006-2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of Edje_editor.
 *  Edje_editor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Edje_editor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Edje_editor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <Etk.h>
#include "main.h"
#include "inout.h"

//~ #define MAIN_EDC_NAME "edje_editor_link.edc"

//~ Etk_Widget *Load_TextView;
//~ Etk_Widget *Load_ProgBar;
//~ Etk_Widget *Load_Win;
//~ Etk_Widget *Save_ProgBar;
//~ Etk_Widget *Save_TextView;
//~ Ecore_Timer *progress_bar_timer;
  
//Ecore_Event_Handler *eeh1, *eeh2, *eeh3;

//~ void
//~ etk_textview_append(Etk_Widget *tv, char *str)
//~ {
   //~ Etk_Textblock_Iter *iter;
   //~ if (!tv || !str) return;
   //~ iter = etk_text_view_cursor_get (ETK_TEXT_VIEW(tv));
   //~ etk_textblock_insert_markup(ETK_TEXT_VIEW(tv)->textblock, iter, 
                              //~ str, strlen(str));
//~ }

//~ void
//~ create_load_window()
//~ {
   //~ Etk_Widget *vbox;
   //~ Etk_Widget *scrolled_view;
   //~ Etk_Widget *button;
   //~ Etk_Widget *label;
   
   //~ //Load Win
   //~ Load_Win = etk_window_new();
   //~ etk_widget_size_request_set(Load_Win, 400, 300);
   //~ etk_window_title_set(ETK_WINDOW(Load_Win), "Loading...");
   //~ etk_signal_connect("destroyed", ETK_OBJECT(Load_Win),
                      //~ ETK_CALLBACK(etk_main_quit), NULL);

   //~ vbox = etk_vbox_new(ETK_FALSE, 0);
   //~ etk_container_add(ETK_CONTAINER(Load_Win), vbox);
   
   //~ label = etk_label_new("<font_size=20><b>Loading file</b></>");
   //~ etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   //~ etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_NONE, 5);

   //~ //Progress bar
   //~ Load_ProgBar = etk_progress_bar_new_with_text("Working...");
   //~ etk_progress_bar_pulse_step_set(ETK_PROGRESS_BAR(Load_ProgBar), 0.03);
   //~ etk_box_append(ETK_BOX(vbox), Load_ProgBar, ETK_BOX_START, ETK_BOX_NONE, 5);

   //~ //Text_view
   //~ Load_TextView = etk_text_view_new ();
   //~ scrolled_view = etk_scrolled_view_new();
   //~ etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view),
                                //~ ETK_POLICY_HIDE, ETK_POLICY_SHOW);
   //~ etk_bin_child_set( ETK_BIN(scrolled_view), Load_TextView);
   //~ etk_box_append(ETK_BOX(vbox), scrolled_view,
                  //~ ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);

   //~ //buttons
   //~ button = etk_button_new_from_stock(ETK_STOCK_DIALOG_CLOSE);
   //~ etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 5);
   //~ etk_signal_connect("clicked", ETK_OBJECT(button),
                      //~ ETK_CALLBACK(etk_main_quit), NULL);

   //~ etk_widget_show_all(Load_Win);
//~ }

//~ void
//~ create_save_window()
//~ {
   //~ Etk_Widget *Save_Win;
   //~ Etk_Widget *vbox;
   //~ Etk_Widget *scrolled_view;
   //~ Etk_Widget *button;
   //~ Etk_Widget *label;

   //~ //Save Win
   //~ Save_Win = etk_window_new();
   //~ etk_widget_size_request_set(Save_Win, 400, 300);
   //~ etk_window_title_set(ETK_WINDOW(Save_Win), "Saving...");

   //~ vbox = etk_vbox_new(ETK_FALSE, 0);
   //~ etk_container_add(ETK_CONTAINER(Save_Win), vbox);

   //~ label = etk_label_new("<font_size=20><b>Saving file</b></>");
   //~ etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   //~ etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_NONE, 5);

   //~ //Progress bar
   //~ Save_ProgBar = etk_progress_bar_new_with_text("Working...");
   //~ etk_progress_bar_pulse_step_set(ETK_PROGRESS_BAR(Save_ProgBar), 0.03);
   //~ etk_box_append(ETK_BOX(vbox), Save_ProgBar, ETK_BOX_START, ETK_BOX_NONE, 5);

   //~ //Text_view
   //~ Save_TextView = etk_text_view_new ();
   //~ scrolled_view = etk_scrolled_view_new();
   //~ etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view),
                                //~ ETK_POLICY_HIDE, ETK_POLICY_SHOW);
   //~ etk_bin_child_set( ETK_BIN(scrolled_view), Save_TextView);
   //~ etk_box_append(ETK_BOX(vbox), scrolled_view,
                  //~ ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);

   //~ //buttons
   //~ button = etk_button_new_from_stock(ETK_STOCK_DIALOG_CLOSE);
   //~ etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 5);
   //~ etk_signal_connect_swapped("clicked", ETK_OBJECT(button),
                              //~ ETK_CALLBACK(etk_widget_hide), Save_Win);

   //~ etk_widget_show_all(Save_Win); 
//~ }

//~ int
//~ backup_file(char *file_name)
//~ {
   //~ char bkname[4096];
   //~ if (!ecore_file_exists(file_name))
      //~ return 0;

   //~ snprintf(bkname,4095,"%s.bkp",file_name);

   //~ if (ecore_file_cp(file_name,bkname))
      //~ return TRUE;
   //~ else
      //~ return FALSE;
//~ }

//~ char*
//~ SaveEDC(char *file_name)
//~ {
   //~ //TODO
   //~ return NULL;
//~ }

//~ int
//~ SaveEDJ(char *file_name)
//~ {
   //~ char *tmpn;
   //~ char ipart[1024], fpart[1024];
   //~ char buf[4096];
   //~ char cmd[2048];
   //~ const char *imgdir, *fontdir;

   //~ create_save_window();

   //~ //Start progress_bar
   //~ progress_bar_timer = ecore_timer_add(0.025,
                     //~ (int(*)(void *data))etk_progress_bar_pulse, Save_ProgBar);

   //~ //backup existing file
   //~ backup_file(file_name);

   //~ //Create temp edc file
   //~ tmpn = SaveEDC(NULL);   //TODO remove the temp file
   //~ if (!tmpn)
   //~ {
      //~ snprintf(buf,4096,"<font color=#FF0000><b>Can't create edc tmp file:</b> %s\n</font>\n", tmpn);
      //~ etk_textview_append(Save_TextView,buf);
      //~ stop_bar(Save_ProgBar,"Error");
      //~ return 0;
   //~ }
   
   //~ snprintf(buf,4096,"<b>Created edc tmp file:</b> %s\n",tmpn);
   //~ etk_textview_append(Save_TextView,buf);
   
   //~ //Get image and font directory
   //~ imgdir = engrave_file_image_dir_get(Cur.ef);
   //~ fontdir = engrave_file_font_dir_get(Cur.ef);
   //~ snprintf(buf,4096,"<b>Image dir:</b> %s\n<b>Font dir:</b> %s\n",
            //~ imgdir,fontdir);
   //~ etk_textview_append(Save_TextView,buf);
   
   //~ if (imgdir) snprintf(ipart, sizeof(ipart), "-id %s", imgdir);
   //~ else ipart[0] = '\0';

   //~ if (fontdir) snprintf(fpart, sizeof(fpart), "-fd %s", fontdir);
   //~ else fpart[0] = '\0';

   //~ //Create edje_cc command line
   //~ snprintf(cmd, sizeof(cmd), "edje_cc -v %s %s %s \"%s\"",
            //~ ipart, fpart, tmpn, file_name);
   //~ snprintf(buf,4096,"<b>Executing: </b>%s\n",cmd);
   //~ etk_textview_append(Save_TextView, buf);

   //~ //Execute edje_cc through a pipe
   //~ eeh1 = ecore_event_handler_add(ECORE_EXE_EVENT_DATA, pipe_data, Save_TextView);
   //~ eeh2 = ecore_event_handler_add(ECORE_EXE_EVENT_ERROR, pipe_data, Save_TextView);
   //~ eeh3 = ecore_event_handler_add(ECORE_EXE_EVENT_DEL, save_pipe_exit, Save_TextView);
   //~ ecore_exe_pipe_run(cmd,
      //~ ECORE_EXE_PIPE_READ | ECORE_EXE_PIPE_READ_LINE_BUFFERED |
      //~ ECORE_EXE_PIPE_ERROR | ECORE_EXE_PIPE_ERROR_LINE_BUFFERED,
      //~ (void*)SAVE_WIN); 

   //~ return 1; //1 means success
//~ }

//~ void
//~ LoadEDJ(char *file)
//~ {
   //~ create_load_window();

   //~ ecore_timer_add(0.1,Decompile,file);

   //~ etk_main();
   //~ etk_widget_hide(Load_Win);
//~ }

//~ int Decompile(void *data)//data is the name of the file to open
//~ {
   //~ char *file = (char*)data;
   //~ char buf[4096];
   //~ char tmpn[4096];
   //~ char cmd[4096];
   //~ char *old_fname;
   //~ char *ptr;
   //~ char *work_dir = NULL;

   //~ progress_bar_timer = ecore_timer_add(0.025, 
                                    //~ (int(*)(void *data))etk_progress_bar_pulse,
                                    //~ Load_ProgBar);

   //~ //TODO Check source integrity (is an edje file?)
   //~ Cur.open_file_name = strdup(file);
   
   //~ //Create working dir
   //~ memset(tmpn, '\0', sizeof(tmpn));
   //~ strcpy(tmpn, "/tmp/edje_editor-tmp-XXXXXX");
//~ #if defined (__SVR4) && defined (__sun)
   //~ if (mkdir(tmpn, S_IRWXU | S_IRWXG) == NULL)
//~ #else 
   //~ if (mkdtemp(tmpn) == NULL)
//~ #endif
   //~ {
      //~ snprintf(buf,4096,"<font color=#FF0000><b>Can't create working dir:</b> %s\n<b>Error:</b> %s</font>\n",
               //~ tmpn,strerror(errno));
      //~ etk_textview_append(Load_TextView,buf);
      //~ stop_bar(Load_ProgBar,"Error");
      //~ return 0;
   //~ }
   //~ work_dir = strdup(tmpn);

   //~ //Change to the working dir
   //~ snprintf(buf,4096,"<b>Change to working dir:</b> %s\n",work_dir);
   //~ etk_textview_append(Load_TextView,buf);
   //~ getcwd(tmpn, sizeof(tmpn));
   //~ if (chdir(work_dir) == -1)
   //~ {
      //~ snprintf(buf,4096,"<font color=#FF0000><b>Can't change to dir:</b> %s\n<b>Error: </b> %s</font>\n",
               //~ work_dir,strerror(errno));
      //~ etk_textview_append(Load_TextView, buf);
      //~ stop_bar(Load_ProgBar, "Error");
      //~ return 0;
   //~ }

   //~ //Calculate Cur.source_dir, image_dir, font_dir, main_source_file
   //~ old_fname = strdup(file);
   //~ ptr = strstr(old_fname, ".edj");
   //~ *ptr = '\0';
   //~ ptr = strrchr(old_fname, '/');
   //~ if (ptr == NULL) ptr = old_fname;
   //~ snprintf(buf,4096,"%s%s",work_dir,ptr);
   //~ Cur.source_dir = strdup(buf);
   //~ snprintf(buf,4096,"%s/"MAIN_EDC_NAME,Cur.source_dir);
   //~ Cur.main_source_file = strdup(buf);

   //~ //Execute edje_decc through a pipe
   //~ snprintf(cmd, 4096, "edje_decc \"%s\" -main-out "MAIN_EDC_NAME, file);
   //~ snprintf(buf,4096,"<b>Executing: </b>%s\n",cmd);
   //~ etk_textview_append(Load_TextView, buf);

   //~ eeh1 = ecore_event_handler_add(ECORE_EXE_EVENT_DATA, pipe_data, Load_TextView);
   //~ eeh2 = ecore_event_handler_add(ECORE_EXE_EVENT_ERROR, pipe_data, Load_TextView);
   //~ eeh3 = ecore_event_handler_add(ECORE_EXE_EVENT_DEL, load_pipe_exit, NULL);
   //~ ecore_exe_pipe_run(cmd,
      //~ ECORE_EXE_PIPE_READ | ECORE_EXE_PIPE_READ_LINE_BUFFERED |
      //~ ECORE_EXE_PIPE_ERROR | ECORE_EXE_PIPE_ERROR_LINE_BUFFERED,
      //~ (void*)LOAD_WIN);

   //~ return 0; //Stop executing timer
//~ }

//~ int
//~ pipe_data(void *data, int ev_type, void *ev)//Data is Load_TextView or Save_TextView
//~ {
   //~ Ecore_Exe_Event_Data *e;
   //~ e = (Ecore_Exe_Event_Data *)ev;
   //~ Etk_Widget* text_view = data;

   //~ if (e->lines)
   //~ {
      //~ int i;
      //~ for (i = 0; e->lines[i].line != NULL; i++)
      //~ {
         //~ //printf("++ %d %s\n", e->lines[i].size, e->lines[i].line);
         //~ //Don't show edje_cc malicious warnings
         //~ if (strcmp(e->lines[i].line,"*** CAUTION ***"))
         //~ {
            //~ etk_textview_append(text_view,"<font color=#0000FF>");
            //~ etk_textview_append(text_view,e->lines[i].line);
            //~ etk_textview_append(text_view,"</font>\n");
         //~ }
         //~ else
            //~ i++;
      //~ }
      //~ //etk_textblock_object_yoffset_set(ETK_TEXT_VIEW(LS_TextView)->textblock, -200 );
   //~ }
   //~ return 1;
//~ }

//~ int
//~ save_pipe_exit(void *data, int ev_type, void *ev)//Data is LOAD_WIN or SAVE_WIN
//~ {
   //~ char buf[1024];
   //~ Ecore_Exe_Event_Del *e = (Ecore_Exe_Event_Del *)ev;

   //~ if (e->exit_code)
   //~ {
      //~ snprintf(buf,1024,
         //~ "<font color=#FF0000><b>Error in edje_cc, exit code: </b> %d</font>\n",
         //~ e->exit_code);
      //~ stop_bar(Save_ProgBar,"Error");
   //~ }
   //~ else
   //~ {
      //~ snprintf(buf,1024, "<font color=#00AA00><b>Operation completed.</b>\n");
      //~ stop_bar(Save_ProgBar, "Done !!");
   //~ }

   //~ etk_textview_append(Save_TextView,buf);
   
   //~ ecore_event_handler_del(eeh1);
   //~ ecore_event_handler_del(eeh2);
   //~ ecore_event_handler_del(eeh3);

   //~ return 0;
//~ }

//~ int
//~ load_pipe_exit(void *data, int ev_type, void *ev)//Data is LOAD_WIN or SAVE_WIN
//~ {
   //~ char buf[4096];
   //~ Ecore_Exe_Event_Del *e = (Ecore_Exe_Event_Del *)ev;
    
   //~ if (e->exit_code)
   //~ {
      //~ snprintf(buf,sizeof(buf),"<font color=#FF0000><b>Error in edje_decc, exit code: </b> %d</font>\n",
               //~ e->exit_code);
      //~ etk_textview_append(data,buf);
      //~ stop_bar(Load_ProgBar, "Error");
   //~ }
   //~ else
   //~ {
      //~ Cur.ef = engrave_load_edc(Cur.main_source_file,
                                //~ Cur.source_dir, Cur.source_dir);
      //~ if (!Cur.ef)
      //~ {
         //~ //TODO catch the engrave error and display it.
         //~ etk_textview_append(Load_TextView,"<font color=#FF0000><b>Error parsing edc file. See terminal output for more information.</b></font>\n");
         //~ stop_bar(Load_ProgBar, "Error");
      //~ }
      //~ else
      //~ {
         //~ etk_textview_append(Load_TextView,
                           //~ "<font color=#00AA00><b>Operation completed.</b>\n");
      
         //~ snprintf(buf,sizeof(buf),"<b>Main source file:</b> %s\n",
                  //~ Cur.main_source_file);
         //~ etk_textview_append(Load_TextView, buf);

         //~ snprintf(buf,sizeof(buf),"<b>Source dir:</b> %s\n",Cur.source_dir);
         //~ etk_textview_append(Load_TextView, buf);

         //~ snprintf(buf,sizeof(buf),"<b>Image dir:</b> %s\n",
                  //~ engrave_file_image_dir_get(Cur.ef));
         //~ etk_textview_append(Load_TextView, buf);

         //~ snprintf(buf,sizeof(buf),"<b>Font dir:</b> %s</font>\n",
                  //~ engrave_file_font_dir_get(Cur.ef));
         //~ etk_textview_append(Load_TextView, buf);
          
         //~ stop_bar(Load_ProgBar, "Done !!");
      //~ }
   //~ }
   //~ ecore_event_handler_del(eeh1);
   //~ ecore_event_handler_del(eeh2);
   //~ ecore_event_handler_del(eeh3);
   //~ return 0;
//~ }

//~ void
//~ stop_bar(Etk_Widget* pbar, char *text)
//~ {
   //~ ecore_timer_del(progress_bar_timer);
   //~ etk_progress_bar_text_set (ETK_PROGRESS_BAR(pbar), text);
   //~ etk_progress_bar_fraction_set (ETK_PROGRESS_BAR(pbar), 1.0);
//~ }
