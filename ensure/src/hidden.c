#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/signalfd.h>

#include <Eina.h>
#include <Elementary.h>

#include "config.h"
#include "ensure.h"
#include "hidden.h"
#include "enobj.h"
#include "display.h"


static void hidden_sel(void *objv, Evas_Object *gl, void *event);
static char *hiddenclass_label_get(void *data, Evas_Object *obj __UNUSED__,
		const char *part __UNUSED__);
static Evas_Object *hiddenclass_icon_get(void *idv, Evas_Object *obj, const char *part);
/*
static Eina_Bool hiddenclass_state_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
		const char *part __UNUSED__);*/
//static void hiddenclass_del(void *data, Evas_Object *obj);


static const Elm_Genlist_Item_Class hiddenclass = {
	.item_style = "default",
	.func = {
		.label_get = hiddenclass_label_get,
	//	.state_get = hiddenclass_state_get,
		.content_get = hiddenclass_icon_get
	},
};



/**
 * Set the view to hidden list
 */
void
view_set_hidden(void *ensurev, Evas_Object *button __UNUSED__, void *event_info __UNUSED__){
	struct ensure *ensure = ensurev;
	Eina_List *l;
	Evas_Object *o;

	if (ensure->current_view == ENVIEW_HIDDEN) return;
	ensure->current_view = ENVIEW_HIDDEN;

	elm_object_text_set(ensure->viewselect, "Hidden");
	elm_gen_clear(ensure->view);

	evas_object_data_set(ensure->view, "ensure", ensure);

	EINA_LIST_FOREACH(ensure->hidden, l, o){
		elm_genlist_item_append(ensure->view, &hiddenclass,
				o,  NULL,
				ELM_GENLIST_ITEM_NONE,
				hidden_sel,
				o);
	}

}

/**
 * Save to the hidden list
 */
int
hidden_object_add(struct ensure *ensure, uintptr_t o){
	ensure->hidden = eina_list_append(ensure->hidden,(void *)o);
	if (ensure->current_view == ENVIEW_HIDDEN){
		ensure->current_view = -1;
		view_set_hidden(ensure, NULL, NULL);
	}
	return 0;
}

int
hidden_object_remove(struct ensure *ensure, uintptr_t o){
	ensure->hidden = eina_list_remove(ensure->hidden,(void *)o);
	if (ensure->current_view == ENVIEW_HIDDEN){
		ensure->current_view = -1;
		view_set_hidden(ensure, NULL, NULL);
	}
	return 0;
}

/**
 * Returns true if an object is hidden.
 *
 * @param ensure Ensure pointer
 * @param obj Object id to check
 * @return true if the object is hidden, false otherwise.
 */
bool
hidden_get(struct ensure *ensure, uintptr_t obj){
	uintptr_t *hiddenobj;
	Eina_List *l;

	if (!ensure || !ensure->hidden || !obj) return false;

	EINA_LIST_FOREACH(ensure->hidden, l, hiddenobj){
		if ((uintptr_t)hiddenobj == obj)
			return true;
	}
	return false;
}



static void
hidden_sel(void *objv, Evas_Object *gl __UNUSED__,
		void *event __UNUSED__){
	Evas_Object *o = objv;

	printf("Selected hidden object %p\n",o);

}


static char *
hiddenclass_label_get(void *hiddenobjv, Evas_Object *obj __UNUSED__,
		const char *part __UNUSED__){
	Evas_Object *hiddenobj = hiddenobjv;
	char buf[30];

	snprintf(buf,sizeof(buf), "%p", hiddenobj);
	return strdup(buf);
}

/**
 * Get the icon.
 *
 * if hte object exists, add a view / highlight combo.
 * else nothing.
 */
static Evas_Object *
hiddenclass_icon_get(void *idv, Evas_Object *obj, const char *part){
	struct enobj *enobj;
	struct ensure *ensure;
	uintptr_t id = (uintptr_t)idv;

	if (strcmp(part, "elm.swallow.end") != 0) return NULL;

	ensure = evas_object_data_get(obj, "ensure");
	if (!ensure) return NULL;

	enobj = enobj_get(ensure, id);

	if (!enobj) return NULL;

	return display_buttons_add(obj, enobj);
}
