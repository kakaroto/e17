#ifndef __EWL_LABEL_H__
#define __EWL_LABEL_H__

/**
 * @file ewl_label.h
 * @defgroup Ewl_Label Label: A fully-themable text label
 * @brief Defines a class for a fullty-themable text label
 * 
 * @{
 */

/**
 * @themekey /label
 */

#define EWL_LABEL_TYPE "label"

typedef struct Ewl_Label Ewl_Label;

/**
 * @def EWL_LABEL(label)
 * Typecasts a pointer to an Ewl_Label pointer
 */
#define EWL_LABEL(label) ((Ewl_Label *)label)

/**
 * @struct Ewl_Label
 * Inherits from Ewl_Widget class
 */
struct Ewl_Label
{
	Ewl_Widget widget;      /**< Inherit from Ewl_Widget */
};

Ewl_Widget *ewl_label_new(void);
int         ewl_label_init(Ewl_Label *la);

void        ewl_label_text_set(Ewl_Label *la, const char *text);
const char *ewl_label_text_get(Ewl_Label *la);

/**
 * @}
 */

#endif

