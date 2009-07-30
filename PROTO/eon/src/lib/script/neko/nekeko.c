#include <stdio.h>
#include <neko.h>
#include <neko_vm.h>
#include <Ekeko.h>
/*============================================================================*
 *                          Ekeko Object Interface                            *
 *============================================================================*/

DEFINE_KIND(k_value);
DEFINE_KIND(k_object);
DEFINE_ENTRY_POINT(object_main);

void object_main(void)
{
	ekeko_init();
	kind_share(&k_object, "ekeko_object");
}

//val_iter_fields( value obj, void f( value v, field f, void * ), void *p );

static value object_property_value_get(value prop)
{
	value t = val_this();
	value inst;
	field f;
	Ekeko_Value ev;
	value v;
	Ekeko_Object *o;
	char *eprop;

	printf("GETTING PROPERTY!!!\n");
	val_check(t, object);
	f = val_id("__inst");
	inst = val_field(t, f);
	val_check_kind(inst, k_object);
	o = val_data(inst);

	if (!val_is_string(prop))
		goto error;

	v = val_null;
	eprop = val_string(prop);
	ekeko_object_property_value_get(o, eprop, &ev);
	/* check the type */
	switch (ev.type)
	{
		case EKEKO_PROPERTY_INT:
		v = alloc_best_int(ev.value.int_value);
		break;

		case EKEKO_PROPERTY_STRING:
		v = alloc_string(ev.value.string_value);
		break;

		default:
		break;
	}
	return v;
error:
	return val_null;
}

static value object_property_value_set(value prop, value v)
{
	value t = val_this();
	value inst;
	field f;
	Ekeko_Value ev;
	Ekeko_Object *o;
	char *eprop;

	val_check(t, object);
	f = val_id("__inst");
	inst = val_field(t, f);
	val_check_kind(inst, k_object);
	
	printf("SETTING PROPERTY!!!\n");
	if (!val_is_string(prop))
		goto error;

	eprop = val_string(prop);
	o = val_data(inst);
	/* check the type */
	switch (val_type(v))
	{
		case VAL_NULL:
		goto error;
		break;

		case VAL_INT:
		ekeko_value_int_from(&ev, val_int(v));
		break;

		case VAL_STRING:
		ekeko_value_str_from(&ev, val_string(v));
		break;

		default:
		break;
	}
	ekeko_object_property_value_set(o, eprop, &ev);
	return val_true;
error:
	return val_false;
}

static void object_delete(value v)
{
	printf("deleting the object!!\n");
	ekeko_type_instance_delete(val_data(v));
}

static value object_new(void)
{
	Ekeko_Object *o;
	value i;

	o = ekeko_object_new();
	i = alloc_abstract(k_object, o);
	val_gc(i, object_delete);

	return i;
}

static value object_init(void)
{
	return val_true;
}

DEFINE_PRIM(object_new, 0);
DEFINE_PRIM(object_init, 0);
DEFINE_PRIM(object_property_value_set, 2);
DEFINE_PRIM(object_property_value_get, 1);

