#ifndef EMPOWER_H
#define EMPOWER_H

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <regex.h>
#include <pwd.h>
#include "Ecore_X.h"
#include "Ewl.h"

#define WIDTH 0
#define HEIGHT 85
Ewl_Widget *win;
Ewl_Widget *exec;
Ewl_Widget *entry;
char cmd[1024];
Ecore_Exe *sudo;
int auth_passed;
int startup;
int failure;

enum{SUDO,PASS,SUDOPROG,HELP} mode;

/* empower_cb prototypes */
void key_down_cb(Ewl_Widget *w, void *event, void *data);
void focus_cb(Ewl_Widget *w, void *event, void *data);
void unfocus_cb(Ewl_Widget *w, void *event, void *data);
void destroy_cb(Ewl_Widget *w, void *event, void *data);
void reveal_cb(Ewl_Widget *w, void *event, void *data);
void check_pass_cb(Ewl_Widget *w, void *event, void *data);
int wait_for_sudo(void *data);
int sudo_done_cb(void *data, int type, void *event);
int sudo_data_cb(void *data, int type, void *event);
int exit_cb(void *data, int type, void *event);
/* empower_cb prototypes end */

/* empower_gui prototypes */
void setup_window();
void display_window();
void hide_window();
/* empower_gui prototypes end */

/* empower_helper prototypes */
int authorize(char* password);
int idle(void *data);
int idle_exit(void *data);
void check_sudo_timeout_job(void *data);
/* empower_helper prototypes end */

#endif
