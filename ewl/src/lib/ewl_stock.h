/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_STOCK_H
#define EWL_STOCK_H

#include "ewl_box.h"

/**
 * @addtogroup Ewl_Stock Ewl_Stock: The Stock
 * @brief The stock class is an abstact class to provide the stock feature of
 * buttons and icons in a common place. This class inherits from the Ewl_Box
 * to allow for placing any other widget inside the stock.
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_Stock_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @internal
 * The stock function pointers.
 */
typedef struct Ewl_Stock_Funcs Ewl_Stock_Funcs;

/**
 * @def EWL_STOCK_TYPE
 * The type name for the Ewl_Stock widget
 */
#define EWL_STOCK_TYPE "stock"

/**
 * @def EWL_STOCK_IS(w)
 * Returns TRUE if the widget is an Ewl_Stock, FALSE otherwise
 */
#define EWL_STOCK_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_STOCK_TYPE))

/**
 * The stock structure.
 */
typedef struct Ewl_Stock Ewl_Stock;

/**
 * @def EWL_STOCK(stock)
 * Typecast a pointer to an Ewl_Stock pointer.
 */
#define EWL_STOCK(stock) ((Ewl_Stock *) stock)

/**
 * @def EWL_STOCK_LABEL_SET(f)
 * Typecasts a pointer to a label set pointer
 */
#define EWL_STOCK_LABEL_SET(f) ((Ewl_Stock_Label_Set)f)

/**
 * The label set function definition
 */
typedef void (*Ewl_Stock_Label_Set)(Ewl_Stock *s, const char *txt);

/**
 * @def EWL_STOCK_IMAGE_SET(f)
 * Typecasts a pointer to an image set pointer
 */
#define EWL_STOCK_IMAGE_SET(f) ((Ewl_Stock_Image_Set)f)

/**
 * The image set function definition
 */
typedef void (*Ewl_Stock_Image_Set)(Ewl_Stock *s, const char *file, const char *key);

/**
 * @def EWL_STOCK_TOOLTIP_SET(f)
 * Typecast to the tooltip set pointer
 */
#define EWL_STOCK_TOOLTIP_SET(f) ((Ewl_Stock_Tooltip_set)f)

/**
 * The tooltip set function definition
 */
typedef void (*Ewl_Stock_Tooltip_Set)(Ewl_Stock *s, const char *tip);

/**
 * @brief Inherits from Ewl_Box and provides the base for the other stock
 * icons
 */
struct Ewl_Stock
{
        Ewl_Box box;                         /**< Inherit from the box for adding widgets */
        Ewl_Stock_Type stock_type;        /**< The stock type of the stock */
        const Ewl_Stock_Funcs *stock_funcs;        /**< The stock functions */
};

/**
 * @brief The function callbacks that an inheriting widget needs to
 * implement
 */
struct Ewl_Stock_Funcs
{
        Ewl_Stock_Label_Set label_set;                /**< The label set function */
        Ewl_Stock_Image_Set image_set;                 /**< The image set function */
        Ewl_Stock_Tooltip_Set tooltip_set;        /**< The tooltip set function */
};


int              ewl_stock_init(Ewl_Stock *s);

void             ewl_stock_functions_set(Ewl_Stock *s,
                                const Ewl_Stock_Funcs * const funcs);

void             ewl_stock_type_set(Ewl_Stock *s, Ewl_Stock_Type stock);
Ewl_Stock_Type   ewl_stock_type_get(Ewl_Stock *s);

/**
 * @}
 */

#endif

