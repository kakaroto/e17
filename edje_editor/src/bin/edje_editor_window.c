/*  Copyright (C) 2006-2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of Edje_editor.
 *  Edje_editor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Edje_editor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Edje_editor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <dirent.h>
#include <string.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include <Etk.h>
#include "main.h"


/***   Implementation   ***/
void
window_main_create(void)
{
   //Create the main ecore_evas window
#if USE_GL_ENGINE
   UI_ecore_MainWin = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 32, 32);
#else
   UI_ecore_MainWin = ecore_evas_software_x11_new(NULL, 0, 0, 0, 32, 32);
#endif
   ecore_evas_title_set(UI_ecore_MainWin, "Edje Editor");
   ecore_evas_callback_resize_set(UI_ecore_MainWin, _window_resize_cb);
   ecore_evas_callback_delete_request_set(UI_ecore_MainWin, _window_delete_cb);
   ecore_evas_resize(UI_ecore_MainWin, 950, 500);
   ecore_evas_size_min_set(UI_ecore_MainWin, 600, 350);
   UI_evas = ecore_evas_get(UI_ecore_MainWin);

   //Load main edje interface
   edje_ui = edje_object_add(UI_evas);
   edje_object_file_set(edje_ui, EdjeFile, "MainUI");
   evas_object_move(edje_ui, 0, 0);
   evas_object_show(edje_ui);

   //Tooltips
   etk_tooltips_init();
   etk_tooltips_enable();

   //Create the evas objects needed by the canvas (fakewin, handlers)
   canvas_prepare();

   //ToolbarEmbed
   UI_ToolbarEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_ToolbarEmbed),
                     toolbar_create(ETK_TOOLBAR_HORIZ));
   etk_embed_position_method_set(ETK_EMBED(UI_ToolbarEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_ToolbarEmbed);
   evas_object_move(etk_embed_object_get(ETK_EMBED(UI_ToolbarEmbed)), 130, 0);
   evas_object_resize(etk_embed_object_get(ETK_EMBED(UI_ToolbarEmbed)), 0, 50);

   //TreeEmbed
   UI_PartsTreeEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_PartsTreeEmbed), tree_create());
   etk_embed_position_method_set(ETK_EMBED(UI_PartsTreeEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_PartsTreeEmbed);

   //GroupEmbed
   UI_GroupEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_GroupEmbed), group_frame_create());
   etk_embed_position_method_set(ETK_EMBED(UI_GroupEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_GroupEmbed);
   edje_object_part_swallow(edje_ui,"group_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_GroupEmbed)));

   //PartEmbed
   UI_PartEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_PartEmbed), part_frame_create());
   etk_embed_position_method_set(ETK_EMBED(UI_PartEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_PartEmbed);
   edje_object_part_swallow(edje_ui,"part_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_PartEmbed)));

   //DescriptionEmbed
   UI_DescriptionEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_DescriptionEmbed), state_frame_create());
   etk_embed_position_method_set(ETK_EMBED(UI_DescriptionEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_DescriptionEmbed);
   edje_object_part_swallow(edje_ui,"description_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_DescriptionEmbed)));

   //RectEmbed
   UI_RectEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_RectEmbed), rectangle_frame_create(UI_evas));
   etk_embed_position_method_set(ETK_EMBED(UI_RectEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_RectEmbed);
   edje_object_part_swallow(edje_ui,"rect_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_RectEmbed)));

   //TextEmbed
   UI_TextEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_TextEmbed), text_frame_create(UI_evas));
   etk_embed_position_method_set(ETK_EMBED(UI_TextEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_TextEmbed);
   edje_object_part_swallow(edje_ui,"text_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_TextEmbed)));

   //ImageEmbed
   UI_ImageEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_ImageEmbed), image_frame_create());
   etk_embed_position_method_set(ETK_EMBED(UI_ImageEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_ImageEmbed);
   edje_object_part_swallow(edje_ui,"image_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_ImageEmbed)));

   //GradientEmbed
   UI_GradientEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_GradientEmbed), gradient_frame_create());
   etk_embed_position_method_set(ETK_EMBED(UI_GradientEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_GradientEmbed);
   edje_object_part_swallow(edje_ui,"gradient_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_GradientEmbed)));

   //FillEmbed
   UI_FillEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_FillEmbed), fill_frame_create());
   etk_embed_position_method_set(ETK_EMBED(UI_FillEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_FillEmbed);
   edje_object_part_swallow(edje_ui,"fill_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_FillEmbed)));

   //PositionEmbed
   UI_PositionEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_PositionEmbed), position_frame_create());
   etk_embed_position_method_set(ETK_EMBED(UI_PositionEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_PositionEmbed);
   edje_object_part_swallow(edje_ui,"position_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_PositionEmbed)));

   //ProgramEmbed
   UI_ProgramEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_ProgramEmbed), program_frame_create());
   etk_embed_position_method_set(ETK_EMBED(UI_ProgramEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_ProgramEmbed);
   edje_object_part_swallow(edje_ui,"program_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_ProgramEmbed)));

   //ScriptEmbed
   UI_ScriptEmbed = etk_embed_new(UI_evas);
   etk_container_add(ETK_CONTAINER(UI_ScriptEmbed), script_frame_create());
   etk_embed_position_method_set(ETK_EMBED(UI_ScriptEmbed),
                                 window_embed_position_set, UI_ecore_MainWin);
   etk_widget_show_all(UI_ScriptEmbed);
   edje_object_part_swallow(edje_ui,"script_frame_swallow",
                            etk_embed_object_get(ETK_EMBED(UI_ScriptEmbed)));


   //Logo (keygrabber)
   Evas_Object *logo;
   logo = edje_object_add(UI_evas);
   edje_object_file_set(logo, EdjeFile, "Logo");
   evas_object_event_callback_add(logo, EVAS_CALLBACK_KEY_DOWN,
                                  _window_logo_key_press, NULL);
   Evas_Modifier_Mask mask;
   mask = evas_key_modifier_mask_get(UI_evas, "Control");
   evas_object_key_grab(logo, "q", mask, 0, 0); // quit
   evas_object_key_grab(logo, "f", mask, 0, 0); // fullscreen
   evas_object_key_grab(logo, "s", mask, 0, 0); // save
   evas_object_key_grab(logo, "c", mask, 0, 0); // copy selection (TODO)
   evas_object_key_grab(logo, "v", mask, 0, 0); // paste selection (TODO)
   evas_object_key_grab(logo, "x", mask, 0, 0); // cut selection (TODO)
   evas_object_key_grab(logo, "d", mask, 0, 0); // duplicate selection (TODO)
   evas_object_key_grab(logo, "n", mask, 0, 0); // new object (TODO)
   evas_object_show(logo);

   //Consolle
   EV_Consolle = consolle_create();

   //Filechooser
   UI_FileChooserDialog = dialog_filechooser_create();

   //Alert Dialog
   UI_AlertDialog = dialog_alert_create();

   //ColorPicker
   UI_ColorWin = dialog_colorpicker_create();

   //Image Browser
   image_browser_create();

   //Spetrum window
   spectra_window_create();
   
   //Data window
   data_window_create();
   
   //Color Classes window
   colors_window_create();

   //Create the main edje object to edit
   edje_o = edje_object_add(UI_evas);
   edje_object_signal_callback_add(edje_o, "*", "*",
                                   _window_edit_obj_signal_cb, NULL);
   evas_object_event_callback_add(edje_o, EVAS_CALLBACK_MOUSE_DOWN,
                                  _window_edit_obj_click, NULL);
}

Etk_Widget*
window_color_button_create(char* label_text, int color_button_enum, int w, int h, Evas *evas)
{
   Etk_Widget *vbox;
   Etk_Widget *label;
   Etk_Widget *shadow;
   Evas_Object* rect;
   Etk_Widget *etk_evas_object;

   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 0);

   //shadow
   shadow = etk_shadow_new();
   etk_shadow_border_set(ETK_SHADOW(shadow), 0);
   etk_shadow_shadow_set(ETK_SHADOW(shadow), ETK_SHADOW_OUTSIDE, ETK_SHADOW_ALL, 10, 2, 2, 200);
   etk_box_append(ETK_BOX(vbox), shadow, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Add the colored rectangle
   rect = evas_object_rectangle_add(evas);
   evas_object_color_set(rect, 255,100,100,255);
   evas_object_resize(rect, w, h);
   etk_evas_object = etk_evas_object_new();
   etk_evas_object_set_object(ETK_EVAS_OBJECT(etk_evas_object), rect);
   evas_object_show(rect);
   etk_widget_show_all(etk_evas_object);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_DOWN,
                                  _window_color_canvas_click,
                                  (void*)(long)color_button_enum);
   etk_container_add(ETK_CONTAINER(shadow), etk_evas_object);
   etk_widget_size_request_set(etk_evas_object, w, h);
   switch (color_button_enum)
   {
      case COLOR_OBJECT_RECT:
         RectColorObject = rect;
         break;
      case COLOR_OBJECT_TEXT:
         TextColorObject = rect;
         break;
      case COLOR_OBJECT_SHADOW:
         ShadowColorObject = rect;
         break;
      case COLOR_OBJECT_OUTLINE:
         OutlineColorObject = rect;
         break;
      case COLOR_OBJECT_CC1:
         ColorClassC1 = rect;
         break;
      case COLOR_OBJECT_CC2:
         ColorClassC2 = rect;
         break;
      case COLOR_OBJECT_CC3:
         ColorClassC3 = rect;
         break;
   }

   //Label
   if (label_text){
      label = etk_label_new(label_text);
      etk_object_properties_set(ETK_OBJECT(label), "xalign",0.5,NULL);
      etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   }
   return vbox;
}

