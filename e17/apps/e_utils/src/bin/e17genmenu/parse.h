#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <Ecore_Data.h>
#include "dumb_tree.h"

/* Function Prototypes */
void parse_desktop_file(char *app, char *menu_path);
void parse_debian_file(char *file);

#endif
