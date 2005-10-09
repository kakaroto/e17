#include "ewl_test.h"

static void
__destroy_test_window(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
  ewl_widget_destroy(EWL_WIDGET(w));
}

void
__create_window_response(Ewl_Widget *w __UNUSED__, int *id, 
					void *data __UNUSED__)
{
  Ewl_Widget *win_ok     = NULL;
  Ewl_Widget *win_cancel = NULL;
  Ewl_Widget *label;
  char        str[100];
  int         response;
  
  response = (int)*id;
  switch (response)
    {
    case EWL_RESPONSE_OK:
      {
	if (win_ok != NULL)
	  {
	    ewl_window_raise (EWL_WINDOW (win_ok));
	    return;
	  }
	win_ok = ewl_window_new ();
	ewl_window_title_set (EWL_WINDOW (win_ok), "OK Window");
	ewl_window_name_set (EWL_WINDOW (win_ok), "EWL Test Application");
	ewl_window_class_set (EWL_WINDOW (win_ok), "EFL Test Application");
	ewl_object_insets_set (EWL_OBJECT (win_ok), 5, 5, 5, 5);
	ewl_callback_append (win_ok, EWL_CALLBACK_DELETE_WINDOW,
			     __destroy_test_window, NULL);

	sprintf (str, "You have clicked on the OK Button\nThe response id is %d.\n", response);

	label = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(label), str);
	ewl_container_child_append (EWL_CONTAINER (win_ok), label);
	ewl_widget_show (label);
	
	ewl_widget_show (win_ok);

	break;
      }
    case EWL_RESPONSE_CANCEL:
      {
	if (win_cancel != NULL)
	  {
	    ewl_window_raise (EWL_WINDOW (win_cancel));
	    return;
	  }

	win_cancel = ewl_window_new ();
	ewl_window_title_set (EWL_WINDOW (win_cancel), "CANCEL Window");
	ewl_window_name_set (EWL_WINDOW (win_cancel), "EWL Test Application");
	ewl_window_class_set (EWL_WINDOW (win_cancel), "EFL Test Application");
	ewl_object_insets_set (EWL_OBJECT (win_cancel), 5, 5, 5, 5);
	ewl_callback_append (win_cancel, EWL_CALLBACK_DELETE_WINDOW,
			     __destroy_test_window, NULL);

	sprintf (str, "You have clicked on the CANCEL Button\nThe response id is %d.\n", response);

	label = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(label), str);
	ewl_container_child_append (EWL_CONTAINER (win_cancel), label);
	ewl_widget_show (label);
	
	ewl_widget_show (win_cancel);

	break;
      }
    }
}

static void
__destroy_dialog_test_window (Ewl_Widget *w, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
  ewl_widget_destroy (EWL_WIDGET (w));
}

void
__create_dialog_test_window (Ewl_Widget * w, void *ev __UNUSED__,
					     void *data __UNUSED__)
{
  Ewl_Widget *dialog_win;
  Ewl_Widget *hbox;
  Ewl_Widget *image;
  Ewl_Widget *label;
  Ewl_Widget *button;

  dialog_win = ewl_dialog_new ();
  ewl_window_title_set (EWL_WINDOW (dialog_win), "Dialog Test");
  ewl_window_name_set (EWL_WINDOW (dialog_win), "EWL Test Application");
  ewl_window_class_set (EWL_WINDOW (dialog_win), "EFL Test Application");
  ewl_object_insets_set (EWL_OBJECT (dialog_win), 5, 5, 5, 5);

  if (w) 
	  ewl_callback_append (dialog_win, EWL_CALLBACK_DELETE_WINDOW,
	       EWL_CALLBACK_FUNCTION (__destroy_dialog_test_window), NULL);
  else
  	ewl_callback_append(dialog_win, EWL_CALLBACK_DELETE_WINDOW,
						__close_main_window, NULL);

  hbox = ewl_hbox_new ();
  ewl_dialog_widget_add (EWL_DIALOG (dialog_win), hbox);
  ewl_widget_show (hbox);

  image = ewl_image_new ();
  ewl_image_file_set(EWL_IMAGE(image), "my_image.png", NULL);
  ewl_container_child_append (EWL_CONTAINER (hbox), image);
  ewl_widget_show (image);

  label = ewl_text_new();
  ewl_text_text_set(EWL_TEXT(label), "This is a test for the Dialog widget");
  ewl_container_child_append (EWL_CONTAINER (hbox), label);
  ewl_widget_show (label);

  button = ewl_button_stock_new();
  ewl_button_stock_id_set(EWL_BUTTON_STOCK(button), EWL_STOCK_OK);
  ewl_button_stock_response_id_set(EWL_BUTTON_STOCK(button), EWL_RESPONSE_OK);
  ewl_container_child_append (EWL_CONTAINER (dialog_win), button);
  ewl_callback_append (button, EWL_CALLBACK_VALUE_CHANGED,
		       EWL_CALLBACK_FUNCTION (__create_window_response), NULL);
  ewl_widget_show (button);

  button = ewl_button_stock_new();
  ewl_button_stock_id_set(EWL_BUTTON_STOCK(button), EWL_STOCK_CANCEL);
  ewl_button_stock_response_id_set(EWL_BUTTON_STOCK(button),
				   EWL_RESPONSE_CANCEL);
  ewl_container_child_append (EWL_CONTAINER (dialog_win), button);
  ewl_callback_append (button, EWL_CALLBACK_VALUE_CHANGED,
		       EWL_CALLBACK_FUNCTION (__create_window_response), NULL);
  ewl_widget_show (button);
  
  ewl_widget_show (dialog_win);
}

