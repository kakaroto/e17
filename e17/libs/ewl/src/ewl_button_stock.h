#ifndef __EWL_BUTTON_STOCK_H__
#define __EWL_BUTTON_STOCK_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @defgroup Ewl_Button_Stock Button: A Button with a Stock Icon in it.
 * @brief The button_stock class is a button with an stock icon or a
 * label. This class inherits from the Ewl_Button. Stock icons are
 * pre-defined and are referenced by an Id.
 *
 * @{
 */

  /* Stock Id */
  
#define EWL_STOCK_OK     "stock_ok"
#define EWL_STOCK_APPLY  "stock_apply"
#define EWL_STOCK_CANCEL "stock_cancel"
#define EWL_STOCK_OPEN   "stock_open"
#define EWL_STOCK_SAVE   "stock_save"


typedef struct Ewl_Stock_Item Ewl_Stock_Item;
  
struct Ewl_Stock_Item
{
  char *stock_id;
  char *label;
};

static const Ewl_Stock_Item builtin_items [] = 
  {
    { EWL_STOCK_OK,     "ok" },
    { EWL_STOCK_APPLY,  "apply" },
    { EWL_STOCK_CANCEL, "cancel" },
    { EWL_STOCK_OPEN,   "open" },
    { EWL_STOCK_SAVE,   "save" }
  };


/**
 * The button_stck provides a simple wrapper for creating a button
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
 * label, which set the label of the button.
 */
struct _Ewl_Button_Stock
{
  Ewl_Button  button; /* Inherit from the button */
  Ewl_Widget *image_object; /* Here is the image */
};

Ewl_Widget *ewl_button_with_stock_new (char *stock_id);
int         ewl_button_with_stock_init(Ewl_Button_Stock *b, 
				       char             *stock_id);

/**
 * @}
 */
  

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __EWL_BUTTON_STOCK_H__ */
