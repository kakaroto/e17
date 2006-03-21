#include "global.h"
#include "config.h"
#include "menus.h"
#include "parse.h"
#include "icons.h"
#include "sort.h"
#include "fdo_menus.h"
#include "fdo_paths.h"
#include "xmlame.h"

#define DEBUG 1

/* Function Prototypes */
void _e17genmenu_backup(void);
void _e17genmenu_help(void);
void _e17genmenu_parseargs(int argc, char **argv);
void _e17genmenu_init(void);
void _e17genmenu_shutdown(void);

/* Functions */
void
_e17genmenu_backup()
{
#ifdef DEBUG
   fprintf(stderr, "Backing up Existing Eaps...\n");
#endif
   backup_eaps();
}

void
_e17genmenu_help()
{
   printf("e17genmenu - Generate Enlightenment DR17 Menus\n");
   printf("Usage: e17genmenu <-options>\n");
   printf("\n");
   printf(" -b | --backup\tBackup Existing Eaps First\n");
   printf(" -s=<size> | --icon-size=<size>\tUse <size> icons. (48x48)\n");
   printf(" -t=<theme> | --icon-theme=<theme>\tUse <theme> for icons\n");
   printf(" -l | --lossy\tUse Lossy Compression For Eaps\n");
   printf(" -d=<dir> | --desktop-dir=<dir>\tCreate eaps for .desktop files in <dir>\n");
   printf(" -o | --overwrite\tOverwrite Eaps\n");
   printf(" -m | --mapping\tGenerate Mapping File\n");
   printf(" -f | --fdo\tGenerate menus from freedesktop.org (fdo) files only\n");
   printf(" -h | --help\t\tShow this help screen\n");

   /* Stop E Stuff */
   _e17genmenu_shutdown();

   exit(0);
}

void
_e17genmenu_parseargs(int argc, char **argv)
{
   int i;

   for (i = 1; i < argc; i++)
     {
        if (argv[i])
          {
             if ((strstr(argv[i], "--help")) || (strstr(argv[i], "-h")))
                _e17genmenu_help();
             if ((strstr(argv[i], "--backup")) || (strstr(argv[i], "-b")))
                _e17genmenu_backup();
          }
     }
}

void
_e17genmenu_init()
{
   char path[MAX_PATH];

   /* Setup Ecore */
   if (!ecore_init())
     {
        fprintf(stderr, "ERROR: Unable to init ecore, exiting\n");
        exit(-1);
     }

   /* Setup Ecore_File */
   if (!ecore_file_init())
     {
        fprintf(stderr, "ERROR: Unable to init ecore_file, exiting\n");
        ecore_shutdown();
        exit(-1);
     }

   /* Setup Eet */
   if (!eet_init())
     {
        fprintf(stderr, "ERROR: Unable To Init Eet !\n");
        ecore_file_shutdown();
        ecore_shutdown();
        exit(-1);
     }

   /* Check That Dir All Exists */
   snprintf(path, sizeof(path), "%s" EAPPDIR, get_home());
   if (!ecore_file_exists(path))
     {
        fprintf(stderr, "ERROR: %s doesn't exist. Where are the eapps?\n", path);
        eet_shutdown();
        ecore_file_shutdown();
        ecore_shutdown();
        exit(-1);
     }
}

void
_e17genmenu_shutdown()
{
   eet_shutdown();
   ecore_file_shutdown();
   ecore_shutdown();
   return;
}

double convert_time = 0.0, icon_time = 0.0, cache_time = 0.0, generate_time = 0.0;
int menu_count, item_count, reject_count;

