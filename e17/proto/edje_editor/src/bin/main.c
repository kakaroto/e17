#include "config.h"
#include <string.h>
#include <errno.h>
#include <Edje.h>
#if TEST_DIRECT_EDJE
   #include <Edje_Edit.h>
#endif
#include <Etk.h>
#include <Engrave.h>
#include <Ecore_Str.h>

#include "evas.h"
#include "interface.h"
#include "inout.h"
#include "main.h"

Evas_Object *EdjeTest_bg;
Evas_Object *EdjeTest_edje;

void
on_test_win_resize(Ecore_Evas * ee)
{
	int w, h;
	
	evas_output_size_get(ecore_evas_get(ee), &w, &h);
	printf("RESIZE: %d - %d\n", w,h);
 
	//evas_object_move(preview, barwidth, 0);
	evas_object_resize(EdjeTest_bg, w , h );
	evas_object_resize(EdjeTest_edje, w , h );
}
/**
 * engrave_part_state_image_tween_remove_nth - Remove the nth image from the tween list.
 * @param eps: The Engrave_Part_State to remove the image to.
 * @param tween_num: The number of the image to remove from the tween list.
 *
 * @return Returns no value 
 */
/*EAPI*/ void
PROTO_engrave_part_state_image_tween_remove_nth(Engrave_Part_State *eps,
                                       int tween_num)
{
   Evas_List *l;

   if ((!eps) || (tween_num < 0)) return;
   
   l = 	evas_list_nth_list (eps->image.tween, tween_num);
   //printf("Remove tween num: %d (%s)\n",tween_num,((Engrave_Image*)(l->data))->name);
   if (!l) return;

   eps->image.tween = evas_list_remove_list (eps->image.tween, l);
}
/**
 * engrave_part_state_image_tween_remove_all - Clear the tween list for a state.
 * @param eps: The Engrave_Part_State that contain the list to clear.
 *
 * @return Returns no value 
 */
/*EAPI*/ void
PROTO_engrave_part_state_image_tween_remove_all(Engrave_Part_State *eps)
{
   if (!eps && !eps->image.tween) return;
   eps->image.tween = evas_list_free(eps->image.tween);
}
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
 * engrave_group_program_remove - remove the given program from the group 
 * @param eg: The Engrave_Group to remove the program too.
 * @param ep: The Engrave_Program to remove.
 * 
 * @return Returns no value.
 */
