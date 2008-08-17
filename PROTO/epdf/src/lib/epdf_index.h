#ifndef __EPDF_INDEX_H__
#define __EPDF_INDEX_H__


#include "epdf_forward.h"


/**
 * @file epdf_index.h
 *
 * @defgroup Epdf_Index Epdf Index
 *
 * @brief Functions that manage indexes of a PDF document
 *
 * Functions that manage indexes of a PDF document
 *
 * @ingroup Epdf
 *
 * @{
 */


/**
 * @brief Return a newly created Epdf_Index_Item object
 *
 * @return A pointer to a newly created Epdf_Index_Item
 *
 * Return a newly created Epdf_Index_Item object. This is
 * mainly used for internal usage. You surely don't need to use it
 */
Epdf_Index_Item * epdf_index_item_new ();

/**
 * @brief Return the title of an item
 *
 * @param item The index item
 * @return The children of the item
 *
 * Return the title of the item @p item
 */
const char *epdf_index_item_title_get (const Epdf_Index_Item *item);

/**
 * @brief Return the children of an item
 *
 * @param item The index item
 * @return The children of the item, as a list
 *
 * Return a list of all the children of the item @p item (that is, the
 * subsection), or NULL if no child.
 */
Ecore_List *epdf_index_item_children_get (const Epdf_Index_Item *item);

/**
 * @brief Return the action kind of an item
 *
 * @param item The index item
 * @return The kind of action of the item
 *
 * Return the kind of action of the item @p item
 */
Epdf_Link_Action_Kind epdf_index_item_action_kind_get (const Epdf_Index_Item *item);

/**
 * @brief Return the destination page of an item
 *
 * @param document The document
 * @param item The index item
 * @return The destination page of the item
 *
 * Return the destination of the item @p item. The first page is @c 0
 */
int
epdf_index_item_page_get (const Epdf_Document *document, const Epdf_Index_Item *item);

/**
 * @brief Return a newly created Epdf_Index object
 *
 * @param document The document
 * @return A pointer to a newly created Epdf_Index
 *
 * Return a newly created Epdf_Index object built from the
 * document @p document. It contains a tree of the index of the
 * documant (a kind of Table of Contents). The result must be freed with
 * epdf_index_delete()
 */
Ecore_List *epdf_index_new (const Epdf_Document *document);

/**
 * @brief Delete an Epdf_Index object
 *
 * @param index The index to delete
 *
 * Delete the Epdf_Index @p index that has been created
 * with epdf_index_new()
 *
 */
void epdf_index_delete (Ecore_List *index);

/**
 * @}
 */


#endif /* __EPDF_INDEX_H__ */
