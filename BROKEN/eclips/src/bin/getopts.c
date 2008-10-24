#include <limits.h>
#include "eclipse.h"
#include "file.h"

extern Eina_List *views;

Eclipse_Options *parseArgs(int argc, char *argv[], Eclipse_Options *eo)
{
   
   int  c;
   int  ok = 1;
   int  modeset = 0;
   char img[PATH_MAX];
   char *t,*t2,*arg,*tok;
   static char *options = "d:m:t:ha:T:g:pM:w:sRx";
   int this_option_optind = optind ? optind : 1;
   int option_index = 0;
   Eclipse_View *view;
   static struct option long_options[] = 
     {   
	  {"pnp"  , 1, 0, 1},  /* pic-in-pic mode    */
	  {"multi", 1, 0, 2},  /* multiview mode     */
	//{"pcn"  , 0, 0, 3},  /* prev-cur-next mode */
	  {0, 0, 0, 0}
     };
   eo->images = NULL;
           
   //      processing loop
   while ((c = getopt_long(argc, argv, options,long_options,&option_index)) != -1) {
      
      switch (c) {	 
       case 1:
	 /* pic-in-pic mode */
	 printf ("option %s", long_options[option_index].name);	 
	 if (optarg)
	   printf (" with arg %s\n", optarg);
	 
	 if(modeset == 1)
	   break;
	 view         = NULL;
	 view         = (Eclipse_View*)calloc(1,sizeof(Eclipse_View));	 
	 view->images = eclips_file_add_dir(view->images,optarg);
	 view->imfirst= view->images;
	 view->mode   = c;
	 view->display= 1;      
	 views        = eina_list_append(views,view);	 
	 modeset      = 1;
	 break;
	 
       case 2:
	 /*  multiview mode */
	 printf ("option %s", long_options[option_index].name);	 
	 if (optarg)
	   printf (" with arg %s\n", optarg);
	 
	 if(modeset == 1)
	   break;
	 arg = strdup(optarg);
	 tok = strtok(arg,",");
	 printf("----> adding %s\n",arg);
	 view = NULL;
	 view = (Eclipse_View*)calloc(1,sizeof(Eclipse_View));
	 view->images = eclips_file_add_dir(view->images,tok);
	 view->imfirst= view->images;	 
	 view->mode   = c;
	 view->display= 1;	 
	 views = eina_list_append(views,view);
	 while(tok = strtok(NULL,","))
	   {	       	
	      printf("----> adding %s\n",arg);
	      view = NULL;
	      view = (Eclipse_View*)calloc(1,sizeof(Eclipse_View));
	      view->images = eclips_file_add_dir(view->images,tok);
	      view->imfirst= view->images;	      
	      view->mode   = c;	   
	      view->display= 1;	      
	      views = eina_list_append(views,view);
	   }	 
	 modeset = 1;
	 break;	 
	 
       case 3:
	 /* pcn mode - not working yet */
	 printf ("option %s", long_options[option_index].name);	 
	 if (optarg)
	   printf (" with arg %s\n", optarg);
	 
	 if(modeset == 1)
	   break;
	 view         = NULL;
	 view         = (Eclipse_View*)calloc(1,sizeof(Eclipse_View));	 
	 view->images = NULL;
	 view->mode   = c;
	 view->display= 1;	 
	 views        = eina_list_append(views,view);	 
	 modeset      = 1;	 
	 
       case 'd':
	 /* image cycle delay OR webcam refresh */
	 eo->delay = atoi(optarg);
	 break;
       case 'm':
	 /* slide show mode or webcam mode */
	 eo->mode = strdup(optarg);
	 if(strcmp(eo->mode,"s") != 0 && strcmp(eo->mode,"w") != 0)
	   usage();
	 break;
	 /* initial transparency to start with */
       case  't':
	 eo->trans = atoi(optarg);
	 break;	 
       case 'h':
	 /* show the help menu */
	 usage();
	 break;
       case 'a':
	 /* threshhold of alphabending when changing images */
	 eo->alpha_increment =  atoi(optarg);	 
	 break;
       case 'T':
	 /* time pause between each alpha change when changing images */
	 eo->transition_timer =  strtod(optarg,NULL);
	 break;	 
       case 'g':
	 /* time pause between each alpha change when changing images */
	 t2 = strdup(optarg);
	 t = strtok(t2,"x");
	 eo->geom_w =  atoi(strdup(t));
	 t = strtok(NULL,"x");
	 eo->geom_h =  atoi(strdup(t));
	 break;
       case 'p':
	 /* we want pseudo transparency onto the desktop */
	 eo->pseudo_trans = 1;
	 break;
	 /* control menu transparency level */
       case 'M':
	 eo->menu_trans = atoi(optarg);
	 break;
       case 'w':
	 /* we want pseudo transparency onto the desktop and windows */
	 eo->pseudo_trans = 2;
	 break;	 
       case 's':
	 /* we want to display images in random order */
	 eo->shadows = 1;
	 break;
       case 'R':
	 /* we want pseudo transparency onto the desktop and windows */
	 eo->random = 1;
	 break;
       case 'x':
	 /* we want pseudo transparency onto the desktop and windows */
	 eo->borderless = 1;
	 break;	 
       default:
	 //printf ("?? getopt returned character code 0%o ??\n", c);	 
	 ok = 0;
      }
   }
   
   /* add list of images for main window */
   if (optind < argc) 
     {	
	Eclipse_View *view = (Eclipse_View*)calloc(1,sizeof(Eclipse_View));
	while (optind < argc)
	  {
	     //img = (char*)malloc(sizeof(argv[optind]));
	     sprintf(img,"%s",argv[optind++]);
	     view->images = eina_list_append(view->images,(void*)strdup(img));
	  }
	/* add view to list of views */
	view->imfirst= view->images;
	view->mode = 0;
	view->display= 1;	
	views = eina_list_append(views,view);
     }   
            
   if (!ok || optind < argc) {
      usage();
   }    
   
   return eo;
}
