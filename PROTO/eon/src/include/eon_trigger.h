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
#ifndef EON_TRIGGER_H_
#define EON_TRIGGER_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_TRIGGER_EVENT_CHANGED "eventChanged"
#define EON_TRIGGER_TARGET_CHANGED "targetChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_TRIGGER_EVENT;
extern Ekeko_Property_Id EON_TRIGGER_TARGET;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Trigger_Private Eon_Trigger_Private;
struct _Eon_Trigger_Object
{
	Eon_Style_Applier parent;
	Eon_Trigger_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Eon_Trigger_Object * eon_trigger_new(Eon_Document *d);
EAPI void eon_trigger_event_set(Eon_Trigger_Object *t, const char *event);
EAPI void eon_trigger_target_set(Eon_Trigger_Object *t, Ekeko_Object *o);

#endif /* EON_TRIGGER_H_ */
