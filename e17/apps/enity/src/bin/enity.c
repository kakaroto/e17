#include "enity.h"

static int _en_retval = 0;
static Evas_List *_en_checked_rows = NULL;

static Etk_Bool _en_window_delete_cb(void *data)
{
   etk_main_quit();
   return ETK_TRUE;
}

static Evas_List *_en_arg_data_get(Etk_Argument *args, char *key)
{
   Etk_Argument *arg;
   
   arg = args;   
   while(arg->short_name != -1)
     {
	if(!strcmp(arg->long_name, key))
	  return arg->data;
	++arg;
     }
   return NULL;
}

static void _en_tree_checkbox_toggled_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_Bool checked;
   Etk_Tree_Col *col;
   
   if (!(col = ETK_TREE_COL(object)) || !row)
     return;
   
   etk_tree_row_fields_get(row, col, &checked, NULL);
   if (checked)
     _en_checked_rows = evas_list_append(_en_checked_rows, row);
   else
     _en_checked_rows = evas_list_remove(_en_checked_rows, row);
}


static void _en_ok_print_stdout_cb(Etk_Object *obj, int response_id, void *data)
{
   switch(response_id)
     {
      case ETK_RESPONSE_OK:
	if (!data)
	  {
	     _en_retval = 0;
	     break;
	  }
	else if(ETK_IS_SLIDER(data))
	  {
	     printf("%d\n", (int)etk_range_value_get(ETK_RANGE(data)));
	     break;
	  }
	else if(ETK_IS_ENTRY(data))
	  {
	     printf("%s\n", etk_entry_text_get(ETK_ENTRY(data)));
	     break;
	  }
	else if(ETK_IS_TREE(data))
	  {
	     Etk_Tree_Row *row;
	     Evas_List *cols;
	     Evas_List *rows;	     
	     char *str = NULL;
	     int check_value;
	     
	     if(_en_checked_rows)	       
	       row = _en_checked_rows->data;
	     else
	       row = etk_tree_selected_row_get(ETK_TREE(data));
		  
	     if(!row) break;

	     cols = etk_tree_row_data_get(row);
	     
	     if(!cols) break;
	     
	     switch(((Enity_Tree_Col*)(cols->data))->model)
	       {
		case ENITY_COL_MODEL_TEXT:
		  if(!row) break;		       
		  etk_tree_row_fields_get(row, ((Enity_Tree_Col*)(cols->data))->col, &str, NULL);
		  break;
		  
		case ENITY_COL_MODEL_CHECK:
		  
		  str = calloc(PATH_MAX, sizeof(char));
		  
		  for(rows = _en_checked_rows; rows; rows = rows->next)
		    {
		       char *val;		       
		       
		       if(!(cols->next))
			 break;
		       
		       if(strlen(str) > 0)
			 strncat(str, "|", PATH_MAX);			    
		       etk_tree_row_fields_get(rows->data, ((Enity_Tree_Col*)(cols->data))->col, &check_value, ((Enity_Tree_Col*)(cols->next->data))->col, &val, NULL);
		       strncat(str, val, PATH_MAX);
		    }
		  break;
		  
		case ENITY_COL_MODEL_RADIO:
		  break;
	       }
	     
	     printf("%s\n", str);
	  }
	break;
	
      case ETK_RESPONSE_CANCEL:
	_en_retval = 1;
	break;
     }
   
   etk_main_quit();
}

