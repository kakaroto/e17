#include "emphasis.h"
#include "emphasis_search.h"

void
emphasis_search_row_add(Emphasis_Player_Gui *player)
{
  Etk_Combobox_Item *item;
  Etk_Widget *hbox;
  Etk_Widget *combo = NULL;
  Etk_Widget *entry;
  Etk_Widget *button;
  Evas_List *children;

  combo = etk_combobox_new_default();
  item = etk_combobox_item_append(ETK_COMBOBOX(combo), "Artist");
  item = etk_combobox_item_append(ETK_COMBOBOX(combo), "Album");
  item = etk_combobox_item_append(ETK_COMBOBOX(combo), "Title");
  item = etk_combobox_item_append(ETK_COMBOBOX(combo), "Track");
  item = etk_combobox_item_append(ETK_COMBOBOX(combo), "Name");
  item = etk_combobox_item_append(ETK_COMBOBOX(combo), "Genre");
  item = etk_combobox_item_append(ETK_COMBOBOX(combo), "Date");
  item = etk_combobox_item_append(ETK_COMBOBOX(combo), "Composer");
  item = etk_combobox_item_append(ETK_COMBOBOX(combo), "Performer");
  item = etk_combobox_item_append(ETK_COMBOBOX(combo), "Comment");
  item = etk_combobox_item_append(ETK_COMBOBOX(combo), "Disc");
  item = etk_combobox_item_append(ETK_COMBOBOX(combo), "Filename");
  etk_widget_size_request_set(combo, 140, 0);

  entry = etk_entry_new();

  button = etk_button_new();
  etk_button_image_set(ETK_BUTTON(button), 
                       ETK_IMAGE(etk_image_new_from_stock(ETK_STOCK_LIST_REMOVE,
                                                          ETK_STOCK_SMALL)));

  hbox = etk_hbox_new(ETK_FALSE, 0);
  etk_box_append(ETK_BOX(hbox), combo, ETK_BOX_START, ETK_BOX_NONE, 10);
  etk_box_append(ETK_BOX(hbox), entry, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
  etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

  etk_widget_show_all(hbox);

  children =
   etk_container_children_get(ETK_CONTAINER(player->media.search_root));

  etk_box_insert_at(ETK_BOX(player->media.search_root), hbox,
                    ETK_BOX_START,
                    evas_list_count(children)-2, ETK_BOX_FILL, 0);
  evas_list_free(children);

  etk_signal_connect("clicked", ETK_OBJECT(button),
                     ETK_CALLBACK(cb_media_search_btn_remove_search_clicked),
                     player->media.search_root);
  etk_signal_connect("key_down", ETK_OBJECT(entry),
                     ETK_CALLBACK(cb_media_search_entry_text_changed),
                     player);
}

void
emphasis_search_tree_fill(Emphasis_Player_Gui *player, Evas_List *results)
{
  Etk_Tree *tree;
  Etk_Tree_Col *col1;
  Etk_Tree_Col *col2;
  Etk_Tree_Col *col3;
  Etk_Tree_Col *col4;
  Evas_List *list;
  Emphasis_Data *data;
  Emphasis_Song *song;

  list = results;
  tree = ETK_TREE(player->media.search_tree);
  col1 = etk_tree_nth_col_get(tree, 0);
  col2 = etk_tree_nth_col_get(tree, 1);
  col3 = etk_tree_nth_col_get(tree, 2);
  col4 = etk_tree_nth_col_get(tree, 3);

  etk_tree_clear(tree);
  etk_tree_freeze(tree);

  while (results)
    {
      data = evas_list_data(results);
      if (data->type == EMPHASIS_DATA_TYPE_SONG)
        {
          song = data->song;
          etk_tree_row_append(tree, NULL,
                               col1, song->artist, 
                               col2, song->album, 
                               col3, song->title, 
                               col4, song->file,
                               NULL);
        }
      results = evas_list_next(results);
    }
  etk_tree_thaw(tree);
  emphasis_list_free(list);
}
