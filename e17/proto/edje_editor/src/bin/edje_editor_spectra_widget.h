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

#ifndef _EDJE_EDITOR_SPECTRA_WIDGET_H_
#define _EDJE_EDITOR_SPECTRA_WIDGET_H_

/* Spectra Widget */
Etk_Widget* spectra_widget_new             (Evas *evas);
void        spectra_widget_name_set        (Etk_Widget *spectra, const char *name);
const char* spectra_widget_name_get        (Etk_Widget *spectra);
void        spectra_widget_color_add       (Etk_Widget* spectra, int r, int g, int b, int a, int d);
void        spectra_widget_color_clear_all (Etk_Widget* spectra);
int         spectra_widget_colors_num_get  (Etk_Widget* spectra);
void        spectra_widget_color_get       (Etk_Widget *spectra, int color_num, int *r, int *g, int *b, int *a, int *d);


#endif
