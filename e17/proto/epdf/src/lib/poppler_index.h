#ifndef __POPPLER_INDEX_H__
#define __POPPLER_INDEX_H__


#include "poppler_forward.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Return a newly created Evas_Poppler_Index_Item object
 *
 * @return A pointer to a newly created Evas_Poppler_Index_Item
 *
 * Return a newly created Evas_Poppler_Index_Item object. This is
 * mainly used for internal usage. You surely don't need to use it
 */
Evas_Poppler_Index_Item * evas_poppler_index_item_new ();

/**
 * Return the title of an item
 *
 * @param item The index item
 * @return The children of the item
 *
 * Return the title of the item @p item
 */
const char *evas_poppler_index_item_title_get (Evas_Poppler_Index_Item *item);

/**
 * Return the children of an item
 *
 * @param item The index item
 * @return The children of the item, as a list
 *
 * Return a list of all the children of the item @p item (that is, the
 * subsection), or NULL if no child.
 */
Ecore_List *evas_poppler_index_item_children_get (Evas_Poppler_Index_Item *item);

/**
 * Return the action kind of an item
 *
 * @param item The index item
 * @return The kind of action of the item
 *
 * Return the kind of action of the item @p item
 */
Evas_Poppler_Link_Action_Kind evas_poppler_index_item_action_kind_get (Evas_Poppler_Index_Item *item);

/**
 * Return the destination page of an item
 *
 * @param document The document
 * @param item The index item
 * @return The destination page of the item
 *
 * Return the destination of the item @p item. The first page is @c 0
 */
int
evas_poppler_index_item_page_get (Evas_Poppler_Document *document, Evas_Poppler_Index_Item *item);

/**
 * Return a newly created Evas_Poppler_Index object
 *
 * @param document The document
 * @return A pointer to a newly created Evas_Poppler_Index
 *
 * Return a newly created Evas_Poppler_Index object built from the
 * document @p document. It contains a tree of the index of the
 * documant (a kind of Table of Contents). The result must be freed with
 * evas_poppler_index_delete
 */
Ecore_List *evas_poppler_index_new (Evas_Poppler_Document *document);

/**
 * Delete an Evas_Poppler_Index object
 *
 * @param index The index to delete
 *
 * Delete the Evas_Poppler_Index @p index that has been created
 * with evas_poppler_index_new
 * 
 */
void evas_poppler_index_delete (Ecore_List *index);


#ifdef __cplusplus
}
#endif


#endif /* __POPPLER_INDEX_H__ */
