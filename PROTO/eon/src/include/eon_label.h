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
#ifndef _LABEL_H
#define _LABEL_H

typedef struct _Label Label;
typedef struct _Label_Private Label_Private;

struct _Label
{
	Widget widget;
	Label_Private *private;
};

Label *label_new(void);
Ekeko_Type   *label_type_get(void);
void label_text_set(Label *label, char *text);
char *label_text_get(Label *label);

#endif