int
main(int argc, char **argv)
{
   char path[MAX_PATH];
   double start, begin, paths, gen;
#ifdef DEBUG
   int i;
#endif

   /* Init E Stuff */
   _e17genmenu_init();

   start = ecore_time_get();
   /* Parse Arguments */
   _e17genmenu_parseargs(argc, argv);

   /* Set App Args */
   ecore_app_args_set(argc, (const char **)argv);

   /* Get the fdo paths. */
   begin = ecore_time_get();
   fdo_paths_init();
   paths = ecore_time_get() - begin;
   parse_ini_init();

#ifdef DEBUG
   /* You can iterate through the various path lists as needed. */
   for (i = 0; i < fdo_paths_config->size; i++)
      printf("FDO config path = %s\n", (char *)fdo_paths_config->elements[i].element);
   for (i = 0; i < fdo_paths_menus->size; i++)
      printf("FDO menu path = %s\n", (char *)fdo_paths_menus->elements[i].element);
   for (i = 0; i < fdo_paths_directories->size; i++)
      printf("FDO directory path = %s\n", (char *)fdo_paths_directories->elements[i].element);
   for (i = 0; i < fdo_paths_desktops->size; i++)
      printf("FDO desktop path = %s\n", (char *)fdo_paths_desktops->elements[i].element);
   for (i = 0; i < fdo_paths_icons->size; i++)
      printf("FDO icon path = %s\n", (char *)fdo_paths_icons->elements[i].element);
   for (i = 0; i < fdo_paths_kde_legacy->size; i++)
      printf("FDO kde legacy path = %s\n", (char *)fdo_paths_kde_legacy->elements[i].element);
#endif


   /* Just being paranoid, and cause people have removed these during testing. */
   snprintf(path, sizeof(path), "%s/.e/e/applications/all", get_home());
   ecore_file_mkpath(path);
   snprintf(path, sizeof(path), "%s/.e/e/applications/favorite", get_home());
   ecore_file_mkpath(path);

   begin = ecore_time_get();
   /* Start Making Menus */
   make_menus();
   gen = ecore_time_get() - begin;

   /* Sort Menus */
   /* FIXME: One or both of these get's it wrong, plus they don't seem to be needed anyway.
   sort_favorites();
   sort_menus();
   *
   * Currently I do this instead, it seems to work -
   */
   snprintf(path, sizeof(path), "%s/.e/e/applications/favorite/.eap.cache.cfg", get_home());
   ecore_file_unlink(path);

   /* Update E Cache */
   begin = ecore_time_get();
#ifdef DEBUG
   fprintf(stderr, "Regenerating Eapp Caches...\n");
#endif
   snprintf(path, sizeof(path), "enlightenment_eapp_cache_gen %s" EAPPDIR " -r", get_home());
   system(path);
   snprintf(path, sizeof(path), "enlightenment_eapp_cache_gen %s/.e/e/applications/favorite -r", get_home());
   system(path);
   cache_time += ecore_time_get() - begin;

   printf("********************************************************************************\n");
   printf("* WARNING!                                                                     *\n");
   printf("*                                                                              *\n");
   printf("* E17 will be completely unresponsive for a few minutes after this finishes    *\n");
   printf("* running.  This is due to a bad interaction with the eap caching code when    *\n");
   printf("* you generate a lot of eaps at once, like e17genmenu just did.  raster is     *\n");
   printf("* aware of this problem, and has a fix on his TODO list.                       *\n");
   printf("********************************************************************************\n");

   printf("\nTotal time %3.3f seconds, finding fdo paths %3.3f, converting fdo menus %3.3f, converting to %d (rejected %d) eaps in %d fdo menus %3.3f, generating menus %3.3f, finding icons %3.3f, generating eap caches %3.3f.\n",
       ecore_time_get() - start, paths, convert_time, item_count, reject_count, menu_count, gen - (icon_time + generate_time), generate_time, icon_time, cache_time);

#if 0
   /* This is just a test of parsing speed for the old weather data's huge xml file.  It passed the parse test.  B-) */
   {
      double weather;
      Dumb_Tree *weather_xml;

      begin = ecore_time_get();
      weather_xml = xmlame_get("dir.xml");
      weather = ecore_time_get() - begin;
      dumb_tree_dump(weather_xml, 0);
      printf("\nWeather horror parsed in %3.3f seconds.\n", weather);
   }
#endif

   parse_ini_shutdown();
   fdo_paths_shutdown();

   /* Shutdown */
   _e17genmenu_shutdown();

   return (0);
}
