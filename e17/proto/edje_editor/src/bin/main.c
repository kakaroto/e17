#include "config.h"
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include <Etk.h>
#include <Engrave.h>

#include "evas.h"
#include "interface.h"
#include "main.h"

#define MAIN_EDC_NAME "edje_editor_link.edc"


/**
 * engrave_part_state_remove - remove the state from the part.
 * @param ep: The Engrave_Part to remove the state to.
 * @param eps: The Engrave_Part_State to remove.
 *
 * @return Returns no value.
 */
/*EAPI*/ void
PROTO_engrave_part_state_remove(Engrave_Part *ep, Engrave_Part_State *eps)
{
   if (!eps || !ep) return;

   //If eps its the current one then set current to NULL
   if (eps == engrave_part_current_state_get(ep))
      ep->current_state = NULL;

   engrave_part_state_parent_set(eps, NULL);

   ep->states = evas_list_remove(ep->states, eps);
}

/**
 * engrave_group_part_remove - remove the given part from the group
 * @param eg: The Engrave_Group to remove the part too.
 * @param ep: The Engrave_Part to remove.
 *
 * @return Returns no value.
 */
/*EAPI*/ void
PROTO_engrave_group_part_remove(Engrave_Group *eg, Engrave_Part *ep)
{
  Engrave_Group * group;
  Evas_List * list;

  if (!eg || !ep) return;

  group = (Engrave_Group *) engrave_part_parent_get(ep);

  if (ep->name)
  {
    // for all the programs in the group
    for (list = group->programs; list; list = list->next)
    {
      Engrave_Program * ep2;
      Evas_List * list2;
      ep2 = (Engrave_Program *) list->data;

      // if source matches, update
      if (ep2->source && !strcmp(ep2->source, ep->name))
        IF_FREE(ep2->source);

      // if any of the targets match, update too
      for (list2 = ep2->targets; list2; list2 = list2->next)
      {
        char * n;
        n = (char *) list2->data;
        if (n && !strcmp(n, ep->name))
        {
          ep2->targets = evas_list_remove(ep2->targets, n);
          IF_FREE(n);
        }
      }
    }

    // for all other parts in the group
    for (list = group->parts; list; list = list->next)
    {
      Engrave_Part * ep2;
      Evas_List * list2;

      ep2 = (Engrave_Part *) list->data;
      // for each state
      for (list2 = ep2->states; list2; list2 = list2->next)
      {
        Engrave_Part_State * eps2;
        eps2 = (Engrave_Part_State *) list2->data;

        if (eps2->rel1.to_x != NULL && !strcmp(eps2->rel1.to_x, ep->name))
          IF_FREE(eps2->rel1.to_x);

        if (eps2->rel1.to_y != NULL && !strcmp(eps2->rel1.to_y, ep->name))
          IF_FREE(eps2->rel1.to_y);

        if (eps2->rel2.to_x != NULL && !strcmp(eps2->rel2.to_x, ep->name))
          IF_FREE(eps2->rel2.to_x);

        if (eps2->rel2.to_y != NULL && !strcmp(eps2->rel2.to_y, ep->name))
          IF_FREE(eps2->rel2.to_y);
      }
    }
  }

  if (ep->object) evas_object_del(ep->object);

  eg->parts = evas_list_remove(eg->parts,ep);
  engrave_part_parent_set(ep, NULL);
}
/**
 * engrave_file_group_remove - remove the group from the given file
 * @param ef: The Engrave_File to remove the group too.
 * @param eg: The Engrave_Group to remove.
 * 
 * @return Returns no value.
 */
/*EAPI*/ void
PROTO_engrave_file_group_remove(Engrave_File *ef, Engrave_Group *eg)
{
   if (!ef || !eg) return;

  // if (ecanvas.current_group == eg) TODO: quando le altre due sono state "commesse"
  //    ecanvas.current_group = NULL;
   ef->groups = evas_list_remove(ef->groups, eg);
   engrave_group_parent_set(eg, NULL);
}

