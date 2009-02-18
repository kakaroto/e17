#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "config.h"
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Ewl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Ephoto Ephoto;
struct _Ephoto {
	Ewl_Widget *win;
	Ewl_Widget *ewin;
	Ewl_Widget *view_box;
	Ewl_Widget *normal_vbox;
	Ewl_Widget *fbox;
	Ewl_Widget *ftree;
	Ewl_Widget *single_vbox;
	Ewl_Widget *single_image;
	Ecore_List *fsystem;
	Ecore_List *images;
	char *current_directory;
	int thumb_size;
};

void ephoto_set_main_window(Ewl_Widget *w);
void ephoto_set_effects_window(Ewl_Widget *w);
void ephoto_set_view_box(Ewl_Widget *w);
void ephoto_set_normal_vbox(Ewl_Widget *w);
void ephoto_set_fbox(Ewl_Widget *w);
void ephoto_set_ftree(Ewl_Widget *w);
void ephoto_set_single_vbox(Ewl_Widget *w);
void ephoto_set_single_image(Ewl_Widget *w);
void ephoto_set_fsystem(Ecore_List *l);
void ephoto_set_images(Ecore_List *l);
void ephoto_set_current_directory(char *dir);
void ephoto_set_thumb_size(int i);
Ewl_Widget *ephoto_get_main_window(void);
Ewl_Widget *ephoto_get_effects_window(void);
Ewl_Widget *ephoto_get_view_box(void);
Ewl_Widget *ephoto_get_normal_vbox(void);
Ewl_Widget *ephoto_get_fbox(void);
Ewl_Widget *ephoto_get_ftree(void);
Ewl_Widget *ephoto_get_single_vbox(void);
Ewl_Widget *ephoto_get_single_image(void);
Ecore_List *ephoto_get_fsystem(void);
Ecore_List *ephoto_get_images(void);
char *ephoto_get_current_directory(void);
int ephoto_get_thumb_size(void);
void create_main_window(void);

void add_normal_view(Ewl_Widget *c);
void add_single_view(Ewl_Widget *c);
void show_normal_view(Ewl_Widget *c, void *event, void *data);
void show_single_view(Ewl_Widget *c, void *event, void *data);
extern Ephoto *em;

#endif
