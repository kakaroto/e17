#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include <math.h>

#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_dvi.h"

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif


#define round(a) ( ((a)<0.0) ? (int)(floor((a) - 0.5)) : (int)(floor((a) + 0.5)) )


/**
 * @addtogroup Ewl_Dvi
 * @{
 */

/**
 * @return Returns a pointer to a new dvi widget on success, NULL on failure.
 * @brief Load an dvi widget with specified dvi contents
 */
Ewl_Widget *
ewl_dvi_new(void)
{
        Ewl_Dvi *dvi;

        DENTER_FUNCTION(DLEVEL_STABLE);

        dvi = NEW(Ewl_Dvi, 1);
        if (!dvi)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_dvi_init(dvi)) {
                ewl_widget_destroy(EWL_WIDGET(dvi));
                dvi = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(dvi), DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to initialize
 * @return Returns no value.
 * @brief Initialize an dvi widget to default values and callbacks
 *
 * Sets the fields and callbacks of @a dvi to their default
 * values. The property EDVI_PROPERTY_DELAYED_FONT_OPEN is set by
 * default.
 */
int
ewl_dvi_init(Ewl_Dvi *dvi)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dvi, FALSE);

        w = EWL_WIDGET(dvi);

        if (!ewl_widget_init(w))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(w, EWL_DVI_TYPE);
        ewl_widget_inherit(w, EWL_DVI_TYPE);

        /*
         * Append necessary callbacks.
         */
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_dvi_configure_cb,
                            NULL);
        ewl_callback_append(w, EWL_CALLBACK_REVEAL, ewl_dvi_reveal_cb,
                            NULL);
        ewl_callback_append(w, EWL_CALLBACK_OBSCURE, ewl_dvi_obscure_cb,
                            NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, ewl_dvi_destroy_cb,
                            NULL);

        dvi->filename = NULL;

        dvi->dvi_device = edvi_device_new (edvi_dpi_get(), edvi_dpi_get());
        dvi->dvi_property = edvi_property_new();
        edvi_property_property_set (dvi->dvi_property, EDVI_PROPERTY_DELAYED_FONT_OPEN);
        dvi->dvi_document = NULL;
        dvi->dvi_page = NULL;

        dvi->dirty = 1;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to change the displayed dvi
 * @param filename: the path to the new dvi to be displayed by @a dvi
 * @return Returns no value.
 * @brief Change the dvi file displayed by an dvi widget
 *
 * Set the dvi displayed by @a dvi to the one found at the path @a filename. If an
 * edje is used, a minimum size should be specified in the edje or the code.
 */
int
ewl_dvi_file_set(Ewl_Dvi *dvi, const char *filename)
{
        Ewl_Widget *w;
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dvi, FALSE);
        DCHECK_TYPE(dvi, EWL_DVI_TYPE);

        w = EWL_WIDGET(dvi);
        emb = ewl_embed_widget_find(w);

        if (!filename || (filename[0] == '\0'))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        if (dvi->filename != filename) {
                IF_FREE(dvi->filename);
        }

        if (dvi->dvi_page) {
                edvi_page_delete(dvi->dvi_page);
                dvi->dvi_page = NULL;
        }

        if (dvi->dvi_document) {
                edvi_document_delete(dvi->dvi_document);
                dvi->dvi_document = NULL;
        }

        dvi->filename = strdup(filename);

        /*
         * Load the new dvi if widget has been realized
         */

        dvi->dvi_document = edvi_document_new(dvi->filename, dvi->dvi_device, dvi->dvi_property);
        if (!dvi->dvi_document)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        dvi->dvi_page = edvi_page_new(dvi->dvi_document);
        if (!dvi->dvi_page) {
                edvi_document_delete(dvi->dvi_document);
                dvi->dvi_document = NULL;
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        if (REALIZED(w)) {
                ewl_widget_obscure(w);
                ewl_widget_reveal(w);
        }

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param dvi the dvi widget to get the file of
 * @return Returns the currently set file name
 * @brief get the file name this dvi uses
 */
const char *
ewl_dvi_file_get(Ewl_Dvi *dvi)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dvi, NULL);
        DCHECK_TYPE_RET(dvi, EWL_DVI_TYPE, NULL);

        DRETURN_PTR(dvi->filename, DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi to set the page of
 * @param page:  the page number
 * @return Returns no value.
 * @brief Set the page of the document
 *
 * Sets the page of the document @p dvi to @p page
 */
void ewl_dvi_page_set(Ewl_Dvi *dvi, int page)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(dvi);
        DCHECK_TYPE(dvi, EWL_DVI_TYPE);

        if (!dvi->dvi_document ||
            !dvi->dvi_page ||
            (page < 0) ||
            (page >= edvi_document_page_count_get(dvi->dvi_document)) ||
            (page == edvi_page_page_get(dvi->dvi_page)))
                DRETURN(DLEVEL_STABLE);

        dvi->dirty = 1;
        edvi_page_page_set(dvi->dvi_page, page);
        ewl_widget_configure(EWL_WIDGET(dvi));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to get the page of
 * @return Returns the currently set page
 * @brief get the page this dvi uses
 */
int
ewl_dvi_page_get(Ewl_Dvi *dvi)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dvi, 0);
        DCHECK_TYPE_RET(dvi, EWL_DVI_TYPE, 0);

        if (!dvi->dvi_page)
                DRETURN_INT(0, DLEVEL_STABLE);

        DRETURN_INT(edvi_page_page_get(dvi->dvi_page), DLEVEL_STABLE);
}

