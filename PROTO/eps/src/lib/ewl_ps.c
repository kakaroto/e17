#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include <math.h>

#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_ps.h"

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif


#define round(a) ( ((a)<0.0) ? (int)(floor((a) - 0.5)) : (int)(floor((a) + 0.5)) )


/**
 * @addtogroup Ewl_Ps
 * @{
 */

/**
 * @return Returns a pointer to a new ps widget on success, NULL on failure.
 * @brief Create a ps widget
 */
Ewl_Widget *
ewl_ps_new(void)
{
        Ewl_Ps *ps;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ps = NEW(Ewl_Ps, 1);
        if (!ps)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_ps_init(ps)) {
                ewl_widget_destroy(EWL_WIDGET(ps));
                ps = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(ps), DLEVEL_STABLE);
}

/**
 * @param ps: the ps widget to initialize
 * @return Returns no value.
 * @brief Initialize an ps widget to default values and callbacks
 *
 * Sets the fields and callbacks of @a ps to their default
 * values.
 */
int
ewl_ps_init(Ewl_Ps *ps)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(ps, FALSE);

        w = EWL_WIDGET(ps);

        if (!ewl_widget_init(w))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(w, EWL_PS_TYPE);
        ewl_widget_inherit(w, EWL_PS_TYPE);

        /*
         * Append necessary callbacks.
         */
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_ps_configure_cb,
                            NULL);
        ewl_callback_append(w, EWL_CALLBACK_REVEAL, ewl_ps_reveal_cb,
                            NULL);
        ewl_callback_append(w, EWL_CALLBACK_OBSCURE, ewl_ps_obscure_cb,
                            NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, ewl_ps_destroy_cb,
                            NULL);

        ps->filename = NULL;

        ps->ps_document = NULL;
        ps->ps_page = NULL;

        ps->dirty = 1;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param ps: the ps widget to change the displayed ps
 * @param filename: the path to the new postscript file to be displayed by @a ps
 * @return 0 on failure, 1 otherwise.
 * @brief Change the postscript file displayed by a ps widget
 *
 * Set the postscript file displayed by @a ps to the one found at the path
 * @a filename.
 */
int
ewl_ps_file_set(Ewl_Ps *ps, const char *filename)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(ps, FALSE);
        DCHECK_TYPE(ps, EWL_PS_TYPE);

        w = EWL_WIDGET(ps);

        if (ps->filename != filename) {
                IF_FREE(ps->filename);
        }
        if (!filename || (filename[0] == '\0'))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        if (ps->ps_page) {
                eps_page_delete(ps->ps_page);
                ps->ps_page = NULL;
        }

        if (ps->ps_document) {
                eps_document_delete(ps->ps_document);
                ps->ps_document = NULL;
        }

        ps->filename = strdup(filename);

        /*
         * Load the new Postscript document
         */

        ps->ps_document = eps_document_new(filename);
        if (!ps->ps_document)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ps->ps_page = eps_page_new(ps->ps_document);
        if (!ps->ps_page) {
                eps_document_delete(ps->ps_document);
                ps->ps_document = NULL;
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        if (REALIZED(w)) {
                ewl_widget_unrealize(w);
                ewl_widget_reveal(w);
        }

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param ps the ps widget to get the file of
 * @return Returns the currently set file name
 * @brief get the file name this ps uses
 */
const char *
ewl_ps_file_get(Ewl_Ps *ps)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(ps, NULL);
        DCHECK_TYPE_RET(ps, EWL_PS_TYPE, NULL);

        DRETURN_PTR(ps->filename, DLEVEL_STABLE);
}

/**
 * @param ps: the ps to set the page of
 * @param page: the page number
 * @return Returns no value.
 * @brief Set the page of the document
 *
 * Sets the page of the document @p ps to @p page
 */
void
ewl_ps_page_set(Ewl_Ps *ps, int page)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(ps);
        DCHECK_TYPE(ps, EWL_PS_TYPE);

        if (!ps->ps_document ||
            !ps->ps_page ||
            (page < 0) ||
            (page >= eps_document_page_count_get(ps->ps_document)) ||
            (page == eps_page_page_get(ps->ps_page)))
                DLEAVE_FUNCTION(DLEVEL_STABLE);

        ps->dirty = 1;
        eps_page_page_set(ps->ps_page, page);
        ewl_widget_configure(EWL_WIDGET(ps));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ps: the ps widget to get the page of
 * @return Returns the currently set page
 * @brief get the page this ps uses
 */
int
ewl_ps_page_get(Ewl_Ps *ps)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(ps, 0);
        DCHECK_TYPE_RET(ps, EWL_PS_TYPE, 0);

        if (!ps->ps_page)
                DRETURN_INT(0, DLEVEL_STABLE);

        DRETURN_INT(eps_page_page_get(ps->ps_page), DLEVEL_STABLE);
}

