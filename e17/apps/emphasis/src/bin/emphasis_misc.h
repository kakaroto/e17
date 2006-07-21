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

/** @} */
#endif /* EMPHASIS_MISC_H_ */
