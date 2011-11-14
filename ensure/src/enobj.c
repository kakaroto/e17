/**
 * Manages the list of all objects
 */

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#include <Elementary.h>
#include <Eina.h>

#include "config.h"
#include "enobj.h"
#include "ensure.h"
#include "enasn.h"

//static Eina_Bool enobj_hash_del_cb(const Eina_Hash *hash, const void *key, void *data, void *ensure);
static Eina_Bool enobj_prepare_object(const Eina_Hash *hash, const void *key, void *obj, void *ensurev);

/*
void
enobj_clear(){
	if (!objdb) return;
	eina_hash_foreach(objdb, enobj_hash_del_cb, NULL);
	eina_hash_free(objdb);
	objdb = NULL;

}
*/

int
enobj_add(struct ensure *ensure, struct enobj *eno){

	if (eno->magic != (int)ENOBJMAGIC)
		eno->magic = ENOBJMAGIC;
	eina_hash_add(ensure->cur->objdb, &eno->id, eno);

	if (!eno->ensure) eno->ensure = ensure;

	return 0;
}

struct enobj *
enobj_parent_get(struct ensure *ensure, struct enobj *eno){
	struct enobj *parent;
	if (!eno) return NULL;
	if (eno->cache.parent) return eno->cache.parent;
	if (eno->parent == 0) return NULL;

	parent = enobj_get(ensure, eno->id);
	eno->cache.parent = parent;
	return parent;
}

struct enobj *
enobj_clip_get(struct ensure *ensure, struct enobj *eno){
	struct enobj *clip;
	if (!eno) return NULL;
	if (eno->cache.clip) return eno->cache.clip;
	if (eno->clip == 0) return NULL;

	clip = enobj_get(ensure, eno->id);
	eno->cache.clip = clip;
	return clip;
}

struct enobj *
enobj_get(struct ensure *ensure, uintptr_t id){
	struct enobj *obj;
	obj = eina_hash_find(ensure->cur->objdb, &id);
	assert(obj->magic == (int)ENOBJMAGIC);
	assert(id == obj->id);
	return obj;
}


/**
 * Prepares the enboj db by linking parents & children, and clips and clippees
 *
 * @param ensure Ensure pointer
 * @retun 0 on success
 */
int
enobj_prepare(struct ensure *ensure){
	eina_hash_foreach(ensure->cur->objdb, enobj_prepare_object, ensure);
	return 0;
}


static Eina_Bool
enobj_prepare_object(const Eina_Hash *hash __UNUSED__, const void *key __UNUSED__, void *obj,
		void *ensurev){
	struct enobj *enobj = obj;
	struct enobj *parent, *clip;
//printf("Obj: %p %llx %llx\n",enobj, enobj->parent, enobj->clip);
	if (enobj->parent){
		parent = enobj_parent_get(ensurev, enobj);
		parent->children = eina_list_append(parent->children, enobj);
//		printf("\tChildren: %p\n",parent->children);
	}
	if (enobj->clip){
		clip = enobj_parent_get(ensurev, enobj);
		clip->clippees = eina_list_append(clip->clippees, enobj);
//		printf("\tClips: %p\n",parent->clip);
	}
	return true;
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
	eina_list_free(enobj->children);
	eina_list_free(enobj->clippees);

	if (enobj->win) evas_object_del(enobj->win);

	eina_stringshare_del(enobj->type);

	free(enobj);
}

#if 0
static Eina_Bool
enobj_hash_del_cb(const Eina_Hash *hash __UNUSED__,
		const void *key __UNUSED__, void *enobjv,
		void *ensure __UNUSED__){
	enobj_free(enobjv);
	return true;
}
#endif
