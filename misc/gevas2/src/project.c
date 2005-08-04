/*
 * Gtk1.2.x compatibility.
 *
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

#include "project.h"
#include <gtk/gtkmarshal.h>
#include <gtk/gtksignal.h>
#include <stdio.h>

/* I was using this from gtk+1.2.x and then they took it from me. */
/* Thus I have replicated this code here. This might be changed to */
/* gtk2 style when I know more about it. */


void
gtk_marshal_INT__POINTER_POINTER (GClosure     *closure,
                          GValue       *return_value,
                          guint         n_param_values,
                          const GValue *param_values,
                          gpointer      invocation_hint,
                          gpointer      marshal_data)
{
  typedef gint (*GMarshalFunc_INT__POINTER_POINTER)(gpointer     data1,
                                             gpointer     arg_1,
                                             gpointer     arg_2,
                                             gpointer     data2);
  register GMarshalFunc_INT__POINTER_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gint v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_INT__POINTER_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_value_get_pointer (param_values + 1),
                       g_value_get_pointer (param_values + 2),
                       data2);

//  fprintf(stderr,"gtk_marshal_BOOL__PTR_PTR()\n");
  g_value_set_int (return_value, v_return);
}




/*
void
gtk_marshal_BOOL__POINTER_INT_INT_INT (GtkObject * object,
                                       GtkSignalFunc func,
                                       gpointer func_data, GtkArg * args)
{
  GtkSignal_BOOL__POINTER_INT_INT_INT rfunc;
  gboolean *return_val;
  return_val = GTK_RETLOC_BOOL (args[4]);
  rfunc = (GtkSignal_BOOL__POINTER_INT_INT_INT) func;
  *return_val = (*rfunc) (object,
                          GTK_VALUE_POINTER (args[0]),
                          GTK_VALUE_INT (args[1]),
                          GTK_VALUE_INT (args[2]),
                          GTK_VALUE_INT (args[3]), func_data);
}
*/




void
gtk_marshal_BOOL__POINTER_INT_INT_INT (GClosure     *closure,
                          GValue       *return_value,
                          guint         n_param_values,
                          const GValue *param_values,
                          gpointer      invocation_hint,
                          gpointer      marshal_data)
{
  typedef gint (*GMarshalFunc_BOOL__POINTER_INT_INT_INT)(gpointer     data1,
                                                         gpointer arg_1,
                                                         gint     arg_2,
                                                         gint     arg_3,
                                                         gint     arg_4,
                                                         gpointer     data2);
  register GMarshalFunc_BOOL__POINTER_INT_INT_INT callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gint v_return = 0;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 5);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOL__POINTER_INT_INT_INT) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_value_get_pointer (param_values + 1),
                       g_value_get_int (param_values + 2),
                       g_value_get_int (param_values + 3),
                       g_value_get_int (param_values + 4),
                       data2);

//  fprintf(stderr,"gtk_marshal_BOOL__PTR_3INTS()\n");
  g_value_set_boolean (return_value, v_return);
//  g_value_set_int (return_value, v_return);
}





void
gtk_marshal_BOOL__INT_INT_INT (GClosure     *closure,
                          GValue       *return_value,
                          guint         n_param_values,
                          const GValue *param_values,
                          gpointer      invocation_hint,
                          gpointer      marshal_data)
{
  typedef gint (*GMarshalFunc_BOOL__INT_INT_INT)(gpointer     data1,
                                             gint     arg_1,
                                             gint     arg_2,
                                             gint     arg_3,
                                             gpointer     data2);
  register GMarshalFunc_BOOL__INT_INT_INT callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gint v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 4);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOL__INT_INT_INT) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_value_get_int (param_values + 1),
                       g_value_get_int (param_values + 2),
                       g_value_get_int (param_values + 3),
                       data2);

//fprintf(stderr,"gtk_marshal_BOOL__INT_INT_INT()\n");
  g_value_set_int (return_value, v_return);
}
