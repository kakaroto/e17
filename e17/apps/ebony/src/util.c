#include "util.h"
#include "advanced.h"
#include "callbacks.h"

/** Parse the ebony previously modified bg dbs 
 * Return a GList 
 */
GList *
parse_ebony_bg_list_db(void)
{
    char buf[PATH_MAX], dbname[PATH_MAX];
    E_DB_File *db;
    GList *result = NULL;

    snprintf(dbname, PATH_MAX, "%s/.ebony.db", getenv("HOME"));

    db = e_db_open(dbname);
    if(db)
    {
	int count, i;
	char *s;

	if(e_db_int_get(db, "/bg/count", &count))
	{
	    for(i = 0; (i < count) && (i < MAX_RECENT_BG_COUNT); i++)
	    {
		snprintf(buf, PATH_MAX, "/bg/%d/filename", i);
		s = e_db_str_get(db, buf);
		result = g_list_append(result, s);
	    }
	}
    }
    e_db_close(db);
    return(result);
}
/* write_ebony_bg_list_db - Write the GList to the db clearing out old
 * entries
 * @l - a Glist from the bg_list gtk_list
 * This DESTROYS the list it's passed.
 */
void
write_ebony_bg_list_db(GList *l)
{
    char buf[PATH_MAX], dbname[PATH_MAX];
    E_DB_File *db;

    snprintf(dbname, PATH_MAX, "%s/.ebony.db", getenv("HOME"));
    db = e_db_open(dbname);
    if(db)
    {
	int i, key_count = 0;
	char **keys;
    
	keys = e_db_dump_key_list(dbname, &key_count);
	if(keys)
	{
	    for(i = 0; i < key_count; i++)
	    {
		e_db_data_del(db, keys[i]);
		free(keys[i]);
	    }
	    free(keys);
	}
	
	if(l)
	{
	    GList *ll = NULL;
	    for(ll = l,i = 0; ll && i < MAX_RECENT_BG_COUNT; ll = ll->next, i++)
	    {
		snprintf(buf, PATH_MAX, "/bg/%d/filename", i);
		e_db_str_set(db, buf, (char*)ll->data);
		if(ll->data) free((char*)ll->data);
	    }
	    e_db_int_set(db, "/bg/count", i);
	}
	e_db_close(db);
	e_db_flush();
	g_list_free(l);
    }
}
/**
 * get_shortname_for - chop the absolute path off of a bg filename
 * @filename the file to chop
 * Return the shortened filename, caller should free this
 */
char*
get_shortname_for(const char *filename)
{
    char *tmp, *str, *result = NULL;
    int length, i;

    tmp = strdup(filename);
    length = strlen(filename);
    
    /* start from the back, find the first slash */
    for(i = length; tmp[i] != '/'; i--) ;
    /* chop the slash */
    i++;
    
    str = &tmp[i];
    length = strlen(str);
    if(length > 5)
    {
	char *s = &str[length - 6];
	if(!(strcmp(s, ".bg.db")))
	{
	    str[length - 6] = '\0';
	    result = strdup(str);
	    str[length - 6] = '.';
	}
    }
    /* see if it has our extension */
    else
	result = strdup(&tmp[i]);
    
    free(tmp);
    return(result);
}

/**
 * update_background - e_bg_resize on the image, and allows layers to swap
 * @_bg - the bg to update
 */
void
update_background(E_Background _bg)
{
    int w, h;
    
    if(!_bg) return;
    w = _bg->geom.w;
    h = _bg->geom.h;
    _bg->geom.w = 0;
    _bg->geom.h = 0;
    e_bg_resize(_bg, w, h);
    e_bg_set_layer(_bg, 0);
    if((bl) && (bl->obj)) outline_evas_object(bl->obj);
    DRAW();
}
/**
 * move_layer_up - move the currently selected bl "up" one layer
 * @_bl - the background layer to move up
 * Return 1 on success, 0 on failure
 */