/*EAPI*/ void 
PROTO_engrave_group_program_remove(Engrave_Group *eg, Engrave_Program *epr) {
  Evas_List * list;

  if (!eg || !epr) return;

  eg->programs = evas_list_remove(eg->programs, epr);
  engrave_program_parent_set(epr, NULL);

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
void
PROTO_engrave_part_raise(Engrave_Part *ep)
{
   Engrave_Group *eg;
   Engrave_Part *next;
   Evas_List *l;
    
   if (!ep) return;
   
   eg = ep->parent;
   if (!eg) return;
   
   printf("RAISE part: '%s' in group: '%s'\n",ep->name, eg->name);
   
   l = evas_list_find_list(eg->parts, ep);
   if (!l || !l->next) return;
   
   next = l->next->data;
   eg->parts = evas_list_remove (eg->parts, ep);
   eg->parts = evas_list_append_relative (eg->parts, ep, next);
}
int
PROTO_engrave_part_lower(Engrave_Part *ep)
{
   Engrave_Group *eg;
   Engrave_Part *prev;
   Evas_List *l;
    
   if (!ep) return FALSE;
   
   eg = ep->parent;
   if (!eg) return FALSE;
   
   printf("LOWER part: '%s' in group: '%s'\n",ep->name, eg->name);
   
   l = evas_list_find_list(eg->parts, ep);
   if (!l || !l->prev) return FALSE;
   
   prev = l->prev->data;
   eg->parts = evas_list_remove (eg->parts, ep);
   eg->parts = evas_list_prepend_relative (eg->parts, ep, prev);
    
   //evas_object_lower(ep->object);
   return TRUE;
}
#if TEST_DIRECT_EDJE
Evas_Bool
_DebugInfo_helper(Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   printf("Key: '%s' [%d]\n", key, data);
}
void
DebugInfo(int full)
{
   Engrave_Group *gro;
   Engrave_Part *par;
   Engrave_Part_State *sta;
   Evas_List *gp,*pp,*sp;

   printf("\n\n ********************* D E B U G ***************************\n");
   printf(" ** open file name: %s\n",Cur.edj_file_name->string);
   printf(" ** temp file name: %s\n",Cur.edj_temp_name->string);
   printf(" ** edje_editor.edj: %s\n",EdjeFile);
   if (etk_string_length_get(Cur.group))
      printf(" ** Cur group: %s\n",Cur.group->string);
   else
      printf(" ** Cur group: (NULL)\n");
   if (etk_string_length_get(Cur.part))
      printf(" ** Cur part: %s\n",Cur.part->string);
   else
      printf(" ** Cur part: (NULL)\n");
   if (etk_string_length_get(Cur.state))
      printf(" ** Cur state: %s\n",Cur.state->string);
   else
      printf(" ** Cur state: (NULL)\n");
   if (etk_string_length_get(Cur.prog))
      printf(" ** Cur program: %s\n",Cur.prog->string);
   else
      printf(" ** Cur program: (NULL)\n");
   printf(" *********************** Parts_Hash *****************************\n");
   evas_hash_foreach(Parts_Hash, _DebugInfo_helper, NULL);
   printf(" *********************** E N D *****************************\n\n");
}
#else
void
DebugInfo(int full)
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
   if (Cur.epr)
      printf(" ** Cur program: %s\n",Cur.epr->name);
   else
      printf(" ** Cur program: (NULL)\n");

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
#endif
void
TestEdjeGroup(char *File,char *Group)
{
   Ecore_Evas  *ee;
   Evas        *evas;
   
   printf("Test EdjeFile: %s\nGroup: %s\nTheme: %s\n",File,Group,EdjeFile);
   
   /* ecore evas init */
	ecore_init();      
	ecore_evas_init();
	ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, 0, 0);
	ecore_evas_title_set(ee, "Edje Test Application");
   ecore_evas_callback_resize_set(ee, on_test_win_resize);
	edje_init();
   evas = ecore_evas_get(ee);
   
   /* Background */
   EdjeTest_bg = evas_object_image_add(evas);
   evas_object_image_file_set(EdjeTest_bg, EdjeFile, "images/1");		//TODO Find a method to load by name and not by number
   evas_object_image_fill_set(EdjeTest_bg,0,0,240,240);
   evas_object_move(EdjeTest_bg, 0, 0);
   evas_object_resize(EdjeTest_bg, 300, 300);
   evas_object_show(EdjeTest_bg);

   /* Edje Group */
   EdjeTest_edje = edje_object_add(evas);
   edje_object_file_set(EdjeTest_edje,File, Group);
   evas_object_move(EdjeTest_edje, 0, 0);
   evas_object_resize(EdjeTest_edje, 300, 300);
   evas_object_show(EdjeTest_edje);
   
   /* Window Size */
   int minw,minh,maxw,maxh;
   edje_object_size_min_get(EdjeTest_edje, &minw, &minh);
   edje_object_size_max_get(EdjeTest_edje, &maxw, &maxh);
   if (minw <= 0) minw = 30;
   if (minh <= 0) minh = 30;
   ecore_evas_size_min_set(ee,minw,minh);
   ecore_evas_size_max_set(ee,maxw,maxh);
   ecore_evas_resize(ee, minw, minh);
   
   ecore_evas_show(ee);
   
   ecore_main_loop_begin();
}
void
PrintUsage(void)
{
   printf("\nUsage:\n");
   printf(" edje_editor [EDC | EDJ] [IMAGE_DIR] [FONT_DIR]\n");
   printf("\nExample:\n");
   printf(" edje_editor                #Open an empty file\n");
   printf(" edje_editor default.edj    #Open the given EDJ\n");
   printf(" edje_editor default.edc    #Open the given EDC\n");
   printf("\nTesting Edje file:\n");
   printf(" edje_editor -t file.edj \"group_name\"\n");
   printf("\nNote on open an EDC:\n");
   printf(" To open an EDC file you must pass the IMAGE_DIR and FONT_DIR\n");
   printf(" parameters. If not given the EDC directory is assumed.\n\n");
}
#if TEST_DIRECT_EDJE
void
ChangeGroup(char *group)
{
   if (!group) return;

   if (Cur.group->string && !strcmp(group, Cur.group->string)) return;
   printf("Change group: %s\n", group);
   edje_edit_save(edje_o);
   evas_object_hide(edje_o);
   edje_object_file_set(edje_o, Cur.edj_temp_name->string, group);
   evas_object_show(edje_o);
   Parts_Hash = NULL;         //TODO FREE
   
   Cur.group = etk_string_set(Cur.group, group);
   Cur.part = etk_string_clear(Cur.part);
   Cur.state = etk_string_clear(Cur.state);
   
   
   PopulateTree2();
   PopulateRelComboBoxes();
   
   UpdateGroupFrame();
   
   //update FakeWin title
   edje_object_part_text_set(EV_fakewin, "title", group);
}