static void _en_entry_cb(Etk_Argument *args, int index)
{  
   Etk_Widget *dialog;   
   Etk_Widget *label;
   Etk_Widget *entry;
   Evas_List *data;
   
   dialog = etk_dialog_new();
   etk_signal_connect("delete_event", ETK_OBJECT(dialog), ETK_CALLBACK(_en_window_delete_cb), NULL);
   
   if((data = _en_arg_data_get(args, "text")) != NULL)
     label = etk_label_new(data->data);
   else
     label = etk_label_new(_("Enter new text:"));
   
   entry = etk_entry_new();
   if((data = _en_arg_data_get(args, "entry-text")) != NULL)
     etk_entry_text_set(ETK_ENTRY(entry), data->data);
      
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), label, ETK_TRUE, ETK_TRUE, 3, ETK_FALSE);
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), entry, ETK_TRUE, ETK_TRUE, 3, ETK_FALSE);
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_OK, ETK_RESPONSE_OK);
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_CANCEL, ETK_RESPONSE_CANCEL);
   etk_signal_connect("response", ETK_OBJECT(dialog), ETK_CALLBACK(_en_ok_print_stdout_cb), entry);
   
   etk_container_border_width_set(ETK_CONTAINER(dialog), 4);
   
   if((data = _en_arg_data_get(args, "title")) != NULL)
     etk_window_title_set(ETK_WINDOW(dialog), data->data);
   else
     etk_window_title_set(ETK_WINDOW(dialog), _("Add a new entry"));
   
   etk_widget_show_all(dialog);
}

static void _en_error_cb(Etk_Argument *args, int index)
{
   Etk_Widget *dialog;
   const char *value;
   Evas_List *data;
     
   if((data = _en_arg_data_get(args, "text")) == NULL)
     value = strdup(_("No information available"));
   else
     value = data->data;

   dialog = etk_message_dialog_new(ETK_MESSAGE_DIALOG_ERROR, ETK_MESSAGE_DIALOG_OK, value);
   etk_signal_connect("delete_event", ETK_OBJECT(dialog), ETK_CALLBACK(_en_window_delete_cb), NULL);
   etk_signal_connect_swapped("response", ETK_OBJECT(dialog), ETK_CALLBACK(etk_main_quit), NULL);
      
   etk_container_border_width_set(ETK_CONTAINER(dialog), 4);
   
   if((data = _en_arg_data_get(args, "title")) != NULL)
     etk_window_title_set(ETK_WINDOW(dialog), data->data);
   else
     etk_window_title_set(ETK_WINDOW(dialog), _("Error"));

   etk_widget_show_all(dialog);
}

static void _en_question_cb(Etk_Argument *args, int index)
{
   Etk_Widget *dialog;
   const char *value;
   Evas_List *data;
     
   if((data = _en_arg_data_get(args, "text")) == NULL)
     value = strdup(_("No information available"));
   else
     value = data->data;

   dialog = etk_message_dialog_new(ETK_MESSAGE_DIALOG_QUESTION, ETK_MESSAGE_DIALOG_OK_CANCEL, value);
   etk_signal_connect("delete_event", ETK_OBJECT(dialog), ETK_CALLBACK(_en_window_delete_cb), NULL);
   etk_signal_connect("response", ETK_OBJECT(dialog), ETK_CALLBACK(_en_ok_print_stdout_cb), NULL);
   
   etk_container_border_width_set(ETK_CONTAINER(dialog), 4);
   
   if((data = _en_arg_data_get(args, "title")) != NULL)
     etk_window_title_set(ETK_WINDOW(dialog), data->data);
   else
     etk_window_title_set(ETK_WINDOW(dialog), _("Question"));
   
   etk_widget_show_all(dialog);
}

static void _en_info_cb(Etk_Argument *args, int index)
{
   Etk_Widget *dialog;
   const char *value;
   Evas_List *data;
     
   if((data = _en_arg_data_get(args, "text")) == NULL)
     value = strdup(_("No information available"));
   else
     value = data->data;

   dialog = etk_message_dialog_new(ETK_MESSAGE_DIALOG_INFO, ETK_MESSAGE_DIALOG_OK, value);
   etk_signal_connect("delete_event", ETK_OBJECT(dialog), ETK_CALLBACK(_en_window_delete_cb), NULL);   
   etk_signal_connect_swapped("response", ETK_OBJECT(dialog), ETK_CALLBACK(etk_main_quit), NULL);
      
   etk_container_border_width_set(ETK_CONTAINER(dialog), 4);
   
   if((data = _en_arg_data_get(args, "title")) != NULL)
     etk_window_title_set(ETK_WINDOW(dialog), data->data);
   else
     etk_window_title_set(ETK_WINDOW(dialog), _("Information"));       

   etk_widget_show_all(dialog);
}

