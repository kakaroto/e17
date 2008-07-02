#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


Etk_Widget*
script_frame_create(void)
{
   Etk_Widget *sv;
   Etk_Widget *vbox;

   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 0);

   //ScriptBox
   UI_ScriptBox = etk_text_view_new();
   sv = etk_scrolled_view_new();
   etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(sv),
                                 ETK_POLICY_AUTO, ETK_POLICY_AUTO);
   etk_bin_child_set(ETK_BIN(sv), UI_ScriptBox);
   etk_box_append(ETK_BOX(vbox), sv, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //ScriptSaveButton
   UI_ScriptSaveButton = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE);
   etk_object_properties_set(ETK_OBJECT(UI_ScriptSaveButton),
                              "label","Save script",NULL);
   etk_box_append(ETK_BOX(vbox), UI_ScriptSaveButton,
                  ETK_BOX_START, ETK_BOX_NONE, 0);

   etk_signal_connect("clicked", ETK_OBJECT(UI_ScriptSaveButton),
            ETK_CALLBACK(on_AllButton_click), (void*)SAVE_SCRIPT);

   return vbox;
}

void
script_frame_update(void)
{
   if (etk_string_length_get(Cur.group))
   {
      edje_edit_script_get(edje_o);
      //printf("Update group script: %s\n",edje_edit_script_get(edje_o));
   }
}
