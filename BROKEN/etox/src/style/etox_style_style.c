#include "../../config.h"
#include "etox_style_private.h"

#define SET_REL_COLOR(a, b) ((a + b) > 255 ? 255 : ((a + b) < 0 ? 0 : a + b))

static Ecore_Hash *styles = NULL;
static Eina_List *gc = NULL;
static int style_path = 0;

static void __etox_style_style_read(Etox_Style_Style_Info * info);
static void __etox_style_style_info_load(Etox_Style_Style_Info * info);
static Evas_Object *__etox_style_style_layer_draw(Etox_Style_Style_Layer * layer,
		Etox_Style * es, char *text);
static int __etox_style_style_stack_compare(void *style1, void *style2);

/*
 * _etox_style_style_instance - get an instance of a style
 * @name: the name of the style to get an instance
 *
 * Returns a pointer to the style instance found and increments the reference
 * count to that style.
 */
Etox_Style_Style *_etox_style_style_instance(char *name)
{
	Etox_Style_Style *ret;
	Etox_Style_Style_Info *found;

	CHECK_PARAM_POINTER_RETURN("name", name, NULL);

	/*
	 * Get a reference to the info and create the instance to be
	 * returned to the calling function.
	 */
	found = _etox_style_style_info_reference(name);
	ret = (Etox_Style_Style *) malloc(sizeof(Etox_Style_Style));
	memset(ret, 0, sizeof(Etox_Style_Style));
	ret->info = found;

	return ret;
}

/*
 * _etox_style_style_release - release an instance of a style
 * @style: a pointer to a style instance that will be released
 *
 * Returns no value. The reference to the style is released and the style is
 * freed if appropriate.
 */
void _etox_style_style_release(Etox_Style_Style *style, Evas *ev)
{
	Evas_Object *ob;
	Eina_List *ptr_list;

	CHECK_PARAM_POINTER("style", style);

	_etox_style_style_info_dereference((Etox_Style_Style_Info *)style->info);

	/*
	 * Destroy the list of evas_objects
	 */
	if (style->bits) {

		/*
		 * Destroy all of the objects for this etox_style
		 */
		for (ptr_list = style->bits; ptr_list; 
			ptr_list = ptr_list->next) {

			ob = ptr_list->data;
			evas_object_hide(ob);
			evas_object_del(ob);
		}

		/*
		 * Destroy the list that held the objects
		 */
		eina_list_free(style->bits);
	}

	FREE(style);
}

/*
 * _etox_style_style_hide - hide the bits of the style
 * @es: the etox_style to hide style bits
 *
 * Returns no value. Hides the style bits associated with the etox_style @es.
 */
void _etox_style_style_hide(Etox_Style *es)
{
	Evas_Object *ob;
	Eina_List *ptr_list;

	/*
	 * Check if we need to hide any style bits.
	 */
	if (!es->style || !es->style->bits)
		return;

	/*
	 * Hide each bit of the style representation
	 */
	for (ptr_list = es->style->bits; ptr_list; 
		ptr_list = ptr_list->next ) {

		ob = ptr_list->data;
		evas_object_hide(ob);
	}
}

/*
 * _etox_style_style_show - show the bits of the style
 * @es: the etox_style to show style bits
 *
 * Returns no value. Shows the style bits associated with the etox_style @es.
 */
void _etox_style_style_show(Etox_Style *es)
{
	Evas_Object *ob;
	Eina_List *ptr_list;

	/*
	 * Check if we need to show any style bits.
	 */
	if (!es->style || !es->style->bits)
		return;

	/*
	 * Hide each bit of the style representation
	 */
	for (ptr_list = es->style->bits; ptr_list; 
		ptr_list = ptr_list->next ) {

		ob = ptr_list->data;
		evas_object_show(ob);
	}
}

/**
 * _etox_style_style_add_path - add a path to search for style names
 * @path: the path to add to the search path
 *
 * Returns no value. Add @path to the path group that will be searched to find
 * styles.
 */
void _etox_style_style_add_path(char *path)
{
	if (!style_path)
		style_path = ecore_path_group_new(PACKAGE_DATA_DIR "/etox/styles");

	ecore_path_group_add(style_path, path);
}

/**
 * _etox_style_style_remove_path - remove a path from being searched for styles
 * @path: the path that will no longer be searched for styles
 *
 * Returns no value. Removes @path from the list of directories to search
 * through when loading a style.
 */
void _etox_style_style_remove_path(char *path)
{
	if (!style_path)
		return;

	ecore_path_group_remove(style_path, path);
}

