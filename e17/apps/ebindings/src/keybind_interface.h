/**************************************************************************
 * keybind_interface.h
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * October 12, 2001
 * Notebook tab, and callback functions for the keybind interface in
 * ebindings
 *************************************************************************/
#ifndef __EBINDINGS_GTK_KEYBIND_IFACE
#define __EBINDINGS_GTK_KEYBIND_IFACE

#include "ebindings.h"
#include <X11/Xlib.h>
#include <gdk/gdkx.h>
#include "eaction_item.h"
#include "util.h"

#define MOD_STR_MAX 16
#define ACTION_STR_MAX 22

void add_keybinds_notebook(GtkWidget * w, GtkWidget * note, int sheet);

#endif
