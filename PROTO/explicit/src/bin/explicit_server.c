#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <limits.h>

#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Con.h>
#include <Ecore_Getopt.h>
#include <Ecore_File.h>

#include <explicit_server.h>

static char *cf = NULL;
static int cf_length = 0;

static const Ecore_Getopt optdesc = {
  "Explicit Daemon",
  NULL,
  PACKAGE_VERSION,
  "(C) 2010 - Freebox S.A.S.",
  "v3 - GNU Lesser General Public License",
  "Explicit Daemon.\n"
  "\n"
  "explicitd centralize all remote ressource request for any apps.\n"
  "It work accross network, but not 'yet' with dbus'\n",
  0,
  {
    ECORE_GETOPT_STORE_STR('u', "user", "user to switch identity to."),
    ECORE_GETOPT_STORE_STR('c', "chroot", "chroot into specified directory."),
    ECORE_GETOPT_STORE_INT('p', "port", "specify server port."),
    ECORE_GETOPT_STORE_STR('s', "server", "server address on which to bind explicitd."),
    ECORE_GETOPT_STORE_STR('a', "cache", "specify where to store/retrieve cache content information."),
    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_COPYRIGHT('C', "copyright"),
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

static Eina_Bool
_explicitd_idler_save(void *data)
{
   Explicit_Cache *cache = data;

   /* Check if we need to save eet files databases. */
   if (cache->changed)
     {
	Eet_File *sync_cache;
	char buffer[PATH_MAX];
	char buffer_final[PATH_MAX];
	int i;

	/* First try to save the cache. */
	strcpy(buffer, cf);
	strcat(buffer, "-tmp");
	sync_cache = eet_open(buffer, EET_FILE_MODE_WRITE);
	if (!sync_cache)
	  return 0;

	if (!eet_data_write(sync_cache, _explicit_cache_descriptor, "explicit", cache, 1))
	  goto on_error;

	if (eet_close(sync_cache) != EET_ERROR_NONE)
	  goto on_error_closed;

	/* Then make all backup file older. */
	for (i = 9; i >= 0; --i)
	  {
	     char buffer_in[PATH_MAX];
	     char buffer_out[PATH_MAX];

	     strcpy(buffer_in, cf);
	     eina_convert_itoa(i, buffer_in + cf_length);

	     strcpy(buffer_out, cf);
	     eina_convert_itoa(i + 1, buffer_out + cf_length);

	     rename(buffer_in, buffer_out);
	  }

	/* Move the newly saved file to first backup file. */
	strcpy(buffer_final, cf);
	eina_convert_itoa(0, buffer_final + cf_length);

	rename(buffer, buffer_final);

	/* Remove backup 10 if everything succeeded. */
	strcpy(buffer, cf);
	eina_convert_itoa(10, buffer + cf_length);

	unlink(buffer);

	cache->changed = EINA_FALSE;

	return 1;

     on_error:
	eet_close(sync_cache);
     on_error_closed:
	unlink(buffer);
	return 0;
     }

   return 0;
}

int
main(int argc, char **argv)
{
   Ecore_Idle_Enterer *idler = NULL;
   Ecore_Con_Server *conn = NULL;
   Explicit_Cache *cache = NULL;
   Eet_File *cache_file = NULL;
   char *server = EXPLICIT_REMOTE_SERVER;
   char *chroot_dir = NULL;
   char *user = NULL;
   char *cf_dir = NULL;
   Eina_Bool quit_option = EINA_FALSE;
   int port = EXPLICIT_REMOTE_PORT;
   int i;

   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_STR(user),
     ECORE_GETOPT_VALUE_STR(chroot_dir),
     ECORE_GETOPT_VALUE_INT(port),
     ECORE_GETOPT_VALUE_STR(server),
     ECORE_GETOPT_VALUE_STR(cf),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_NONE
   };

   eina_init();
   eet_init();
   ecore_init();
   ecore_con_init();

   if (ecore_getopt_parse(&optdesc, values, argc, argv) < 0)
     {
	fprintf(stderr, "Could not parse argument");
	return -2;
     }

   if (quit_option)
     return -1;

   if (!cf)
     {
	char *home = getenv("HOME");

	if (!home) home = "/root/";

	cf_length = strlen(home) + strlen(EXPLICITD_CACHE) + 2;
	cf = alloca(cf_length);
	snprintf(cf, cf_length, "%s/%s", home, EXPLICITD_CACHE);
     }

   cf_length = strlen(cf);

   /* Crete cache file directory */
   cf_dir = ecore_file_dir_get(cf);
   ecore_file_mkpath(cf_dir);
   free(cf_dir);

   explicit_edd_file_init();

   /* Try to load one of eet files database */
   for (i = 0; i < 10; ++i)
     {
	char buffer[PATH_MAX];

	strcpy(buffer, cf);
	eina_convert_itoa(i, buffer + cf_length);
	cache_file = eet_open(buffer, EET_FILE_MODE_READ);

	if (cache_file)
	  {
	     /* Try reading cache content */
	     cache = eet_data_read(cache_file, _explicit_cache_descriptor, "explicit");
	     if (cache) break ;
	  }

	eet_close(cache_file);
     }

   if (!cache)
     {
	cache = calloc(1, sizeof (Explicit_Cache));
	/* Do set some minimal sane value. */
     }

   /* Check cache content, or do it async ? */

   /* Start listening server */
   conn = ecore_con_server_add(ECORE_CON_REMOTE_TCP, server, port,
			       /* Pointer to root cache needed */ cache);

   /* FIXME: setup the idler when cache changed. */
   /* Setup idler to save eet files databases update */
   idler = ecore_idle_enterer_add(_explicitd_idler_save,
				  /* Pointer to root cache needed */ cache);

   /* Wait for request and download */
   ecore_main_loop_begin();

   explicit_edd_file_shutdown();

   if (cache_file)
     {
	/* Rename all file */
	/* Save new file cache content */

	/* Close old one and loose all strings references */
	eet_close(cache_file);
     }

   ecore_con_shutdown();
   ecore_shutdown();
   eet_shutdown();
   eina_shutdown();

   return 0;
}
