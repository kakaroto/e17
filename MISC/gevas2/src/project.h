/*
 * makes Gtk1.x code work with less pain in gtk2
 *
 * Developed by Ben Martin based on gtkev sample widget by Havoc Pennington from GGAD.
 *
 *
 * Copyright (C) 2000 Ben Martin.
 *
 * Original author: Ben Martin
 *
 * See COPYING for full details of copying & use of this software.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *
 */

#ifdef  GTK_DISABLE_DEPRECATED
#undef  GTK_DISABLE_DEPRECATED
#endif

#define GTK_ENABLE_BROKEN 1


#include <gtk/gtk.h>
#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>



void
gtk_marshal_INT__POINTER_POINTER (GClosure     *closure,
                          GValue       *return_value,
                          guint         n_param_values,
                          const GValue *param_values,
                          gpointer      invocation_hint,
                                  gpointer      marshal_data);


void
gtk_marshal_BOOL__POINTER_INT_INT_INT (GClosure     *closure,
                          GValue       *return_value,
                          guint         n_param_values,
                          const GValue *param_values,
                          gpointer      invocation_hint,
                                       gpointer      marshal_data);


void
gtk_marshal_BOOL__INT_INT_INT (GClosure     *closure,
                          GValue       *return_value,
                          guint         n_param_values,
                          const GValue *param_values,
                          gpointer      invocation_hint,
                               gpointer      marshal_data);


