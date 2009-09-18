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
#ifndef EON_VALUE_H_
#define EON_VALUE_H_

extern Ekeko_Value_Type EON_PROPERTY_COORD;
extern Ekeko_Value_Type EON_PROPERTY_CLOCK;
extern Ekeko_Value_Type EON_PROPERTY_TRIGGER;
extern Ekeko_Value_Type EON_PROPERTY_MATRIX;
extern Ekeko_Value_Type EON_PROPERTY_COLOR;

typedef struct _Eon_Trigger
{
	Ekeko_Object *obj;
	char *event;
} Eon_Trigger;

typedef struct _Eon_Clock
{
	unsigned int seconds;
	unsigned int micro; /* 10^-6 */
} Eon_Clock;

typedef enum _Eon_Calc
{
	EON_CALC_LINEAR,
	EON_CALC_DISCRETE,
} Eon_Calc;


static inline void eon_value_coord_from(Ekeko_Value *v, Eon_Coord *coord)
{
	v->value.pointer_value = coord;
	v->type = EON_PROPERTY_COORD;
}

static inline void eon_value_matrix_from(Ekeko_Value *v, Enesim_Matrix *m)
{
	v->value.pointer_value = m;
	v->type = EON_PROPERTY_MATRIX;
}

static inline void eon_value_clock_from(Ekeko_Value *v, Eon_Clock *clock)
{
	v->value.pointer_value = clock;
	v->type = EON_PROPERTY_CLOCK;
}

static inline void eon_value_clock_seconds_from(Ekeko_Value *v, Eon_Clock *clock, unsigned int sec)
{
	clock->seconds = sec;
	clock->micro = 0;
	v->value.pointer_value = clock;
	v->type = EON_PROPERTY_CLOCK;
}

static inline void eon_value_clock_micro_from(Ekeko_Value *v, Eon_Clock *clock, unsigned int micro)
{
	clock->seconds = 0;
	clock->micro = micro;
	v->value.pointer_value = clock;
	v->type = EON_PROPERTY_CLOCK;
}

static inline void eon_value_trigger_from(Ekeko_Value *v, Eon_Trigger *t)
{
	v->value.pointer_value = t;
	v->type = EON_PROPERTY_TRIGGER;
}

static inline void eon_value_color_from(Ekeko_Value *v, Eon_Color c)
{
	v->value.int_value = c;
	v->type = EON_PROPERTY_COLOR;
}

#endif /* EON_VALUE_H_ */
