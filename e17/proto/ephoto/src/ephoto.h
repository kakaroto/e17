#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#include <Ecore_File.h>
#include <Ewl.h>
#include <limits.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

void destroy_cb(Ewl_Widget *w, void *event, void *data);
void add_album_cb(Ewl_Widget *w, void *event, void *data);
void add_slideshow_cb(Ewl_Widget *w, void *event, void *data);
void album_clicked_cb(Ewl_Widget *w, void *event, void *data);
void slideshow_clicked_cb(Ewl_Widget *w, void *event, void *data);
int populate_album_cb(void *NotUsed, int argc, char **argv, char **ColName);
int populate_slideshow_cb(void *NotUsed, int argc, char **argv, char **ColName);

typedef struct _Main Main;

struct _Main
{
 Ewl_Widget *win;
 Ewl_Widget *vbox;
 Ewl_Widget *hbox;
 Ewl_Widget *groups;
 Ewl_Widget *menubar;
 Ewl_Widget *menu;
 Ewl_Widget *menu_item;
 Ewl_Widget *hpaned;
 Ewl_Widget *hseparator;
 Ewl_Widget *albums;
 Ewl_Widget *albums_border;
 Ewl_Widget *slideshows;
 Ewl_Widget *slideshows_border;
 Ewl_Widget *viewer;
 Ewl_Widget *viewer_border;
 Ewl_Widget *viewer_freebox;
 Ewl_Widget *icon;
};

extern Main *m;
extern const char *current_album;
extern const char *current_slideshow;

#endif
