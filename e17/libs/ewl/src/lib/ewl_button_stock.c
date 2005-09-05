#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static const Ewl_Stock_Item builtin_items [] = 
  {
    { EWL_STOCK_OK,          "ok" },
    { EWL_STOCK_APPLY,       "apply" },
    { EWL_STOCK_CANCEL,      "cancel" },
    { EWL_STOCK_OPEN,        "open" },
    { EWL_STOCK_PAUSE,       "pause" },
    { EWL_STOCK_PLAY,        "play" },
    { EWL_STOCK_SAVE,        "save" },
    { EWL_STOCK_STOP,        "stop" },
    { EWL_STOCK_REWIND,      "rewind" },
    { EWL_STOCK_FASTFORWARD, "fast forward" },
    { EWL_STOCK_QUIT,        "quit" },
    { EWL_STOCK_ARROW_UP,    "up" },
    { EWL_STOCK_HOME,        "home" },
  };

/* Return the label of the stock item if it exists */
/* Otherwise, return NULL */
/* The result must be freed when not used anymore */
char *
ewl_stock_label_get (const char *stock_id)
{
	int i, val, num;
	char *label = NULL;

	num = sizeof(builtin_items) / sizeof(Ewl_Stock_Item);
	for (i = 0; i < num ; i++) {
		val = strcmp (stock_id, builtin_items[i].stock_id);
		if (val == 0) {
			label = strdup (builtin_items[i].label);
			break;
		}
	}

	return label;
}


/**
 * @param stock_id: the string to use as a stock If for the button, or
 * just a string for the label.
 * @return Returns NULL on failure, a pointer to a new button on success.
 * @brief Allocate and initialize a new button with eventually a stock
 * icon.
 */
Ewl_Widget *ewl_button_stock_new (char *stock_id)
{
	Ewl_Button_Stock *b;

	DENTER_FUNCTION(DLEVEL_STABLE);
  
	b = NEW(Ewl_Button_Stock, 1);
	if (!b)
		return NULL;
  
	ewl_button_stock_init(b, stock_id);

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param b: the button to initialize.
 * @param stock_id: set the stock Id or the label of the button @b to
 * @a stock_id.
 * @return Returns TRUE on success, FALSE otherwise.
 * @brief Initialize a button to starting values
 *
 * Initializes a button to default values and callbacks.
 */
int ewl_button_stock_init(Ewl_Button_Stock * b, char *stock_id)
{
	Ewl_Widget *w;
	char       *label;
	int         test = FALSE;
  
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("b", b, 0);

	label = ewl_stock_label_get (stock_id);
 
	if (label) {
		/* TODO : */
		/* mettre le theme ici ? */
		if (!ewl_button_init(EWL_BUTTON(b), label))
			DRETURN_INT(FALSE, DLEVEL_STABLE);
	      free (label);
	      test = TRUE;
	}
	else {
		if (!ewl_button_init(EWL_BUTTON(b), stock_id))
			DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	/* Set the homogeneous flag to false, and add some space between image
	 * and label */
	ewl_box_homogeneous_set(EWL_BOX (b), FALSE);
	ewl_box_spacing_set(EWL_BOX (b), 6);
  
	w = EWL_WIDGET(b);

	/* Create and setup the image for the button if it's desired */
	if (test) {
		b->image_object = ewl_image_new(NULL, NULL);
		ewl_widget_appearance_set(b->image_object, stock_id);
	}
/*       ewl_widget_appearance_set(b->image_object, stock_id); */

	if (b->image_object) {
		ewl_object_fill_policy_set(EWL_OBJECT(b->image_object),
					   EWL_FLAG_FILL_NONE);
		ewl_object_alignment_set(EWL_OBJECT(b->image_object),
					 EWL_FLAG_ALIGN_LEFT);
		ewl_container_child_prepend(EWL_CONTAINER(b), b->image_object);
		ewl_widget_show(b->image_object);
	}

	/* Tweak the default alignment of the label */
	if (EWL_BUTTON(b)->label_object) {
		ewl_object_alignment_set(EWL_OBJECT(EWL_BUTTON(b)->label_object),
					 EWL_FLAG_ALIGN_CENTER);
	}
  
	DRETURN_INT(TRUE, DLEVEL_STABLE); 
}

/**
 * @param stock_id: the text of the button or a stock Id.
 * @param response_id: The Id that will be retured when clicking on the button.
 * @return Returns a button, or NULL on failure.
 * @brief Convenient function to create a (stock) button, with a
 * response id. To add this
 * button in the action_area of a dialog, just use
 * ewl_container_child_append(dialog, button) or 
 * ewl_container_child_prepend(dialog, button). Use this way to add
 * buttons, instead of ewl_dialog_button_add() or
 * ewl_dialog_button_left_add().
 */
Ewl_Widget *
ewl_button_stock_with_id_new (char *stock_id,
			      int   response_id)
{
	Ewl_Widget *button;
  
	DENTER_FUNCTION(DLEVEL_STABLE);

	button = ewl_button_stock_new (stock_id);
	ewl_object_padding_set(EWL_OBJECT (button), 0, 3, 3, 3);
	ewl_object_fill_policy_set(EWL_OBJECT (button), EWL_FLAG_FILL_VFILL |
							EWL_FLAG_FILL_SHRINK);

	EWL_BUTTON_STOCK(button)->response_id = response_id;

	ewl_callback_append(button, EWL_CALLBACK_CLICKED,
			    ewl_button_stock_click_cb,
			    &(EWL_BUTTON_STOCK (button)->response_id));
  
	DRETURN_PTR(button, DLEVEL_STABLE);
}

/*
 * Internally used callbacks, override at your own risk.
 */

void
ewl_button_stock_click_cb (Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
	ewl_callback_call_with_event_data (w, EWL_CALLBACK_VALUE_CHANGED, data);
}