void DebugInfo(int full)
{
   Engrave_Group *gro;
   Engrave_Part *par;
   Engrave_Part_State *sta;
   Evas_List *gp,*pp,*sp;

   printf("\n\n ********************* D E B U G ***************************\n");
   printf(" ** open file name: %s\n",Cur.open_file_name);
   printf(" ** main source file: %s\n",Cur.main_source_file);
   printf(" ** sources directory: %s\n",Cur.source_dir);
   printf(" ** images directory: %s\n",engrave_file_image_dir_get(Cur.ef));
   printf(" ** fonts directory: %s\n",engrave_file_font_dir_get(Cur.ef));
   printf(" **\n");
   printf(" ** edje_editor.edj: %s\n",EdjeFile);
  /* printf("Current engrave file: %s\n",Cur.ef->);
   printf("Current EDCFileDir: %s\n",EDCFileDir->str);
   */
   printf(" ** Tot groups: %d\n",engrave_file_groups_count (Cur.ef));
   printf(" ** Tot images: %d\n",engrave_file_images_count (Cur.ef));
   printf(" ** Tot fonts: %d\n",engrave_file_fonts_count (Cur.ef));
   printf(" ** Tot spectra: %d\n",engrave_file_spectra_count (Cur.ef));
   printf(" ** Tot styles: %d\n",engrave_file_styles_count (Cur.ef));
   printf(" ** Tot data: %d\n",engrave_file_data_count (Cur.ef));
   printf(" **\n");
   if (Cur.eg)
      printf(" ** Cur group: %s\n",Cur.eg->name);
   else
      printf(" ** Cur group: (NULL)\n");
   if (Cur.ep)
      printf(" ** Cur part: %s\n",Cur.ep->name);
   else
      printf(" ** Cur part: (NULL)\n");
   if (Cur.eps)
      printf(" ** Cur state: %s %.2f\n",Cur.eps->name, Cur.eps->value);
   else
      printf(" ** Cur state: (NULL)\n");

   if (full)
   {
      printf(" ********************* F U L L **********************\n");
      for (gp = Cur.ef->groups; gp ;gp = gp->next)
      {
         gro = gp->data;
         printf(" ** Group: %s (%d par)\n",gro->name,engrave_group_parts_count(gro));
         for (pp = gro->parts; pp ;pp = pp->next)
         {
            par = pp->data;
            printf(" **    Part: %s \n",par->name);
            for (sp = par->states; sp; sp = sp->next)
            {
               sta = sp->data;
               printf(" **       State: %s %.2f\n",sta->name,sta->value);
            }
         }
      }
   }

   printf(" *********************** E N D *****************************\n\n");
}




//Etk_Widget *LABEL;
Etk_Widget *UI_LoadTextView;
Etk_Widget *UI_LoadPBar;
Ecore_Timer *progress_bar_timer;


void 
etk_textview_append(Etk_Widget *tv, char *str)
{
   Etk_Textblock_Iter *iter;
   iter = etk_text_view_cursor_get (ETK_TEXT_VIEW(tv));
	etk_textblock_insert_markup(ETK_TEXT_VIEW(tv)->textblock, iter, 
                              str, strlen(str));
}

int 
exe_data(void *data, int ev_type, void *ev)
{
   Ecore_Exe_Event_Data *e;
   e = (Ecore_Exe_Event_Data *)ev;
   
   if (e->lines)
   {
      int i;
      for (i = 0; e->lines[i].line != NULL; i++)
      {
         //printf("++ %d %s\n", e->lines[i].size, e->lines[i].line);
         etk_textview_append(UI_LoadTextView,e->lines[i].line);
         etk_textview_append(UI_LoadTextView,"\n");
      }
      //etk_textblock_object_yoffset_set(ETK_TEXT_VIEW(UI_LoadTextView)->textblock, -200 );
   }
   
   return 1;
}

void
stop_bar(void)
{
   ecore_timer_del(progress_bar_timer);
   etk_progress_bar_text_set (ETK_PROGRESS_BAR(UI_LoadPBar), "Done!");
   etk_progress_bar_fraction_set (ETK_PROGRESS_BAR(UI_LoadPBar), 1.0);
}

int 
exe_exit(void *data, int ev_type, void *ev)
{
   char buf[1024];
   Ecore_Exe_Event_Del *e = (Ecore_Exe_Event_Del *)ev;
  
   stop_bar();
   if (e->exit_code){
      snprintf(buf,1024,"<b>Error in edje_decc, exit code: </b> %d\n",e->exit_code);
      etk_textview_append(UI_LoadTextView,buf);
      return 1;
   }
   
   etk_textview_append(UI_LoadTextView,"<b>Decompile complete.</b>\n");
   
   
   Cur.ef = engrave_load_edc(Cur.main_source_file, Cur.source_dir, Cur.source_dir);
   
   snprintf(buf,1024,"<b>Main source file:</b> %s\n",Cur.main_source_file);
   etk_textview_append(UI_LoadTextView, buf);
   
   snprintf(buf,1024,"<b>Source dir:</b> %s\n",Cur.source_dir);
   etk_textview_append(UI_LoadTextView, buf);
   
   snprintf(buf,1024,"<b>Image dir:</b> %s\n",engrave_file_image_dir_get(Cur.ef));
   etk_textview_append(UI_LoadTextView, buf);
   
   snprintf(buf,1024,"<b>Font dir:</b> %s\n",engrave_file_font_dir_get(Cur.ef));
   etk_textview_append(UI_LoadTextView, buf);
   
   
   
   //TODO Check engrave load success
   etk_main_quit();
   return 0;
}


