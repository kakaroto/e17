/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_filedialog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_constructor(char *buf, int len);

Ewl_Unit_Test filedialog_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *fd;
        int ret = 0;
        const char *txt;

        fd = ewl_filedialog_new();

        if (!EWL_FILEDIALOG_IS(fd))
        {
                LOG_FAILURE(buf, len, "Returned widget is not of the type"
                                " " EWL_FILEDIALOG_TYPE);
                goto DONE;
        }
        txt = ewl_window_title_get(EWL_WINDOW(fd));
        if (!txt || strcmp(txt, "Ewl Filedialog"))
        {
                LOG_FAILURE(buf, len, "Window title is not \"Ewl Filedialog\"");
                goto DONE;
        }
        txt = ewl_window_name_get(EWL_WINDOW(fd));
        if (!txt || strcmp(txt, "Ewl Filedialog"))
        {
                LOG_FAILURE(buf, len, "Window name is not \"Ewl Filedialog\"");
                goto DONE;
        }
        txt = ewl_window_class_get(EWL_WINDOW(fd));
        if (!txt || strcmp(txt, "Ewl Filedialog"))
        {
                LOG_FAILURE(buf, len, "Window class is not \"Ewl Filedialog\"");
                goto DONE;
        }

        txt = ewl_widget_appearance_get(EWL_WIDGET(fd));
        if (!txt || strcmp(txt, EWL_DIALOG_TYPE))
        {
                LOG_FAILURE(buf, len, "Appearance is not " EWL_DIALOG_TYPE);
                goto DONE;
        }
        if (ewl_filedialog_multiselect_get(EWL_FILEDIALOG(fd)))
        {
                LOG_FAILURE(buf, len, "Default is multiselect");
                goto DONE;
        }
        if (ewl_filedialog_show_dot_files_get(EWL_FILEDIALOG(fd)))
        {
                LOG_FAILURE(buf, len, "Default is to show dot files");
                goto DONE;
        }
        if (ewl_filedialog_show_favorites_get(EWL_FILEDIALOG(fd)))
        {
                LOG_FAILURE(buf, len, "Default is to show the favorites");
                goto DONE;
        }
        if (ewl_filedialog_save_as_get(EWL_FILEDIALOG(fd)))
        {
                LOG_FAILURE(buf, len, "Default is a \'Save As\' dialog");
                goto DONE;
        }
        if (!ewl_filedialog_return_directories_get(EWL_FILEDIALOG(fd)))
        {
                LOG_FAILURE(buf, len, "The \'return directories\' flag is not set");
                goto DONE;
        }

        ret = 1;
DONE:
        ewl_widget_destroy(fd);

        return ret;
}

