#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Document_Private *)((Eon_Document *)(d))->private)

struct _Eon_Button_Private
{
	Ekeko_Object parent;
	Eon_External *ext;
	Eon_Coord x, y, w, h;
	char *label;
};

static void _ext_click_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	/* send the event onclick but on the button itself */
}

static void _ctor(void *instance)
{
	Eon_Button *b;
	Eon_Button_Private *prv;

	b = (Eon_Document*) instance;
	b->private = prv = ekeko_type_instance_private_get(eon_button_type_get(), instance);
	/* external object */
	/* TODO register on mouse click on the external object */
}

static void _dtor(void *instance)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_BUTTON_LABEL;

Ekeko_Type *eon_button_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_BUTTON, sizeof(Eon_Button),
				sizeof(Eon_Button_Private), ekeko_object_type_get(), _ctor,
				_dtor, _appendable);
		EON_BUTTON_LABEL = EKEKO_TYPE_PROP_SINGLE_ADD(type, "label", EKEKO_PROPERTY_STRING, OFFSET(Eon_Button_Private, label));
	}
	return type;
}