int Decompile(void *data) //data is the name of the file to open
{
   char buf[4096];
   char tmpn[4096];
   char cmd[4096];
   char *file = (char*)data;
   char *old_fname;
   char *ptr;
   char *work_dir = NULL;

   
   progress_bar_timer = 
      ecore_timer_add(0.025, etk_progress_bar_pulse, UI_LoadPBar);

   //TODO Check source integrity (is an edje file?
   Cur.open_file_name = strdup(file);
   
   //Create working dir
   memset(tmpn, '\0', sizeof(tmpn));
   strcpy(tmpn, "/tmp/edje_editor-tmp-XXXXXX");
#if defined (__SVR4) && defined (__sun)
  if (mkdir(tmpn, S_IRWXU | S_IRWXG) == NULL) {
#else 
  if (mkdtemp(tmpn) == NULL) {
#endif
      snprintf(buf,4096,"<font color=#FF0000><b>Can't create working dir:</b> %s\n<b>Error:</b> %s</font>\n",tmpn,strerror(errno));
      etk_textview_append(UI_LoadTextView,buf);
      stop_bar();
      return 0;
   }
   work_dir = strdup(tmpn);
   
   //Change to the working dir
   snprintf(buf,4096,"<b>Change to working dir:</b> %s\n",work_dir);
   etk_textview_append(UI_LoadTextView,buf);
   getcwd(tmpn, sizeof(tmpn));
   if (chdir(work_dir) == -1) {
      snprintf(buf,4096,"<font color=#FF0000><b>Can't change to dir:</b> %s\n<b>Error: </b> %s</font>\n",work_dir,strerror(errno));
      etk_textview_append(UI_LoadTextView, buf);
      stop_bar();
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
   snprintf(cmd, 4096, "edje_decc %s -main-out "MAIN_EDC_NAME, file);
   snprintf(buf,4096,"<b>Executing: </b>%s\n",cmd);
   etk_textview_append(UI_LoadTextView, buf);
   
   Ecore_Exe* exe;
   Ecore_Event_Handler *handler1 = NULL;
   Ecore_Event_Handler *handler2 = NULL;
   handler1 = ecore_event_handler_add(ECORE_EXE_EVENT_DATA, exe_data, NULL);
   handler2 = ecore_event_handler_add(ECORE_EXE_EVENT_DEL, exe_exit, NULL);
   exe = ecore_exe_pipe_run(cmd,ECORE_EXE_PIPE_READ | ECORE_EXE_PIPE_READ_LINE_BUFFERED,"test"); 
   
   /* if (!(pipe = popen(cmd, "r"))){
      etk_textview_append(UI_LoadTextView,"<font color=#FF0000>Error executing edje_decc</font>\n");
		return 0;
	}
	while (fgets(pbuf, 4096, pipe) != NULL){
       etk_textview_append(UI_LoadTextView, pbuf);
	}
	pclose(pipe);
    */

   /* we need the info on the work dir to pass the cpp so it can
   * include files correctly 
   */
/*    ptr = strstr(old_fname, ".edj");
   *ptr = '\0';
    
   ptr = strrchr(old_fname, '/');
   if (ptr == NULL)
      ptr = old_fname;
   
   
   len = strlen(ptr) + strlen(work_dir) + 2;
   out_dir = (char *)calloc(len, sizeof(char));
   snprintf(out_dir, len, "%s/%s", work_dir, ptr);
   etk_textview_append(UI_LoadTextView, "<b>Out dir:</b> ");
   etk_textview_append(UI_LoadTextView, out_dir);
   etk_textview_append(UI_LoadTextView, "\n");

   len = strlen(out_dir) + strlen(MAIN_EDC_NAME) + 2;
   new_fname = (char *)calloc(len, sizeof(char));
   snprintf(new_fname, len, "%s/%s", out_dir, MAIN_EDC_NAME);
   //FREE(old_fname);
   etk_textview_append(UI_LoadTextView, "<b>Main edc:</b> ");
   etk_textview_append(UI_LoadTextView, new_fname);
   etk_textview_append(UI_LoadTextView, "\n");

   
   Cur.ef = engrave_load_edc(new_fname, out_dir, out_dir);
   
 */
   //ecore_timer_add(0,cb,NULL);
   return 0;
}
void
LoadEDJ(char *file)
{
   Etk_Widget *win;
   Etk_Widget *vbox;
   Etk_Widget *scrolled_view;
   Etk_Widget *button;
   Etk_Widget *label;
   
   
   //Load Win
   win = etk_window_new();
   etk_widget_size_request_set(win, 400, 300);
   etk_window_title_set(ETK_WINDOW(win), "Loading...");
   etk_signal_connect("destroyed", ETK_OBJECT(win), ETK_CALLBACK(etk_main_quit), NULL);
  // etk_signal_connect("show", ETK_OBJECT(win), ETK_CALLBACK(on_LoadWin_realize), NULL);
 
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   label = etk_label_new("<font_size=20><b>Loading</b></>");
   etk_object_properties_set (ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_NONE, 5);
   
   UI_LoadPBar = etk_progress_bar_new_with_text("Loading...");
   etk_progress_bar_pulse_step_set(ETK_PROGRESS_BAR(UI_LoadPBar), 0.03);
   
   etk_box_append(ETK_BOX(vbox), UI_LoadPBar, ETK_BOX_START, ETK_BOX_NONE, 5);
 
   //Text_view
   UI_LoadTextView = etk_text_view_new ();
   //etk_textblock_object_cursor_visible_set(ETK_TEXT_VIEW(UI_LoadTextView)->textblock, ETK_FALSE); //TODO questa riga non funziona
   scrolled_view = etk_scrolled_view_new();
   etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), ETK_POLICY_HIDE, ETK_POLICY_SHOW);
   etk_bin_child_set( ETK_BIN(scrolled_view), UI_LoadTextView);
   etk_box_append(ETK_BOX(vbox), scrolled_view, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);
 
   //buttons
   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_CLOSE);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 5);
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(etk_main_quit), NULL);
   
   
   etk_widget_show_all(win);
   
   ecore_timer_add(0.1,Decompile,file);
   printf("MAIN\n");
   //etk_progress_bar_pulse(ETK_PROGRESS_BAR(UI_LoadPBar));
   etk_main();
   etk_widget_hide(win);
}

