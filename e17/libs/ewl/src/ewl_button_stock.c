#include <Ewl.h>

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
  };

/* Return the label of the stock item if it exists */
/* Otherwise, return NULL */
/* The result must be freed when not used anymore */
char *
ewl_stock_get_label (const char *stock_id)
{
  int i, val;
  char *label = NULL;

  for (i=0 ; i<(sizeof(builtin_items)/sizeof(Ewl_Stock_Item)) ; i++)
    {
      val = strcmp (stock_id, builtin_items[i].stock_id);
      if (val == 0)
	{
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
Ewl_Widget *ewl_button_with_stock_new (char *stock_id)
{
  Ewl_Button_Stock *b;

  DENTER_FUNCTION(DLEVEL_STABLE);
  
  b = NEW(Ewl_Button_Stock, 1);
  if (!b)
    return NULL;
  
  ewl_button_with_stock_init(b, stock_id);

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
int ewl_button_with_stock_init(Ewl_Button_Stock * b, char *stock_id)
{
  Ewl_Widget *w;
  char       *label;
  int         test = FALSE;
  
  DENTER_FUNCTION(DLEVEL_STABLE);
  DCHECK_PARAM_PTR_RET("b", b, 0);

  label = ewl_stock_get_label (stock_id);
 
  if (label)
    {
      /* TODO : */
      /* mettre le theme ici ? */
      if (!ewl_button_init(EWL_BUTTON(b), label))
	DRETURN_INT(FALSE, DLEVEL_STABLE);
      free (label);
      test = TRUE;
    }
  else
    {
      if (!ewl_button_init(EWL_BUTTON(b), stock_id))
	DRETURN_INT(FALSE, DLEVEL_STABLE);
    }


  /* Set the homogeneous flag to false, and add some space between image and
   * label */
  ewl_box_set_homogeneous (EWL_BOX (b), FALSE);
  ewl_box_set_spacing(EWL_BOX (b), 6);
  
  w = EWL_WIDGET(b);

  /* Create and setup the image for the button if it's desired */
  if (test)
    {
      b->image_object = ewl_image_new(NULL, NULL);
      ewl_widget_set_appearance (b->image_object, stock_id);
    }
/*       ewl_widget_set_appearance (b->image_object, stock_id); */
  
  if (b->image_object)
    {
      ewl_object_set_fill_policy(EWL_OBJECT(b->image_object),
				 EWL_FLAG_FILL_NONE);
      ewl_object_set_alignment(EWL_OBJECT(b->image_object),
			       EWL_FLAG_ALIGN_LEFT);
      ewl_container_prepend_child(EWL_CONTAINER(b), b->image_object);
      ewl_widget_show(b->image_object);
    }
  
  /* Tweak the default alignment of the label */
  if (EWL_BUTTON(b)->label_object) {
    ewl_object_set_alignment(EWL_OBJECT(EWL_BUTTON(b)->label_object),
			     EWL_FLAG_ALIGN_CENTER);
  }
  
  DRETURN_INT(TRUE, DLEVEL_STABLE); 
}
