#ifndef ICONBAR_ICON_H
#define ICONBAR_ICON_H

#include "iconbar.h"

typedef struct _Icon Icon;

struct _Icon
{
  Iconbar *iconbar;
  Evas_Object *image;

  char *file;

  pid_t launch_pid;
  int launch_id;
  void *launch_id_cb;
};

Icon *iconbar_icon_new(Iconbar *ib, char *path);
void iconbar_icon_free(Icon *ic);

void cb_exec(void *data, Evas_Object *o, const char *sig, const char *src);
#endif