int
move_layer_up(E_Background_Layer _bl)
{
    Evas_List l, ll;
    if(!bg) return(0);
    if(!_bl) return(0);
    for(l = bg->layers; l && l->data != _bl; l = l->next) ;
    if(!l) return(0);	/* not in the list */
    ll = l->next;
    if(!ll) return(0);	/* top layer */
    bg->layers = evas_list_remove(bg->layers, _bl);
    bg->layers = evas_list_append_relative(bg->layers, _bl, ll->data);
    display_layer_values(_bl);
    update_background(bg);
    return(1);
}
/**
 * move_layer_down - move the currently selected bl "down" one layer
 * @_bl - the background layer to move down
 * Return 1 on success, 0 on failure
 */
int
move_layer_down(E_Background_Layer _bl)
{
    Evas_List l, ll;
    if(!bg) return(0);
    if(!_bl) return(0);
    for(l = bg->layers; l && l->data != _bl; l = l->next) ;
    if(!l) return(0);	/* not in the list */
    ll = l->prev;
    if(!ll) return(0);	/* bottom layer */
    bg->layers = evas_list_remove(bg->layers, _bl);
    bg->layers = evas_list_prepend_relative(bg->layers, _bl, ll->data);
    display_layer_values(_bl);
    update_background(bg);
    return(1);
}
/**
 * outline_evas_object - outline the evas object in the current layer
 * @_o - the object to outline
 */
void
outline_evas_object(Evas_Object _o)
{
    double x,y,w,h;
    /* int colors[] = { 255, 255, 255, 255 }; */
    /* int colors[] = { 0, 0, 0, 90 }; */
    Evas_Object o;
    
    if(!_o) return;
    evas_get_geometry(evas, _o, &x, &y, &w, &h);
    o = evas_object_get_named(evas, "top_line");
    if(o)
    {
	evas_set_line_xy(evas, o, x , y, x + w, y );
	evas_set_layer(evas, o, 100);
    }
    o = evas_object_get_named(evas, "bottom_line");
    if(o)
    {
	evas_set_line_xy(evas, o, x , y + h, x + w, y + h);
	evas_set_layer(evas, o, 100);
    }
    o = evas_object_get_named(evas, "right_line");
    if(o)
    {
	evas_set_line_xy(evas, o, x + w , y, x + w, y + h);
	evas_set_layer(evas, o, 100);
    }
    o = evas_object_get_named(evas, "left_line");
    if(o)
    {
	evas_set_line_xy(evas, o, x, y, x, y + h);
	evas_set_layer(evas, o, 100);
    }
}

/**
 * fill_background_images - load Imlib2 Image into memory for saving
 * @_bg - the E_Background to be "filled"
 */
void
fill_background_images(E_Background _bg)
{
    Evas_List l;
    E_Background_Layer _bl;
    
    if(!_bg) return;
    for(l = _bg->layers; l; l = l->next)
    {
	_bl = (E_Background_Layer)l->data;
	if(_bl->type == E_BACKGROUND_TYPE_IMAGE)
	{
	    if(!_bl->image) _bl->image = imlib_load_image(_bl->file);
	}
    }
}
/**
 * clear_bg_db_keys - clears out all the keys and data in the E_Background
 * @_bg - the bg requesting all of its data to be deleted
 */
void
clear_bg_db_keys(E_Background _bg)
{
   E_DB_File *db;
   char **keys;
   int i, key_count;

   if(!_bg) return;
   if(!_bg->file) return;
   db = e_db_open(_bg->file);
   if(db)
   {
    keys = e_db_dump_key_list(_bg->file, &key_count);
    if (keys)
    {
      for (i = 0; i < key_count; i++)
      {
         e_db_data_del(db, keys[i]);
         free(keys[i]);
      }
      free(keys);
    }
    e_db_close(db);
    e_db_flush();
   }
}

