#ifndef ELM_HOME_H
#define ELM_HOME_H

typedef struct _Elm_Home_Win Elm_Home_Win;
typedef struct _Elm_Home_Exec Elm_Home_Exec;

struct _Elm_Home_Win 
{
   Evas_Object *win, *gl;
};
struct _Elm_Home_Exec 
{
   Efreet_Desktop *desktop;
   Ecore_Exe *exe;
   Ecore_Timer *timer;
   pid_t pid;
};

EAPI int elm_home_init(void);
EAPI int elm_home_shutdown(void);
EAPI void elm_home_win_new(Ecore_X_Window zone);

#endif
