#ifndef __POPPLER_PAGE_TRANSITION_H__
#define __POPPLER_PAGE_TRANSITION_H__


#include "poppler_enum.h"
#include "poppler_forward.h"

/**
 * Delete an Evas_Poppler_Page_Transition object
 * @param transition The page transition to delete
 *
 * Delete an Evas_Poppler_Page_Transition object @p transition
 */
void evas_poppler_page_transition_delete (Evas_Poppler_Page_Transition *transition);

/**
 * Return the type of a transition
 *
 * @param transition The page transition
 * @return The type of the transition
 *
 * Return the type of the transition @p transition, as an
 * Evas_Poppler_Page_Transition_Type 
 */
Evas_Poppler_Page_Transition_Type evas_poppler_page_transition_type_get (Evas_Poppler_Page_Transition *transition);

/**
 * Return the duration of a transition
 *
 * @param transition The page transition
 * @return The duration of the transition
 *
 * Return the duration of the transition @p transition
 */
int evas_poppler_page_transition_duration_get (Evas_Poppler_Page_Transition *transition);

/**
 * Return the alignment of a transition
 *
 * @param transition The page transition
 * @return The alignment of the transition
 *
 * Return the alignment of the transition @p transition
 */
Evas_Poppler_Page_Transition_Alignment evas_poppler_page_transition_alignment_get (Evas_Poppler_Page_Transition *transition);

/**
 * Return the direction of a transition
 *
 * @param transition The page transition
 * @return The direction of a transition
 *
 * Return the direction of the transition @p transition
 */
Evas_Poppler_Page_Transition_Direction evas_poppler_page_transition_direction_get (Evas_Poppler_Page_Transition *transition);

/**
 * Return the angle of a transition
 *
 * @param transition The page transition
 * @return The angle of a transition
 *
 * Return the angle of the transition @p transition
 */
int evas_poppler_page_transition_angle_get (Evas_Poppler_Page_Transition *transition);

/**
 * Return the scale of a transition
 *
 * @param transition The page transition
 * @return The scale of a transition
 *
 * Return the scale of the transition @p transition
 */
double evas_poppler_page_transition_scale_get (Evas_Poppler_Page_Transition *transition);

/**
 * Whether the transition is rectangular or not
 *
 * @param transition The page transition 
 * @return 1 if the transition is rectangular, 0 otherwise
 *
 * Whether the transition is rectangular or not
 */
unsigned char evas_poppler_page_transition_is_rectangular_get (Evas_Poppler_Page_Transition *transition);


#endif /* __POPPLER_PAGE_TRANSITION_H__ */
