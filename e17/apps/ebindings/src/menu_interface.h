/**************************************************************************
 * menu_interface.h
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * October 12, 2001
 * Notebook tab, and callback functions for the menu interface in ebindings
 *************************************************************************/
#ifndef __EBINDINGS_GTK_MENU_IFACE
#define __EBINDINGS_GTK_MENU_IFACE

#include "ebindings.h"
#include "emenu_item.h"
#include "util.h"

#define DEFAULT_ICON_PIXMAP_DIR "/usr/share/pixmaps/"

void add_menu_notebook(GtkWidget *w, GtkWidget *note, int sheet);
/* @param w: widget pointing to the window
 * @param note: widget pointing to the notebook to attach to
 * @param sheet: what tab to insert as
 */

#endif
