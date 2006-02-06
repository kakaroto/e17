#include <Ewl.h>
#include <ewl_macros.h>
#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

typedef struct _test_set test_set;
struct _test_set
{
	char *name;
	Ewl_Callback_Function func;
	char *filename;
	char *tip;
};

void __close_main_window                (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_border_test_window        (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_box_test_window           (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_button_test_window        (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_calendar_test_window	(Ewl_Widget * w, void *ev_data, void *user_data);
void __create_colordialog_test_window   (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_colorpicker_test_window   (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_combo_test_window         (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_datepicker_test_window    (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_dialog_test_window        (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_freebox_test_window	(Ewl_Widget * w, void *ev_data, void *user_data);
void __create_fileselector_test_window  (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_filedialog_test_window    (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_filedialog_multi_test_window (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_floater_test_window       (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_entry_test_window         (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_icon_test_window		(Ewl_Widget * w, void *ev_data, void *user_data);
void __create_iconbox_test_window       (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_image_test_window         (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_imenu_test_window         (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_menu_test_window          (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_menubar_test_window       (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_notebook_test_window      (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_paned_test_window         (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_password_test_window      (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_progressbar_test_window   (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_scrollpane_test_window    (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_seeker_test_window        (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_selectionbar_test_window  (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_selectionbook_test_window (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_spinner_test_window       (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_statusbar_test_window     (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_table_test_window         (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_text_test_window          (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_text_editor_test_window	(Ewl_Widget * w, void *ev_data, void *user_data);
void __create_theme_test_window         (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_tooltip_test_window       (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_tree_test_window          (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_tree2_test_window         (Ewl_Widget * w, void *ev_data, void *user_data); 
