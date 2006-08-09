#include <ctype.h>
#include "global.h"
#include "config.h"
#include "eaps.h"
#include "parse.h"

#define DEBUG 1

extern int not_found_count;

static void _write_eap(Eet_File *ef, char *section, char *value);

#define APPLICATIONICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_applications.png"
#define COREICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_applications.png"
#define EDITORICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_editors.png"
#define EDUTAINMENTICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_edutainment.png"
#define GAMESICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_games.png"
#define GRAPHICSICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_graphics.png"
#define INTERNETICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_network.png"
#define MULTIMEDIAICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_multimedia.png"
#define OFFICEICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_wordprocessing.png"
#define PROGRAMMINGICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_development.png"
#define SETTINGSICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_settings.png"
#define SYSTEMICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_system.png"
#define TOYSICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_toys.png"
#define UTILITYICON PACKAGE_DATA_DIR"/data/e17genmenu/icons/package_utilities.png"
static char *set_icon(char *token);


static char *
set_icon(char *token)
{
#ifdef DEBUG
   fprintf(stderr, "Setting Icon: %s\n", token);
#endif
   if (strstr(token, "Core") != NULL)
      return COREICON;
   if (strstr(token, "Development") != NULL)
      return PROGRAMMINGICON;
   if (strstr(token, "Editors") != NULL)
      return EDITORICON;
   if (strstr(token, "Edutainment") != NULL)
      return EDUTAINMENTICON;
   if (strstr(token, "Game") != NULL)
      return GAMESICON;
   if (strstr(token, "Graphics") != NULL)
      return GRAPHICSICON;
   if (strstr(token, "Internet") != NULL)
      return INTERNETICON;
   if (strstr(token, "Office") != NULL)
      return OFFICEICON;
   if (strstr(token, "Programming") != NULL)
      return PROGRAMMINGICON;
   if (strstr(token, "Toys") != NULL)
      return TOYSICON;
   if (strstr(token, "Utilities") != NULL)
      return UTILITYICON;
   if ((strstr(token, "Accessories") != NULL) || (strstr(token, "Applications") != NULL))
      return APPLICATIONICON;
   if ((strstr(token, "Multimedia") != NULL) || (strstr(token, "Sound_Video") != NULL))
      return MULTIMEDIAICON;
   if ((strstr(token, "Preferences") != NULL) || (strstr(token, "Settings") != NULL))
      return SETTINGSICON;
   if (strstr(token, "System") != NULL)
      return SYSTEMICON;
   return token;
}


/* Create a .directory.eap for this dir */
void
create_dir_eap(char *path, char *cat)
{
   char path2[MAX_PATH];
   G_Eap eap;

   memset(&eap, 0, sizeof(G_Eap));
   eap.name = cat;
   snprintf(path2, sizeof(path2), "%s/.directory.eap", path);
   if (!ecore_file_exists(path2))
     {
        eap.icon_path = set_icon(cat);
        if (!eap.icon_path)
          {
             fprintf(stderr, "ERROR: Cannot Find Icon For %s\n", cat);
             return;
          }
        write_icon(path2, &eap);
     }
}

#define EAP_MIN_WIDTH 8
#define EAP_MIN_HEIGHT 8

#define EAP_EDC_TMPL \
"images {\n"  \
"   image: \"%s\" COMP;\n" \
"}\n" \
"collections {\n" \
"   group {\n" \
"      name: \"icon\";\n" \
"      max: %d %d;\n" \
"      parts {\n" \
"	 part {\n" \
"	    name: \"image\";\n" \
"	    type: IMAGE;\n" \
"	    mouse_events: 0;\n" \
"	    description {\n" \
"	       state: \"default\" 0.00;\n" \
"	       visible: 1;\n" \
"	       aspect: 1.00 1.00;\n" \
"	       rel1 {\n" \
"		  relative: 0.00 0.00;\n" \
"		  offset: 0 0;\n" \
"	       }\n" \
"	       rel2 {\n" \
"		  relative: 1.00 1.00;\n" \
"		  offset: -1 -1;\n" \
"	       }\n" \
"	       image {\n" \
"		  normal: \"%s\";\n" \
"	       }\n" \
"	    }\n" \
"	 }\n" \
"      }\n" \
"   }\n" \
"}\n"

#define EAP_EDC_TMPL_EMPTY \
"images {\n " \
"}\n" \
"collections {\n" \
"}\n"


/* How to create the perfect eap.
 *
 * /dir/to/icon_name.png
 * icon size
 * write EAP_EDC_TMPL to /tmp/temp_file.edc
 * edje_cc -id /dir/to -fd . /tmp/temp_file.edc ~/.e/e/applications/all/file.eap
 *
 * eet_open ~/.e/e/applications/all/file.eap
 * app/info/name =  eap->name
 * app/info/generic = eap->generic
 * app/info/comments = eap->comment
 *
 * app/info/name[lang] =  eap->name->lang
 * app/info/generic[lang] = eap->generic->lang
 * app/info/comments[lang] = eap->comment->lang
 *
 * app/info/startup_notify = eap->startup
 * app/info/exe = eap->exec
 *    Handle the fdo %x replacable params.  Some should be stripped, some should be expanded.
 * app/icon/class
 *    icon/class is a list of standard icons from the theme that can override the icon created above.
 *    Use (from .desktop) eap name,exe name,categories.  It's case sensitive, the reccomendation is to lowercase it.
 *    It should be most specific to most generic.  firefox,browser,internet for instance
 *
 * app/window/class
 *    Guess - exe name vith first letter capitalized.
 * app/window/name
 * app/window/title
 * app/window/role
 * app/info/wait_exit
 *    Wait for app to exit before starting next one.
 * 
 * Some other stuff from e_apps.c e_app_fields_save()
 *    get the icon image into an evas then -
 *    eet_data_image_write(ef, "images/0", 
 *       ecore_evas_buffer_pixels_get(buf), 
 *       a->width, a->height, 
 *       evas_object_image_alpha_get(im), 
 *       1, 0, 0);
 */

