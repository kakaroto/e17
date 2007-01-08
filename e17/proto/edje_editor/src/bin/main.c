#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include <Etk.h>
#include <Engrave.h>

#include "parse.h"
#include "evas.h"
#include "interface.h"
#include "main.h"


/* EAPI */void 
RenamePart(Engrave_Part* ep, const char* name) //engrave_part_name_set
{
/*    Engrave_Group *eg = engrave_part_parent_get(ep);
   Evas_List *l,*ll;
   
   printf("Rename Part: %s -> %s\n",ep->name, name);

   if (eg)
   {
      //Update all the named links to the part (rel1_x_to, ecc)
      for (l = eg->parts; l; l = l->next)
      {
         Engrave_Part *all_ep = l->data;
         for (ll = all_ep->states; ll; ll = ll->next)
         {
            Engrave_Part_State *eps = ll->data;
      
            if (engrave_part_state_rel1_to_x_get(eps))
               if (0 == strcmp(eps->rel1.to_x,ep->name))  
                  engrave_part_state_rel1_to_x_set(eps,name);
            if (engrave_part_state_rel1_to_y_get(eps))
               if (0 == strcmp(eps->rel1.to_y,ep->name))  
                  engrave_part_state_rel1_to_y_set(eps,name);
            if (engrave_part_state_rel2_to_x_get(eps))
               if (0 == strcmp(eps->rel2.to_x,ep->name))  
                  engrave_part_state_rel2_to_x_set(eps,name);
            if (engrave_part_state_rel2_to_y_get(eps))
               if (0 == strcmp(eps->rel2.to_y,ep->name))  
                  engrave_part_state_rel2_to_y_set(eps,name);
         }
      }
   }
   //Change the name of the part
   engrave_part_name_set(ep,name);
   //IF_FREE(ep->name);
   //ep->name = (name ? strdup(name) : NULL); */
}

void FileCopy(char* source, char*dest){
/*    int c;
   GString	*str=g_string_new("");
   FILE *IPFile,*OPFile;

   //printf("COPY FROM: %s TO: %s/%s\n",source,dest,g_path_get_basename (source));
   g_string_printf(str,"%s/%s",dest,g_path_get_basename (source));

   // Open the file - no error checking done TODO error checking
   IPFile = fopen(source,"r");
   OPFile = fopen(str->str,"w");

   // Read one character at a time, checking for the End of File.
   while ((c = fgetc(IPFile)) != EOF){
      fputc(c, OPFile);
   }

   //close files
   fclose(IPFile);
   fclose(OPFile);
   g_string_free(str,TRUE); */
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
  /* printf("Current EDCFile: %s\n",EDCFile->str);
   printf("Current EDCFileDir: %s\n",EDCFileDir->str);
   */
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
  // FileName = NULL;
   Cur.eg = NULL;
   Cur.ep = NULL;
   Cur.eps = NULL;
   Cur.ef = NULL;
   char buf[4096];

   
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
   
   
   
   
   //FileName = etk_string_new("/home/dave/test/decc/default/default.edc");

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

