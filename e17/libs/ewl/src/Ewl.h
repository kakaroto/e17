#ifndef _EWL_H
#define _EWL_H

/**
 * @file Ewl.h
 * @brief The file that should be included by any project using EWL.
 * It provides all the necessary headers and includes to work with EWL,
 * it is discouraged to include each header file individually.
 */

/**
 * @mainpage Enlightened Widget Library Documentation
 *
 * @image html e_mini.png
 *
 * @section intro Introduction
 *
 * The Enlightenment Widget Library (EWL) is a widget toolkit based on the
 * libraries provided for Enlightenment 17 development. Rendering is performed
 * using Evas, a fast abstracted canvas library that supports multiple
 * backends. The appearances of the widgets are described by Edje files,
 * which are essentially files containing a collection of images and
 * descriptions for laying out those images. The goal of EWL is to abstract
 * the use of these backends and to present an easy to use object model to the
 * end programmer.
 *
 * Overall, EWL is similar in design and functionality to other common
 * toolkits such as GTK+ and QT. The API's differ, but the overall concepts
 * and ideas are similar. If you are familiar with these other toolkits,
 * getting into EWL should be relatively simple.
 *
 * EWL uses the concept of inheritance for describing it's widgets. When a
 * class inherits from another class, the functions that operated on the base
 * class can also operate on the inheriting class. For example, in EWL the
 * class Ewl_Button inherits from Ewl_Box, which inherits from Ewl_Container.
 * This means you can add widgets to the button, just like you could to the
 * box or any other container by using ewl_container_append.
 *
 * @image html e_mini.png
 *
 * @section model The Object Model
 *
 * The basis for all widgets in EWL is the Ewl_Object. Ewl_Objects are never
 * allocated outside of another widget, it provides size and position
 * information for the widget as well as info about it's padding and insets.
 * There are also fields for indicating object alignment and fill policies.
 * Eventually, it will also contain a type field, which will essentially be a
 * colon separated listing of all the inheriting classes for a particular
 * widget.
 *
 * The next step above the Ewl_Object is the Ewl_Widget. Again, these are
 * never allocated by themselves, but are part of all the other widgets
 * available in EWL. The Ewl_Widget class provides necessary information about
 * a widget that relates to it's appearance, it's parent container, event
 * handling, as well as a few miscellaneous tasks common to all widgets.
 *
 * A necessary class that some widgets inherit from is Ewl_Container. This is
 * used for holding collections of widgets and laying them out. Containers are
 * the building blocks of the widget set, they allow for creating heirarchies
 * of widgets that are bounded within their parent containers. The class
 * inherits from Ewl_Widget, so that any container can also be treated as a
 * widget, and thus you can put containers within other containers. Examples
 * of inheriting classes are Ewl_Window and Ewl_Box. In the case of the
 * Ewl_Window, widgets inside the window are given any position they request
 * within the insets of the window. For the Ewl_Box, contained widgets are
 * layed out either from top to bottom, or from left to right, depending on
 * the box orientation.
 *
 * @image html e_mini.png
 *
 * @section callbacks Callbacks
 *
 * To do work in a GUI, it is necessary to know when certain actions occur.
 * EWL handles notification of actions using a common technique called
 * callbacks. When the end programmer wants to know when a specific event
 * occurs to a widget, they can add a callback to it using ewl_callback_append
 * or one of the similar functions. One of the arguments to these functions is
 * a pointer to a function. This function will get called when EWL receives
 * the specified event on that widget. You can attach callbacks to any widget,
 * and with containers you can even mark them to intercept events to their
 * child widgets.
 *
 * One feature of EWL that is different from other toolkits is that it makes
 * extensive use of internal callbacks. In fact, almost all appearance changes
 * for widgets are actually callbacks, and most of the ewl_widget calls
 * actually do very little work, but trigger specific callbacks. This feature
 * allows for overriding specific actions on a widget, or for ordering user
 * specified callbacks relative to internal ones.
 * Example Application Walk-through
 *
 * One of the easiest applications to build for EWL is a simple image viewer.
 * The basic image viewer needs a window, and an image widget. The following
 * app is a fully functional simple image viewer based on code written by Ben
 * Rockwood of Cuddletech. The first part necessary for creating an EWL
 * application is to include the necessary header Ewl.h. Following the include
 * statements are global variable declarations.
 *
 * @code
 * #include <Ewl.h>
 *
 * Ewl_Widget *main_win;
 * Ewl_Widget *main_box;
 * Ewl_Widget *images;
 * @endcode
 *
 * Now declarations of function callbacks are
 * made, normally when writing an application
 * these are added after the GUI code is written.
 * The next piece of code is common to most apps,
 * the windows in EWL are not closed unless they
 * are destroyed, so a callback must be attached
 * for the windows delete callback.
 *
 * @code
 * void
 * __destroy_main_window(Ewl_Widget *main_win, void *ev_data, void *user_data)
 * {
 * 	ewl_widget_destroy(main_win);
 * 	ewl_main_quit();
 *
 * 	return;
 * }
 * @endcode
 *
 * For this simple application, that is the only necessary callback, now we
 * have the main function.  This is where EWL is initialized,
 * widgets are created, and the main EWL loop is started.  First,
 * declare the main function and check to be sure that an image
 * file was specified, then initialize the EWL library.
 *
 * @code
 * int main (int argc, char **argv)
 * {
 * 	if (argc < 2) {
 *		fprintf(stderr, "Usage: %s <image>\n", argv[0]);
 *		return 1;
 * 	}
 *	ewl_init(&argc, argv);
 * @endcode
 *
 * Next allocate the window, set it's title and attach a callback to catch it's
 * delete event.  Also, set a minimum size on the window, also mark it to be
 * auto-sized, and visible.  Marking it auto-sized will cause the widget to
 * resize to fit the contents.
 *
 * @code
 * 	main_win = ewl_window_new();
 * 	ewl_window_set_title(EWL_WINDOW(main_win), "EWL Simple Image Viewer");
 * 	ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW,
 *			    __destroy_main_window, NULL);
 * 	ewl_object_set_minimum_size(EWL_OBJECT(main_win), 100, 100);
 * 	ewl_widget_show(main_win);
 * @endcode
 *
 * Now we create a box for holding the image, this is not really necessary for
 * this app, but it demonstrates further how to use containers, and makes it
 * easier to add more widgets later.
 *
 * @code
 * 	main_box = ewl_vbox_new();
 * 	ewl_container_append_child(EWL_CONTAINER(main_win), main_box);
 * 	ewl_widget_show(main_box);
 * @endcode
 *
 * Next, create the image widget, we just attempt to load the image
 * file that was specified on the command line, and add it to the
 * box in the window.
 *
 * @code
 * 	image = ewl_image_new(argv[1]);
 * 	ewl_container_append_child(EWL_CONTAINER(main_box), image);
 * 	ewl_widget_show(image);
 * @endcode
 *
 * Finally, we call the main function that starts the EWL event
 * processing loop, and with that our app is complete.
 *
 * @code
 * 	ewl_main();
 *
 * 	return 0;
 * }
 * @endcode
 *
 * Now that the application source has been written, it must be
 * compiled. This is fairly simple with EWL, if you name the app
 * simple_viewer.c just use the command:
 *
 * gcc -o simple_viewer `ewl-config --cflags --libs` simple_viewer.c
 *
 * @image html e_mini.png
 *
 * @section conclusion Conclusion
 *
 * Obviously, creating a simple image viewer does not take much
 * effort at all, but it is a good basis for understanding the
 * basics of EWL.  Hopefully, readers will extend this app, and
 * possibly create more robust image viewers, and with any luck,
 * other applications to demonstrate EWL's capabilities.
 *
 * If you have questions, corrections, or improvements, please send
 * them to <a href="mailto: rbdpngn@users.sourceforge.net">RbdPngn</a>. 
 */

