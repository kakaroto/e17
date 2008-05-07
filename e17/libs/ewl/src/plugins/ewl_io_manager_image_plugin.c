/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_image.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_macros.h"

#include <Evas.h>

Ewl_Widget *
ewl_io_manager_plugin_uri_read(const char *uri)
{
        Ewl_Widget *ret = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(uri, NULL);

        ret = ewl_image_new();
        ewl_image_file_path_set(EWL_IMAGE(ret), uri);

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

Ewl_Widget *
ewl_io_manager_plugin_string_read(const char *string __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        /* XXX not sure if we want to provide this? ... */

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}

int
ewl_io_manager_plugin_uri_write(Ewl_Widget *data, const char *uri)
{
        Ewl_Image *img;
        int ret = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, FALSE);
        DCHECK_PARAM_PTR_RET(uri, FALSE);
        DCHECK_TYPE_RET(data, EWL_IMAGE_TYPE, FALSE);

        img = EWL_IMAGE(data);
        if (!img->image)
                DRETURN_INT(ret, DLEVEL_STABLE);

        ret = evas_object_image_save(img->image, uri, NULL, NULL);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

int
ewl_io_manager_plugin_string_write(Ewl_Widget *data,
                                const char **string __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, FALSE);
        DCHECK_TYPE_RET(data, EWL_IMAGE_TYPE, FALSE);

        /* XXX not sure if we want to provide this ? ... */

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

