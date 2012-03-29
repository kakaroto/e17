/* Examine - a tool for memory leak detection on Windows
 *
 * Copyright (C) 2012 Vincent Torri. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef EXAMINE_LIST_H
#define EXAMINE_LIST_H


typedef struct _Exm_List Exm_List;

struct _Exm_List
{
  void *data;
  Exm_List *next;
};

Exm_List *exm_list_append(Exm_List *l, void *data);
void      exm_list_free(Exm_List *l, void (*free_cb)(void *ptr));
void      exm_list_print(Exm_List *l);


#endif /* EXAMINE_LIST_H */
