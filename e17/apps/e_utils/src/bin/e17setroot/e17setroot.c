#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <Imlib2.h>
#include <Engrave.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_File.h>

#include "config.h"

#define IF_FREE(x) { if (x) free(x); x = NULL; }

/* TODO:
 * make Esetroot respect our options
 * add empty -s -t -c flags that will take current bg and apply option
 */

static int e_bg_type = 0;
static int e_bg_no_load = 0;
static int e_bg_no_fake = 0;
static char *e_bg_img_file = NULL;
static char *esetroot_opt = NULL;

enum E_Bg_Types
{
     E_BG_TILE = 1,
     E_BG_SCALE = 2,
     E_BG_CENTER = 3,
     E_BG_FIT = 4,
     E_BG_GET = 5,
     E_BG_NO_LOAD = 6,
     E_BG_NO_FAKE = 7,
};
typedef enum E_Bg_Types E_Bg_Types;

void _e_bg_bg_help() {
   printf("e17setroot - Manipulate Enlightenment DR17's background\n");
   printf("Usage: e17setroot <imagename> | <edj>\n");
   printf(" -t | --tile  <imagename>   Tile the suppied image.\n");
   printf(" -c | --center <imagename>  Center the supplied image.\n");
   printf(" -s | --scale <imagename>   Scale the supplied image to the screen.\n");
   printf(" -n | --noload <imagename>  Create .edj without setting it.\n");
   printf(" -g | --get                 Get current E17 background.\n");
   printf(" -k | --nofake              Do not use Esetroot for fake transparency. (Not Supported)\n");
   printf(" -h                         Show this help screen.\n");
}

void _e_bg_bg_get() {
   char buf[4096];
   
   snprintf(buf, sizeof(buf), "enlightenment_remote -default-bg-get");
   system(buf);
}

