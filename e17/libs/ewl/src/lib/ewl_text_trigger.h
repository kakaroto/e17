/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_TEXT_TRIGGER_H
#define EWL_TEXT_TRIGGER_H

#include "ewl_base.h"
#include "ewl_text.h"
#include "ewl_text_context.h"

/**
 * @addtogroup Ewl_Text_Trigger Ewl_Text_Trigger: References portions of the
 * text that respond to actions.
 * @brief Defines a class for triggering actions within the text
 * @remarks Inherits from Ewl_Widget.
 *
 * @{
 */

/**
 * @def EWL_TEXT_TRIGGER_TYPE
 * The type name for the Ewl_Text_Trigger widget
 */
#define EWL_TEXT_TRIGGER_TYPE "trigger"

/**
 * @def EWL_TEXT_TRIGGER_IS(w)
 * Returns TRUE if the widget is an Ewl_Text_Trigger, FALSE otherwise
 */
#define EWL_TEXT_TRIGGER_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TEXT_TRIGGER_TYPE))

/**
 * A trigger in the text, used for selections and clickable text
 */
typedef struct Ewl_Text_Trigger Ewl_Text_Trigger;

/**
 * @brief Inherits from Ewl_Widget and extends to provide a trigger for the text
 * widget
 */
struct Ewl_Text_Trigger
{
        Ewl_Widget widget;                /**< Inherit from widget */
        Ewl_Text_Trigger_Type type;         /**< Trigger type */

        unsigned int char_pos;        /**< Trigger start position */
        unsigned int char_len;        /**< Trigger length */
        unsigned int char_base;        /**< Used for the selection. Start position */

        Ewl_Text *text_parent;        /**< The parent text area */
        Ecore_List *areas;        /**< The list of objects for the trigger */
};

/**
 * @def EWL_TEXT_TRIGGER(trigger)
 * Typecasts a pointer to an Ewl_Text_Trigger pointer
 */
#define EWL_TEXT_TRIGGER(trigger) ((Ewl_Text_Trigger *) trigger)

Ewl_Widget              *ewl_text_trigger_new(Ewl_Text_Trigger_Type type);
int                      ewl_text_trigger_init(Ewl_Text_Trigger *trigger,
                                                Ewl_Text_Trigger_Type type);

Ewl_Text_Trigger_Type    ewl_text_trigger_type_get(Ewl_Text_Trigger *t);

void                     ewl_text_trigger_start_pos_set(Ewl_Text_Trigger *t,
                                                unsigned int char_pos);
unsigned int             ewl_text_trigger_start_pos_get(Ewl_Text_Trigger *t);

void                     ewl_text_trigger_base_set(Ewl_Text_Trigger *t,
                                                unsigned int char_pos);
unsigned int             ewl_text_trigger_base_get(Ewl_Text_Trigger *t);

void                     ewl_text_trigger_length_set(Ewl_Text_Trigger *t,
                                                unsigned int char_len);
unsigned int             ewl_text_trigger_length_get(Ewl_Text_Trigger *t);

void                     ewl_text_trigger_area_add(Ewl_Text *t,
                                                Ewl_Text_Trigger *cur,
                                                int x, int y, int w, int h);
void                     ewl_text_trigger_areas_cleanup(Ewl_Text_Trigger *trig);

/*
 * Internal stuff
 */
void ewl_text_trigger_cb_show(Ewl_Widget *w, void *ev, void *data);
void ewl_text_trigger_cb_hide(Ewl_Widget *w, void *ev, void *data);
void ewl_text_trigger_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_text_trigger_cb_unrealize(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