static void _en_warning_cb(Etk_Argument *args, int index)
{
   Etk_Widget *dialog;
   Evas_List  *data;
   const char *value;
     
   if((data = _en_arg_data_get(args, "text")) == NULL)
     value = strdup(_("No warning available"));
   else
     value = data->data;

   dialog = etk_message_dialog_new(ETK_MESSAGE_DIALOG_WARNING, ETK_MESSAGE_DIALOG_OK, value);
   etk_signal_connect("delete_event", ETK_OBJECT(dialog), ETK_CALLBACK(_en_window_delete_cb), NULL);   
   etk_signal_connect_swapped("response", ETK_OBJECT(dialog), ETK_CALLBACK(etk_main_quit), NULL);
      
   etk_container_border_width_set(ETK_CONTAINER(dialog), 4);
   
   if((data = _en_arg_data_get(args, "title")) != NULL)
     etk_window_title_set(ETK_WINDOW(dialog), data->data);
   else
     etk_window_title_set(ETK_WINDOW(dialog), _("Question"));

   etk_widget_show_all(dialog);
}

static void _en_list_column_cb(Etk_Argument *args, int index)
{
   /* do any changes / fixes / checks here */
}

static void _en_list_check_cb(Etk_Argument *args, int index)
{
   /* do any changes / fixes / checks here */
}

static void _en_list_cb(Etk_Argument *args, int index)
{
   Etk_Widget *dialog;   
   Etk_Widget *label;
   Etk_Widget *tree;
   Evas_List *cols = NULL;
   Evas_List *data;
   
   dialog = etk_dialog_new();
   etk_signal_connect("delete_event", ETK_OBJECT(dialog), ETK_CALLBACK(_en_window_delete_cb), NULL);   
      
   if((data = _en_arg_data_get(args, "text")) != NULL)
     label = etk_label_new(data->data);
   else
     label = etk_label_new(_(" "));
   
   tree = etk_tree_new();
   etk_widget_size_request_set(tree, 320, 240);
   etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);
   
   if((data = _en_arg_data_get(args, "column")) != NULL)
     {
	Evas_List *l;
	
	for(l = data; l; l = l->next)
	  {
	     Enity_Tree_Col *col;
	     
	     /* todo - implement checkboxes and radio buttons */
	     col = calloc(1, sizeof(Enity_Tree_Col));
	     	     
	     if(etk_argument_is_set(args, "checklist", ' ') && evas_list_count(cols) == 0)
	       {
		  col->col = etk_tree_col_new(ETK_TREE(tree), l->data, etk_tree_model_checkbox_new(ETK_TREE(tree)), 60);
		  col->model = ENITY_COL_MODEL_CHECK;
		  etk_signal_connect("cell_value_changed", ETK_OBJECT(col->col), ETK_CALLBACK(_en_tree_checkbox_toggled_cb), NULL);
		  
	       }
	     else
	       {
		  col->col = etk_tree_col_new(ETK_TREE(tree), l->data, etk_tree_model_text_new(ETK_TREE(tree)), 60);
		  col->model = ENITY_COL_MODEL_TEXT;		  
	       }
	     cols = evas_list_append(cols, col);
	  }
	etk_tree_build(ETK_TREE(tree));
	
	for(l = etk_argument_extra_find("column"); l; l = l->next)
	  {
	     int i;
	     int j = 0;
	     void **valist;
	     Etk_Tree_Row *row;
	     
	     valist = calloc(evas_list_count(cols) * 2 + 1, sizeof(void*));
	     
	     for(i = 0; i < evas_list_count(cols); i++)
	       {
		  char *value;
		  
		  value = l->data;
		  
		  valist[j] = ((Enity_Tree_Col*)evas_list_nth(cols, i))->col;
		  
		  switch(((Enity_Tree_Col*)evas_list_nth(cols, i))->model)
		    {
		     case ENITY_COL_MODEL_TEXT:
		       valist[j + 1] = value;
		       break;
		       
		     case ENITY_COL_MODEL_CHECK:
		       valist[j + 1] = ETK_FALSE;
		       break;
		       
		     case ENITY_COL_MODEL_RADIO:
		       break;
		    }
		  
		  j += 2;
		  		  
		  if(l->next && i < evas_list_count(cols) - 1)
		    {
		       l = l->next;
		    }
		  else
		    break;		  		  
	       }
	     
	     valist[j] = NULL;
	     	     
	     row = etk_tree_append_valist(ETK_TREE(tree), (va_list)valist);
	     etk_tree_row_data_set(row, cols);
	  }
     }
            
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), label, ETK_TRUE, ETK_TRUE, 3, ETK_FALSE);
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), tree, ETK_TRUE, ETK_TRUE, 3, ETK_FALSE);
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_OK, ETK_RESPONSE_OK);
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_CANCEL, ETK_RESPONSE_CANCEL);
   etk_signal_connect("response", ETK_OBJECT(dialog), ETK_CALLBACK(_en_ok_print_stdout_cb), tree);
   etk_container_border_width_set(ETK_CONTAINER(dialog), 4);
   
   if((data = _en_arg_data_get(args, "title")) != NULL)
     etk_window_title_set(ETK_WINDOW(dialog), data->data);
   else
     etk_window_title_set(ETK_WINDOW(dialog), _("Select value"));         
   
   etk_widget_show_all(dialog);
}

