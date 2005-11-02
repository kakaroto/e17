/** @file etk_stock.h */
#ifndef _ETK_STOCK_H_
#define _ETK_STOCK_H_

#include "etk_types.h"

/**
 * @defgroup Etk_Stock Etk_Stock
 * @{
 */

enum _Etk_Stock_Id
{
   ETK_STOCK_CLOSE,
   ETK_STOCK_OPEN,
   ETK_STOCK_SAVE,
   ETK_NUM_STOCK_IDS
};

char *etk_stock_key_get(Etk_Stock_Id stock_id);
char *etk_stock_label_get(Etk_Stock_Id stock_id);

/** @} */

#endif
