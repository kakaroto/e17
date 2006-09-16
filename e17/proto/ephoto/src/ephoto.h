#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Ewl.h>
#include <fnmatch.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void destroy_cb(Ewl_Widget *w, void *event, void *data);
void populate_browser(Ewl_Widget *w, void *event, void *data);
void populate_albums(Ewl_Widget *w, void *event, void *data);
void populate_images(Ewl_Widget *w, void *event, void *data);
void go_up(Ewl_Widget *w, void *event, void *data);
void go_home(Ewl_Widget *w, void *event, void *data);
void go_favorites(Ewl_Widget *w, void *event, void *data);
void entry_change(Ewl_Widget *w, void *event, void *data);
void create_slideshow_config(Ewl_Widget *w, void *event, void *data);
void start_slideshow(Ewl_Widget *w, void *event, void *data);

typedef struct _Main Main;

struct _Main
{
 Ewl_Widget *win;
 Ewl_Widget *entry;
 Ewl_Widget *browser;
 Ewl_Widget *albums;
 Ewl_Widget *viewer;
 Ewl_Widget *viewer_freebox;
};

extern Main *m;
extern char *current_directory;
extern Ecore_List *current_thumbs;

#endif
