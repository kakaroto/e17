#include <Ewl.h>

#include "ewl_dialog.h"
#include "ewl_button_stock.h"

Ewl_Widget *
ewl_dialog_new (Ewl_Position pos)
{
  Ewl_Dialog *d;
  
  DENTER_FUNCTION(DLEVEL_STABLE);
  
  d = NEW(Ewl_Dialog, 1);
  if (!d)
    return NULL;
  
  ewl_dialog_init(d, pos);

  DRETURN_PTR(EWL_WIDGET(d), DLEVEL_STABLE);
}

int
ewl_dialog_init (Ewl_Dialog *dialog, Ewl_Position pos)
{
  Ewl_Widget *w;
  Ewl_Widget *box;
  Ewl_Widget *spacer;
  
  DENTER_FUNCTION(DLEVEL_STABLE);
  DCHECK_PARAM_PTR_RET("d", dialog, 0);
  
  w = EWL_WIDGET(dialog);

  if (!ewl_window_init(EWL_WINDOW(dialog)))
    DRETURN_INT(FALSE, DLEVEL_STABLE);
  
  ewl_widget_set_appearance(w, "window");

  dialog->position = pos;
  
  switch (pos)
    {
    case EWL_POSITION_LEFT : box = ewl_hbox_new (); break;
    case EWL_POSITION_RIGHT: box = ewl_hbox_new (); break;
    case EWL_POSITION_TOP  : box = ewl_vbox_new (); break;
    default                : box = ewl_vbox_new (); break;
    }

  if (box)
    {
      ewl_container_append_child(EWL_CONTAINER(dialog), box);
      ewl_widget_show(box);
    }

  dialog->vbox = ewl_vbox_new ();
  if (dialog->vbox)
    {
      ewl_container_append_child(EWL_CONTAINER(box), dialog->vbox);
      ewl_box_set_homogeneous (EWL_BOX (dialog->vbox), FALSE);
      switch (pos)
	{
	case EWL_POSITION_LEFT: 
	  {
	    dialog->action_area = ewl_vbox_new (); 
	    dialog->separator = ewl_vseparator_new();
	    break;
	  }
	case EWL_POSITION_RIGHT: 
	  {
	    dialog->action_area = ewl_vbox_new (); 
	    dialog->separator = ewl_vseparator_new();
	    break;
	  }
	case EWL_POSITION_TOP: 
	  {
	    dialog->action_area = ewl_hbox_new (); 
	    dialog->separator = ewl_hseparator_new();
	    break;
	  }
	default: 
	  {
	    dialog->action_area = ewl_hbox_new ();
	    dialog->separator = ewl_hseparator_new();
	    break;
	  }
	}
      ewl_widget_show(dialog->vbox);
    }

  if (dialog->separator)
    {
      switch (pos)
	{
	case EWL_POSITION_LEFT: 
	  {
	    ewl_container_prepend_child (EWL_CONTAINER (box),
					 dialog->separator);
	    break;
	  }
	case EWL_POSITION_TOP:
	  {
	    ewl_container_prepend_child (EWL_CONTAINER (box),
					 dialog->separator);
	    break;
	  }
	case EWL_POSITION_RIGHT:
	  {
	    ewl_container_append_child (EWL_CONTAINER (box),
					dialog->separator);
	    break;
	  }
	default: 
	  {
	    ewl_container_append_child (EWL_CONTAINER (box),
					dialog->separator);
	    break;
	  }
	}
      ewl_object_set_fill_policy (EWL_OBJECT (dialog->separator), 
				  EWL_FLAG_FILL_SHRINK);
      ewl_widget_show(dialog->separator);
    }

  if (dialog->action_area)
    {
      switch (pos)
	{
	case EWL_POSITION_LEFT: 
	  {
	    ewl_container_prepend_child (EWL_CONTAINER (box), 
					dialog->action_area);
	    ewl_object_set_fill_policy (EWL_OBJECT (dialog->action_area), 
					EWL_FLAG_FILL_VFILL);
	    break;
	  }
	case EWL_POSITION_TOP:
	  {
	    ewl_container_prepend_child (EWL_CONTAINER (box), 
					dialog->action_area);
	    ewl_object_set_fill_policy (EWL_OBJECT (dialog->action_area), 
					EWL_FLAG_FILL_HFILL);
	    break;
	  }
	case EWL_POSITION_RIGHT:
	  {
	    ewl_container_append_child (EWL_CONTAINER (box), 
					 dialog->action_area);
	    ewl_object_set_fill_policy (EWL_OBJECT (dialog->action_area), 
					EWL_FLAG_FILL_VFILL);
	    break;
	  }
	default: 
	  {
	    ewl_container_append_child (EWL_CONTAINER (box), 
					dialog->action_area);
	    ewl_object_set_fill_policy (EWL_OBJECT (dialog->action_area), 
					EWL_FLAG_FILL_HFILL);
	    break;
	  }
	}

      ewl_box_set_homogeneous (EWL_BOX (dialog->action_area), FALSE);
      ewl_widget_show (dialog->action_area);
      
      spacer = ewl_spacer_new();
      ewl_container_append_child(EWL_CONTAINER(dialog->action_area),
				 spacer);
      ewl_object_set_fill_policy (EWL_OBJECT (spacer), EWL_FLAG_FILL_FILL);
      ewl_widget_show (spacer);

      ewl_container_set_redirect (EWL_CONTAINER (dialog), 
				  EWL_CONTAINER (dialog->action_area));
    }
  
  DRETURN_INT(TRUE, DLEVEL_STABLE); 
}

