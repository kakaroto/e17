#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


Etk_Widget*
rectangle_frame_create(Evas *evas)
{
   return window_color_button_create("Color", COLOR_OBJECT_RECT, 100, 30, evas);
}

void
rectangle_frame_update(void)
{
   Etk_Color color;

   if (etk_string_length_get(Cur.state))
   {
      edje_edit_state_color_get(edje_o, Cur.part->string, Cur.state->string,
                                &color.r, &color.g, &color.b, &color.a);

      etk_signal_block("color-changed", ETK_OBJECT(UI_ColorPicker),
                       ETK_CALLBACK(_dialog_colorpicker_change_cb), NULL);

      //Set ColorPicker
      etk_colorpicker_current_color_set (ETK_COLORPICKER(UI_ColorPicker), color);
      //Set Color rect
      evas_color_argb_premul(color.a,&color.r,&color.g,&color.b);
      evas_object_color_set(RectColorObject,color.r,color.g,color.b,color.a);

      etk_signal_unblock("color-changed", ETK_OBJECT(UI_ColorPicker),
                         ETK_CALLBACK(_dialog_colorpicker_change_cb), NULL);
   }
}
