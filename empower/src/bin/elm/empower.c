#include "Empower.h"

void parse_options(int argc, char** argv)
{
  regex_t regexp;
  
  if(regcomp(&regexp, "empower-askpass", 0))
  {
    printf(_("Failed to create regular expression\n"));
    return;
  }

  /*Do initial mode guessing based on executable name and number of args*/
  if(!regexec (&regexp, *argv, 0, NULL, 0))
    mode = PASS;
  else if(argc > 1)
    mode = SUDO;
  else
    mode = SUDOPROG;

  --argc; ++argv; //move to the next argument
  
  if(mode == SUDO || mode == SUDOPROG)
    snprintf(cmd, 1024, "sudo");
  
  while(argc--)
  {
    if(!strcmp(*argv,"-h") || !strcmp(*argv, "-help"))	//help mode
    {
      mode = HELP;
      break;
    }
    else if(!strcmp(*argv, "-a") || !strcmp(*argv, "-askpass"))	//ssh-askpass mode
    {
      mode = PASS;
      break;
    }
    else
    {
      if(mode == SUDO || mode == SUDOPROG)
      {
        strncat(cmd, " ", sizeof(cmd) - strlen(cmd) - 1);
        strncat(cmd, *argv, sizeof(cmd) - strlen(cmd) - 1);
      }
    }
    
    argv++;
  }
}

#ifndef ELM_LIB_QUICKLAUNCH
int elm_main(int argc, char** argv)
{
  int i;
  sudo = NULL;

  // initialize gettext
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
  textdomain(PACKAGE);
  
  if(!ecore_init())
  {
    printf(_("Unable to init ecore\n"));
    return 1;
  }

  parse_options(argc,argv);
  
  if(mode == HELP)
  {
    printf(_("-=Usage=-\n"));
    printf(_("    Sudo:  empower [SUDO OPTIONS] [PROGRAM] [PROGRAM OPTIONS] [ELEMENTARY OPTIONS]\n"));
    printf(_("    AskPass:  empower-askpass [ELEMENTARY OPTIONS] or empower -p [ELEMENTARY OPTIONS]\n"));
    return 0;	
  }

  ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT,exit_cb,NULL);

  if(mode == SUDO || mode == SUDOPROG)
    ecore_job_add(check_sudo_timeout_job, NULL);
  else if(mode == PASS)
    display_window();

  elm_run();

  if(mode == SUDO || mode == SUDOPROG)
    ecore_exe_run(cmd,NULL);

  elm_shutdown();

  return 0;
}
#endif
ELM_MAIN()
