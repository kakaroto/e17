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


Entranced_Display* edd_new(void);
void edd_spawn_x(Entranced_Display *);
void edd_spawn_entrance(Entranced_Display *);
int edd_x_restart(Entranced_Display *);
void edd_x_ready_set(unsigned char i);

#endif
