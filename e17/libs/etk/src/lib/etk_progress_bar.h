/** @file etk_progress_bar.h */
#ifndef _ETK_PROGRESS_BAR_H_
#define _ETK_PROGRESS_BAR_H_

#include "etk_bin.h"
#include "etk_types.h"
#include "etk_stock.h"

/**
 * @defgroup Etk_Progress_Bar Etk_Progress_Bar
 * @{
 */

/** @brief Gets the type of a progress bar */
#define ETK_PROGRESS_BAR_TYPE       (etk_progress_bar_type_get())
/** @brief Casts the object to an Etk_Progress_Bar */
#define ETK_PROGRESS_BAR(obj)       (ETK_OBJECT_CAST((obj), ETK_PROGRESS_BAR_TYPE, Etk_Progress_Bar))
/** @brief Checks if the object is an Etk_Progress_Bar */
#define ETK_IS_PROGRESS_BAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_PROGRESS_BAR_TYPE))

/**
 * @enum Etk_Progress_Bar_Orientation
 * @brief Describes in which orientation the progress bar should move (left to right by default)
 */
enum _Etk_Progress_Bar_Orientation
{
   ETK_PROGRESS_BAR_LEFT_TO_RIGHT,
   ETK_PROGRESS_BAR_RIGHT_TO_LEFT
};   

/**
 * @struct Etk_Progress_Bar
 * @brief An Etk_Progress_Bar is a widget used to indicates the progress of a process
 */
struct _Etk_Progress_Bar
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   char *text;
   double fraction;
   double pulse_pos;
   double pulse_step;
   double filler_pulse_w;
   Etk_Progress_Bar_Orientation pulse_dir;
   Etk_Progress_Bar_Orientation orientation;
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
  
/** @} */

#endif