/**
 * @param dvi the dvi widget to get the size of
 * @param width width of the current page
 * @param height height of the current page
 * @brief get the size of the dvi @p dvi. If @p dvi is NULL,
 * return a width equal to 0 and a height equal to 0
 */
void
ewl_dvi_dvi_size_get(Ewl_Dvi *dvi, int *width, int *height)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(dvi);
        DCHECK_TYPE(dvi, EWL_DVI_TYPE);

        if (!dvi->dvi_page) {
                if (width) *width = 0;
                if (height) *height = 0;
                DRETURN(DLEVEL_STABLE);
        }

        edvi_page_size_get(dvi->dvi_page, width, height);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi to change the orientation
 * @param o: the orientation
 * @return Returns no value.
 * @brief Set an orientation of the document
 *
 * Sets an orientation @p o of the document
 */
void
ewl_dvi_orientation_set(Ewl_Dvi *dvi, Edvi_Page_Orientation o)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(dvi);
        DCHECK_TYPE(dvi, EWL_DVI_TYPE);

        if (!dvi->dvi_page || (edvi_page_orientation_get(dvi->dvi_page) == o))
                DRETURN(DLEVEL_STABLE);

        dvi->dirty = 1;
        edvi_page_orientation_set(dvi->dvi_page, o);
        ewl_widget_configure(EWL_WIDGET(dvi));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to get the orientation of
 * @return Returns the orientation of the document.
 * @brief get the orientation of the document @p dvi. If @p dvi
 * is NULL, return EDVI_PAGE_ORIENTATION_PORTRAIT
 */