void 
window_embed_position_set(void *position_data, int *x, int *y)
{
   ecore_evas_geometry_get(position_data, x, y, NULL, NULL);
}


/***   Callbacks   ***/
void
_window_delete_cb(Ecore_Evas *ee)
{
   etk_main_quit();
}

void
_window_resize_cb(Ecore_Evas *ecore_evas)
{
   Evas_Object *embed_object;
   int win_w, win_h;

   //Get window size
   ecore_evas_geometry_get(UI_ecore_MainWin, NULL, NULL, &win_w, &win_h);

   //Resize main edje interface
   evas_object_resize(edje_ui, win_w, win_h);

   //Resize tree
   embed_object = etk_embed_object_get(ETK_EMBED(UI_PartsTreeEmbed));
   evas_object_move(embed_object, 0, 55);
   evas_object_resize(embed_object, TREE_WIDTH, win_h - 55);

   //Resize Consolle
   evas_object_move(EV_Consolle, TREE_WIDTH + 5, win_h - 105);
   evas_object_resize(EV_Consolle, win_w - TREE_WIDTH - 10, 75);
}

void 
_window_edit_obj_signal_cb(void *data, Evas_Object *o, const char *sig, const char *src)
{
   /* Catch all the signal from the editing edje object */
   char buf[1024];

   consolle_count++;
   snprintf(buf, sizeof(buf), "[%d]  SIGNAL = '%s'     SOURCE = '%s'",
            consolle_count, sig, src);
   consolle_log(buf);
}

