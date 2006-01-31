#ifndef SORT_H
#define SORT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Eet.h>

char *get_name(char *path);
void sort_menu(char *path);
void sort_menus(void);
void sort_favorites(void);
Ecore_List *add_list(Ecore_List *list, char *tmp);

#endif
