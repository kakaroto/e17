#ifndef __EWL_MYFILEDIALOG_H__
#define __EWL_MYFILEDIALOG_H__

/**
 * @defgroup Ewl_Filedialog_Stock Filedialog_Stock: A Dialog For Picking Files
 *
 * The filedialog_stock is intended to be used for a simple file chooser. It can be
 * placed inside any other container, and provides the ability to pack extra
 * buttons or widgets along the left side. It currently supports two types, an
 * Open and a Save dialog.
 *
 * The normal use of the filedialog_stock is to create a new one the first time an
 * event occurs that requires one. Setting a callback for
 * EWL_CALLBACK_VALUE_CHANGED, allows the programmer to determine when the
 * Open/Save buttons were chosen. If the event data on the callback is NULL,
 * Cancel was clicked, otherwise, the event data is a pointer to the chosen
 * file(s).
 *
 * @{
 */

/**
 * @themekey /filedialog_stock/file
 * @themekey /filedialog_stock/group
 */

/**
 * The Ewl_Filedialog_Stock provides a filedialog_stock
 */
typedef struct Ewl_Filedialog_Stock Ewl_Filedialog_Stock;

/**
 * @def EWL_FILEDIALOGSTOCK(fd)
 * Typecasts a pointer to an Ewl_Filedialog_Stock pointer.
 */
#define EWL_FILEDIALOGSTOCK(fd) ((Ewl_Filedialog_Stock *) fd)

/**
 * @struct Ewl_Filedialog_Stock
 * Creates a floating widget with different filedialog_stock components.
 */
struct Ewl_Filedialog_Stock
{
	Ewl_Dialog     dialog;          /**< the box container */
	Ewl_Filedialog_Type type; /**< define what type of filedialog_stock */
	
	Ewl_Widget     *selector;    /**< Ewl_Fileselector */
	Ewl_Widget     *path_label;  /**< label to display current path */
	Ewl_Widget     *entry;       /**< entry for manual input or current selected */
	
	Ewl_Widget     *decor_box;   /**< box to hold additional widgets */

	Ewl_Widget     *ok;          /**< open/save button */
	Ewl_Widget     *cancel;      /**< cancel button */
};


Ewl_Widget *ewl_filedialog_stock_new (Ewl_Filedialog_Type type);
void ewl_filedialog_stock_init (Ewl_Filedialog_Stock * fd, Ewl_Filedialog_Type type);
void ewl_filedialog_stock_set_directory(Ewl_Filedialog_Stock *fd, char *path);

/*
 * Internally used callback, override at your own risk.
 */
void ewl_filedialog_stock_change_labels_cb (Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_filedialog_stock_change_path_cb (Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_filedialog_stock_ok_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_filedialog_stock_cancel_cb(Ewl_Widget * w, void *ev_data, void *user_data);


/**
 * @}
 */

#endif /* __EWL_MYFILEDIALOG_H__ */
