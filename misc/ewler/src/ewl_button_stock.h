#ifndef __EWL_BUTTON_STOCK_H__
#define __EWL_BUTTON_STOCK_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include "ewl_stock.h"

typedef struct Ewl_Button_Stock Ewl_Button_Stock;

#define EWL_BUTTON_STOCK(button) ((Ewl_Button_Stock *) button)

struct Ewl_Button_Stock
{
  Ewl_Button  button; /* Inherit from the button */
  Ewl_Widget *image_object; /* Here is the image */
};

Ewl_Widget *ewl_button_with_stock_new (char *stock_id);
int         ewl_button_with_stock_init(Ewl_Button_Stock *b, 
				       char       *stock_id);
  

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __EWL_BUTTON_STOCK_H__ */
