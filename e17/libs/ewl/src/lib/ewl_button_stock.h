#ifndef __EWL_BUTTON_STOCK_H__
#define __EWL_BUTTON_STOCK_H__

/**
 * @file ewl_button_stock.h
 * @defgroup Ewl_Button_Stock Button_Stock: A Button with a Stock Icon in it.
 * @brief The button_stock class is a button with a stock icon or a
 * label. This class inherits from the Ewl_Button. Stock icons are
 * pre-defined and are referenced by an Id.
 *
 * @{
 */

/**
 * @themekey /stock_ok/file
 * @themekey /stock_ok/group
 *
 * @themekey /stock_apply/file
 * @themekey /stock_apply/group
 *
 * @themekey /stock_cancel/file
 * @themekey /stock_cancel/group
 *
 * @themekey /stock_open/file
 * @themekey /stock_open/group
 *
 * @themekey /stock_pause/file
 * @themekey /stock_pause/group
 *
 * @themekey /stock_play/file
 * @themekey /stock_play/group
 *
 * @themekey /stock_save/file
 * @themekey /stock_save/group
 *
 * @themekey /stock_stop/file
 * @themekey /stock_stop/group
 *
 * @themekey /stock_rewind/file
 * @themekey /stock_rewind/group
 *
 * @themekey /stock_fastforward/file
 * @themekey /stock_fastforward/group
 *
 * @themekey /stock_quit/file
 * @themekey /stock_quit/group
 *
 * @themekey /stock_arrow_up/file
 * @themekey /stock_arrow_up/group
 *
 * @themekey /stock_home/file
 * @themekey /stock_home/group
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
#define EWL_STOCK_ARROW_UP    "stock_arrow_up"
#define EWL_STOCK_HOME        "stock_home"

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
 * identify a response (see ewl_dialog_button_set()).
 */
struct _Ewl_Button_Stock
{
  Ewl_Button  button;       /* Inherit from the button */
  Ewl_Widget *image_object; /* Here is the image */

  char       *stock_id;     /* Copy of set stock id */
  int         response_id;  /* the response Id */
};

Ewl_Widget *ewl_button_stock_new(void);

int         ewl_button_stock_init(Ewl_Button_Stock *b);
void        ewl_button_stock_id_set(Ewl_Button_Stock *b, char *stock_id);
char       *ewl_button_stock_id_get(Ewl_Button_Stock *b);
void        ewl_button_stock_response_id_set(Ewl_Button_Stock *b,
					     int response_id);
int         ewl_button_stock_response_id_get(Ewl_Button_Stock *b);

/*
 * Internally used callbacks, override at your own risk.
 */

void ewl_button_stock_click_cb (Ewl_Widget *w, void *ev, void *data);
void ewl_button_stock_destroy_cb(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */
  
#endif /* __EWL_BUTTON_STOCK_H__ */