/*
 * _etox_style_style_draw - display the style parts for the text
 * @et: the etox_style that will display the style bits
 *
 * Returns no value. Adds the evas objects for the style bits of the main text
 * layer.
 */
void _etox_style_style_draw(Etox_Style *es, char *text)
{
	int i = 0;
	Etox_Style_Style_Info *info;
	Etox_Style_Style_Layer *layer;
	Evas_Object *ob;

	if (!es->style)
		return;

	info = ((Etox_Style_Style *) es->style)->info;

	if (!info->layers)
		return;

	/*
	 * Draw each of the lower layers and add their bits to the style
	 * instance for later manipulation.
	 */
	while ((layer = _etox_style_heap_item(info->layers, i))) {
		ob = __etox_style_style_layer_draw(layer, es, text);
		((Etox_Style_Style *) es->style)->bits = eina_list_append( ((Etox_Style_Style *)es->style)->bits, ob );
		evas_object_smart_member_add(ob, es->smart_obj);
		i++;
	}

	ob = evas_object_clip_get(es->bit);
	if (ob)
		_etox_style_style_set_clip(es, ob);
}

/*
 * _etox_style_style_set_layer_lower - set the layer of layers below the main text
 * @es: the etox_style containing the style to change layer
 * @l: the new layer for the style to be displayed
 *
 * Returns no value. Changes the layer of the style bits in @es to @layer.
 */
int _etox_style_style_set_layer_lower(Etox_Style * es, int l)
{
	int i = 0;
	Etox_Style_Style_Info *info;
	Etox_Style_Style_Layer *layer;
	Evas_Object *ob;
	Eina_List *ptr_list;

	if (!es->style)
		return 0;

	info = ((Etox_Style_Style *) es->style)->info;

	if (!info->layers)
		return 0;

	/*
	 * Move all of the lower layers bits into the correct layer
	 */
	while ((layer = _etox_style_heap_item(info->layers, i)) 
		&& layer->stack < 0) {

		for (ptr_list = es->style->bits; ptr_list; 
			ptr_list = ptr_list->next ) {

			ob = ptr_list->data;
			evas_object_layer_set(ob, l);
		}
		i++;
	}

	return i;
}

/*
 * _etox_style_style_set_layer_upper - set the layer of style parts above the main text
 * @es: the etox_style containing style bits to change layer
 * @l: the layer to move the style bits
 * @start: the index of the style at which to start changing layers
 *
 * Returns no value. Adds the evas objects for the style bits that are above
 * the main text layer.
 */
int _etox_style_style_set_layer_upper(Etox_Style *es, int l, int start)
{
	char *text;
	int i = start;
	Etox_Style_Style_Info *info;
	Etox_Style_Style_Layer *layer;
	Evas_Object *ob;
	Eina_List *ptr_list;

	if (!es->bit)
		return 0;

	if (!es->style)
		return 0;

	text = (char *)evas_object_text_text_get(es->bit);

	info = ((Etox_Style_Style *) es->style)->info;

	if (!info->layers)
		return 0;

	/*
	 * Move all of the upper bits into the correct layer
	 */
	while ((layer = _etox_style_heap_item(info->layers, i)) 
		&& layer->stack) {

		for (ptr_list = es->style->bits; ptr_list; 
			ptr_list = ptr_list->next ) {

			ob = ptr_list->data;
			evas_object_layer_set(ob, l);
		}

		i++;
	}

	return i;
}

/*
 * _etox_style_style_move - move the style bits into their correct positions
 * @es: the etox_style to move style bits
 *
 * Returns no value. Moves all of the evas objects representing style bits
 * into their correct relative positions to @es.
 */
void _etox_style_style_move(Etox_Style *es)
{
	int i;
	Evas_Object *ob;
	Eina_List *ptr_list;
	Etox_Style_Style_Info *info;
	Etox_Style_Style_Layer *layer;

	if (!es->style || !es->style->bits)
		return;

	/*
	 * Prepare to traverse the list of bits and layers to get the correct
	 * layout.
	 */
	info = (Etox_Style_Style_Info *)es->style->info;
	ptr_list = es->style->bits;
	i = 0;

	/*
	 * Each layer corresponds to a bit in the evas. So we need to
	 * increment through each list at the same time. Both lists should
	 * have the same number of items in them, but check for that just in
	 * case there isn't.
	 */
	while (ptr_list && 
		(layer = _etox_style_heap_item(info->layers, i++)) != NULL &&
		(ob = ptr_list->data) != NULL) {
		
		evas_object_move(ob, (Evas_Coord)(es->x + layer->x_offset +
					info->left_push),
					(Evas_Coord)(es->y + layer->y_offset +
					info->right_push));

		ptr_list = ptr_list->next;
	}
}

