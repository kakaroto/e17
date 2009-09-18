/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _WIDGET_H
#define _WIDGET_H

typedef struct _Widget Widget;
typedef struct _Widget_Private Widget_Private;

struct _Widget
{
	Ekeko_Object object;
	Widget_Private *private;
};

Widget *widget_new(void);
void widget_delete(Widget *w);
Ekeko_Type   *widget_type_get(void);
void    widget_geom_get(Widget *widget, int *x, int *y, int *w, int *h);
void    widget_geom_set(Widget *widget, int x, int y, int w, int h);
char *widget_theme_get(Widget *widget);
void widget_theme_set(Widget *widget, char *theme);
#endif
