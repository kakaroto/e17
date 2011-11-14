/**
 * The error view window
 *
 * This displays the list of errors in the current view
 */
#include <stdbool.h>

#include <Eina.h>
#include <Elementary.h>

#include "config.h"
#include "ensure.h"
#include "enobj.h"
#include "errors.h"
#include "display.h"

static void enobj_select(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *itemv);
static char * enobj_label_get(void *data, Evas_Object *obj __UNUSED__,
		const char *part __UNUSED__);
static Evas_Object * enobj_icon_get(void *enobjv, Evas_Object *obj,
		const char *part);
static Eina_Bool enobj_state_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__);
static void enobj_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__);

static char *enwin_label_get(void *data, Evas_Object *obj, const char *part);
static Eina_Bool enwin_state_get(void *data, Evas_Object *obj, const char *);
static void enwin_select(void *data, Evas_Object *obj, void *event);
static void enwin_del(void *data, Evas_Object *obj);



static const Elm_Genlist_Item_Class objc = {
	.item_style = "default",
	.func = {
		.label_get = enobj_label_get,
		.content_get = enobj_icon_get,
		.state_get = enobj_state_get,
		.del = enobj_del
	}
};

static const Elm_Genlist_Item_Class windowclass = {
	.item_style = "default",
	.func = {
		.label_get = enwin_label_get,
		.state_get = enwin_state_get,
		.del = enwin_del,
	},
};


void
errors_view_set(void *ensurev, Evas_Object *button __UNUSED__,
		void *event_info __UNUSED__){
	struct ensure *ensure = ensurev;

	if (ensure->current_view == ENVIEW_ERROR) return;
	ensure->current_view = ENVIEW_ERROR;

	elm_object_text_set(ensure->viewselect, "Errors");

	errors_update(ensure);


}


void
errors_update(struct ensure *ensure){
	struct enobj *enobj;
	Eina_Iterator *iter;


	if (!ensure) return;

	elm_gen_clear(ensure->view);
	elm_object_text_set(ensure->viewselect, "Errors");

	if (!ensure->cur) return;

	/* For each object */
	iter = eina_hash_iterator_data_new(ensure->cur->objdb);
	EINA_ITERATOR_FOREACH(iter, enobj){
		if (!enobj->bugs) continue;

		if (!enobj->enwin->genitem){
			enobj->enwin->genitem = elm_genlist_item_append(ensure->view,
					&windowclass, enobj->enwin, NULL,
					ELM_GENLIST_ITEM_SUBITEMS, enwin_select,
					enobj->enwin);
		}

		enobj->genitem = elm_genlist_item_append(ensure->view, &objc,
				enobj, enobj->enwin->genitem, ELM_GENLIST_ITEM_SUBITEMS,
				enobj_select, enobj);

	}

}

/*
 * Tree view callbacks
 */
static void
enobj_select(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
		void *itemv){
	elm_genlist_item_expanded_set(itemv, true);
}
static char *
enobj_label_get(void *data, Evas_Object *obj __UNUSED__,
		const char *part __UNUSED__){
	const struct enobj *enobj = data;
	char buf[200];

	if (enobj->name){
		snprintf(buf,sizeof(buf), "%s (%"PRIuPTR") - %s",
				enobj->name, enobj->id, enobj->type);
	} else {
		snprintf(buf,sizeof(buf), "%"PRIuPTR" - %s",
				enobj->id,enobj->type);
	}
	return strdup(buf);
}
static Evas_Object *
enobj_icon_get(void *enobjv, Evas_Object *obj, const char *part){
	if (strcmp(part, "elm.swallow.end") == 0){
		return display_buttons_add(obj, enobjv);
	}
	return NULL;
}
static Eina_Bool
enobj_state_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
		const char *part __UNUSED__){
	return false;
}


static void
enobj_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__){
	struct enobj *enobj = data;

	enobj->genitem = NULL;
}


static char *
enwin_label_get(void *data, Evas_Object *obj __UNUSED__,
		const char *part __UNUSED__){
	const struct enwin *enwin;
	const char *fmt = "Untitled Window '%p'";
	char *buf;
	int len;

	enwin = data;

	if (enwin->name && strlen(enwin->name) > 1){
		return strdup(enwin->name);
	}

	len = snprintf(NULL,0,fmt,enwin->id);
	if (len < 1) return NULL;
	len ++;
	buf = malloc(len);
	if (!buf) return NULL;
	/* remmeber to update both snprintfs if you change it */
	snprintf(buf,len,fmt,enwin->id);
	return buf;
}
static Eina_Bool
enwin_state_get(void *data __UNUSED__ __UNUSED__, Evas_Object *obj __UNUSED__,
		const char *state __UNUSED__){
	return false;
}
static void
enwin_select(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__){
	/* FIXME: Do something or delete this */
	printf("Select... ignoring\n");
}
static void
enwin_del(void *enwinv, Evas_Object *obj __UNUSED__){
	struct enwin *enwin = enwinv;

	enwin->genitem = NULL;
}

