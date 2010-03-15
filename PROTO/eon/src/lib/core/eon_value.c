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
#if 0
/*
 * rotate(degrees)
 * move(x, y)
 * scale(s)
 * scale(sx, sy)
 */
Eina_Bool eon_parser_matrix_str_from(Enesim_Matrix *m, char *v)
{
	/* well known names */
	/* rotate(degrees) */
	if (!strncmp(v, "rotate", strlen("rotate")))
	{
		float grad;
		int num;

		eon_parser_function_str_from(v + strlen("rotate"), &num, &grad);
		if (num != 1)
		{
			/* TODO put some warnings */
			return EINA_FALSE;
		}
		enesim_matrix_rotate(m, grad);
		return EINA_TRUE;
	}
	/* move(x, y) */
	else if (!strncmp(v, "move", strlen("move")))
	{
		float tx, ty;
		int num;

		eon_parser_function_str_from(v + strlen("move"), &num, &tx, &ty);
		if (num != 2)
		{
			/* TODO put some warnings */
			return EINA_FALSE;
		}
		enesim_matrix_translate(m, tx, ty);
		return EINA_TRUE;

	}
	/* scale(s) scale(sx, sy) */
	else if (!strncmp(v, "scale", strlen("scale")))
	{
		float sx, sy;
		int num;

		eon_parser_function_str_from(v + strlen("scale"), &num, &sx, &sy);
		if ((num < 1) || (num > 2))
		{
			/* TODO put some warnings */
			return EINA_FALSE;
		}
		if (num == 1)
			sy = sx;
		enesim_matrix_scale(m, sx, sy);
		return EINA_TRUE;
	}
	/* matrix="xx xy xz yx yy yz zx zy zz */
	else
	{
		float matrix[9];
		int i;
		char *tmp, *end;

		tmp = v;
		for (i = 0; i < 9; i++)
		{
			/* get the value */
			matrix[i] = strtof(tmp, &end);
			if (end == tmp)
				break;
			if (*end)
			{
				tmp = end + 1;
			}
			else
				break;
		}
		if (i < 8)
			return EINA_FALSE;
		enesim_matrix_values_set(m, matrix[0], matrix[1], matrix[2],
				matrix[3], matrix[4], matrix[5], matrix[6],
				matrix[7], matrix[8]);
		return EINA_TRUE;
	}
	return EINA_FALSE;
}
/*
 * white
 * red
 * blue
 * green
 * 0xnnnnnnnn
 */
Eina_Bool eon_parser_color_str_from(Eon_Color *c, char *v)
{
	if (!strcmp(v, "white"))
		eon_color_set(c, 0xff, 0xff, 0xff, 0xff);
	else if (!strcmp(v, "black"))
		eon_color_set(c, 0xff, 0x00, 0x00, 0x00);
	else if (!strcmp(v, "red"))
		eon_color_set(c, 0xff, 0xff, 0x00, 0x00);
	else if (!strcmp(v, "blue"))
		eon_color_set(c, 0xff, 0x00, 0x00, 0xff);
	else if (!strcmp(v, "green"))
		eon_color_set(c, 0xff, 0x00, 0xff, 0x00);
	else
	{
		/* hex | dec */
		*c = strtoul(v, NULL, 0);
	}
	return EINA_TRUE;
}
/*
 * blend
 * fill
 */

Eina_Bool eon_parser_rop_str_from(Ekeko_Value *ev, char *v)
{
	Enesim_Rop rop = ENESIM_BLEND;

	if (!strcmp(v, "blend"))
		rop = ENESIM_BLEND;
	else if (!strcmp(v, "fill"))
		rop = ENESIM_FILL;
	else
	{
		printf("undefined rop type? %s\n", v);
		return EINA_FALSE;
	}
	ekeko_value_int_from(ev, rop);

	return EINA_TRUE;
}

/*
 * #id.event
 */
Eina_Bool eon_parser_trigger_str_from(Eon_Trigger *t, Eon_Document *d, char *v)
{
	Ekeko_Object *oid;
	char *tmp;
	char *ttmp;
	char *token;

	if (!d)
		return EINA_FALSE;
	if (*v != '#')
		return EINA_FALSE;

	tmp = strdup(v);
	ttmp = tmp + 1;

	token = strtok(ttmp, ".");
	oid = eon_document_object_get_by_id(d, token);
	if (!oid)
		return EINA_FALSE;
	t->obj = oid;
	token = strtok(NULL, ".");
	t->event = token;

	return EINA_FALSE;
}

/*
 * X.Xm
 * X.Xs
 */
