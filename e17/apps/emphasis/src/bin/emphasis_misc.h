/** @file emphasis_misc.h */
#ifndef EMPHASIS_MISC_H_
#define EMPHASIS_MISC_H_

/**
 * @defgroup misc
 * @{
 */
#define evas_list_first(list) do{list=evas_list_prev(list);} while(evas_list_prev(list))

Evas_List *convert_rowlist_in_playlist_with_file(Evas_List *rowlist);
Evas_List *convert_rowlist_in_playlist_with_id(Evas_List *rowlist);
/* void mpd_data_full_free(MpdData * list); */
void emphasis_playlist_append_selected(Etk_Tree *tree, Emphasis_Type type);
void emphasis_playlist_search_and_delete(Etk_Tree *tree, char *str,
                                         Emphasis_Type type);

char *strdupnull(char *str);
void strescape(char *str);
Evas_List *evas_list_concatenate(Evas_List *head, Evas_List *tail);

void go_in_vbox(Etk_Widget *child, void *data);

void etk_container_inspect(Etk_Container *container, int lvl, int *to_trace);
char *etk_strescape(const char *str);


/* ETK functions++ */
Evas_List *etk_tree_selected_rows_get(Etk_Tree *tree);
Evas_List *etk_tree_unselected_rows_get(Etk_Tree *tree);
Etk_Bool etk_image_has_error(Etk_Image *widget);
void etk_textblock_cursor_visible_set(Etk_Textblock *tb, Etk_Bool visible);
Etk_Bool etk_textblock_cursor_visible_get(Etk_Textblock *tb);
Etk_Tree_Row* etk_tree_nth_row_get(Etk_Tree *tree, int n);

/** @} */
#endif /* EMPHASIS_MISC_H_ */
