#ifndef ORDER_H
#define ORDER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int search_list(Ecore_List *list, char *search);
void create_order(char *path);
void modify_order (char *path, char *entry);

#endif
