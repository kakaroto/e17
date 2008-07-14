/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_ICONDIALOG_H
#define EWL_ICONDIALOG_H

/**
 * @addtogroup Ewl_Icondialog Ewl_Icondialog: A dialog window with icon
 * @brief Defines the Ewl_Icondialog class which extends the Ewl_Dialog class.
 *
 * @remarks Inherits from Ewl_Dialo.
 * @if HAVE_IMAGES
 * @image html Ewl_Icondialog_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /icondialog/file
 * @themekey /icondialog/group
 */

/**
 * @def EWL_ICONDIALOG_TYPE
 * The type name for the Ewl_Icondialog widget
 */
#define EWL_ICONDIALOG_TYPE "icondialog"

/**
 * @def EWL_ICONDIALOG_IS(w)
 * Returns TRUE if the widget is an Ewl_Icondialog, FALSE otherwise
 */
#define EWL_ICONDIALOG_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_ICONDIALOG_TYPE))

/**
 * The icondialog structure extends the dialog with an icon.
 */
typedef struct Ewl_Icondialog Ewl_Icondialog;

/**
 * @def EWL_ICONDIALOG(dialog)
 * Typecasts a pointer to an Ewl_Icondialog pointer.
 */
#define EWL_ICONDIALOG(dialog) ((Ewl_Icondialog *) dialog)

/**
 * @brief Inherits from Ewl_Dialog and extends to provide a icondialog widget.
 */
struct Ewl_Icondialog
{
        Ewl_Dialog dialog;        /**< Inherit from a window */

        Ewl_Widget *icon;        /**< The icon */
        Ewl_Widget *vbox;        /**< The box where messages are displayed */
};

Ewl_Widget      *ewl_icondialog_new(void);
int              ewl_icondialog_init(Ewl_Icondialog *dialog);

void             ewl_icondialog_icon_set(Ewl_Icondialog *d, 
                                                const char *theme_icon);

/**
 * @}
 */

#endif
