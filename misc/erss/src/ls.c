#include "erss.h"

static int erss_alphasort (const void *a, const void *b)
{
	struct dirent **ad = (struct dirent **)a;
	struct dirent **bd = (struct dirent **)b;
	return (strcmp((*bd)->d_name, (*ad)->d_name));
}

void erss_list_config_files (int output)
{
	char *str;
	char *ptr;
	Ewd_List *paths;
	struct dirent **dentries;
	struct stat statbuf;
	int num, i;
	char file[PATH_MAX];
	int found_files = FALSE;
	int no_dir = FALSE;

	paths = ewd_list_new ();
	config_files = ewd_list_new ();

	str = malloc (PATH_MAX);
	snprintf (str, PATH_MAX, "%s/.%s/config", getenv("HOME"), PACKAGE);
	ewd_list_append (paths, str);

	str = malloc (PATH_MAX);
	snprintf (str, PATH_MAX, "/etc/%s/config", PACKAGE);
	ewd_list_append (paths, str);

	str = malloc (PATH_MAX);
	snprintf (str, PATH_MAX, "%s/config", PACKAGE_DATA_DIR);
	ewd_list_append (paths, str);

	if (output)
		printf ("\n%s processing potential dirs ...\n", PACKAGE);

	ptr = ewd_list_goto_first (paths);
	while ((ptr = ewd_list_current (paths))) {
		if (output)
			printf ("\nprocessing '%s':\n", ptr);

		i = stat (ptr, &statbuf);

		if (i == -1) {
			no_dir = TRUE;
		} else {
			if (S_ISDIR(statbuf.st_mode)) {

				if ((num = scandir(ptr, &dentries, 0, erss_alphasort)) < 0) 
					perror("erss - scandir");

				while (num--) {
					snprintf(file, PATH_MAX, "%s/%s", ptr, dentries[num]->d_name);

					i = stat (file, &statbuf);
					if (i == -1) {
						perror("erss - stat 1");
						continue;
					}

					if (S_ISDIR(statbuf.st_mode))
						continue;

					if (strstr (dentries[num]->d_name, ".cfg")) {
						found_files = TRUE;
						ewd_list_append (config_files, strdup (file));
						if (output)
							printf ("\t%s\n", file);
					}
				}

				if (!found_files) {
					if (output)
						printf ("\tno config files in this dir ...\n");
					found_files = FALSE;
				}
			}
		}

		if (output) {
			if (no_dir)
				printf ("\tno such dir ...\n");
			no_dir = FALSE;
		}

		ewd_list_next (paths);
	}

	/*
	 * Finished reading and printing available config files
	 * now remove the paths list since we don't need it anymore.
	 */
	ptr = ewd_list_goto_first (paths);
	while ((ptr = ewd_list_current (paths))) {
		if (ptr)
			free (ptr);
		
		ewd_list_next (paths);
	}

	ewd_list_destroy (paths);
}
