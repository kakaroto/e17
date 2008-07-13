#include <string.h>
#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


void
spectra_window_show(void)
{
   etk_widget_show_all(UI_SpectrumWin);
   spectra_window_populate();
}

Etk_Widget*
spectra_window_create(void)
{
   Etk_Widget *vbox, *hbox, *hbox2;
   Etk_Tree_Col *col;
   Etk_Widget *button;

   //SpectraWin
   UI_SpectrumWin = etk_window_new();
   etk_window_title_set(ETK_WINDOW(UI_SpectrumWin), "Spectrum Editor");
   etk_window_resize(ETK_WINDOW(UI_SpectrumWin), 590, 310);
   etk_signal_connect("delete-event", ETK_OBJECT(UI_SpectrumWin),
                      ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_container_border_width_set(ETK_CONTAINER(UI_SpectrumWin), 5);

   //hbox
   hbox = etk_hbox_new(ETK_FALSE, 5);
   etk_container_add (ETK_CONTAINER(UI_SpectrumWin), hbox);

   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(hbox), vbox, ETK_BOX_START, ETK_BOX_NONE, 0);

   //UI_SpectrumList
   UI_SpectrumList = etk_tree_new();
   etk_tree_mode_set(ETK_TREE(UI_SpectrumList), ETK_TREE_MODE_LIST);
   etk_widget_size_request_set(UI_SpectrumList, 200, 200);
   etk_tree_headers_visible_set(ETK_TREE(UI_SpectrumList), TRUE);
   col = etk_tree_col_new(ETK_TREE(UI_SpectrumList), "Spectrum", 130, 0.0);
   etk_tree_col_model_add(col, etk_tree_model_image_new());
   etk_tree_col_model_add(col, etk_tree_model_text_new());
   etk_tree_build(ETK_TREE(UI_SpectrumList));
   etk_box_append(ETK_BOX(vbox), UI_SpectrumList, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_signal_connect("row-selected", ETK_OBJECT(UI_SpectrumList),
                      ETK_CALLBACK(_spectra_list_row_selected_cb), NULL);

   //hbox2
   hbox2 = etk_hbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(vbox), hbox2);

   //AddSpectraButton
   button = etk_button_new_from_stock(ETK_STOCK_LIST_ADD);
   etk_button_style_set(ETK_BUTTON(button), ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(button), 
                      ETK_CALLBACK(_spectra_add_button_click_cb), NULL);
   etk_box_append(ETK_BOX(hbox2), button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //DelSpectraButton
   button = etk_button_new_from_stock(ETK_STOCK_LIST_REMOVE);
   etk_button_style_set(ETK_BUTTON(button), ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(button), 
                      ETK_CALLBACK(_spectra_del_button_click_cb), NULL);
   etk_box_append(ETK_BOX(hbox2), button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //vbox
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(hbox), vbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_SpectraWidget
   UI_SpectraWidget = spectra_widget_new(etk_widget_toplevel_evas_get(UI_SpectrumWin));
   etk_box_append(ETK_BOX(vbox), UI_SpectraWidget, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);

   return UI_SpectrumWin;
}

void
spectra_window_populate(void)
{
   Evas_List *spectrum, *l;
   Etk_Tree_Col *col;
   Etk_Tree_Row *row;
   char *name;

   col = etk_tree_nth_col_get(ETK_TREE(UI_SpectrumList), 0);

   etk_tree_clear(ETK_TREE(UI_SpectrumList));

   spectrum = edje_edit_spectrum_list_get(edje_o);
   for (l = spectrum; l; l = l->next)
      etk_tree_row_append(ETK_TREE(UI_SpectrumList), NULL,
                          col, EdjeFile, "SPECTRA.PNG", l->data,
                          NULL);

   edje_edit_string_list_free(spectrum);

   //Select first row in the tree
   row = etk_tree_first_row_get(ETK_TREE(UI_SpectrumList));
   etk_tree_row_select(row);
   etk_tree_row_scroll_to(row, ETK_FALSE);

   //Set Current
   etk_tree_row_fields_get(row, col, NULL, NULL, &name, NULL);
   etk_string_set(Cur.spectra, name);
}

void //TODO this should be done as a new Etk_Event with a callback
spectra_window_gradient_changed(Etk_Widget *spectra)
{
   int num, i;

   //printf("SPECTRA ChANGED\n");

   if (!etk_string_length_get(Cur.spectra)) return;

   num = spectra_widget_colors_num_get(spectra);
   if (num != edje_edit_spectra_stop_num_get(edje_o, Cur.spectra->string))
      edje_edit_spectra_stop_num_set(edje_o, Cur.spectra->string, num);

   //Set all stops
   for (i = 0; i < num; i++)
   {
      int r, g, b, a, d;
      spectra_widget_color_get(spectra, i, &r, &g, &b, &a, &d);
      edje_edit_spectra_stop_color_set(edje_o, Cur.spectra->string, i, r, g, b, a, d);
      //printf("%d %d %d %d %d\n", r, g, b, a, d);
   }
}

void //TODO this should be done as a new Etk_Event with a callback
spectra_window_gradient_name_changed(Etk_Widget *spectra)
{
   Etk_Tree_Row * row;
   Etk_Tree_Col *col;
   const char* name;

   if (!etk_string_length_get(Cur.spectra)) return;

   //Set name
   name = spectra_widget_name_get(spectra);
   if (!name || strlen(name) < 1) return;
   edje_edit_spectra_name_set(edje_o, Cur.spectra->string, name);
   etk_string_set(Cur.spectra, name);

   //Set the new name in the tree
   col = etk_tree_nth_col_get(ETK_TREE(UI_SpectrumList), 0);
   row = etk_tree_selected_row_get(ETK_TREE(UI_SpectrumList));
   etk_tree_row_fields_set(row, ETK_FALSE,
                           col, EdjeFile, "GRAD.PNG", name, NULL);
}

Etk_Bool
_spectra_list_row_selected_cb(Etk_Tree *tree, Etk_Tree_Row *row, void *data)
{
   Etk_Tree_Col *col;
   const char *name;
   int i, num;

   col = etk_tree_nth_col_get(ETK_TREE(UI_SpectrumList), 0);
   etk_tree_row_fields_get(row, col, NULL, NULL, &name, NULL);

   if (!name) return ETK_TRUE;

   spectra_widget_name_set(UI_SpectraWidget, name);
   spectra_widget_color_clear_all(UI_SpectraWidget);
   num = edje_edit_spectra_stop_num_get(edje_o, name);

   for (i = 0; i < num ; i++)
   {
      int r, g, b, a, d;
      edje_edit_spectra_stop_color_get(edje_o, name, i, &r, &g, &b, &a, &d);
      spectra_widget_color_add(UI_SpectraWidget, r, g, b, a, d);
   }

   etk_string_set(Cur.spectra, name);

   return ETK_TRUE;
}

Etk_Bool
_spectra_add_button_click_cb(Etk_Button *button, void *data)
{
   char buf [1024];
   int i = 2;

   //Generate a unique name
   strcpy(buf, "New spectra");
   while (!edje_edit_spectra_add(edje_o, buf) && i < 100)
      snprintf(buf, 1024, "New spectra %d", i++);
   if (i >= 100) return ETK_TRUE;

   //Set 3 default stops
   edje_edit_spectra_stop_num_set(edje_o, buf, 3);
   edje_edit_spectra_stop_color_set(edje_o, buf, 0, 255, 0, 0, 255, 10);
   edje_edit_spectra_stop_color_set(edje_o, buf, 1, 0, 255, 0, 255, 10);
   edje_edit_spectra_stop_color_set(edje_o, buf, 2, 0, 0, 255, 255, 10);

   spectra_window_populate();

   //Select last row in the tree
   Etk_Tree_Row *row;
   row = etk_tree_last_row_get(ETK_TREE(UI_SpectrumList));
   etk_tree_row_select(row);
   etk_tree_row_scroll_to(row, ETK_FALSE);
   
   gradient_spectra_combo_populate();
   gradient_frame_update();

   return ETK_TRUE;
}

Etk_Bool
_spectra_del_button_click_cb(Etk_Button *button, void *data)
{
   if (!etk_string_length_get(Cur.spectra)) return ETK_TRUE;

   edje_edit_spectra_del(edje_o, Cur.spectra->string);
   spectra_window_populate();
   gradient_spectra_combo_populate();
   gradient_frame_update();

   return ETK_TRUE;
}


