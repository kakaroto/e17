#include <stdlib.h>
#include <limits.h>

#include "global.h"
#include "config.h"
#include "fdo_paths.h"
#include "icons.h"
#include "parse.h"


/* FIXME: Ideally this should be -
 * {".png", ".svg", ".xpm", "", NULL}
 * Add them in when they are supported in .eaps.
 */ 
static const char *ext[] = {".png", "", NULL};


char *
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
   if ((strstr(token, "Accessories") != NULL) ||
       (strstr(token, "Applications") != NULL))
      return APPLICATIONICON;
   if ((strstr(token, "Multimedia") != NULL) ||
       (strstr(token, "Sound_Video") != NULL))
      return MULTIMEDIAICON;
   if ((strstr(token, "Preferences") != NULL) ||
       (strstr(token, "Settings") != NULL))
      return SETTINGSICON;
   if (strstr(token, "System") != NULL)
      return SYSTEMICON;
   return token;
}

char *
find_icon(char *icon)
{
   char icn[MAX_PATH], path[MAX_PATH];
   char *dir, *icon_size, *icon_theme, *home;

   if (icon == NULL)
      return strdup(DEFAULTICON);

   home = get_home();

   snprintf(icn, sizeof(icn), "%s", icon);
#ifdef DEBUG
   fprintf(stderr, "\tTrying To Find Icon %s\n", icn);
#endif

   /* Check For Unsupported Extension */
   if ((!strcmp(icon + strlen(icon) - 4, ".svg"))
       || (!strcmp(icon + strlen(icon) - 4, ".ico"))
       || (!strcmp(icon + strlen(icon) - 4, ".xpm")))
      return strdup(DEFAULTICON);

   /* Check For An Extension, Append PNG If Missing */
   if (strrchr(icon, '.') == NULL)
      snprintf(icn, sizeof(icn), "%s.png", icon);

   /* Get Icon Options */
   icon_size = get_icon_size();
   icon_theme = get_icon_theme();

   if (!icon_theme) 
     {
	
	/* Check If Dir Supplied In Desktop File */
	dir = ecore_file_get_dir(icn);
	if (!strcmp(dir, icn) == 0)
	  {
	     snprintf(path, MAX_PATH, "%s", icn);
	     /* Check Supplied Dir For Icon */
	     if (ecore_file_exists(path))
	       return strdup(icn);
	  }
     }
   
   return strdup(find_fdo_icon(icon, icon_size, icon_theme));
}

/** Search for an icon the fdo way.
 *
 * This complies with the freedesktop.org Icon Theme Specification version 0.7
 *
 * @param   icon The icon to search for.
 * @param   icon_size The icon size to search for.
 * @param   icon_theme The icon theme to search in.
 * @return  The full path to the found icon.
 */
