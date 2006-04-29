#include "global.h"
#include "config.h"

/* Functions */
char *
get_home()
{
   char *d;
   int length;
   char home[MAX_PATH];

   /* Get Home Dir, check for trailing '/', strip it */
   snprintf(home, sizeof(home), "%s", getenv("HOME"));
   d = strrchr(home, '/');
   if (d)
     {
        if (strlen(d) == 1)
          {
             if (home[(length = strlen(home) - 1)] == '/')
                home[length] = '\0';
          }
     }
   return strdup(home);
}

char *
get_desktop_dir()
{
   int i, argc;
   char **argv;
   char *c;

   ecore_app_args_get(&argc, &argv);
   for (i = 1; i < argc; i++)
     {
        if ((strncmp(argv[i], "-d", 2) == 0) || (strncmp(argv[i], "--desktop-dir", 13) == 0))
          {
             c = strstr(argv[i], "=");
             c = strtok(c, "=");
             if (c)
                return strdup(c);
             fprintf(stderr, "You must specify a directory for the %s argument\n", argv[i]);
             exit(1);
          }
     }
   return NULL;
}

int
get_overwrite()
{
   int i, argc;
   char **argv;

   ecore_app_args_get(&argc, &argv);
   for (i = 1; i < argc; i++)
     {
        if ((!strcmp(argv[i], "-o")) || (!strcmp(argv[i], "--overwrite")))
           return 1;
     }
   return 0;
}

int
get_fdo()
{
   int i, argc;
   char **argv;

   ecore_app_args_get(&argc, &argv);
   for (i = 1; i < argc; i++)
     {
        if ((!strcmp(argv[i], "-f")) || (!strcmp(argv[i], "--fdo")))
           return 1;
     }
   return 0;
}

char *
get_icon_compression()
{
   int i, argc;
   char **argv;

   ecore_app_args_get(&argc, &argv);
   for (i = 1; i < argc; i++)
     {
        if ((strncmp(argv[i], "--lossy", 11) == 0) || (strncmp(argv[i], "-l", 2) == 0))
           return "LOSSY";
     }
   return "COMP";
}

char *
get_eap_name(char *file)
{
   char *tmp2;
   char eap[MAX_PATH];

   tmp2 = strdup(ecore_file_get_file(file));
   if (!tmp2)
      return NULL;

   if (strrchr(tmp2, '.') != NULL)
     {
        snprintf(eap, sizeof(eap), "%s.eap", strtok(tmp2, "."));
	free(tmp2);
        return strdup(eap);
     }
   free(tmp2);
   return NULL;
}

char *
get_icon_size()
{
   int i, argc;
   char **argv;
   char *c;

   ecore_app_args_get(&argc, &argv);
   for (i = 1; i < argc; i++)
     {
        if ((strncmp(argv[i], "--icon-size", 11) == 0) || (strncmp(argv[i], "-s", 2) == 0))
          {
             c = strstr(argv[i], "=");
             c = strtok(c, "=");
             return strdup(c);
          }
     }
   return "48x48";
}

char *
get_icon_theme()
{
   int i, argc;
   char **argv;
   char *c;

   ecore_app_args_get(&argc, &argv);
   for (i = 1; i < argc; i++)
     {
        if ((strncmp(argv[i], "--icon-theme", 12) == 0) || (strncmp(argv[i], "-t", 2) == 0))
          {
             c = strstr(argv[i], "=");
             c = strtok(c, "=");
             return strdup(c);
          }
     }
   /* Doesn't this tie us specifically to KDE ? */
//   return "crystalsvg";
   return "hicolor";
}

int
get_mapping()
{
   int i, argc;
   char **argv;

   ecore_app_args_get(&argc, &argv);
   for (i = 1; i < argc; i++)
     {
        if ((!strcmp(argv[i], "--mapping")) || (!strcmp(argv[i], "-m")))
           return 1;
     }
   return 0;
}

void
backup_eaps()
{
   int result;
   char *file;
   char path[MAX_PATH], tmp[MAX_PATH], dest[MAX_PATH];
   Ecore_List *eaps = NULL;

   /* Check That Dir All Exists */
   snprintf(path, sizeof(path), "%s" EAPPDIR, get_home());
   if (!ecore_file_exists(path))
     {
        fprintf(stderr, "ERROR: %s doesn't exist. Where are the eapps ?\n", path);
        return;
     }

   if (ecore_file_is_dir(path))
      eaps = ecore_file_ls(path);
   if (!eaps)
      return;

   ecore_list_goto_first(eaps);
   while ((file = ecore_list_next(eaps)) != NULL)
     {
        if (!strcmp(file, ".") || !strcmp(file, ".."))
           continue;
        if (!ecore_file_is_dir(file))
          {
             snprintf(tmp, sizeof(tmp), "%s/%s", path, file);
#ifdef DEBUG
             fprintf(stderr, "Backing Up %s\n", tmp);
#endif
             snprintf(dest, sizeof(dest), "%s/EapBackup", get_home());
             if (!ecore_file_exists(dest))
               {
                  if (!ecore_file_mkdir(dest))
                     return;
               }

             /* Copy The File */
             snprintf(dest, sizeof(dest), "%s/EapBackup/%s", get_home(), file);
             result = ecore_file_cp(tmp, dest);
             if (!result)
                break;
          }
     }
   if (eaps)
      ecore_list_destroy(eaps);
}

void
write_mapping_file(G_Eap *eap)
{
   FILE *f;
   char buff[MAX_PATH], path[MAX_PATH];

   if ((!eap->icon) || (!eap->name) || (!eap->exec) || (!eap->window_class))
      return;

   snprintf(path, sizeof(path), "%s/MAPPING", get_home());
   f = fopen(path, "a+");
   if (!f)
     {
        fprintf(stderr, "ERROR: Cannot Open Mapping File\n");
        return;
     }
   snprintf(buff, sizeof(buff), "%s|!%s|!%s|!%s\n", eap->icon, eap->name, eap->window_class, eap->exec);
   fwrite(buff, sizeof(char), strlen(buff), f);
   fclose(f);
}
