/** @file etk_stock.c */
#include <stdlib.h>
#include <string.h>
#include "etk_stock.h"

/**
 * @addtogroup Etk_Stock
 * @{
 */

typedef struct _Etk_Stock_Item
{
   char *small_icon_key;
   char *medium_icon_key;
   char *big_icon_key;
   char *label;
} Etk_Stock_Item;

static const Etk_Stock_Item _etk_stock_items[] =
{
   { NULL, NULL },
   { "actions/process-stop_16", "actions/process-stop_22", "actions/process-stop_48", "Stop" },
   { "actions/document-open_16", "actions/document-open_22", "actions/document-open_48", "Open" },
   { "actions/document-save_16", "actions/document-save_22", "actions/document-save_48", "Save" },
   { "actions/document-save-as_16", "actions/document-save-as_22", "actions/document-save-as_48", "Save As" },
   { "actions/edit-cut_16", "actions/edit-cut_22", "actions/edit-cut_48", "Cut" },
   { "actions/edit-copy_16", "actions/edit-copy_22", "actions/edit-copy_48", "Copy" },
   { "actions/edit-paste_16", "actions/edit-paste_22", "actions/edit-paste_48", "Paste" },
   { "actions/process-stop_16", "actions/process-stop_22", "actions/process-stop_48", "Cancel" },
   { "actions/go-up_16", "actions/go-up_22", "actions/go-up_48", "Go Up" },
   { "actions/go-down_16", "actions/go-down_22", "actions/go-down_48", "Go Down" },
};

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the edje key corresponding to the stock id
 * @param stock_id a stock id
 * @param size the size of the stock icon
 * @return Returns the edje key corresponding to the stock id
 */
char *etk_stock_key_get(Etk_Stock_Id stock_id, Etk_Stock_Size size)
{
   if (stock_id < 0 || stock_id >= ETK_NUM_STOCK_IDS)
      return NULL;
   
   if (size == ETK_STOCK_SMALL)
      return _etk_stock_items[stock_id].small_icon_key;
   else if (size == ETK_STOCK_MEDIUM)
      return _etk_stock_items[stock_id].medium_icon_key;
   else
      return _etk_stock_items[stock_id].big_icon_key;
}

/**
 * @brief Gets the label corresponding to the stock id
 * @param stock_id a stock id
 * @return Returns the label corresponding to the stock id
 */
char *etk_stock_label_get(Etk_Stock_Id stock_id)
{
   if (stock_id < 0 || stock_id >= ETK_NUM_STOCK_IDS)
      return NULL;
   return _etk_stock_items[stock_id].label;
}

/** @} */
