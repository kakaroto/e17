/*
 * common_main.c
 *
 *  Created on: 15-jul-2009
 *      Author: jl
 */
#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool eon_parser_polygon_points_str_from(Eon_Polygon *p, char *v)
{
	char *tmp = v;
	char *end = tmp;
	int x, y;
	/* comma separated coordinates */
	do
	{
		tmp = end;
		x = strtod(tmp, &end);
		if (*end == ',')
		{
			tmp = end + 1;
			y = strtod(tmp, &end);
		}
		else
			break;
		eon_polygon_point_add(p, x, y);
	} while (tmp != end);
}

/* TODO rename this and document */
Eina_Bool eon_parser_function_str_from(char *v, int *num, ...)
{
	va_list ap;
	char *tmp;
	char *end;

	*num = 0;
	tmp = v;
	if (*tmp == '(')
		tmp++;
	va_start(ap, num);
	do
	{
		float *f;
		float ftmp;

		/* get the value */
		ftmp = strtof(tmp, &end);
		if (end == tmp)
			break;
		if (*end)
		{
			tmp = end + 1;
		}
		else
			break;
		f = va_arg(ap, float *);
		if (!f)
		{
			va_end(ap);
			return EINA_TRUE;
		}
		*f = ftmp;
		(*num)++;
	} while (tmp);
	va_end(ap);
	return EINA_TRUE;
}

Ekeko_Object * eon_parser_object_new(Ekeko_Object *p, Eon_Type_Constructor ctr)
{
	Ekeko_Object *o;

	o = ekeko_type_instance_new(ctr());

	if (!p)
		return o;

	if (ekeko_object_child_append(p, o))
		return o;
	else
	{
		ekeko_object_delete(o);
		return NULL;
	}
}

Ekeko_Object * eon_parser_shape_new(Ekeko_Object *p, Eon_Type_Constructor ctr)
{
	Ekeko_Object *o;

	o = eon_parser_object_new(p, ctr);
	/* default atributres */
	if (o)
	{
		eon_shape_show((Eon_Shape *)o);
	}
	return o;
}

Ekeko_Object * eon_parser_polygon_new(Ekeko_Object *p)
{
	return eon_parser_shape_new(p, eon_polygon_type_get);
}
#if 0
Ekeko_Object * eon_parser_rect_new(Ekeko_Object *p)
{
	return eon_parser_shape_new(p, eon_rect_type_get);
}
Ekeko_Object * eon_parser_circle_new(Ekeko_Object *p)
{
	return eon_parser_shape_new(p, eon_circle_type_get);
}
Ekeko_Object * eon_parser_text_new(Ekeko_Object *p)
{
	return eon_parser_shape_new(p, eon_text_type_get);
}
#endif

Ekeko_Object * eon_parser_script_new(Ekeko_Object *p)
{
	return eon_script_new();
}


