#include <Ewl.h>

#include "ewl_stock.h"
#include "ewl_button_stock.h"

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

int ewl_button_with_stock_init(Ewl_Button_Stock * b, char *stock_id)
{
  Ewl_Widget *w;
  char       *label;
  char       *filename;
  int         test = FALSE;
  
  DENTER_FUNCTION(DLEVEL_STABLE);
  DCHECK_PARAM_PTR_RET("b", b, 0);

  label = ewl_stock_get_label (stock_id);
 
  /* MUST INITIALIZE FIRST! Otherwise, you are in for bad things. Also be sure
   * to initialize using the function from the class you inherited from
   * directly */
  if (label)
    {
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
  if (stock_id)
    {
      filename = ewl_stock_get_filename (stock_id);
      if (filename)
	{
	  b->image_object = ewl_image_new(filename, NULL);
	  free (filename);
	}
    }
  
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
