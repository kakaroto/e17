/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "ewl_base.h"
#include "ewl_highlight.h"
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_highlight_cb_follow_configure(Ewl_Widget *w, 
					void *ev, void *data);
static void ewl_highlight_cb_follow_destroy(Ewl_Widget *w, 
					void *ev, void *data);
static void ewl_highlight_cb_destroy(Ewl_Widget *w, void *ev, void *data);

/**
 * @return Returns a new Ewl_Widget on success or NULL on failure
 * @brief Creates and returns a new widget on success or NULL on failure.
 */
Ewl_Widget *
ewl_highlight_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Highlight, 1);
	if (!w) 
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_highlight_init(EWL_HIGHLIGHT(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param h: The Ewl_Highlight widget to initiailze
 * @return Returns TRUE on successful initialization, FALSE othrwise
 * @brief Initializes an Ewl_Highlight widget to default values
 */
int
ewl_highlight_init(Ewl_Highlight *h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("h", h, FALSE);

	if (!ewl_floater_init(EWL_FLOATER(h)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(h), EWL_HIGHLIGHT_TYPE);
	ewl_widget_inherit(EWL_WIDGET(h), EWL_HIGHLIGHT_TYPE);
	ewl_widget_internal_set(EWL_WIDGET(h), TRUE);
	ewl_widget_layer_top_set(EWL_WIDGET(h), TRUE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param h: The Ewl_Highlight widget to set the follower on
 * @param w: The widget to highlight
 * @return Returns no value
 * @brief Sets the highlight widget to follow the given @a w widget
 */
void
ewl_highlight_follow_set(Ewl_Highlight *h, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("h", h);
	DCHECK_TYPE("h", h, EWL_HIGHLIGHT_TYPE);

	ewl_floater_follow_set(EWL_FLOATER(h), w);
	if (!w) DRETURN(DLEVEL_STABLE);

	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
				ewl_highlight_cb_follow_destroy, h);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
				ewl_highlight_cb_follow_configure, h);

	/* prepend this so we can use the floater follow set call to cleanup
	 * floater stuff */
	ewl_callback_prepend(EWL_WIDGET(h), EWL_CALLBACK_DESTROY,
				ewl_highlight_cb_destroy, w);


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param h: The highlight widget to work with
 * @return Returns the followed widget or NULL
 * @brief Retrieves the followed widget for the @a h highlight
 */
Ewl_Widget *
ewl_highlight_follow_get(Ewl_Highlight *h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("h", h, NULL);
	DCHECK_TYPE_RET("h", h, EWL_HIGHLIGHT_TYPE, NULL);

	DRETURN_PTR(ewl_floater_follow_get(EWL_FLOATER(h)), DLEVEL_STABLE);
}

static void
ewl_highlight_cb_follow_configure(Ewl_Widget *w, void *ev __UNUSED__, 
							void *data)
{
	int width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_object_current_size_get(EWL_OBJECT(w), &width, &height);
	ewl_object_size_request(EWL_OBJECT(data), width, height);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_highlight_cb_follow_destroy(Ewl_Widget *w __UNUSED__, 
				void *ev __UNUSED__, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	ewl_floater_follow_set(EWL_FLOATER(data), NULL);
	ewl_widget_destroy(EWL_WIDGET(data));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_highlight_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_floater_follow_set(EWL_FLOATER(w), NULL);
	ewl_callback_del(EWL_WIDGET(data), EWL_CALLBACK_CONFIGURE,
					ewl_highlight_cb_follow_configure);
	ewl_callback_del(EWL_WIDGET(data), EWL_CALLBACK_DESTROY,
					ewl_highlight_cb_follow_destroy);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