char *
find_fdo_icon(char *icon, char *icon_size, char *icon_theme)
{
   /*  NOTES ON OPTIMIZATIONS
    *
    * The spec has this to say -
    *
    * "The algorithm as described in this document works by always looking up 
    * filenames in directories (a stat in unix terminology). A good 
    * implementation is expected to read the directories once, and do all 
    * lookups in memory using that information.
    *
    * "This caching can make it impossible for users to add icons without having 
    * to restart applications. In order to handle this, any implementation that 
    * does caching is required to look at the mtime of the toplevel icon 
    * directories when doing a cache lookup, unless it already did so less than 
    * 5 seconds ago. This means that any icon editor or theme installation 
    * program need only to change the mtime of the the toplevel directory where 
    * it changed the theme to make sure that the new icons will eventually get 
    * used."
    *
    * On the other hand, OS caching (at least in linux) seems to do a reasonable 
    * job here.
    *
    * We could also precalculate and cache all the information extracted from 
    * the .theme files.
    */

   char icn[MAX_PATH], path[MAX_PATH];
   char *theme_path, *found;

   if (icon == NULL)
      return DEFAULTICON;

#ifdef DEBUG
   fprintf(stderr, "\tTrying To Find Icon %s (%s) in theme %s\n", icon, icon_size, icon_theme);
#endif

   /* Get the theme description file. */
   snprintf(icn, MAX_PATH, "%s/index.theme", icon_theme);
#ifdef DEBUG
   printf("SEARCHING FOR %s\n", icn);
#endif
   theme_path = fdo_paths_search_for_file(FDO_PATHS_TYPE_ICON, icn, 1, NULL, NULL);
   if (theme_path)
      {
         Ecore_Hash *theme;

         /* Parse the theme description file. */
#ifdef DEBUG
         printf("Path to %s is %s\n", icn, theme_path);
#endif
         theme = parse_ini_file(theme_path);
	 if (theme)
	    {
	       Ecore_Hash *icon_group;

               /* Grab the themes directory list, and what it inherits. */
               icon_group = (Ecore_Hash *) ecore_hash_get(theme, "Icon Theme");
	       if (icon_group)
	          {
	             char *directories, *inherits;

                     directories = (char *) ecore_hash_get(icon_group, "Directories");
                     inherits = (char *) ecore_hash_get(icon_group, "Inherits");
		     if (directories)
		        {
                           Dumb_List *directory_paths;

                           /* Split the directory list. */
#ifdef DEBUG
                           printf("Inherits %s Directories %s\n", inherits, directories);
#endif
                           directory_paths = dumb_list_from_paths(directories);
			   if (directory_paths)
			      {
			         int wanted_size;
				 int minimal_size = INT_MAX;
			         int i;
				 char *closest = NULL;

                                 wanted_size = atoi(icon_size);
                                 /* Loop through the themes directories. */
                                 for (i = 0; i < directory_paths->size; i++)
				    {
	                               Ecore_Hash *sub_group;

#ifdef DEBUG
                                       printf("FDO icon path = %s\n", directory_paths->elements[i].element);
#endif
				       /* Get the details for this theme directory. */
                                       sub_group = (Ecore_Hash *) ecore_hash_get(theme, directory_paths->elements[i].element);
				       if (sub_group)
				          {
	                                     char *size, *type, *minsize, *maxsize, *threshold;
					     int j;

                                             size = (char *) ecore_hash_get(sub_group, "Size");
                                             type = (char *) ecore_hash_get(sub_group, "Type");
                                             minsize = (char *) ecore_hash_get(sub_group, "MinSize");
                                             maxsize = (char *) ecore_hash_get(sub_group, "MaxSize");
                                             threshold = (char *) ecore_hash_get(sub_group, "Threshold");
					     if (size)
					        {
						   int match = 0;
						   int this_size, result_size = 0, min_size, max_size, thresh_size;

                                                   if (!minsize)   minsize = size;
                                                   if (!maxsize)   maxsize = size;
                                                   if (!threshold)   threshold = "2";
                                                   min_size = atoi(minsize);
                                                   max_size = atoi(maxsize);
                                                   thresh_size = atoi(threshold);

                                                   /* Does this theme directory match the required icon size? */
                                                   this_size = atoi(size);
						   if (!type)
						      type = "Threshold";
						   switch (type[0])
						      {
						         case 'F' :   /* Fixed. */
							    {
							       match = (wanted_size == this_size);
							       result_size = abs(this_size - wanted_size);
							       break;
							    }
						         case 'S' :   /* Scaled. */
							    {
							       match = ((min_size <= wanted_size) && (wanted_size <= max_size));
							       if (wanted_size < min_size)
							          result_size = min_size - wanted_size;
							       if (wanted_size > max_size)
							          result_size = wanted_size - max_size;
							       break;
							    }
						         default :    /* Threshold. */
							    {
							       match = ( ((this_size - thresh_size) <= wanted_size) && (wanted_size <= (this_size + thresh_size)) );
							       if (wanted_size < (this_size - thresh_size))
							          result_size = min_size - wanted_size;
							       if (wanted_size > (this_size + thresh_size))
							          result_size = wanted_size - max_size;
							       break;
							    }
						      }

                                                   /* Look for icon with all extensions. */
                                                   for (j = 0; ext[j] != NULL; j++)
						      {
                                                         snprintf(path, MAX_PATH, "%s/%s/%s%s", icon_theme, (char *) directory_paths->elements[i].element, icon, ext[j]);
#ifdef DEBUG
                                                         printf("FDO icon = %s\n", path);
#endif
                                                         found = fdo_paths_search_for_file(FDO_PATHS_TYPE_ICON, path, 0, NULL, NULL);
							 if (found)
							    {
							       if (match)   /* If there is a match in sizes, return the icon. */
							          return found;
							       if (result_size < minimal_size)   /* While we are here, figure out our next fallback strategy. */
							          {
								     minimal_size = result_size;
								     closest = found;
								  }
							    }
						      }

						}
					  }
				    }   /* for (i = 0; i < directory_paths->size; i++) */

                                 /* Fall back strategy #1, look for closest size in this theme. */
				 if (closest)
				    return closest;

                                 /* Fall back strategy #2, Try again with the parent theme. */
				 if ((inherits) && (inherits[0] != '\0') && (strcmp(icon_theme, "hicolor") != 0))
				    {
                                       found = find_fdo_icon(icon, icon_size, inherits);
				       if (found != DEFAULTICON)
				          return found;
				    }

                                 /* Fall back strategy #3, Try the default hicolor theme. */
				 if ( (! ((inherits) && (inherits[0] != '\0')) ) && (strcmp(icon_theme, "hicolor") != 0))
				    {
                                       found = find_fdo_icon(icon, icon_size, "hicolor");
				       if (found != DEFAULTICON)
				          return found;
				    }

                                 /* Fall back strategy #4, Just search in the base of the icon directories. */
                                  for (i = 0; ext[i] != NULL; i++)
				     {
                                        snprintf(path, MAX_PATH, "%s%s", icon, ext[i]);
#ifdef DEBUG
                                        printf("FDO icon = %s\n", path);
#endif
                                        found = fdo_paths_search_for_file(FDO_PATHS_TYPE_ICON, path, 0, NULL, NULL);
				        if (found)
				          return found;
				     }

			      }
			}
		  }
	    }
	 free(theme_path);
      }

   return DEFAULTICON;
}
