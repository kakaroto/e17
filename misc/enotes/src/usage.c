
/**************************************************
 **               E  -  N O T E S                **
 **                                              **
 **  The contents of this file are released to   **
 **  the public under the General Public Licence **
 **  Version 2.                                  **
 **                                              **
 **  By  Thomas Fletcher (www.fletch.vze.com)    **
 **                                              **
 **************************************************/


#include "usage.h"

extern MainConfig *main_config;
int             dispusage;

/* Reading the Usage */
char           *
read_usage_for_configuration_fn(int argc, char *argv[])
{
	int             a = -1;

	while (a++ < argc - 1) {
		if (!strcmp(argv[a], "-c")) {
			return (strdup(argv[a + 1]));
		}
	}
	return (NULL);
}

void
read_usage_configuration(MainConfig * p, int argc, char *argv[])
{
	int             c;
	int             option_index = 0;
	Ecore_Ipc_Server *svr;

	while (1) {
		c = getopt_long(argc, argv, OPTSTR, long_options,
				&option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'd':
			if (atoi(optarg) > -1 && atoi(optarg) < 3)
				main_config->debug = atoi(optarg);
			break;
		case 'R':
			if (find_server() == 1) {
				send_to_server(optarg);
			}
			dispusage = 1;
			break;
		case 'r':
			if (main_config->render_method != NULL)
				free(main_config->render_method);
			main_config->render_method = strdup(optarg);
			break;
		case 't':
			if (main_config->theme != NULL)
				free(main_config->theme);
			main_config->theme = strdup(optarg);
			break;
		case 'C':
			if (atoi(optarg) == 0 || atoi(optarg) == 1)
				main_config->controlcentre = atoi(optarg);
			break;
		case 'A':
			if (atoi(optarg) == 0 || atoi(optarg) == 1)
				main_config->autosave = atoi(optarg);
			break;
		case 'i':
			if (atoi(optarg) == 0 || atoi(optarg) == 1)
				main_config->intro = atoi(optarg);
			break;
		case 'v':
			printf(USAGE_VERSION, VERSION);
			dispusage = 1;
			break;
		case '?':
			print_usage();
			break;
		default:
			print_usage();
			break;
		}
	}
	return;
}


/* Printing the Usage */
void
print_usage(void)
{
	dispusage = 1;
	printf(USAGE);
	return;
}
