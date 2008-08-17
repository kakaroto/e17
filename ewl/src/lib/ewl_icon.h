/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_ICON_H
#define EWL_ICON_H

#include "ewl_stock.h"

/**
 * @addtogroup Ewl_Icon Ewl_Icon: An icon widget
 * @brief Describes a widget to layout and manipulate icons
 *
 * @remarks Inherits from Ewl_Stock.
 * @if HAVE_IMAGES
 * @image html Ewl_Icon_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_ICON_TYPE
 * The type name for the Ewl_Icon widget
 */
#define EWL_ICON_TYPE "icon"

/**
 * @def EWL_ICON_IS(w)
 * Returns TRUE if the widget is an Ewl_Icon, FALSE otherwise
 */
#define EWL_ICON_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_ICON_TYPE))

/**
 * @def EWL_ICON(icon)
 * Typecasts a pointer to an Ewl_Icon pointer.
 */
#define EWL_ICON(icon) ((Ewl_Icon *)icon)

/**
 * A widget to display and manipluate an icon
 */
typedef struct Ewl_Icon Ewl_Icon;

/**
 * @brief Inherits from Ewl_Stock and extends to provide for an icon layout
 */
struct Ewl_Icon
{
        Ewl_Stock stock;        /**< Inherit from Ewl_Box */
        Ewl_Widget *label;        /**< The icons label */
        Ewl_Widget *image;        /**< The icons image */
        Ewl_Widget *alt;        /**< The icons alt text */
        Ewl_Widget *extended;        /**< The icons extended information */

        char *label_text;        /**< The label text */

        unsigned int constrain;                /**< The image constrain value */

        Ewl_Icon_Type type;                /**< The icons type */
        Ewl_Icon_Part hidden;                /**< The hidden part */
        unsigned char editable:1;        /**< Is the icon editable? */
        unsigned char compress_label:1; /**< Should the label be compressed? */
        unsigned char complex_label:1;  /**< Should the label be a Ewl_Text? */
        unsigned char thumbnailing:1;   /**< Should the image be thumbnailed?*/
};

Ewl_Widget      *ewl_icon_new(void);
Ewl_Widget      *ewl_icon_simple_new(void);
int              ewl_icon_init(Ewl_Icon *icon);

void             ewl_icon_type_set(Ewl_Icon *icon, Ewl_Icon_Type type);
Ewl_Icon_Type    ewl_icon_type_get(Ewl_Icon *icon);

void             ewl_icon_part_hide(Ewl_Icon *icon, Ewl_Icon_Part part);

void             ewl_icon_image_set(Ewl_Icon *icon, const char *file,
                                                const char *key);
const char      *ewl_icon_image_file_get(Ewl_Icon *icon);

void             ewl_icon_thumbnailing_set(Ewl_Icon *icon, unsigned int thumb);
unsigned int     ewl_icon_thumbnailing_get(Ewl_Icon *icon);

void             ewl_icon_constrain_set(Ewl_Icon *icon, unsigned int val);
unsigned int     ewl_icon_constrain_get(Ewl_Icon *icon);

void             ewl_icon_alt_text_set(Ewl_Icon *icon, const char *txt);
const char      *ewl_icon_alt_text_get(Ewl_Icon *icon);

void             ewl_icon_label_set(Ewl_Icon *icon, const char *label);
const char      *ewl_icon_label_get(Ewl_Icon *icon);

void             ewl_icon_editable_set(Ewl_Icon *icon, unsigned int e);
unsigned int     ewl_icon_editable_get(Ewl_Icon *icon);

void             ewl_icon_label_compressed_set(Ewl_Icon *icon,
                                                unsigned int compress);
unsigned int     ewl_icon_label_compressed_get(Ewl_Icon *icon);

void             ewl_icon_label_complex_set(Ewl_Icon *icon,
                                                unsigned int complex_label);
unsigned int     ewl_icon_label_complex_get(Ewl_Icon *icon);

void             ewl_icon_extended_data_set(Ewl_Icon *icon, Ewl_Widget *ext);
Ewl_Widget      *ewl_icon_extended_data_get(Ewl_Icon *icon);

/*
 * Internal stuff
 */
void ewl_icon_cb_destroy(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

