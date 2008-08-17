/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_progress_bar.h */
#ifndef _ETK_PROGRESS_BAR_H_
#define _ETK_PROGRESS_BAR_H_

#include "etk_types.h"
#include "etk_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

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
typedef enum
{
   ETK_PROGRESS_BAR_LEFT_TO_RIGHT,      /**< The progress bar grows from left to right */
   ETK_PROGRESS_BAR_RIGHT_TO_LEFT       /**< The progress bar grows from right to left */
} Etk_Progress_Bar_Direction;


/**
 * @brief @widget A widget used to indicates the progress of an operation
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
   Etk_Bool is_pulsing:1;
};


Etk_Type   *etk_progress_bar_type_get(void);
Etk_Widget *etk_progress_bar_new(void);
Etk_Widget *etk_progress_bar_new_with_text(const char *label);

void        etk_progress_bar_text_set(Etk_Progress_Bar *progress_bar, const char *label);
const char *etk_progress_bar_text_get(Etk_Progress_Bar *progress_bar);

void        etk_progress_bar_fraction_set(Etk_Progress_Bar *progress_bar, double fraction);
double      etk_progress_bar_fraction_get(Etk_Progress_Bar *progress_bar);

void        etk_progress_bar_pulse(Etk_Progress_Bar *progress_bar);
void        etk_progress_bar_pulse_step_set(Etk_Progress_Bar *progress_bar, double pulse_step);
double      etk_progress_bar_pulse_step_get(Etk_Progress_Bar *progress_bar);

void        etk_progress_bar_direction_set(Etk_Progress_Bar *progress_bar, Etk_Progress_Bar_Direction direction);
Etk_Progress_Bar_Direction etk_progress_bar_direction_get(Etk_Progress_Bar *progress_bar);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
