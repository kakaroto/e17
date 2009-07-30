#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(l) ((l)->private)
#define TYPE_NAME "Label"

struct _Label_Private
{
	char *text;
};

static void label_ctor(void *instance)
{
	Label *label = (Label*) instance;

	label->private = type_instance_private_get(label_type_get(), instance);
	label->private->text = NULL;
	printf("[label] ctor %p %p\n", label, label->private);
}

static void label_dtor(void *label)
{
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Type *label_type_get(void)
{
	static Type *label_type = NULL;

	if (!label_type)
	{
		label_type = type_new(TYPE_NAME, sizeof(Label), sizeof(Label_Private),
				widget_type_get(), label_ctor, label_dtor, NULL);
		EKEKO_TYPE_PROP_SINGLE_ADD(label_type, "text", EKEKO_PROPERTY_STRING, OFFSET(Label_Private, text));
	}

	return label_type;
}

Label *label_new(void)
{
	Label *label;

	label = type_instance_new(label_type_get());

	return label;
}

void label_text_set(Label *label, char *text)
{
	Ekeko_Value v;

	RETURN_IF(label == NULL);
	/* FIXME how to inform the property system that this property has changed ?
	 * for now we do this way but is slow because we need to get the property
	 * from the hash even knowing what offset we are working with
	 */
	ekeko_value_str_from(&v, text);
	object_property_value_set((Object *)label, "text", &v);
}

char *label_text_get(Label *label)
{
	Label_Private *private;

	RETURN_NULL_IF(label == NULL);

	private = PRIVATE(label);
	return private->text;
}