int
LoadEDJ2(char *file)
{
   char *realp = NULL;
   char *old_temp;
   if (!file) return 0;
   
   printf("** Load EDJ: '%s'\n",file);
    
   realp = ecore_file_realpath(file);
   if (!ecore_file_exists(realp))
   {
      ShowAlert("<b>ERROR:</b><br>File not exists.");
      return 0;
   }
   if (!ecore_file_can_read(realp))
   {
      ShowAlert("<b>ERROR</b>:<br>Can't read file.");
      return 0;
   }
   if (!ecore_str_has_suffix(realp, ".edj"))  //TODO: better check
   {
      ShowAlert("<b>ERROR</b>:<br>File is not an edje file.");
      return 0;
   }
   
   
   //Create temp file
   if (etk_string_length_get(Cur.edj_temp_name) > 0)
      old_temp = strdup(Cur.edj_temp_name->string);
   else
      old_temp = NULL;
   
   char tmpn[1024];
   int fd = 0;
   strcpy(tmpn, "/tmp/edje_editor_tmp.edj-XXXXXX");
   fd = mkstemp(tmpn);
   if (fd < 0)
   {
      printf("Can't create temp file '%s'\nError: %s\n", tmpn, strerror(errno));
      return 0;
   }
   Cur.edj_temp_name = etk_string_set(Cur.edj_temp_name, tmpn);

    
   if (!ecore_file_cp(realp, Cur.edj_temp_name->string))
   {
      ShowAlert("<b>ERROR</b>:<br>Can't copy to temp file.");
      return 0;
   }
        
   
   Cur.edj_file_name = etk_string_set(Cur.edj_file_name, realp);
   
   ecore_evas_title_set(UI_ecore_MainWin, Cur.edj_file_name->string);
   
   Cur.group  = etk_string_clear(Cur.group);
   Cur.part   = etk_string_clear(Cur.part);
   Cur.state  = etk_string_clear(Cur.state);
   Cur.prog   = etk_string_clear(Cur.prog);
   Parts_Hash = NULL;
    
   PopulateGroupsComboBox();
   PopulateFontsComboBox();
   PopulateImagesComboBox();
   
    
    
   //Delete old temp file
   if (old_temp && !ecore_file_unlink(old_temp))
   {
      ShowAlert("<b>ERROR</b>:<br>Can't remove temp file.");
      return 0;
   }
   if (old_temp) free(old_temp);
   return 1;
}
#endif


/* main */
int
main(int argc, char **argv)
{
   //Init Globals
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
         PrintUsage();
         return 1;
      }
   }

   //Test EDJ
   if (argc > 1 && (0 == strcmp(argv[1],"-t")))
   {
      if (argc < 4)
      {
         PrintUsage();
         return 1;
      }
      TestEdjeGroup(argv[2],argv[3]);
      return 0;
   }
   //Init ETK
   if (!etk_init(argc, argv))
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

   //Create the etk window and all the widgets
   create_main_window();

#if TEST_DIRECT_EDJE
   printf("*********************************\n");
   printf("Testing edje direct access!!...\n");
   printf("*********************************\n");

   double val = 1.2;
   printf("TEST:\n");
   
   setlocale(LC_NUMERIC,"C");
   sscanf("3.2","%lf", &val);
   printf("%f\n", val);
   
   Cur.edj_file_name = etk_string_new("");
   Cur.edj_temp_name = etk_string_new("");
   Cur.group = etk_string_new("");
   Cur.part = etk_string_new("");
   Cur.state = etk_string_new("");
   Cur.prog = etk_string_new("");
   
    
   //~ //Create temp file
   //~ char tmpn[1024];
   //~ int fd = 0;
   //~ strcpy(tmpn, "/tmp/edje_editor_tmp.edj-XXXXXX");
   //~ fd = mkstemp(tmpn);
   //~ if (fd < 0)
   //~ {
      //~ printf("Can't create temp file '%s'\nError: %s\n", tmpn, strerror(errno));
      //~ return 1;
   //~ }
   //~ Cur.edj_temp_name = etk_string_set(Cur.edj_temp_name, tmpn);
    
   //Open a file from command line
   if (argc == 2)
   {
      printf("Opening edje file: '%s'\n",argv[1]);
      LoadEDJ2(argv[1]);
   }
   
#else
   //Open a file
   if (argc > 1)
   {
      char *file = ecore_file_realpath(argv[1]);
      if (!ecore_file_exists(file)){
         printf("File not exists: '%s'\n",argv[1]);
         return 1;
      }
      if (ecore_str_has_suffix(file, ".edj"))
         LoadEDJ(file);
      else if (ecore_str_has_suffix(file, ".edc"))
      {
         char *edc_dir =  ecore_file_dir_get(file);
         if (argc == 2)
            Cur.ef = engrave_load_edc(file, edc_dir, edc_dir);
         if (argc == 3)
            Cur.ef = engrave_load_edc(file, argv[2], edc_dir);
         if (argc == 4)
            Cur.ef = engrave_load_edc(file, argv[2], argv[3]);
      }
      ecore_evas_title_set(UI_ecore_MainWin, file);
   }

   if (!Cur.ef)
      Cur.ef = engrave_file_new();

   engrave_canvas_file_set (engrave_canvas, Cur.ef);

   //Populate Interface
   PopulateTree();
   PopulateImagesComboBox();
   PopulateFontsComboBox();

#endif
   //DebugInfo(FALSE);

   //Start main loop
   etk_main();
   
#if TEST_DIRECT_EDJE
   //Remove temp file
   ecore_file_unlink(Cur.edj_temp_name->string); //TODO move this line down (when don't use etk_string anymore)
#endif

   etk_shutdown();
   
   return 0;
}