/**
 * @param ps: the ps widget to get the size of
 * @param width width of the current page
 * @param height height of the current page
 * @brief get the size of the ps @p ps. If @p ps is NULL,
 * return a width equal to 0 and a height equal to 0
 */
void
ewl_ps_size_get(Ewl_Ps *ps, int *width, int *height)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(ps);
        DCHECK_TYPE(ps, EWL_PS_TYPE);

        if (!ps->ps_page) {
                if (width) *width = 0;
                if (height) *height = 0;
                DRETURN(DLEVEL_STABLE);
        }

        eps_page_size_get(ps->ps_page, width, height);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ps: the ps to change the orientation
 * @param o: the orientation
 * @return Returns no value.
 * @brief Set an orientation of the document
 *
 * Sets an orientation @p o of the document
 */
void
ewl_ps_orientation_set(Ewl_Ps *ps, Eps_Page_Orientation o)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(ps);
        DCHECK_TYPE(ps, EWL_PS_TYPE);

        if (!ps->ps_page || (eps_page_orientation_get(ps->ps_page) == o))
                DLEAVE_FUNCTION(DLEVEL_STABLE);

        ps->dirty = 1;
        eps_page_orientation_set(ps->ps_page, o);
        ewl_widget_configure(EWL_WIDGET(ps));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ps: the ps widget to get the orientation of
 * @return Returns the orientation of the document.
 * @brief get the orientation of the document @p ps. If @p ps
 * is NULL, return EPS_PAGE_ORIENTATION_PORTRAIT
 */
Eps_Page_Orientation
ewl_ps_orientation_get(Ewl_Ps *ps)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(ps, EPS_PAGE_ORIENTATION_PORTRAIT);
        DCHECK_TYPE_RET(ps, EWL_PS_TYPE, EPS_PAGE_ORIENTATION_PORTRAIT);

        if (!ps->ps_page)
                DRETURN_INT(EPS_PAGE_ORIENTATION_PORTRAIT, DLEVEL_STABLE);

        DRETURN_INT(eps_page_orientation_get(ps->ps_page), DLEVEL_STABLE);
}

/**
 * @param ps: the ps to change the scale
 * @param hscale: the horizontal scale
 * @param vscale: the vertical scale
 * @return Returns no value.
 * @brief Set the scale of the document
 *
 * Sets the horizontal scale @p hscale ans the vertical scale @p vscale
 * of the document @p ps
 */
