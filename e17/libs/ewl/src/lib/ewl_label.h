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
	char * text;            /**< The text set into the widget */
};

Ewl_Widget *ewl_label_new(char *text);
int         ewl_label_init(Ewl_Label *la, char *text);

void        ewl_label_text_set(Ewl_Label *la, char *text);
char  	   *ewl_label_text_get(Ewl_Label *la);

/*
 * Internally used callbacks
 */
void ewl_label_realize_cb(Ewl_Widget *w, void *ev, void *data);
void ewl_label_destroy_cb(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