static void _en_dialog_text_cb(Etk_Argument *args, int index)
{
   /* do any changes / fixes / checks here */
}

static void _en_dialog_title_cb(Etk_Argument *args, int index)
{
   /* do any changes / fixes / checks here */
}

static void _en_entry_entry_text_cb(Etk_Argument *args, int index)
{
   /* do any changes / fixes / checks here */
}

void _en_slider_value_changed(Etk_Object *object, double value, void *data)
{
   char string[256];
   
   snprintf(string, 255, "%d", (int)value);
   etk_label_set(ETK_LABEL(data), string);
}

static void _en_scale_cb(Etk_Argument *args, int index)
{
   Etk_Widget *dialog;   
   Etk_Widget *label;
   Etk_Widget *slider_label;
   Etk_Widget *slider;
   Etk_Widget *slider_hbox;
   Evas_List *data;
   int min_value;
   int max_value;
   int step_value;
   int value;
   
   dialog = etk_dialog_new();
   etk_signal_connect("delete_event", ETK_OBJECT(dialog), ETK_CALLBACK(_en_window_delete_cb), NULL);
   
   if((data = _en_arg_data_get(args, "text")) != NULL)
     label = etk_label_new(data->data);
   else
     label = etk_label_new(_("Adjust the slider value:"));

   slider_hbox = etk_hbox_new(ETK_FALSE, 5);
   
   if((data = _en_arg_data_get(args, "min-value")) != NULL)
     min_value = atoi(data->data);
   else
     min_value = 0;
      
   if((data = _en_arg_data_get(args, "max-value")) != NULL)
     max_value = atoi(data->data);
   else
     max_value = 100;
   
   if((data = _en_arg_data_get(args, "step")) != NULL)
     step_value = atoi(data->data);
   else
     step_value = 1;
   
   if((data = _en_arg_data_get(args, "value")) != NULL)     	
     value = atoi(data->data);
   else
     value = 0;
   
   slider = etk_hslider_new((double)min_value, (double)max_value, (double)value, (double)step_value, 10.0);
   etk_box_pack_start(ETK_BOX(slider_hbox), slider, ETK_TRUE, ETK_TRUE, 0);
   
   if(!etk_argument_is_set(args, "hide-value", ' '))
     {
	char str[256];
	
	snprintf(str, sizeof(str), "%d", (int)value);
	slider_label = etk_label_new(str);
	etk_signal_connect("value_changed", ETK_OBJECT(slider), ETK_CALLBACK(_en_slider_value_changed), slider_label);
	etk_box_pack_start(ETK_BOX(slider_hbox), slider_label, ETK_TRUE, ETK_TRUE, 0);
     }
   
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), label, ETK_FALSE, ETK_FALSE, 3, ETK_FALSE);
   etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), slider_hbox, ETK_FALSE, ETK_FALSE, 3, ETK_FALSE);
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_OK, ETK_RESPONSE_OK);
   etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DIALOG_CANCEL, ETK_RESPONSE_CANCEL);
   etk_signal_connect("response", ETK_OBJECT(dialog), ETK_CALLBACK(_en_ok_print_stdout_cb), slider);
   
   etk_container_border_width_set(ETK_CONTAINER(dialog), 4);
   
   if((data = _en_arg_data_get(args, "title")) != NULL)
     etk_window_title_set(ETK_WINDOW(dialog), data->data);
   else
     etk_window_title_set(ETK_WINDOW(dialog), _("Adjust the slider value"));
   
   etk_widget_show_all(dialog);
}

