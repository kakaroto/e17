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
void __create_imenu_test_window         (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_menu_test_window          (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_notebook_test_window      (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_progressbar_test_window   (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_scrollpane_test_window    (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_spinner_test_window       (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_table_test_window         (Ewl_Widget * w, void *ev_data, void *user_data);
void __create_theme_test_window         (Ewl_Widget * w, void *ev_data, void *user_data);
