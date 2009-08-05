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
#include "Elementary.h"
#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif
#include "../../../config.h"
#define _(x) gettext(x)

#define WIDTH 0
#define HEIGHT 85
Evas_Object *win;
Evas_Object *exec;
Evas_Object *entry;
char cmd[1024];
Ecore_Exe *sudo;
int auth_passed;
int startup;
int failure;

enum {SUDO,PASS,SUDOPROG,HELP} mode;

/* empower_cb prototypes */
void win_show(void *data, Evas *e, Evas_Object *w, void *event);
void key_down_cb(void *data, Evas *e, Evas_Object *w, void *event);
void focus_cb(void *data, Evas_Object *w, void *event);
void unfocus_cb(void *data, Evas_Object *w, void *event);
void destroy_cb(void *data, Evas_Object *w, void *event);
void check_pass_cb(void *data, Evas_Object *w, void *event);
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
int authorize(const char* password);
//int idle(void *data);
//int idle_exit(void *data);
void check_sudo_timeout_job(void *data);
/* empower_helper prototypes end */

#endif
