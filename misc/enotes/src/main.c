
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


#include "main.h"

/* The Main Function */
int
main(int argc, char *argv[])
{
	char           *spec_conf;

	/* IPC Check */
	ecore_ipc_init();
	dml("IPC Initiated Successfully", 1);

	/* Read the Usage and Configurations */
	main_config = mainconfig_new();
	spec_conf = read_usage_for_configuration_fn(argc, argv);
	if (spec_conf != NULL) {
		read_configuration(main_config, spec_conf);
		free(spec_conf);
	} else {
		read_global_configuration(main_config);
		check_local_configuration();
		read_local_configuration(main_config);
	}
	read_usage_configuration(main_config, argc, argv);

	if (dispusage == 1) {
		mainconfig_free(main_config);
		return (0);
	}

	dml("Successfully Read Configurations and Usage", 1);

	if (find_server() == 0) {
		dml("Server wasn't found.. Creating one", 1);
		/* Setup Server */
		setup_server();

		/* Initialise the E-Libs */
		ecore_init();
		ecore_app_args_set(argc, (const char **) argv);
		if (!ecore_evas_init()) {	/* Initialises Evas.. I hope. :) */
			mainconfig_free(main_config);
			return -1;
		}
		ewl_init(&argc, argv);	/* Initialises Edje.. I hope. :) */
		edje_init();

		dml("Efl Successfully Initiated", 1);

		/* Begin the Control Centre */
		if (main_config->controlcentre == 1) {
			setup_cc();
			dml("Control Centre Setup", 1);
		}

		/* Display the Introduction Note */
		if (main_config->intro == 1) {
			FILE           *file;
			char           *tmpstr = malloc(MAX_TEMPLATE_SIZE);
			char           *template_path = malloc(PATH_MAX);
			NoteStor       *tmpn = NULL;

			snprintf(template_path, PATH_MAX, TEMPLATE_LOC,
				 getenv("HOME"));
			if ((file = fopen(template_path, "r")) != NULL) {
				dml("Using the supplied note template", 2);
				fgets(tmpstr, MAX_TEMPLATE_SIZE, file);
				if ((tmpn =
				     get_notestor_from_value(tmpstr)) != NULL)
					new_note_with_values(tmpn->width,
							     tmpn->height,
							     tmpn->title,
							     tmpn->content);
			} else {
				dml("Using default note template", 2);
				new_note_with_values(0, 0, INTRO_TITLE,
						     INTRO_CONTENT);
			}

			dml("Introduction Note Created", 1);
			if (file != NULL)
				fclose(file);
			if (tmpstr != NULL)
				free(tmpstr);
			if (template_path != NULL)
				free(template_path);
			if (tmpn != NULL)
				free_note_stor(tmpn);
		}

		/* Begin the main loop */
		dml("Starting Main Loop", 1);
		ecore_main_loop_begin();

		dml("Main Loop Ended", 1);

		/* Shutdown the E-Libs */
		edje_shutdown();
		ecore_evas_shutdown();
		ecore_shutdown();
		dml("Efl Shutdown", 1);
	}

	/* End IPC */
	ecore_ipc_shutdown();
	dml("IPC Shutdown", 1);

	/* Free the Configuration */
	mainconfig_free(main_config);
	dml("Configuration Structure Free'd", 1);

	dml("Leaving.", 1);
	return (0);
}
