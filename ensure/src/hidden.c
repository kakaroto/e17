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

#include "ensure.h"
#include "hidden.h"


static void hidden_sel(void *objv, Evas_Object *gl, void *event);
static char *hiddenclass_label_get(void *data, Evas_Object *obj ensure_unused,
		const char *part ensure_unused);
/*
static Eina_Bool hiddenclass_state_get(void *data ensure_unused, Evas_Object *obj ensure_unused,
		const char *part ensure_unused);*/
//static void hiddenclass_del(void *data, Evas_Object *obj);


static const Elm_Genlist_Item_Class hiddenclass = {
	.item_style = "default",
	.func = {
		.label_get = hiddenclass_label_get,
	//	.state_get = hiddenclass_state_get,
	},
};



/**
 * Set the view to hidden list
 */
void
view_set_hidden(void *ensurev, Evas_Object *button, void *event_info){
	struct ensure *ensure = ensurev;
	Eina_List *l;
	Evas_Object *o;

	if (ensure->current_view == ENVIEW_HIDDEN) return;
	ensure->current_view = ENVIEW_HIDDEN;

	elm_hoversel_label_set(ensure->viewselect, "Hidden");
	elm_genlist_clear(ensure->view);

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
hidden_object_add(struct ensure *ensure, Evas_Object *o){
	ensure->hidden = eina_list_append(ensure->hidden,o);
	return 0;
}

int
hidden_object_remove(struct ensure *ensure, Evas_Object *o){
	ensure->hidden = eina_list_remove(ensure->hidden,o);
	return 0;
}




static void
hidden_sel(void *objv, Evas_Object *gl ensure_unused,
		void *event ensure_unused){
	Evas_Object *o;

	printf("Selected hidden object %p\n",o);

}


static char *
hiddenclass_label_get(void *hiddenobjv, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	Evas_Object *hiddenobj = hiddenobjv;
	char buf[30];

	snprintf(buf,sizeof(buf), "%p", hiddenobj);
	return strdup(buf);
}

