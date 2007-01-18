#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include <Etk.h>
#include <Engrave.h>

#include "evas.h"
#include "interface.h"
#include "main.h"

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
void
DebugInfo(int full)
{
   Engrave_Group *gro;
   Engrave_Part *par;
   Engrave_Part_State *sta;
   Evas_List *gp,*pp,*sp;

   printf("\n\n ********************* D E B U G ***************************\n");
   printf(" ** edje_editor.edj: %s\n",EdjeFile);
  /* printf("Current engrave file: %s\n",Cur.ef->);
   printf("Current EDCFileDir: %s\n",EDCFileDir->str);
   */
   printf(" ** Tot groups in EDC: %d\n",engrave_file_groups_count (Cur.ef));
   printf(" ** Tot images in EDC: %d\n",engrave_file_images_count (Cur.ef));
   printf(" ** Tot fonts in EDC: %d\n",engrave_file_fonts_count (Cur.ef));
   printf(" ** Tot spectra in EDC: %d\n",engrave_file_spectra_count (Cur.ef));
   printf(" ** Tot styles in EDC: %d\n",engrave_file_styles_count (Cur.ef));
   printf(" ** Tot data in EDC: %d\n",engrave_file_data_count (Cur.ef));
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
         printf("Usage:\n");
         printf("edje_editor [EDC | EDJ] [IMAGE_DIR] [FONT_DIR]\n");
         printf("Example:\n");
         printf(" edje_editor                #Open an empty file\n");
         printf(" edje_editor default.edj    #Open the given EDJ\n");
         printf(" edje_editor default.edc    #Open the given EDC\n");
         printf("Note on open an EDC:\n");
         printf(" To open an EDC file you must pass the IMAGE_DIR and FONT_DIR\n");
         printf(" parameters. If not given the EDC directory is assumed.\n");
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
         Cur.ef = engrave_load_edj(file);
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
   }

   if (!Cur.ef)
      Cur.ef = engrave_file_new();


   engrave_canvas_file_set (ecanvas, Cur.ef);


   //Populate Interface
   PopulateTree();
   PopulateImagesComboBox();
   PopulateFontsComboBox();


   DebugInfo(FALSE);

   //Start main loop
   etk_main();

   etk_shutdown();


   return 0;
}
