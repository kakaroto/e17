#ifndef EWL_STOCK_H
#define EWL_STOCK_H

/**
 * @addtogroup Ewl_Stock Ewl_Stock: The Stock
 * @brief The stock class is an abstact class to provide the stock feature of
 * buttons and icons in a common place. This class inherits from the Ewl_Box 
 * to allow for placing any other widget inside the stock.
 *
 * @{
 */

/*
 * predefinition of the function structure
 */
typedef struct Ewl_Stock_Funcs Ewl_Stock_Funcs;

/**
 * @def EWL_STOCK_TYPE
 * The type name for the Ewl_Stock widget
 */
#define EWL_STOCK_TYPE "stock"

/**
 */
typedef struct Ewl_Stock Ewl_Stock;

/**
 * @def EWL_STOCK(stock)
 * Typecast a pointer to an Ewl_Stock pointer.
 */
#define EWL_STOCK(stock) ((Ewl_Stock *) stock)

/**
 * @brief An abstract Ewl_Widget to provide the base for Ewl_Icon and Ewl_Button
 */
struct Ewl_Stock
{
	Ewl_Box         box; 		/**< Inherit from the box for adding widgets */
	Ewl_Stock_Type	stock_type;	/**< The stock type of the stock */
	Ewl_Stock_Funcs *stock_funcs;/**< The stock functions */
};

struct Ewl_Stock_Funcs
{
	void (*label_set)(Ewl_Stock *s, const char *txt);
	void (*image_set)(Ewl_Stock *s, const char *file, const char *key);
};


int		 ewl_stock_init(Ewl_Stock *s);

void		 ewl_stock_type_set(Ewl_Stock *s, Ewl_Stock_Type stock);
Ewl_Stock_Type	 ewl_stock_type_get(Ewl_Stock *s);

/**
 * @}
 */

#endif

