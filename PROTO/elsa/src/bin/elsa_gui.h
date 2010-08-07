#ifndef ELSA_GUI_H_
#define ELSA_GUI_H_

int elsa_gui_init();
void elsa_gui_run();
void elsa_gui_shutdown();
char *elsa_gui_user_get();
char *elsa_gui_password_get();
void elsa_gui_auth_error();

#endif /* ELSA_GUI_H_ */