/*
 * _etox_style_style_set_color - change the color used by the relative style objects
 * @es: the etox_style to change the style relative color
 *
 * Returns no value. Changes the color for the relative layers of the style for
 * @es.
 */
void _etox_style_style_set_color(Etox_Style *es)
{
	int i;
	Evas_Object *sob;
	Eina_List *ptr_list;
	Etox_Style_Style_Info *info;
	Etox_Style_Style_Layer *layer;

	if (!es->style)
		return;

	/*
	 * Prepare to traverse the list of bits and layers to get the correct
	 * layout.
	 */
	info = (Etox_Style_Style_Info *) es->style->info;
	if (!info)
		return;

	if (!es->style->bits)
		return;

	ptr_list = es->style->bits;

	i = 0;

	/*
	 * Each layer corresponds to a bit in the evas. So we need to
	 * increment through each list at the same time. Both lists should
	 * have the same number of items in them, but check for that just in
	 * case there isn't.
	 */
	while (ptr_list &&
		(layer = _etox_style_heap_item(info->layers, i++)) != NULL &&
		(sob = ptr_list->data) != NULL) {
			int r, g, b, a;

		if (layer->relative_color) {

			r = SET_REL_COLOR(es->color->r, layer->r);
			g = SET_REL_COLOR(es->color->g, layer->g);
			b = SET_REL_COLOR(es->color->b, layer->b);
			a = SET_REL_COLOR(es->color->a, layer->a);

			evas_object_color_set(sob, r, g, b, a);
		}

		ptr_list = ptr_list->next;
	}
}

/*
 * _etox_style_style_set_font - set the font for the style portion of the etox_style
 * @es: the etox_style to use for updating the font for the etox_style
 *
 * Returns no value. Updates the font for the etox_style @es.
 */
void _etox_style_style_set_font(Etox_Style *es, char *font, int size)
{
	int i;
	Evas_Object *sob;
	Eina_List *ptr_list;
	Etox_Style_Style_Info *info;
	Etox_Style_Style_Layer *layer;

	if (!es->style || !es->style->bits)
		return;

	/*
	 * Prepare to traverse the list of bits and layers to get the correct
	 * layout.
	 */
	info = (Etox_Style_Style_Info *) es->style->info;

	ptr_list = es->style->bits;

	i = 0;

	/*
	 * Each layer corresponds to a bit in the evas. So we need to
	 * increment through each list at the same time. Both lists should
	 * have the same number of items in them, but check for that just in
	 * case there isn't.
	 */
	while (ptr_list &&
		(layer = _etox_style_heap_item(info->layers, i++)) != NULL &&
		(sob = ptr_list->data) != NULL) {

		evas_object_text_font_set(sob, font, size);
		ptr_list = ptr_list->next;
	}
}

/*
 * _etox_style_style_set_text - set the text for the style portion of the etox_style
 * @es: the etox_style to use for updating the text for the etox_style
 *
 * Returns no value. Updates the text for the etox_style @es.
 */
void _etox_style_style_set_text(Etox_Style *es)
{
	int i;
	char *text;
	Evas_Object *sob;
	Eina_List *ptr_list;
	Etox_Style_Style_Info *info;
	Etox_Style_Style_Layer *layer;

	if (!es->style || !es->style->bits)
		return;

	/*
	 * Prepare to traverse the list of bits and layers to get the correct
	 * layout.
	 */
	text = (char *)evas_object_text_text_get(es->bit);
	if (!text)
		return;

	info = (Etox_Style_Style_Info *) es->style->info;

	ptr_list = es->style->bits;

	i = 0;

	/*
	 * Each layer corresponds to a bit in the evas. So we need to
	 * increment through each list at the same time. Both lists should
	 * have the same number of items in them, but check for that just in
	 * case there isn't.
	 */
	while (ptr_list &&
		(layer = _etox_style_heap_item(info->layers, i++)) != NULL &&
		(sob = ptr_list->data) != NULL) {
			
		evas_object_text_text_set(sob, text);
		ptr_list = ptr_list->next;
	}
}

/*
 * _etox_style_style_set_clip - change the clip rectangle used by the style objects
 * @es: the etox_style to change the style clip rectangle
 * @ob: the evas object to be used as a clip rectangle
 *
 * Returns no value. Changes the clip rectangle for each evas object used to
 * represent the style for @es.
 */