void
_window_edit_obj_click(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   int x, y;

   evas_pointer_output_xy_get(e, &x, &y);

   //o2 = evas_object_top_at_pointer_get(e);
   //Evas_List *l =	evas_objects_at_xy_get (e, ev->canvas.x, ev->canvas.y, 1, 1);
   //printf("CLIK %x [%d %d] num: %d\n", obj, ev->canvas.x, ev->canvas.y, evas_list_count(l));
}

Etk_Bool
_window_all_button_click_cb(Etk_Button *button, void *data)
{
   /* Unique callbacks for all the buttons in edje editor */
   Etk_String *text;
   const char *tween;
   Etk_Tree_Row *row, *next, *prev;
   Evas_List *icons, *l;

   switch ((int)(long)data)
      {
   case TOOLBAR_NEW:
      system("edje_editor &");
      break;
   case TOOLBAR_OPEN:
      dialog_filechooser_show(FILECHOOSER_OPEN);
      break;
   case TOOLBAR_SAVE:
      if (!etk_string_length_get(Cur.edj_file_name))
      {
         dialog_filechooser_show(FILECHOOSER_SAVE_EDJ);
         break;
      }

      edje_edit_save(edje_o);
      if (!ecore_file_cp(Cur.edj_temp_name->string, Cur.edj_file_name->string))
      {
         dialog_alert_show("<b>ERROR:<\b><br>Can't write file");
      }
      break;
   case TOOLBAR_SAVE_EDC:
      dialog_alert_show("Not yet reimplemented ;)");
      break;
   case TOOLBAR_SAVE_EDJ:
      dialog_filechooser_show(FILECHOOSER_SAVE_EDJ);
      break;
   case TOOLBAR_ADD:
      if (!etk_string_length_get(Cur.part))
         etk_widget_disabled_set(UI_AddStateButton, 1);
      else etk_widget_disabled_set(UI_AddStateButton, 0);
      etk_menu_popup(ETK_MENU(UI_AddMenu));
      break;
   case TOOLBAR_REMOVE:
      if (!etk_string_length_get(Cur.state))
         etk_widget_disabled_set(UI_RemoveStateButton, 1);
      else etk_widget_disabled_set(UI_RemoveStateButton, 0);
      if (!etk_string_length_get(Cur.part))
         etk_widget_disabled_set(UI_RemovePartButton, 1);
      else etk_widget_disabled_set(UI_RemovePartButton, 0);
      if (!etk_string_length_get(Cur.prog))
         etk_widget_disabled_set(UI_RemoveProgramButton, 1);
      else etk_widget_disabled_set(UI_RemoveProgramButton, 0);

      etk_menu_popup(ETK_MENU(UI_RemoveMenu));
      break;
   case TOOLBAR_QUIT:
      etk_main_quit();
      break;
   case TOOLBAR_MOVE_UP: //Lower
      if (!etk_string_length_get(Cur.part))
      {
         dialog_alert_show("You must select a part to lower");
         break;
      }
      if (!edje_edit_part_restack_below(edje_o, Cur.part->string))
         break;

      row = evas_hash_find(Parts_Hash, Cur.part->string);
      prev = etk_tree_row_prev_get(row);
      if (!prev) break;
      prev = etk_tree_row_prev_get(prev);

      Parts_Hash = evas_hash_del(Parts_Hash, NULL, row);
      etk_tree_row_delete(row);

      if (prev)
         row = tree_part_add(Cur.part->string, prev);
      else
         row = tree_part_add(Cur.part->string, (void*)1);
      etk_tree_row_select(row);
      //Reload the edje if needed
      if (edje_edit_part_type_get(edje_o, Cur.part->string) == EDJE_PART_TYPE_GROUP)
         reload_edje();
      break;

   case TOOLBAR_MOVE_DOWN: //Raise
      if (!etk_string_length_get(Cur.part))
      {
         dialog_alert_show("You must select a part to lower");
         break;
      }
      if (!edje_edit_part_restack_above(edje_o, Cur.part->string))
         break;

      row = evas_hash_find(Parts_Hash, Cur.part->string);
      next = etk_tree_row_next_get(row);
      if (!next) break;

      Parts_Hash = evas_hash_del(Parts_Hash, NULL, row);
      etk_tree_row_delete(row);
      row = tree_part_add(Cur.part->string, next);
      etk_tree_row_select(row);
      //Reload the edje if needed
      if (edje_edit_part_type_get(edje_o, Cur.part->string) == EDJE_PART_TYPE_GROUP)
         reload_edje();
      break;
   case IMAGE_BROWSER_SHOW:
      image_browser_show(1);
      break;
   case IMAGE_BROWSER_CLOSE:
      etk_widget_hide(ETK_WIDGET(UI_ImageBrowserWin));
      break;
   case TOOLBAR_IMAGE_BROWSER:
      image_browser_show(0);
      break;
   case TOOLBAR_SPECTRUM:
      spectra_window_show();
      break;
   case TOOLBAR_DATA:
      data_window_show();
      break;
   case TOOLBAR_COLORS:
      colors_window_show();
      break;
   case TOOLBAR_FONT_BROWSER:
      dialog_alert_show("Font Browser");
      break;
   case TOOLBAR_IMAGE_FILE_ADD:
      dialog_filechooser_show(FILECHOOSER_IMAGE);
      break;
   case TOOLBAR_FONT_FILE_ADD:
      dialog_filechooser_show(FILECHOOSER_FONT);
      break;
   case IMAGE_TWEEN_ADD:
      icons = etk_iconbox_icon_get_selected(ETK_ICONBOX(UI_ImageBrowserIconbox));
      for (l = icons; l; l = l->next)
         if ((tween = etk_iconbox_icon_label_get(l->data)))
            edje_edit_state_tween_add(edje_o, Cur.part->string,
                                      Cur.state->string, tween);
      if (icons)
      {
         evas_list_free(icons);
         image_tweenlist_populate();
         row = etk_tree_last_row_get(ETK_TREE(UI_ImageTweenList));
         etk_tree_row_select(row);
         etk_tree_row_scroll_to(row, ETK_FALSE);
      }
      else
      {
         dialog_alert_show("You must choose an image to add from the Image Browser");
      }
      break;
   case IMAGE_TWEEN_DELETE:
      //TODO delete the correct tween (not the first with that name)
      if (!etk_string_length_get(Cur.tween)) break;
      printf("REMOVE TWEEN %s\n", Cur.tween->string);
      edje_edit_state_tween_del(edje_o, Cur.part->string, Cur.state->string,
                                   Cur.tween->string);
      row = etk_tree_selected_row_get(ETK_TREE(UI_ImageTweenList));
      next = etk_tree_row_next_get(row);
      if (!next) 
         next = etk_tree_row_prev_get(row);
      if (next)
         etk_tree_row_select(next);
      else
      {
         Cur.tween = etk_string_clear(Cur.tween);
         etk_widget_disabled_set(UI_DeleteTweenButton, TRUE);
         etk_widget_disabled_set(UI_MoveDownTweenButton, TRUE);
         etk_widget_disabled_set(UI_MoveUpTweenButton, TRUE);
      }
      etk_tree_row_delete(row);
      break;
   case TOOLBAR_OPTIONS:
      etk_menu_popup(ETK_MENU(UI_OptionsMenu));
      //etk_menu_popup_at_xy (ETK_MENU(AddMenu), 10, 10);
      break;
   case TOOLBAR_OPTION_BG1:
      edje_object_signal_emit(edje_ui,"set_bg1","edje_editor");
      break;
    case TOOLBAR_OPTION_BG2:
      edje_object_signal_emit(edje_ui,"set_bg2","edje_editor");
      break;
   case TOOLBAR_OPTION_BG3:
      edje_object_signal_emit(edje_ui,"set_bg3","edje_editor");
      break;
   case TOOLBAR_OPTION_BGC:
      current_color_object = COLOR_OBJECT_BG;
      etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Interface Background Color");
      //etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c); //TODO get current class color values
      if (UI_ColorWin) etk_widget_show_all(UI_ColorWin);
      break;
   case TOOLBAR_OPTION_FOPA100:
      evas_object_color_set(EV_rel1_handler, 255,255, 255, 255);
      evas_object_color_set(EV_rel2_handler, 255, 255, 255, 255);
      evas_object_color_set(EV_focus_handler, 255, 255, 255, 255);
      break;
   case TOOLBAR_OPTION_FOPA50:
      evas_object_color_set(EV_rel1_handler, 128,128, 128, 128);
      evas_object_color_set(EV_rel2_handler, 128, 128, 128, 128);
      evas_object_color_set(EV_focus_handler, 128, 128, 128, 128);
      break;
   case TOOLBAR_OPTION_FOPA25:
      evas_object_color_set(EV_rel1_handler, 64,64, 64, 64);
      evas_object_color_set(EV_rel2_handler, 64, 64, 64, 64);
      evas_object_color_set(EV_focus_handler, 64, 64, 64, 64);
      break;
   case TOOLBAR_OPTION_FULLSCREEN:
      Cur.fullscreen = !Cur.fullscreen;
      ecore_evas_fullscreen_set(UI_ecore_MainWin, Cur.fullscreen);
      break;
   case TOOLBAR_PLAY:
      toolbar_play_button_toggle(-1);
      break;
   case TOOLBAR_DEBUG:
      //print_debug_info(FALSE);
      //ChangeGroup("New group");
      //edje_edit_group_add(edje_o, "dai cazzo");
      //on_AddMenu_item_activated(NULL, NEW_RECT);
      edje_edit_print_internal_status(edje_o);
      break;
   case IMAGE_TWEEN_UP:
      dialog_alert_show("Up not yet implemented.");
      break;
   case IMAGE_TWEEN_DOWN:
      dialog_alert_show("Down not yet implemented.");
      break;
   case RUN_PROG:
      if (etk_string_length_get(Cur.prog))
         edje_edit_program_run(edje_o, Cur.prog->string);
      break;
   case SAVE_SCRIPT:
      text = etk_textblock_text_get(ETK_TEXT_VIEW(UI_ScriptBox)->textblock,
                                    ETK_TRUE);
      dialog_alert_show("Script not yet implemented.");
      etk_object_destroy(ETK_OBJECT(text));
      break;
   default:
      break;
      }

   return ETK_TRUE;
}

