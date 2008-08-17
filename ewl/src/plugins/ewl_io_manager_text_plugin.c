/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_text.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_macros.h"

Ewl_Widget *
ewl_io_manager_plugin_uri_read(const char *uri)
{
        Ewl_Widget *ret = NULL;
        FILE *file;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(uri, NULL);

        file = fopen(uri, "r");
        if (file)
        {
                struct stat buf;
                char *str;

                ret = ewl_text_new();

                stat(uri, &buf);
                str = malloc(sizeof(char) * (buf.st_size + 1));
                fread(str, buf.st_size, 1, file);
                str[buf.st_size] = '\0';
                fclose(file);

                ewl_text_text_set(EWL_TEXT(ret), str);
                FREE(str);
        }

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

Ewl_Widget *
ewl_io_manager_plugin_string_read(const char *string)
{
        Ewl_Widget *ret = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ret = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(ret), string);

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

int
ewl_io_manager_plugin_uri_write(Ewl_Widget *data, const char *uri)
{
        FILE *file;
        int ret = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, FALSE);
        DCHECK_PARAM_PTR_RET(uri, FALSE);
        DCHECK_TYPE_RET(data, EWL_TEXT_TYPE, FALSE);

        file = fopen(uri, "w");
        if (file)
        {
                char *txt;

                txt = ewl_text_text_get(EWL_TEXT(data));
                fwrite(txt, sizeof(char), strlen(txt), file);

                FREE(txt);
                fclose(file);

                ret = TRUE;
        }

        DRETURN_INT(ret, DLEVEL_STABLE);
}

int
ewl_io_manager_plugin_string_write(Ewl_Widget *data, const char **string)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, FALSE);
        DCHECK_TYPE_RET(data, EWL_TEXT_TYPE, FALSE);

        *string = ewl_text_text_get(EWL_TEXT(data));

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

