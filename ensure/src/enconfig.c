#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#include <Elementary.h>
#include <Eina.h>

#include "enobj.h"
#include "ensure.h"
#include "enasn.h"

static void config_add_classes(Evas_Object *gl);
void cfg_sel(void *data, Evas_Object *obj, void *event);
void generic_contract(void *data, Evas_Object *obj, void *event);
void generic_exp_req(void *data, Evas_Object *obj, void *event);
void generic_con_req(void *data, Evas_Object *obj, void *event);

char *asn_label_get(void *data, Evas_Object *, const char *);
Evas_Object *asn_icon_get(void *data, Evas_Object *, const char *);
Eina_Bool asn_state_get(void *data, Evas_Object *, const char *);
void asn_del(void *data, Evas_Object *obj);
void asn_select(void *data, Evas_Object *obj, void *event);
void asn_select_toggle(void *data, Evas_Object *obj, void *event);


char *cfg_label_get(void *, Evas_Object *, const char *part);
Evas_Object *cfg_icon_get(void *, Evas_Object *, const char *part);
Eina_Bool cfg_state_get(void *, Evas_Object *, const char *part);
void cfg_del(void *, Evas_Object *obj);


struct severityinfo severity[ENSURE_N_SEVERITIES] = {
	[ENSURE_CRITICAL] = {
		.name = "Critical",
	},
	[ENSURE_BUG] = {
		.name = "Bug",
	},
	[ENSURE_BADFORM] = {
		.name = "Bad Form",
	},
	[ENSURE_POLICY] = {
		.name = "Local Policy",
	},
	[ENSURE_PEDANTIC] = {
		.name = "Pedantry",
	}
};
static const Elm_Genlist_Item_Class clc = {
	.item_style = "default",
	.func = {
		.label_get = cfg_label_get,
		.icon_get = cfg_icon_get,
		.state_get = cfg_state_get,
		.del = cfg_del,
	},
};


static const Elm_Genlist_Item_Class asnclass = {
	.item_style = "default",
	.func = {
		.label_get = asn_label_get,
		.icon_get = asn_icon_get,
		.state_get = asn_state_get,
		.del = asn_del
	},
};



/**
 * Set the current view to the list of config flags.
 */
void
view_set_config(void *ensurev, Evas_Object *button, void *event_info ensure_unused){
	struct ensure *ensure = ensurev;

	assert(ensure->magic == ENSURE_MAGIC);

	if (ensure->current_view == ENVIEW_CONFIG) return;
	ensure->current_view = ENVIEW_CONFIG;

	elm_object_text_set(ensure->viewselect, "Config");

	elm_genlist_clear(ensure->view);

	config_add_classes(ensure->view);
}


static void
config_add_classes(Evas_Object *gl){
	int i;
	for (i = 0 ; i < ENSURE_N_SEVERITIES ; i ++){
		severity[i].item = elm_genlist_item_append(gl,&clc,
				severity + i,
				NULL, /* No parent */
				ELM_GENLIST_ITEM_SUBITEMS,
				cfg_sel,
				severity + i /* data */);

	}
}

void
config_expand(struct ensure *ensure, Elm_Genlist_Item *item){
	struct severityinfo *sev;
	struct asninfo *ai;
	Eina_List *l;

	sev = (struct severityinfo *)elm_genlist_item_data_get(item);

	EINA_LIST_FOREACH(sev->asninfo, l, ai){
		printf("Add %p\n",ai);
		elm_genlist_item_append(ensure->view, &asnclass,
				ai, item,
				ELM_GENLIST_ITEM_NONE,
				asn_select, ai);
	}

}

void
cfg_sel(void *data, Evas_Object *obj ensure_unused, void *event ensure_unused){
       const struct severityinfo *info = data;

       printf("Item selected! %s\n",info->name);
}


char *
cfg_label_get(void *data, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	const struct severityinfo *info;

	info = data;
	return strdup(info->name);
}
Evas_Object *
cfg_icon_get(void *data ensure_unused, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	return NULL;
}
Eina_Bool
cfg_state_get(void *data ensure_unused, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	return false;
}

void
cfg_del(void *data, Evas_Object *obj ensure_unused){
	struct severityinfo *severity;

	severity = data;
	severity->item = NULL;
}


