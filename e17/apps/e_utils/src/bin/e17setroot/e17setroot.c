#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <E.h>

#include "config.h"

enum E_Bg_Types
{
     E_BG_TILE = 1,
     E_BG_SCALE = 2,
     E_BG_CENTER = 3,
     E_BG_FIT = 4,
     E_BG_GET = 5,
};
typedef enum E_Bg_Types E_Bg_Types;


void _e_bg_bg_help() { 
   printf("Usage: e17setroot <imagename> | <eet>\n");
}

/* parse command line options */
void _e_bg_bg_parseargs(int argc, char **argv) {
   int c;
   int digit_optind = 0;

   int this_option_optind = optind ? optind : 1;
   int option_index = 0;
   static char *options = "t:s:c:f:g";
   static struct option long_options[] = {
	{"tile",   1, 0, E_BG_TILE},
	{"scale",  1, 0, E_BG_SCALE},
	{"center", 1, 0, E_BG_CENTER},
	{"fit",    1, 0, E_BG_FIT},
	{"get",    0, 0, E_BG_GET},
	{0,        0, 0, 0}
   };

   while((c = getopt_long (argc, argv, options, long_options, &option_index))
	 != -1) {
      switch (c) {
	 
	 /* tile */
       case E_BG_TILE:
       case 't':	    
	 //long_options[option_index].name
	 //optarg
	 break;

	 /* scale */
       case E_BG_SCALE:
       case 's':

	 break;
	 
	 /* center */
       case E_BG_CENTER:
       case 'c':
	 break;
	 
	 /* fit */
       case E_BG_FIT:
       case 'f':
	 break;

	 /* get current bg */
       case E_BG_GET:
       case 'g':
	 break;
	 
	 /* show help screen */
       default:
	 break;
      }
   }

   if (optind < argc) {
      printf ("non-option ARGV-elements: ");
      while (optind < argc)
	printf ("%s ", argv[optind++]);
      printf ("\n");
   }
}

/* return dir from a full path filename */
char *_e_bg_bg_file_getdir(char *path) {
   char *ptr;
   char *c;
   char *dir;
   int i = 0;

   ptr=path;
   c=strrchr(ptr, '/');
   dir = malloc(strlen(path) + 1);

   while(ptr != c) {
      dir[i] = *ptr;
      ptr++;
      i++;
   }

   dir[i] = '\0';
   return dir;
}

/* return filename from a full path filename */
char *_e_bg_bg_file_getfile(char *path) {
   char *ptr;
   char *c;
   char *file;
   int i;

   i = 0;
   ptr = path;
   c = strrchr(ptr, '/');
   file = malloc(strlen(ptr) + 1);

   while(ptr != c) {
      file[i] = *ptr;
      ptr++;
      i++;
   }

   file[i] = '\0';
   return c;
}

/* strip extention from a file */
char *_e_bg_bg_file_stripext(char *path) {
   char *ptr;
   char *c;
   char *file;
   int i;

   i = 0;
   ptr = path;
   c = strrchr(ptr,'.');
   file = malloc(strlen(path));

   while(ptr != c) {
      file[i] = *ptr;
      ptr++;
      i++;
   }

   file[i] = '\0';
   return file;
}

void _e_bg_bg_eet_gen(char *filename) {
   int i, w, h;
   char *cmd, *file, *dir, *sed, *cp, *eet, *edc, *edj, *edje, *filenoext, 
     *esetroot;
   Imlib_Image *im;

   if (strcmp(filename + strlen(filename) - 4, ".eet") == 0)
     {
	e_background_set(filename);
	return;
     }
		   
   i = 0;

   file = _e_bg_bg_file_getfile(filename);
   dir = _e_bg_bg_file_getdir(filename);
   edc = strdup(PACKAGE_DATA_DIR "/data/e17setroot/e17setroot_template.edc ");
   edj = strdup("edje_cc -id ");

   filenoext = _e_bg_bg_file_stripext(filename);
   filenoext = _e_bg_bg_file_getfile(filenoext);

   /* Copy edc to /tmp so we can fiddle with it there */
   cp = malloc(strlen("cp ") + strlen(edc) + strlen(" /tmp/") + 3);
   strcpy(cp, "cp ");
   strcat(cp, edc);
   strcat(cp, " /tmp/");
   system(cp);
   free(cp);
   
   /* change edc to the one stored in /tmp */
   free(edc);
   edc = strdup("/tmp/e17setroot_template.edc ");
   
   /* Set up eet path */
   eet = malloc(strlen(getenv("HOME")) +  strlen("/.e/e/backgrounds/") 
		+ strlen(filenoext) + strlen(".eet") + 1);
   strcpy(eet, getenv("HOME"));
   strcat(eet, "/.e/e/backgrounds/");
   strcat(eet, filenoext);
   strcat(eet, ".eet");
   
   /* Set up edje_cc command sans eet path */
   edje = malloc(strlen(edc) + strlen(edj) + strlen(dir) + 2);
   strcpy(edje, edj);
   strcat(edje, dir);
   strcat(edje, " ");
   strcat(edje, edc);

   /* Set up edje_cc command + eet path */
   cmd = malloc(strlen(edje) + strlen(eet) + 1);
   strcpy(cmd, edje);
   strcat(cmd, eet);

   /* Determine image width / height */
   im = imlib_load_image(filename);
   imlib_context_set_image(im);
   w = imlib_image_get_width();
   h = imlib_image_get_height();
   
   sed = malloc(100 + strlen(filename) + strlen(edc));
   sprintf(sed, "sed -i -e 's,FILENAME,%s,' %s",filename,edc);
   system(sed);
   free(sed);

   sed = malloc(100 + 1 + strlen(edc));
   sprintf(sed, "sed -i -e 's/WIDTH/%d/' %s",w,edc);
   system(sed);
   free(sed);

   sed = malloc(100 + 1 + strlen(edc));
   sprintf(sed, "sed -i -e 's/HEIGHT/%d/' %s",h,edc);
   system(sed);
   free(sed);

   imlib_free_image_and_decache();

   system(cmd);
   free(cmd);

   e_background_set(eet);

   sed = malloc(100 + strlen(filename) + strlen(edc));
   sprintf(sed, "sed -i -e 's,%s,FILENAME,' %s",filename,edc);
   system(sed);
   free(sed);

   sed = malloc(100  + 1 + strlen(edc));
   sprintf(sed, "sed -i -e 's/%d/WIDTH/' %s",w,edc);
   system(sed);
   free(sed);

   sed = malloc(100 + 1 + strlen(edc));
   sprintf(sed, "sed -i -e 's/%d/HEIGHT/' %s",h,edc);
   system(sed);
   free(sed);
   
   /* If we're using pseudo-trans for eterm, then this will help */
   esetroot = malloc(strlen("Esetroot ") + strlen(filename) + 1);
   strcpy(esetroot, "Esetroot ");
   strcat(esetroot, filename);
   system(esetroot);
   free(esetroot);
}

int main(int argc, char **argv)
{
   if (!e_init(":0.0")) {
      printf("Can't connect to enlightenment, perhaps we are not on :0.0!\n");
      exit(-1);
   }

   //_e_bg_bg_parseargs(argc, argv);
   _e_bg_bg_eet_gen(argv[1]);

   e_shutdown();

   return 0;
}
