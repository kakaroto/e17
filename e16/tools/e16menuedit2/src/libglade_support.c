/* Copyright (C) 2004 Andreas Volz and various contributors
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
 *
 *  File: treeview.c
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#include <string.h>
#include "libglade_support.h"

static GHashTable *glade_hash;

void register_libglade_parent (GladeXML *glade_xml, const char *parent_name)
{
  static gboolean init = FALSE;

  if (!init)
  {
    glade_hash = g_hash_table_new_full (g_str_hash, g_str_equal,
                                        g_free, NULL);
    init = TRUE;
  }

  g_hash_table_replace (glade_hash,
                        strdup (parent_name),
                        glade_xml);

}

GtkWidget *lookup_libglade_widget (const char *parent_name,
                                   const char *widget_name)
{
  GladeXML *glade_xml;
  GtkWidget *found_widget;
  gpointer lookup;

  lookup = g_hash_table_lookup (glade_hash,
                                parent_name);

  glade_xml = (GladeXML *) lookup;

  found_widget = glade_xml_get_widget (glade_xml, widget_name);

  return found_widget;
}