void _etox_style_style_set_clip(Etox_Style *es, Evas_Object *ob)
{
	int i;
	Evas_Object *sob;
	Eina_List *ptr_list;
	Etox_Style_Style_Info *info;
	Etox_Style_Style_Layer *layer;

	if (!es->style || !es->style->bits)
		return;

	/*
	 * Prepare to traverse the list of bits and layers to get the correct
	 * layout.
	 */
	info = (Etox_Style_Style_Info *)es->style->info;
	
	ptr_list = es->style->bits;

	i = 0;

	/*
	 * Each layer corresponds to a bit in the evas. So we need to
	 * increment through each list at the same time. Both lists should
	 * have the same number of items in them, but check for that just in
	 * case there isn't.
	 */
	while (ptr_list &&
		(layer = _etox_style_heap_item(info->layers, i++)) != NULL &&
		(sob = ptr_list->data) != NULL) {
		if (!ob)
			evas_object_clip_unset(sob);
		else
			evas_object_clip_set(sob, ob);

		ptr_list = ptr_list->next;
	}
}

/*
 * _etox_style_style_info_reference - get a reference to a style info
 * @name: the name of the style to get a style info reference
 *
 * Returns a poiner to the style info on success, NULL on failure.
 */
Etox_Style_Style_Info *_etox_style_style_info_reference(char *name)
{
	Etox_Style_Style_Info *found;

	if (!styles)
		styles = ecore_hash_new(ecore_str_hash, ecore_str_compare);

	/*
	 * Look for a previous instance of the info in the hash table.
	 */
	found = ecore_hash_get(styles, name);
	if (!found) {

		/*
		 * No previous instance found, so create one and read in it's
		 * fields from the appropriate db.
		 */
		found =
		    (Etox_Style_Style_Info *)
		    malloc(sizeof(Etox_Style_Style_Info));
		memset(found, 0, sizeof(Etox_Style_Style_Info));

		/*
		 * Load the information for this style info
		 */
		found->name = ecore_string_instance(name);
		__etox_style_style_info_load(found);
		ecore_hash_set(styles, strdup(name), found);
	}
	else {
		if (eina_list_data_find(gc, found))
			gc = eina_list_remove(gc, found);
	}

	found->references++;

	return found;
}

/*
 * _etox_style_style_info_dereference - remove a reference to a style_info structure
 * @info: the style info struct to dereference
 */
void _etox_style_style_info_dereference(Etox_Style_Style_Info *info)
{
	info->references--;

	if (info->references < 1 && !eina_list_data_find(gc, info))
		gc = eina_list_append(gc, info);
}

void _etox_style_style_info_collect()
{
	/*
	 * If there are no more references to the style, then we can remove
	 * the info from the hash table and free up some memory.
	 */
	while (gc) {
		Etox_Style_Style_Info *info;

		info = gc->data;
		gc = eina_list_remove(gc, info);
		ecore_hash_remove(styles, info->name);
		if (info->layers)
			_etox_style_heap_destroy(info->layers);
		ecore_string_release(info->name);

		FREE(info);
	}
}

/*
 * __etox_style_style_layer_draw - draw a specified layer for a etox_style
 * @layer: the layer to be drawn
 * @es: used to get info about the evas for drawing
 */
static Evas_Object *__etox_style_style_layer_draw(Etox_Style_Style_Layer *layer,
		Etox_Style *es, char *text)
{
	int r, g, b, a;
	char *font;
	Evas_Font_Size size;
	Evas_Object *ret;

	/*
	 * Create the text at the correct size and move it into position
	 */
	evas_object_text_font_get(es->bit, &font, &size);
	ret = evas_object_text_add(es->evas);
	evas_object_repeat_events_set(ret, 1);
	evas_object_text_font_set(ret, font, size + layer->size_change);
	evas_object_text_text_set(ret, text); 

	evas_object_move(ret, (Evas_Coord)(es->x + layer->x_offset +
				 es->style->info->left_push),
			(Evas_Coord)(es->y + layer->y_offset +
				 es->style->info->top_push));

	/*
	 * Now determine if it has relative or absolute color and change it's
	 * color appropriately.
	 */
	if (layer->relative_color) {

		/*
		 * Bounds check the color components
		 */
		r = SET_REL_COLOR(es->color->r, layer->r);
		g = SET_REL_COLOR(es->color->g, layer->g);
		b = SET_REL_COLOR(es->color->b, layer->b);
		a = SET_REL_COLOR(es->color->a, layer->a);
	} else {
		r = layer->r;
		g = layer->g;
		b = layer->b;
		a = layer->a;
	}

	if (layer->blend_alpha)
		a = (layer->blend_alpha * (a + 1)) / 256;

	evas_object_color_set(ret, r, g, b, a);

	return ret;
}

