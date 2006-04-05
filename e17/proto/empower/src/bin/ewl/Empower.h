#ifndef EMPOWER_H
#define EMPOWER_H

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include "Ewl.h"
//#include "../config.h"

#define WIDTH 200
#define HEIGHT 80

char buf[1024];
Ewl_Widget *win;
char password[1024];
int xpos, ypos;
uid_t userID;		//holds the userID
struct passwd* userinfo;	//holds the user info

/* empower_cb prototypes */
void key_down_cb(Ewl_Widget *w, void *event, void *data);
void destroy_cb(Ewl_Widget *w, void *event, void *data);
void reveal_cb(Ewl_Widget *w, void *event, void *data);
void pipe_to_sudo_cb(Ewl_Widget *w, void *event, void *data);
/* empower_cb prototypes end */

/* empower_gui prototypes */
void display_window();
/* empower_gui prototypes end */

#endif
