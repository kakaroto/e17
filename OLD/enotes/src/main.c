
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

MainConfig     *main_config;
char           *remotecmd;
Ecore_Timer    *autosave_timer = NULL;

static int
_autosave_timer_tick(void *data)
{
	autosave();

	return 1;
}

void
update_autosave(void)
{
	if (main_config->autosave == 1 && autosave_timer == NULL)
		autosave_timer =
			ecore_timer_add(30, _autosave_timer_tick, NULL);
	if (main_config->autosave == 0 && autosave_timer != NULL) {
		ecore_timer_del(autosave_timer);
		autosave_timer = NULL;
	}
}

/* The Main Function */

/**
 * @param argc: Number of command line arguments supplied.
 * @param argv: Char* array containing the command line arguments supplied.
 * @return: To the system, normally 0.
 * @brief: The first function once enotes is called.
 */
int
main(int argc, char *argv[])
{
	int             note_count;

	/* IPC Check */
	ecore_ipc_init();
	dml("IPC Initiated Successfully", 1);

	/* loading will increment this if there are notes if not we may need to
	 * create a blank one */
	note_count = 0;

	if ((ecore_config_init("enotes")) == ECORE_CONFIG_ERR_FAIL) {
		ecore_ipc_shutdown();
		return (-1);
	}
	ecore_app_args_set(argc, (const char **) argv);

	ecore_config_app_describe("E-Notes - Sticky Notes for Enlightenment\n\
Copyright (c) Thomas Fletcher\n\
Usage: enotes [options]");

	/* Read the Usage and Configurations */
	main_config = mainconfig_new();
	if (read_configuration(main_config) != ECORE_CONFIG_PARSE_CONTINUE) {
		ecore_config_shutdown();
		ecore_ipc_shutdown();
		ecore_shutdown();
		mainconfig_free(main_config);
		return (-1);
	}

	dml("Successfully Read Configurations and Usage", 1);

	process_note_storage_locations();

	if (find_server() != 0) {
		if (remotecmd != NULL)
			send_to_server(remotecmd);
		else
			send_to_server("DEFNOTE");
	} else {
		dml("Server wasn't found.. Creating one", 1);
		/* Setup Server */
		setup_server();

		/* Initialise the E-Libs */
		ecore_init();
		ecore_x_init(NULL);
		ecore_app_args_set(argc, (const char **) argv);
		if (!ecore_evas_init()) {
			mainconfig_free(main_config);
			return -1;
		}
		ewl_init(&argc, argv);
		edje_init();

		dml("Efl Successfully Initiated", 1);

		autoload();
		/* create autosave timer */
		update_autosave();

		if (remotecmd != NULL)
			handle_ipc_message(remotecmd);

		/* Begin the Control Centre */
		if (main_config->controlcentre == 1) {
			setup_cc();
			dml("Control Centre Setup", 1);
		} else {
			dml("No Control Centre - Displaying Notice", 1);
			if (get_note_count() == 0)
				new_note();
		}

		if (main_config->welcome == 1) {
			open_welcome();
		}

		/* Begin the main loop */
		dml("Starting Main Loop", 1);
		ecore_main_loop_begin();

		dml("Main Loop Ended", 1);

		/* Save Controlcentre Settings */
		set_cc_pos();

		autosave();
		if (autosave_timer)
			ecore_timer_del(autosave_timer);

		/* Save and Free the Configuration */
		ecore_config_save();
		dml("Configuration Saved", 1);
		mainconfig_free(main_config);
		dml("Configuration Structure Free'd", 1);

		/* Shutdown the E-Libs */
		edje_shutdown();
		ecore_evas_shutdown();
		ecore_x_shutdown();
		ecore_shutdown();
		dml("Efl Shutdown", 1);
	}

	/* End IPC */
	ecore_ipc_shutdown();
	dml("IPC Shutdown", 1);

	dml("Leaving.", 1);
	return (0);
}
