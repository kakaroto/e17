#ifndef __ENTRANCED_DISPLAY_H
#define __ENTRANCED_DISPLAY_H
/*TODO: also make Entrance_Edit.h be __ENTRANCE_EDIT_H*/

typedef struct _Entranced_Display
{
   Display     *display;
   int         dispnum;         /* FIXME */
   char        *name;           /* the name of the x display */
   char        *xprog;          /* the X execution string */
   int         attempts;
   int         status;
   int         auth_en;         /* Enable XAuth access control */
   pid_t       pid;
   Ecore_List  *auths;
   char        *authfile;
   char        *hostname;
   
   char        *config;         /* Config file for greeter */
   Ecore_Exe   *e_exe;          /* Exe handle for Entrance session */
   Ecore_Exe   *x_exe;          /* Exe handle for X server */

   Entranced_Client client;
} Entranced_Display;


Entranced_Display* Entranced_Display_New(void);
void Entranced_Display_Spawn_X(Entranced_Display *);
pid_t Entranced_Display_Start_Server_Once(Entranced_Display *);
void Entranced_Display_Spawn_Entrance(Entranced_Display *);
int Entranced_Display_X_Restart(Entranced_Display *);
void Entranced_Display_XReady_Set(unsigned char i);

#endif
