#ifndef ELSA_GUI_H_
#define ELSA_GUI_H_

int elsa_gui_init(const char *theme);
void elsa_gui_run();
void elsa_gui_shutdown();
char *elsa_gui_user_get();
char *elsa_gui_password_get();
void elsa_gui_auth_error();
void elsa_gui_auth_valid();
char *elsa_gui_login_command_get();
void elsa_gui_xsession_set(Eina_List *xsessions);
void elsa_gui_users_set(Eina_List *users);
void elsa_gui_actions_set(Eina_List *actions);

#endif /* ELSA_GUI_H_ */
