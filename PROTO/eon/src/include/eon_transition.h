/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
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
