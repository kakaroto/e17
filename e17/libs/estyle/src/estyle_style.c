#include "estyle-config.h"
#include "Estyle_private.h"

#define SET_REL_COLOR(a, b) ((a + b) > 255 ? 255 : ((a + b) < 0 ? 0 : a + b))

static Ewd_Hash *styles = NULL;
static int style_path = 0;

static void __estyle_style_read(Estyle_Style_Info * info);
static void __estyle_style_info_load(Estyle_Style_Info * info);
static Evas_Object *__estyle_style_layer_draw(Estyle_Style_Layer * layer,
		Estyle * es, char *text);
static int __estyle_style_stack_compare(void *style1, void *style2);

/*
 * _estyle_style_instance - get an instance of a style
 * @name: the name of the style to get an instance
 *
 * Returns a pointer to the style instance found and increments the reference
 * count to that style.
 */
Estyle_Style *_estyle_style_instance(char *name)
{
	Estyle_Style *ret;
	Estyle_Style_Info *found;

	CHECK_PARAM_POINTER_RETURN("name", name, NULL);

	/*
	 * Get a reference to the info and create the instance to be
	 * returned to the calling function.
	 */
	found = _estyle_style_info_reference(name);
	ret = (Estyle_Style *) malloc(sizeof(Estyle_Style));
	memset(ret, 0, sizeof(Estyle_Style));
	ret->info = found;

	return ret;
}

/*
 * _estyle_style_release - release an instance of a style
 * @style: a pointer to a style instance that will be released
 *
 * Returns no value. The reference to the style is released and the style is
 * freed if appropriate.
 */
