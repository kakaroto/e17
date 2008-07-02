#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"

//#include "callbacks.h"




void
ShowAlert(char* text)
{
   etk_message_dialog_text_set(ETK_MESSAGE_DIALOG(UI_AlertDialog), text);
   etk_widget_show_all(UI_AlertDialog);
}

Etk_Widget*
create_alert_dialog(void)
{
   UI_AlertDialog = etk_message_dialog_new (ETK_MESSAGE_DIALOG_INFO,
                                       ETK_MESSAGE_DIALOG_OK, "Hallo world!");
   etk_widget_size_request_set(UI_AlertDialog, 240, 100);
   etk_signal_connect("delete-event", ETK_OBJECT(UI_AlertDialog),
                      ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_signal_connect("response", ETK_OBJECT(UI_AlertDialog),
                      ETK_CALLBACK(on_AlertDialog_response), NULL);
   return UI_AlertDialog;
}

Etk_Widget*
create_filechooser_dialog(void)
{
   //Dialog
   UI_FileChooserDialog = etk_dialog_new();
   etk_object_properties_set (ETK_OBJECT(UI_FileChooserDialog), "action-area-homogeneous",ETK_FALSE,NULL);
   etk_signal_connect("delete-event", ETK_OBJECT(UI_FileChooserDialog), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_signal_connect("response", ETK_OBJECT(UI_FileChooserDialog), ETK_CALLBACK(on_FileChooserDialog_response), NULL);

   //Filechooser
   UI_FileChooser = etk_filechooser_widget_new();
   etk_dialog_pack_in_main_area(ETK_DIALOG(UI_FileChooserDialog), UI_FileChooser,
      ETK_BOX_START, ETK_BOX_EXPAND_FILL,0);
   etk_signal_connect("selected", ETK_OBJECT(UI_FileChooser), ETK_CALLBACK(on_FileChooser_selected), NULL);

   etk_dialog_button_add_from_stock(ETK_DIALOG(UI_FileChooserDialog),
      ETK_STOCK_DIALOG_CANCEL, ETK_RESPONSE_CANCEL );
   UI_FilechooserLoadButton = etk_dialog_button_add_from_stock(ETK_DIALOG(UI_FileChooserDialog),
      ETK_STOCK_DOCUMENT_OPEN ,ETK_RESPONSE_OK );
   UI_FilechooserSaveButton = etk_dialog_button_add_from_stock(ETK_DIALOG(UI_FileChooserDialog),
      ETK_STOCK_DOCUMENT_SAVE ,ETK_RESPONSE_OK );

   return UI_FileChooserDialog;
}

void
ShowFilechooser(int FileChooserType)
{
   etk_widget_show_all(UI_FileChooserDialog);
   
   FileChooserOperation = FileChooserType;
   switch(FileChooserType){
      case FILECHOOSER_OPEN:
         etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose an EDJ or EDC file to open");
         etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(UI_FileChooser),ETK_FALSE);
         etk_widget_hide(UI_FilechooserSaveButton);
      break;
      case FILECHOOSER_IMAGE:
         etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose an image to import");
         etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(UI_FileChooser),ETK_FALSE);
         etk_widget_hide(UI_FilechooserSaveButton);
      break;
      case FILECHOOSER_FONT:
         etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose an font to import");
         etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(UI_FileChooser),ETK_FALSE);
         etk_widget_hide(UI_FilechooserSaveButton);
      break;
      case FILECHOOSER_SAVE_EDJ:
         etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose the new edje name");
         etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(UI_FileChooser),ETK_TRUE);
         etk_widget_hide(UI_FilechooserLoadButton);
      break;
      case FILECHOOSER_SAVE_EDC:
         etk_window_title_set(ETK_WINDOW(UI_FileChooserDialog), "Choose the new edc name");
         etk_filechooser_widget_is_save_set(ETK_FILECHOOSER_WIDGET(UI_FileChooser),ETK_TRUE);
         etk_widget_hide(UI_FilechooserLoadButton);
      break;
      default:
      break;
   }
}



