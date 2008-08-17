#ifndef ICONBAR_ICON_EDITOR_H
#define ICONBAR_ICON_EDITOR_H

#include <Ewl.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "iconbar.h"

typedef struct _Icon_Editor Icon_Editor;

struct _Icon_Editor
{
  Ewl_Widget *win;
  Ewl_Widget *main_vbox, *top_hbox, *right_vbox, *bot_hbox;
  Ewl_Widget *icon_image;
  Ewl_Widget *cancel_but, *ok_but;

  struct {
    Ewl_Widget *hbox;
    Ewl_Widget *label;
    Ewl_Widget *entry;
    char *value;
  } name, exec;

  struct {
    Ewl_Widget *win;
    Ewl_Widget *dialog;
  } filesel;

  char *image_path;
  char *file;
  char *icon_name;
  Icon *icon;

  Edje_Edit_File *edf;
  Edje_Part_Collection *group;
  Edje_Program *exec_prog;
};


int icon_editor_init(int *argc, char **argv);
void icon_editor_shutdown();

void icon_editor_show(void);
void icon_editor_hide(void);

int icon_editor_icon_set(Icon *icon);
int icon_editor_file_set(char *file);
void icon_editor_save();

char *icon_editor_exec_get();
void icon_editor_exec_set(char *exec);

void icon_editor_image_data_get(void **data, int *w, int *h);
void icon_editor_image_file_set(char *file);
void icon_editor_image_data_set(void *data, int w, int h);


void icon_editor_file_save(void);
#endif
