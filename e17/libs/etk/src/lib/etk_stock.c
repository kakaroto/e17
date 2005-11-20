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
   { "close", "Close" },
   { "open",  "Open"  },
   { "save",  "Save"  },
   { "cut",   "Cut"  },
   { "copy",  "Copy"  },
   { "paste", "Paste"  }
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