Edvi_Page_Orientation
ewl_dvi_orientation_get(Ewl_Dvi *dvi)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dvi, EDVI_PAGE_ORIENTATION_PORTRAIT);
        DCHECK_TYPE_RET(dvi, EWL_DVI_TYPE, EDVI_PAGE_ORIENTATION_PORTRAIT);

        if (!dvi->dvi_page)
                DRETURN_INT(EDVI_PAGE_ORIENTATION_PORTRAIT, DLEVEL_STABLE);

        DRETURN_INT(edvi_page_orientation_get(dvi->dvi_page), DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi to change the magnification
 * @param mag: the magnification
 * @return Returns no value.
 * @brief Set the magnification of the document
 *
 * Sets the magnification @p mag of the document @p dvi
 */
void
ewl_dvi_mag_set(Ewl_Dvi *dvi, double mag)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(dvi);
        DCHECK_TYPE(dvi, EWL_DVI_TYPE);

        if (!dvi->dvi_page)
                DRETURN(DLEVEL_STABLE);

        if (mag == edvi_page_mag_get(dvi->dvi_page))
                DRETURN(DLEVEL_STABLE);

        dvi->dirty = 1;
        edvi_page_mag_set(dvi->dvi_page, mag);
        ewl_widget_configure(EWL_WIDGET(dvi));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to get the orientation of
 * @return Returns the magnification.
 * @brief get the magnification of the document @p dvi. If @p dvi
 * is NULL, their values are 1.0
 */
double
ewl_dvi_mag_get(Ewl_Dvi *dvi)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dvi, 1.0);
        DCHECK_TYPE_RET(dvi, EWL_DVI_TYPE, 1.0);

        if (!dvi->dvi_page)
        {
                DRETURN_INT(1.0, DLEVEL_STABLE);
        }

        DRETURN_INT(edvi_page_mag_get(dvi->dvi_page), DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget
 * @return Returns  no value.
 * @brief go to the next page and render it
 */
void
ewl_dvi_page_next(Ewl_Dvi *dvi)
{
        int page;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(dvi);
        DCHECK_TYPE(dvi, EWL_DVI_TYPE);

        if (!dvi->dvi_page)
                DRETURN(DLEVEL_STABLE);

        page = edvi_page_page_get(dvi->dvi_page);
        if (page < (edvi_document_page_count_get(dvi->dvi_document) - 1))
                page++;
        ewl_dvi_page_set(dvi, page);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget
 * @return Returns  no value.
 * @brief go to the previous page and render it
 */
void
ewl_dvi_page_previous(Ewl_Dvi *dvi)
{
        int page;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(dvi);
        DCHECK_TYPE(dvi, EWL_DVI_TYPE);

        if (!dvi->dvi_page)
                DRETURN(DLEVEL_STABLE);

        page = edvi_page_page_get(dvi->dvi_page);
        if (page > 0)
                page--;
        ewl_dvi_page_set(dvi, page);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to get the document of
 * @return Returns the document of the dvi (NULL on failure)
 * @brief get the document of the dvi
 */
const Edvi_Document *ewl_dvi_dvi_document_get (Ewl_Dvi *dvi)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dvi, NULL);
        DCHECK_TYPE_RET(dvi, EWL_DVI_TYPE, NULL);

        DRETURN_PTR(dvi->dvi_document, DLEVEL_STABLE);
}

/**
 * @param dvi: the dvi widget to get the current page of
 * @return Returns the current page of the dvi (NULL on failure)
 * @brief get the current page of the dvi
 */
const Edvi_Page *ewl_dvi_dvi_page_get (Ewl_Dvi *dvi)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dvi, NULL);
        DCHECK_TYPE_RET(dvi, EWL_DVI_TYPE, NULL);

        DRETURN_PTR(dvi->dvi_page, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The reveal callback
 */
void
ewl_dvi_reveal_cb(Ewl_Widget *w,
                  void       *ev_data __UNUSED__,
                  void       *user_data __UNUSED__)
{
        Ewl_Dvi   *dvi;
        Ewl_Embed *emb;
        double     mag;
        int        ow;
        int        oh;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        dvi = EWL_DVI(w);
        emb = ewl_embed_widget_find(w);
        if (!emb)
                DRETURN(DLEVEL_STABLE);

        if (!dvi->image)
          dvi->image = ewl_embed_object_request(emb, "dvi");
        if (!dvi->image)
          dvi->image = evas_object_image_add(emb->canvas);
        if (!dvi->image)
          DRETURN(DLEVEL_STABLE);

        evas_object_smart_member_add(dvi->image, w->smart_object);
        if (w->fx_clip_box)
                evas_object_stack_below(dvi->image, w->fx_clip_box);

        if (w->fx_clip_box)
                evas_object_clip_set(dvi->image, w->fx_clip_box);

        evas_object_pass_events_set(dvi->image, TRUE);
        evas_object_show(dvi->image);

        edvi_page_size_get(dvi->dvi_page, &ow, &oh);
        mag = edvi_page_mag_get(dvi->dvi_page);
        ow = (int)(ow * mag);
        oh = (int)(oh * mag);

        ewl_object_preferred_inner_w_set(EWL_OBJECT(dvi), ow);
        ewl_object_preferred_inner_h_set(EWL_OBJECT(dvi), oh);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The obscure callback
 */
void
ewl_dvi_obscure_cb(Ewl_Widget *w,
                   void       *ev_data __UNUSED__,
                   void       *user_data __UNUSED__)
{
        Ewl_Dvi *dvi;
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        emb = ewl_embed_widget_find(w);

        dvi = EWL_DVI(w);
        if (emb && dvi->image) {
                ewl_embed_object_cache(emb, dvi->image);
                dvi->image = NULL;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_dvi_configure_cb(Ewl_Widget *w,
                     void       *ev_data __UNUSED__,
                     void       *user_data __UNUSED__)
{
        Ewl_Dvi *dvi;
        Ewl_Embed *emb;
        int        ww;
        int        hh;
        int        ow;
        int        oh;
        int dx = 0, dy = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        dvi = EWL_DVI(w);
        if (!dvi->image || !dvi->dvi_page)
                DRETURN(DLEVEL_STABLE);

        emb = ewl_embed_widget_find(w);

        edvi_page_size_get(dvi->dvi_page, &ow, &oh);

        ww = CURRENT_W(w);
        hh = CURRENT_H(w);
        if (ww > ow)
                ww = ow;
        if (hh > oh)
                hh = oh;

        dx = (CURRENT_W(w) - ww) / 2;
        dy = (CURRENT_H(w) - hh) / 2;

        evas_object_image_size_set(dvi->image, ow, oh);
        evas_object_image_fill_set(dvi->image, 0, 0, ow, oh);

        evas_object_move(dvi->image, CURRENT_X(w), CURRENT_Y(w));
        if (dvi->dirty) {
                unsigned int *m;

                m = (unsigned int *)evas_object_image_data_get (dvi->image, 1);
                if (!m)
                        DRETURN(DLEVEL_STABLE);

                memset(m, (255 << 24) | (255 << 16) | (0 << 8) | 255, ow * oh * 4);
                evas_object_image_data_update_add (dvi->image, 0, 0, ow, oh);
                evas_object_resize(dvi->image, ow, oh);
                edvi_page_render(dvi->dvi_page, dvi->dvi_device, dvi->image);
                dvi->dirty = 0;
        }

        ewl_object_preferred_inner_w_set(EWL_OBJECT(w), ow);
        ewl_object_preferred_inner_h_set(EWL_OBJECT(w), oh);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_dvi_destroy_cb(Ewl_Widget *w,
                   void       *ev_data __UNUSED__,
                   void       *user_data __UNUSED__)
{
        Ewl_Dvi *dvi;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, "widget");

        dvi = EWL_DVI(w);

        if (dvi->dvi_document)
                edvi_document_delete (dvi->dvi_document);
        if (dvi->dvi_page)
                edvi_page_delete (dvi->dvi_page);
        if (dvi->dvi_property)
                edvi_property_delete (dvi->dvi_property);
        if (dvi->dvi_device)
                edvi_device_delete (dvi->dvi_device);
        IF_FREE(dvi->filename);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @}
 */
