/**
 * Manages the list of all objects
 */

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#include <Elementary.h>
#include <Eina.h>

#include "enobj.h"
#include "ensure.h"
#include "enasn.h"

Eina_Hash *objdb;

static Eina_Bool enobj_hash_del_cb(const Eina_Hash *hash, const void *key, void *data, void *ensure);

void
enobj_clear(void){
	if (!objdb) return;
	eina_hash_foreach(objdb, enobj_hash_del_cb, NULL);
	eina_hash_free(objdb);
	objdb = NULL;

}

int
enobj_add(struct enobj *eno){

	if (!objdb)
		objdb = eina_hash_pointer_new(enobj_free);

	if (eno->magic != ENOBJMAGIC)
		eno->magic = ENOBJMAGIC;
	eina_hash_add(objdb, &eno->id, eno);

	return 0;
}

struct enobj *
enobj_parent_get(struct enobj *eno){
	struct enobj *parent;
	if (!eno) return NULL;
	if (eno->cache.parent) return eno->cache.parent;
	if (eno->parent == 0) return NULL;

	parent = enobj_get(eno->id);
	eno->cache.parent = parent;
	return parent;
}

struct enobj *
enobj_clip_get(struct enobj *eno){
	struct enobj *clip;
	if (!eno) return NULL;
	if (eno->cache.clip) return eno->cache.clip;
	if (eno->clip == 0) return NULL;

	clip = enobj_get(eno->id);
	eno->cache.clip = clip;
	return clip;
}

struct enobj *
enobj_get(uintptr_t id){
	struct enobj *obj;
	obj = eina_hash_find(objdb, &id);
	assert(obj->magic == ENOBJMAGIC);
	assert(id == obj->id);
	return obj;
}

void
enobj_free(void *enobjv){
	struct enobj *enobj = enobjv;
	struct bug *bug;

	if (!enobj) return;
	if (enobj->name) eina_stringshare_del(enobj->name);
	if (streq(enobj->type, "text")){
		if (enobj->data.text.text)
			eina_stringshare_del(enobj->data.text.text);
		if (enobj->data.text.font)
			eina_stringshare_del(enobj->data.text.font);
		if (enobj->data.text.source)
			eina_stringshare_del(enobj->data.text.source);
	} else if (streq(enobj->type, "image")){
		if (enobj->data.image.file)
			eina_stringshare_del(enobj->data.image.file);
		if (enobj->data.image.key)
			eina_stringshare_del(enobj->data.image.key);
		if (enobj->data.image.err)
			eina_stringshare_del(enobj->data.image.err);
	} else if (streq(enobj->type, "edje")){
		if (enobj->data.edje.file)
			eina_stringshare_del(enobj->data.edje.file);
		if (enobj->data.edje.group)
			eina_stringshare_del(enobj->data.edje.group);
		if (enobj->data.edje.err)
			eina_stringshare_del(enobj->data.edje.err);
	}
	eina_stringshare_del(enobj->type);

	/* Genlist item */
//	elm_genlist_item_del(enobj->genitem);

	/* bugs */
	EINA_LIST_FREE(enobj->bugs, bug){
		free(bug->desc);
	}

	if (enobj->win) evas_object_del(enobj->win);

	eina_stringshare_del(enobj->type);

	free(enobj);
}


static Eina_Bool
enobj_hash_del_cb(const Eina_Hash *hash ensure_unused,
		const void *key ensure_unused, void *enobjv,
		void *ensure ensure_unused){
	enobj_free(enobjv);
	return true;
}
