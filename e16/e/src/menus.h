/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison,
 *                         and various contributors
 * Copyright (C) 2004-2005 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _MENUS_H_
#define _MENUS_H_

typedef struct _menu Menu;
typedef struct _menuitem MenuItem;
typedef struct _menustyle MenuStyle;

/* menus.c */
int                 MenuStyleConfigLoad(FILE * fs);

Menu               *MenuCreate(const char *name, const char *title,
			       Menu * parent, MenuStyle * ms);
void                MenuDestroy(Menu * m);
void                MenuHide(Menu * m);
void                MenuEmpty(Menu * m);
void                MenuRepack(Menu * m);
MenuItem           *MenuItemCreate(const char *text, ImageClass * ic,
				   const char *action_params, Menu * child);
void                MenuSetName(Menu * m, const char *name);
void                MenuSetStyle(Menu * m, MenuStyle * ms);
void                MenuSetTitle(Menu * m, const char *title);
void                MenuSetData(Menu * m, char *data);
void                MenuSetTimestamp(Menu * m, time_t t);
const char         *MenuGetName(const Menu * m);
const char         *MenuGetData(const Menu * m);
time_t              MenuGetTimestamp(const Menu * m);
int                 MenuIsNotEmpty(const Menu * m);
void                MenuAddItem(Menu * m, MenuItem * mi);
void                ShowInternalMenu(Menu ** pm, MenuStyle ** pms,
				     const char *style,
				     Menu * (mcf) (const char *name,
						   MenuStyle * ms));

int                 MenusActive(void);

/* menus-misc.c */
Menu               *MenuCreateFromDirectory(const char *name, Menu * parent,
					    MenuStyle * ms, const char *dir);
Menu               *MenuCreateFromFlatFile(const char *name, Menu * parent,
					   MenuStyle * ms, const char *file);
Menu               *MenuCreateFromGnome(const char *name, Menu * parent,
					MenuStyle * ms, const char *dir);
Menu               *MenuCreateFromAllEWins(const char *name, MenuStyle * ms);
Menu               *MenuCreateFromDesktopEWins(const char *name, MenuStyle * ms,
					       int desk);
Menu               *MenuCreateFromDesktops(const char *name, MenuStyle * ms);
Menu               *MenuCreateFromThemes(const char *name, MenuStyle * ms);
Menu               *MenuCreateFromBorders(const char *name, MenuStyle * ms);

#endif /* _MENUS_H_ */
