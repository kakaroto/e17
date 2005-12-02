/** @file etk_stock.h */
#ifndef _ETK_STOCK_H_
#define _ETK_STOCK_H_

#include "etk_types.h"

/**
 * @defgroup Etk_Stock Etk_Stock
 * @{
 */

/* TODO: doc */
enum _Etk_Stock_Id
{
   ETK_STOCK_PROCESS_STOP,
   ETK_STOCK_DOCUMENT_OPEN,
   ETK_STOCK_DOCUMENT_SAVE,
   ETK_STOCK_DOCUMENT_SAVE_AS,
   ETK_STOCK_EDIT_CUT,
   ETK_STOCK_EDIT_COPY,
   ETK_STOCK_EDIT_PASTE,
   ETK_STOCK_DIALOG_CANCEL,
   ETK_NUM_STOCK_IDS
};

char *etk_stock_key_get(Etk_Stock_Id stock_id);
char *etk_stock_label_get(Etk_Stock_Id stock_id);

/** @} */

#endif
