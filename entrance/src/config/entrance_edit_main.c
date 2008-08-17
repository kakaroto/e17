#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <Entrance_Edit.h>

void 
usage()
{
	printf("entrance_edit - Commandline config file editor for Entrance daemon\n");
	printf("Usage: %s [OPTION <value>] ...\n\n", "entrance_edit");
	printf("   --help                       Show this help text and exit\n");
	printf("   --attempts ATTEMPTS          Set the number of retry attempts to ATTEMPTS\n");
	printf("   --xserver XSERVER            Manually set the location of the X server executable. Else\n");
	printf("                                    use the default location\n");
	/*TODO: first we must be able to create a fresh config file before we start specifying alternate ones*/
	/*printf("   -c, --config-file CONFIGFILE     Manually specify config file. Else, use the default location\n");*/
	printf("\nNOTE: When called without any argument, entrance_edit will list all values of all parameters\n");

	exit(EXIT_SUCCESS);
}

int 
main(int argc, char** argv)
{

	struct option d_opt[] = {
		{"help", 0, 0, 'h'},
		{"attempts", 1, 0, 'a'},
		{"xserver", 1, 0, 'x'},
		{"xsession", 1, 0, 'X'},
		{"auth", 1, 0, 'A'},
		{"engine", 1, 0, 'e'},
		{"reboot", 1, 0, 'r'},
		{"halt", 1, 0, 'H'},
		{"theme", 1, 0, 't'},
		{"background", 1, 0, 'b'},
		{"pointer", 1, 0, 'p'},
		{"greeting-before", 1, 0, 'g'},
		{"greeting-after", 1, 0, 'T'},
		{"date-format", 1, 0, 'd'},
		{"time-format", 1, 0, 'i'},
		{"autologin-mode", 1, 0, 'm'},
		{"autologin-user", 1, 0, 'u'},
		{"presel-mode", 1, 0, 'M'},
		{"presel-prevuser", 1, 0, 'U'},
		{"user-remember", 1, 0, 'R'},
		{"user-remember-count", 1, 0, 'n'},
		{"user-count", 1, 0, 'N'},
		{"session-count", 1, 0, 's'},
		{"default-session", 1, 0, 'S'},
		{"default-session-title", 1, 0, 'o'},
		{"default-session-icon", 1, 0, 'I'},
		{0, 0, 0, 0}
	};

	/*TODO: This profileration of config_* variables
	 * are crying out to be properly grouped. Either as members
	 * of a struct or a hash-table. The code just looks whack!!
	 *
	 * Also, the options struct array, may be better utitlized
	 * if the config_* variables are filled directly, by specifying
	 * then in the array, instead of strduping on a match.
	 * see: man getopt_long_only for details
	 * */

	int flag_list = 1;

	int config_attempts = -1, config_auth = -1, config_engine = -1, config_reboot = -1,
		config_halt = -1, config_autologin_mode = -1, config_presel_mode = -1,
		config_user_remember = -1, config_user_remember_count = -1,
		config_user_count = -1, config_session_count = -1;

	char* config_file = NULL;
	char* config_xserver = NULL;
	char* config_xsession = NULL;
	char* config_theme = NULL;
	char* config_background = NULL;
	char* config_pointer = NULL;
	char* config_greeting_before = NULL;
	char* config_greeting_after = NULL;
	char* config_date_format = NULL;
	char* config_time_format = NULL;
	char* config_autologin_user = NULL;
	char* config_presel_prevuser = NULL;
	char* config_default_session = NULL;
	char* config_default_session_title = NULL;
	char* config_default_session_icon = NULL;


	int c;
	while((c = getopt_long_only(argc, argv, ":a:A:b:d:e:g:hH:i:I:m:M:n:N:o:p:r:R:s:S:t:T:u:U:x:X:", d_opt, NULL)) != -1) 
	{
		switch(c) 
		{
			case 'a':
				config_attempts = atoi(optarg);
				break;
			case 'A':
				config_auth = atoi(optarg);
				break;
			case 'b':
				config_background = strdup(optarg);
				break;
			case 'd':
				config_date_format = strdup(optarg);
				break;
			case 'e':
				config_engine = atoi(optarg);
				break;
			case 'g':
				config_greeting_before = strdup(optarg);
				break;
			case 'h':
				usage();
				break;
			case 'H':
				config_halt = atoi(optarg);
				break;
			case 'i':
				config_time_format = strdup(optarg);
				break;
			case 'I':
				config_default_session_icon = strdup(optarg);
				break;
			case 'm':
				config_autologin_mode = atoi(optarg);
				break;
			case 'M':
				config_presel_mode = atoi(optarg);
				break;
			case 'n':
				config_user_remember_count = atoi(optarg);
				break;
			case 'N':
				config_user_count = atoi(optarg);
				break;
			case 'o':
				config_default_session_title = strdup(optarg);
				break;
			case 'p':
				config_pointer = strdup(optarg);
				break;
			case 'r':
				config_reboot = atoi(optarg);
				break;
			case 'R':
				config_user_remember = atoi(optarg);
				break;
			case 's':
				config_session_count = atoi(optarg);
				break;
			case 'S':
				config_default_session = strdup(optarg);
				break;
			case 't':
				config_theme = strdup(optarg);
				break;
			case 'T':
				config_greeting_after = strdup(optarg);
				break;
			case 'u':
				config_autologin_user = strdup(optarg);
				break;
			case 'U':
				config_presel_prevuser = strdup(optarg);
				break;
			case 'x':
				config_xserver = strdup(optarg);
				break;
			case 'X':
				config_xsession = strdup(optarg);
				break;
			default:
				usage();
				break;
		}
		flag_list = 0;
	}


	if(!entrance_edit_init(config_file)) 
	{
		exit(EXIT_FAILURE);
	}

	if(flag_list) 
	{
		entrance_edit_list();
	} 
	else 
	{

	   /*TODO: for int config_*'s consider using a flag
	    * to check if it has been modified or not.
	    * checking for >= 0 may break if entrance begins
	    * to use negative values as valid values.
	    * */

	   if(config_attempts >= 0) 
	   {
		   entrance_edit_int_set(ENTRANCE_EDIT_KEY_DAEMON_ATTEMPTS_INT, config_attempts);
	   }

	   if(config_xserver >= 0) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_DAEMON_XSERVER_STR, config_xserver);
	   }

	   if(config_auth >= 0) 
	   { 
		   entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_AUTH_INT, config_auth);
	   }

	   if(config_engine >= 0) 
	   {
		   entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_ENGINE_INT, config_engine);
	   }
	   
	   if(config_reboot >= 0) 
	   {
		   entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_REBOOT_INT, config_reboot);
	   }
	   
	   if(config_halt >= 0) 
	   {
		   entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_HALT_INT, config_halt);
	   }
	   
	   if(config_autologin_mode >= 0) 
	   {
		   entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_MODE_INT, config_autologin_mode);
	   }
	   
	   if(config_presel_mode >= 0) 
	   {
		   entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_PRESEL_MODE_INT, config_presel_mode);
	   }
	   
	   if(config_user_remember >= 0) 
	   {
		   entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_INT, config_user_remember);
	   }
	   
	   if(config_user_remember_count >= 0) 
	   {
		   entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_N_INT, config_user_remember_count);
	   }

	   if(config_user_count >= 0) 
	   {
		   entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_USER_COUNT_INT, config_user_count);
	   }
	   
	   if(config_session_count >= 0) 
	   {
		   entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_SESSION_COUNT_INT, config_session_count);
	   }

	   if(config_xsession) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_XSESSION_STR, config_xsession);
	   }
	   
	   if(config_background) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_BACKGROUND_STR, config_background);
	   }

	   if(config_pointer) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_POINTER_STR, config_pointer);
	   }

	   if(config_greeting_before) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_GREETING_BEFORE_STR, config_greeting_before);
	   }

	   if(config_greeting_after) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_GREETING_AFTER_STR, config_greeting_after);
	   }

	   if(config_date_format) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_DATE_FORMAT_STR, config_date_format);
	   }

	   if(config_time_format) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_TIME_FORMAT_STR, config_time_format);
	   }

	   if(config_autologin_user) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_USER_STR, config_autologin_user);
	   }

	   if(config_presel_prevuser) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_PRESEL_PREVUSER_STR, config_presel_prevuser);
	   }

	   if(config_default_session) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_SESSION_0_SESSION_STR, config_default_session);
	   }

	   if(config_default_session_title) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_SESSION_0_TITLE_STR, config_default_session_title);
	   }

	   if(config_default_session_icon) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_SESSION_0_ICON_STR, config_default_session_icon);
	   }

	   if(config_theme) 
	   {
		   entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_THEME_STR, config_theme);
	   }

	   if(!entrance_edit_save()) 
	   {
	      fprintf(stderr, "entrance_edit: Error writing to config file!\n");
	   }
	}

	entrance_edit_shutdown();
	exit(EXIT_SUCCESS);
}

