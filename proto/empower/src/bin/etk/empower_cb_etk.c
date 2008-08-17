#include "Empower.h"

void _em_run_cmd(Empower *em)
{
   const char *passwd = NULL;
   FILE *sudo_pipe;
   
   passwd = etk_entry_text_get(ETK_ENTRY(em->entry));
   if(passwd)
     {
	pid_t pid;
	
	pid = fork();
	if(pid == 0)
	  {
	     snprintf(em->password, 1024, "%s", passwd);
	     sudo_pipe = popen(em->buf, "w");
	     fprintf(sudo_pipe, "%s\n", em->password);
	     pclose(sudo_pipe);
	  }
	else
	  {
	     etk_widget_hide_all(em->dialog);
	     etk_main_iterate();
	     wait(pid);
	  }
     }
   etk_main_quit();
}

void
_em_dialog_size_request_cb(Etk_Object *object, Etk_Size *size_request, void *data)
{
   Empower *em;
   
   em = data;
   if(!_em_centered)
     {
	etk_window_center_on_window(ETK_WINDOW(object), NULL);
	etk_widget_focus(em->entry);
	etk_window_focused_set(ETK_WINDOW(object), ETK_TRUE);
	_em_centered = ETK_TRUE;
     }
}

Etk_Bool
_em_dialog_delete_event_cb(void *data)
{
   etk_main_quit();
   return ETK_TRUE;
}

void
_em_entry_key_down(Etk_Object *object, Etk_Event_Key_Down *ev, void *data)
{
   Empower *em;
   
   em = data;
   if(!strcmp(ev->key, "Return") || !strcmp(ev->key, "KP_Enter"))
     {
	_em_run_cmd(em);
     }
   if(!strcmp(ev->key, "Escape"))
     {
	etk_main_quit();
     }   
}

void
_em_dialog_response_cb(Etk_Object *object, int response_id, void *data)
{
   Empower *em;
   FILE *sudo_pipe;
   const char *passwd = NULL;
   
   em = data;
   switch(response_id)
     {
      case 1:
	/* user pressed ok */
	_em_run_cmd(em);
	break;
	
      case 0:
	/* user pressed cancel */
	etk_main_quit();
	break;
     }
}
