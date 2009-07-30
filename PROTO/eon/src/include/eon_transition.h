/*
 * eon_transition.h
 *
 *  Created on: 24-jul-2009
 *      Author: jl
 */

#ifndef EON_TRANSITION_H_
#define EON_TRANSITION_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_TRANSITION_STEP_CHANGED "stepChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_TRANSITION_STEP;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Transition_Private Eon_Transition_Private;
struct _Eon_Transition
{
	Eon_Paint parent;
	Eon_Transition_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_transition_type_get(void);

EAPI void eon_transition_step_set(Eon_Transition *t, float step);
EAPI float eon_transition_step_get(Eon_Transition *t);

#endif /* EON_TRANSITION_H_ */
