#include "enity.h"

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

static void _en_ok_print_stdout_cb(Etk_Object *obj, int response_id, void *data)
{
   switch(response_id)
     {
      case ETK_RESPONSE_OK:
	if(ETK_IS_ENTRY(data))
	  {
	     printf("%s\n", etk_entry_text_get(ETK_ENTRY(data)));
	     break;
	  }
	else if(ETK_IS_TREE(data))
	  {
	     Etk_Tree_Row *row;
	     Evas_List *cols;
#if 0	     
	     Evas_List *l;
	     void **valist;
	     int j = 0;
#endif	     
	     	     	    
	     row = etk_tree_selected_row_get(ETK_TREE(data));
	     cols = etk_tree_row_data_get(row);	     
	     if(!row) break;
	     
	       {
		  char *str;
		  etk_tree_row_fields_get(row, ((Enity_Tree_Col*)(cols->data))->col, &str, NULL);
		  printf("%s\n", str);
	       }	     
#if 0	     
	     valist = calloc(evas_list_count(cols) * 2 + 1, sizeof(void*));
	     
	     for(l = cols; l; l = l->next)
	       {
		  switch( ((Enity_Tree_Col*)(cols->data))->model)
		    {
		     case ENITY_COL_MODEL_TEXT:
		       printf("text...\n");
		       valist[j] = ((Enity_Tree_Col*)l->data)->col;
		       valist[j + 1] = calloc(1, sizeof(char*));
		       j += 2;
		       
		       break;
		     case ENITY_COL_MODEL_CHECK:
		       break;
		     case ENITY_COL_MODEL_RADIO:
		       break;
		    }
	       }
	     
	     valist[j] = NULL;	     
	     etk_tree_row_fields_get_valist(row, (va_list)valist);
	     printf("%s\n", valist[1]);
#endif	     
	     break;
	  }
	break;
      case ETK_RESPONSE_CANCEL:
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
   etk_window_title_set(ETK_WINDOW(dialog), _("Add a new entry"));
   
   etk_widget_show_all(dialog);
}

static void _en_error_cb(Etk_Argument *args, int index)
{
   Etk_Widget *dialog;
   Evas_List  *data;
   const char *value;
     
   if((data = _en_arg_data_get(args, "text")) == NULL)
     value = strdup(_("An error has occured"));

   dialog = etk_message_dialog_new(ETK_MESSAGE_DIALOG_ERROR, ETK_MESSAGE_DIALOG_OK, value);
   etk_signal_connect_swapped("response", ETK_OBJECT(dialog), ETK_CALLBACK(etk_main_quit), NULL);
   
   
   etk_container_border_width_set(ETK_CONTAINER(dialog), 4);
   etk_window_title_set(ETK_WINDOW(dialog), _("Error"));

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
   etk_signal_connect_swapped("response", ETK_OBJECT(dialog), ETK_CALLBACK(etk_main_quit), NULL);
   
   
   etk_container_border_width_set(ETK_CONTAINER(dialog), 4);
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
   etk_signal_connect_swapped("response", ETK_OBJECT(dialog), ETK_CALLBACK(etk_main_quit), NULL);
   
   
   etk_container_border_width_set(ETK_CONTAINER(dialog), 4);
   etk_window_title_set(ETK_WINDOW(dialog), _("Warning"));

   etk_widget_show_all(dialog);
}

static void _en_list_column_cb(Etk_Argument *args, int index)
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
	     col->col = etk_tree_col_new(ETK_TREE(tree), l->data, etk_tree_model_text_new(ETK_TREE(tree)), 60);
	     col->model = ENITY_COL_MODEL_TEXT;
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
		  valist[j + 1] = value;
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
   etk_window_title_set(ETK_WINDOW(dialog), _("Select vaue"));
   
   etk_widget_show_all(dialog);
}

static void _en_dialog_text_cb(Etk_Argument *args, int index)
{
   /* do any changes / fixes / checks here */
}

static void _en_entry_entry_text_cb(Etk_Argument *args, int index)
{
   /* do any changes / fixes / checks here */
}

Etk_Argument args[] = {
     /* global options that are used with more than one dialog type */
     { "text", ' ', NULL, _en_dialog_text_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL|ETK_ARGUMENT_FLAG_VALUE_REQUIRED, "Set the dialog text" },
   
     /* --entry options */
     { "entry-text", ' ', NULL, _en_entry_entry_text_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL|ETK_ARGUMENT_FLAG_VALUE_REQUIRED, "Set the dialog text" },
     { "entry", ' ', NULL, _en_entry_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Display text entry dialog" },
     
     /* --error options */
     { "error", ' ', NULL, _en_error_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Display error dialog" },

     /* --info options */
     { "info", ' ', NULL, _en_info_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Display info dialog" },
   
     /* --warning options */
     { "warning", ' ', NULL, _en_warning_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Display warning dialog" },

     /* --list options */
     { "column", ' ', NULL, _en_list_column_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL|ETK_ARGUMENT_FLAG_VALUE_REQUIRED|ETK_ARGUMENT_FLAG_MULTIVALUE, "Set the column header" },
     { "list", ' ', NULL, _en_list_cb, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, "Display list dialog" },
     /* terminator */
     { NULL, -1,  NULL, NULL, NULL, ETK_ARGUMENT_FLAG_NONE, " " }
};  

int main(int argc, char **argv)
{
   etk_init();
      
   etk_arguments_parse(args, argc, argv);
   
   etk_main();
   etk_main_quit();
   etk_shutdown();
   
   return 0;
}