void
_window_logo_key_press(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;
  
   printf("*** Logo receive key pressed\n");
   printf("   keyname: %s\n", ev->keyname);
   printf("   key: %s\n", ev->key);
   printf("   string: %s\n", ev->string);
   printf("   compose: %s\n", ev->compose);

   /* NOTE: To add new bindings you must add a keygrab for the key
      you want in create_main_window(). And remember to update the README */

   /* quit */
   if (!strcmp(ev->key, "q") &&
       evas_key_modifier_is_set(ev->modifiers, "Control"))
      etk_main_quit();

   /* fullscreen */
   else if (!strcmp(ev->key, "f") &&
            evas_key_modifier_is_set(ev->modifiers, "Control"))
   {    
      Cur.fullscreen = !Cur.fullscreen;
      ecore_evas_fullscreen_set(UI_ecore_MainWin, Cur.fullscreen); 	
   }

   /* save (TODO make some sort of feedback for the user)*/
   else if (!strcmp(ev->key, "s") &&
            evas_key_modifier_is_set(ev->modifiers, "Control"))
      _window_all_button_click_cb(NULL, (void *)TOOLBAR_SAVE);
}

void
_window_color_canvas_click(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Color c;
   printf("Clik Signal on ColorCanvas Emitted\n");
   if (UI_ColorWin) etk_widget_show_all(UI_ColorWin);
   current_color_object = (int)(long)data;

   etk_signal_block("color-changed", ETK_OBJECT(UI_ColorPicker),
                    ETK_CALLBACK(_dialog_colorpicker_change_cb), NULL);
   switch (current_color_object)
   {
      case COLOR_OBJECT_RECT:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Rectangle color");
         edje_edit_state_color_get(edje_o, Cur.part->string, Cur.state->string,
                                   &c.r,&c.g,&c.b,&c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_TEXT:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Text color");
         edje_edit_state_color_get(edje_o, Cur.part->string, Cur.state->string,
                                   &c.r,&c.g,&c.b,&c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_SHADOW:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Shadow color");
         edje_edit_state_color3_get(edje_o, Cur.part->string, Cur.state->string,
                                    &c.r,&c.g,&c.b,&c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_OUTLINE:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Outline color");
         edje_edit_state_color2_get(edje_o, Cur.part->string, Cur.state->string,
                                    &c.r,&c.g,&c.b,&c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_CC1:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Color Class: Standard Color");
         evas_object_color_get(ColorClassC1, &c.r, &c.g, &c.b, &c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_CC2:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Color Class: Color 2");
         evas_object_color_get(ColorClassC2, &c.r, &c.g, &c.b, &c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
      case COLOR_OBJECT_CC3:
         etk_window_title_set(ETK_WINDOW(UI_ColorWin), "Color Class: Color 3");
         evas_object_color_get(ColorClassC3, &c.r, &c.g, &c.b, &c.a);
         etk_colorpicker_current_color_set(ETK_COLORPICKER(UI_ColorPicker), c);
         break;
   }
   etk_signal_unblock("color-changed", ETK_OBJECT(UI_ColorPicker),
                      ETK_CALLBACK(_dialog_colorpicker_change_cb), NULL);
}

