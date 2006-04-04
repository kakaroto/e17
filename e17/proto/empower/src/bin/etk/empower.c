#include "../Empower.h"

int main(int argc, char** argv)
{
	--argc; ++argv;
	
	if(argc)	//commands
	{
		pid_t id = fork();
		
		if(id == 0)
		{			
			ecore_exe_run("sudo -k", NULL);
			return 0;
		}
		else	
		{
			snprintf(buf, 1024, "sudo -S %s ", *argv);
			
			--argc; ++argv;
			while(argc)
			{
				strncat(buf, " ", 1024);
				strncat(buf, *argv, 1024);
				--argc; ++argv;
			}
		}
	}
	else
	{
		printf("Usage: ./empower <program name>\n");
		return 1;
	}
	
	display_window(argc, argv);
	
	return 0;
}

