/**************************************************************************
 * Name: interface.h
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * Date: October 10, 2001
 * Description: Gtk Gui used is built with these
 *************************************************************************/
#ifndef __EBINDINGS_INTERFACE_H_
#define __EBINDINGS_INTERFACE_H_

#include "ebindings.h"
#include "menu_interface.h"
#include "keybind_interface.h"
#include "action_extra.h"
#include "focus_interface.h"
#include "desktops.h"

GtkWidget *create_main_ebindings_window(void);

#endif