void
ewl_dialog_add_widget (Ewl_Dialog *dialog, Ewl_Widget *w)
{
  DENTER_FUNCTION(DLEVEL_STABLE);
  DCHECK_PARAM_PTR("d", dialog);

  if (!dialog)
    return;
  
  switch (dialog->position)
    {
    case EWL_POSITION_LEFT: 
      {
	ewl_container_append_child (EWL_CONTAINER (dialog->vbox), w);
	break;
      }
    case EWL_POSITION_TOP:
      {
	ewl_container_append_child (EWL_CONTAINER (dialog->vbox), w);
	break;
      }
    case EWL_POSITION_RIGHT:
      {
	ewl_container_prepend_child (EWL_CONTAINER (dialog->vbox), w);
	break;
      }
    default: 
      {
	ewl_container_prepend_child (EWL_CONTAINER (dialog->vbox), w);
	break;
      }
    }
  
  DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Widget *
ewl_dialog_set_button (char *button_text,
		       int response_id)
{
  Ewl_Widget *button;
  
  DENTER_FUNCTION(DLEVEL_STABLE);

  button = ewl_button_with_stock_new (button_text);
  ewl_object_set_padding (EWL_OBJECT (button), 0, 3, 3, 3);
  ewl_object_set_fill_policy (EWL_OBJECT (button),
			      EWL_FLAG_FILL_VFILL);

  ewl_widget_set_data (button, "RESPONSE_ID", (void *)response_id);
  
  DRETURN_PTR(button, DLEVEL_STABLE);
}

Ewl_Widget *
ewl_dialog_add_button (Ewl_Dialog *dialog, char *button_text,
		       int response_id)
{
  Ewl_Widget *button;
  
  DENTER_FUNCTION(DLEVEL_STABLE);
  DCHECK_PARAM_PTR_RET("d", dialog, 0);

  if (!dialog)
    return NULL;

  button = ewl_button_with_stock_new (button_text);
  ewl_object_set_padding (EWL_OBJECT (button), 0, 3, 3, 3);
  ewl_container_append_child (EWL_CONTAINER (dialog->action_area),
			      button);
  ewl_object_set_fill_policy (EWL_OBJECT (button),
			      EWL_FLAG_FILL_VFILL);

  ewl_widget_set_data (button, "RESPONSE_ID", (void *)response_id);

  ewl_widget_show (button);
  
  DRETURN_PTR(button, DLEVEL_STABLE);
}

Ewl_Widget *
ewl_dialog_add_button_left (Ewl_Dialog *dialog, char *button_text,
			    int response_id)
{
  Ewl_Widget *button;
  
  DENTER_FUNCTION(DLEVEL_STABLE);
  DCHECK_PARAM_PTR_RET("d", dialog, 0);

  if (!dialog)
    return NULL;

  button = ewl_button_with_stock_new (button_text);
  ewl_object_set_padding (EWL_OBJECT (button), 0, 3, 3, 3);
  ewl_container_prepend_child (EWL_CONTAINER (dialog->action_area),
			       button);
  ewl_object_set_fill_policy (EWL_OBJECT (button),
			      EWL_FLAG_FILL_VFILL);

  ewl_widget_set_data (button, "RESPONSE_ID", (void *)response_id);

  ewl_widget_show (button);
  
  DRETURN_PTR(button, DLEVEL_STABLE);
}

unsigned int
ewl_dialog_get_has_separator (Ewl_Dialog *dialog)
{
  DENTER_FUNCTION(DLEVEL_STABLE);
  DCHECK_PARAM_PTR_RET("d", dialog, 0);

  if (!dialog)
    return FALSE;

  DRETURN_INT(dialog->separator != NULL, DLEVEL_STABLE);
}

void
ewl_dialog_set_has_separator (Ewl_Dialog *dialog,
			      unsigned int has_sep)
{
  Ewl_Widget *child;
  int         n;
  
  DENTER_FUNCTION(DLEVEL_STABLE);
  DCHECK_PARAM_PTR("d", dialog);

  if (!dialog)
    DLEAVE_FUNCTION(DLEVEL_STABLE);

  if (has_sep && (dialog->separator == NULL))
    {
      ewl_container_child_iterate_begin (EWL_CONTAINER (EWL_DIALOG
							(dialog)->vbox));
      n = 0;
      child = ewl_container_next_child (EWL_CONTAINER (EWL_DIALOG
						       (dialog)->vbox));
      while (child)
	{
	  n++;
	  child = ewl_container_next_child (EWL_CONTAINER (EWL_DIALOG
							   (dialog)->vbox));
	}
      dialog->separator = ewl_hseparator_new();
      ewl_container_insert_child(EWL_CONTAINER(dialog->vbox),
				 dialog->separator, n);
      ewl_object_set_fill_policy (EWL_OBJECT (dialog->separator), 
				  EWL_FLAG_FILL_SHRINK);
      ewl_widget_show(dialog->separator);
      
    }
  else if (!has_sep && (dialog->separator != NULL))
    {
      ewl_widget_destroy (dialog->separator);
      dialog->separator = NULL;
    }

  DLEAVE_FUNCTION(DLEVEL_STABLE);
}


