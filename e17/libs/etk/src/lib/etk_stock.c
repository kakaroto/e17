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
   char *theme_name;
   char *label;
} Etk_Stock_Item;

static const Etk_Stock_Item _etk_stock_items[] =
{
   { NULL, NULL },
   { "actions/process-stop", "Stop" },
   { "actions/document-open", "Open" },
   { "actions/document-save", "Save" },
   { "actions/document-save-as", "Save As" },
   { "actions/edit-cut", "Cut" },
   { "actions/edit-copy", "Copy" },
   { "actions/edit-paste", "Paste" },
   { "actions/dialog-cancel", "Cancel" },
   { "actions/go-up", "Go Up" },
   { "actions/go-down", "Go Down" },
};

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the edje key corresponding to the stock id
 * @param stock_id a stock id
 * @return Returns the edje key corresponding to the stock id
 */
char *etk_stock_key_get(Etk_Stock_Id stock_id)
{
   if (stock_id < 0 || stock_id >= ETK_NUM_STOCK_IDS)
      return NULL;
   return _etk_stock_items[stock_id].theme_name;
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
