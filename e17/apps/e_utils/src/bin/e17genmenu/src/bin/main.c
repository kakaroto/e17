#include "global.h"
#include "config.h"
#include "menus.h"
#include "sort.h"

/* Function Prototypes */
void _e17genmenu_backup(void);
void _e17genmenu_help(void);
void _e17genmenu_parseargs(int argc, char **argv);
void _e17genmenu_init(void);
void _e17genmenu_shutdown(void);

/* Functions */
void _e17genmenu_backup()
{
#ifdef DEBUG
   fprintf(stderr, "Backing up Existing Eaps...\n");
#endif
   backup_eaps();
}

void _e17genmenu_help()
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
   printf(" -h | --help\t\tShow this help screen\n");

   /* Stop E Stuff */
   _e17genmenu_shutdown();

   exit(0);
}

void _e17genmenu_parseargs(int argc, char **argv)
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

void _e17genmenu_init()
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
   snprintf(path, sizeof(path), "%s"EAPPDIR, get_home());
   if (!ecore_file_exists(path))
     {
	fprintf(stderr, "ERROR: %s doesn't exist. Where are the eapps?\n", path);
	eet_shutdown();
	ecore_file_shutdown();
	ecore_shutdown();
	exit(-1);
     }
}

void _e17genmenu_shutdown()
{
#ifdef DEBUG
   fprintf(stderr, "Finished\n");
#endif
   eet_shutdown();
   ecore_file_shutdown();
   ecore_shutdown();
   return;
}

int main(int argc, char **argv)
{
   char path[MAX_PATH];

   char *tmp;
   int ret;      
   
   /* Init E Stuff */
   _e17genmenu_init();

    /* Parse Arguments */
   _e17genmenu_parseargs(argc, argv);

    /* Set App Args */
   ecore_app_args_set(argc, (const char **)argv);

   /* Start Making Menus */
   make_menus();

   /* Sort Menus */
   sort_favorites();
   sort_menus();

   /* Update E Cache */
#ifdef DEBUG
   fprintf(stderr, "Regenerating Eapp Cache...\n");
#endif
   snprintf(path, sizeof(path), "enlightenment_eapp_cache_gen %s"EAPPDIR" -r", get_home());
   system(path);

   /* Shutdown */
   _e17genmenu_shutdown();

   return(0);
}