/**
 * @page images Documentation Images
 *
 * Miniature E logo.
 *
 * @image html e_mini.png
 *
 * Diagram describing Ewl_Object fields
 *
 * @image html object_fields.png
 *
 * Diagram describing how Ewl_Object fields affect sizing
 *
 * @image html object_sizing.png
 *
 * A background hilite used on these pages.
 *
 * @image html hilite.png
 */

#ifdef __cplusplus
extern"C" {
#endif

#include <Evas.h>
#include <Edje.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Edb.h>
#include <Etox.h>
#include <Ewd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include <ewl_debug.h>
#include <ewl_macros.h>

#include <ewl_enums.h>

#include <ewl_object.h>
#include <ewl_widget.h>
#include <ewl_container.h>

#include <ewl_callback.h>
#include <ewl_events.h>

#include <ewl_misc.h>

#include <ewl_box.h>

#include <ewl_cell.h>
#include <ewl_row.h>

#include <ewl_grid.h>
#include <ewl_table.h>

#include <ewl_config.h>
#include <ewl_theme.h>

#include <ewl_button.h>
#include <ewl_fileselector.h>
#include <ewl_floater.h>
#include <ewl_filedialog.h>
#include <ewl_overlay.h>
#include <ewl_embed.h>
#include <ewl_window.h>
#include <ewl_text.h>
#include <ewl_entry.h>

#include <ewl_password.h>
#include <ewl_seeker.h>
#include <ewl_scrollbar.h>
#include <ewl_spacer.h>
#include <ewl_spinner.h>
#include <ewl_image.h>
#include <ewl_spectrum.h>
#include <ewl_menu_base.h>
#include <ewl_imenu.h>
#include <ewl_menu.h>
#include <ewl_check.h>
#include <ewl_checkbutton.h>
#include <ewl_radiobutton.h>
#include <ewl_cursor.h>
#include <ewl_separator.h>
#include <ewl_notebook.h>
#include <ewl_progressbar.h>
#include <ewl_scrollpane.h>
#include <ewl_textarea.h>
#include <ewl_selectionbar.h>
#include <ewl_selectionbook.h>
#include <ewl_tooltip.h>

#include <ewl_tree.h>

#ifdef __cplusplus
}
#endif
#endif
