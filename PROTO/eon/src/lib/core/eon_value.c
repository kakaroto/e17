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
#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/*                                  Coord                                     *
 *============================================================================*/
/* val = pointer to private->coord
 * v = Ekeko_Value without pointer set
 */
void * _coord_create(void)
{
	return malloc(sizeof(Eon_Coord));
}
void _coord_pointer_from(Ekeko_Value *v, Eon_Coord *val)
{
	Eon_Coord *cv = v->value.pointer_value;

	*cv = *val;
}
/* ptr = memory address to private->coord
 * v = value with pointer set
 */
void _coord_pointer_to(Ekeko_Value *v, Eon_Coord *val)
{
	Eon_Coord *cv = v->value.pointer_value;

	*val = *cv;
}
Eina_Bool _coord_cmp(Eon_Coord *a, Eon_Coord *b)
{
	if ((a->value == b->value) && (a->type == b->type))
		return EINA_FALSE;
	return EINA_TRUE;
}

void _coord_free(Eon_Coord *v)
{
	free(v);
}
/*                                  Clock                                     *
 *============================================================================*/
void * _clock_create(void)
{
	return malloc(sizeof(Eon_Clock));
}

void _clock_pointer_from(Ekeko_Value *v, Eon_Clock *val)
{
	Eon_Clock *cv = v->value.pointer_value;

	*cv = *val;
}

void _clock_pointer_to(Ekeko_Value *v, Eon_Clock *val)
{
	Eon_Clock *cv = v->value.pointer_value;
	*val = *cv;
}

Eina_Bool _clock_cmp(Eon_Clock *a, Eon_Clock *b)
{
	if ((a->seconds == b->seconds) && (a->micro == b->micro))
		return EINA_FALSE;
	return EINA_TRUE;
}

void _clock_free(Eon_Clock *v)
{
	free(v);
}

/*                                 Trigger                                    *
 *============================================================================*/
void * _trigger_create(void)
{
	return malloc(sizeof(Eon_Trigger));
}

void _trigger_pointer_from(Ekeko_Value *v, Eon_Trigger *t)
{
	Eon_Trigger *cv = v->value.pointer_value;

	*cv = *t;
}

void _trigger_pointer_to(Ekeko_Value *v, Eon_Trigger *t)
{
	Eon_Trigger *cv = v->value.pointer_value;
	*t = *cv;
#if 0
	Eon_Trigger *cv, *cval;

	cval = val;
	cv = v->value.pointer_value = malloc(sizeof(Eon_Trigger));
	if (cval->event)
		cv->event = strdup(cval->event);
	cv->obj = cval->obj;
#endif
}


Eina_Bool _trigger_cmp(Eon_Trigger *a, Eon_Trigger *b)
{
	if ((a->obj == b->obj) && (!strcmp(a->event, b->event)))
		return EINA_FALSE;
	return EINA_TRUE;
}

void _trigger_free(Eon_Trigger *t)
{
	free(t);
}

/*                                 Matrix                                     *
 *============================================================================*/
void * _matrix_create(void)
{
	return malloc(sizeof(Enesim_Matrix));
}

void _matrix_pointer_from(Ekeko_Value *v, Enesim_Matrix *m)
{
	Enesim_Matrix *cv = v->value.pointer_value;

	*cv = *m;
}

void _matrix_pointer_to(Ekeko_Value *v, Enesim_Matrix *m)
{
	Enesim_Matrix *cv = v->value.pointer_value;
	*m = *cv;
}


Eina_Bool _matrix_cmp(Enesim_Matrix *a, Enesim_Matrix *b)
{
	if ((a->xx == b->xx) && (a->xy == b->xy) && (a->xz == b->xz) &&
			(a->yx == b->yx) && (a->yy == b->yy) && (a->yz == b->yz) &&
			(a->zx == b->zx) && (a->zy == b->zy) && (a->zz == b->zz))
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

void _matrix_free(Enesim_Matrix *m)
{
	free(m);
}

/*                                  Color                                     *
 *============================================================================*/
void * _color_create(void)
{
	return NULL;
}

void _color_pointer_from(Ekeko_Value *v, Eon_Color *c)
{
	v->value.int_value = *c;
}

void _color_pointer_to(Ekeko_Value *v, Eon_Color *c)
{
	*c = v->value.int_value;
}

Eina_Bool _color_cmp(Eon_Color *a, Eon_Color *b)
{
	if (*a == *b)
		return EINA_FALSE;
	else
		return EINA_TRUE;
}

void _color_free(Eon_Color *c)
{

}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_value_init(void)
{
	/* register the eon values */
	EON_PROPERTY_COORD = ekeko_value_register("Eon_Coord",
			EKEKO_VALUE_CREATE(_coord_create),
			EKEKO_VALUE_FREE(_coord_free),
			EKEKO_VALUE_CMP(_coord_cmp),
			EKEKO_VALUE_POINTER_FROM(_coord_pointer_from),
			EKEKO_VALUE_POINTER_TO(_coord_pointer_to));
	EON_PROPERTY_CLOCK = ekeko_value_register("Eon_Clock",
			EKEKO_VALUE_CREATE(_clock_create),
			EKEKO_VALUE_FREE(_clock_free),
			EKEKO_VALUE_CMP(_clock_cmp),
			EKEKO_VALUE_POINTER_FROM(_clock_pointer_from),
			EKEKO_VALUE_POINTER_TO(_clock_pointer_to));
	EON_PROPERTY_TRIGGER = ekeko_value_register("Eon_Trigger",
			EKEKO_VALUE_CREATE(_trigger_create),
			EKEKO_VALUE_FREE(_trigger_free),
			EKEKO_VALUE_CMP(_trigger_cmp),
			EKEKO_VALUE_POINTER_FROM(_trigger_pointer_from),
			EKEKO_VALUE_POINTER_TO(_trigger_pointer_to));
	EON_PROPERTY_MATRIX = ekeko_value_register("Eon_Matrix",
			EKEKO_VALUE_CREATE(_matrix_create),
			EKEKO_VALUE_FREE(_matrix_free),
			EKEKO_VALUE_CMP(_matrix_cmp),
			EKEKO_VALUE_POINTER_FROM(_matrix_pointer_from),
			EKEKO_VALUE_POINTER_TO(_matrix_pointer_to));
	EON_PROPERTY_COLOR = ekeko_value_register("Eon_Color",
			EKEKO_VALUE_CREATE(_color_create),
			EKEKO_VALUE_FREE(_color_free),
			EKEKO_VALUE_CMP(_color_cmp),
			EKEKO_VALUE_POINTER_FROM(_color_pointer_from),
			EKEKO_VALUE_POINTER_TO(_color_pointer_to));
}
void eon_value_shutdown(void)
{
	/* TODO */
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Value_Type EON_PROPERTY_COORD;
Ekeko_Value_Type EON_PROPERTY_CLOCK;
Ekeko_Value_Type EON_PROPERTY_TRIGGER;
Ekeko_Value_Type EON_PROPERTY_MATRIX;
Ekeko_Value_Type EON_PROPERTY_COLOR;
