/**
 * smart template.  i stole this from rephorm and made it check the
 * data_get calls for some validity.
 *
 * $ cp evas_smart_template_atmos.c mysmartobj.c
 * $ perl -p -i -e 's/engage_element/my_smart_obj/g' mysmartobj.c
 * $ perl -p -i -e 's/Engage_Element/My_Smart_Obj/g' mysmartobj.c
 * There's example structs in the #if0 block you can easily copy to a
 * header(mysmartobj.h)
 *
 */
#include <Evas.h>
#include <Edje.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#if 0
typedef struct _Engage_Element Engage_Element;

struct _Engage_Element
{
    Evas_Object *clip;
    Evas_Coord x, y, w, h;
};
#endif

/* smart object handlers */
static Evas_Smart * _engage_element_object_smart_get();
static Evas_Object * engage_element_object_new(Evas *evas);
void _engage_element_object_add(Evas_Object *o);
void _engage_element_object_del(Evas_Object *o);
void _engage_element_object_layer_set(Evas_Object *o, int l);
void _engage_element_object_raise(Evas_Object *o);
void _engage_element_object_lower(Evas_Object *o);
void _engage_element_object_stack_above(Evas_Object *o, Evas_Object *above);
void _engage_element_object_stack_below(Evas_Object *o, Evas_Object *below);
void _engage_element_object_move(Evas_Object *o, double x, double y);
void _engage_element_object_resize(Evas_Object *o, double w, double h);
void _engage_element_object_show(Evas_Object *o);
void _engage_element_object_hide(Evas_Object *o);
void _engage_element_object_color_set(Evas_Object *o, int r, int g, int b, int a);
void _engage_element_object_clip_set(Evas_Object *o, Evas_Object *clip);
void _engage_element_object_clip_unset(Evas_Object *o);

static Evas_Object *
engage_element_behavior_new(Evas *evas, const char *file, const char *group)
{
    Evas_Object *result = NULL;
    
    if(file && group && file[0] == '/')
    {
	result = edje_object_add(evas);
	if(edje_object_file_set(result, file, group) > 0)
	{
	    evas_object_move(result, -60, -60);
	    evas_object_show(result);
	}
	else
	{
	    evas_object_del(result);
	    result = NULL;
	}
    }
    return(result);
}
static Evas_Object *
engage_element_icon_new(Evas *evas, const char *file, const char *group)
{
    Evas_Object *result = NULL;
    
    /* edje */
    if(group)
    {
	if(file && file[0] == '/')
	{
	    result = edje_object_add(evas);
	    if(edje_object_file_set(result, file, group) > 0)
	    {
		evas_object_move(result, -60, -60);
		evas_object_show(result);
	    }
	}
	else
	{
	    evas_object_del(result);
	    result = NULL;
	}
    }
    /* image */
    else
    {
    }
    return(result);
}

Evas_Object*
engage_element_new(Evas *e, const char *bfile, const char *bgroup,
			    const char *ifile, const char *igroup)
{
  char buf[PATH_MAX];
  int w = 48, h = 48;
  Evas_Object *result = NULL;
  Engage_Element *data = NULL;
 
  if((result = engage_element_object_new(e)))
  {
    if((data = evas_object_smart_data_get(result)))
    {
	data->clip = evas_object_rectangle_add(e);
	evas_object_color_set(data->clip, 255, 255, 255, 255);
	evas_object_layer_set(data->clip, 0);
	evas_object_show(data->clip);

	if((data->behavior = engage_element_behavior_new(e, bfile, bgroup)))
	{
	    evas_object_clip_set(data->behavior, data->clip);
	    if((data->icon = engage_element_icon_new(e, ifile, igroup)))
	    {
		evas_object_clip_set(data->icon, data->clip);
	    }
	    else
	    {
		evas_object_del(data->behavior);
		data->behavior = NULL;
		evas_object_del(data->clip);
		data->clip = NULL;
		evas_object_del(result);
		result = NULL;
	    }
	}
    }
  }
  return(result);
}


/*==========================================================================
 * Smart Object Code, Go Away
 *========================================================================*/


/*** external API ***/

static Evas_Object *
engage_element_object_new(Evas *evas)
{
  Evas_Object *engage_element_object;

  engage_element_object = evas_object_smart_add(evas, _engage_element_object_smart_get());

  return engage_element_object;
}

/*** smart object handler functions ***/

static Evas_Smart *
_engage_element_object_smart_get()
{
  Evas_Smart *smart = NULL;
  smart = evas_smart_new ("engage_element_object",
                          _engage_element_object_add,
                          _engage_element_object_del,
                          _engage_element_object_layer_set,
                          _engage_element_object_raise,
                          _engage_element_object_lower,
                          _engage_element_object_stack_above,
                          _engage_element_object_stack_below,
                          _engage_element_object_move,
                          _engage_element_object_resize,
                          _engage_element_object_show,
                          _engage_element_object_hide,
                          _engage_element_object_color_set,
                          _engage_element_object_clip_set,
                          _engage_element_object_clip_unset,
                          NULL
                          );

  return smart; 
}

void
_engage_element_object_add(Evas_Object *o)
{
  Engage_Element *data = NULL;
    
  data = malloc(sizeof(Engage_Element));
  memset(data, 0, sizeof(Engage_Element));
  evas_object_smart_data_set(o, data);
}


void
_engage_element_object_del(Evas_Object *o)
{
  Engage_Element *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    free(data);
  }
}

void
_engage_element_object_layer_set(Evas_Object *o, int l)
{
  Engage_Element *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_layer_set(data->clip, l);
  }
}

void
_engage_element_object_raise(Evas_Object *o)
{
  Engage_Element *data;
  
  data = evas_object_smart_data_get(o);

  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_raise(data->clip);
  }
}

void
_engage_element_object_lower(Evas_Object *o)
{
  Engage_Element *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_lower(data->clip);
  }
}

void
_engage_element_object_stack_above(Evas_Object *o, Evas_Object *above)
{
  Engage_Element *data;
  
  data = evas_object_smart_data_get(o);

  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_stack_above(data->clip, above);
  }
}

void
_engage_element_object_stack_below(Evas_Object *o, Evas_Object *below)
{
  Engage_Element *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_stack_below(data->clip, below);
  }
}

void
_engage_element_object_move(Evas_Object *o, double x, double y)
{
  Engage_Element *data;
  
  data = evas_object_smart_data_get(o);

  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_move(data->clip, y, y);
  }
}

void
_engage_element_object_resize(Evas_Object *o, double w, double h)
{
  Engage_Element *data;
  
  data = evas_object_smart_data_get(o);

  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_resize(data->clip, w, h);
  }
}

void
_engage_element_object_show(Evas_Object *o)
{
  Engage_Element *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
      evas_object_show(data->clip);
  }
}

void
_engage_element_object_hide(Evas_Object *o)
{
  Engage_Element *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_hide(data->clip);
  }
}

void
_engage_element_object_color_set(Evas_Object *o, int r, int g, int b, int a)
{
  Engage_Element *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_color_set(data->clip, r, g, b, a);
  }
}

void
_engage_element_object_clip_set(Evas_Object *o, Evas_Object *clip)
{
  Engage_Element *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_clip_set(data->clip, clip);
  }
}

void
_engage_element_object_clip_unset(Evas_Object *o)
{
  Engage_Element *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_clip_unset(data->clip);
  }
}