static void _en_scale_step_cb(Etk_Argument *args, int index)
{
   /* do any changes / fixes / checks here */
}

static void _en_scale_value_cb(Etk_Argument *args, int index)
{
   /* do any changes / fixes / checks here */
}

static void _en_scale_min_value_cb(Etk_Argument *args, int index)
{
   /* do any changes / fixes / checks here */
}

static void _en_scale_max_value_cb(Etk_Argument *args, int index)
{
   /* do any changes / fixes / checks here */
}

static void _en_scale_hide_value_cb(Etk_Argument *args, int index)
{
   /* do any changes / fixes / checks here */
}

Etk_Argument args[] = {
     /* global options that are used with more than one dialog type */
     { "text", ' ', NULL, _en_dialog_text_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL|ETK_ARGUMENT_FLAG_VALUE_REQUIRED, "Set the dialog text" },
     { "title", ' ', NULL, _en_dialog_title_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL|ETK_ARGUMENT_FLAG_VALUE_REQUIRED, "Set the dialog title" },
   
     /* --entry options */
     { "entry-text", ' ', NULL, _en_entry_entry_text_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL|ETK_ARGUMENT_FLAG_VALUE_REQUIRED, "Set the dialog text" },
     { "entry", ' ', NULL, _en_entry_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Display text entry dialog" },
     
     /* --error options */
     { "error", ' ', NULL, _en_error_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Display error dialog" },
   
     /* --question options */
     { "question", ' ', NULL, _en_question_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Display question dialog" },   

     /* --info options */
     { "info", ' ', NULL, _en_info_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Display info dialog" },
   
     /* --warning options */
     { "warning", ' ', NULL, _en_warning_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Display warning dialog" },
   
     /* --scale options */
     { "step", ' ', NULL, _en_scale_step_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL|ETK_ARGUMENT_FLAG_VALUE_REQUIRED, "Step value for the scale" },
     { "value", ' ', NULL, _en_scale_value_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL|ETK_ARGUMENT_FLAG_VALUE_REQUIRED, "Initial value for the scale" },
     { "min-value", ' ', NULL, _en_scale_min_value_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL|ETK_ARGUMENT_FLAG_VALUE_REQUIRED, "Min value for the scale" },
     { "max-value", ' ', NULL, _en_scale_max_value_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL|ETK_ARGUMENT_FLAG_VALUE_REQUIRED, "Max value for the scale" },
     { "hide-value", ' ', NULL, _en_scale_hide_value_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Hide the label for the scale" },
     { "scale", ' ', NULL, _en_scale_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Display scale dialog" },

     /* --list options */
     { "checklist", ' ', NULL, _en_list_check_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Use check boxes for first column" },
     { "column", ' ', NULL, _en_list_column_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL|ETK_ARGUMENT_FLAG_VALUE_REQUIRED|ETK_ARGUMENT_FLAG_MULTIVALUE, "Set the column header" },
     { "list", ' ', NULL, _en_list_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Display list dialog" },
     /* terminator */
     { NULL, -1,  NULL, NULL, NULL, ETK_ARGUMENT_FLAG_NONE, " " }
};  

int main(int argc, char **argv)
{
   etk_init();
      
   if(etk_arguments_parse(args, argc, argv) != ETK_ARGUMENT_RETURN_OK)
     {
	etk_argument_help_show(args);
	goto SHUTDOWN;
     }
   
   etk_main();
   etk_main_quit();
SHUTDOWN:   
   etk_shutdown();
   
   return _en_retval;
}
