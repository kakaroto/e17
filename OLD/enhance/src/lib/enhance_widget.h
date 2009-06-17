/* Enhance
 * Copyright (C) 2006-2008 Hisham Mardam-Bey, Samuel Mendes, Ugo Riboni, Simon Treny
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

#ifndef EN_WIDGET_H
#define EN_WIDGET_H

typedef struct _E_Widget E_Widget;
typedef struct _E_Widget_Packing E_Widget_Packing;

struct _E_Widget
{
   Etk_Widget *wid;
   EXML_Node  *node;
   E_Widget_Packing *packing;
};

struct _E_Widget_Packing
{
   /* Box packing */
   int      padding;
   Etk_Bool expand;
   Etk_Bool fill;
   Etk_Box_Group box_group;

   /* Table packing */
   int   left_attach;
   int   right_attach;
   int   top_attach;
   int   bottom_attach;
   int   x_padding;
   int   y_padding;
   char *x_options;
   char *y_options;
   
   /* Frame packing */
   char *type;
   
   /* Paned packing */
   Etk_Bool shrink;
};
    
E_Widget *_e_widget_handle(Enhance *en, EXML_Node *node);
void      _e_widget_parent_add(E_Widget *parent, E_Widget *child);

#endif
