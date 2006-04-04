#ifndef EMPOWER_H
#define EMPOWER_H

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "Ewl.h"
//#include "../config.h"

#define WIDTH 250
#define HEIGHT 100

char buf[1024];
Ewl_Widget *win;
char password[1024];
int xpos, ypos;

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
