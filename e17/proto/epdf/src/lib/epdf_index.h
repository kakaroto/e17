#ifndef __EPDF_INDEX_H__
#define __EPDF_INDEX_H__


#include "epdf_forward.h"


/**
 * @file epdf_index.h
 * @defgroup Epdf_Index Epdf Index
 * @brief Functions that manage indexes of a PDF document
 * @ingroup Epdf
 *
 * Functions that manage indexes of a PDF document
 */


/**
 * Return a newly created Epdf_Index_Item object
 *
 * @return A pointer to a newly created Epdf_Index_Item
 *
 * Return a newly created Epdf_Index_Item object. This is
 * mainly used for internal usage. You surely don't need to use it
 *
 * @ingroup Epdf_Index
 */
Epdf_Index_Item * epdf_index_item_new ();

/**
 * Return the title of an item
 *
 * @param item The index item
 * @return The children of the item
 *
 * Return the title of the item @p item
 *
 * @ingroup Epdf_Index
 */
const char *epdf_index_item_title_get (Epdf_Index_Item *item);

/**
 * Return the children of an item
 *
 * @param item The index item
 * @return The children of the item, as a list
 *
 * Return a list of all the children of the item @p item (that is, the
 * subsection), or NULL if no child.
 *
 * @ingroup Epdf_Index
 */
Ecore_List *epdf_index_item_children_get (Epdf_Index_Item *item);

/**
 * Return the action kind of an item
 *
 * @param item The index item
 * @return The kind of action of the item
 *
 * Return the kind of action of the item @p item
 *
 * @ingroup Epdf_Index
 */
Epdf_Link_Action_Kind epdf_index_item_action_kind_get (Epdf_Index_Item *item);

/**
 * Return the destination page of an item
 *
 * @param document The document
 * @param item The index item
 * @return The destination page of the item
 *
 * Return the destination of the item @p item. The first page is @c 0
 *
 * @ingroup Epdf_Index
 */
int
epdf_index_item_page_get (Epdf_Document *document, Epdf_Index_Item *item);

/**
 * Return a newly created Epdf_Index object
 *
 * @param document The document
 * @return A pointer to a newly created Epdf_Index
 *
 * Return a newly created Epdf_Index object built from the
 * document @p document. It contains a tree of the index of the
 * documant (a kind of Table of Contents). The result must be freed with
 * epdf_index_delete()
 *
 * @ingroup Epdf_Index
 */
Ecore_List *epdf_index_new (Epdf_Document *document);

/**
 * Delete an Epdf_Index object
 *
 * @param index The index to delete
 *
 * Delete the Epdf_Index @p index that has been created
 * with epdf_index_new()
 *
 * @ingroup Epdf_Index
 *
 */
void epdf_index_delete (Ecore_List *index);


#endif /* __EPDF_INDEX_H__ */
