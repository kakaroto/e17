/***************************************************************************
                          str.h  -  description
                             -------------------
    begin                : Sun Apr 9 2000
    copyright            : (C) 2000 by Chris Thomas
    email                : x5rings@fsck.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef E_STR_H
#define E_STR_H 1

#include <stdio.h>
#include <string.h>

void                e_string_free_list(char **slist, int num);
char               *e_string_dup(char *str);
int                 e_string_cmp(char *s1, char *s2);
int                 e_string_case_cmp(char *s1, char *s2);
int                 e_string_length(char *str);
void                e_string_clear(char *str);
void                e_string_cat(char *str, char *cat);
void                e_string_cat_n(char *str, char *cat, int start, int len);
char               *e_string_escape(char *str);
char               *e_string_build(char *format, char **rep_list, int rep_num);

#endif