void _estyle_style_release(Estyle_Style *style, Evas *ev)
{
	Evas_Object *ob;
	Evas_List *ptr_list;

	CHECK_PARAM_POINTER("style", style);

	_estyle_style_info_dereference((Estyle_Style_Info *)style->info);

	/*
	 * Destroy the list of evas_objects
	 */
	if (style->bits) {

		/*
		 * Destroy all of the objects for this estyle
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
		evas_list_free(style->bits);
	}

	FREE(style);
}

/*
 * _estyle_style_hide - hide the bits of the style
 * @es: the estyle to hide style bits
 *
 * Returns no value. Hides the style bits associated with the estyle @es.
 */
void _estyle_style_hide(Estyle *es)
{
	Evas_Object *ob;
	Evas_List *ptr_list;

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
 * _estyle_style_show - show the bits of the style
 * @es: the estyle to show style bits
 *
 * Returns no value. Shows the style bits associated with the estyle @es.
 */
void _estyle_style_show(Estyle *es)
{
	Evas_Object *ob;
	Evas_List *ptr_list;

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
 * _estyle_style_add_path - add a path to search for style names
 * @path: the path to add to the search path
 *
 * Returns no value. Add @path to the path group that will be searched to find
 * styles.
 */
void _estyle_style_add_path(char *path)
{
	if (!style_path)
		style_path = ewd_path_group_new("/estyle/styles");

	ewd_path_group_add(style_path, path);
}

/**
 * _estyle_style_remove_path - remove a path from being searched for styles
 * @path: the path that will no longer be searched for styles
 *
 * Returns no value. Removes @path from the list of directories to search
 * through when loading a style.
 */
void _estyle_style_remove_path(char *path)
{
	if (!style_path)
		return;

	ewd_path_group_remove(style_path, path);
}

/*
 * _estyle_style_draw - display the style parts for the text
 * @et: the estyle that will display the style bits
 *
 * Returns no value. Adds the evas objects for the style bits of the main text
 * layer.
 */
void _estyle_style_draw(Estyle *es, char *text)
{
	int i = 0;
	Estyle_Style_Info *info;
	Estyle_Style_Layer *layer;
	Evas_Object *ob;

	if (!es->style)
		return;

	info = ((Estyle_Style *) es->style)->info;

	if (!info->layers)
		return;

	/*
	 * Draw each of the lower layers and add their bits to the style
	 * instance for later manipulation.
	 */
	while ((layer = _estyle_heap_item(info->layers, i))) {
		ob = __estyle_style_layer_draw(layer, es, text);
		((Estyle_Style *) es->style)->bits = evas_list_append( ((Estyle_Style *)es->style)->bits, ob );
		evas_object_smart_member_add(ob, es->smart_obj);
		i++;
	}

	ob = evas_object_clip_get(es->bit);
	if (ob)
		_estyle_style_set_clip(es, ob);
}

/*
 * _estyle_style_set_layer_lower - set the layer of layers below the main text
 * @es: the estyle containing the style to change layer
 * @l: the new layer for the style to be displayed
 *
 * Returns no value. Changes the layer of the style bits in @es to @layer.
 */
int _estyle_style_set_layer_lower(Estyle * es, int l)
{
	int i = 0;
	Estyle_Style_Info *info;
	Estyle_Style_Layer *layer;
	Evas_Object *ob;
	Evas_List *ptr_list;

	if (!es->style)
		return 0;

	info = ((Estyle_Style *) es->style)->info;

	if (!info->layers)
		return 0;

	/*
	 * Move all of the lower layers bits into the correct layer
	 */
	while ((layer = _estyle_heap_item(info->layers, i)) 
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
 * _estyle_style_set_layer_upper - set the layer of style parts above the main text
 * @es: the estyle containing style bits to change layer
 * @l: the layer to move the style bits
 * @start: the index of the style at which to start changing layers
 *
 * Returns no value. Adds the evas objects for the style bits that are above
 * the main text layer.
 */
int _estyle_style_set_layer_upper(Estyle *es, int l, int start)
{
	char *text;
	int i = start;
	Estyle_Style_Info *info;
	Estyle_Style_Layer *layer;
	Evas_Object *ob;
	Evas_List *ptr_list;

	if (!es->bit)
		return 0;

	if (!es->style)
		return 0;

	text = (char *)evas_object_text_text_get(es->bit);

	info = ((Estyle_Style *) es->style)->info;

	if (!info->layers)
		return 0;

	/*
	 * Move all of the upper bits into the correct layer
	 */
	while ((layer = _estyle_heap_item(info->layers, i)) 
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
 * _estyle_style_move - move the style bits into their correct positions
 * @es: the estyle to move style bits
 *
 * Returns no value. Moves all of the evas objects representing style bits
 * into their correct relative positions to @es.
 */
void _estyle_style_move(Estyle *es)
{
	int i;
	Evas_Object *ob;
	Evas_List *ptr_list;
	Estyle_Style_Info *info;
	Estyle_Style_Layer *layer;

	if (!es->style || !es->style->bits)
		return;

	/*
	 * Prepare to traverse the list of bits and layers to get the correct
	 * layout.
	 */
	info = (Estyle_Style_Info *)es->style->info;
	ptr_list = es->style->bits;
	i = 0;

	/*
	 * Each layer corresponds to a bit in the evas. So we need to
	 * increment through each list at the same time. Both lists should
	 * have the same number of items in them, but check for that just in
	 * case there isn't.
	 */
	while (ptr_list && 
		(layer = _estyle_heap_item(info->layers, i++)) != NULL &&
		(ob = ptr_list->data) != NULL) {
		
		evas_object_move(ob, (Evas_Coord)(es->x + layer->x_offset +
					info->left_push),
					(Evas_Coord)(es->y + layer->y_offset +
					info->right_push));

		ptr_list = ptr_list->next;
	}
}

/*
 * _estyle_style_set_color - change the color used by the relative style objects
 * @es: the estyle to change the style relative color
 *
 * Returns no value. Changes the color for the relative layers of the style for
 * @es.
 */
void _estyle_style_set_color(Estyle *es)
{
	int i;
	Evas_Object *sob;
	Evas_List *ptr_list;
	Estyle_Style_Info *info;
	Estyle_Style_Layer *layer;

	if (!es->style)
		return;

	/*
	 * Prepare to traverse the list of bits and layers to get the correct
	 * layout.
	 */
	info = (Estyle_Style_Info *) es->style->info;
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
		(layer = _estyle_heap_item(info->layers, i++)) != NULL &&
		(sob = ptr_list->data) != NULL) {

		if (layer->relative_color) {
			int r, g, b, a;

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
 * _estyle_style_set_font - set the font for the style portion of the estyle
 * @es: the estyle to use for updating the font for the estyle
 *
 * Returns no value. Updates the font for the estyle @es.
 */
void _estyle_style_set_font(Estyle *es, char *font, int size)
{
	int i;
	Evas_Object *sob;
	Evas_List *ptr_list;
	Estyle_Style_Info *info;
	Estyle_Style_Layer *layer;

	if (!es->style || !es->style->bits)
		return;

	/*
	 * Prepare to traverse the list of bits and layers to get the correct
	 * layout.
	 */
	info = (Estyle_Style_Info *) es->style->info;

	ptr_list = es->style->bits;

	i = 0;

	/*
	 * Each layer corresponds to a bit in the evas. So we need to
	 * increment through each list at the same time. Both lists should
	 * have the same number of items in them, but check for that just in
	 * case there isn't.
	 */
	while (ptr_list &&
		(layer = _estyle_heap_item(info->layers, i++)) != NULL &&
		(sob = ptr_list->data) != NULL) {

		evas_object_text_font_set(sob, font, size);
		ptr_list = ptr_list->next;
	}
}

/*
 * _estyle_style_set_text - set the text for the style portion of the estyle
 * @es: the estyle to use for updating the text for the estyle
 *
 * Returns no value. Updates the text for the estyle @es.
 */
void _estyle_style_set_text(Estyle *es)
{
	int i;
	char *text;
	Evas_Object *sob;
	Evas_List *ptr_list;
	Estyle_Style_Info *info;
	Estyle_Style_Layer *layer;

	if (!es->style || !es->style->bits)
		return;

	/*
	 * Prepare to traverse the list of bits and layers to get the correct
	 * layout.
	 */
	text = (char *)evas_object_text_text_get(es->bit);
	if (!text)
		return;

	info = (Estyle_Style_Info *) es->style->info;

	ptr_list = es->style->bits;

	i = 0;

	/*
	 * Each layer corresponds to a bit in the evas. So we need to
	 * increment through each list at the same time. Both lists should
	 * have the same number of items in them, but check for that just in
	 * case there isn't.
	 */
	while (ptr_list &&
		(layer = _estyle_heap_item(info->layers, i++)) != NULL &&
		(sob = ptr_list->data) != NULL) {
			
		evas_object_text_text_set(sob, text);
		ptr_list = ptr_list->next;
	}
}

/*
 * _estyle_style_set_clip - change the clip rectangle used by the style objects
 * @es: the estyle to change the style clip rectangle
 * @ob: the evas object to be used as a clip rectangle
 *
 * Returns no value. Changes the clip rectangle for each evas object used to
 * represent the style for @es.
 */
void _estyle_style_set_clip(Estyle *es, Evas_Object *ob)
{
	int i;
	Evas_Object *sob;
	Evas_List *ptr_list;
	Estyle_Style_Info *info;
	Estyle_Style_Layer *layer;

	if (!es->style || !es->style->bits)
		return;

	/*
	 * Prepare to traverse the list of bits and layers to get the correct
	 * layout.
	 */
	info = (Estyle_Style_Info *)es->style->info;
	
	ptr_list = es->style->bits;

	i = 0;

	/*
	 * Each layer corresponds to a bit in the evas. So we need to
	 * increment through each list at the same time. Both lists should
	 * have the same number of items in them, but check for that just in
	 * case there isn't.
	 */
	while (ptr_list &&
		(layer = _estyle_heap_item(info->layers, i++)) != NULL &&
		(sob = ptr_list->data) != NULL) {
		if (!ob)
			evas_object_clip_unset(sob);
		else
			evas_object_clip_set(sob, ob);

		ptr_list = ptr_list->next;
	}
}

/*
 * _estyle_style_info_reference - get a reference to a style info
 * @name: the name of the style to get a style info reference
 *
 * Returns a poiner to the style info on success, NULL on failure.
 */
Estyle_Style_Info *_estyle_style_info_reference(char *name)
{
	Estyle_Style_Info *found;

	if (!styles)
		styles = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	/*
	 * Look for a previous instance of the info in the hash table.
	 */
	found = ewd_hash_get(styles, name);
	if (!found) {

		/*
		 * No previous instance found, so create one and read in it's
		 * fields from the appropriate db.
		 */
		found =
		    (Estyle_Style_Info *)
		    malloc(sizeof(Estyle_Style_Info));
		memset(found, 0, sizeof(Estyle_Style_Info));

		/*
		 * Load the information for this style info
		 */
		found->name = ewd_string_instance(name);
		__estyle_style_info_load(found);
		ewd_hash_set(styles, name, found);
	}

	found->references++;

	return found;
}

/*
 * _estyle_style_info_dereference - remove a reference to a style_info structure
 * @info: the style info struct to dereference
 */
void _estyle_style_info_dereference(Estyle_Style_Info *info)
{
	info->references--;

	/*
	 * If there are no more references to the style, then we can remove
	 * the info from the hash table and free up some memory.
	 */
	if (info->references < 1) {

		ewd_hash_remove(styles, info->name);
		if (info->layers)
			_estyle_heap_destroy(info->layers);
		ewd_string_release(info->name);

		FREE(info);
	}
}

/*
 * __estyle_style_layer_draw - draw a specified layer for a estyle
 * @layer: the layer to be drawn
 * @es: used to get info about the evas for drawing
 */
static Evas_Object *__estyle_style_layer_draw(Estyle_Style_Layer *layer,
		Estyle *es, char *text)
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

	evas_object_color_set(ret, r, g, b, a);

	return ret;
}

/*
 * __estyle_style_info_load - load the info for the specified info from it's db
 * @info: the info to be loaded
 */
static void __estyle_style_info_load(Estyle_Style_Info * info)
{
	char *real_path;
	char file_name[PATH_MAX];

	snprintf(file_name, PATH_MAX, "%s.style.db", info->name);

	/*
	 * Determine whether the name is absolute or relative to the search
	 * paths.
	 */
	if (style_path && *info->name != '/')
		real_path = ewd_path_group_find(style_path, file_name);
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

	__estyle_style_read(info);
	e_db_close(info->style_db);

      load_exit:
	FREE(real_path);
	return;
}

/*
 * __estyle_style_read - read in the data from the db to the style
 * @info: the info structure that will hold the data read in
 *
 * Returns no value. Read in the data from the db into the info structure
 * creating layers as needed.
 */
static void __estyle_style_read(Estyle_Style_Info * info)
{
	int i;
	int layers;
	char key[1024];
	Estyle_Style_Layer *layer;
	float x_offset, y_offset;

	if (!info->style_db)
		return;
	/*
	   if (info->layers)
	   	_estyle_heap_destroy(info->layers);
	 */

	/*
	 * Determine the number of layers to read in
	 */
	strcpy(key, "/layers/count");
	e_db_int_get(info->style_db, key, &layers);
	if (layers < 1) {
		fprintf(stderr, "Estyle style %s is empty, no formatting "
			"will be displayed for text using this style\n",
			info->name);
		return;
	}

	info->layers = _estyle_heap_new(__estyle_style_stack_compare, layers);

	/*
	 * Read in each layer
	 */
	for (i = 0; i < layers; i++) {
		layer =
		    (Estyle_Style_Layer *)
		    malloc(sizeof(Estyle_Style_Layer));

		sprintf(key, "/layers/%d/stack", i);
		e_db_int_get(info->style_db, key, &layer->stack);

		sprintf(key, "/layers/%d/size_change", i);
		e_db_int_get(info->style_db, key, &layer->size_change);

		sprintf(key, "/layers/%d/x_offset", i);
		e_db_float_get(info->style_db, key, &x_offset);
		layer->x_offset = x_offset;

		sprintf(key, "/layers/%d/y_offset", i);
		e_db_float_get(info->style_db, key, &y_offset);
		layer->y_offset = y_offset;

		sprintf(key, "/layers/%d/relative_color", i);
		e_db_int_get(info->style_db, key, &layer->relative_color);

		sprintf(key, "/layers/%d/color/a", i);
		e_db_int_get(info->style_db, key, &layer->a);

		sprintf(key, "/layers/%d/color/r", i);
		e_db_int_get(info->style_db, key, &layer->r);

		sprintf(key, "/layers/%d/color/g", i);
		e_db_int_get(info->style_db, key, &layer->g);

		sprintf(key, "/layers/%d/color/b", i);
		e_db_int_get(info->style_db, key, &layer->b);

		/*
		 * Adjust the push of the left and right edges of the estyle.
		 */
		if (layer->x_offset > info->right_push)
			info->right_push = layer->x_offset;
		else if (-layer->x_offset > info->left_push)
			info->left_push = -layer->x_offset;

		/*
		 * Adjust the push of the top and bottom edges of the estyle.
		 */
		if (layer->y_offset > info->bottom_push)
			info->bottom_push = layer->y_offset;
		else if (-layer->y_offset > info->top_push)
			info->top_push = -layer->y_offset;

		_estyle_heap_insert(info->layers, layer);
	}
}

/*
 * Compare two styles and return an integer in the style of strcmp that
 * indicates which style has a higher stacking order.
 */
static int __estyle_style_stack_compare(void *style1, void *style2)
{
	if (((Estyle_Style_Layer *) style1)->stack <
	    ((Estyle_Style_Layer *) style2)->stack)
		return -1;

	if (((Estyle_Style_Layer *) style1)->stack >
	    ((Estyle_Style_Layer *) style2)->stack)
		return 1;

	return 0;
}
