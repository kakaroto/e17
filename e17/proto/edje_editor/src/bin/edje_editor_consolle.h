/*  Copyright (C) 2006-2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of Edje_editor.
 *  Edje_editor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Edje_editor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Edje_editor.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _EDJE_EDITOR_CONSOLLE_H_
#define _EDJE_EDITOR_CONSOLLE_H_


Evas_Object  *EV_Consolle;     //The lower consolle panel
Evas_List    *stack;           //Stack for the consolle
int           consolle_count;  //Counter for the consolle



Evas_Object* consolle_create(void);
void         consolle_log   (char *text);
void         consolle_clear (void);


void PopulateSourceComboEntry(void);
void PopulateSignalComboEntry(void);



#endif
