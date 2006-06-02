/** @file etk_progress_bar.h */
#ifndef _ETK_PROGRESS_BAR_H_
#define _ETK_PROGRESS_BAR_H_

#include "etk_widget.h"
#include "etk_types.h"

/* TODO/FIXME list:
 * - Support format string "%p" for the text of the progress bar
 * - Change the theme because, when the progress is 0.0, it can be confused with an empty entry
 */

/**
 * @defgroup Etk_Progress_Bar Etk_Progress_Bar
 * @brief An Etk_Progress_Bar is a widget used to indicates the progress of an operation
 * @{
 */

/** Gets the type of a progress bar */
#define ETK_PROGRESS_BAR_TYPE       (etk_progress_bar_type_get())
/** Casts the object to an Etk_Progress_Bar */
#define ETK_PROGRESS_BAR(obj)       (ETK_OBJECT_CAST((obj), ETK_PROGRESS_BAR_TYPE, Etk_Progress_Bar))
/** Checks if the object is an Etk_Progress_Bar */
#define ETK_IS_PROGRESS_BAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_PROGRESS_BAR_TYPE))

/** @brief Possible growth directions for the progress bar (left to right by default) */
typedef enum Etk_Progress_Bar_Direction
{
   ETK_PROGRESS_BAR_LEFT_TO_RIGHT,      /**< The progress bar grows from left to right */
   ETK_PROGRESS_BAR_RIGHT_TO_LEFT       /**< The progress bar grows from right to left */
} Etk_Progress_Bar_Direction;

/**
 * @brief @widget The structure of a progress bar
 * @structinfo
 */
struct Etk_Progress_Bar
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   char *text;
   double fraction;
   double pulse_pos;
   double pulse_step;
   double filler_pulse_w;
   Etk_Progress_Bar_Direction pulse_dir;
   Etk_Progress_Bar_Direction direction;
   Etk_Bool is_pulsing;
};

Etk_Type *etk_progress_bar_type_get();
Etk_Widget *etk_progress_bar_new();
Etk_Widget *etk_progress_bar_new_with_text(const char *label);

void etk_progress_bar_text_set(Etk_Progress_Bar *progress_bar, const char *label);
const char *etk_progress_bar_text_get(Etk_Progress_Bar *progress_bar);

void etk_progress_bar_fraction_set(Etk_Progress_Bar *progress_bar, double fraction);
double etk_progress_bar_fraction_get(Etk_Progress_Bar *progress_bar);

void etk_progress_bar_pulse(Etk_Progress_Bar *progress_bar);
void etk_progress_bar_pulse_step_set(Etk_Progress_Bar *progress_bar, double pulse_step);
double etk_progress_bar_pulse_step_get(Etk_Progress_Bar *progress_bar);

void etk_progress_bar_direction_set(Etk_Progress_Bar *progress_bar, Etk_Progress_Bar_Direction direction);
Etk_Progress_Bar_Direction etk_progress_bar_direction_get(Etk_Progress_Bar *progress_bar);
  
/** @} */

#endif