/*
 * __etox_style_style_info_load - load the info for the specified info from it's db
 * @info: the info to be loaded
 */
static void __etox_style_style_info_load(Etox_Style_Style_Info * info)
{
	char *real_path;
	char file_name[PATH_MAX];

	snprintf(file_name, PATH_MAX, "%s.style.db", info->name);

	/*
	 * Determine whether the name is absolute or relative to the search
	 * paths.
	 */
	if (style_path && *info->name != '/')
		real_path = ecore_path_group_find(style_path, file_name);
	else
		real_path = strdup(file_name);

	if (!real_path)
		return;

	/*
	 * Now load the db for reading in the style information
	 */
	info->style_db = e_db_open_read(real_path);
	if (!info->style_db)
		goto load_exit;

	__etox_style_style_read(info);
	e_db_close(info->style_db);

      load_exit:
	FREE(real_path);
	return;
}

/*
 * __etox_style_style_read - read in the data from the db to the style
 * @info: the info structure that will hold the data read in
 *
 * Returns no value. Read in the data from the db into the info structure
 * creating layers as needed.
 */
static void __etox_style_style_read(Etox_Style_Style_Info * info)
{
	int i;
	int layers;
	char key[1024];
	Etox_Style_Style_Layer *layer;
	float x_offset, y_offset;

	if (!info->style_db)
		return;
	/*
	   if (info->layers)
	   	_etox_style_heap_destroy(info->layers);
	 */

	/*
	 * Determine the number of layers to read in
	 */
	strcpy(key, "/layers/count");
	e_db_int_get(info->style_db, key, &layers);
	if (layers < 1) {
		fprintf(stderr, "Etox_Style style %s is empty, no formatting "
			"will be displayed for text using this style\n",
			info->name);
		return;
	}

	info->layers = _etox_style_heap_new(__etox_style_style_stack_compare, layers);

	/*
	 * Read in each layer
	 */
	for (i = 0; i < layers; i++) {
		layer = (Etox_Style_Style_Layer *)calloc(sizeof(Etox_Style_Style_Layer),
						     1);

		snprintf(key, sizeof(key), "/layers/%d/stack", i);
		e_db_int_get(info->style_db, key, &layer->stack);

		snprintf(key, sizeof(key), "/layers/%d/size_change", i);
		e_db_int_get(info->style_db, key, &layer->size_change);

		snprintf(key, sizeof(key), "/layers/%d/x_offset", i);
		e_db_float_get(info->style_db, key, &x_offset);
		layer->x_offset = x_offset;

		snprintf(key, sizeof(key), "/layers/%d/y_offset", i);
		e_db_float_get(info->style_db, key, &y_offset);
		layer->y_offset = y_offset;

		snprintf(key, sizeof(key), "/layers/%d/relative_color", i);
		e_db_int_get(info->style_db, key, &layer->relative_color);

		snprintf(key, sizeof(key), "/layers/%d/blend_alpha", i);
		e_db_int_get(info->style_db, key, &layer->blend_alpha);

		snprintf(key, sizeof(key), "/layers/%d/color/a", i);
		e_db_int_get(info->style_db, key, &layer->a);

		snprintf(key, sizeof(key), "/layers/%d/color/r", i);
		e_db_int_get(info->style_db, key, &layer->r);

		snprintf(key, sizeof(key), "/layers/%d/color/g", i);
		e_db_int_get(info->style_db, key, &layer->g);

		snprintf(key, sizeof(key), "/layers/%d/color/b", i);
		e_db_int_get(info->style_db, key, &layer->b);

		/*
		 * Adjust the push of the left and right edges of the etox_style.
		 */
		if (layer->x_offset > info->right_push)
			info->right_push = layer->x_offset;
		else if (-layer->x_offset > info->left_push)
			info->left_push = -layer->x_offset;

		/*
		 * Adjust the push of the top and bottom edges of the etox_style.
		 */
		if (layer->y_offset > info->bottom_push)
			info->bottom_push = layer->y_offset;
		else if (-layer->y_offset > info->top_push)
			info->top_push = -layer->y_offset;

		_etox_style_heap_insert(info->layers, layer);
	}
}

/*
 * Compare two styles and return an integer in the style of strcmp that
 * indicates which style has a higher stacking order.
 */
static int __etox_style_style_stack_compare(void *style1, void *style2)
{
	if (((Etox_Style_Style_Layer *) style1)->stack <
	    ((Etox_Style_Style_Layer *) style2)->stack)
		return -1;

	if (((Etox_Style_Style_Layer *) style1)->stack >
	    ((Etox_Style_Style_Layer *) style2)->stack)
		return 1;

	return 0;
}
