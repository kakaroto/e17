#ifndef _EWL_H
#define _EWL_H

#ifdef __cplusplus
extern          "C" {
#endif

#include <Ebits.h>
#include <Ecore.h>
#include <Edb.h>
#include <Evas.h>
#include <Evas_Engine_Software_X11.h>
#include <Etox.h>
#include <Ewd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef PATH_LEN
#define PATH_LEN 4096
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
#include <ewl_window.h>
#include <ewl_text.h>
#include <ewl_entry.h>

#include <ewl_fx.h>

#include <ewl_seeker.h>
#include <ewl_scrollbar.h>
#include <ewl_spacer.h>
#include <ewl_spinner.h>
#include <ewl_list.h>
#include <ewl_image.h>
#include <ewl_menu_base.h>
#include <ewl_imenu.h>
#include <ewl_menu.h>
#include <ewl_check.h>
#include <ewl_checkbutton.h>
#include <ewl_radiobutton.h>
#include <ewl_cursor.h>
#include <ewl_separator.h>
#include <ewl_notebook.h>
#include <ewl_scrollpane.h>
#include <ewl_textarea.h>
#include <ewl_selectionbar.h>
#include <ewl_selectionbook.h>

#include <ewl_tree.h>

#ifdef __cplusplus
}
#endif
#endif
