#ifndef __EWL_BUTTON_STOCK_H__
#define __EWL_BUTTON_STOCK_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @defgroup Ewl_Button_Stock Button_Stock: A Button with a Stock Icon in it.
 * @brief The button_stock class is a button with a stock icon or a
 * label. This class inherits from the Ewl_Button. Stock icons are
 * pre-defined and are referenced by an Id.
 *
 * @{
 */

  /* Stock Id */
  
#define EWL_STOCK_OK          "stock_ok"
#define EWL_STOCK_APPLY       "stock_apply"
#define EWL_STOCK_CANCEL      "stock_cancel"
#define EWL_STOCK_OPEN        "stock_open"
#define EWL_STOCK_PAUSE       "stock_pause"
#define EWL_STOCK_PLAY        "stock_play"
#define EWL_STOCK_SAVE        "stock_save"
#define EWL_STOCK_STOP        "stock_stop"
#define EWL_STOCK_REWIND      "stock_rewind"
#define EWL_STOCK_FASTFORWARD "stock_fastforward"
#define EWL_STOCK_QUIT        "stock_quit"

typedef struct Ewl_Stock_Item Ewl_Stock_Item;
  
struct Ewl_Stock_Item
{
  char *stock_id;
  char *label;
};


/**
 * The button_stock provides a simple wrapper for creating a button
 * with predefined icons.
 */
typedef struct _Ewl_Button_Stock Ewl_Button_Stock;

/**
 * @def EWL_BUTTON_STOCK(button)
 * Typecast a pointer to an Ewl_Button_Stock pointer.
 */
#define EWL_BUTTON_STOCK(button) ((Ewl_Button_Stock *) button)

/**
 * @struct _Ewl_Button_Stock
 * @brief A button with a label, and eventually an image. A convenient
 * way to add the image is the use of stock icons. Stock icons are
 * pre-defined icons, referenced by a Id. These icons have also a
 * label, which set the label of the button and an Id in order to
 * identify a response (see ewl_dialog_set_button()).
 */
struct _Ewl_Button_Stock
{
  Ewl_Button  button;       /* Inherit from the button */
  Ewl_Widget *image_object; /* Here is the image */

  int         response_id;  /* the response Id */
};

Ewl_Widget *ewl_button_stock_new         (char *stock_id);
Ewl_Widget *ewl_button_stock_new_with_id (char *stock_id,
					  int   response_id);

int         ewl_button_stock_init        (Ewl_Button_Stock *b, 
					  char *stock_id);

/**
 * @}
 */
  

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __EWL_BUTTON_STOCK_H__ */
