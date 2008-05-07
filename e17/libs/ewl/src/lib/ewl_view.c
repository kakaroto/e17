/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_view.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns a new Ewl_View object on success or NULL on failure
 * @brief Creates a new Ewl_View object
 */
Ewl_View *
ewl_view_new(void)
{
        Ewl_View *view;

        DENTER_FUNCTION(DLEVEL_STABLE);

        view = NEW(Ewl_View, 1);
        if (!ewl_view_init(view))
        {
                FREE(view);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(view, DLEVEL_STABLE);
}

/**
 * @param src: An existing view to copy as a basis for a new view
 * @return Returns a new Ewl_View object on success or NULL on failure
 * @brief Creates a new Ewl_View object
 */
Ewl_View *
ewl_view_clone(const Ewl_View *src)
{
        Ewl_View *view;

        DENTER_FUNCTION(DLEVEL_STABLE);

        view = NEW(Ewl_View, 1);
        if (!ewl_view_init(view))
        {
                FREE(view);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        memcpy(view, src, sizeof(Ewl_View));
        /* FIXME: Reset reference count once we add that feature */

        DRETURN_PTR(view, DLEVEL_STABLE);
}

/**
 * @param view: The Ewl_View to initialize
 * @return Returns TRUEE on success or FALSE on failure
 * @brief Initializes an Ewl_View object to default values
 */
int
ewl_view_init(Ewl_View *view)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(view, FALSE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param v: The Ewl_View to set the widget fetch callback into
 * @param fetch: The Ewl_View_Widget_Fetch to set into the view
 * @return Returns no value.
 * @brief This will set the given widget fetch callback into the view
 */
void
ewl_view_widget_fetch_set(Ewl_View *v, Ewl_View_Widget_Fetch fetch)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(v);

        v->fetch = fetch;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param v: The Ewl_View to get the constructor from
 * @return Returns the Ewl_View_Constructor set into the view or NULL if
 * none set.
 * @brief Get the constructor set on this view
 */
Ewl_View_Widget_Fetch
ewl_view_widget_fetch_get(const Ewl_View *v)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(v, NULL);

        DRETURN_PTR(v->fetch, DLEVEL_STABLE);
}

/**
 * @param v: The Ewl_View to set the header_fetch callback on
 * @param f: The Ewl_View_Header_Fetch callback
 * @return Returns no value.
 * @brief Sets the header fetch callback into the view
 */
void
ewl_view_header_fetch_set(Ewl_View *v, Ewl_View_Header_Fetch f)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(v);

        v->header_fetch = f;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param v: The Ewl_View to get the Ewl_View_Header_Fetch function from
 * @return Returns the Ewl_View_Header_Fetch callback set on the view, or
 * NULL on failure.
 * @brief Gets the header fetch callback from the view
 */
Ewl_View_Header_Fetch
ewl_view_header_fetch_get(const Ewl_View *v)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(v, NULL);

        DRETURN_PTR(v->header_fetch, DLEVEL_STABLE);
}

/**
 * @param v: The Ewl_View to set the expansion_view_fetch callback on
 * @param f: The Ewl_View_Expansion_View_Fetch callback
 * @return Returns no value.
 * @brief Sets the expansion view fetch callback into the view
 */
void
ewl_view_expansion_view_fetch_set(Ewl_View *v, Ewl_View_Expansion_View_Fetch f)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(v);

        v->expansion = f;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param v: The Ewl_View to get the Ewl_View_Expansion_View_Fetch function from
 * @return Returns the Ewl_View_Expansion_View_Fetch callback set on the view,
 * or NULL on failure.
 * @brief Gets the expansion view fetch callback from the view
 */
Ewl_View_Expansion_View_Fetch
ewl_view_expansion_view_fetch_get(const Ewl_View *v)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(v, NULL);

        DRETURN_PTR(v->expansion, DLEVEL_STABLE);
}

