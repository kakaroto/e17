/*
 * eon_transition.c
 *
 *  Created on: 24-jul-2009
 *      Author: jl
 */
#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Transition_Private *)((Eon_Transition *)(d))->private)

struct _Eon_Transition_Private
{
	Eina_List *paints;
	float step;
};

/* in case some paint child has changed we should trigger the change too */
static void _paint_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	//printf("some childs's property changed!!\n");
}

static void _child_appended_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Transition *t = (Eon_Transition *)o;
	Eon_Transition_Private *prv = PRIVATE(t);
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;


	if (ekeko_type_instance_is_of(e->target, EON_TYPE_PAINT))
	{
		prv->paints = eina_list_append(prv->paints, e->target);
		ekeko_event_listener_add(e->target, EKEKO_EVENT_PROP_MODIFY, _paint_change, EINA_FALSE, t);
	}
}

static void _ctor(void *instance)
{
	Eon_Transition *t;
	Eon_Transition_Private *prv;

	t = (Eon_Transition *) instance;
	t->private = prv = ekeko_type_instance_private_get(eon_transition_type_get(), instance);
	ekeko_event_listener_add((Ekeko_Object *)t, EKEKO_EVENT_OBJECT_APPEND, _child_appended_cb, EINA_TRUE, NULL);
}

static void _dtor(void *hswitch)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool eon_transition_paint_get(Eon_Transition *t, Eon_Paint **p1, Eon_Paint **p2, float *step)
{
	Eon_Transition_Private *prv = PRIVATE(t);
	int num;
	unsigned int pos;

	/* get the number of elements that are of type paint */
	num = eina_list_count(prv->paints);
	if (num < 2)
		return EINA_FALSE;
	pos = prv->step * (num - 1);

	*step = ((float)(prv->step * (num - 1))) - pos;
	*p1 = eina_list_nth(prv->paints, pos);
	*p2 = eina_list_nth(prv->paints, pos + 1);
	//printf("step = %g (%g) pos = %d %d\n", *step, prv->step, pos, pos + 1);

	return EINA_TRUE;
}

Eina_Bool eon_transition_appendable(Ekeko_Object *t, Ekeko_Object *child)
{
	if (ekeko_type_instance_is_of(child, EON_TYPE_PAINT))
		return EINA_TRUE;
	else
		return eon_paint_appendable(t, child);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_TRANSITION_STEP;

EAPI Ekeko_Type *eon_transition_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_TRANSITION, sizeof(Eon_Transition),
				sizeof(Eon_Transition_Private), eon_paint_type_get(),
				_ctor, _dtor, NULL);
		EON_TRANSITION_STEP = EKEKO_TYPE_PROP_SINGLE_ADD(type, "step", EKEKO_PROPERTY_FLOAT, OFFSET(Eon_Transition_Private, step));
	}

	return type;
}

EAPI void eon_transition_step_set(Eon_Transition *t, float step)
{
	Ekeko_Value v;

	ekeko_value_float_from(&v, step);
	ekeko_object_property_value_set((Ekeko_Object *)t, "step", &v);
}

EAPI float eon_transition_step_get(Eon_Transition *t)
{
	Eon_Transition_Private *prv = PRIVATE(t);
	return prv->step;
}
