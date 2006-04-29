#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <Ecore_Data.h>
#include "dumb_tree.h"

typedef struct _Desktop Desktop;
struct _Desktop
{
   Ecore_Hash *data, *group;
   Dumb_Tree *Categories, *OnlyShowIn, *NotShowIn;      /* FIXME: Better to have these as hashes. */
   int NoDisplay, Hidden, allocated;
};

Ecore_Hash *ini_file_cache;
Ecore_Hash *desktop_cache;

/* Function Prototypes */
char *get_t(char *icon);
char *parse_buffer(char *b, char *section);
void parse_desktop_file(char *app, char *menu_path);
void parse_debian_file(char *file);
void process_file(char *file, char *menu_path, G_Eap *eap);
void parse_ini_init(void);
Ecore_Hash *parse_ini_file(char *file);
Desktop *parse_desktop_ini_file(char *file);
void parse_ini_shutdown(void);

#endif
