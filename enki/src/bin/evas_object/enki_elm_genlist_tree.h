/*
 * enki_elm_genlist_tree.h
 *
 *  Created on: May 26, 2011
 *      Author: watchwolf
 */

#ifndef ENKI_ELM_GENLIST_TREE_H_
#define ENKI_ELM_GENLIST_TREE_H_

#include <Elementary.h>

Elm_Genlist_Item *
enki_elm_genlist_item_menu_append(Evas_Object *edje,
                                  Elm_Genlist_Item_Class *itc,
                                  const void *data, Elm_Genlist_Item *parent,
                                  Evas_Smart_Cb func, const void *func_data);
Elm_Genlist_Item *
enki_elm_genlist_item_album_append(Evas_Object *edje,
                                   Elm_Genlist_Item_Class *itc,
                                   const void *data, Elm_Genlist_Item *parent,
                                   Evas_Smart_Cb func, const void *func_data);
Elm_Genlist_Item *
enki_elm_genlist_item_album_prepend(Evas_Object *edje,
                                    Elm_Genlist_Item_Class *itc,
                                    const void *data, Elm_Genlist_Item *parent,
                                    Evas_Smart_Cb func, const void *func_data);
Elm_Genlist_Item *
enki_elm_genlist_item_album_insert_after(Evas_Object *edje,
                                         Elm_Genlist_Item_Class *itc,
                                         const void *data,
                                         Elm_Genlist_Item *parent,
                                         Elm_Genlist_Item *relative,
                                         Evas_Smart_Cb func,
                                         const void *func_data);
Elm_Genlist_Item *
enki_elm_genlist_item_album_insert_after(Evas_Object *edje,
                                         Elm_Genlist_Item_Class *itc,
                                         const void *data,
                                         Elm_Genlist_Item *parent,
                                         Elm_Genlist_Item *after,
                                         Evas_Smart_Cb func,
                                         const void *func_data);
Elm_Genlist_Item *
enki_elm_genlist_item_collection_append(Evas_Object *edje,
                                        Elm_Genlist_Item_Class *itc,
                                        const void *data,
                                        Elm_Genlist_Item *parent,
                                        Evas_Smart_Cb func,
                                        const void *func_data);
Elm_Genlist_Item *
enki_elm_genlist_item_tag_append(Evas_Object *edje,
                                 Elm_Genlist_Item_Class *itc, const void *data,
                                 Elm_Genlist_Item *parent, Evas_Smart_Cb func,
                                 const void *func_data);
#endif /* ENKI_ELM_GENLIST_TREE_H_ */
