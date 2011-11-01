#include "Empower.h"

Eina_Bool grab_keyboard(void *data)
{
  static int failures = 0;
  Ecore_X_Window ewin;
  Eina_Bool ret = 0;

  // Get window from elm
  ewin = elm_win_xwindow_get(win);

  // Grab keyboard
  ret = ecore_x_keyboard_grab(ewin);

  // If this fails 20 times (2 seconds), exit
  // the program since we can't grab the keyboard
  if(failures++ > 20)
  {
    printf(_("Unable to grab keyboard, exiting...\n"));
    elm_exit();
    elm_shutdown();
    exit(1);
  }

  // If keyboard is grabbed, disable this timer, otherwise
  // continue to try.
  return (ret == 0);
}

void win_show(void *data, Evas *e, Evas_Object *w, void *event)
{
  ecore_timer_add(0.1, grab_keyboard, NULL);
}

void key_down_cb(void *data, Evas *e, Evas_Object *w, void *event)
{
  Evas_Event_Key_Down *ev = event;

  if(strcmp(ev->keyname, "Escape") == 0)
  {
    elm_exit();
    elm_shutdown();
    exit(0);
  }
  #if 0
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
  #endif
}

void destroy_cb(void *data, Evas_Object *w, void *event)
{
  /* Exit the main loop and shutdown */
  elm_exit();
  elm_shutdown();

  exit(0);
}

void check_pass_cb(void *data, Evas_Object *w, void *event)
{
  char *pass = NULL;
  char *prog = NULL;

  if(!auth_passed)
  {
    if(entry)
      pass = elm_entry_markup_to_utf8(elm_entry_entry_get(entry));

    if(exec)
      prog = elm_entry_markup_to_utf8(elm_entry_entry_get(exec));

    if((exec) && (entry) && (!pass || (pass[0] == 0)))
    {
      if(prog && (prog[0] != 0))
        elm_object_focus(entry);
      if(pass) free(pass);
      if(prog) free(prog);
      return;
    }

    if(pass && (pass[0] != 0))
    {
      evas_object_hide(win);

      if(mode == SUDO || mode == SUDOPROG)
      {
        authorize(pass);

        // Clear password from memory and entry
        memset(pass, 0, strlen(pass));
        if(entry)
          elm_entry_entry_set(entry, "");
      }
      else
      {
        printf("%s",pass);
        elm_exit();
      }
    }

    if(pass)
    {
      // Clear password from memory
      memset(pass,0,strlen(pass));
      free(pass);
    }
    pass = NULL;
  }
  else
    authorize(NULL);
}

int sudo_done_cb(void *data, int type, void *event)
{
  Ecore_Exe_Event_Del *ev = event;
  int* code = data;
  char *e = NULL;

  sudo = NULL;

  if((ev->exit_code))
  {
    elm_exit();
    elm_shutdown();
    exit(0);
  }
  else if(mode == SUDOPROG)
  {
    e = elm_entry_markup_to_utf8(elm_entry_entry_get(exec));

    if(exec && e && strlen(e))
    {
      elm_exit();
      strncat(cmd, " ", sizeof(cmd) - strlen(cmd) - 1);
      strncat(cmd, e, sizeof(cmd) - strlen(cmd) - 1);
    }
    else
    {
      auth_passed = 1;
      display_window();
    }
    if(e) free(e);
    e = NULL;
  }
  else
    elm_exit();

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
  // Exit the elementary main loop
  elm_exit();
  elm_shutdown();

  exit(0);
}
