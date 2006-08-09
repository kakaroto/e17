#ifndef FDO_DESKTOPS_H
#define FDO_DESKTOPS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <Ecore_Data.h>
#include "dumb_tree.h"

typedef struct _Fdo_Desktop Fdo_Desktop;
struct _Fdo_Desktop
{
   Ecore_Hash *data, *group;
   Dumb_Tree *Categories, *OnlyShowIn, *NotShowIn;      /* FIXME: Better to have these as hashes. */
   char *name;
   char *generic;
   char *comment;
   char *type;
   char *categories;
   char *exec;
   char *icon;
   char *icon_path;
   char *startup;
   char *window_class;
   int allocated;   /* FIXME: NoDisplay, Hidden */
};

/* Function Prototypes */
void fdo_desktops_init(void);
void fdo_desktops_shutdown(void);
Ecore_Hash *fdo_desktops_parse_ini_file(char *file);
Fdo_Desktop *fdo_desktops_parse_desktop_file(char *file);

#endif