Eina_Bool eon_parser_clock_str_from(Eon_Clock *c, char *v)
{
	float num = 0;
	char *units;
	float dec;

	units = v + strlen(v) - 1;

	/* minutes */
	if (*units == 'm')
	{
		float secs;

		*units = '\0';
		num = atof(v);
		secs = num * 60;

		dec = secs - (int)secs;
		c->seconds = (int)secs;
		c->micro = dec * 100000;
	}
	/* seconds  | no units */
	else if (*units == 's')
	{

		*units = '\0';
		num = strtof(v, NULL);
		dec = num - (int)num;
		c->seconds = (int)num;
		c->micro = dec * 1000000;
	}
	else
	{
		num = atof(v);
		dec = num - (int)num;
		c->seconds = (int)num;
		c->micro = dec * 100000;
	}
	return EINA_TRUE;
}

#endif
/*----------------------------------------------------------------------------*
 *                                  Coord                                     *
 *----------------------------------------------------------------------------*/
/*
 * x%
 * x
 */
Eina_Bool _coord_string_from(Ekeko_Value *v, const char *str)
{
	Eon_Coord *coord;

	coord = v->value.pointer_value;
#if 0
	Eon_Coord_Type ctype = EON_COORD_ABSOLUTE;
	int i;
	char *rel;

	rel = strchr(v, '%');
	if (rel)
	{
		rel = '\0';
		ctype = EON_COORD_RELATIVE;
	}
	i = strtol(v, NULL, 10);
	eon_coord_set(c, i, ctype);
#endif
	return EINA_TRUE;
}

char * _coord_string_to(Ekeko_Value *v, Eon_Coord *val)
{
	*val = *(Eon_Coord *)v->value.pointer_value;
}

Eina_Bool _coord_cmp(Ekeko_Value *v1, Ekeko_Value *v2)
{
	Eon_Coord *a, *b;

	a = v1->value.pointer_value;
	b = v2->value.pointer_value;

	if ((a->value == b->value) && (a->type == b->type))
		return EINA_FALSE;
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                                  Clock                                     *
 *----------------------------------------------------------------------------*/
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
/*----------------------------------------------------------------------------*
 *                                 Trigger                                    *
 *----------------------------------------------------------------------------*/
void _trigger_pointer_from(Ekeko_Value *v, Eon_Trigger *t)
{
	Eon_Trigger *cv = v->value.pointer_value;

	*cv = *t;
}

void _trigger_pointer_to(Ekeko_Value *v, Eon_Trigger *t)
{
	Eon_Trigger *cv = v->value.pointer_value;
	*t = *cv;
}

Eina_Bool _trigger_cmp(Eon_Trigger *a, Eon_Trigger *b)
{
	if ((a->obj == b->obj) && (!strcmp(a->event, b->event)))
		return EINA_FALSE;
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                                  Matrix                                    *
 *----------------------------------------------------------------------------*/
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
/*                                  Color                                     *
 *============================================================================*/
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
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_value_init(void)
{
	/* register the eon values */
	EON_PROPERTY_COORD = ekeko_value_register("Eon_Coord",
			EKEKO_VALUE_CMP(_coord_cmp),
			EKEKO_VALUE_STRING_FROM(_coord_string_from),
			EKEKO_VALUE_STRING_TO(_coord_string_to));
	EON_PROPERTY_CLOCK = ekeko_value_register("Eon_Clock",
			EKEKO_VALUE_CMP(_clock_cmp),
			EKEKO_VALUE_STRING_FROM(_clock_pointer_from),
			EKEKO_VALUE_STRING_TO(_clock_pointer_to));
	EON_PROPERTY_TRIGGER = ekeko_value_register("Eon_Trigger",
			EKEKO_VALUE_CMP(_trigger_cmp),
			EKEKO_VALUE_STRING_FROM(_trigger_pointer_from),
			EKEKO_VALUE_STRING_TO(_trigger_pointer_to));
	EON_PROPERTY_MATRIX = ekeko_value_register("Eon_Matrix",
			EKEKO_VALUE_CMP(_matrix_cmp),
			EKEKO_VALUE_STRING_FROM(_matrix_pointer_from),
			EKEKO_VALUE_STRING_TO(_matrix_pointer_to));
	EON_PROPERTY_COLOR = ekeko_value_register("Eon_Color",
			EKEKO_VALUE_CMP(_color_cmp),
			EKEKO_VALUE_STRING_FROM(_color_pointer_from),
			EKEKO_VALUE_STRING_TO(_color_pointer_to));
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

/* TODO we should add a new way to add a value type, that is, a wrapper of
 * the ekeko value register system. On eon we'll need a way to serialize the
 * data too
 */
