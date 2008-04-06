#include "Empower.h"

int authorize(char* password)
{
  Ecore_Exe* sudo_ptr;
  char buf[128];
    
  snprintf(buf, 11, "sudo -v -S");

  if(!sudo)
  {
    ecore_event_handler_add(ECORE_EXE_EVENT_DEL,sudo_done_cb,NULL);   
    ecore_event_handler_add(ECORE_EXE_EVENT_ERROR,sudo_data_cb,NULL);
    sudo = ecore_exe_pipe_run(buf,ECORE_EXE_PIPE_WRITE|ECORE_EXE_PIPE_READ|ECORE_EXE_PIPE_ERROR,NULL);
  }
  
  if(password)
  {
    ecore_exe_send(sudo,password,strlen(password));
    ecore_exe_send(sudo,"\n",1);
  }
  
  return 1;
}

void check_sudo_timeout_job(void *data)
{
  auth_passed = 0;
  authorize(NULL);
}
