/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_H
#define EWL_H

/**
 * @file Ewl.h
 * @brief The file that should be included by any project using EWL.
 * Provides all the necessary headers and includes to work with EWL.
 * It is discouraged to include each header file individually.
 */

#ifdef __cplusplus
extern"C" {
#endif

/* Include the core functionality wrapped in the proper order */
#include <ewl_base.h>

/* IO and MVC helper functionality, MVC must be here for widgets to provide
 * default views */
#include <ewl_io_manager.h>

#include <ewl_model.h>
#include <ewl_view.h>

/* Base widgets and containers */
#include <ewl_box.h>
#include <ewl_border.h>

#include <ewl_overlay.h>

#include <ewl_cell.h>
#include <ewl_row.h>

#include <ewl_grid.h>
#include <ewl_table.h>

#include <ewl_label.h>
#include <ewl_stock.h>
#include <ewl_button.h>
#include <ewl_floater.h>
#include <ewl_dialog.h>
#include <ewl_icondialog.h>
#include <ewl_message.h>
#include <ewl_popup.h>

#include <ewl_text.h>
#include <ewl_text_trigger.h>
#include <ewl_entry.h>

#include <ewl_colorpicker.h>
#include <ewl_colordialog.h>
#include <ewl_range.h>
#include <ewl_seeker.h>
#include <ewl_scrollbar.h>
#include <ewl_spacer.h>
#include <ewl_spinner.h>
#include <ewl_image.h>
#include <ewl_histogram.h>
#include <ewl_spectrum.h>
#include <ewl_menu_item.h>
#include <ewl_menu.h>
#include <ewl_menubar.h>
#include <ewl_context_menu.h>
#include <ewl_toolbar.h>
#include <ewl_check.h>
#include <ewl_checkbutton.h>
#include <ewl_radiobutton.h>
#include <ewl_separator.h>
#include <ewl_calendar.h>
#include <ewl_datepicker.h>
#include <ewl_icon.h>
#include <ewl_icon_theme.h>
#include <ewl_notebook.h>
#include <ewl_progressbar.h>
#include <ewl_paned.h>
#include <ewl_scrollport.h>
#include <ewl_scrollpane.h>
#include <ewl_statusbar.h>
#include <ewl_shadow.h>

/* MVC definitions and containers */
#include <ewl_mvc.h>

#include <ewl_combo.h>

#include <ewl_tree.h>
#include <ewl_tree_view.h>
#include <ewl_tree_view_plain.h>
#include <ewl_tree_view_scrolled.h>
#include <ewl_list.h>
#include <ewl_list2.h>

#include <ewl_freebox.h>
#include <ewl_freebox_mvc.h>

#include <ewl_filelist.h>
#include <ewl_filelist_model.h>
#include <ewl_filelist_view.h>
#include <ewl_filepicker.h>
#include <ewl_filedialog.h>

#include <ewl_media.h>

#ifdef __cplusplus
}
#endif
#endif