/* main */
int
main(int argc, char **argv)
{
   //Init Globals
   ETK_canvas = NULL;
   Cur.eg = NULL;
   Cur.ep = NULL;
   Cur.eps = NULL;
   Cur.ef = NULL;
   Cur.open_file_name = NULL;
   Cur.source_dir = NULL;
   Cur.main_source_file = NULL;
   
   //Setting Globals
   EdjeFile = PACKAGE_DATA_DIR"/edje_editor.edj";

   //--help
   if (argc > 1)
   {
      if ( (argc > 4) ||
           (0 == strcmp(argv[1],"-h")) ||
           (0 == strcmp(argv[1],"--help")) ||
           (0 == strcmp(argv[1],"--usage")) )
      {
         printf("\nUsage:\n");
         printf(" edje_editor [EDC | EDJ] [IMAGE_DIR] [FONT_DIR]\n");
         printf("\nExample:\n");
         printf(" edje_editor                #Open an empty file\n");
         printf(" edje_editor default.edj    #Open the given EDJ\n");
         printf(" edje_editor default.edc    #Open the given EDC\n");
         printf("\nNote on open an EDC:\n");
         printf(" To open an EDC file you must pass the IMAGE_DIR and FONT_DIR\n");
         printf(" parameters. If not given the EDC directory is assumed.\n\n");
         return 0;
      }
   }

   //Init ETK
   if (!etk_init(&argc, &argv))
   {
      printf("Could not init etk!\n");
      return 1;
   }

   if (!ecore_init())
   {
     printf("ERROR: Cannot init Ecore!\n");
     return -1;
   }


   if (!(hash = ecore_hash_new(ecore_direct_hash,ecore_direct_compare)))
   {
      printf("Could not create hash!\n");
      return 1;
   }

   //Create the etk window with all his widget
   create_main_window();

   //Create the evas objects needed by the canvas (fakewin, handlers)
   prepare_canvas();

   //Open a file
   if (argc > 1)
   {
      char *file = ecore_file_realpath(argv[1]);
      if (!ecore_file_exists(file)){
         printf("File not exists: '%s'\n",argv[1]);
         return 1;
      }
      if (strstr(file, ".edj"))
         //Cur.ef = engrave_load_edj(file);
         LoadEDJ(file);
      if (strstr(file, ".edc"))
      {
         char *edc_dir =  ecore_file_get_dir(file);
         if (argc == 2)
            Cur.ef = engrave_load_edc(file, edc_dir, edc_dir);
         if (argc == 3)
            Cur.ef = engrave_load_edc(file, argv[2], edc_dir);
         if (argc == 4)
            Cur.ef = engrave_load_edc(file, argv[2], argv[3]);
      }
      etk_window_title_set(ETK_WINDOW(UI_MainWin),file);
   }

   if (!Cur.ef)
      Cur.ef = engrave_file_new();


   engrave_canvas_file_set (engrave_canvas, Cur.ef);


   //Populate Interface
   PopulateTree();
   PopulateImagesComboBox();
   PopulateFontsComboBox();


   //DebugInfo(FALSE);

   //Start main loop
   etk_main();

   etk_shutdown();


   return 0;
}
