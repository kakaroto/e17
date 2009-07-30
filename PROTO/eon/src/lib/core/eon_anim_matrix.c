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
#define PRIVATE(d) ((Eon_Animation_Matrix_Private *)((Eon_Animation_Matrix *)(d))->private)
struct _Eon_Animation_Matrix_Private
{
	int type;
};

static void _animation_matrix_callback(Eon_Animation *a, const char *prop,
		Ekeko_Object *o, const Etch_Data *curr, const Etch_Data *prev)
{
	Eon_Animation_Matrix *am = (Eon_Animation_Matrix *)a;
	Eon_Animation_Matrix_Private *prv = PRIVATE(am);
	Enesim_Matrix m;
	Ekeko_Value v;

	if (prv->type == EON_ROTATE)
	{
		enesim_matrix_rotate(&m, curr->data.f);
	}
	else if (prv->type == EON_SCALE)
	{
		enesim_matrix_scale(&m, curr->data.f, curr->data.f);
	}
	/* get the matrix type from the animation */
	/* create a new one */
	/* set the matrix on the object */
	eon_value_matrix_from(&v, &m);
	ekeko_object_property_value_set(o, prop, &v);
}

static void _value_set(Ekeko_Value *v, Etch_Animation_Keyframe *k)
{
	if (v->type == EKEKO_PROPERTY_FLOAT)
	{
#if 0
		Enesim_Matrix *m = v->value.pointer_value;
		printf("[Eon_Animation] Setting matrix to:\n");
		printf("[%g %g %g]\n[%g %g %g]\n[%g %g %g]\n", m->xx, m->xy, m->xz, m->yx, m->yy, m->yz, m->zx, m->zy, m->zz);
#endif
		etch_animation_keyframe_value_set(k, v->value.float_value);
	}
}

static void _callback_set(Ekeko_Value_Type vtype, Etch_Data_Type *dtype, Eon_Animation_Callback *cb)
{
	if (vtype == EON_PROPERTY_MATRIX)
	{
		*dtype = ETCH_FLOAT;
		*cb = _animation_matrix_callback;
	}
}

static void _ctor(void *instance)
{
	Eon_Animation_Basic *a;
	Eon_Animation_Basic_Private *prv;

	a = (Eon_Animation_Basic *) instance;
	a->private = prv = ekeko_type_instance_private_get(eon_animation_matrix_type_get(), instance);
	/* default values */
	a->parent.callback_set = _callback_set;
	a->parent.value_set = _value_set;
}

static void _dtor(void *rect)
{

}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_ANIMATION_MATRIX_TYPE;

EAPI Ekeko_Type *eon_animation_matrix_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_ANIMATION_MATRIX, sizeof(Eon_Animation_Matrix),
				sizeof(Eon_Animation_Matrix_Private), eon_animation_type_get(),
				_ctor, _dtor, NULL);
		EON_ANIMATION_MATRIX_TYPE = EKEKO_TYPE_PROP_SINGLE_ADD(type, "type", EKEKO_PROPERTY_INT, OFFSET(Eon_Animation_Matrix_Private, type));
	}
	return type;
}


EAPI Eon_Animation_Matrix * eon_animation_matrix_new(void)
{
	Eon_Animation_Matrix *a;

	a = ekeko_type_instance_new(eon_animation_matrix_type_get());

	return a;
}

