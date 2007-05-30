#include "Empower.h"

int main(int argc, char** argv)
{
	--argc; ++argv;		//pop off program name
	sudo = NULL;
	
	if(!ecore_init())
	{
		printf("Unable to init ecore\n");
		return 1;
	}
	
	if(argc)	//commands
	{		
		int i;

		ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT,exit_cb,NULL);
		
		snprintf(cmd, 1024, "sudo");
		
		while(argc)
		{
			if(strcmp(*argv, "--"))		//-- is used to stop sudo options, lets use that
			{
				strncat(cmd, " ", 1024);
				strncat(cmd, *argv, 1024);
				--argc; ++argv;		//pop these options off so ewl just gets its options
			}
			else
				break;
		}	
		
		if(!ewl_init(&argc, argv))
		{
			printf("Unable to init ewl\n");
			return;
		}
		
		ecore_job_add(check_sudo_timeout_job, NULL);
		
		ewl_main();
	
		ecore_exe_run(cmd,NULL);
	}
	else
	{
		printf("Usage: ./empower [SUDO OPTIONS] <program name> [PROGRAM OPTIONS] -- [EWL OPTIONS]\n");
		return 1;
	}
	
	return 0;
}
