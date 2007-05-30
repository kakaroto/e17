#include "Empower.h"

int main(int argc, char** argv)
{
	sudo = NULL;
	
	if(!strcmp(*argv,"empower-askpass"))
		mode = PASS;
	else if(!strcmp(*argv, "./empower-askpass"))
		mode = PASS;
	else
		mode = SUDO;

	--argc; ++argv;		//pop off program name
	
	if(mode != PASS && !strcmp(*argv, "-p"))
		mode = PASS;
	
	if(!ecore_init())
	{
		printf("Unable to init ecore\n");
		return 1;
	}
	
	if(argc || mode == PASS)	//commands
	{		
		int i;

		ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT,exit_cb,NULL);
		
		if(mode == SUDO)
			snprintf(cmd, 1024, "sudo");
		
		while(argc)
		{
			if(strcmp(*argv, "--"))
			{
				if(mode == SUDO)
				{
					strncat(cmd, " ", 1024);
					strncat(cmd, *argv, 1024);
					--argc; ++argv;
				}
			}
			else
				break;
		}	
		
		if(!ewl_init(&argc, argv))
		{
			printf("Unable to init ewl\n");
			return;
		}
		
		if(mode == SUDO)
			ecore_job_add(check_sudo_timeout_job, NULL);
		else if(mode == PASS)
			display_window();
		
		ewl_main();
	
		if(mode == SUDO)
			ecore_exe_run(cmd,NULL);
	}
	else
	{
		printf("-=Usage=-\n");
		printf("    Sudo:  empower [SUDO OPTIONS] <program> [PROGRAM OPTIONS] -- [EWL OPTIONS]\n");
		printf("    AskPass:  empower-askpass -- [EWL OPTIONS] or empower -p -- [EWL OPTIONS]\n");
		return 1;
	}
	
	return 0;
}
