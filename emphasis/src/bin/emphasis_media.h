#ifndef EMPHASIS_MEDIA_H_
#define EMPHASIS_MEDIA_H_

void emphasis_tree_mlib_init(Emphasis_Player_Gui *player, Emphasis_Type type);

void emphasis_tree_mlib_set(Etk_Tree *tree, Evas_List *list,
                            MpdDataType mpd_type, char *tag);

void emphasis_tree_mlib_append(Etk_Tree *tree, Evas_List *list,
                               MpdDataType mpd_type, char *tag);

void emphasis_tree_pls_set(Etk_Tree *tree, Evas_List *playlist);

void emphasis_pls_mark_current(Etk_Tree *tree, int id);

void emphasis_pls_list_init(Emphasis_Player_Gui *player);

#define LABEL_MLIB_ALL "<b>All</b>"

#endif /* EMPHASIS_MEDIA_H_ */
