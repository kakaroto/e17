#include "Empower.h"

void key_down_cb(Ewl_Widget *w, void *event, void *data)
{
  Ewl_Event_Key_Down *ev = event;
  
  if(!ev->base.modifiers)
  {
    if(strcmp(ev->base.keyname, "Escape") == 0)
    {
      ewl_widget_destroy(EWL_WIDGET(win));
      ewl_main_quit();
      exit(0);
    }
    else
    {
      if(!(startup++))
      {
        if(mode != SUDOPROG || failure)
        {
          ewl_widget_focus_send(entry);
          /*Check for single alpha or numeric value*/
          if(ev->base.keyname[1] == '\0' && 
            (isalpha(*(ev->base.keyname)) || 
              isdigit(*(ev->base.keyname))))
            ewl_text_text_set(EWL_TEXT(entry),ev->base.keyname);
          failure = 0;
        }
        else
        {
          ewl_widget_focus_send(exec);
          /*Check for single alpha or numeric value*/
          if(ev->base.keyname[1] == '\0' && 
            (isalpha(*(ev->base.keyname)) || 
              isdigit(*(ev->base.keyname))))
            ewl_text_text_set(EWL_TEXT(exec),ev->base.keyname);
        }
      }
    }
  }
}

void focus_cb(Ewl_Widget *w, void *event, void *data)
{
  const char* text = (char*)data;
  const char obscure[] = {0xe2, 0x97, 0x8f, 0x00};

  if(ewl_text_text_get(EWL_TEXT(w)) && 
    !strcmp(ewl_text_text_get(EWL_TEXT(w)),text))
    ewl_text_clear(EWL_TEXT(w));

  if(ewl_widget_name_get(w) && !strcmp(ewl_widget_name_get(w),"password"))
    ewl_text_obscure_set(EWL_TEXT(w), obscure);
}

void unfocus_cb(Ewl_Widget *w, void *event, void *data)
{
  const char* text = (char*)data;

  if(!ewl_text_text_get(EWL_TEXT(w)))
  {
    ewl_text_text_set(EWL_TEXT(w),text);			
    ewl_text_cursor_position_set(EWL_TEXT(w),0);
    ewl_text_color_apply(EWL_TEXT(w),100,100,100,200,strlen(text));

    if(ewl_widget_name_get(w) && !strcmp(ewl_widget_name_get(w),"password"))
      ewl_text_obscure_set(EWL_TEXT(w), NULL);
  }
}

void destroy_cb(Ewl_Widget *w, void *event, void *data)
{
  ewl_widget_destroy(EWL_WIDGET(win));
  ewl_main_quit();
  
  exit(0);
}

void reveal_cb(Ewl_Widget *w, void *event, void *data)
{
  ewl_window_raise(EWL_WINDOW(win));
}

void check_pass_cb(Ewl_Widget *w, void *event, void *data)
{	
  char *pass;
  
  if(data && !auth_passed) pass = ewl_text_text_get(EWL_TEXT(data));

  if(pass && strlen(pass))
  {
    ewl_widget_disable(win);
    
    if(mode == SUDO || mode == SUDOPROG)
      authorize(pass);
    else
    {
      printf("%s",pass);
      ewl_main_quit();
    }
  }
}

int sudo_done_cb(void *data, int type, void *event)
{
  Ecore_Exe_Event_Del *ev = event;
  int* code = data;
  
  sudo = NULL;
  
  if((ev->exit_code))
  {
    ewl_main_quit();
    exit(0);
  }
  else if(mode == SUDOPROG)
  {
    if(exec && strlen(ewl_text_text_get(EWL_TEXT(exec))))
    {
      ewl_main_quit();
      strncat(cmd, " ", 1024);
      strncat(cmd, ewl_text_text_get(EWL_TEXT(exec)), 1024);
    }
    else
    {
      auth_passed = 1;
      display_window();
    }
  }
  else
    ewl_main_quit();
  
  return 0;
}

int sudo_data_cb(void *data, int type, void *event)
{
  Ecore_Exe_Event_Data *ev = event;
  
  if(ev->size > 1)
    display_window();
  
  return 0;
}

//EXIT SIGNAL HANDLER
int exit_cb(void *data, int type, void *event)
{	
  ewl_main_quit();
  
  exit(0);
}
