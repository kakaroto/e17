/**************************************************************************
 * focus_interface.h
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * October 16, 2001
 * Notebook tab, and callback functions for the focus interface in ebindings
 *************************************************************************/
#ifndef __EBINDINGS_GTK_FOCUS_IFACE
#define __EBINDINGS_GTK_FOCUS_IFACE

#include "ebindings.h"
#include <X11/Xlib.h>
#include <gdk/gdkx.h>

#include "eaction_item.h"
#include "util.h"


void add_focus_notebook(GtkWidget *w, GtkWidget *note, int sheet);
/* @param w: widget pointing to the window
 * @param note: widget pointing to the notebook to attach to
 * @param sheet: what tab to insert as
 */

#endif