/* parse command line options */
void _e_bg_bg_parseargs(int argc, char **argv) {
   int c;

   static char *options = "tscfghnk";
   static struct option long_options[] = {
	{"tile",   0, 0, E_BG_TILE},
	{"scale",  0, 0, E_BG_SCALE},
	{"center", 0, 0, E_BG_CENTER},
	{"fit",    0, 0, E_BG_FIT},
	{"get",    0, 0, E_BG_GET},
	{"noload", 0, 0, E_BG_NO_LOAD},
	{"nofake", 0, 0, E_BG_NO_FAKE},      
	{0,        0, 0, 0}
   };

   while((c = getopt_long (argc, argv, options, long_options, NULL)) != -1) {
      switch (c) {
	 /* tile */
       case E_BG_TILE:
       case 't':
	 IF_FREE(esetroot_opt);
	 e_bg_type = E_BG_TILE;
	 esetroot_opt = strdup(" ");
	 break;

	 /* scale */
       case E_BG_SCALE:
       case 's':
	 IF_FREE(esetroot_opt);
	 e_bg_type = E_BG_SCALE;
	 esetroot_opt = strdup(" -s ");
	 break;

	 /* center */
       case E_BG_CENTER:
       case 'c':
	 IF_FREE(esetroot_opt);
	 e_bg_type = E_BG_CENTER;
	 esetroot_opt = strdup(" -c ");
	 break;

	 /* fit */
       case E_BG_FIT:
       case 'f':
	 IF_FREE(esetroot_opt);
	 esetroot_opt = strdup(" -f ");
	 break;

	 /* get current bg */
       case E_BG_GET:
       case 'g':
	 IF_FREE(esetroot_opt);
         e_bg_type = E_BG_GET;
	 break;

       case E_BG_NO_LOAD:
       case 'n':
	 e_bg_no_load = 1;
	 break;
       case E_BG_NO_FAKE:
       case 'k':
	 e_bg_no_fake = 1;
	 break;
	 /* show help screen */
       case 'h':
       default:
	 _e_bg_bg_help();
	 break;
      }
   }

   if (optind < argc)
     e_bg_img_file = argv[optind];
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

int _e_bg_bg_set(char *filename) {
   char buf[4096];
   
   snprintf(buf, sizeof(buf), "enlightenment_remote -default-bg-set %s", filename);
   system(buf);
   return 1;
}

void _e_bg_bg_edj_gen(char *filename) {
   int w, h;
   char *file, *dir, *edj_file, *filenoext, *esetroot;
   Imlib_Image *im;
   Engrave_File *edj;
   Engrave_Image *image;
   Engrave_Group *grp;
   Engrave_Part *part;
   Engrave_Part_State *ps;

   /* make sure we got a file name */
   if (!filename || strlen(filename) <= 4) return;

   file = (char *)ecore_file_file_get(filename);
   dir = ecore_file_dir_get(filename);

   filenoext = _e_bg_bg_file_stripext(filename);
   filenoext = (char *)ecore_file_file_get(filenoext);

   if (strcmp(filename + strlen(filename) - 4, ".edj") == 0) {
      int w, h, num;
      char static_bg[PATH_MAX];
      char esetroot_s[PATH_MAX];
      char filename_s[PATH_MAX];
      Ecore_X_Window *roots = NULL;

      if (!ecore_x_init(NULL))
	     return;
      num = 0;
      roots = ecore_x_window_root_list(&num);
      ecore_x_window_size_get(roots[0], &w, &h);
      snprintf(filename_s, PATH_MAX, "/tmp/%s.png", filenoext);
      snprintf(static_bg, PATH_MAX, "edje_thumb %s e/desktop/background %s -g %dx%d -og %dx%d", filename, filename_s, w, h, w, h);
      system(static_bg);
      _e_bg_bg_set(filename);

      if (!e_bg_no_fake) 
	{
	   snprintf(esetroot_s, PATH_MAX, "Esetroot %s %s ", esetroot_opt, filename_s);
	   system(esetroot_s);
	}
      return;
   }

   /* Set up edj path */
   edj_file = malloc(strlen(getenv("HOME")) +  strlen("/.e/e/backgrounds/")
		+ strlen(filenoext) + strlen(".edj") + 1);
   strcpy(edj_file, getenv("HOME"));
   strcat(edj_file, "/.e/e/backgrounds/");
   strcat(edj_file, filenoext);
   strcat(edj_file, ".edj");

   /* Determine image width / height */
   im = imlib_load_image(filename);
   imlib_context_set_image(im);
   w = imlib_image_get_width();
   h = imlib_image_get_height();
   imlib_free_image_and_decache();

   /* create the .edj */
   edj = engrave_file_new();
   engrave_file_image_dir_set(edj, dir);
   image = engrave_image_new(file, ENGRAVE_IMAGE_TYPE_COMP, 0);
   engrave_file_image_add(edj, image);

   grp = engrave_group_new();
   engrave_group_name_set(grp, "e/desktop/background");
   engrave_file_group_add(edj, grp);

   part = engrave_part_new(ENGRAVE_PART_TYPE_IMAGE);
   engrave_part_name_set(part, "background_image");
   engrave_group_part_add(grp, part);

   ps = engrave_part_state_new();
   engrave_part_state_name_set(ps, "default", 0.0);
   engrave_part_state_image_normal_set(ps, image);

   switch(e_bg_type) {
    case E_BG_CENTER:
      engrave_part_state_max_size_set(ps, w, h);
      break;

    case E_BG_SCALE:
      break;

    case E_BG_FIT:
      break;

    case E_BG_TILE:
      /* FIXME: This is a temp until dj2 fixes engrave */
      //engrave_part_state_max_size_set(ps, w, h);
      engrave_part_state_fill_size_relative_set(ps, 0.0, 0.0);
      engrave_part_state_fill_size_offset_set(ps, w, h);
      break;

    default:
      /* FIXME: This is a temp until dj2 fixes engrave */
      //engrave_part_state_max_size_set(ps, w, h);
      engrave_part_state_fill_size_relative_set(ps, 0.0, 0.0);
      engrave_part_state_fill_size_offset_set(ps, w, h);
      break;
   }

   engrave_part_state_add(part, ps);

   engrave_edj_output(edj, edj_file);
   engrave_file_free(edj);

   /* don't do anything more if we're not loading the image */
   if (e_bg_no_load)
      return;

   /* set the background */
   if (!_e_bg_bg_set(edj_file))
      return;

   /* If we're using pseudo-trans for eterm, then this will help */
   if (!e_bg_no_fake) 
     {
	esetroot = malloc(strlen("Esetroot ") + strlen(esetroot_opt) + strlen(filename) + 1);
	strcpy(esetroot, "Esetroot ");
	strcat(esetroot, esetroot_opt);
	strcat(esetroot, filename);
	system(esetroot);
	free(esetroot);
     }
}

int main(int argc, char **argv)
{
   if (argc == 1) 
     {
	_e_bg_bg_help();
	return 0;
     }
   
   _e_bg_bg_parseargs(argc, argv);

   if (e_bg_type == E_BG_GET) {
      _e_bg_bg_get();
   } else
     _e_bg_bg_edj_gen(e_bg_img_file);

   return 0;
}