void
ewl_ps_scale_set(Ewl_Ps *ps, double hscale, double vscale)
{
        double hs;
        double vs;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(ps);
        DCHECK_TYPE(ps, EWL_PS_TYPE);

        if (!ps->ps_page)
                DRETURN(DLEVEL_STABLE);

        eps_page_scale_get(ps->ps_page, &hs, &vs);
        if ((hs == hscale) && (vs == vscale))
                DRETURN(DLEVEL_STABLE);

        ps->dirty = 1;
        eps_page_scale_set(ps->ps_page, hscale, vscale);
        ewl_widget_configure(EWL_WIDGET(ps));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ps: the ps widget to get the orientation of
 * @param hscale: horizontal scale of the current page
 * @param vscale: vertical scale of the current page
 * @return Returns  no value.
 * @brief get the horizontal scale @p hscale ans the vertical scale
 * @p vscale of the document @p ps. If @p ps is NULL, their values are 1.0
 */
void
ewl_ps_scale_get(Ewl_Ps *ps, double *hscale, double *vscale)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(ps);
        DCHECK_TYPE(ps, EWL_PS_TYPE);

        if (!ps->ps_page) {
                if (hscale) *hscale = 1.0;
                if (vscale) *vscale = 1.0;
                DRETURN(DLEVEL_STABLE);
        }

        eps_page_scale_get(ps->ps_page, hscale, vscale);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ps: the ps widget
 * @return Returns  no value.
 * @brief go to the next page and render it
 */
void
ewl_ps_page_next(Ewl_Ps *ps)
{
        int page;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(ps);
        DCHECK_TYPE(ps, EWL_PS_TYPE);

        if (!ps->ps_page)
                DLEAVE_FUNCTION(DLEVEL_STABLE);

        page = eps_page_page_get(ps->ps_page);
        if (page < (eps_document_page_count_get(ps->ps_document) - 1))
                page++;
        ewl_ps_page_set(ps, page);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ps: the ps widget
 * @return Returns  no value.
 * @brief go to the previous page and render it
 */
void
ewl_ps_page_previous(Ewl_Ps *ps)
{
        int page;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(ps);
        DCHECK_TYPE(ps, EWL_PS_TYPE);

        if (!ps->ps_page)
                DLEAVE_FUNCTION(DLEVEL_STABLE);

        page = eps_page_page_get(ps->ps_page);
        if (page > 0)
                page--;
        ewl_ps_page_set(ps, page);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param ps: the ps widget to get the document of
 * @return Returns the document of the ps (NULL on failure)
 * @brief get the document of the ps
 *
 * Get the document of @p ps. The returned value must not
 * be freed.
 */
const Eps_Document *
ewl_ps_ps_document_get(Ewl_Ps *ps)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(ps, NULL);
        DCHECK_TYPE_RET(ps, EWL_PS_TYPE, NULL);

        DRETURN_PTR(ps->ps_document, DLEVEL_STABLE);
}

/**
 * @param ps: the ps widget to get the current page of
 * @return Returns the current page of the ps (NULL on failure)
 * @brief get the current page of the ps
 *
 * Get the current page of @p ps. The returned value must not
 * be freed.
 */
const Eps_Page *
ewl_ps_ps_page_get(Ewl_Ps *ps)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(ps, NULL);
        DCHECK_TYPE_RET(ps, EWL_PS_TYPE, NULL);

        DRETURN_PTR(ps->ps_page, DLEVEL_STABLE);
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
ewl_ps_reveal_cb(Ewl_Widget *w,
                 void       *ev_data __UNUSED__,
                 void       *user_data __UNUSED__)
{
        Ewl_Ps    *ps;
        Ewl_Embed *emb;
        double     hscale;
        double     vscale;
        int        ow;
        int        oh;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ps = EWL_PS(w);
        emb = ewl_embed_widget_find(w);
        if (!emb)
                DRETURN(DLEVEL_STABLE);

        if (!ps->image)
          ps->image = ewl_embed_object_request(emb, "ps");
        if (!ps->image)
          ps->image = evas_object_image_add(emb->canvas);
        if (!ps->image)
                DRETURN(DLEVEL_STABLE);

        evas_object_smart_member_add(ps->image, w->smart_object);
        if (w->fx_clip_box)
                evas_object_stack_below(ps->image, w->fx_clip_box);

        if (w->fx_clip_box)
                evas_object_clip_set(ps->image, w->fx_clip_box);

        evas_object_pass_events_set(ps->image, TRUE);
        evas_object_show(ps->image);

        eps_page_size_get(ps->ps_page, &ow, &oh);
        eps_page_scale_get(ps->ps_page, &hscale, &vscale);
        ow = (int)(ow * hscale);
        oh = (int)(oh * vscale);

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
 * @brief The obscure callback
 */
void
ewl_ps_obscure_cb(Ewl_Widget *w,
                  void       *ev_data __UNUSED__,
                  void       *user_data __UNUSED__)
{
        Ewl_Ps *ps;
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        emb = ewl_embed_widget_find(w);

        ps = EWL_PS(w);
        if (emb && ps->image) {
                ewl_embed_object_cache(emb, ps->image);
                ps->image = NULL;
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
ewl_ps_configure_cb(Ewl_Widget *w,
                    void       *ev_data __UNUSED__,
                    void       *user_data __UNUSED__)
{
        Ewl_Ps    *ps;
        Ewl_Embed *emb;
        double     hscale;
        double     vscale;
        int        ww;
        int        hh;
        int        ow;
        int        oh;
        int        dx = 0;
        int        dy = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ps = EWL_PS(w);
        if (!ps->image)
                DRETURN(DLEVEL_STABLE);

        emb = ewl_embed_widget_find(w);

        eps_page_size_get(ps->ps_page, &ow, &oh);
        eps_page_scale_get(ps->ps_page, &hscale, &vscale);
        ow = (int)(ow * hscale);
        oh = (int)(oh * vscale);

        ww = CURRENT_W(w);
        hh = CURRENT_H(w);
        if (ww > ow)
                ww = ow;
        if (hh > oh)
                hh = oh;

        dx = (CURRENT_W(w) - ww) / 2;
        dy = (CURRENT_H(w) - hh) / 2;

        evas_object_image_fill_set(ps->image, 0, 0, ow, oh);

        evas_object_move(ps->image, CURRENT_X(w), CURRENT_Y(w));
        evas_object_resize(ps->image, ow, oh);
        if (ps->dirty) {
                eps_page_render(ps->ps_page, ps->image);
                ps->dirty = 0;
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
ewl_ps_destroy_cb(Ewl_Widget *w,
                  void       *ev_data __UNUSED__,
                  void       *user_data __UNUSED__)
{
        Ewl_Ps *ps;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, "widget");

        ps = EWL_PS(w);

        if (ps->ps_document)
                eps_document_delete(ps->ps_document);
        if (ps->ps_page)
                eps_page_delete(ps->ps_page);
        IF_FREE(ps->filename);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @}
 */