Etk_Bool
on_FileChooserDialog_response(Etk_Dialog *dialog, int response_id, void *data)
{
   char cmd[4096];

   printf("Response Signal on Filechooser EMITTED\n");

   if (response_id == ETK_RESPONSE_OK){

      switch(FileChooserOperation){
         case FILECHOOSER_OPEN:
            snprintf(cmd,4096,"%s/%s",
            etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
            etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            LoadEDJ(cmd);
         break;
         case FILECHOOSER_SAVE_EDJ:
            printf("SAVE EDJ\n");
            snprintf(cmd,4096,"%s/%s",
               etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
               etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            edje_edit_save(edje_o);
            if(!ecore_file_cp(Cur.edj_temp_name->string, cmd))
            {
               ShowAlert("<b>ERROR:<\b><br>Can't write file");
            }
            else
            {
               Cur.edj_file_name = etk_string_set(Cur.edj_file_name, cmd);
               ecore_evas_title_set(UI_ecore_MainWin, cmd);
            }
         break;
         case FILECHOOSER_SAVE_EDC:
              ShowAlert("Not yet implemented.");
         break;
         case FILECHOOSER_IMAGE:
            snprintf(cmd, 4096, "%s/%s", 
               etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
               etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            if (!edje_edit_image_add(edje_o, cmd))
            {
               ShowAlert("ERROR: Can't import image file.");
               break;
            }
            PopulateImageBrowser();
            
            Etk_Range *range;
            double upper;
            range = etk_scrolled_view_vscrollbar_get(
                    etk_iconbox_scrolled_view_get(ETK_ICONBOX(UI_ImageBrowserIconbox)));
            etk_range_range_get(range, NULL, &upper);
            etk_range_value_set(range, upper);
            
            Etk_Iconbox_Icon *icon;
            icon = etk_iconbox_icon_get_by_label(ETK_ICONBOX(UI_ImageBrowserIconbox),
                                 etk_filechooser_widget_selected_file_get(
                                 ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            etk_iconbox_icon_select(icon);
            
            break;
         case FILECHOOSER_FONT:
            snprintf(cmd, 4096, "%s/%s", 
               etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)),
               etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(UI_FileChooser)));
            if (!edje_edit_font_add(edje_o, cmd))
            {
               ShowAlert("ERROR: Can't import font file.");
               break;
            }
            PopulateFontsComboBox();
            etk_combobox_active_item_set(ETK_COMBOBOX(UI_FontComboBox),
                  etk_combobox_last_item_get(ETK_COMBOBOX(UI_FontComboBox)));
         break;
      }
      etk_widget_hide(ETK_WIDGET(dialog));
   }
   else
   {
      etk_widget_hide(ETK_WIDGET(dialog));
   }

   return ETK_TRUE;
}

Etk_Bool
on_FileChooser_selected(Etk_Filechooser_Widget *filechooser)
{
   printf("*** FILECHOOSER SELECTD ON *** \n");
   on_FileChooserDialog_response(ETK_DIALOG(UI_FileChooserDialog), ETK_RESPONSE_OK, NULL);
   return ETK_TRUE;
}



Etk_Bool
on_AlertDialog_response(Etk_Dialog *dialog, int response_id, void *data)
{
   etk_widget_hide(ETK_WIDGET(dialog));
   return ETK_TRUE;
}

Etk_Widget *
create_colorpicker(void)
{
   UI_ColorWin = etk_window_new();
   etk_signal_connect("delete-event", ETK_OBJECT(UI_ColorWin),
                      ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   UI_ColorPicker = etk_colorpicker_new();
   etk_colorpicker_use_alpha_set (ETK_COLORPICKER(UI_ColorPicker), TRUE);
   etk_container_add(ETK_CONTAINER(UI_ColorWin), UI_ColorPicker);
   etk_signal_connect("color-changed", ETK_OBJECT(UI_ColorPicker),
                        ETK_CALLBACK(on_ColorDialog_change), NULL);
   return UI_ColorWin;
}

Etk_Bool
on_ColorDialog_change(Etk_Object *object, void *data)
{
  // printf("ColorChangeSignal on ColorDialog EMITTED\n");
   Etk_Color color;
   Etk_Color premuled;

   color = etk_colorpicker_current_color_get(ETK_COLORPICKER(object));
 //  printf("Color: %d %d %d %d\n",color.r,color.g,color.b,color.a);

   if (color.r > 255) color.r = 255;
   if (color.g > 255) color.g = 255;
   if (color.b > 255) color.b = 255;
   if (color.a > 255) color.a = 255;

   if (color.r < 0) color.r = 0;
   if (color.g < 0) color.g = 0;
   if (color.b < 0) color.b = 0;
   if (color.a < 0) color.a = 0;

   premuled = color;
   evas_color_argb_premul(premuled.a,&premuled.r,&premuled.g,&premuled.b);

   switch (current_color_object){
    case COLOR_OBJECT_RECT:
      evas_object_color_set(RectColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
      edje_edit_state_color_set(edje_o, Cur.part->string, Cur.state->string,
                                premuled.r,premuled.g,premuled.b,premuled.a);

      break;
    case COLOR_OBJECT_TEXT:
      evas_object_color_set(TextColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
      edje_edit_state_color_set(edje_o, Cur.part->string, Cur.state->string,
                                premuled.r,premuled.g,premuled.b,premuled.a);

      break;
    case COLOR_OBJECT_SHADOW:
      evas_object_color_set(ShadowColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
      edje_edit_state_color3_set(edje_o, Cur.part->string, Cur.state->string,
                                premuled.r,premuled.g,premuled.b,premuled.a);

      break;
    case COLOR_OBJECT_OUTLINE:
      evas_object_color_set(OutlineColorObject,premuled.r,premuled.g,premuled.b,premuled.a);
      edje_edit_state_color2_set(edje_o, Cur.part->string, Cur.state->string,
                                premuled.r,premuled.g,premuled.b,premuled.a);

      break;
   }

   ev_redraw();
   return ETK_TRUE;
}
