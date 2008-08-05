/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_filelist_view.h"
#include "ewl_filelist.h"
#include "ewl_icon_theme.h"
#include "ewl_icon.h"
#include "ewl_label.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static void ewl_filelist_view_cb_dnd_data_request(Ewl_Widget *w,
                        void *event, void *data);

static void
ewl_filelist_view_cb_dnd_data_request(Ewl_Widget *w, void *event,
                        void *data __UNUSED__)
{
        char buf[PATH_MAX + 8], *file, *uri;
        int len = 0;
        Ewl_Embed *emb;
        Ewl_Widget *icon;
        Ewl_Event_Dnd_Data_Request *ev = event;
        Ecore_List *sel;

        icon = w;

        /* Get the filelist to find the name */
        while (!EWL_FILELIST_IS(w))
        {
                if (!w->parent)
                        DRETURN(DLEVEL_STABLE);
                w = w->parent;
        }

        emb = ewl_embed_widget_find(w);
        if (!emb)
        {
                DWARNING("Widget doesn't have an embed parent!");
                DRETURN(DLEVEL_STABLE);
        }

        /* Watch for changes here */
        if ((icon->theme_state) && (!strcmp(icon->theme_state, "parent,selected")))
        {
                /* Our icon is selected, so we can just grab a list
                 * of the selected files from mvc
                 */
                sel = ewl_filelist_selected_files_get(EWL_FILELIST(w));

                /* Our length needs to include the file:// and \n for every
                 * file
                 */
                ecore_list_first_goto(sel);
                while ((file = ecore_list_next(sel)))
                        len += strlen(file) + 8;
                len++;

                uri = alloca(sizeof(char) * len);
                if (!uri)
                        DRETURN(DLEVEL_STABLE);

                uri[0] = '\0';
                while ((file = ecore_list_first_remove(sel)))
                {
                        snprintf(buf, sizeof(buf), "file://%s\n", file);
                        ecore_strlcat(uri, buf, len);
                        FREE(file);
                }
                IF_FREE_LIST(sel);
        }
        else
        {
                /* Just grab the selected one from the icon name */
                file = ewl_filelist_selected_file_get(EWL_FILELIST(w));
                snprintf(buf, sizeof(buf), "file://%s/%s",
                                ewl_filelist_directory_get(EWL_FILELIST(w)),
                                ewl_icon_label_get(EWL_ICON(icon)));

                FREE(file);
                uri = buf;
                len = strlen(buf);
        }

        ewl_engine_embed_dnd_drag_data_send(emb, ev->handle,
                                (void *)uri, len);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Widget *ewl_filelist_view_widget_fetch(void *data,
                        unsigned int row __UNUSED__, unsigned int column)
{
        Ewl_Widget *ret;
        const char *img = NULL, *stock, *filename;
        const char *dnd_types[] = {"text/uri-list", NULL};

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, NULL);

        /* Create icon */
        ret = ewl_icon_simple_new();
        ewl_icon_constrain_set(EWL_ICON(ret), EWL_ICON_SIZE_MEDIUM);
        ewl_dnd_provided_types_set(ret, dnd_types);
        ewl_callback_append(ret, EWL_CALLBACK_DND_DATA_REQUEST,
                       ewl_filelist_view_cb_dnd_data_request, NULL);
        ewl_box_orientation_set(EWL_BOX(ret),
                        EWL_ORIENTATION_HORIZONTAL);
        ewl_object_alignment_set(EWL_OBJECT(ret), EWL_FLAG_ALIGN_LEFT);

        /* Get and set data into icon */
        if (column == 0)
        {
                if (!strcmp(data, ".."))
                        img = ewl_icon_theme_icon_path_get
                                                (EWL_ICON_GO_UP,
                                                 EWL_ICON_SIZE_MEDIUM);
                else
                {
                        stock = ewl_filelist_stock_icon_get(data);
                        img = ewl_icon_theme_icon_path_get(stock,
                                                EWL_ICON_SIZE_MEDIUM);
                }
                if (img) ewl_icon_image_set(EWL_ICON(ret), 
                                                img, NULL);

                filename = ecore_file_file_get(data);
                ewl_icon_label_set(EWL_ICON(ret), filename);
        }
        else
        {
                ewl_icon_label_set(EWL_ICON(ret), data);
        }
        
        FREE(data);
        ewl_widget_show(ret);

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

Ewl_Widget *ewl_filelist_view_header_fetch(void *data __UNUSED__,
                                                unsigned int column)
{
        Ewl_Widget *l;
        char *t;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if (column == 0) t = "Filename";
        else if (column == 1) t = "Size";
        else if (column == 2) t = "Permissions";
        else if (column == 3) t = "Username";
        else if (column == 4) t = "Groupname";
        else if (column == 5) t = "Modified";
        else t = "N/A";

        l = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(l), t);
        ewl_widget_show(l);

        DRETURN_PTR(l, DLEVEL_STABLE);
}
