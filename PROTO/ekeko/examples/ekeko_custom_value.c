/* This example just creates some new value types
 * adds them as properties to the type system
 * and sets/gets the values
 */
#include "Ekeko.h"
#include <stdio.h>

#define OFFSET(type, mem) ((size_t) ((char *)&((type *) 0)->mem - (char *)((type *) 0)))

Ekeko_Value_Type custom;
Ekeko_Type *type;

typedef struct _Custom_Value
{
	int a;
	int b;
	char c;
} Custom_Value;

typedef struct _Custom_Object
{
	Ekeko_Object base;
	void *prv;
} Custom_Object;

typedef struct _Custom_Object_Private
{
	Custom_Value val;

} Custom_Object_Private;


void _custom_pointer_from(Ekeko_Value *v, void *p)
{
	Custom_Value *cv = p;

	v->value.pointer_value = p;
	printf("Pointer to %p %p\n", v, p);
	printf("value = %d %d %c\n", cv->a, cv->b, cv->c); 
}

void _custom_pointer_to(Ekeko_Value *v, void *p)
{
	Custom_Value *cv = p;

	*cv = *(Custom_Value *)v->value.pointer_value;
	printf("Pointer from %p %p\n", v, p);
	printf("value = %d %d %c\n", cv->a, cv->b, cv->c);
}

Eina_Bool _custom_cmp(void *a, void *b)
{
	Custom_Value *cva = a, *cvb = b;
	printf("Comparing\n");
	
}

void custom_value_to(Ekeko_Value *v, Custom_Value *cv, int a, int b, char c)
{
	v->type = custom;
	v->value.pointer_value = cv;
	cv->a = a;
	cv->b = b;
	cv->c = c;
}


void ctor(Ekeko_Object *o)
{
	Custom_Object *c = o;

	c->prv = ekeko_type_instance_private_get(type, o);
	printf("Creating a new custom object at %p with prv at %p\n", c, c->prv);
}


int main(void)
{
	Ekeko_Object *obj;
	Ekeko_Value v1, v2;
	Custom_Value cv;

	ekeko_init();
	/* Let's register our new type */
	custom = ekeko_value_register("Custom",
			EKEKO_VALUE_CMP(_custom_cmp),
			EKEKO_VALUE_POINTER_FROM(_custom_pointer_from),
			EKEKO_VALUE_POINTER_TO(_custom_pointer_to));
	/* Let's create our type with a property of type custom */
	type = ekeko_type_new("Custom_Type", sizeof(Custom_Object),
			sizeof(Custom_Object_Private), ekeko_object_type_get(),
			ctor, NULL, NULL);
	EKEKO_TYPE_PROP_SINGLE_ADD(type, "val", custom, OFFSET(Custom_Object_Private, val));

	/* Instantiate an object of such type and try setting/getting properties */
	obj = ekeko_type_instance_new(type);
	/* Set a value different than the type expected, this one should trigger an error */
	ekeko_object_property_value_set(obj, "val", &v1);
	/* Set a value with the same type */
	custom_value_to(&v1, &cv, 10, 15, 'f');
	ekeko_object_property_value_set(obj, "val", &v1);

	/* getting the values */
	ekeko_object_property_value_get(obj, "val", &v2);
	cv = *(Custom_Value *)v2.value.pointer_value;
	printf("Properties get are %d %d %c\n", cv.a, cv.b, cv.c);
}