void
write_icon(char *file, G_Eap *eap)
{
   Engrave_File *eet;
   Engrave_Image *image;
   Engrave_Group *grp;
   Engrave_Part *part;
   Engrave_Part_State *ps;
   Eet_File *ef;

   const char *ifile;
   char *idir;

#ifdef DEBUG
   fprintf(stderr, "\tWriting file %s\t\twith icon (%s) %s\n", file, eap->icon, eap->icon_path);
#endif
   if ((!eap->icon_path) || (eap->icon_path[0] == '\0') || (!ecore_file_exists(eap->icon_path)))
   {
      eap->icon_path = DEFAULTICON;
      not_found_count++;
   }
   ifile = ecore_file_get_file(eap->icon_path);
   idir = ecore_file_get_dir(eap->icon_path);

   eet = engrave_file_new();
   engrave_file_image_dir_set(eet, idir);

   if (!strcmp(get_icon_compression(), "COMP"))
      image = engrave_image_new(ifile, ENGRAVE_IMAGE_TYPE_COMP, 0);
   else
      image = engrave_image_new(ifile, ENGRAVE_IMAGE_TYPE_LOSSY, 0);

   engrave_file_image_add(eet, image);

   grp = engrave_group_new();
   engrave_group_name_set(grp, "icon");
   engrave_group_max_size_set(grp, 48, 48);
   engrave_file_group_add(eet, grp);

   part = engrave_part_new(ENGRAVE_PART_TYPE_IMAGE);
   engrave_part_name_set(part, "image");
   engrave_part_mouse_events_set(part, 0);
   engrave_group_part_add(grp, part);

   ps = engrave_part_state_new();
   engrave_part_state_name_set(ps, "default", 0.0);
   engrave_part_state_aspect_set(ps, 1.0, 1.0);
   engrave_part_state_image_normal_set(ps, image);
   engrave_part_state_add(part, ps);

   engrave_edj_output(eet, file);
   engrave_file_free(eet);

   if (idir)
      free(idir);

   ef = eet_open(file, EET_FILE_MODE_READ_WRITE);
   if (ef)
     {
        /* FIXME: if there is no name, strip the path and extension of the eap file name and use that. */
        if (eap->name != NULL)
           _write_eap(ef, "app/info/name", eap->name);
        if (eap->generic != NULL)
           _write_eap(ef, "app/info/generic", eap->generic);
        if (eap->comment != NULL)
           _write_eap(ef, "app/info/comments", eap->comment);

        if (eap->exec != NULL)
          {
	     /* FIXME: Handle the fdo %x replacable params.  Some should be stripped, some should be expanded. */
             _write_eap(ef, "app/info/exe", eap->exec);
          }

        if (eap->startup != NULL)
           _write_eap(ef, "app/info/startup_notify", eap->startup);

        if (eap->window_class == NULL)
	{
           int i;
	   char *tmp;

           /* Try reading it from the existing eap. */
           tmp = eet_read(ef, "app/window/class", &i);
	   if (tmp)
	     {
                eap->window_class = malloc(i + 1);
                if (eap->window_class)
	          {
                     memcpy(eap->window_class, tmp, i);
                     eap->window_class[i] = 0;
	          }
	        free(tmp);
	     }
	}
        if ((eap->window_class == NULL) && (eap->exec != NULL))
	  {
	     char *tmp;

	     /* Guess - exe name with first letter capitalized. */
             tmp = strdup(eap->exec);
	     if (tmp)
	       {
	          char *p;

	          p = tmp;
	          while ((*p != '\0') && (*p != ' '))
	            {
	               *p = tolower(*p);
		       p++;
	            }
	          *p = '\0';
	          p = ecore_file_get_file(tmp);  /* In case the exe included a path. */
	          *p = toupper(*p);
                  eap->window_class = strdup(p);
	          free(tmp);
	       }
	  }
        if (eap->window_class != NULL)
           _write_eap(ef, "app/window/class", eap->window_class);

        eet_close(ef);
     }
}


static void
_write_eap(Eet_File *ef, char *section, char *value)
{
   if ((!value) || (value[0] == '\0'))
      eet_delete(ef, section);
   else
     {
        int i = 0;

#ifdef DEBUG
        fprintf(stderr, "\t\t%s:%s\n", section, value);
#endif
        if (!strcmp(section, "app/info/startup_notify"))
          {
             i = atoi(value);
             i = eet_write(ef, section, &i, 1, 0);
          }
        else
             i = eet_write(ef, section, value, strlen(value), 0);
        if (i == 0)
           fprintf(stderr, "Failed to write %s to %s\n", value, section);
     }
}
