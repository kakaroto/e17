/**************************************************************************
 * action_extra.h
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * October 13, 2001
 * Notebook tab, and callback functions for the focus interface in ebindings
 *************************************************************************/
#ifndef __EBINDINGS_GTK_ACTION_XTRA_
#define __EBINDINGS_GTK_ACTION_XTRA_

#include "ebindings.h"
#include <X11/Xlib.h>
#include <gdk/gdkx.h>

#include "callbacks.h"
#include "eaction_item.h"
#include "util.h"

#define EBITS_SPOT_MAX 23
#define E_ACTION_TYPE_MAX 8
#define FOCUS_ACTION_STR_MAX 19

void add_actions_extra_notebook(GtkWidget * w, GtkWidget * note, int sheet);

/* @param w: widget pointing to the window
 * @param note: widget pointing to the notebook to attach to
 * @param sheet: what tab to insert as
 */

#endif
