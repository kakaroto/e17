/* This example just creates some new value types
 * adds them as properties to the type system
 * and sets/gets the values
 */
#include "Ekeko.h"
#include <stdio.h>

const char *CUSTOM_VAL = NULL;

Ekeko_Value_Type CUSTOM_PROPERTY;
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
	/* Let's create our type with a property of type custom */
	type = ekeko_type_new("Custom_Type", sizeof(Custom_Object),
			sizeof(Custom_Object_Private), ekeko_object_type_get(),
			ctor, NULL, NULL);

	/* Instantiate an object of such type and try setting/getting properties */
	obj = ekeko_type_instance_new(type);
	/* Set a value different than the type expected, this one should trigger an error */
	ekeko_object_property_value_set(obj, "val", &v1);
	/* Set a value with the same type */
	ekeko_object_property_value_set(obj, "val", &v1);
	/* getting the values */
	ekeko_object_property_value_get(obj, "val", &v2);
	cv = *(Custom_Value *)v2.value.pointer_value;
	printf("Properties get are %d %d %c\n", cv.a, cv.b, cv.c);
}
