/** @file etk_stock.h */
#ifndef _ETK_STOCK_H_
#define _ETK_STOCK_H_

#include "etk_types.h"

/**
 * @defgroup Etk_Stock Etk_Stock
 * @{
 */

enum _Etk_Stock_Size
{
   ETK_STOCK_SMALL,     /* 16x16 */
   ETK_STOCK_MEDIUM,    /* 22x22 */
   ETK_STOCK_BIG,       /* 48x48 */
};

/* TODO: doc */
enum _Etk_Stock_Id
{
   ETK_STOCK_NO_STOCK,
   ETK_STOCK_PROCESS_STOP,
   ETK_STOCK_DOCUMENT_OPEN,
   ETK_STOCK_DOCUMENT_SAVE,
   ETK_STOCK_DOCUMENT_SAVE_AS,
   ETK_STOCK_EDIT_CUT,
   ETK_STOCK_EDIT_COPY,
   ETK_STOCK_EDIT_PASTE,
   ETK_STOCK_DIALOG_CANCEL,
   ETK_STOCK_GO_UP,
   ETK_STOCK_GO_DOWN,
   ETK_NUM_STOCK_IDS
};

char *etk_stock_key_get(Etk_Stock_Id stock_id, Etk_Stock_Size size);
char *etk_stock_label_get(Etk_Stock_Id stock_id);

/** @} */

#endif
