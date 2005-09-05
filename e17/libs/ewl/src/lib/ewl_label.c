#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_label_apply(Ewl_Label *la);

/**
 * @param text: The text to set into the label
 * @return Returns a new Ewl_Widget if successful, NULL on failure
 * @brief Creates a new Ewl_Label widget with the @a text text in it
 */
Ewl_Widget *
ewl_label_new(char *text)
{
	Ewl_Label *label;

	DENTER_FUNCTION(DLEVEL_STABLE);

	label = NEW(Ewl_Label, 1);
	if (!label) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_label_init(label, text)) {
		ewl_widget_destroy(EWL_WIDGET(label));
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}
	DRETURN_PTR(EWL_WIDGET(label), DLEVEL_STABLE);
}

/**
 * @param la: The Ewl_Label to initialize
 * @param text: The text to initialize into the widget
 * @return Returns TRUE on success, FALSE on falure
 * @brief Initializes the @a la widget
 */
int
ewl_label_init(Ewl_Label *la, char *text)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("la", la, 0);

	w = EWL_WIDGET(la);
	ewl_widget_init(w, "label");

	ewl_widget_inherit(w, "label");
	ewl_object_fill_policy_set(EWL_OBJECT(la), EWL_FLAG_FILL_FILL);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE, ewl_label_realize_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, ewl_label_destroy_cb, NULL);

	ewl_label_text_set(la, text);

	DRETURN_INT(1, DLEVEL_STABLE);
}

/**
 * @param la: The Ewl_Widget to set the text on
 * @param text: The text to set into the widget
 * @return Returns no value
 * @brief Sets the given @a text into the widget @a la
 */
void
ewl_label_text_set(Ewl_Label *la, char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("la", la);

	IF_FREE(la->text);
	if (text) la->text = strdup(text);
	else la->text = strdup("");

	ewl_label_apply(la);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param la: The Ewl_Label to get the text from
 * @return Returns no value.
 * @brief Gets the current text set into the label
 */
char *
ewl_label_text_get(Ewl_Label *la)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("la", la, NULL);

	DRETURN_PTR(la->text, DLEVEL_STABLE);
}

void
ewl_label_realize_cb(Ewl_Widget *w, void *ev, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_label_apply(EWL_LABEL(w));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_label_destroy_cb(Ewl_Widget *w, void *ev, void *data)
{
        Ewl_Label *label;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	label = EWL_LABEL(w);
	IF_FREE(label->text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_label_apply(Ewl_Label *la)
{
	Ewl_Widget *w;
	Evas_Coord nw, nh;

	w = EWL_WIDGET(la);
	if (!w->theme_object) return;

	/* Should htis check be in here? 
	if (!edje_object_part_exists(w->theme_object, "text"))
	printf(" NO PART\n");
	*/
	edje_object_part_text_set(w->theme_object, "text", la->text);
	edje_object_size_min_calc(w->theme_object, &nw, &nh);

	ewl_object_preferred_inner_size_set(EWL_OBJECT(la), (int)nw, (int)nh);
}


